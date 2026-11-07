#include "serein/hooks/privacy/bot_login.h"

#include "config.h"
#include "intro/intro_step.h"
#include "intro/intro_widget.h"
#include "lang/lang_keys.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/fields/password_input.h"

#include <QtCore/QRegularExpression>
#include <openssl/crypto.h>

#include "styles/style_intro.h"
#include "styles/style_widgets.h"

namespace Serein::Privacy {
namespace {

class BotLogin final : public Intro::details::Step {
public:
	BotLogin(
		QWidget *parent,
		not_null<Main::Account*> account,
		not_null<Intro::details::Data*> data)
	: Step(parent, account, data)
	, _token(this, st::defaultInputField, tr::lng_serein_bot_token()) {
		setTitleText(tr::lng_serein_bot_login());
		setDescriptionText(tr::lng_serein_bot_login_about());
		setErrorCentered(true);
		connect(_token.data(), &Ui::MaskedInputField::submitted, this, [=] { submit(); });
	}

	bool hasBack() const override {
		return true;
	}

	void setInnerFocus() override {
		_token->setFocusFast();
	}

	void cancelled() override {
		api().request(base::take(_request)).cancel();
		_token->clear();
	}

	void submit() override {
		if (_request) return;
		static const auto format = QRegularExpression(u"^[0-9]{1,20}:[A-Za-z0-9_-]{20,128}$"_q);
		const auto text = _token->getLastText().trimmed();
		if (!format.match(text).hasMatch()) {
			_token->showError();
			return;
		}
		auto token = text.toUtf8();
		_token->clear();
		_token->setDisabled(true);
		hideError();
		_request = api().request(MTPauth_ImportBotAuthorization(
			MTP_int(0),
			MTP_int(ApiId),
			MTP_string(ApiHash),
			MTP_string(token)
		)).done([=](const MTPauth_Authorization &result) {
			_request = 0;
			if (result.type() != mtpc_auth_authorization
				|| result.c_auth_authorization().vuser().type() != mtpc_user
				|| !result.c_auth_authorization().vuser().c_user().is_bot()) {
				failed();
				return;
			}
			finish(result);
		}).fail([=] {
			failed();
		}).send();
		OPENSSL_cleanse(token.data(), token.size());
	}

protected:
	void resizeEvent(QResizeEvent *event) override {
		Step::resizeEvent(event);
		_token->resize(st::introStepWidth, _token->height());
		_token->moveToLeft(contentLeft(), contentTop() + st::introStepFieldTop);
	}

private:
	void failed() {
		_request = 0;
		_token->setDisabled(false);
		_token->setFocusFast();
		showError(tr::lng_serein_bot_login_failed());
	}

	object_ptr<Ui::PasswordInput> _token;
	mtpRequestId _request = 0;

};

} // namespace

void InstallBotLoginLink(
		not_null<Ui::RpWidget*> parent,
		not_null<Ui::RpWidget*> anchor,
		Fn<void()> open) {
	const auto link = Ui::CreateChild<Ui::LinkButton>(parent, tr::lng_serein_bot_login(tr::now));
	link->setClickedCallback(std::move(open));
	rpl::combine(anchor->geometryValue(), link->widthValue()) | rpl::on_next([=](QRect geometry, int width) {
		link->moveToLeft((parent->width() - width) / 2, geometry.bottom() + 8 * st::lineWidth);
	}, link->lifetime());
	link->show();
}

Intro::details::Step *CreateBotLoginStep(
		QWidget *parent,
		not_null<Main::Account*> account,
		not_null<Intro::details::Data*> data) {
	return new BotLogin(parent, account, data);
}

} // namespace Serein::Privacy

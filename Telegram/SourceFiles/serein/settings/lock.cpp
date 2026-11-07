#include "serein/settings/lock.h"

#include "core/application.h"
#include "lang/lang_keys.h"
#include "main/main_domain.h"
#include "serein/core/options.h"
#include "serein/schema/gen/settings/privacy.h"
#include "storage/storage_domain.h"
#include "ui/rp_widget.h"
#include "ui/ui_utility.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/fields/password_input.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/vertical_layout.h"
#include "styles/style_boxes.h"
#include "styles/style_layers.h"
#include "styles/style_widgets.h"

#include <openssl/crypto.h>

namespace Serein {
namespace {

auto Unlocked = false;

[[nodiscard]] bool Locked() {
	return !Unlocked
		&& ForDevice().Get(Privacy::kLockSettings)
		&& Core::App().domain().local().hasLocalPasscode();
}

class LockWatcher final : public QObject {
public:
	explicit LockWatcher(QObject *parent) : QObject(parent) {
		Core::App().passcodeLockChanges(
		) | rpl::filter([](bool locked) {
			return locked;
		}) | rpl::on_next([](bool) {
			Unlocked = false;
		}, _lifetime);
	}

private:
	rpl::lifetime _lifetime;

};

} // namespace

void GuardSettings(not_null<Ui::VerticalLayout*> content, Fn<void()> build) {
	if (!Locked()) {
		build();
		return;
	}
	Ui::AddSkip(content);
	content->add(
		object_ptr<Ui::FlatLabel>(
			content,
			tr::lng_serein_settings_locked(),
			st::boxLabel),
		st::boxRowPadding);
	Ui::AddSkip(content);
	const auto row = content->add(
		object_ptr<Ui::RpWidget>(content),
		st::boxRowPadding);
	row->resize(row->width(), st::defaultInputField.heightMin);
	const auto field = Ui::CreateChild<Ui::PasswordInput>(
		row,
		st::defaultInputField,
		tr::lng_passcode_ph());
	row->widthValue() | rpl::on_next([=](int width) {
		field->resize(width, field->height());
	}, row->lifetime());
	Ui::AddSkip(content);
	const auto unlock = content->add(
		object_ptr<Ui::RoundButton>(
			content,
			tr::lng_passcode_submit(),
			st::defaultActiveButton),
		st::boxRowPadding,
		style::al_left);
	const auto submit = [=] {
		auto passcode = field->getLastText().toUtf8();
		const auto started = Core::App().domain().tryPasscode(
			passcode,
			crl::guard(row, [=](bool correct) {
				field->setDisabled(false);
				unlock->setDisabled(false);
				if (!correct || Core::App().passcodeLocked()) {
					field->selectAll();
					field->showError();
					return;
				}
				Unlocked = true;
				crl::on_main(row, [=] {
					content->clear();
					build();
				});
			}));
		OPENSSL_cleanse(passcode.data(), passcode.size());
		if (started) {
			field->setDisabled(true);
			unlock->setDisabled(true);
		}
	};
	unlock->setClickedCallback(submit);
	QObject::connect(
		field,
		&Ui::MaskedInputField::submitted,
		field,
		[=] { submit(); });
	crl::on_main(field, [=] {
		field->setFocusFast();
	});
}

void StartSettingsLock() {
	new LockWatcher(&Core::App());
}

} // namespace Serein

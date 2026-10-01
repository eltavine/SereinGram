#include "serein/settings/lock.h"

#include "core/application.h"
#include "lang/lang_keys.h"
#include "main/main_domain.h"
#include "serein/core/options.h"
#include "serein/schema/gen/settings/privacy.h"
#include "storage/storage_domain.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/fields/password_input.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/vertical_layout.h"
#include "styles/style_boxes.h"
#include "styles/style_layers.h"
#include "styles/style_widgets.h"

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
	const auto field = content->add(
		object_ptr<Ui::PasswordInput>(
			content,
			st::defaultInputField,
			tr::lng_passcode_ph()),
		st::boxRowPadding);
	Ui::AddSkip(content);
	const auto unlock = content->add(
		object_ptr<Ui::RoundButton>(
			content,
			tr::lng_passcode_submit(),
			st::defaultActiveButton),
		st::boxRowPadding,
		style::al_left);
	const auto submit = [=] {
		const auto passcode = field->getLastText().toUtf8();
		if (!Core::App().domain().local().checkPasscode(passcode)) {
			field->selectAll();
			field->showError();
			return;
		}
		Unlocked = true;
		crl::on_main(content, [=] {
			content->clear();
			build();
		});
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

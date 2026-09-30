#include "serein/links/settings.h"
#include "serein/links/model.h"

#include "core/application.h"
#include "core/core_settings.h"
#include "lang/lang_keys.h"
#include "serein/core/options.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/widgets/labels.h"
#include "ui/widgets/popup_menu.h"
#include "ui/wrap/vertical_layout.h"

#include <optional>

#include "styles/style_layers.h"
#include "styles/style_settings.h"

namespace Serein::Links {
namespace {

std::optional<LinkRules> Current() {
	return ReadRules(ForDevice().Get(kRules));
}

bool Save(
		not_null<Ui::GenericBox*> box,
		const LinkRules &before,
		const LinkRules &after) {
	const auto raw = WriteRules(after);
	if (Current() != before) {
		box->showToast(tr::lng_serein_config_changed_error(tr::now));
		return false;
	} else if (!Validate(raw)) {
		box->showToast(tr::lng_serein_link_invalid(tr::now));
		return false;
	}
	Expects(ForDevice().Set(kRules, raw));
	return true;
}

void PreviewBox(not_null<Ui::GenericBox*> box, LinkRules config) {
	box->setTitle(tr::lng_serein_link_preview());

	const auto field = box->addRow(object_ptr<Ui::InputField>(
		box, st::defaultInputField, Ui::InputField::Mode::SingleLine,
		tr::lng_serein_link_sample()));
	field->setMaxLength(16384);
	const auto result = box->addRow(object_ptr<Ui::FlatLabel>(
		box, tr::lng_serein_link_preview_about(), st::boxLabel));
	result->setSelectable(true);
	box->addButton(tr::lng_serein_link_preview(), [=] {
		const auto rewritten = Rewrite(config, field->getLastText());
		result->setText(rewritten.error.isEmpty()
			? rewritten.url.toString(QUrl::FullyEncoded)
			: rewritten.error);
	});
	box->addButton(tr::lng_close(), [=] { box->closeBox(); });
}

void RuleBox(
		not_null<Ui::GenericBox*> box,
		LinkRules config,
		int index) {
	box->setTitle(tr::lng_serein_link_rule());

	const auto existing = (index < int(config.rules.size()));
	const auto rule = existing ? config.rules[index] : NewRule();
	box->addRow(object_ptr<Ui::FlatLabel>(box, tr::lng_serein_link_rule_about(), st::boxLabel));
	const auto host = box->addRow(object_ptr<Ui::InputField>(
		box, st::defaultInputField, Ui::InputField::Mode::SingleLine,
		tr::lng_serein_link_host(), rule.host));
	const auto replacement = box->addRow(object_ptr<Ui::InputField>(
		box, st::defaultInputField, Ui::InputField::Mode::SingleLine,
		tr::lng_serein_link_replacement(), rule.replacementHost));
	auto names = QStringList();
	for (const auto &name : rule.removeParameters) {
		names.push_back(name);
	}
	const auto parameters = box->addRow(object_ptr<Ui::InputField>(
		box, st::defaultInputField, Ui::InputField::Mode::MultiLine,
		tr::lng_serein_link_parameters(), names.join('\n')));
	host->setMaxLength(253);
	replacement->setMaxLength(253);
	parameters->setMaxLength(2200);
	const auto enabled = box->addRow(object_ptr<Ui::Checkbox>(
		box, tr::lng_serein_link_enabled(tr::now), rule.enabled));
	const auto updated = [=] {
		auto changed = rule;
		changed.host = host->getLastText().trimmed().toLower();
		changed.replacementHost = replacement->getLastText().trimmed().toLower();
		changed.enabled = enabled->checked();
		changed.removeParameters.clear();
		for (const auto &part : parameters->getLastText().split('\n')) {
			if (const auto name = part.trimmed(); !name.isEmpty()) {
				changed.removeParameters.push_back(name);
			}
		}
		auto result = config;
		if (existing) {
			result.rules[index] = changed;
		} else {
			result.rules.push_back(changed);
		}
		return result;
	};
	box->addButton(tr::lng_settings_save(), [=] {
		if (Save(box, config, updated())) {
			box->closeBox();
		}
	});
	box->addButton(tr::lng_serein_link_preview(), [=] {
		const auto sample = updated();
		if (!Validate(WriteRules(sample))) {
			box->showToast(tr::lng_serein_link_invalid(tr::now));
			return;
		}
		const auto position = existing ? index : int(sample.rules.size()) - 1;
		auto single = sample.rules[position];
		single.enabled = true;
		box->getDelegate()->show(
			Box(PreviewBox, LinkRules{ .rules = { single } }),
			Ui::LayerOption::KeepOther);
	});
	box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
}

} // namespace

void SettingsBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(tr::lng_serein_link_rules());

	box->addRow(object_ptr<Ui::FlatLabel>(box, tr::lng_serein_link_rules_about(), st::boxLabel));
	const auto rows = box->addRow(object_ptr<Ui::VerticalLayout>(box));
	const auto refresh = [=] {
		rows->clear();
		const auto config = Current();
		if (!config) {
			rows->add(object_ptr<Ui::FlatLabel>(rows, tr::lng_serein_link_invalid(), st::boxLabel));
			return;
		}
		const auto confirm = rows->add(object_ptr<Ui::Checkbox>(
			rows, tr::lng_serein_link_confirm_all(tr::now), config->confirmAll));
		confirm->checkedChanges() | rpl::on_next([=](bool value) {
			auto updated = *config;
			updated.confirmAll = value;
			Save(box, *config, updated);
		}, confirm->lifetime());
		const auto count = int(config->rules.size());
		for (auto index = 0; index != count; ++index) {
			const auto &rule = config->rules[index];
			const auto button = rows->add(object_ptr<Ui::SettingsButton>(
				rows, rpl::single(rule.host
					+ (rule.enabled ? QString() : u" · "_q + tr::lng_serein_config_off(tr::now))),
				st::settingsButtonNoIcon));
			button->setClickedCallback([=] {
				const auto menu = Ui::CreateChild<Ui::PopupMenu>(box);
				menu->addAction(tr::lng_serein_link_rule(tr::now), [=] {
					box->getDelegate()->show(Box(RuleBox, *config, index), Ui::LayerOption::KeepOther);
				});
				for (const auto delta : { -1, 1 }) {
					if (index + delta < 0 || index + delta >= count) {
						continue;
					}
					menu->addAction(delta < 0 ? tr::lng_link_move_up(tr::now) : tr::lng_link_move_down(tr::now), [=] {
						auto updated = *config;
						std::swap(updated.rules[index], updated.rules[index + delta]);
						Save(box, *config, updated);
					});
				}
				menu->addAction(tr::lng_box_delete(tr::now), [=] {
					auto updated = *config;
					updated.rules.erase(updated.rules.begin() + index);
					Save(box, *config, updated);
				});
				menu->popup(QCursor::pos());
			});
		}
	};
	ForDevice().changes() | rpl::on_next([=](std::string_view) {
		InvokeQueued(box, refresh);
	}, box->lifetime());
	box->addButton(tr::lng_serein_link_add(), [=] {
		if (const auto config = Current()) {
			box->getDelegate()->show(Box(RuleBox, *config, int(config->rules.size())), Ui::LayerOption::KeepOther);
		}
	});
	box->addButton(tr::lng_serein_link_preview(), [=] {
		if (const auto config = Current()) {
			box->getDelegate()->show(Box(PreviewBox, *config), Ui::LayerOption::KeepOther);
		}
	});
	box->addButton(tr::lng_close(), [=] { box->closeBox(); });
	refresh();
}

} // namespace Serein::Links

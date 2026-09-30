#include "serein/filters/settings.h"
#include "serein/filters/model.h"

#include "lang/lang_keys.h"
#include "main/main_session.h"

#include "serein/core/options.h"

#include "ui/boxes/confirm_box.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/vertical_layout.h"

#include <QtCore/QRegularExpression>
#include <QtCore/QUuid>
#include <QtGui/QClipboard>
#include <QtWidgets/QApplication>

#include "styles/style_layers.h"
#include "styles/style_settings.h"

#include <algorithm>

namespace Serein::Filters {
namespace {

using IdList = std::vector<QString> FilterRules::*;

[[nodiscard]] bool Valid(const FilterRules &value) {
	return ReadRules(SerializeFilterRules(value)).has_value();
}

[[nodiscard]] std::optional<FilterRules> ReadFilters(
		not_null<Main::Session*> session) {
	return ReadRules(ForAccount(session).Get(kRules));
}

[[nodiscard]] std::vector<QString> ParsePeers(const QString &text) {
	auto result = std::vector<QString>();
	static const auto separators = QRegularExpression(u"[,\\s]+"_q);
	for (const auto &part : text.split(separators, Qt::SkipEmptyParts)) {
		if (std::find(result.begin(), result.end(), part) == result.end()) {
			result.push_back(part);
		}
	}
	return result;
}

bool Save(
		not_null<Ui::GenericBox*> box,
		not_null<Main::Session*> session,
		const FilterRules &expected,
		const FilterRules &value) {
	if (ReadFilters(session) != expected) {
		box->showToast(tr::lng_serein_config_changed_error(tr::now));
		return false;
	} else if (!Valid(value)) {
		box->showToast(tr::lng_serein_filter_invalid(tr::now));
		return false;
	}
	Expects(ForAccount(session).Set(kRules, (value == FilterRules())
		? QByteArray()
		: SerializeFilterRules(value)));
	return true;
}

void Saved(
		not_null<Main::Session*> session,
		const Fn<void(FilterRules)> &changed) {
	if (const auto saved = ReadFilters(session)) {
		changed(*saved);
	}
}

void PreviewBox(not_null<Ui::GenericBox*> box, FilterRules config) {
	box->setTitle(tr::lng_serein_filter_preview());

	const auto input = box->addRow(object_ptr<Ui::InputField>(
		box, st::defaultInputField, Ui::InputField::Mode::MultiLine));
	input->setMaxLength(16384);
	const auto result = box->addRow(object_ptr<Ui::FlatLabel>(
		box, tr::lng_serein_filter_preview_about(), st::boxLabel));
	result->setSelectable(true);
	const auto preview = [=] {
		if (!Valid(config)) {
			result->setText(tr::lng_serein_filter_invalid(tr::now));
			return;
		}
		auto active = config;
		active.enabled = true;
		const auto text = TextWithEntities{ input->getLastText() };
		const auto value = Apply(
			SerializeFilterRules(active),
			text, QString(), QString(), false, false, text.text);
		result->setText(!value.error.isEmpty() ? value.error
			: value.hidden ? tr::lng_serein_filter_hidden(tr::now)
			: value.text.text);
	};
	input->changes() | rpl::on_next(preview, input->lifetime());
	box->addButton(tr::lng_serein_filter_preview(), preview);
	box->addButton(tr::lng_close(), [=] { box->closeBox(); });
}

void RuleBox(
		not_null<Ui::GenericBox*> box,
		not_null<Main::Session*> session,
		FilterRules current,
		int index,
		Fn<void(FilterRules)> changed) {
	box->setTitle(tr::lng_serein_filter_rule());

	const auto count = int(current.rules.size());
	const auto original = (index < count)
		? current.rules[index]
		: FilterRule{
			.id = QUuid::createUuid().toString(QUuid::WithoutBraces),
			.action = u"mask"_q,
		};
	const auto field = [&](
			rpl::producer<QString> label,
			const QString &value,
			int maximum) {
		box->addRow(object_ptr<Ui::FlatLabel>(
			box, std::move(label), st::boxLabel));
		const auto input = box->addRow(object_ptr<Ui::InputField>(
			box, st::defaultInputField, Ui::InputField::Mode::SingleLine,
			rpl::single(QString()), value));
		input->setMaxLength(maximum);
		return input;
	};
	const auto title = field(
		tr::lng_serein_filter_title(), original.title, 128);
	const auto pattern = field(
		tr::lng_serein_filter_pattern(), original.pattern, 2048);
	const auto replacement = field(
		tr::lng_serein_filter_replacement(), original.replacement, 4096);
	const auto peers = field(
		tr::lng_serein_filter_rule_peers(),
		QStringList(original.peers.begin(), original.peers.end()).join(
			u", "_q),
		2200);
	const auto flags = std::array{
		std::pair(&FilterRule::enabled,
			tr::lng_serein_filter_rule_enabled(tr::now)),
		std::pair(&FilterRule::caseInsensitive,
			tr::lng_serein_filter_case(tr::now)),
		std::pair(&FilterRule::reversed,
			tr::lng_serein_filter_reverse(tr::now)),
	};
	auto toggles = std::vector<Ui::Checkbox*>();
	for (const auto &[member, label] : flags) {
		toggles.push_back(box->addRow(object_ptr<Ui::Checkbox>(
			box, label, original.*member)));
	}
	const auto actions = QStringList{ u"mask"_q, u"replace"_q, u"hide"_q };
	const auto group = std::make_shared<Ui::RadiobuttonGroup>(
		std::max(int(actions.indexOf(original.action)), 0));
	const auto labels = std::array{
		tr::lng_serein_filter_mask(tr::now),
		tr::lng_serein_filter_replace(tr::now),
		tr::lng_serein_filter_hide(tr::now),
	};
	for (auto i = 0; i != int(labels.size()); ++i) {
		box->addRow(object_ptr<Ui::Radiobutton>(box, group, i, labels[i]));
	}
	const auto collect = [=] {
		auto rule = original;
		rule.title = title->getLastText().trimmed();
		rule.pattern = pattern->getLastText();
		rule.replacement = replacement->getLastText();
		rule.peers = ParsePeers(peers->getLastText());
		for (auto i = 0; i != int(flags.size()); ++i) {
			rule.*(flags[i].first) = toggles[i]->checked();
		}
		rule.action = actions[group->current()];
		auto updated = current;
		if (index < count) {
			updated.rules[index] = rule;
		} else {
			updated.rules.push_back(rule);
		}
		return updated;
	};
	box->addButton(tr::lng_settings_save(), [=] {
		if (Save(box, session, current, collect())) {
			Saved(session, changed);
			box->closeBox();
		}
	});
	box->addButton(tr::lng_serein_filter_preview(), [=] {
		auto updated = collect();
		auto rule = updated.rules[index];
		rule.enabled = true;
		rule.peers.clear();
		updated.rules = { rule };
		box->uiShow()->showBox(Box(PreviewBox, updated));
	});
	if (index < count) {
		for (const auto delta : { -1, 1 }) {
			const auto target = index + delta;
			if (target < 0 || target >= count) {
				continue;
			}
			box->addButton(delta < 0
				? tr::lng_link_move_up()
				: tr::lng_link_move_down(), [=] {
				auto updated = current;
				std::swap(updated.rules[index], updated.rules[target]);
				if (Save(box, session, current, updated)) {
					Saved(session, changed);
					box->closeBox();
				}
			});
		}
		box->addButton(tr::lng_box_delete(), [=] {
			auto updated = current;
			updated.rules.erase(updated.rules.begin() + index);
			if (Save(box, session, current, updated)) {
				Saved(session, changed);
				box->closeBox();
			}
		});
	}
	box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
}

void PeerListBox(
		not_null<Ui::GenericBox*> box,
		not_null<Main::Session*> session,
		FilterRules current,
		IdList member,
		Fn<void(FilterRules)> changed) {
	box->setTitle((member == &FilterRules::hiddenAuthors)
		? tr::lng_serein_filter_hidden_authors()
		: tr::lng_serein_filter_excluded_chats());
	box->addRow(object_ptr<Ui::FlatLabel>(box,
		tr::lng_serein_filter_id_help(), st::boxLabel));
	const auto input = box->addRow(object_ptr<Ui::InputField>(
		box, st::defaultInputField, Ui::InputField::Mode::SingleLine,
		tr::lng_serein_filter_add_id()));
	input->setMaxLength(20);
	box->addButton(tr::lng_serein_filter_add(), [=] {
		const auto id = input->getLastText().trimmed();
		auto updated = current;
		auto &list = updated.*member;
		if (std::find(list.begin(), list.end(), id) == list.end()) {
			list.push_back(id);
		}
		if (Save(box, session, current, updated)) {
			Saved(session, changed);
			box->closeBox();
		}
	});
	const auto &list = current.*member;
	for (auto index = 0; index != int(list.size()); ++index) {
		const auto row = box->addRow(object_ptr<Ui::SettingsButton>(
			box, rpl::single(list[index]), st::settingsButtonNoIcon));
		row->setClickedCallback([=] {
			auto updated = current;
			auto &values = updated.*member;
			values.erase(values.begin() + index);
			if (Save(box, session, current, updated)) {
				Saved(session, changed);
				box->closeBox();
			}
		});
	}
	box->addButton(tr::lng_close(), [=] { box->closeBox(); });
}

void ImportRules(
		not_null<Ui::GenericBox*> box,
		not_null<Main::Session*> session,
		const FilterRules &current,
		Fn<void(FilterRules)> changed) {
	auto imported = ReadRuleList(QApplication::clipboard()->text().toUtf8());
	if (!imported) {
		box->showToast(tr::lng_serein_filter_invalid(tr::now));
		return;
	}
	auto names = QStringList();
	for (auto &rule : *imported) {
		rule.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
		rule.enabled = false;
		names.push_back(rule.title);
	}
	auto updated = current;
	updated.rules.insert(
		updated.rules.end(),
		imported->begin(),
		imported->end());
	if (!Valid(updated)) {
		box->showToast(tr::lng_serein_filter_invalid(tr::now));
		return;
	}
	box->uiShow()->showBox(Ui::MakeConfirmBox({
		.text = tr::lng_serein_filter_import_about(tr::now)
			+ u"\n\n"_q
			+ names.join('\n'),
		.confirmed = crl::guard(box, [=](Fn<void()> close) {
			if (Save(box, session, current, updated)) {
				Saved(session, changed);
				close();
			}
		}),
	}));
}

void FiltersBox(
		not_null<Ui::GenericBox*> box,
		not_null<Main::Session*> session) {
	box->setTitle(tr::lng_serein_filters());

	const auto initial = ReadFilters(session);
	if (!initial) {
		box->addRow(object_ptr<Ui::FlatLabel>(box,
			tr::lng_serein_filter_invalid(), st::boxLabel));
		box->addButton(tr::lng_close(), [=] { box->closeBox(); });
		return;
	}
	box->addRow(object_ptr<Ui::FlatLabel>(
		box, tr::lng_serein_filters_about(), st::boxLabel));
	const auto state = box->lifetime().make_state<
		rpl::variable<FilterRules>>(*initial);
	const auto rows = box->addRow(object_ptr<Ui::VerticalLayout>(box));
	const auto changed = crl::guard(box, [=](FilterRules value) {
		crl::on_main(box, [=] { *state = value; });
	});
	state->value() | rpl::on_next([=](const FilterRules &current) {
		rows->clear();
		const auto add = [&](QString text, Fn<void()> click) {
			const auto row = rows->add(object_ptr<Ui::SettingsButton>(
				rows, rpl::single(std::move(text)), st::settingsButtonNoIcon));
			row->setClickedCallback(std::move(click));
			return row;
		};
		for (const auto &[member, label] : std::array{
			std::pair(&FilterRules::enabled,
				tr::lng_serein_filter_enabled(tr::now)),
			std::pair(&FilterRules::filterOutgoing,
				tr::lng_serein_filter_outgoing(tr::now)),
			std::pair(&FilterRules::hideBlocked,
				tr::lng_serein_filter_blocked(tr::now)),
			std::pair(&FilterRules::stripZalgo,
				tr::lng_serein_filter_zalgo(tr::now)),
		}) {
			const auto row = add(label, nullptr);
			row->toggleOn(rpl::single(current.*member));
			row->toggledChanges() | rpl::on_next([=](bool value) {
				auto updated = current;
				updated.*member = value;
				if (Save(box, session, current, updated)) {
					Saved(session, changed);
				}
			}, row->lifetime());
		}
		for (const auto &[member, label] : std::array{
			std::pair(IdList(&FilterRules::hiddenAuthors),
				tr::lng_serein_filter_hidden_authors(tr::now)),
			std::pair(IdList(&FilterRules::excludedPeers),
				tr::lng_serein_filter_excluded_chats(tr::now)),
		}) {
			add(label, [=] {
				box->uiShow()->showBox(Box(PeerListBox,
					session, current, member, changed));
			});
		}
		const auto count = int(current.rules.size());
		for (auto i = 0; i != count; ++i) {
			add(QString::number(i + 1) + u". "_q + current.rules[i].title, [=] {
				box->uiShow()->showBox(Box(
					RuleBox, session, current, i, changed));
			});
		}
		add(tr::lng_serein_filter_add(tr::now), [=] {
			box->uiShow()->showBox(Box(
				RuleBox, session, current, count, changed));
		});
		add(tr::lng_serein_filter_preview(tr::now), [=] {
			box->uiShow()->showBox(Box(PreviewBox, current));
		});
		add(tr::lng_serein_filter_export(tr::now), [=] {
			auto rules = current.rules;
			for (auto &rule : rules) {
				rule.enabled = false;
				rule.peers.clear();
			}
			QApplication::clipboard()->setText(
				QString::fromUtf8(WriteRuleList(std::move(rules))));
			box->showToast(tr::lng_serein_filter_exported(tr::now));
		});
		add(tr::lng_serein_filter_import(tr::now), [=] {
			ImportRules(box, session, current, changed);
		});
	}, box->lifetime());
	box->addButton(tr::lng_close(), [=] { box->closeBox(); });
}

} // namespace

void SettingsBox(
		not_null<Ui::GenericBox*> box,
		not_null<Main::Session*> session) {
	FiltersBox(box, session);
}

} // namespace Serein::Filters

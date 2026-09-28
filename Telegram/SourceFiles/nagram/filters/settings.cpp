#include "nagram/filters/settings.h"
#include "nagram/filters/model.h"

#include "base/unique_qptr.h"
#include "core/application.h"
#include "core/core_settings.h"
#include "data/data_peer.h"
#include "history/history.h"
#include "history/history_item.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"

#include "nagram/core/options.h"

#include "settings/settings_builder.h"
#include "ui/boxes/confirm_box.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/widgets/labels.h"
#include "ui/widgets/popup_menu.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QUuid>
#include <QtGui/QClipboard>
#include <QtWidgets/QApplication>

#include "styles/style_layers.h"
#include "styles/style_settings.h"

namespace Nagram::Filters {
namespace {

QString ValidationError(const QJsonObject &value) {
	return Validate(QJsonDocument(value).toJson(QJsonDocument::Compact))
		? QString() : tr::lng_nagram_filter_invalid(tr::now);
}

QJsonObject ReadFilters(not_null<Main::Session*> session) {
	const auto bytes = ForAccount(session).Get(kRules);
	return bytes.isEmpty() ? Defaults()
		: QJsonDocument::fromJson(bytes).object();
}

bool Save(
		not_null<Ui::GenericBox*> box,
		not_null<Main::Session*> session,
		const QJsonObject &expected,
		const QJsonObject &value) {
	if (ReadFilters(session) != expected) {
		box->showToast(tr::lng_nagram_config_changed_error(tr::now));
		return false;
	}
	if (const auto error = ValidationError(value); !error.isEmpty()) {
		box->showToast(error);
		return false;
	}
	Expects(ForAccount(session).Set(kRules,
		value == Defaults() ? QByteArray()
			: QJsonDocument(value).toJson(QJsonDocument::Compact)));
	return true;
}

void PreviewBox(not_null<Ui::GenericBox*> box, QJsonObject config) {
	box->setTitle(tr::lng_nagram_filter_preview());

	const auto input = box->addRow(object_ptr<Ui::InputField>(
		box, st::defaultInputField, Ui::InputField::Mode::MultiLine));
	input->setMaxLength(16384);
	const auto result = box->addRow(object_ptr<Ui::FlatLabel>(
		box, tr::lng_nagram_filter_preview_about(), st::boxLabel));
	result->setSelectable(true);
	const auto preview = [=] {
		if (const auto error = ValidationError(config); !error.isEmpty()) {
			result->setText(error);
			return;
		}
		auto active = config;
		active.insert(u"enabled"_q, true);
		const auto text = TextWithEntities{ input->getLastText() };
		const auto value = Apply(
			QJsonDocument(active).toJson(QJsonDocument::Compact),
			text, QString(), QString(), false, false, text.text);
		result->setText(!value.error.isEmpty() ? value.error
			: value.hidden ? tr::lng_nagram_filter_hidden(tr::now)
			: value.text.text);
	};
	input->changes() | rpl::on_next(preview, input->lifetime());
	box->addButton(tr::lng_nagram_filter_preview(), preview);
	box->addButton(tr::lng_close(), [=] { box->closeBox(); });
}

void RuleBox(
		not_null<Ui::GenericBox*> box,
		not_null<Main::Session*> session,
		QJsonObject current,
		int index,
		Fn<void(QJsonObject)> changed) {
	box->setTitle(tr::lng_nagram_filter_rule());

	const auto rules = current.value(u"rules"_q).toArray();
	const auto original = index < rules.size() ? rules[index].toObject() : QJsonObject{
		{ u"id"_q, QUuid::createUuid().toString(QUuid::WithoutBraces) },
		{ u"title"_q, QString() },
		{ u"pattern"_q, QString() },
		{ u"enabled"_q, false },
		{ u"caseInsensitive"_q, false },
		{ u"reversed"_q, false },
		{ u"action"_q, u"mask"_q },
		{ u"replacement"_q, QString() },
	};
	const auto field = [&](rpl::producer<QString> label, QString key, int maximum) {
		box->addRow(object_ptr<Ui::FlatLabel>(box, std::move(label), st::boxLabel));
		const auto input = box->addRow(object_ptr<Ui::InputField>(
			box, st::defaultInputField, Ui::InputField::Mode::SingleLine,
			rpl::single(QString()), original.value(key).toString()));
		input->setMaxLength(maximum);
		return input;
	};
	const auto title = field(tr::lng_nagram_filter_title(), u"title"_q, 128);
	const auto pattern = field(tr::lng_nagram_filter_pattern(), u"pattern"_q, 2048);
	const auto replacement = field(tr::lng_nagram_filter_replacement(), u"replacement"_q, 4096);
	const auto flags = std::array{
		std::pair(u"enabled"_q, tr::lng_nagram_filter_rule_enabled(tr::now)),
		std::pair(u"caseInsensitive"_q, tr::lng_nagram_filter_case(tr::now)),
		std::pair(u"reversed"_q, tr::lng_nagram_filter_reverse(tr::now)),
	};
	auto toggles = std::vector<Ui::Checkbox*>();
	for (const auto &[key, label] : flags) {
		toggles.push_back(box->addRow(object_ptr<Ui::Checkbox>(
			box, label, original.value(key).toBool())));
	}
	const auto actions = QStringList{ u"mask"_q, u"replace"_q, u"hide"_q };
	const auto group = std::make_shared<Ui::RadiobuttonGroup>(
		actions.indexOf(original.value(u"action"_q) == u"maskMessage"_q
			? u"hide"_q : original.value(u"action"_q).toString()));
	const auto labels = std::array{
		tr::lng_nagram_filter_mask(tr::now), tr::lng_nagram_filter_replace(tr::now),
		tr::lng_nagram_filter_hide(tr::now),
	};
	for (auto i = 0; i != labels.size(); ++i) {
		box->addRow(object_ptr<Ui::Radiobutton>(box, group, i, labels[i]));
	}
	const auto collect = [=] {
		auto rule = original;
		rule.insert(u"title"_q, title->getLastText().trimmed());
		rule.insert(u"pattern"_q, pattern->getLastText());
		rule.insert(u"replacement"_q, replacement->getLastText());
		for (auto i = 0; i != flags.size(); ++i) {
			rule.insert(flags[i].first, toggles[i]->checked());
		}
		rule.insert(u"action"_q, actions[group->current()]);
		auto updated = current;
		auto list = rules;
		if (index < list.size()) {
			list[index] = rule;
		} else {
			list.push_back(rule);
		}
		updated.insert(u"rules"_q, list);
		return updated;
	};
	box->addButton(tr::lng_settings_save(), [=] {
		const auto updated = collect();
		if (Save(box, session, current, updated)) {
			changed(ReadFilters(session));
			box->closeBox();
		}
	});
	box->addButton(tr::lng_nagram_filter_preview(), [=] {
		auto updated = collect();
		auto rule = updated.value(u"rules"_q).toArray()[index].toObject();
		rule.insert(u"enabled"_q, true);
		updated.insert(u"rules"_q, QJsonArray{ rule });
		box->uiShow()->showBox(Box(PreviewBox, updated));
	});
	if (index < rules.size()) {
		for (const auto delta : { -1, 1 }) {
			if (index + delta < 0 || index + delta >= rules.size()) {
				continue;
			}
			box->addButton(delta < 0 ? tr::lng_link_move_up() : tr::lng_link_move_down(), [=] {
				auto updated = current;
				auto list = rules;
				const auto rule = list.takeAt(index);
				list.insert(index + delta, rule);
				updated.insert(u"rules"_q, list);
				if (Save(box, session, current, updated)) {
					changed(ReadFilters(session));
					box->closeBox();
				}
			});
		}
		box->addButton(tr::lng_box_delete(), [=] {
			auto updated = current;
			auto list = rules;
			list.removeAt(index);
			updated.insert(u"rules"_q, list);
			if (Save(box, session, current, updated)) {
				changed(ReadFilters(session));
				box->closeBox();
			}
		});
	}
	box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
}

void PeerListBox(
		not_null<Ui::GenericBox*> box,
		not_null<Main::Session*> session,
		QJsonObject current,
		QString key,
		Fn<void(QJsonObject)> changed) {
	box->setTitle(key == u"hiddenAuthors"_q
		? tr::lng_nagram_filter_hidden_authors()
		: tr::lng_nagram_filter_excluded_chats());
	box->addRow(object_ptr<Ui::FlatLabel>(box,
		tr::lng_nagram_filter_id_help(), st::boxLabel));
	const auto input = box->addRow(object_ptr<Ui::InputField>(
		box, st::defaultInputField, Ui::InputField::Mode::SingleLine,
		tr::lng_nagram_filter_add_id()));
	input->setMaxLength(20);
	box->addButton(tr::lng_nagram_filter_add(), [=] {
		const auto id = input->getLastText().trimmed();
		auto list = current.value(key).toArray();
		if (!list.contains(id)) {
			list.push_back(id);
		}
		auto updated = current;
		updated.insert(key, list);
		if (Save(box, session, current, updated)) {
			changed(ReadFilters(session));
			box->closeBox();
		}
	});
	const auto list = current.value(key).toArray();
	for (auto index = 0; index != list.size(); ++index) {
		const auto id = list[index].toString();
		const auto row = box->addRow(object_ptr<Ui::SettingsButton>(
			box, rpl::single(id), st::settingsButtonNoIcon));
		row->setClickedCallback([=] {
			auto updated = current;
			auto values = list;
			values.removeAt(index);
			updated.insert(key, values);
			if (Save(box, session, current, updated)) {
				changed(ReadFilters(session));
				box->closeBox();
			}
		});
	}
	box->addButton(tr::lng_close(), [=] { box->closeBox(); });
}

void FiltersBox(not_null<Ui::GenericBox*> box, not_null<Main::Session*> session) {
	box->setTitle(tr::lng_nagram_filters());

	const auto initial = ReadFilters(session);
	if (const auto error = ValidationError(initial); !error.isEmpty()) {
		box->addRow(object_ptr<Ui::FlatLabel>(box, rpl::single(error), st::boxLabel));
		box->addButton(tr::lng_close(), [=] { box->closeBox(); });
		return;
	}
	box->addRow(object_ptr<Ui::FlatLabel>(box, tr::lng_nagram_filters_about(), st::boxLabel));
	const auto state = box->lifetime().make_state<rpl::variable<QJsonObject>>(initial);
	const auto rows = box->addRow(object_ptr<Ui::VerticalLayout>(box));
		const auto changed = crl::guard(box, [=](QJsonObject value) {
		crl::on_main(box, [=] { *state = value; });
	});
	state->value() | rpl::on_next([=](const QJsonObject &current) {
		rows->clear();
		const auto add = [&](QString text, Fn<void()> click) {
			const auto row = rows->add(object_ptr<Ui::SettingsButton>(
				rows, rpl::single(std::move(text)), st::settingsButtonNoIcon));
			row->setClickedCallback(std::move(click));
			return row;
		};
		for (const auto &[key, label] : std::array{
			std::pair(u"enabled"_q, tr::lng_nagram_filter_enabled(tr::now)),
			std::pair(u"filterOutgoing"_q, tr::lng_nagram_filter_outgoing(tr::now)),
			std::pair(u"hideBlocked"_q, tr::lng_nagram_filter_blocked(tr::now)),
			std::pair(u"stripZalgo"_q, tr::lng_nagram_filter_zalgo(tr::now)),
		}) {
			const auto row = add(label, nullptr);
			row->toggleOn(rpl::single(current.value(key).toBool()));
			row->toggledChanges() | rpl::on_next([=](bool value) {
				auto updated = current;
				updated.insert(key, value);
				if (Save(box, session, current, updated)) {
					changed(ReadFilters(session));
				}
			}, row->lifetime());
		}
		for (const auto &[key, label] : std::array{
			std::pair(u"hiddenAuthors"_q,
				tr::lng_nagram_filter_hidden_authors(tr::now)),
			std::pair(u"excludedPeers"_q,
				tr::lng_nagram_filter_excluded_chats(tr::now)),
		}) {
			add(label, [=] {
				box->uiShow()->showBox(Box(PeerListBox,
					session, current, key, changed));
			});
		}
		const auto rules = current.value(u"rules"_q).toArray();
		for (auto i = 0; i < rules.size(); ++i) {
			const auto rule = rules[i].toObject();
			add(QString::number(i + 1) + u". "_q + rule.value(u"title"_q).toString(), [=] {
				box->uiShow()->showBox(Box(RuleBox, session, current, i, changed));
			});
		}
		add(tr::lng_nagram_filter_add(tr::now), [=] {
			box->uiShow()->showBox(Box(RuleBox, session, current, rules.size(), changed));
		});
		add(tr::lng_nagram_filter_preview(tr::now), [=] {
			box->uiShow()->showBox(Box(PreviewBox, current));
		});
		add(tr::lng_nagram_filter_export(tr::now), [=] {
			auto list = rules;
			for (auto i = 0; i < list.size(); ++i) {
				auto rule = list[i].toObject();
				rule.insert(u"enabled"_q, false);
				list[i] = rule;
			}
			const auto data = QJsonObject{ { u"version"_q, 1 }, { u"rules"_q, list } };
			QApplication::clipboard()->setText(QString::fromUtf8(QJsonDocument(data).toJson()));
			box->showToast(tr::lng_nagram_filter_exported(tr::now));
		});
		add(tr::lng_nagram_filter_import(tr::now), [=] {
			const auto text = QApplication::clipboard()->text();
			if (text.size() > 128 * 1024) {
				box->showToast(tr::lng_nagram_filter_invalid(tr::now));
				return;
			}
			const auto data = QJsonDocument::fromJson(text.toUtf8()).object();
			if (data.size() != 2
				|| data.value(u"version"_q) != 1 || !data.value(u"rules"_q).isArray()) {
				box->showToast(tr::lng_nagram_filter_invalid(tr::now));
				return;
			}
			auto imported = data.value(u"rules"_q).toArray();
			auto validated = Defaults();
			validated.insert(u"rules"_q, imported);
			if (const auto error = ValidationError(validated); !error.isEmpty()) {
				box->showToast(error);
				return;
			}
			for (auto i = 0; i < imported.size(); ++i) {
				auto rule = imported[i].toObject();
				rule.insert(u"id"_q, QUuid::createUuid().toString(QUuid::WithoutBraces));
				rule.insert(u"enabled"_q, false);
				imported[i] = rule;
			}
			auto updated = current;
			auto list = rules;
			for (const auto &rule : imported) {
				list.push_back(rule);
			}
			updated.insert(u"rules"_q, list);
			if (const auto error = ValidationError(updated); !error.isEmpty()) {
				box->showToast(error);
				return;
			}
			auto names = QStringList();
			for (const auto &rule : imported) {
				names.push_back(rule.toObject().value(u"title"_q).toString());
			}
			box->uiShow()->showBox(Ui::MakeConfirmBox({
				.text = tr::lng_nagram_filter_import_about(tr::now) + u"\n\n"_q + names.join('\n'),
				.confirmed = crl::guard(box, [=](Fn<void()> close) {
					if (Save(box, session, current, updated)) {
						changed(ReadFilters(session));
						close();
					}
				}),
			}));
		});
	}, box->lifetime());
	box->addButton(tr::lng_close(), [=] { box->closeBox(); });
}

} // namespace

void SettingsBox(not_null<Ui::GenericBox*> box, not_null<Main::Session*> session) {
	FiltersBox(box, session);
}

} // namespace Nagram::Filters

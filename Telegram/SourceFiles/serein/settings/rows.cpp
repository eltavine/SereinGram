#include "serein/settings/rows.h"

#include "serein/display/icon_tile.h"
#include "serein/settings/restart.h"
#include "settings/settings_common.h"
#include "ui/layers/generic_box.h"
#include "ui/painter.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"
#include "styles/style_layers.h"
#include "styles/style_serein.h"
#include "styles/style_settings.h"
#include "styles/style_widgets.h"

#include <algorithm>

namespace Serein {
namespace {

[[nodiscard]] RowVisual VisualOf(
		const style::icon *icon,
		const style::color *tile,
		const std::optional<tr::phrase<>> &about) {
	return { .icon = icon, .tile = tile, .about = about };
}

template <typename Type>
[[nodiscard]] Fn<Options&()> StoreFor(
		::Settings::Builder::SectionBuilder &builder,
		const Option<Type> &option) {
	if (option.scope == Scope::Account) {
		const auto session = builder.session();
		return [=]() -> Options & { return ForAccount(session); };
	}
	return []() -> Options & { return ForDevice(); };
}

void NumberBox(
		not_null<Ui::GenericBox*> box,
		NumberRow row,
		Fn<Options&()> store) {
	box->setTitle(row.title());
	const auto current = store().Get(*row.option);
	const auto field = box->addRow(object_ptr<Ui::InputField>(
		box,
		st::defaultInputField,
		row.hint ? (*row.hint)() : row.zeroLabel(),
		current ? QString::number(current) : QString()));
	field->setInputMethodHints(Qt::ImhDigitsOnly);
	box->setFocusCallback([=] { field->setFocusFast(); });
	const auto submit = [=] {
		const auto text = field->getLastText().trimmed();
		auto valid = false;
		const auto value = text.isEmpty() ? 0 : text.toInt(&valid);
		if (!text.isEmpty()
			&& (!valid || value < row.minimum || value > row.maximum)) {
			field->showError();
			return;
		} else if (!store().Set(*row.option, value)) {
			field->showError();
			return;
		}
		box->closeBox();
	};
	field->submits(
	) | rpl::on_next([=](auto) { submit(); }, field->lifetime());
	box->addButton(tr::lng_settings_save(), submit);
	box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
}

[[nodiscard]] QString ChoiceLabel(const ChoiceRow &row, int value) {
	const auto i = std::find(row.values.begin(), row.values.end(), value);
	const auto index = int(i - row.values.begin());
	return (index < int(row.labels.size()))
		? row.labels[index](tr::now)
		: (QString::number(value) + row.suffix);
}

void ChoiceBox(
		not_null<Ui::GenericBox*> box,
		ChoiceRow row,
		Fn<Options&()> store) {
	box->setTitle(row.title());
	const auto group = std::make_shared<Ui::RadiobuttonGroup>(
		store().Get(*row.option));
	for (const auto value : row.values) {
		box->addRow(object_ptr<Ui::Radiobutton>(
			box,
			group,
			value,
			ChoiceLabel(row, value),
			st::settingsSendType), st::settingsSendTypePadding);
	}
	group->setChangedCallback([=](int value) {
		Expects(store().Set(*row.option, value));
		box->closeBox();
	});
}

void TextBox(
		not_null<Ui::GenericBox*> box,
		const TextRow &row,
		Fn<Options&()> store) {
	box->setTitle(row.title());
	const auto field = box->addRow(
		object_ptr<Ui::InputField>(
			box,
			st::defaultInputField,
			row.placeholder(),
			store().Get(*row.option)),
		st::boxRowPadding);
	box->setFocusCallback([=] { field->setFocusFast(); });
	const auto submit = [=] {
		if (store().Set(*row.option, field->getLastText().trimmed())) {
			box->closeBox();
		} else {
			field->showError();
		}
	};
	field->submits() | rpl::on_next(submit, field->lifetime());
	box->addButton(tr::lng_settings_save(), submit);
	box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
}

} // namespace

Ui::SettingsButton *AddRow(
		::Settings::Builder::SectionBuilder &builder,
		RowArgs &&args) {
	auto result = (Ui::SettingsButton*)nullptr;
	const auto described = args.visual.about.has_value();
	const auto &st = args.st
		? *args.st
		: args.visual.icon
		? (described
			? st::sereinSettingsButtonDescribed
			: st::sereinSettingsButton)
		: (described
			? st::sereinSettingsButtonPlainDescribed
			: st::settingsButtonNoIcon);
	const auto searchIcon = args.visual.icon;
	builder.addControl({
		.factory = [&](not_null<Ui::VerticalLayout*> container) {
			auto wrap = object_ptr<Ui::VerticalLayout>(container);
			const auto raw = wrap.data();
			const auto button = raw->add(object_ptr<Ui::SettingsButton>(
				raw,
				rpl::duplicate(args.title),
				st));
			if (const auto icon = args.visual.icon) {
				const auto tile = args.visual.tile;
				Display::AddIconTile(
					button,
					*icon,
					tile ? *tile : st::settingsIconBg4);
			}
			if (args.label) {
				::Settings::CreateRightLabel(
					button,
					std::move(args.label),
					st,
					rpl::duplicate(args.title));
			}
			if (args.toggled) {
				button->toggleOn(std::move(args.toggled));
			}
			if (args.onClick) {
				button->addClickHandler(std::move(args.onClick));
			}
			if (const auto about = args.visual.about) {
				raw->add(
					object_ptr<Ui::FlatLabel>(
						raw,
						(*about)(),
						st::sereinSettingsAbout),
					args.visual.icon
						? st::sereinSettingsAboutPadding
						: st::sereinSettingsAboutPlainPadding);
			}
			result = button;
			return object_ptr<Ui::RpWidget>(std::move(wrap));
		},
		.id = std::move(args.id),
		.title = rpl::duplicate(args.title),
		.shown = std::move(args.shown),
		.keywords = std::move(args.keywords),
		.searchIcon = { searchIcon },
	});
	return result;
}

void AddPageButton(
		::Settings::Builder::SectionBuilder &builder,
		PageButton &&button) {
	const auto showOther = builder.showOther();
	const auto section = button.section;
	AddRow(builder, {
		.title = std::move(button.title),
		.onClick = [=] {
			if (showOther) {
				showOther(section);
			}
		},
		.keywords = std::move(button.keywords),
		.visual = {
			.icon = button.icon,
			.tile = button.tile,
			.about = button.about,
		},
	});
}

void AddToggle(
		::Settings::Builder::SectionBuilder &builder,
		const ToggleRow &row) {
	Expects(row.option != nullptr);

	const auto option = *row.option;
	const auto store = StoreFor(builder, option);
	const auto controller = builder.controller();
	const auto button = AddRow(builder, {
		.id = row.id,
		.title = row.title(),
		.toggled = store().Value(option),
		.keywords = row.keywords,
		.visual = VisualOf(row.icon, row.tile, row.about),
	});
	if (!button) {
		return;
	}
	button->toggledChanges(
	) | rpl::on_next([=](bool value) {
		Expects(store().Set(option, value));
		if (controller
			&& (option.flags & static_cast<unsigned>(Flag::RequiresRestart))) {
			ShowRestartPrompt(controller);
		}
	}, button->lifetime());
	if (const auto source = row.disabledBy) {
		StoreFor(builder, *source)().Value(
			*source
		) | rpl::on_next([=](bool disabled) {
			button->setDisabled(disabled);
			button->setEnabled(!disabled);
		}, button->lifetime());
	}
}

void AddToggles(
		::Settings::Builder::SectionBuilder &builder,
		std::span<const ToggleRow> rows) {
	for (const auto &row : rows) {
		AddToggle(builder, row);
	}
}

void AddNumber(
		::Settings::Builder::SectionBuilder &builder,
		const NumberRow &row) {
	Expects(row.option != nullptr);
	Expects(row.minimum <= row.maximum);

	const auto store = StoreFor(builder, *row.option);
	const auto controller = builder.controller();
	AddRow(builder, {
		.id = row.id,
		.title = row.title(),
		.label = store().Value(*row.option) | rpl::map([=](int value) {
			return !value
				? row.zeroLabel(tr::now)
				: row.format
				? row.format(value)
				: QString::number(value);
		}),
		.onClick = [=] {
			if (controller) {
				controller->show(Box(NumberBox, row, store));
			}
		},
		.keywords = row.keywords,
		.visual = VisualOf(row.icon, row.tile, row.about),
	});
}

void AddChoice(
		::Settings::Builder::SectionBuilder &builder,
		const ChoiceRow &row) {
	Expects(row.option != nullptr);
	Expects(!row.values.empty());

	const auto store = StoreFor(builder, *row.option);
	const auto controller = builder.controller();
	AddRow(builder, {
		.id = row.id,
		.title = row.title(),
		.label = store().Value(*row.option) | rpl::map([=](int value) {
			return ChoiceLabel(row, value);
		}),
		.onClick = [=] {
			if (controller) {
				controller->show(Box(ChoiceBox, row, store));
			}
		},
		.keywords = row.keywords,
		.visual = VisualOf(row.icon, row.tile, row.about),
	});
}

void AddText(
		::Settings::Builder::SectionBuilder &builder,
		const TextRow &row) {
	Expects(row.option != nullptr);

	const auto store = StoreFor(builder, *row.option);
	const auto controller = builder.controller();
	auto shown = rpl::producer<bool>();
	if (row.hiddenBy) {
		shown = StoreFor(builder, *row.hiddenBy)().Value(*row.hiddenBy)
			| rpl::map([](bool hidden) { return !hidden; });
	}
	AddRow(builder, {
		.id = row.id,
		.title = row.title(),
		.label = store().Value(*row.option) | rpl::map([=](const QString &text) {
			return text.isEmpty() ? row.placeholder(tr::now) : text;
		}),
		.onClick = [=] {
			if (controller) {
				controller->show(Box(TextBox, row, store));
			}
		},
		.keywords = row.keywords,
		.shown = std::move(shown),
		.visual = VisualOf(row.icon, row.tile, row.about),
	});
}

void AddSection(
		::Settings::Builder::SectionBuilder &builder,
		const SectionRow &row) {
	builder.addSkip();
	builder.addSubsectionTitle({
		.id = row.id,
		.title = row.title(),
		.keywords = row.keywords,
	});
}

void AddNote(
		::Settings::Builder::SectionBuilder &builder,
		tr::phrase<> text) {
	builder.addDividerText(text());
	builder.addSkip();
}

void EndSection(::Settings::Builder::SectionBuilder &builder) {
	builder.addSkip();
	builder.addDivider();
}

void EndSection(
		::Settings::Builder::SectionBuilder &builder,
		tr::phrase<> note) {
	builder.addSkip();
	builder.addDividerText(note());
}

} // namespace Serein

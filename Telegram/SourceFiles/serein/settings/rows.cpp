#include "serein/settings/rows.h"

#include "serein/settings/restart.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/fields/input_field.h"
#include "window/window_session_controller.h"
#include "styles/style_layers.h"
#include "styles/style_settings.h"
#include "styles/style_widgets.h"

namespace Serein {
namespace {

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
		row.zeroLabel(),
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

} // namespace

void AddToggle(
		::Settings::Builder::SectionBuilder &builder,
		const ToggleRow &row) {
	Expects(row.option != nullptr);

	const auto option = *row.option;
	const auto store = StoreFor(builder, option);
	const auto controller = builder.controller();
	const auto button = builder.addButton({
		.id = row.id,
		.title = row.title(),
		.st = &st::settingsButtonNoIcon,
		.toggled = store().Value(option),
		.keywords = row.keywords,
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
	builder.addButton({
		.id = row.id,
		.title = row.title(),
		.st = &st::settingsButtonNoIcon,
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
	});
}

void AddSection(
		::Settings::Builder::SectionBuilder &builder,
		const SectionRow &row) {
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
}

} // namespace Serein

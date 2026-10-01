#include "serein/compose/format_toolbar.h"

#include "base/unique_qptr.h"
#include "lang/lang_keys.h"
#include "serein/compose/options.h"
#include "serein/core/options.h"
#include "ui/ui_utility.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/widgets/inner_dropdown.h"
#include "ui/widgets/tooltip.h"
#include "styles/style_serein.h"

#include <QtGui/QCursor>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QTextEdit>

namespace Serein::Compose {
namespace {

enum class Kind {
	Tag,
	Link,
	Clear,
};

struct Action {
	Kind kind = Kind::Tag;
	QString tag;
	const style::icon *icon = nullptr;
	const style::icon *active = nullptr;
	QString label;
};

class Button final
	: public Ui::IconButton
	, public Ui::AbstractTooltipShower {
public:
	Button(QWidget *parent, const QString &label)
	: IconButton(parent, st::sereinFormatToolbarButton)
	, _label(label) {
		setAccessibleName(_label);
	}

	QString tooltipText() const override {
		return _label;
	}
	QPoint tooltipPos() const override {
		return QCursor::pos();
	}
	bool tooltipWindowActive() const override {
		return Ui::AppInFocus() && Ui::InFocusChain(window());
	}

protected:
	void enterEventHook(QEnterEvent *e) override {
		Ui::Tooltip::Show(1000, this);
		IconButton::enterEventHook(e);
	}
	void leaveEventHook(QEvent *e) override {
		Ui::Tooltip::Hide();
		IconButton::leaveEventHook(e);
	}

private:
	const QString _label;

};

struct Entry {
	not_null<Button*> button;
	Action action;
};

struct State {
	base::unique_qptr<Ui::InnerDropdown> panel;
	std::vector<Entry> entries;
	bool enabled = false;
	bool links = false;
};

[[nodiscard]] std::vector<Action> Actions() {
	using Field = Ui::InputField;
	const auto tag = [](
			const QString &id,
			const style::icon &icon,
			const style::icon &active,
			const QString &label) {
		return Action{ Kind::Tag, id, &icon, &active, label };
	};
	return {
		tag(
			Field::kTagBold,
			st::sereinFormatBold,
			st::sereinFormatBoldActive,
			tr::lng_menu_formatting_bold(tr::now)),
		tag(
			Field::kTagItalic,
			st::sereinFormatItalic,
			st::sereinFormatItalicActive,
			tr::lng_menu_formatting_italic(tr::now)),
		tag(
			Field::kTagUnderline,
			st::sereinFormatUnderline,
			st::sereinFormatUnderlineActive,
			tr::lng_menu_formatting_underline(tr::now)),
		tag(
			Field::kTagStrikeOut,
			st::sereinFormatStrikeOut,
			st::sereinFormatStrikeOutActive,
			tr::lng_menu_formatting_strike_out(tr::now)),
		tag(
			Field::kTagCode,
			st::sereinFormatCode,
			st::sereinFormatCodeActive,
			tr::lng_menu_formatting_monospace(tr::now)),
		tag(
			Field::kTagSpoiler,
			st::sereinFormatSpoiler,
			st::sereinFormatSpoilerActive,
			tr::lng_menu_formatting_spoiler(tr::now)),
		tag(
			Field::kTagBlockquote,
			st::sereinFormatQuote,
			st::sereinFormatQuoteActive,
			tr::lng_menu_formatting_blockquote(tr::now)),
		Action{
			Kind::Link,
			QString(),
			&st::sereinFormatLink,
			&st::sereinFormatLinkActive,
			tr::lng_menu_formatting_link_create(tr::now),
		},
		Action{
			Kind::Clear,
			QString(),
			&st::sereinFormatClear,
			&st::sereinFormatClear,
			tr::lng_menu_formatting_clear(tr::now),
		},
	};
}

[[nodiscard]] bool Available(
		not_null<Ui::InputField*> field,
		const Action &action,
		bool links) {
	const auto markdown = field->markdownEnabledState();
	switch (action.kind) {
	case Kind::Link: return links;
	case Kind::Clear: return true;
	case Kind::Tag: return markdown.enabledForTag(action.tag)
		&& (action.tag != Ui::InputField::kTagCode
			|| markdown.enabledForTag(Ui::InputField::kTagPre));
	}
	Unexpected("Kind in Serein::Compose::Available.");
}

[[nodiscard]] bool Active(
		not_null<Ui::InputField*> field,
		const Action &action) {
	switch (action.kind) {
	case Kind::Link: return field->hasCurrentMarkdownLink();
	case Kind::Clear: return false;
	case Kind::Tag: return field->isMarkdownTagActive(
		field->selectionMarkdownTagForToggle(action.tag));
	}
	Unexpected("Kind in Serein::Compose::Active.");
}

void Apply(not_null<Ui::InputField*> field, const Action &action) {
	switch (action.kind) {
	case Kind::Link: field->editCurrentMarkdownLink(); return;
	case Kind::Clear: field->clearCurrentMarkdown(); return;
	case Kind::Tag: field->toggleCurrentMarkdownTag(action.tag); return;
	}
	Unexpected("Kind in Serein::Compose::Apply.");
}

void Layout(not_null<State*> state, not_null<Ui::InputField*> field) {
	auto left = 0;
	for (const auto &entry : state->entries) {
		const auto available = Available(field, entry.action, state->links);
		entry.button->setVisible(available);
		if (available) {
			entry.button->moveToLeft(left, 0);
			left += entry.button->width();
		}
		const auto icon = (available && Active(field, entry.action))
			? entry.action.active
			: entry.action.icon;
		entry.button->setIconOverride(icon, icon);
	}
	const auto content = state->entries.front().button->parentWidget();
	content->resize(left, st::sereinFormatToolbarButton.height);
	state->panel->resizeToContent();
}

void Place(
		not_null<Ui::InnerDropdown*> panel,
		not_null<Ui::InputField*> field) {
	const auto edit = field->rawTextEdit();
	const auto viewport = edit->viewport();
	const auto parent = panel->parentWidget();
	const auto cursor = edit->textCursor();
	auto from = cursor;
	from.setPosition(cursor.selectionStart());
	auto till = cursor;
	till.setPosition(cursor.selectionEnd());
	const auto area = QRect(field->mapTo(parent, QPoint()), field->size());
	const auto start = viewport->mapTo(
		parent,
		edit->cursorRect(from).topLeft());
	const auto end = viewport->mapTo(
		parent,
		edit->cursorRect(till).bottomLeft());
	const auto top = std::max(start.y(), area.y());
	const auto bottom = std::min(end.y(), area.y() + area.height());
	const auto &padding = st::sereinFormatToolbar.padding;
	const auto skip = st::sereinFormatToolbarSkip;
	auto y = top - skip - panel->height() + padding.bottom();
	if (y + padding.top() < 0) {
		y = bottom + skip - padding.top();
	}
	const auto minLeft = -padding.left();
	const auto maxLeft = std::max(
		parent->width() - panel->width() + padding.right(),
		minLeft);
	panel->move(
		std::clamp(start.x() - padding.left(), minLeft, maxLeft),
		y);
}

void Build(not_null<State*> state, not_null<Ui::InputField*> field);

void Refresh(not_null<State*> state, not_null<Ui::InputField*> field) {
	const auto show = state->enabled
		&& field->isVisible()
		&& field->hasFocus()
		&& field->textCursor().hasSelection()
		&& !field->markdownEnabledState().disabled();
	if (!show) {
		if (state->panel) {
			state->panel->hideAnimated();
		}
		return;
	} else if (!state->panel) {
		Build(state, field);
	}
	const auto panel = state->panel.get();
	Layout(state, field);
	Place(panel, field);
	if (panel->isHidden() || panel->isHiding()) {
		panel->raise();
		panel->showAnimated(Ui::PanelAnimation::Origin::BottomLeft);
	}
}

void Build(not_null<State*> state, not_null<Ui::InputField*> field) {
	state->panel = base::make_unique_q<Ui::InnerDropdown>(
		field->window(),
		st::sereinFormatToolbar);
	const auto panel = state->panel.get();
	panel->setAutoHiding(false);
	auto content = object_ptr<Ui::RpWidget>(panel);
	const auto raw = content.data();
	for (const auto &action : Actions()) {
		const auto button = Ui::CreateChild<Button>(raw, action.label);
		button->setClickedCallback([=] {
			Apply(field, action);
			Refresh(state, field);
		});
		state->entries.push_back({ button, action });
	}
	panel->setOwnedWidget(std::move(content));
}

} // namespace

void InstallFormatToolbar(not_null<Ui::InputField*> field, bool links) {
	const auto state = field->lifetime().make_state<State>();
	state->links = links;
	const auto refresh = [=] {
		Refresh(state, field);
	};
	const auto edit = field->rawTextEdit();
	QObject::connect(
		edit.get(),
		&QTextEdit::selectionChanged,
		field.get(),
		refresh);
	QObject::connect(
		edit->verticalScrollBar(),
		&QScrollBar::valueChanged,
		field.get(),
		refresh);
	rpl::merge(
		field->focusedChanges() | rpl::to_empty,
		field->geometryValue() | rpl::to_empty
	) | rpl::on_next(refresh, field->lifetime());
	ForDevice().Value(kFormatToolbar) | rpl::on_next([=](bool enabled) {
		state->enabled = enabled;
		refresh();
	}, field->lifetime());
}

} // namespace Serein::Compose

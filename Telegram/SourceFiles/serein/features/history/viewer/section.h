#pragma once

#include "history/view/history_view_corner_buttons.h"
#include "history/view/history_view_list_widget.h"
#include "serein/features/history/viewer/source.h"
#include "window/section_memento.h"
#include "window/section_widget.h"

class History;

namespace Ui {
class ChatTheme;
class ElasticScroll;
class PlainShadow;
} // namespace Ui

namespace Ui::Menu {
struct MenuCallback;
} // namespace Ui::Menu

namespace Serein::HistoryFeature::Viewer {

class Memento;
class TopBar;

class Widget final
	: public Window::SectionWidget
	, private HistoryView::WindowListDelegate
	, private HistoryView::CornerButtonsDelegate {
public:
	Widget(
		QWidget *parent,
		not_null<Window::SessionController*> controller,
		not_null<::History*> history,
		Query query,
		std::unique_ptr<Source> source);
	~Widget();

	Dialogs::RowDescriptor activeChat() const override;
	bool hasTopBarShadow() const override {
		return true;
	}
	QPixmap grabForShowAnimation(
		const Window::SectionSlideParams &params) override;
	bool showInternal(
		not_null<Window::SectionMemento*> memento,
		const Window::SectionShow &params) override;
	std::shared_ptr<Window::SectionMemento> createMemento() override;
	std::shared_ptr<Window::SectionMemento> createIdentityMemento() override;
	void setInternalState(const QRect &geometry, not_null<Memento*> memento);

	bool floatPlayerHandleWheelEvent(QEvent *e) override;
	QRect floatPlayerAvailableRect() override;

	HistoryView::Context listContext() override;
	bool listScrollTo(int top, bool syntetic = true) override;
	void listCancelRequest() override;
	void listDeleteRequest() override;
	void listTryProcessKeyInput(not_null<QKeyEvent*> e) override;
	rpl::producer<Data::MessagesSlice> listSource(
		Data::MessagePosition aroundId,
		int limitBefore,
		int limitAfter) override;
	bool listAllowsMultiSelect() override;
	bool listIsItemGoodForSelection(not_null<HistoryItem*> item) override;
	bool listIsLessInOrder(
		not_null<HistoryItem*> first,
		not_null<HistoryItem*> second) override;
	void listSelectionChanged(HistoryView::SelectedItems &&items) override;
	void listMarkReadTill(not_null<HistoryItem*> item) override;
	void listMarkContentsRead(
		const base::flat_set<not_null<HistoryItem*>> &items) override;
	HistoryView::MessagesBarData listMessagesBar(
		const std::vector<not_null<HistoryView::Element*>> &elements,
		bool markLastAsRead) override;
	void listContentRefreshed() override;
	void listUpdateDateLink(
		ClickHandlerPtr &link,
		not_null<HistoryView::Element*> view) override;
	bool listElementHideReply(
		not_null<const HistoryView::Element*> view) override;
	bool listElementShownUnread(
		not_null<const HistoryView::Element*> view) override;
	bool listIsGoodForAroundPosition(
		not_null<const HistoryView::Element*> view) override;
	void listSendBotCommand(
		const QString &command,
		const FullMsgId &context) override;
	void listSearch(const QString &query, const FullMsgId &context) override;
	void listHandleViaClick(not_null<UserData*> bot) override;
	not_null<Ui::ChatTheme*> listChatTheme() override;
	HistoryView::CopyRestrictionType listCopyRestrictionType(
		HistoryItem *item) override;
	HistoryView::CopyRestrictionType listCopyMediaRestrictionType(
		not_null<HistoryItem*> item) override;
	HistoryView::CopyRestrictionType listSelectRestrictionType() override;
	auto listAllowedReactionsValue()
		-> rpl::producer<Data::AllowedReactions> override;
	void listShowPremiumToast(not_null<DocumentData*> document) override;
	void listOpenPhoto(
		not_null<PhotoData*> photo,
		FullMsgId context) override;
	void listOpenDocument(
		not_null<DocumentData*> document,
		FullMsgId context,
		bool showInMediaView) override;
	void listPaintEmpty(
		Painter &p,
		const Ui::ChatPaintContext &context) override;
	QString listElementAuthorRank(
		not_null<const HistoryView::Element*> view) override;
	bool listElementHideTopicButton(
		not_null<const HistoryView::Element*> view) override;
	::History *listTranslateHistory() override;
	void listAddTranslatedItems(
		not_null<HistoryView::TranslateTracker*> tracker) override;
	Ui::ElasticScroll *listScrollArea() const override;
	bool listThanosEffectEnabled() const override;

	void cornerButtonsShowAtPosition(
		Data::MessagePosition position) override;
	Data::Thread *cornerButtonsThread() override;
	FullMsgId cornerButtonsCurrentId() override;
	bool cornerButtonsIgnoreVisibility() override;
	std::optional<bool> cornerButtonsDownShown() override;
	bool cornerButtonsUnreadMayBeShown() override;
	bool cornerButtonsHas(HistoryView::CornerButtonType type) override;

private:
	void resizeEvent(QResizeEvent *e) override;
	void paintEvent(QPaintEvent *e) override;
	void showAnimatedHook(
		const Window::SectionSlideParams &params) override;
	void showFinishedHook() override;
	void doSetInnerFocus() override;
	void checkActivation() override;

	void onScroll();
	void updateInnerVisibleArea();
	void updateControlsGeometry();
	void updateAdaptiveLayout();
	void recountChatWidth();
	void fillMenu(const Ui::Menu::MenuCallback &add);
	[[nodiscard]] rpl::producer<QString> subtitle() const;

	const not_null<::History*> _history;
	const Query _query;
	std::unique_ptr<Source> _source;
	std::shared_ptr<Ui::ChatTheme> _theme;
	object_ptr<TopBar> _topBar;
	object_ptr<Ui::PlainShadow> _topBarShadow;
	std::unique_ptr<Ui::ElasticScroll> _scroll;
	QPointer<HistoryView::ListWidget> _inner;
	HistoryView::CornerButtons _cornerButtons;
	bool _skipScrollEvent = false;

};

class Memento final : public Window::SectionMemento {
public:
	Memento(not_null<::History*> history, Query query);

	object_ptr<Window::SectionWidget> createWidget(
		QWidget *parent,
		not_null<Window::SessionController*> controller,
		Window::Column column,
		const QRect &geometry) override;

	[[nodiscard]] not_null<::History*> history() const {
		return _history;
	}
	[[nodiscard]] const Query &query() const {
		return _query;
	}
	[[nodiscard]] not_null<HistoryView::ListMemento*> list() {
		return &_list;
	}
	void setSource(std::unique_ptr<Source> source) {
		_source = std::move(source);
	}

private:
	const not_null<::History*> _history;
	const Query _query;
	std::unique_ptr<Source> _source;
	HistoryView::ListMemento _list;

};

} // namespace Serein::HistoryFeature::Viewer

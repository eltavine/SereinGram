#include "serein/features/history/viewer/section.h"

#include "serein/features/history/viewer.h"
#include "serein/features/history/viewer/top_bar.h"
#include "data/data_peer_values.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "history/history_view_swipe_back_session.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/chat/chat_style.h"
#include "ui/chat/chat_theme.h"
#include "ui/widgets/elastic_scroll.h"
#include "ui/widgets/menu/menu_add_action_callback.h"
#include "ui/widgets/shadow.h"
#include "ui/ui_utility.h"
#include "window/window_adaptive.h"
#include "window/window_session_controller.h"
#include "styles/style_chat.h"
#include "styles/style_chat_helpers.h"
#include "styles/style_menu_icons.h"
#include "styles/style_window.h"

namespace Serein::HistoryFeature::Viewer {

Memento::Memento(not_null<::History*> history, Query query)
: _history(history)
, _query(query) {
}

object_ptr<Window::SectionWidget> Memento::createWidget(
		QWidget *parent,
		not_null<Window::SessionController*> controller,
		Window::Column column,
		const QRect &geometry) {
	if (column == Window::Column::Third) {
		return nullptr;
	}
	auto result = object_ptr<Widget>(
		parent,
		controller,
		_history,
		_query,
		std::move(_source));
	result->setInternalState(geometry, this);
	return result;
}

Widget::Widget(
	QWidget *parent,
	not_null<Window::SessionController*> controller,
	not_null<::History*> history,
	Query query,
	std::unique_ptr<Source> source)
: Window::SectionWidget(parent, controller, history->peer)
, WindowListDelegate(controller)
, _history(history)
, _query(query)
, _source(source ? std::move(source) : MakeSource(history, query))
, _topBar(
	this,
	controller,
	history->peer,
	subtitle(),
	[=](const Ui::Menu::MenuCallback &add) { fillMenu(add); })
, _topBarShadow(this)
, _scroll(std::make_unique<Ui::ElasticScroll>(
	this,
	controller->chatStyle()->value(lifetime(), st::historyScroll)))
, _cornerButtons(
	_scroll.get(),
	controller->chatStyle(),
	static_cast<HistoryView::CornerButtonsDelegate*>(this)) {
	controller->chatStyle()->paletteChanged(
	) | rpl::on_next([=] {
		_scroll->updateBars();
	}, _scroll->lifetime());

	Window::ChatThemeValueFromPeer(
		controller,
		history->peer
	) | rpl::on_next([=](std::shared_ptr<Ui::ChatTheme> &&theme) {
		_theme = std::move(theme);
		controller->setChatStyleTheme(_theme);
	}, lifetime());

	_topBar->move(0, 0);
	_topBar->resizeToWidth(width());
	_topBar->show();
	_topBarShadow->raise();
	controller->adaptive().value(
	) | rpl::on_next([=] {
		updateAdaptiveLayout();
	}, lifetime());

	_scroll->setHandleTouch(false);
	_inner = _scroll->setOwnedWidget(object_ptr<HistoryView::ListWidget>(
		this,
		&controller->session(),
		static_cast<HistoryView::ListDelegate*>(this)));
	_inner->lower();
	_scroll->move(0, _topBar->height());
	_scroll->show();
	_scroll->setOverscrollBg(QColor(0, 0, 0, 0));
	_scroll->setOverscrollEdges([=] {
		return _inner->loadedAtTopKnown() && _inner->loadedAtTop();
	}, [=] {
		return _inner->loadedAtBottomKnown() && _inner->loadedAtBottom();
	});
	_scroll->scrolls(
	) | rpl::on_next([=] {
		onScroll();
	}, lifetime());
	_inner->scrollKeyEvents(
	) | rpl::on_next([=](not_null<QKeyEvent*> e) {
		_scroll->keyPressEvent(e);
	}, lifetime());
	Window::SetupSwipeBackSection(this, _scroll.get(), _inner);

	_source->countValue(
	) | rpl::filter([](int count) {
		return !count;
	}) | rpl::on_next([=] {
		crl::on_main(this, [=] {
			this->controller()->showBackFromStack();
		});
	}, lifetime());
}

Widget::~Widget() = default;

rpl::producer<QString> Widget::subtitle() const {
	if (_query.versions()) {
		return tr::lng_serein_menu_edit_history();
	}
	return _source->countValue(
	) | rpl::filter([](int count) {
		return count >= 0;
	}) | rpl::map([](int count) {
		return tr::lng_serein_history_deleted_count(
			tr::now,
			lt_amount,
			QString::number(count));
	});
}

void Widget::fillMenu(const Ui::Menu::MenuCallback &add) {
	const auto peer = _history->peer;
	const auto window = controller();
	const auto messageId = _query.versions()
		? _query.messageId
		: ShowAtUnreadMsgId;
	add(tr::lng_serein_history_open_chat(tr::now), [=] {
		window->showPeerHistory(
			peer,
			Window::SectionShow::Way::Forward,
			messageId);
	}, &st::menuIconShowInChat);
	add({
		.text = tr::lng_serein_history_clear_chat(tr::now),
		.handler = [=] { ConfirmClearHistory(window, peer); },
		.icon = &st::menuIconDeleteAttention,
		.isAttention = true,
	});
}

Dialogs::RowDescriptor Widget::activeChat() const {
	return {
		_history,
		FullMsgId(_history->peer->id, ShowAtUnreadMsgId),
	};
}

QPixmap Widget::grabForShowAnimation(
		const Window::SectionSlideParams &params) {
	if (params.withTopBarShadow) {
		_topBarShadow->hide();
	}
	auto result = Ui::GrabWidget(this);
	if (params.withTopBarShadow) {
		_topBarShadow->show();
	}
	return result;
}

bool Widget::showInternal(
		not_null<Window::SectionMemento*> memento,
		const Window::SectionShow &params) {
	if (const auto mine = dynamic_cast<Memento*>(memento.get())) {
		if (mine->history() == _history && mine->query() == _query) {
			_inner->restoreState(mine->list());
			return true;
		}
	}
	return false;
}

std::shared_ptr<Window::SectionMemento> Widget::createMemento() {
	auto result = std::make_shared<Memento>(_history, _query);
	_inner->saveState(result->list());
	result->setSource(std::move(_source));
	return result;
}

auto Widget::createIdentityMemento()
-> std::shared_ptr<Window::SectionMemento> {
	return std::make_shared<Memento>(_history, _query);
}

void Widget::setInternalState(
		const QRect &geometry,
		not_null<Memento*> memento) {
	setGeometry(geometry);
	Ui::SendPendingMoveResizeEvents(this);
	_inner->restoreState(memento->list());
}

bool Widget::floatPlayerHandleWheelEvent(QEvent *e) {
	return _scroll->viewportEvent(e);
}

QRect Widget::floatPlayerAvailableRect() {
	return mapToGlobal(_scroll->geometry());
}

void Widget::resizeEvent(QResizeEvent *e) {
	if (!width() || !height()) {
		return;
	}
	recountChatWidth();
	updateControlsGeometry();
}

void Widget::recountChatWidth() {
	controller()->adaptive().setChatLayout((width() < st::adaptiveChatWideWidth)
		? Window::Adaptive::ChatLayout::Normal
		: Window::Adaptive::ChatLayout::Wide);
}

void Widget::updateControlsGeometry() {
	const auto contentWidth = width();
	const auto newScrollTop = _scroll->isHidden()
		? std::nullopt
		: base::make_optional(_scroll->scrollTop() + takeTopDelta());
	_topBar->resizeToWidth(contentWidth);
	_topBarShadow->resize(contentWidth, st::lineWidth);
	const auto top = _topBar->height();
	const auto scrollSize = QSize(contentWidth, height() - top);
	if (_scroll->size() != scrollSize) {
		_skipScrollEvent = true;
		_scroll->resize(scrollSize);
		_inner->resizeToWidth(scrollSize.width(), _scroll->height());
		_skipScrollEvent = false;
	}
	_scroll->move(0, top);
	if (!_scroll->isHidden()) {
		if (newScrollTop) {
			_scroll->scrollToY(*newScrollTop);
		}
		updateInnerVisibleArea();
	}
	_cornerButtons.updatePositions();
}

void Widget::updateAdaptiveLayout() {
	_topBarShadow->moveToLeft(
		controller()->adaptive().isOneColumn() ? 0 : st::lineWidth,
		_topBar->height());
}

void Widget::paintEvent(QPaintEvent *e) {
	if (animatingShow()) {
		SectionWidget::paintEvent(e);
		return;
	} else if (controller()->contentOverlapped(this, e) || !_theme) {
		return;
	}
	const auto aboveHeight = _topBar->height();
	const auto bg = e->rect().intersected(
		QRect(0, aboveHeight, width(), height() - aboveHeight));
	SectionWidget::PaintBackground(controller(), _theme.get(), this, bg);
}

void Widget::onScroll() {
	if (!_skipScrollEvent) {
		updateInnerVisibleArea();
	}
}

void Widget::updateInnerVisibleArea() {
	const auto scrollTop = _scroll->scrollTop();
	_inner->setVisibleTopBottom(scrollTop, scrollTop + _scroll->height());
	_cornerButtons.updateJumpDownVisibility();
	_cornerButtons.updateUnreadThingsVisibility();
}

void Widget::showAnimatedHook(const Window::SectionSlideParams &params) {
	_topBar->setAnimatingMode(true);
	if (params.withTopBarShadow) {
		_topBarShadow->show();
	}
}

void Widget::showFinishedHook() {
	_topBar->setAnimatingMode(false);
	_inner->showFinished();
}

void Widget::doSetInnerFocus() {
	_inner->setFocus();
}

void Widget::checkActivation() {
	_inner->checkActivation();
}

HistoryView::Context Widget::listContext() {
	return HistoryView::Context::AdminLog;
}

bool Widget::listScrollTo(int top, bool syntetic) {
	top = std::clamp(top, 0, _scroll->scrollTopMax());
	if (_scroll->scrollTop() == top) {
		updateInnerVisibleArea();
		return false;
	}
	_scroll->scrollToY(top);
	return true;
}

void Widget::listCancelRequest() {
	controller()->showBackFromStack();
}

void Widget::listDeleteRequest() {
}

void Widget::listTryProcessKeyInput(not_null<QKeyEvent*> e) {
}

rpl::producer<Data::MessagesSlice> Widget::listSource(
		Data::MessagePosition aroundId,
		int limitBefore,
		int limitAfter) {
	return _source
		? _source->slice(aroundId, limitBefore, limitAfter)
		: rpl::never<Data::MessagesSlice>();
}

bool Widget::listAllowsMultiSelect() {
	return false;
}

bool Widget::listIsItemGoodForSelection(not_null<HistoryItem*> item) {
	return false;
}

bool Widget::listIsLessInOrder(
		not_null<HistoryItem*> first,
		not_null<HistoryItem*> second) {
	return first->position() < second->position();
}

void Widget::listSelectionChanged(HistoryView::SelectedItems &&items) {
}

void Widget::listMarkReadTill(not_null<HistoryItem*> item) {
}

void Widget::listMarkContentsRead(
		const base::flat_set<not_null<HistoryItem*>> &items) {
}

HistoryView::MessagesBarData Widget::listMessagesBar(
		const std::vector<not_null<HistoryView::Element*>> &elements,
		bool markLastAsRead) {
	return {};
}

void Widget::listContentRefreshed() {
}

void Widget::listUpdateDateLink(
		ClickHandlerPtr &link,
		not_null<HistoryView::Element*> view) {
}

bool Widget::listElementHideReply(
		not_null<const HistoryView::Element*> view) {
	return false;
}

bool Widget::listElementShownUnread(
		not_null<const HistoryView::Element*> view) {
	return false;
}

bool Widget::listIsGoodForAroundPosition(
		not_null<const HistoryView::Element*> view) {
	return true;
}

void Widget::listSendBotCommand(
		const QString &command,
		const FullMsgId &context) {
}

void Widget::listSearch(const QString &query, const FullMsgId &context) {
	const auto inChat = _history->peer->isUser()
		? Dialogs::Key()
		: Dialogs::Key(_history);
	controller()->searchMessages(query, inChat);
}

void Widget::listHandleViaClick(not_null<UserData*> bot) {
}

not_null<Ui::ChatTheme*> Widget::listChatTheme() {
	return _theme.get();
}

HistoryView::CopyRestrictionType Widget::listCopyRestrictionType(
		HistoryItem *item) {
	return HistoryView::CopyRestrictionTypeFor(_history->peer, item);
}

HistoryView::CopyRestrictionType Widget::listCopyMediaRestrictionType(
		not_null<HistoryItem*> item) {
	return HistoryView::CopyMediaRestrictionTypeFor(_history->peer, item);
}

HistoryView::CopyRestrictionType Widget::listSelectRestrictionType() {
	return HistoryView::SelectRestrictionTypeFor(_history->peer);
}

auto Widget::listAllowedReactionsValue()
-> rpl::producer<Data::AllowedReactions> {
	return rpl::single(Data::AllowedReactions());
}

void Widget::listShowPremiumToast(not_null<DocumentData*> document) {
}

void Widget::listOpenPhoto(not_null<PhotoData*> photo, FullMsgId context) {
	controller()->openPhoto(photo, { .id = context });
}

void Widget::listOpenDocument(
		not_null<DocumentData*> document,
		FullMsgId context,
		bool showInMediaView) {
	controller()->openDocument(document, showInMediaView, { .id = context });
}

void Widget::listPaintEmpty(
		Painter &p,
		const Ui::ChatPaintContext &context) {
}

QString Widget::listElementAuthorRank(
		not_null<const HistoryView::Element*> view) {
	return QString();
}

bool Widget::listElementHideTopicButton(
		not_null<const HistoryView::Element*> view) {
	return true;
}

::History *Widget::listTranslateHistory() {
	return nullptr;
}

void Widget::listAddTranslatedItems(
		not_null<HistoryView::TranslateTracker*> tracker) {
}

Ui::ElasticScroll *Widget::listScrollArea() const {
	return _scroll.get();
}

bool Widget::listThanosEffectEnabled() const {
	return false;
}

void Widget::cornerButtonsShowAtPosition(Data::MessagePosition position) {
	_inner->showAtPosition(
		position,
		{},
		_cornerButtons.doneJumpFrom(position.fullId, {}));
}

Data::Thread *Widget::cornerButtonsThread() {
	return _history;
}

FullMsgId Widget::cornerButtonsCurrentId() {
	return {};
}

bool Widget::cornerButtonsIgnoreVisibility() {
	return animatingShow();
}

std::optional<bool> Widget::cornerButtonsDownShown() {
	const auto top = _scroll->scrollTop() + st::historyToDownShownAfter;
	if (top < _scroll->scrollTopMax() || _cornerButtons.replyReturn()) {
		return true;
	} else if (_inner->loadedAtBottomKnown()) {
		return !_inner->loadedAtBottom();
	}
	return std::nullopt;
}

bool Widget::cornerButtonsUnreadMayBeShown() {
	return _inner->loadedAtBottomKnown();
}

bool Widget::cornerButtonsHas(HistoryView::CornerButtonType type) {
	return (type == HistoryView::CornerButtonType::Down);
}

} // namespace Serein::HistoryFeature::Viewer

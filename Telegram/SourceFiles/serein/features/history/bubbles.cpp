#include "serein/features/history/bubbles.h"

#include "serein/features/history/restored_message.h"
#include "base/unixtime.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "history/admin_log/history_admin_log_item.h"
#include "history/history.h"
#include "history/history_item.h"
#include "history/view/history_view_element.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/chat/chat_style.h"
#include "ui/chat/chat_theme.h"
#include "ui/layers/generic_box.h"
#include "ui/painter.h"
#include "window/section_widget.h"
#include "window/window_session_controller.h"
#include "styles/style_chat.h"
#include "styles/style_serein.h"

namespace Serein::HistoryFeature {
namespace {

class Delegate final : public HistoryView::SimpleElementDelegate {
public:
	Delegate(
		not_null<Window::SessionController*> controller,
		Fn<void()> update)
	: SimpleElementDelegate(controller, std::move(update)) {
	}

	HistoryView::Context elementContext() override {
		return HistoryView::Context::History;
	}
	bool elementAnimationsPaused() override {
		return true;
	}

};

class Bubbles final : public Ui::RpWidget {
public:
	Bubbles(
		QWidget *parent,
		not_null<Window::SessionController*> controller,
		not_null<PeerData*> peer,
		std::vector<History::Record> records);

protected:
	int resizeGetHeight(int newWidth) override;
	void paintEvent(QPaintEvent *e) override;

private:
	void paintUserpic(
		QPainter &p,
		not_null<HistoryView::Element*> view,
		int top);

	const not_null<Window::SessionController*> _controller;
	Delegate _delegate;
	std::vector<AdminLog::OwnedItem> _items;
	base::flat_map<not_null<PeerData*>, Ui::PeerUserpicView> _userpics;

};

Bubbles::Bubbles(
	QWidget *parent,
	not_null<Window::SessionController*> controller,
	not_null<PeerData*> peer,
	std::vector<History::Record> records)
: RpWidget(parent)
, _controller(controller)
, _delegate(controller, [=] { update(); }) {
	ranges::sort(records, ranges::less(), &History::Record::date);
	const auto history = peer->owner().history(peer);
	auto previous = QDate();
	for (const auto &record : records) {
		auto owned = AdminLog::OwnedItem(&_delegate, MakeRestoredMessage(
			history,
			record,
			history->nextNonHistoryEntryId(),
			MessageFlag::FakeHistoryItem));
		const auto day = base::unixtime::parse(TimeId(record.date)).date();
		owned->setDisplayDate(day != previous);
		owned->initDimensions();
		previous = day;
		_items.push_back(std::move(owned));
	}
	controller->session().downloaderTaskFinished(
	) | rpl::on_next([=] {
		update();
	}, lifetime());
}

int Bubbles::resizeGetHeight(int newWidth) {
	auto result = 0;
	for (const auto &item : _items) {
		result += item->resizeGetHeight(newWidth);
	}
	return result;
}

void Bubbles::paintEvent(QPaintEvent *e) {
	auto p = Painter(this);
	const auto clip = e->rect();
	const auto theme = _controller->currentChatTheme().get();
	const auto st = _controller->chatStyle().get();
	Window::SectionWidget::PaintBackground(p, theme, size(), clip, true);
	const auto full = rect();
	auto top = 0;
	for (const auto &item : _items) {
		const auto view = item.get();
		const auto bottom = top + view->height();
		if (bottom > clip.y() && top < clip.y() + clip.height()) {
			const auto shifted = full.translated(0, -top);
			auto context = theme->preparePaintContext(
				st,
				shifted,
				shifted,
				clip.translated(0, -top).intersected(
					QRect(0, 0, width(), view->height())),
				true);
			context.outbg = view->hasOutLayout();
			p.translate(0, top);
			view->draw(p, context);
			p.translate(0, -top);
			paintUserpic(p, view, top);
		}
		top = bottom;
	}
}

void Bubbles::paintUserpic(
		QPainter &p,
		not_null<HistoryView::Element*> view,
		int top) {
	const auto from = view->displayFromPhoto()
		? view->data()->displayFrom()
		: nullptr;
	if (!from) {
		return;
	}
	from->paintUserpicLeft(
		p,
		_userpics[from],
		st::historyPhotoLeft,
		top + view->height() - view->marginBottom() - st::msgPhotoSize,
		width(),
		st::msgPhotoSize);
}

} // namespace

void ShowDeletedBubbles(
		not_null<Window::SessionController*> controller,
		not_null<PeerData*> peer,
		std::vector<History::Record> records) {
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		box->setTitle(tr::lng_serein_menu_deleted_messages());
		box->setWidth(st::sereinSnapshotWidth);
		box->addRow(
			object_ptr<Bubbles>(box, controller, peer, records),
			style::margins());
		box->addButton(tr::lng_close(), [=] { box->closeBox(); });
	}));
}

} // namespace Serein::HistoryFeature

#include "serein/snapshot/snapshot.h"
#include "serein/hooks/menu/actions.h"
#include "serein/interface/reply_colors.h"

#include "core/application.h"
#include "serein/core/options.h"
#include "core/file_utilities.h"
#include "data/data_groups.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "history/view/history_view_element.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/chat/chat_style.h"
#include "ui/chat/chat_theme.h"
#include "ui/layers/generic_box.h"
#include "ui/painter.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/labels.h"
#include "ui/widgets/popup_menu.h"
#include "window/section_widget.h"
#include "window/window_session_controller.h"

#include <QtCore/QSaveFile>
#include <QtGui/QClipboard>
#include <QtWidgets/QApplication>

#include "styles/style_chat.h"
#include "styles/style_layers.h"
#include "styles/style_serein.h"
#include "ui/widgets/menu/menu_action.h"
#include "ui/widgets/menu/menu.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"

namespace Serein::Snapshot {
namespace {

constexpr auto kMaximumMessages = 20;
constexpr auto kMaximumImageBytes = 64 * 1024 * 1024;

class SnapshotDelegate final : public HistoryView::SimpleElementDelegate {
public:
	SnapshotDelegate(
		not_null<Window::SessionController*> controller,
		bool reactions,
		bool spoilers)
	: SimpleElementDelegate(controller, [] {})
	, _reactions(reactions)
	, _spoilers(spoilers) {
	}

	HistoryView::Context elementContext() override {
		return HistoryView::Context::History;
	}
	bool elementAnimationsPaused() override { return true; }
	bool elementHideReactions() override { return !_reactions; }
	bool elementHideSenderNames() override { return true; }
	std::optional<bool> elementSpoilersRevealed() override { return _spoilers; }

private:
	bool _reactions = false;
	bool _spoilers = false;

};

bool CanCapture(not_null<HistoryItem*> item) {
	return item->allowsForward() && !item->isTtlCoveredMedia()
		&& !item->isEphemeral() && !item->isMediaSensitive();
}

bool AllAvailable(
		not_null<Window::SessionController*> controller,
		const MessageIdsList &ids) {
	return !ids.empty() && ranges::all_of(ids, [&](FullMsgId id) {
		const auto item = controller->session().data().message(id);
		return item && CanCapture(item);
	});
}

void SnapshotBox(
		not_null<Ui::GenericBox*> box,
		not_null<Window::SessionController*> controller,
		MessageIdsList ids) {
	box->setTitle(tr::lng_serein_snapshot());

	const auto parsed = ReadStored(ForDevice().Get(kSettings));
	if (!parsed) {
		box->addRow(object_ptr<Ui::FlatLabel>(box,
			tr::lng_serein_snapshot_unavailable(), st::boxLabel));
		box->addButton(tr::lng_close(), [=] { box->closeBox(); });
		return;
	}
	const auto options = box->lifetime().make_state<SnapshotConfig>(*parsed);
	const auto reveal = box->lifetime().make_state<bool>(false);
	const auto image = box->lifetime().make_state<QImage>();
	const auto label = box->addRow(object_ptr<Ui::FlatLabel>(
		box, tr::lng_serein_snapshot_about(), st::boxLabel));
	const auto preview = box->addRow(object_ptr<Ui::RpWidget>(box));
	const auto resizePreview = [=] {
		const auto height = image->isNull() ? st::sereinSnapshotPreviewMinimum
			: std::clamp(preview->width() * image->height() / image->width(),
				int(st::sereinSnapshotPreviewMinimum), int(st::sereinSnapshotPreviewHeight));
		preview->resize(preview->width(), height);
	};
	preview->widthValue() | rpl::on_next(resizePreview, preview->lifetime());
	preview->paintRequest() | rpl::on_next([=] {
		auto p = QPainter(preview);
		p.fillRect(preview->rect(), st::windowBg->c);
		if (!image->isNull()) {
			const auto size = image->size().scaled(preview->size(), Qt::KeepAspectRatio);
			p.setRenderHint(QPainter::SmoothPixmapTransform);
			p.drawImage(QRect(QPoint((preview->width() - size.width()) / 2, 0), size), *image);
		}
	}, preview->lifetime());
	const auto render = [=] {
		const auto rendered = Render(controller, ids, *options, *reveal);
		if (const auto error = std::get_if<QString>(&rendered)) {
			*image = QImage();
			label->setText(*error);
		} else {
			*image = std::get<QImage>(rendered);
			label->setText(tr::lng_serein_snapshot_about(tr::now));
		}
		resizePreview();
		preview->update();
	};
	for (const auto &[member, title] : std::array{
		std::pair(&SnapshotConfig::background, tr::lng_serein_snapshot_background(tr::now)),
		std::pair(&SnapshotConfig::date, tr::lng_serein_snapshot_date(tr::now)),
		std::pair(&SnapshotConfig::headers, tr::lng_serein_snapshot_headers(tr::now)),
		std::pair(&SnapshotConfig::reactions, tr::lng_serein_snapshot_reactions(tr::now)),
		std::pair(&SnapshotConfig::simpleReplies, tr::lng_serein_snapshot_simple_replies(tr::now)),
		std::pair(&SnapshotConfig::builtinTheme, tr::lng_serein_snapshot_builtin(tr::now)),
	}) {
		const auto toggle = box->addRow(object_ptr<Ui::Checkbox>(
			box, title, (*options).*member));
		toggle->checkedChanges() | rpl::on_next([=](bool value) {
			(*options).*member = value;
			Expects(ForDevice().Set(kSettings, (*options == Defaults())
				? QByteArray()
				: SerializeSnapshotConfig(*options)));
			render();
		}, toggle->lifetime());
	}
	const auto spoiler = box->addRow(object_ptr<Ui::Checkbox>(
		box, tr::lng_serein_snapshot_spoilers(tr::now), false));
	spoiler->checkedChanges() | rpl::on_next([=](bool value) {
		*reveal = value;
		render();
	}, spoiler->lifetime());
	const auto refresh = box->addRow(object_ptr<Ui::SettingsButton>(
		box, tr::lng_serein_snapshot_refresh(), st::settingsButtonNoIcon));
	refresh->setClickedCallback(render);
	const auto matchesPreview = [=](const QImage &expected) {
		if (expected.isNull() || !AllAvailable(controller, ids)) {
			return false;
		}
		const auto current = Render(controller, ids, *options, *reveal);
		const auto value = std::get_if<QImage>(&current);
		return value && *value == expected;
	};
	box->addButton(tr::lng_serein_snapshot_copy(), [=] {
		if (matchesPreview(*image)) {
			QApplication::clipboard()->setImage(*image);
			box->showToast(tr::lng_serein_snapshot_copied(tr::now));
		} else {
			render();
			box->showToast(tr::lng_serein_snapshot_changed(tr::now));
		}
	});
	box->addButton(tr::lng_serein_snapshot_save(), [=] {
		if (!matchesPreview(*image)) {
			render();
			box->showToast(tr::lng_serein_snapshot_changed(tr::now));
			return;
		}
		const auto snapshot = *image;
		FileDialog::GetWritePath(Core::App().getFileDialogParent(),
			tr::lng_serein_snapshot_save(tr::now), u"PNG (*.png)"_q,
			u"serein-message.png"_q,
			crl::guard(box, [=](QString &&path) {
				if (path.isEmpty()) {
					return;
				}
				if (!matchesPreview(snapshot)) {
					render();
					box->showToast(tr::lng_serein_snapshot_changed(tr::now));
					return;
				}
				auto file = QSaveFile(path);
				if (!file.open(QIODevice::WriteOnly)
					|| !snapshot.save(&file, "PNG") || !file.commit()) {
					box->showToast(tr::lng_serein_snapshot_write_error(tr::now));
				} else {
					box->showToast(tr::lng_serein_snapshot_saved(tr::now));
				}
			}));
	});
	box->addButton(tr::lng_close(), [=] { box->closeBox(); });
	render();
}

} // namespace

std::variant<QImage, QString> Render(
		not_null<Window::SessionController*> controller,
		const MessageIdsList &ids,
		const SnapshotConfig &options,
		bool revealSpoilers) {
	if (!AllAvailable(controller, ids)) {
		return tr::lng_serein_snapshot_unavailable(tr::now);
	} else if (ids.size() > kMaximumMessages) {
		return tr::lng_serein_snapshot_limit(tr::now);
	}
	const auto replies = Interface::ThemeReplyColorsScope(options.simpleReplies);
	auto delegate = SnapshotDelegate(controller,
		options.reactions, revealSpoilers);
	auto palette = style::palette();
	palette.finalize();
	auto snapshotStyle = Ui::ChatStyle(controller->session().colorIndicesValue());
	auto builtinTheme = Ui::ChatTheme();
	builtinTheme.setBackground({ .colorForFill = palette.windowBg()->c });
	const auto builtin = options.builtinTheme;
	snapshotStyle.applyCustomPalette(builtin ? &palette : controller->chatStyle().get());
	const auto chatStyle = &snapshotStyle;
	const auto theme = builtin ? &builtinTheme : controller->currentChatTheme().get();
	const auto padding = st::sereinSnapshotPadding;
	const auto width = st::sereinSnapshotWidth;
	auto height = padding;
	auto views = std::vector<std::unique_ptr<HistoryView::Element>>();
	auto seen = base::flat_set<FullMsgId>();
	const auto date = options.date;
	const auto headers = options.headers;
	for (const auto id : ids) {
		if (seen.contains(id)) {
			continue;
		}
		const auto item = controller->session().data().message(id);
		if (const auto group = controller->session().data().groups().find(item)) {
			for (const auto &part : group->items) {
				if (!ranges::contains(ids, part->fullId())) {
					return tr::lng_serein_snapshot_album(tr::now);
				}
				seen.insert(part->fullId());
			}
			views.push_back(group->items.front()->createView(&delegate));
		} else {
			seen.insert(id);
			views.push_back(item->createView(&delegate));
		}
		const auto &view = views.back();
		height += view->resizeGetHeight(width) + padding;
		height += (headers || date) ? st::sereinSnapshotHeaderHeight : 0;
		if (!revealSpoilers) {
			view->hideSpoilers();
		}
	}
	const auto ratio = style::DevicePixelRatio();
	if (int64(width) * height * ratio * ratio * 4 > kMaximumImageBytes) {
		return tr::lng_serein_snapshot_limit(tr::now);
	}
	auto result = QImage(width * ratio, height * ratio, QImage::Format_ARGB32_Premultiplied);
	if (result.isNull()) {
		return tr::lng_serein_snapshot_limit(tr::now);
	}
	result.setDevicePixelRatio(ratio);
	result.fill(Qt::transparent);
	auto p = Painter(&result);
	if (options.background) {
		Window::SectionWidget::PaintBackground(p, theme,
			QSize(width, height), QRect(0, 0, width, height), true);
	}
	auto top = padding;
	for (const auto &view : views) {
		const auto item = view->data();
		if (headers || date) {
			p.setFont(st::msgNameFont);
			p.setPen(chatStyle->windowFg());
			auto left = padding;
			if (headers) {
				auto userpic = Ui::PeerUserpicView();
				item->from()->paintUserpic(p, userpic, {
					.position = { left, top },
					.size = st::sereinSnapshotAvatarSize,
				});
				left += st::sereinSnapshotAvatarSize + padding;
			}
			const auto name = headers ? item->from()->name() : QString();
			const auto text = name
				+ ((!name.isEmpty() && date) ? u" · "_q : QString())
				+ (date ? view->dateTime().toString(Qt::ISODate) : QString());
			p.drawText(QRect(left, top, width - left - padding, st::sereinSnapshotHeaderHeight),
				Qt::AlignVCenter | Qt::AlignLeft,
				st::msgNameFont->elided(text, width - left - padding));
			top += st::sereinSnapshotHeaderHeight;
		}
		auto context = theme->preparePaintContext(chatStyle,
			QRect(0, -top, width, height), QRect(0, -top, width, height),
			QRect(0, 0, width, view->height()), true);
		context.outbg = view->hasOutLayout();
		p.save();
		p.translate(0, top);
		view->draw(p, context);
		p.restore();
		top += view->height() + padding;
	}
	p.end();
	return result;
}

void InsertAction(
		Ui::PopupMenu *menu,
		Window::SessionController *controller,
		HistoryItem *item,
		MessageIdsList selected) {
	if (!menu || !controller || (!item && selected.empty())) {
		return;
	}
	auto ids = selected.empty()
		? MessageIdsList{ item->fullId() } : std::move(selected);
	if (ids.size() == 1) {
		if (const auto source = controller->session().data().message(ids.front())) {
			if (const auto group = controller->session().data().groups().find(source)) {
				ids.clear();
				for (const auto &part : group->items) {
					ids.push_back(part->fullId());
				}
			}
		}
	}
	if (!AllAvailable(controller, ids)) {
		return;
	}
	const auto action = Ui::Menu::CreateAction(menu,
		tr::lng_serein_snapshot(tr::now),
		crl::guard(controller, [=] {
			controller->show(Box(SnapshotBox, controller, ids));
		}));
	auto widget = base::make_unique_q<Ui::Menu::Action>(
		menu->menu(), menu->menu()->st(), action,
		&st::menuIconSaveImage, &st::menuIconSaveImage);
	auto position = int(menu->actions().size());
	for (auto index = 0; index != position; ++index) {
		const auto tag = menu->actions()[index]->property("sereinMenuActionId");
		if (tag.isValid() && tag.toInt() == int(Menu::ActionId::Delete)) {
			position = index;
			break;
		}
	}
	Menu::Tag(menu->insertAction(position, std::move(widget)),
		Menu::ActionId::Screenshot);
}

} // namespace Serein::Snapshot

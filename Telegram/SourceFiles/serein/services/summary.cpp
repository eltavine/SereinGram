#include "serein/services/summary.h"

#include "serein/hooks/privacy/alias.h"
#include "serein/services/request.h"
#include "serein/services/summary_protocol.h"
#include "boxes/translate_box.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "history/view/history_view_element.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "spellcheck/spellcheck_types.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/labels.h"
#include "window/window_session_controller.h"
#include "styles/style_layers.h"
#include "styles/style_widgets.h"

#include <QtGui/QClipboard>
#include <QtGui/QGuiApplication>

#include <algorithm>

namespace Serein {
namespace {

[[nodiscard]] std::optional<ServiceDefinition> SummaryService() {
	const auto config = Services();
	if (!config) {
		return std::nullopt;
	}
	const auto service = FindService(*config, config->translation);
	return (service && SupportsSummary(*service))
		? service
		: std::nullopt;
}

[[nodiscard]] std::vector<SummaryLine> RecentLines(
		not_null<History*> history) {
	auto result = std::vector<SummaryLine>();
	const auto full = [&] {
		return int(result.size()) >= kSummaryMessagesLimit;
	};
	for (auto i = history->blocks.rbegin(); i != history->blocks.rend(); ++i) {
		const auto &messages = (*i)->messages;
		for (auto j = messages.rbegin(); j != messages.rend(); ++j) {
			if (full()) {
				break;
			}
			const auto item = (*j)->data();
			const auto &text = item->originalText().text;
			if (!item->isService() && !text.trimmed().isEmpty()) {
				result.push_back({ Privacy::DisplayName(item->from()), text });
			}
		}
		if (full()) {
			break;
		}
	}
	std::reverse(result.begin(), result.end());
	return result;
}

} // namespace

bool CanSummarizeChats() {
	return SummaryService().has_value();
}

void ShowChatSummary(
		gsl::not_null<Window::SessionController*> controller,
		gsl::not_null<PeerData*> peer) {
	const auto service = SummaryService();
	const auto history = peer->owner().history(peer);
	const auto lines = RecentLines(history);
	if (!service) {
		return;
	} else if (lines.empty()) {
		controller->uiShow()->showToast(
			tr::lng_serein_summary_empty(tr::now));
		return;
	}
	const auto language = QLocale::languageToString(
		Ui::ChooseTranslateTo(history).language());
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		box->setTitle(tr::lng_serein_summary_title());
		box->addRow(object_ptr<Ui::FlatLabel>(
			box,
			tr::lng_serein_summary_about(
				tr::now,
				lt_name,
				service->name),
			st::boxDividerLabel));
		const auto label = box->addRow(object_ptr<Ui::FlatLabel>(
			box,
			tr::lng_serein_summary_loading(tr::now),
			st::boxLabel));
		label->setSelectable(true);
		const auto request = box->lifetime().make_state<ServiceRequest>();
		const auto result = box->lifetime().make_state<QString>();
		request->json(
			*service,
			BuildSummaryCall(*service, lines, language),
			QUrlQuery(),
			crl::guard(box, [=](ServiceResult response) {
				if (response.error != ServiceError::None) {
					label->setText(
						ServiceErrorText(response.error, response.status));
					return;
				}
				const auto summary = ParseSummaryResponse(
					*service,
					response.body);
				if (!summary) {
					label->setText(ServiceErrorText(ServiceError::Response));
					return;
				}
				*result = *summary;
				label->setText(*summary);
			}));
		box->addButton(tr::lng_context_copy_text(), [=] {
			if (!result->isEmpty()) {
				QGuiApplication::clipboard()->setText(*result);
				box->uiShow()->showToast(tr::lng_text_copied(tr::now));
			}
		});
		box->addButton(tr::lng_close(), [=] { box->closeBox(); });
	}));
}

} // namespace Serein

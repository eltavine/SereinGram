#include "serein/links/clipboard.h"

#include "serein/hooks/links/clipboard.h"
#include "serein/links/model.h"
#include "ui/text/text_entity.h"
#include "ui/text/text_utilities.h"
#include "ui/widgets/fields/input_field.h"

#include <QtCore/QMimeData>
#include <QtWidgets/QTextEdit>

namespace Serein::Links {
namespace {

[[nodiscard]] QString CleanText(const QString &text) {
	if (!text.contains(u"http"_q, Qt::CaseInsensitive)) {
		return text;
	}
	const auto parsed = TextUtilities::ParseEntities(text, TextParseLinks);
	auto result = text;
	for (const auto &entity : ranges::views::reverse(parsed.entities)) {
		if (entity.type() == EntityType::Url) {
			const auto url = text.mid(entity.offset(), entity.length());
			const auto clean = StripTrackingUrl(url);
			if (clean != url) {
				result.replace(entity.offset(), entity.length(), clean);
			}
		}
	}
	return result;
}

} // namespace

QString CleanCopiedLink(const QString &link) {
	return ForDevice().Get(kStripTrackingParameters)
		? StripTrackingUrl(link)
		: link;
}

void InstallCleanPaste(gsl::not_null<Ui::InputField*> field) {
	field->addMimeDataHook([=](not_null<const QMimeData*> data, Ui::InputField::MimeAction action) {
		if (!ForDevice().Get(kStripTrackingParameters)
			|| !data->hasText()
			|| data->hasImage()
			|| data->hasHtml()
			|| data->hasFormat(TextUtilities::TagsMimeType())
			|| ranges::any_of(data->urls(), &QUrl::isLocalFile)) {
			return false;
		}
		const auto text = data->text();
		const auto cleaned = CleanText(text);
		if (cleaned == text) return false;
		if (action == Ui::InputField::MimeAction::Insert) field->rawTextEdit()->insertPlainText(cleaned);
		return true;
	});
}

} // namespace Serein::Links

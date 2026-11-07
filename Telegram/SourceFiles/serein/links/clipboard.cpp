#include "serein/links/clipboard.h"

#include "serein/links/model.h"
#include "core/application.h"
#include "ui/widgets/fields/input_field.h"

#include <QtCore/QMimeData>
#include <QtGui/QClipboard>
#include <QtGui/QGuiApplication>
#include <QtWidgets/QTextEdit>

namespace Serein::Links {

void StartClipboardCleaner() {
	const auto clipboard = QGuiApplication::clipboard();
	QObject::connect(clipboard, &QClipboard::dataChanged, &Core::App(), [=] {
		if (!ForDevice().Get(kStripTrackingParameters) || !clipboard->ownsClipboard()) return;
		const auto data = clipboard->mimeData();
		if (!data || !data->hasText() || data->hasImage()) return;
		const auto text = data->text();
		const auto cleaned = StripTrackingUrl(text);
		if (cleaned != text) clipboard->setText(cleaned);
	});
}

void InstallCleanPaste(gsl::not_null<Ui::InputField*> field) {
	field->addMimeDataHook([=](not_null<const QMimeData*> data, Ui::InputField::MimeAction action) {
		if (!ForDevice().Get(kStripTrackingParameters) || !data->hasText() || data->hasImage()) return false;
		const auto text = data->text();
		const auto cleaned = StripTrackingUrl(text);
		if (cleaned == text) return false;
		if (action == Ui::InputField::MimeAction::Insert) field->rawTextEdit()->insertPlainText(cleaned);
		return true;
	});
}

} // namespace Serein::Links

#pragma once

#include <QtCore/QDir>

#include <gsl/pointers>

class DocumentData;

namespace Data {
struct FileOrigin;
} // namespace Data

namespace Serein::Hooks::Media {

[[nodiscard]] QDir ChatDownloadDirectory(
	gsl::not_null<DocumentData*> document,
	const Data::FileOrigin &origin);

} // namespace Serein::Hooks::Media

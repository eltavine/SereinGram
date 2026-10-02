#pragma once

#include "base/basic_types.h"
#include "spellcheck/spellcheck_types.h"

#include <QtCore/QString>

#include <memory>

namespace Iv {
struct RichPage;
} // namespace Iv

namespace Main {
class Session;
} // namespace Main

namespace Serein::IvTranslation {

struct Result {
	std::shared_ptr<const ::Iv::RichPage> page;
	QString error;
};

void TranslatePage(
	not_null<Main::Session*> session,
	std::shared_ptr<const ::Iv::RichPage> page,
	LanguageId to,
	Fn<void(Result result)> done);

} // namespace Serein::IvTranslation

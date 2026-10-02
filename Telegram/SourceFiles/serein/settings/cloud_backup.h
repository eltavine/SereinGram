#pragma once

#include "base/basic_types.h"

#include <QtCore/QByteArray>
#include <gsl/pointers>

namespace Window {
class SessionController;
} // namespace Window

namespace Serein {

void BackUpToSavedMessages(
	gsl::not_null<Window::SessionController*> controller,
	const QByteArray &data);
void RestoreFromSavedMessages(
	gsl::not_null<Window::SessionController*> controller,
	Fn<void(QByteArray)> done);

} // namespace Serein

#include "serein/hooks/history.h"

#include "serein/features/history/deleted_marks.h"
#include "serein/hooks/gen/messages.h"

#include <QtGui/QPainter>

namespace Serein::Hooks {
namespace {

constexpr auto kDeletedOpacity = 0.6;

} // namespace

FadedPaint::FadedPaint(QPainter &p, gsl::not_null<const HistoryItem*> item)
: _p(p)
, _faded(Messages::FadeDeletedMessages()
	&& HistoryFeature::DeletedInPlace(item)) {
	if (_faded) {
		_opacity = _p.opacity();
		_p.setOpacity(_opacity * kDeletedOpacity);
	}
}

FadedPaint::~FadedPaint() {
	if (_faded) {
		_p.setOpacity(_opacity);
	}
}

} // namespace Serein::Hooks

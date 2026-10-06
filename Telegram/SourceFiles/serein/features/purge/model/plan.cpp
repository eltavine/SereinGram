#include "serein/features/purge/model/plan.h"

namespace Serein::Purge {
namespace {

constexpr auto kDay = qint64(24 * 60 * 60);

} // namespace

qint64 Cutoff(Age age, qint64 now, qint64 custom) {
	switch (age) {
	case Age::All: return 0;
	case Age::Day: return now - kDay;
	case Age::Week: return now - 7 * kDay;
	case Age::Month: return now - 30 * kDay;
	case Age::Year: return now - 365 * kDay;
	case Age::Custom: return (custom > 0) ? custom : 0;
	}
	return 0;
}

} // namespace Serein::Purge

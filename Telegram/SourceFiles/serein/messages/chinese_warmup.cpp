#include "serein/messages/chinese_warmup.h"

#include "core/application.h"
#include "serein/hooks/messages/reading.h"
#include "serein/messages/options.h"

namespace Serein::Messages {
namespace {

class WarmUp final : public QObject {
public:
	explicit WarmUp(QObject *parent) : QObject(parent) {
		ForDevice().Value(
			kReadingChinese
		) | rpl::filter([](int mode) {
			return mode != 0;
		}) | rpl::on_next([](int mode) {
			WarmUpChineseConversion(mode == 2);
		}, _lifetime);
	}

private:
	rpl::lifetime _lifetime;

};

} // namespace

void StartChineseWarmUp() {
	if (ChineseConversionAvailable()) {
		new WarmUp(&Core::App());
	}
}

} // namespace Serein::Messages

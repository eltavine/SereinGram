#include "serein/display/icon_tile.h"

#include "ui/painter.h"
#include "ui/rp_widget.h"
#include "styles/style_serein.h"
#include "styles/style_settings.h"

namespace Serein::Display {

void AddIconTile(
		gsl::not_null<Ui::RpWidget*> row,
		const style::icon &icon,
		const style::color &tile) {
	const auto widget = Ui::CreateChild<Ui::RpWidget>(row.get());
	const auto size = st::sereinSettingsTileSize;
	widget->setAttribute(Qt::WA_TransparentForMouseEvents);
	widget->resize(size, size);
	widget->show();
	row->sizeValue(
	) | rpl::on_next([=](QSize outer) {
		widget->moveToLeft(
			st::sereinSettingsTileLeft,
			(outer.height() - size) / 2,
			outer.width());
	}, widget->lifetime());
	widget->paintRequest(
	) | rpl::on_next([=, &icon, &tile] {
		auto p = QPainter(widget);
		auto hq = PainterHighQualityEnabler(p);
		const auto radius = st::sereinSettingsTileRadius;
		p.setPen(Qt::NoPen);
		p.setBrush(tile->b);
		p.drawRoundedRect(widget->rect(), radius, radius);
		icon.paintInCenter(p, widget->rect(), st::settingsIconFg->c);
	}, widget->lifetime());
}

} // namespace Serein::Display

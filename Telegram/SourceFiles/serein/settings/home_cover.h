#pragma once

#include "base/object_ptr.h"

class QWidget;

namespace Settings::Builder {
class SectionBuilder;
} // namespace Settings::Builder

namespace Ui {
class RpWidget;
} // namespace Ui

namespace Serein {

[[nodiscard]] object_ptr<Ui::RpWidget> CreateAppCover(QWidget *parent);
void AddHomeCover(::Settings::Builder::SectionBuilder &builder);

} // namespace Serein

#include "base/assertion.h"
#include "base/basic_types.h"
#include "ui/text/text_entity.h"

// Mirrors lib_ui, which the Qt-only core tests do not link.
EntityInText::EntityInText(
	EntityType type,
	int offset,
	int length,
	const QString &data)
: _type(type)
, _offset(offset)
, _length(length)
, _data(data) {
}

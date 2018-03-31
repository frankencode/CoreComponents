/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Column>

namespace cc {
namespace ui {

Ref<Column> Column::create(View *parent)
{
    return Object::create<Column>(parent);
}

Column::Column(View *parent):
    View(parent),
    ColumnLayout(this)
{}

}} // namespace cc::ui

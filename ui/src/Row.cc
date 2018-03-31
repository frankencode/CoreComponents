/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Row>

namespace cc {
namespace ui {

Ref<Row> Row::create(View *parent)
{
    return Object::create<Row>(parent);
}

Row::Row(View *parent):
    View(parent),
    RowLayout(this)
{}

}} // namespace cc::ui

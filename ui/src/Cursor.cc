/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Cursor>
#include <cc/ui/Application>

namespace cc {
namespace ui {

Ref<Cursor> Cursor::create(CursorShape shape)
{
    return Application::instance()->createCursor(shape);
}

}} // namespace cc::ui

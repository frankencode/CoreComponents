/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Cursor>
#include <cc/ui/Application>

namespace cc::ui {

Cursor::Cursor(CursorShape shape)
{
    *this = Application{}.me().createCursor(shape);
}

} // namespace cc::ui

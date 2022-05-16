/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Cursor>
#include <cc/Application>

namespace cc {

Cursor::Cursor(CursorShape shape)
{
    *this = Application{}.me().createCursor(shape);
}

} // namespace cc

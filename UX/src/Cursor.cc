/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Cursor>
#include <cc/Application>

namespace cc {

Cursor::Cursor(CursorShape shape)
{
    *this = Application{}.me().createCursor(shape);
}

Cursor::Cursor(const Image &image, const Point &hotspot)
{
    *this = Application{}.me().createCursor(image, hotspot);
}

} // namespace cc

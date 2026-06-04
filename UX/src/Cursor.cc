/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
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

/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "FontInfo.h"

namespace cc {
namespace ui {

bool FontInfo::fixedWidth() const
{
    bool value = true;
    for (int i = 0; i < faceCount(); ++i) {
        FontFace *face = faceAt(i);
        if (!face->fixedWidth()) {
            value = false;
            break;
        }
    }
    return value;
}

}} // namespace cc::ui

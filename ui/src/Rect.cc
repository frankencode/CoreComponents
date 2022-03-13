/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Rect>

namespace cc {

String str(const cc::ui::Rect &rect)
{
    return Format{}
        << "Rect {\n"
        << "  pos: " << rect.pos() << nl
        << "  size: " << rect.size() << nl
        << "}";
}

String fixed(const cc::ui::Rect &rect, int ni, int nf)
{
    return Format{}
        << "Rect {\n"
        << "  pos: " << fixed(rect.pos(), ni, nf) << nl
        << "  size: " << fixed(rect.size(), ni, nf) << nl
        << "}";
}

String fixed(const cc::ui::Rect &rect, int nf)
{
    return fixed(rect, 0, nf);
}

} // namespace cc

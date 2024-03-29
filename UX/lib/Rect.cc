/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Rect>

namespace cc {

String str(const cc::Rect &rect)
{
    return Format{}
        << "Rect {\n"
        << "  pos: " << rect.pos() << nl
        << "  size: " << rect.size() << nl
        << "}";
}

String fixed(const cc::Rect &rect, int ni, int nf)
{
    return Format{}
        << "Rect {\n"
        << "  pos: " << fixed(rect.pos(), ni, nf) << nl
        << "  size: " << fixed(rect.size(), ni, nf) << nl
        << "}";
}

String fixed(const cc::Rect &rect, int nf)
{
    return fixed(rect, 0, nf);
}

} // namespace cc

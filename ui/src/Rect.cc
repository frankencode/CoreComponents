/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/format>
#include <cc/ui/Rect>

namespace cc {
namespace ui {

string str(const Rect &rect)
{
    return format{}
        << "Rect {\n"
        << "  pos: " << rect->pos() << nl
        << "  size: " << rect->size() << nl
        << "}";
}

string fixed(const Rect &rect, int ni, int nf)
{
    return format{}
        << "Rect {\n"
        << "  pos: " << fixed(rect->pos(), ni, nf) << nl
        << "  size: " << fixed(rect->size(), ni, nf) << nl
        << "}";
}

string fixed(const Rect &rect, int nf)
{
    return fixed(rect, 0, nf);
}

}} // namespace cc::ui

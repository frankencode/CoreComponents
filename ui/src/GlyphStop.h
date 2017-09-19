/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/types>
#include <cc/List>

namespace cc {
namespace ui {

class GlyphStop: public Object
{
public:
    inline static Ref<GlyphStop> create(uchar_t ch, int offset, double penX, double penY, double advanceX, double advanceY) {
        return new GlyphStop(ch, offset, penX, penY, advanceX, advanceY);
    }

    uchar_t ch;
    int offset;
    double penX, penY;
    double advanceX, advanceY;

protected:
    GlyphStop(uchar_t ch, int offset, double penX, double penY, double advanceX, double advanceY):
        ch(ch),
        offset(offset),
        penX(penX),
        penY(penY),
        advanceX(advanceX),
        advanceY(advanceY)
    {}
};

typedef List< Ref<GlyphStop> > GlyphRun;

}} // namespace cc::ui


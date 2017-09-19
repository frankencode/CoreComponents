/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_RENDER_H

#include <cc/Color>
#include <cc/ui/TypeSetter>

namespace cc {
namespace ui {

class FtFontMetrics;
class FtFontProvider;

class FtTypeSetter: public TypeSetter
{
public:
    inline static Ref<FtTypeSetter> create() { return new FtTypeSetter(); }

protected:
    void stage(TextBlock *block, double *penX, double *penY, Image *image = 0, GlyphRun *run = 0) const;

private:
    friend class FtFontProvider;

    FtTypeSetter() {}
    ~FtTypeSetter();

    struct RasterArgs {
        Image *image;
        Color color;
        int penY;
    };
    static void drawSpans(int y, int spanCount, const FT_Span *spans, void *user);

    bool caching_;
};

}} // namespace cc::ui


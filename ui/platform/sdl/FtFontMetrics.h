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
#include FT_SIZES_H

#include <cc/ui/FontMetrics>
#include <cc/ui/sdl/FtFontFace>

namespace cc {
namespace ui {

class FtFontMetrics: public FontMetrics
{
public:
    inline static Ref<FtFontMetrics> create(FtFontFace *face, double fontSize) {
        return new FtFontMetrics(face, fontSize);
    }

    double fontSize() const { return fontSize_; }
    double ascender() const { return size_->metrics.ascender / 64.; }
    double descender() const { return size_->metrics.descender / 64.; }
    double lineHeight() const { return size_->metrics.height / 64.; }
    double capitalHeight() const;

private:
    FtFontMetrics(FtFontFace *face, double size);
    ~FtFontMetrics();

    mutable Ref<FtFontFace> face_; // FIXME: mutable?
    double fontSize_;
    FT_Size size_;
};

}} // namespace cc::ui

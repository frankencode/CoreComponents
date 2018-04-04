/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ThreadLocalSingleton>
#include <cc/ui/FtScaledFont>
#include <cc/ui/FtFontManager>

namespace cc {
namespace ui {

FtFontManager *FtFontManager::instance()
{
    return ThreadLocalSingleton<FtFontManager>::instance();
}

Ref<FontFace> FtFontManager::openFontFace(String path)
{
    return FtFontFace::open(path);
}

Ref<ScaledFont> FtFontManager::selectFont(
    String family,
    double size,
    Weight weight,
    Slant slant,
    Stretch stretch
) const
{
    const FtFontFace *fontFace = Object::cast<const FtFontFace *>(selectFontFamily(family)->selectFontFace(weight, slant, stretch));
    return Object::create<FtScaledFont>(fontFace, size);
}

}} // namespace cc::ui

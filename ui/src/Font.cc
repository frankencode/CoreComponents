/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/StyleManager>
#include <cc/ui/FontManager>
#include <cc/ui/Font>

namespace cc {
namespace ui {

const ScaledFont *Font::Instance::getScaledFont()
{
    if (scaledFont_) {
        if (
            (family()->count() == 0 || scaledFont_->family()  == family()) &&
            scaledFont_->size()    == size()    &&
            scaledFont_->weight()  == weight()  &&
            scaledFont_->slant()   == slant()   &&
            scaledFont_->stretch() == stretch()
        )
            return scaledFont_;
    }

    scaledFont_ =
        FontManager::instance()->selectFont(
            (family()) ? family() : StyleManager::instance()->activePlugin()->defaultFontFamily(),
            (size() > 0) ? size() : StyleManager::instance()->activePlugin()->defaultFontSize(),
            weight(),
            slant(),
            stretch()
        );

    return scaledFont_;
}

bool Font::differ(const Font &a, const Font &b)
{
    return
        a->family()     != b->family()     ||
        a->size()       != b->size()       ||
        a->weight()     != b->weight()     ||
        a->slant()      != b->slant()      ||
        a->stretch()    != b->stretch()    ||
        a->decoration() != b->decoration() ||
        a->color()      != b->color();
}

}} // namespace cc::ui

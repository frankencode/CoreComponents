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

const ScaledFont *Font::Instance::getScaledFont() const
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
            (family()) ? family() : style()->defaultFont()->family(),
            (size() > 0) ? size() : style()->defaultFont()->size(),
            weight(),
            slant(),
            stretch()
        );

    return scaledFont_;
}

const FontMetrics *Font::Instance::getMetrics() const
{
    return getScaledFont()->getMetrics();
}

StylePlugin *Font::style()
{
    return StyleManager::instance()->activePlugin();
}

Font &Font::operator<<(Pitch pitch)
{
    (*this)->setFamily(
        (pitch == Pitch::Fixed) ?
        style()->defaultFixedFont()->family() :
        style()->defaultFont()->family()
    );
    if ((*this)->size() <= 0 && pitch == Pitch::Fixed)
        (*this)->setSize(style()->defaultFixedFont()->size());
    return *this;
}

Font &Font::operator*=(double scale)
{
    double size = (*this)->size();
    if (size <= 0) size = style()->defaultFixedFont()->size() * scale;
    else size = size * scale;
    (*this)->setSize(size);
    return *this;
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

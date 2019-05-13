/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/List>
#include <cc/ui/FontFamily>

namespace cc {
namespace ui {

Ref< Source<const FontFace *> > FontFamily::getFontFaces() const
{
    return fontFaces_->getAll<const FontFace *>();
}

FontFamily::FontFamily(const String &name, Pitch pitch):
    name_{name},
    pitch_{pitch},
    fontFaces_{FontFaces::create()}
{}

FontFamily::~FontFamily()
{}

const FontFace *FontFamily::selectFontFace(Weight weight, Slant slant, Stretch stretch) const
{
    const FontFace *nearChoice = 0;
    const FontFace *farChoice = 0;
    const FontFace *fallbackChoice = 0;

    for (const FontFace *candidate: fontFaces_)
    {
        if (!fallbackChoice)
            fallbackChoice = candidate;

        if (candidate->slant() == slant) {
            if (candidate->weight() == weight) {
                nearChoice = candidate;
                if (candidate->stretch() == stretch)
                    return candidate;
            }
            else if (
                candidate->weight() == Weight::Normal &&
                candidate->stretch() == Stretch::Normal
            )
                farChoice = candidate;
        }

        if (
            candidate->slant() == Slant::Normal &&
            candidate->weight() == Weight::Normal &&
            candidate->stretch() == Stretch::Normal
        )
            fallbackChoice = candidate;
    }

    if (nearChoice) return nearChoice;
    if (farChoice) return farChoice;
    return fallbackChoice;
}

}} // namespace cc::ui

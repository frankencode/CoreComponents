/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/FontFamily>

namespace cc {

FontFace FontFamily::selectFontFace(Weight weight, Slant slant, Stretch stretch) const
{
    FontFace nearChoice;
    FontFace farChoice;
    FontFace fallbackChoice;

    for (const FontFace &candidate: me().fontFaces_)
    {
        if (!fallbackChoice)
            fallbackChoice = candidate;

        if (candidate.slant() == slant) {
            if (candidate.weight() == weight) {
                nearChoice = candidate;
                if (candidate.stretch() == stretch)
                    return candidate;
            }
            else if (
                candidate.weight() == Weight::Normal &&
                candidate.stretch() == Stretch::Normal
            )
                farChoice = candidate;
        }

        if (
            candidate.slant() == Slant::Normal &&
            candidate.weight() == Weight::Normal &&
            candidate.stretch() == Stretch::Normal
        )
            fallbackChoice = candidate;
    }

    if (nearChoice) return nearChoice;
    if (farChoice) return farChoice;
    return fallbackChoice;
}

} // namespace cc

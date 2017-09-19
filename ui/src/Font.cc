/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Application>
#include <cc/ui/FontFace>
#include <cc/ui/Font>

namespace cc {
namespace ui {

Ref<Font> Font::select(String family)
{
    return Application::instance()->fontProvider()->selectFont(family);
}

Font::Font(int id, String family):
    id_(id),
    family_(family),
    size_(10),
    italic_(false),
    stretch_(Font::Unstretched),
    weight_(Font::Medium),
    color_(Color::black()),
    hinting_(false)
{}

Font::Font(const Font *other):
    id_(other->id_),
    family_(other->family_),
    size_(other->size_),
    italic_(other->italic_),
    stretch_(other->stretch_),
    weight_(other->weight_),
    color_(other->color_),
    hinting_(other->hinting_)
{}

bool Font::equals(const Font *other) const
{
    if (!other) return false;

    return
        id_      == other->id_      &&
        size_    == other->size_    &&
        italic_  == other->italic_  &&
        stretch_ == other->stretch_ &&
        weight_  == other->weight_  &&
        color_   == other->color_   &&
        hinting_ == other->hinting_;
}

String Font::stretchName(Stretch stretch)
{
    String s;
    switch (stretch) {
        case UltraCondensed: { s = "Ultra Condensed"; break; }
        case ExtraCondensed: { s = "Extra Condensed"; break; }
        case Condensed:      { s = "Condensed"; break; }
        case SemiCondensed:  { s = "Semi Condensed"; break; }
        case Unstretched:    { s = "Unstretched"; break; }
        case SemiExpanded:   { s = "Semi Expanded"; break; }
        case Expanded:       { s = "Expaned"; break; }
        case ExtraExpanded:  { s = "Extra Expanded"; break; }
        case UltraExpanded:  { s = "Ultra Expanded"; break; }
    };
    return s;
}

String Font::weightName(Weight weight)
{
    String s;
    switch (weight) {
        case Thin      : { s = "Thin"; break; }
        case ExtraLight: { s = "Extra Light"; break; }
        case Light     : { s = "Light"; break; }
        case Normal    : { s = "Normal"; break; }
        case Medium    : { s = "Medium"; break; }
        case SemiBold  : { s = "Semi Bold"; break; }
        case Bold      : { s = "Bold"; break; }
        case ExtraBold : { s = "Extra Bold"; break; }
        case Black     : { s = "Black"; break; }
    };
    return s;
}

}} // namespace cc::ui

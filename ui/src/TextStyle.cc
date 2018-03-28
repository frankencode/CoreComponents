/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>
#include <cc/ui/TextStyle>

namespace cc {
namespace ui {

Ref<TextStyle> TextStyle::create(const Font *font, Color color)
{
    return new TextStyle(font, color, TextDecoration::None);
}

Ref<TextStyle> TextStyle::create(const Font *font, TextDecoration decoration, Color color)
{
    return new TextStyle(font, color, decoration);
}

Ref<TextStyle> TextStyle::copy() const
{
    return new TextStyle(font_, color_, decoration_);
}

String TextStyle::toString() const
{
    return Format()
        << "TextStyle {" << nl
        << "  font: " << font()->toString()->replace("\n", "\n  ") << nl
        << "  color: " << color() << nl
        << "  decoration: " << decoration() << nl
        << "}";
}

}} // namespace cc::ui

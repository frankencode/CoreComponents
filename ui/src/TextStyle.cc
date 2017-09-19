/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/TextStyle>

namespace cc {
namespace ui {

TextStyle::TextStyle():
    font_(Font::select()),
    boundary_(0),
    bounding_(Wrap),
    wrapBehind_(wrapBehindDefault)
{}

TextStyle::TextStyle(const TextStyle *other):
    font_(other->font_->copy()),
    boundary_(other->boundary_),
    bounding_(other->bounding_),
    wrapBehind_(other->wrapBehind_)
{}

bool TextStyle::wrapBehindDefault(String text, int i)
{
    uint8_t ch = text->byteAt(i);
    uint8_t ch2 = text->has(i + 1) ? text->byteAt(i + 1) : 0;
    return (
        (ch <= 0x2F) ||
        (0x3A <= ch && ch <= 0x3F) ||
        (0x5B <= ch && ch <= 0x60) ||
        (0x7B <= ch && ch <= 0x7E)
    ) && (ch2 < 0x20 || 0x2F < ch2);
}

}} // namespace cc::ui

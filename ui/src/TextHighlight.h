/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/ui/Font>

namespace cc {
namespace ui {

class TextHighlight: public Object
{
public:
    static Ref<TextHighlight> create(int offset0, int offset1, Font *font) {
        return new TextHighlight(offset0, offset1, font);
    }

    int offset0() const { return offset0_; }
    int offset1() const { return offset1_; }
    Font *font() const { return font_; }

private:
    int offset0_, offset1_;
    Ref<Font> font_;
};

}} // namespace cc::ui


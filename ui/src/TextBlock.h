/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/ui/TextStyle>
#include <cc/ui/GlyphStop>

namespace cc {
namespace ui {

class TextBlock: public Object
{
public:
    inline static Ref<TextBlock> create(String text, TextStyle *style = 0) {
        return new TextBlock(text, style);
    }

    inline Ref<TextBlock> copy() {
        return new TextBlock(text_->copy(), style_->copy());
    }

    inline String text() const { return text_; }
    inline void setText(String text) { text_ = text; }

    inline TextStyle *style() const { return style_; }
    inline void setStyle(TextStyle *style) { style_ = style; }

    inline bool interactive() const { return interactive_; }
    inline void setInteractive(bool on) { interactive_ = on; }

protected:
    friend class TypeSetter;

    TextBlock(String text, TextStyle *style);

    String text_;
    Ref<TextStyle> style_;
    Ref<GlyphRun> run_;

    bool interactive_;
};

}} // namespace cc::ui


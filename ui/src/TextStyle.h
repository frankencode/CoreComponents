/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Color>
#include <cc/ui/Font>

namespace cc {
namespace ui {

class TextStyle: public Object
{
public:
    inline static Ref<TextStyle> create() { return new TextStyle; }
    inline Ref<TextStyle> copy() const { return new TextStyle(this); }

    inline Font *font() const { return font_; }
    virtual void setFont(Font *newFont) { font_ = newFont; }

    inline double boundary() const { return boundary_; }
    inline void setBoundary(double newBoundary) { boundary_ = newBoundary; }

    enum Bounding { ElideFront, ElideBack, Wrap, Justify };

    inline Bounding bounding() const { return bounding_; }
    inline void setBounding(Bounding newBounding) { bounding_ = newBounding; }

    typedef bool (* WrapBehind)(String text, int i);
    static bool wrapBehindDefault(String text, int i);

    inline WrapBehind wrapBehind() const { return wrapBehind_; }
    inline void setWrapBehind(WrapBehind wrapBehind) { wrapBehind_ = wrapBehind; }

private:
    TextStyle();
    TextStyle(const TextStyle *other);

    Ref<Font> font_;
    double boundary_;
    Bounding bounding_;
    WrapBehind wrapBehind_;
};

}} // namespace cc::ui


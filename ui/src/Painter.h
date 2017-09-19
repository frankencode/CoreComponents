/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Color>
#include <cc/ui/Rect>
#include <cc/ui/TextBlock>

namespace cc {
namespace ui {

class View;
class Frame;

class Painter: public Object
{
public:
    inline static Ref<Painter> create(View *view) { return new Painter(view); }

    void fillRect(int x, int y, int w, int h, Color c);
    void fillRect(Rect *rect, Color color);

    inline void setFont(Font *font) { textStyle_->setFont(font); }
    inline void setTextStyle(TextStyle *style) { textStyle_ = style; }

    void drawText(String text, Placement place = PlaceAuto, Rect *rect = 0);
    void drawText(TextBlock *block, Placement place = PlaceAuto, Rect *rect = 0);

private:
    Painter(View *view);
    ~Painter();

    Ref<View> view_;
    Ref<Frame> frame_;
    Ref<TextStyle> textStyle_;
};

}} // namespace cc::ui


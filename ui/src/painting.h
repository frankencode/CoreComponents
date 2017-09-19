/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/ui/PaintCommand>
#include <cc/ui/View>
#include <cc/ui/TextStyle>
#include <cc/ui/TextBlock>

namespace cc {
namespace ui {

class FillRectCommand: public PaintCommand
{
public:
    inline static Ref<FillRectCommand> create(Rect *rect, Color color) {
        return new FillRectCommand(rect, color);
    }

    inline Rect *rect() const { return rect_; }
    inline Color color() const { return color_; }

private:
    FillRectCommand(Rect *rect, Color color):
        PaintCommand(FillRect),
        rect_(rect),
        color_(color)
    {}

    Ref<Rect> rect_;
    Color color_;
};

class DrawTextCommand: public PaintCommand
{
public:
    inline static Ref<DrawTextCommand> create(TextBlock *block, Placement place, Rect *rect) {
        return new DrawTextCommand(block, place, rect);
    }

    inline TextBlock *block() const { return block_; }
    inline Placement place() const { return place_; }
    inline Rect *rect() const { return rect_; }

private:
    DrawTextCommand(TextBlock *block, Placement place, Rect *rect):
        PaintCommand(DrawText),
        block_(block),
        place_(place),
        rect_(rect)
    {}

    Ref<TextBlock> block_;
    Placement place_;
    Ref<Rect> rect_;
};

}} // namespace cc::ui


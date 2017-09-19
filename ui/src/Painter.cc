/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Application>
#include <cc/ui/RenderQueue>
#include <cc/ui/Window>
#include <cc/ui/View>
#include <cc/ui/Frame>
#include <cc/ui/painting>
#include <cc/ui/Painter>

namespace cc {
namespace ui {

Painter::Painter(View *view):
    view_(view),
    frame_(Frame::create(view)),
    textStyle_(TextStyle::create())
{}

Painter::~Painter()
{
    Application::instance()->renderQueue()->pushFrame(frame_);
}

void Painter::fillRect(int x, int y, int w, int h, Color c)
{
    frame_->pushPaintCommand(FillRectCommand::create(Rect::create(x, y, w, h), c));
}

void Painter::fillRect(Rect *rect, Color color)
{
    frame_->pushPaintCommand(FillRectCommand::create(rect->copy(), color));
}

void Painter::drawText(String text, Placement place, Rect *rect)
{
    frame_->pushPaintCommand(
        DrawTextCommand::create(
            TextBlock::create(text->copy(), textStyle_->copy()),
            place,
            rect ? rect->copy() : Rect::create(0, 0, view_->w(), view_->h())
        )
    );
}

void Painter::drawText(TextBlock *block, Placement place, Rect *rect)
{
    frame_->pushPaintCommand(
        DrawTextCommand::create(
            block->copy(),
            place,
            rect ? rect->copy() : Rect::create(0, 0, view_->w(), view_->h())
        )
    );
}

}} // namespace cc::ui

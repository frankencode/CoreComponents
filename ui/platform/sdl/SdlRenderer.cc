/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio> // DEBUG
#include <cc/Guard>
#include <cc/ui/Image>
#include <cc/ui/sdl/SdlApplication>
#include <cc/ui/sdl/SdlWindow>
#include <cc/ui/sdl/SdlView>
#include <cc/ui/sdl/FtTypeSetter>
#include <cc/ui/sdl/SdlRenderer>

namespace cc {
namespace ui {

SdlRenderer::SdlRenderer(SdlApplication *app):
    app_(app)
{
    Thread::start();
}

SdlRenderer::~SdlRenderer()
{
    app_->renderQueue()->shutdown();
    Thread::wait();
}

void SdlRenderer::fillRect(FillRectCommand *cmd, Image *image)
{
    fout("fillRect: rect = %%\n") << cmd->rect() << nl;
    image->fillRect(cmd->rect(), cmd->color());
}

void SdlRenderer::drawText(DrawTextCommand *cmd, Image *image)
{
    TextBlock *block = cmd->block();
    String text = block->text();
    int place = cmd->place();
    Rect *rect = cmd->rect();

    // fout("drawText: text->text() = \"%%\"\n") << text->text();
    // fout("  typeSetter->font()->size() = %%\n") << typeSetter->font()->size();
    // double textWidth = typeSetter_->textWidth(text->text());
    // double textHeight = typeSetter_->textHeight(text->text());
    /*fout("  textWidth, textHeight = %%, %%\n") << textWidth << textHeight;
    fout("  rect() = %%\n") << rect;
    fout("  typeSetter->ascender() = %%\n") << typeSetter->ascender();
    fout("  typeSetter->descender() = %%\n") << typeSetter->descender();
    fout("  typeSetter->lineHeight() = %%\n") << typeSetter->lineHeight();
    fout("  typeSetter->capitalHeight() = %%\n") << typeSetter->capitalHeight();*/
    double textWidth = 0, textHeight = 0;
    typeSetter_->pace(block, &textWidth, &textHeight);

    double x = rect->x(), y = rect->y();
    if (place == PlaceAuto) place = PlaceLeft|PlaceVCenter;
    if (place & PlaceHCenter)     x = rect->x() + rect->w() / 2. - textWidth / 2;
    else if (place & PlaceRight)  x = rect->x() + rect->w() - textWidth - 1;
    if (place & PlaceVCenter)     y = rect->y() + rect->h() / 2. + textHeight / 2;
    else if (place & PlaceTop)    y = rect->y() + ::round(textHeight);
    else if (place & PlaceBottom) y = rect->y() + rect->h() - 1;
    // fout("  x, y = %%, %%\n") << x << y;
    typeSetter_->layout(block, &x, &y, image);
}

void SdlRenderer::run()
{
    typeSetter_ = FtTypeSetter::create();
    typeSetter_->setCaching(true);

    for (Ref<Frame> frame; app_->renderQueue()->popFrame(&frame);)
    {
        Ref<SdlView> view = Object::cast<SdlView *>(frame->view());

        Guard<SdlView> guard(view);
        Image *image = view->image_;
        image->clear(view->color());

        for (Ref<PaintCommand> cmd; frame->popPaintCommand(&cmd);)
        {
            if (cmd->type() == PaintCommand::FillRect) {
                fillRect(Object::cast<FillRectCommand *>(cmd), image);
                fout() << "fill rect" << nl;
            }
            else if (cmd->type() == PaintCommand::DrawText) {
                drawText(Object::cast<DrawTextCommand *>(cmd), image);
            }
        }

        app_->pushUpdateEvent(Object::cast<SdlWindow *>(view->window()));
    }
}

}} // namespace cc::ui

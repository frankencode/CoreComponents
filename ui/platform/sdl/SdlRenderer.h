/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Thread>
#include <cc/ui/painting>
#include <cc/ui/RenderQueue>

namespace cc {
namespace ui {

class SdlApplication;
class FtFont;

class SdlRenderer: public Thread
{
public:
    inline static Ref<SdlRenderer> start(SdlApplication *app) {
        return new SdlRenderer(app);
    }

private:
    SdlRenderer(SdlApplication *app);
    ~SdlRenderer();

    void fillRect(FillRectCommand *cmd, Image *image);
    void drawText(DrawTextCommand *cmd, Image *image);
    virtual void run();

    Ref<SdlApplication> app_;
    Ref<FtTypeSetter> typeSetter_;
};

}} // namespace cc::ui


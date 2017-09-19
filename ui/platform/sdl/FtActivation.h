/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/ui/sdl/FtFontFace>
#include <cc/ui/sdl/FtLibrary>

namespace cc {
namespace ui {

class FtFont;
class FtFontFace;

class FtActivation: public FtGuard
{
public:
    FtActivation(Font *font, FtFontFace **face);
};

}} // namespace cc::ui


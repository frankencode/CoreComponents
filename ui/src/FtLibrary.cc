/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/FtLibrary>
#include <cc/DEBUG>
#include <freetype/ftlcdfil.h>
#include <cassert>

namespace cc::ui {

FtLibrary::FtLibrary():
    ThreadLocalSingleton{instance<State>()}
{}

FtLibrary::State::State()
{
    #ifndef NDEBUG
    FT_Error error =
    #endif
    FT_Init_FreeType(&library_);
    assert(error == 0);

    // error = FT_Library_SetLcdFilter(library_, FT_LCD_FILTER_DEFAULT);
    // if (error != 0) CC_DEBUG << "Failed to enable default LCD fitler";
}

FtLibrary::State::~State()
{
    FT_Done_FreeType(library_);
}

} // namespace cc::ui

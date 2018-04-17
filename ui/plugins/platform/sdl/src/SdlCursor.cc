/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/SdlCursor>

namespace cc {
namespace ui {

Ref<SdlCursor> SdlCursor::create(CursorShape shape)
{
    return new SdlCursor(shape);
}

SdlCursor::SdlCursor(CursorShape shape):
    sdlCursor_(SDL_CreateSystemCursor(
        [=]() {
            switch (shape) {
                case CursorShape::Arrow                   : return SDL_SYSTEM_CURSOR_ARROW;
                case CursorShape::IBeam                   : return SDL_SYSTEM_CURSOR_IBEAM;
                case CursorShape::Wait                    : return SDL_SYSTEM_CURSOR_WAIT;
                case CursorShape::CrossHair               : return SDL_SYSTEM_CURSOR_CROSSHAIR;
                case CursorShape::WaitArrow               : return SDL_SYSTEM_CURSOR_WAITARROW;
                case CursorShape::ResizeNorthWestSouthEast: return SDL_SYSTEM_CURSOR_SIZENWSE;
                case CursorShape::ResizeNorthEastSouthWest: return SDL_SYSTEM_CURSOR_SIZENESW;
                case CursorShape::ResizeWestEast          : return SDL_SYSTEM_CURSOR_SIZEWE;
                case CursorShape::ResizeNorthSouth        : return SDL_SYSTEM_CURSOR_SIZENS;
                case CursorShape::ResizeCross             : return SDL_SYSTEM_CURSOR_SIZEALL;
                case CursorShape::Forbidden               : return SDL_SYSTEM_CURSOR_NO;
                case CursorShape::Hand                    : return SDL_SYSTEM_CURSOR_HAND;
            }
            return SDL_SYSTEM_CURSOR_ARROW;
        }()
    ))
{}

SdlCursor::~SdlCursor()
{
    SDL_FreeCursor(sdlCursor_);
}

}} // namespace cc::ui

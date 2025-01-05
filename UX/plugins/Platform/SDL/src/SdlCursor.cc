/*
 * Copyright (C) 2025 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/SdlCursor>

namespace cc {

struct SdlCursor::State final: public Cursor::State
{
    State(CursorShape shape):
        Cursor::State{shape},
        sdlCursor_{SDL_CreateSystemCursor(
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
                    case CursorShape::Custom                  : break;
                }
                return SDL_SYSTEM_CURSOR_ARROW;
            }()
        )}
    {}

    State(const Image &image, const Point &hotspot):
        Cursor::State{CursorShape::Arrow}
    {
        // TODO...
    }

    SDL_Cursor *sdlCursor_;
};

SdlCursor::SdlCursor(CursorShape shape):
    Cursor{new State{shape}}
{}

SdlCursor::SdlCursor(const Image &image, const Point &hotspot):
    Cursor{new State{image, hotspot}}
{}

SdlCursor::SdlCursor(const Cursor &cursor)
{
    static_cast<Cursor &>(*this) = cursor;
}

SDL_Cursor *SdlCursor::sdlCursor() const
{
    return me().sdlCursor_;
}

const SdlCursor::State &SdlCursor::me() const
{
    return Object::me.as<State>();
}

} // namespace cc

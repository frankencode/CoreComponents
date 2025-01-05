/*
 * Copyright (C) 2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Sdl3Cursor>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_surface.h>

namespace cc {

struct Sdl3Cursor::State final: public Cursor::State
{
    State(CursorShape shape):
        Cursor::State{shape},
        cursor_{SDL_CreateSystemCursor(
            [=]() {
                switch (shape) {
                    case CursorShape::Arrow                   : return SDL_SYSTEM_CURSOR_DEFAULT;
                    case CursorShape::IBeam                   : return SDL_SYSTEM_CURSOR_TEXT;
                    case CursorShape::Wait                    : return SDL_SYSTEM_CURSOR_WAIT;
                    case CursorShape::CrossHair               : return SDL_SYSTEM_CURSOR_CROSSHAIR;
                    case CursorShape::WaitArrow               : return SDL_SYSTEM_CURSOR_PROGRESS;
                    case CursorShape::ResizeNorthWestSouthEast: return SDL_SYSTEM_CURSOR_NWSE_RESIZE;
                    case CursorShape::ResizeNorthEastSouthWest: return SDL_SYSTEM_CURSOR_NESW_RESIZE;
                    case CursorShape::ResizeWestEast          : return SDL_SYSTEM_CURSOR_EW_RESIZE;
                    case CursorShape::ResizeNorthSouth        : return SDL_SYSTEM_CURSOR_NS_RESIZE;
                    case CursorShape::ResizeCross             : return SDL_SYSTEM_CURSOR_MOVE;
                    case CursorShape::Forbidden               : return SDL_SYSTEM_CURSOR_NOT_ALLOWED;
                    case CursorShape::Hand                    : return SDL_SYSTEM_CURSOR_POINTER;
                    case CursorShape::Custom                  : break;
                }
                return SDL_SYSTEM_CURSOR_DEFAULT;
            }()
        )}
    {}

    State(const Image &image, const Point &hotspot):
        Cursor::State{CursorShape::Custom},
        image_{image},
        surface_{
            SDL_CreateSurfaceFrom(
                image_.width(),
                image_.height(),
                SDL_PIXELFORMAT_ARGB8888,
                image_.data().bytes(),
                image_.pitch()
            )
        },
        cursor_{
            SDL_CreateColorCursor(
                surface_,
                static_cast<int>(hotspot.x()),
                static_cast<int>(hotspot.y())
            )
        }
    {}

    ~State()
    {
        if (cursor_) SDL_DestroyCursor(cursor_);
        if (surface_) SDL_DestroySurface(surface_);
    }

    Image image_;
    SDL_Surface *surface_ { nullptr };
    SDL_Cursor *cursor_ { nullptr };
};

Sdl3Cursor::Sdl3Cursor(CursorShape shape):
    Cursor{new State{shape}}
{}

Sdl3Cursor::Sdl3Cursor(const Image &image, const Point &hotspot):
    Cursor{new State{image, hotspot}}
{}

Sdl3Cursor::Sdl3Cursor(const Cursor &cursor)
{
    static_cast<Cursor &>(*this) = cursor;
}

SDL_Cursor *Sdl3Cursor::sdlCursor() const
{
    return me().cursor_;
}

const Sdl3Cursor::State &Sdl3Cursor::me() const
{
    return Object::me.as<State>();
}

} // namespace cc

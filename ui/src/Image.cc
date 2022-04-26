/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Image>
#include <cc/shadowBlur>
#include <cairo/cairo.h>
#include <algorithm>
#include <cassert>

namespace cc {

Image::State::~State()
{
    if (cairoSurface_) {
        cairo_surface_destroy(cairoSurface_);
    }
}

cairo_surface_t *Image::State::cairoSurface()
{
    if (!cairoSurface_) {
        cairoSurface_ =
            cairo_image_surface_create_for_data(data_.bytes(), CAIRO_FORMAT_ARGB32, width_, height_, pitch());
    }
    return cairoSurface_;
}

void Image::clear(Color c)
{
    std::fill_n(&pixel(0), count(), c);
}

void Image::premultiply()
{
    const long n = count();
    for (long i = 0; i < n; ++i)
        pixel(i).premultiply();
}

void Image::normalize()
{
    const long n = count();
    for (long i = 0; i < n; ++i)
        pixel(i).normalize();
}

void Image::copyToXy(Out<Image> target, int x, int y) const
{
    const int ws = width();
    const int hs = height();
    const int wt = target->width();
    const int ht = target->height();

    int is = 0;
    int it = wt * y + x;

    int wl = ws;

    if (x < 0) {
        wl += x;
        is -= x;
        it -= x;
        x = 0;
    }

    if (wt < x + wl) {
        wl = wt - x;
    }

    if (wl > 0) {
        for (int ys = 0, yt = y; ys < hs && yt < ht; ++ys, ++yt) {
            if (yt >= 0) {
                std::memcpy(&target->pixel(it), &pixel(is), sizeof(Color) * wl);
            }
            is += ws;
            it += wt;
        }
    }
}

void Image::applyOver(Color paper)
{
    assert(paper.isOpaque());
    const long n = count();
    for (long i = 0; i < n; ++i) {
        auto &p = pixel(i);
        if (!p.isOpaque()) p = Color::blend(p, paper);
    }
}

bool Image::checkOpaque() const
{
    const long n = count();
    for (long i = 0; i < n; ++i) {
        if (!pixel(i).isOpaque())
            return false;
    }
    return true;
}

void Image::shadowBlur(int radius, Color shadowColor)
{
    cc::shadowBlur(*this, radius, shadowColor);
}

} // namespace cc

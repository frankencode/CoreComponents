/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cairo/cairo.h>
#include <cmath>
#include <cc/ui/shadowBlurInsitu>
#include <cc/ui/Image>

namespace cc {
namespace ui {

Image::~Image()
{
    if (cairoSurface_)
        cairo_surface_destroy(cairoSurface_);
}

void Image::init()
{
    if (isValid())
        data_ = String::allocate(w_ * h_ * sizeof(Color));
}

cairo_surface_t *Image::cairoSurface() const
{
    if (!cairoSurface_)
        cairoSurface_ = cairo_image_surface_create_for_data(mutate(data_)->bytes(), CAIRO_FORMAT_ARGB32, w_, h_, pitch());
    return cairoSurface_;
}

void Image::clear(Color c)
{
    const int n = data_->itemCount<Color>();
    for (int i = 0; i < n; ++i)
        mutate(data_)->item<Color>(i) = c;
}

void Image::normalize()
{
    const int n = data_->itemCount<Color>();
    Color *p = &mutate(data_)->item<Color>(0);
    for (int i = 0; i < n; ++i)
        p[i]->normalize();
}

void Image::shadowBlur(int radius, Color shadowColor)
{
    shadowBlurInsitu(this, radius, shadowColor);
}

}} // namespace cc::ui

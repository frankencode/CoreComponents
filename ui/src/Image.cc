/*
 * Copyright (C) 2017 Frank Mertens.
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

Ref<Image> Image::create(Size size)
{
    return new Image(size);
}

Image::Image(Size size):
    w_(std::ceil(size[0])),
    h_(std::ceil(size[1])),
    data_(String::allocate(w_ * h_ * sizeof(Color))),
    cairoSurface_(0)
{}

Image::~Image()
{
    if (cairoSurface_)
        cairo_surface_destroy(cairoSurface_);
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

cairo_surface_t *Image::cairoSurface() const
{
    if (!cairoSurface_)
        cairoSurface_ = cairo_image_surface_create_for_data(mutate(data_)->bytes(), CAIRO_FORMAT_ARGB32, w_, h_, pitch());
    return cairoSurface_;
}

}} // namespace cc::ui

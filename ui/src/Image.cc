/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Image>
#include <cc/ui/shadowBlurInsitu>
#include <cairo/cairo.h>

namespace cc {
namespace ui {

Image::Instance::Instance(int width, int height):
    w_{width},
    h_{height}
{
    if (w_ <= 0) width = 1;
    if (h_ <= 0) height = 1;
    data_ = String::allocate(w_ * h_ * sizeof(Color));
}

Image::Instance::~Instance()
{
    if (cairoSurface_)
        cairo_surface_destroy(cairoSurface_);
}

cairo_surface_t *Image::Instance::cairoSurface() const
{
    if (!cairoSurface_)
        cairoSurface_ = cairo_image_surface_create_for_data(mutate(data_)->bytes(), CAIRO_FORMAT_ARGB32, w_, h_, pitch());
    return cairoSurface_;
}

void Image::Instance::clear(Color c)
{
    const int n = data_->itemCount<Color>();
    for (int i = 0; i < n; ++i)
        mutate(data_)->item<Color>(i) = c;
}

void Image::Instance::normalize()
{
    const int n = data_->itemCount<Color>();
    Color *p = &mutate(data_)->item<Color>(0);
    for (int i = 0; i < n; ++i)
        p[i]->normalize();
}

void Image::Instance::shadowBlur(int radius, Color shadowColor)
{
    shadowBlurInsitu(this, radius, shadowColor);
}

}} // namespace cc::ui

/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cairo/cairo.h>
#include <cc/ui/Image>

namespace cc {
namespace ui {

Ref<Image> Image::create(Size size)
{
    return new Image(size);
}

Image::Image(Size size):
    size_(size),
    data_(String::allocate(size->w() * size->h() * sizeof(Color))),
    cairoSurface_(0)
{}

Image::~Image()
{
    if (cairoSurface_)
        cairo_surface_destroy(cairoSurface_);
}

void Image::clear(Color c)
{
    const int n = size_->w() * size_->h();
    for (int i = 0; i < n; ++i)
        mutate(data_)->item<Color>(i) = c;
}

#if 0
void Image::clearRect(Rect r, Color c)
{
    r = r->clipTo(size_);
    Color *p = &pixel(r->x(), r->y());
    for (int y = 0; y < r->h(); ++y) {
        for (int x = 0; x < r->w(); ++x) p[x] = c;
        p += size_->w();
    }
}

void Image::blendRect(Rect r, Color c)
{
    r = r->clipTo(size_);
    Color *p = &pixel(r->x(), r->y());
    for (int y = 0; y < r->h(); ++y) {
        for (int x = 0; x < r->w(); ++x) p[x] = Color::blend(c, *p);
        p += size_->w();
    }
}
#endif

void Image::normalize()
{
    const int n = data_->itemCount<Color>();
    Color *p = &mutate(data_)->item<Color>(0);
    for (int i = 0; i < n; ++i)
        Color::normalize(p + i);
}

cairo_surface_t *Image::cairoSurface() const
{
    if (!cairoSurface_)
        cairoSurface_ = cairo_image_surface_create_for_data(mutate(data_)->bytes(), CAIRO_FORMAT_ARGB32, size_[0], size_[1], pitch());
    return cairoSurface_;
}

}} // namespace cc::ui

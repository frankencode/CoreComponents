/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/types>
#include <cc/ui/Image>

namespace cc {
namespace ui {

Image::Image(int w, int h):
    data_(ByteArray::allocate(w * h * sizeof(Color))),
    w_(w), h_(h)
{}

void Image::clear(Color color)
{
    int n = w_ * h_;
    for (int i = 0; i < n; ++i)
        data_->item<Color>(i) = color;
}

void Image::drawSpan(int x, int y, int n, Color c)
{
    if (y < 0 || h_ <= y) return;
    if (x >= w_) return;
    if (x < 0) {
        if (x + n < 0) return;
        n += x;
        x = 0;
    }
    else if (x + n > w_) n = w_ - x;

    Color *p = &pixel(x, y);
    if (Color::alpha(c) == 0xFF) {
        for (int i = 0; i < n; ++i) {
            CC_ASSERT(hasPixel(x + i, y));
            p[i] = c;
        }
    }
    else {
        for (int i = 0; i < n; ++i) {
            CC_ASSERT(hasPixel(x + i, y));
            p[i] = Color::blend(c, *p);
        }
    }
}

}} // namespace cc::ui

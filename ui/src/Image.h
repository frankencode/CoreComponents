/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Object>
#include <cc/ByteArray>
#include <cc/Color>
#include <cc/ui/Rect>

namespace cc {
namespace ui {

class Image: public Object
{
public:
    inline static Ref<Image> create(int w, int h) { return new Image(w, h); }

    inline int w() const { return w_; }
    inline int h() const { return h_; }

    inline void *data() { return data_->bytes(); }
    inline int pitch() const { return w_ * sizeof(Color); }

    inline bool hasPixel(int x, int y) const { return 0 <= x && x < w_ && 0 <= y && y < h_; }
    inline Color &pixel(int x, int y) { return data_->item<Color>(y * w_ + x); }

    void drawSpan(int x, int y, int n, Color c);

    inline void fillRect(Rect *rect, Color c)
    {
        return fillRect(rect->x(), rect->y(), rect->w(), rect->h(), c);
    }

    inline void fillRect(int x, int y, int w, int h, Color c)
    {
        // FIXME: improve performance in opaque case by memcpy
        for (int i = 0; i < h; ++i) drawSpan(x, y + i, w, c);
    }

    void clear(Color color);

private:
    Image(int w, int h);
    Ref<ByteArray> data_;
    int w_, h_;
};

}} // namespace cc::ui

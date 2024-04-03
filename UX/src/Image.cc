/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Image>
#include <cc/ResourceManager>
#include <cc/ImageIoPlugin>
#include <cc/File>
#include <cc/WebP>
#include <cc/CaptureSink>
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

void Image::State::finish()
{}

void Image::State::premultiply()
{
    const long n = count();
    Color *p = &pixel(0);
    for (long i = 0; i < n; ++i) {
        p[i].premultiply();
    }
}

void Image::State::normalize()
{
    const long n = count();
    Color *p = &pixel(0);
    for (long i = 0; i < n; ++i) {
        p[i].normalize();
    }
}

Image::Image(const String &path, const Bytes &data)
{
    String realPath = ResourceManager{}.realPath(path);
    int width = 0, height = 0;
    Bytes source = data;
    if (source.count() == 0) source = File{realPath}.map();
    ImageIoPlugin plugin = ImageIoPlugin::detect(
        realPath,
        source,
        &width, &height
    );
    if (plugin) {
        Image image;
        if (plugin.decodeInto(&image, source)) {
            *this = image;
        }
    }
}

void Image::clear(Color c)
{
    std::fill_n(&pixel(0), count(), c);
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

void Image::tone(Color color)
{
    const long n = count();
    const uint32_t a = color.value() & ~Color::AlphaMask;
    for (long i = 0; i < n; ++i) {
        uint32_t &v = pixel(i).value();
        if ((v & Color::AlphaMask) != 0) {
            v &= ~Color::AlphaMask;
            v |= a;
        }
    }
}

void Image::shadowBlur(int radius, Color shadowColor)
{
    cc::shadowBlur(*this, radius, shadowColor);
}

Image Image::scale(int scaledWidth, int scaledHeight) const
{
    if (scaledWidth <= 0 || scaledHeight <= 0) return Image{0, 0};
    if (width() == 0 || height() == 0) return Image{0, 0};
    CaptureSink sink;
    WebP::encode(sink, *this);
    Bytes essence = sink.writtenData();
    Image image { scaledWidth, scaledHeight };
    WebP::decodeInto(&image, essence);
    return image;
}

void Image::save(const String &path, const String &mediaType)
{
    ImageIoPlugin{mediaType, path.fileSuffix()}
    .encode(File{path, FileOpen::Overwrite}, *this);
}

} // namespace cc

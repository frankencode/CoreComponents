/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/binarySearch>
#include <cc/GlyphIndexArray>

namespace cc {
namespace sfnt {

Ref<GlyphIndexArray> GlyphIndexArray::open(const CharArray *data)
{
    return new GlyphIndexArray(data);
}

GlyphIndexArray::GlyphIndexArray(const CharArray *data):
    data_{data}
{}

bool GlyphIndexArray::lookup(unsigned glyphIndex, int *coverageIndex) const
{
    return binarySearch(this, glyphIndex, coverageIndex);
}

}} // namespace cc::sfnt

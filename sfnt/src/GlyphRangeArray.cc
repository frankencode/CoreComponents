/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/sfnt/GlyphRangeArray>

namespace cc {
namespace sfnt {

Ref<GlyphRangeArray> GlyphRangeArray::open(const CharArray *data)
{
    return new GlyphRangeArray(data);
}

GlyphRangeArray::GlyphRangeArray(const CharArray *data):
    data_{data}
{}

bool GlyphRangeArray::lookup(unsigned glyphIndex, int *coverageIndex) const
{
    int rangeIndex = 0;
    if (binarySearch(this, glyphIndex, &rangeIndex)) {
        if (coverageIndex) {
            RangeRecord range = at(rangeIndex);
            *coverageIndex = range->startCoverageIndex() + glyphIndex - range->startGlyphIndex;
        }
        return true;
    }
    return false;
}

}} // namespace cc::sfnt

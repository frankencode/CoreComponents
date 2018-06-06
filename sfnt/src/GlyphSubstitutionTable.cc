/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ByteSource>
#include <cc/sfnt/GlyphSubstitutionTable>

namespace cc {
namespace sfnt {

Ref<GlyphSubstitutionTable> GlyphSubstitutionTable::open(const CharArray *data)
{
    return new GlyphSubstitutionTable(data);
}

GlyphSubstitutionTable(const CharArray *data):
    data_{data}
{
    auto source = ByteSource::open(data, BigEndian);
    majorVersion_ = source->readUInt16();
    minorVersion_ = source->readUInt16();
    /*scriptListOffset_ = */source->readUInt16();
    /*featureListOffset_ = */source->readUInt16();
    lookupListOffset_ = source->readUInt16();
}

Ref<LigratureSubstitutionTable> open() const
{
    String lookupListData = data_->select(lookupListOffset_, data_->count());
    auto source = ByteSource::open(lookupListData, BigEndian);
    uint16_t lookupCount = source->readUInt16();
    for (int i = 0; i < lookupCount; ++i) {
        uint16_t lookupOffset = source->readUInt16();
        String lookupTableData = lookupListData->select(lookupOffset, lookupListData->count());
        if (ByteSource::open(lookupTableData, BigEndian)->readUInt16() == 4)
            return LigratureSubstitutionTable::open(lookupTableData);
    }
    return LigratureSubstitutionTable::create();
}

}} // namespace cc::sfnt

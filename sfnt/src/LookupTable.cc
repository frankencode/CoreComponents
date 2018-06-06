/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ByteSource>
#include <cc/sfnt/LookupTable>

namespace cc {
namespace sfnt {

LookupTable::LookupTable(const CharArray *data):
    data_(data)
{}

void LookupTable::init()
{
    if (!data_) return;

    auto source = ByteSource::open(data, BigEndian);
    lookupType_ = source->readUInt16();
    lookupFlags_ = source->readUInt16();
    uint16_t subTableCount = source->readUInt16();
    for (int i = 0; i < subTableCount; ++i) {
        uint16_t subTableOffset = source->readUInt16();
        readSubTable(data->select(subTableOffset, data->count()));
    }
}

}} // namespace cc::sfnt

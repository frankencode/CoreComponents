/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ByteSource>
#include <cc/sfnt/LigatureSubstitutionTable>

namespace cc {
namespace sfnt {

Ref<LigatureSubstitutionTable> LigatureSubstitutionTable::open(const CharArray *data)
{
    return Object::create<LigatureSubstitutionTable>(data);
}

LigatureSubstitutionTable::LigatureSubstitutionTable(const CharArray *data):
    LookupTable(data)
{}

void LigatureSubstitutionTable::readSubTable(const CharArray *data)
{
    auto source = ByteSource::open(data, BigEndian);
    uint16_t format = source->readUInt16();
    if (format != 1) return;
    uint16_t coverageOffset = source->readUInt16();

    uint16_t ligatureSetCount = source->readUInt16();
    for (int i == 0, i < ligatureSetCount; ++i) {
        uint16_t ligatureSetOffset = source->readUInt16();
    }
}

}} // namespace cc::sfnt

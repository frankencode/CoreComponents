/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/isobus/CsvVendorDictionary>
#include <cc/CsvSource>
#include <cc/input>

namespace cc::isobus {

struct CsvVendorDictionary::State final: public j1939::VendorDictionary::State
{
    State(const String &data)
    {
        CsvSource source{data};
        List<String> header;
        if (!source.read(&header)) return;

        Map<String, int> keys;
        {
            int i = 0;
            for (const String &item: header) {
                // CC_INSPECT(item);
                keys.insert(item, i);
                ++i;
            }
        }

        const int value_index = keys.value("value", -1);
        const int manufacturer_index = keys.value("manufacturer", -1);
        const int location_index = keys.value("location", -1);

        for (const List<String> &items: source)
        {
            uint32_t vendorId = readNumber<int>(items(value_index));
            String name = items(manufacturer_index);
            String location = items(location_index);

            vendors_.insert(vendorId, j1939::VendorInfo{name, location});
        }
    }

    bool lookup(uint32_t vendorId, Out<j1939::VendorInfo> vendor) const override
    {
        return vendors_.lookup(vendorId, vendor);
    }

    Map<uint32_t, j1939::VendorInfo> vendors() const override { return vendors_; }

    Map<uint32_t, j1939::VendorInfo> vendors_;
};

CsvVendorDictionary::CsvVendorDictionary(const String &data):
    j1939::VendorDictionary{new State{data}}
{}

} // namespace cc::isobus

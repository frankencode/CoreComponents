/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/isobus/CsvIndustryDictionary>
#include <cc/CsvSource>
#include <cc/input>

namespace cc::isobus {

struct CsvIndustryDictionary::State final: public j1939::IndustryDictionary::State
{
    explicit State(const String &data)
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

        const int industry_group_id_index = keys.value("industry_group_id", -1);
        const int industry_group_description_index = keys.value("industry_group_description", -1);

        for (const List<String> &items: source)
        {
            int industry = readNumber<int>(items(industry_group_id_index));
            String name = items(industry_group_description_index).trimmed();
            industryInfos_.insert(industry, j1939::IndustryInfo{name});
        }
    }

    const Map<int, j1939::IndustryInfo> &industryInfos() const override
    {
        return industryInfos_;
    }

    Map<int, j1939::IndustryInfo> industryInfos_;
};

CsvIndustryDictionary::CsvIndustryDictionary(const String &data):
    j1939::IndustryDictionary{new State{data}}
{}

} // namespace cc::isobus

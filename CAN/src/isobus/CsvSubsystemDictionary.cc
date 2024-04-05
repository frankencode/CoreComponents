/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/isobus/CsvSubsystemDictionary>
#include <cc/CsvSource>
#include <cc/Casefree>
#include <cc/input>

namespace cc::isobus {

struct CsvSubsystemDictionary::State final: public j1939::SubsystemDictionary::State
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
        const int vehicle_system_id_index = keys.value("vehicle_system_id", -1);
        const int vehicle_system_description_index = keys.value("vehicle_system_description", -1);
        const int function_id_index = keys.value("function_id", -1);
        const int function_description_index = keys.value("function_description", -1);
        const int notes_index = keys.value("notes", -1);

        for (const List<String> &items: source)
        {
            int industry = readNumber<int>(items(industry_group_id_index));
            int subsystem = readNumber<int>(items(vehicle_system_id_index));
            String title = items(vehicle_system_description_index).trimmed();
            int function = readNumber<int>(items(function_id_index));
            String functionTitle = items(function_description_index).trimmed();
            String functionNotes = items(notes_index);

            if (
                Casefree<String>{functionTitle} == "Not Available" ||
                Casefree<String>{functionTitle} == "Reserved"
            ) continue;

            Key key{industry, subsystem};
            j1939::SubsystemInfo subsystemInfo;
            if (!subsystemInfos_.lookup(key, &subsystemInfo)) {
                subsystemInfo = j1939::SubsystemInfo{title};
                subsystemInfos_.insert(key, subsystemInfo);
            }
            access(subsystemInfo)->functionInfos_.insert(function, j1939::FunctionInfo{functionTitle, functionNotes});
        }
    }

    const Map<Key, j1939::SubsystemInfo> &subsystemInfos() const override
    {
        return subsystemInfos_;
    }

    Map<Key, j1939::SubsystemInfo> subsystemInfos_;
};

CsvSubsystemDictionary::CsvSubsystemDictionary(const String &data):
    j1939::SubsystemDictionary{new State{data}}
{}

} // namespace cc::isobus

/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/isobus/CsvMessageDictionary>
#include <cc/j1939/CanId>
#include <cc/CsvSource>
#include <cc/input>
#include <cc/DEBUG>

namespace cc::isobus {

struct CsvMessageDictionary::State final: public j1939::MessageDictionary::State
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

        const int pgn_index = keys.value("pgn", -1);
        const int edp_index = keys.value("edp", -1);
        const int dp_index = keys.value("dp", -1);
        const int acronym_index = keys.value("acronym", -1);
        const int parameter_group_label_index = keys.value("parameter_group_label", -1);
        const int pgn_description_index = keys.value("pgn_description", -1);
        const int pgn_notes_index = keys.value("pgn_notes", -1);
        const int multipacket_index = keys.value("multipacket", -1);
        const int transmission_rate_index = keys.value("transmission_rate", -1);
        const int default_priority_index = keys.value("default_priority", -1);
        const int pgn_document_index = keys.value("pgn_document", -1);
        const int spn_name_index = keys.value("spn_name", -1);
        const int sp_start_bit_index = keys.value("sp_start_bit", -1); // byte(1..8).bit(1..8)
        const int length_min_index = keys.value("length_min", -1);
        const int length_max_index = keys.value("length_max", -1);
        const int spn_description_index = keys.value("spn_description", -1);
        const int spn_notes_index = keys.value("spn_notes", -1);
        const int data_range_index = keys.value("data_range", -1);
        const int operational_range_index = keys.value("operational_range", -1);
        const int units_index = keys.value("units", -1);
        const int scale_factor_index = keys.value("scale_factor", -1);
        const int offset_value_index = keys.value("offset_value", -1);
        const int range_maximum_index = keys.value("range_maximum", -1);
        const int slot_identifier_index = keys.value("slot_identifier", -1);
        const int slot_name_index = keys.value("slot_name", -1);
        const int spn_type_index = keys.value("spn_type", -1);

        for (const List<String> &items: source)
        {
            int ok = 0;
            uint32_t groupNumber =
                readNumber<uint32_t>(items(pgn_index), &ok) |
                (readNumber<uint32_t>(items(dp_index ), &ok) << 24) |
                (readNumber<uint32_t>(items(edp_index), &ok) << 25);

            if (ok != 3) continue;

            j1939::MessageTypeInfo entry;
            if (!entries_.lookup(groupNumber, &entry)) {
                access(entry)->name_ = items(acronym_index).trimmed();
                access(entry)->label_ = items(parameter_group_label_index);
                access(entry)->description_ = readDescription(items(pgn_description_index));

                access(entry)->notes_ = items(pgn_notes_index);
                access(entry)->isMultiPacket_  = (items(multipacket_index) == "Yes");
                access(entry)->transmissionRate_ = items(transmission_rate_index);
                access(entry)->defaultPriority_ = readNumber<int>(items(default_priority_index));
                access(entry)->document_ = items(pgn_document_index);
                entries_.insert(groupNumber, entry);
            }
            j1939::MessageMemberInfo member;
            access(member)->name_ = items(spn_name_index).trimmed();
            access(member)->description_ = readDescription(items(spn_description_index));
            access(member)->notes_ = items(spn_notes_index);
            access(member)->dataRange_ = items(data_range_index);
            access(member)->operationalRange_ = items(operational_range_index);
            access(member)->unit_ = items(units_index);
            access(member)->bitOffset_ = readBitOffset(items(sp_start_bit_index));
            access(member)->bitLengthMin_ = readNumber<int>(items(length_min_index));
            access(member)->bitLengthMax_ = readNumber<int>(items(length_max_index));
            access(member)->scale_ = readNumber<double>(items(scale_factor_index));
            access(member)->offset_ = readNumber<double>(items(offset_value_index));
            access(member)->max_ = readNumber<double>(items(range_maximum_index));
            access(member)->slotIdentifier_ = items(slot_identifier_index);
            access(member)->slotName_ = items(slot_name_index);
            access(member)->type_ = items(spn_type_index);
            if (member.bitLengthMax() > 0) {
                access(entry)->members_.append(member);
            }
        }
    }

    static String readDescription(const String &text)
    {
        if (text.startsWith("See http")) return String{};
        return text.replaced("_x000D_", "");
    }

    static int readBitOffset(const String &item)
    {
        List<String> parts = item.split('.');
        if (parts.count() != 2) return 0;
        return (8 * (parts.at(0).toInt() - 1)) + parts.at(1).toInt() - 1;
    }

    bool lookup(uint32_t canId, Out<j1939::MessageTypeInfo> messageType) const override
    {
        return entries_.lookup(j1939::CanId{canId}.pgn(), messageType);
    }

    Map<uint32_t, j1939::MessageTypeInfo> messageTypes() const override
    {
        return entries_;
    }

    Map<uint32_t, j1939::MessageTypeInfo> entries_;
};

CsvMessageDictionary::CsvMessageDictionary(const String &data):
    j1939::MessageDictionary{new State{data}}
{}

} // namespace cc::isobus

/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/j1939/Message>
#include <cc/j1939/DictionaryManager>
#include <cc/j1939/AddressClaimManager>
#include <cc/Format>

namespace cc::j1939 {

String Message::toString() const
{
    Format f;

    CanId id{frame_};
    uint32_t pgn = id.pgn();

    j1939::MessageTypeInfo type;
    if (DictionaryManager{}.messageTypeDictionary().lookup(pgn << 8, &type))
    {
        f <<
            "{\n"
            "  \"type\": \"j1939." << type.name() << "\",\n"
            "  \"label\": \"" << type.label() << "\",\n"
            "  \"pgn\": " << pgn << ",\n"
            "  \"src\": " << id.src() << ",\n";

        if (id.isDirected()) {
            f << "  \"dst\": " << id.dst() << ",\n";
        }

        String srcName = j1939::AddressClaimManager{}.name(id.src());
        if (srcName != "") {
            f << "  \"src-name\": \"" << srcName << "\",\n";
        }

        if (id.isDirected()) {
            String dstName = j1939::AddressClaimManager{}.name(id.dst());
            if (dstName != "") {
                f << "  \"dst-name\": \"" << dstName << "\",\n";
            }
        }

        uint64_t data = frame_.data<uint64_t>();
        for (const j1939::MessageMemberInfo &member: type.members()) {
            if (!member.name()) continue;
            if (member.bitLengthMax() == 0) continue;
            if (member.bitLengthMin() != member.bitLengthMax()) continue;

            uint64_t value = bit(member.bitOffset() + member.bitLengthMax() - 1, member.bitOffset(), data);

            f << "  \"" << member.name() << "\": " << value << (type.members().lastIs(member) ? "\n" : ",\n");

            #if 0
            if (member.description() != "") {
                f << "      /*" << nl;
                List<String> lines = member.description().split('\n');
                for (const String &line: lines) {
                    f << "        \"" << line << "\"" << (lines.lastIs(line) ? "" : ",") << nl;
                }
                f << "      */" << nl;
            }
            #endif
        }

        f <<
            "  \"prio\": " << id.prio() << "\n"
            "}";
    }
    else {
        f <<
            "{\n"
            "  \"type\": \"j1939.?\",\n"
            "  \"pgn\": " << pgn << ",\n"
            "  \"src\": " << id.src() << ",\n";

        if (id.isDirected()) {
            f << "  \"dst\": " << id.dst() << ",\n";
        }

        String srcName = j1939::AddressClaimManager{}.name(id.src());
        if (srcName != "") {
            f << "  \"src-name\": \"" << srcName << "\",\n";
        }

        if (id.isDirected()) {
            String dstName = j1939::AddressClaimManager{}.name(id.dst());
            if (dstName != "") {
                f << "  \"dst-name\": \"" << dstName << "\",\n";
            }
        }

        f <<
            "  \"prio\": " << id.prio() << "\n"
            "}";
    }

    return f;
}

} // namespace cc::j1939

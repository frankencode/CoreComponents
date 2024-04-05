/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/isobus/pd/VersionReply>
#include <cc/j1939/AddressClaimManager>
#include <cc/Format>

namespace cc::isobus::pd {

String VersionReply::toString() const
{
    Format f;

    f <<
        "{\n"
        "  \"type\": \"" << shortName() << "\",\n"
        "  \"pgn\": " << pgn() << ",\n"
        "  \"src\": " << src() << ",\n"
        "  \"dst\": " << dst() << ",\n";

    String srcName = j1939::AddressClaimManager{}.name(src());
    String dstName = j1939::AddressClaimManager{}.name(dst());

    if (srcName != "") {
        f << "  \"src-name\": \"" << srcName << "\",\n";
    }
    if (dstName != "") {
        f << "  \"dst-name\": \"" << dstName << "\",\n";
    }

    f << "  \"protocol-version\": " << protocolVersion() << ",\n";

    if (hasBootTime()) {
        f << "  \"boot-time\": " << bootTime() << ",\n";
    }

    List<String> optionNames;
    {
        if (options() & Options::Documentation) optionNames << "isobus.pd.options.documentation";
        if (options() & Options::PositionBasedLogging) optionNames << "isobus.pd.options.position_based_logging";
        if (options() & Options::PositionBasedControl) optionNames << "isobus.pd.options.position_based_control";
        if (options() & Options::PeerControl) optionNames << "isobus.pd.options.peer_control";
        if (options() & Options::SectionControl) optionNames << "isobus.pd.options.section_control";
    }
    f <<
        "  \"options\": \"" << optionNames.join("|") << "\",\n"
        "  \"section-control-booms\": " << sectionControlBoomCount() << ",\n"
        "  \"section-control-sections\": " << sectionControlSectionCount() << ",\n"
        "  \"section-control-channels:\" " << sectionControlChannelCount() << ",\n"
        "  \"prio\": " << j1939::CanId{frame_}.prio() << "\n"
        "}";

    return f;
}

} // namespace cc::isobus::pd

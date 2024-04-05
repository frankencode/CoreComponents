/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/sdo/ReadReply>
#include <cc/Format>
#include <cassert>

namespace cc::can_open::sdo {

CanFrame ReadReply::createFrame(int serverId, Selector selector, const Bytes &data)
{
    assert(1 <= serverId && serverId <= 0x7F);
    assert(data.count() > 0);

    const uint8_t scs = +Reply::Specifier::Read;
    const uint8_t e   = data.count() <= 4;
    const uint8_t n   = (e) ? 4 - data.count() : 0;
    const uint8_t s   = 1;

    return
        CanFrame { 0x580 + static_cast<uint32_t>(serverId) }
        (0, (scs << 5) | (n << 2) | (e << 1) | s)
        (1, selector.index())
        (2, selector.index() >> 8)
        (3, selector.subIndex())
        (4, e ? data(0) : static_cast<uint8_t>(data.count()))
        (5, e ? data(1) : static_cast<uint8_t>(data.count() >> 8))
        (6, e ? data(2) : static_cast<uint8_t>(data.count() >> 16))
        (7, e ? data(3) : static_cast<uint8_t>(data.count() >> 24));
}

String ReadReply::toString() const
{
    if (!isValid()) return String{};

    Format f;
    f <<
        "{\n"
        "  \"type\": \"" << typeName() << "\",\n"
        "  \"server\": " << serverId() << ",\n"
        "  \"selector\": \"" << selector() << "\",\n";

    if (hasExpeditedData())
        f << "  \"data\": \"" << hexLine(expeditedData()) << "\",\n";
    else if (hasTotalDataCount())
        f << "  \"total-size\": " << totalSize() << "\n";

    f << "}";

    return f;
}

} // namespace cc::can_open::sdo

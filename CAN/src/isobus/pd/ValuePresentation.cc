/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/isobus/pd/ValuePresentation>
#include <cc/Format>

namespace cc::isobus::pd {

String ValuePresentation::toString() const
{
    return Format{} <<
        "{\n"
        "  \"type\": \"" << shortName() << "\",\n"
        "  \"object-id\": " << objectId() << ",\n"
        "  \"offset\": " << offset() << ",\n"
        "  \"scale\": " << scale() << ",\n"
        "  \"fraction-decimals\": " << fractionDecimals() << ",\n"
        "  \"unit-designator\": \"" << unitDesignator() << "\"\n"
        "}";
}

} // namespace cc::isobus::pd

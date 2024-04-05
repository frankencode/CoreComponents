/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/isobus/pd/ObjectPoolActivationReply>
#include <cc/Format>

namespace cc::isobus::pd {

void ObjectPoolActivationReply::printValue(Format &f) const
{

    if (+errors()) {
        f <<
            "  \"success\": false,\n"
            "  \"errors\": \"" << errors() << "\",\n";

        if (errors() & ObjectPoolActivationReply::Errors::FaultyObjectPool) {
            f <<
                "  \"object-pool-errors\": \"" << objectPoolErrors() << "\",\n"
                "  \"faulty-object-parent-id\": " << faultyObjectParentId() << ",\n"
                "  \"faulty-object-id\": " << faultyObjectId() << ",\n";
        }
    }
    else {
        f << "  \"success\": true,\n";
    }
}

String str(const ObjectPoolActivationReply::Errors &errors)
{
    List<String> parts;
    if (errors & ObjectPoolActivationReply::Errors::FaultyObjectPool) parts << "isobus.device.activation_error.faulty_object_pool";
    if (errors & ObjectPoolActivationReply::Errors::OutOfMemory) parts << "isobus.device.activation_error.out_of_memory";
    if (errors & ObjectPoolActivationReply::Errors::AmbiguousStructureLabel) parts << "isobus.device.activation_error.ambiguous_structure_label";
    return parts.join("|");
}

String str(const ObjectPoolActivationReply::ObjectPoolErrors &errors)
{
    List<String> parts;
    if (errors & ObjectPoolActivationReply::ObjectPoolErrors::UnsupportedProperty) parts << "isobus.device.activation_error.unsupported_property";
    if (errors & ObjectPoolActivationReply::ObjectPoolErrors::UndefinedReference) parts << "isobus.device.activation_error.undefined_reference";
    if (errors & ObjectPoolActivationReply::ObjectPoolErrors::OtherError) parts << "isobus.device.activation_error.other_error";
    if (errors & ObjectPoolActivationReply::ObjectPoolErrors::PoolDeleted) parts << "isobus.device.activation_error.pool_deleted";
    return parts;
}

} // namespace cc::isobus::pd

/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/MetaError>

namespace cc {

MetaError::MetaError(const String &message, const MetaObject &offendingObject, const String &memberName):
    UsageError{message},
    offendingObject_{offendingObject},
    memberName_{memberName}
{}

MetaObject MetaError::offendingObject() const
{
    return offendingObject_;
}

String MetaError::memberName() const
{
    return memberName_;
}

} // namespace cc

/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/PulseError>
#include <cc/str>
#include <pulse/error.h>

namespace cc {

String PulseError::message() const
{
    return List<String>{} << source_ << ":" << str(line_) << ": " << pa_strerror(errorCode_);
}

} // namespace cc

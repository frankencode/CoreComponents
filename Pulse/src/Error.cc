/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/pulse/Error>
#include <cc/str>
#include <pulse/error.h>

namespace cc::pulse {

String Error::message() const
{
    return List<String>{} << source_ << ":" << str(line_) << ": " << pa_strerror(errorCode_);
}

} // namespace cc::pulse

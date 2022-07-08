/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/SystemError>
#include <cc/str>

namespace cc {

String systemError(int errorCode)
{
#ifdef __USE_GNU
    static thread_local String buffer = String::allocate(256);
    const char *unknown = "Unknown error";
    std::memcpy(buffer.items(), unknown, std::strlen(unknown) + 1);
    return strerror_r(errorCode, buffer, buffer.count());
#else
    static thread_local String buffer = String::allocate(256, '\0');
    /*int ret = */strerror_r(errorCode, buffer, buffer.count());
    return buffer;
#endif
}

String SystemError::message() const
{
    return systemError(errorCode_);
}

String SystemResourceError::message() const
{
    return List<String>{} << systemError(errorCode_) << ": \"" << resource_ << "\""
        #ifndef NDEBUG
        << " (" << String{source_}.fileName() << ":" << str(line_) << ", errno = " << str(errorCode_) << ")"
        #endif
        ;
}

String SystemDebugError::message() const
{
    return List<String>{} << systemError(errorCode_) << " (" << String{source_}.fileName() << ":" << str(line_) << ", errno = " << str(errorCode_) << ")";
}

} // namespace cc

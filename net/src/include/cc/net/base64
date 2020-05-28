/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>

namespace cc {
namespace net {
namespace base64 {

String encode(const String &source);
String decode(const String &source);

/** \brief Base64 decoding failed: broken input
  */
class DecodeError {
public:
    ~DecodeError() throw() {}
};

class IllegalInputSize4Error: public DecodeError {
public:
    ~IllegalInputSize4Error() throw() {}

    virtual String message() const {
        return "Base-64 error: input size need to be a multiple of 4 bytes";
    }
};

class IllegalPaddingError: public DecodeError {
public:
    ~IllegalPaddingError() throw() {}

    virtual String message() const {
        return "Base-64 error: unsupported padding";
    }
};

class IllegalCharacterError: public DecodeError {
public:
    ~IllegalCharacterError() throw() {}

    virtual String message() const {
        return "Base-64 error: illegal symbol";
    }
};

}}} // namespace cc::net::base64

/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNET_BASE64_H
#define FLUXNET_BASE64_H

#include <flux/String>

namespace flux {
namespace net {
namespace base64 {

String encode(const String &source);
String decode(const String &source);

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

}}} // namespace flux::net::base64

#endif // FLUXNET_BASE64_H

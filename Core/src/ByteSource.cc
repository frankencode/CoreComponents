/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ByteSource>

namespace cc {

String ByteSource::readCString()
{
    List<char> chars;
    for (char ch; readChar(&ch);) {
        if (!ch) break;
        chars.append(ch);
    }
    String s = String::allocate(chars.count());
    long i = 0;
    for (char ch: chars) {
        s[i++] = ch;
    }
    return s;
}

} // namespace cc

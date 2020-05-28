/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>

namespace ccaes {

using namespace cc;

class Readline
{
public:
    static String getPassword(const String &prompt);
};

} // namespace ccaes

/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/SetInstance>

using namespace cc;
using namespace cc::testing;

namespace cc { template class SetInstance<String>; }

int main(int argc, char **argv)
{
    return 0;
}

/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/TestCase>
#include <cc/TestSuite>

namespace cc {

TestCase::TestCase(const String &name, const std::function<void()> &run, bool skip):
    me{name, run, skip}
{
    TestSuite{}.appendTestCase(*this);
}

} // namespace cc

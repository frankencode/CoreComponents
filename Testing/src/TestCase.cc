/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/TestCase>
#include <cc/TestSuite>

namespace cc {

TestCase::TestCase(const String &name, Function<void()> &&run, bool skip):
    me{name, std::move(run), skip}
{
    TestSuite{}.appendTestCase(*this);
}

} // namespace cc

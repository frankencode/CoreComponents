/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TestCase>
#include <cc/TestSuite>

namespace cc {

TestCase::TestCase(const String &name, Fun<void()> &&run, bool skip):
    me{name, std::move(run), skip}
{
    TestSuite{}.appendTestCase(*this);
}

} // namespace cc

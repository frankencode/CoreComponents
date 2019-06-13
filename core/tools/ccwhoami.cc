/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/Process>
#include <cc/User>

using namespace cc;

int main()
{
    fout() << User::lookup(Process::getEffectiveUserId())->loginName() << nl;
    return 0;
}

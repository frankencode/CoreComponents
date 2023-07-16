/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Process>
#include <cc/UserInfo>
#include <cc/stdio>

int main(int, char *[])
{
    using namespace cc;

    fout() << UserInfo{Process::effectiveUserId()}.loginName() << nl;

    return 0;
}

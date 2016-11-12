/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/KernelInfo>

using namespace cc;

int main(int argc, char **argv)
{
    auto info = KernelInfo::query();
    fout()
        << "name: " << info->name() << nl
        << "release: " << info->release() << nl
        << "version: " << info->version() << nl
        << "machine: " << info->machine() << nl;
    return 0;
}

/*
 * Copyright (C) 2007-2017 Frank Mertens.
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
    KernelInfo info;
    fout()
        << "name: " << info->name() << nl
        << "release: " << info->release() << nl
        << "version: " << info->version() << nl
        << "machine: " << info->machine() << nl;
    return 0;
}

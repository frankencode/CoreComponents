/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/KernelInfo>
#include <cc/stdio>

int main(int argc, char *argv[])
{
    using namespace cc;

    KernelInfo info;

    fout()
        << "name: " << info.name() << nl
        << "release: " << info.release() << nl
        << "version: " << info.version() << nl
        << "machine: " << info.machine() << nl;

    return 0;
}

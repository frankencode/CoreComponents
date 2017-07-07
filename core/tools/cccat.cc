/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/File>

using namespace cc;

int main(int argc, char **argv)
{
    if (argc > 1) {
        for (int i = 1; i < argc; ++i)
            stdOut()->write(File::open(argv[i])->map());
    }
    else {
        stdOut()->write(stdIn()->readAll());
    }

    return 0;
}

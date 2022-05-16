/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/File>
#include <cc/stdio>

int main(int argc, char **argv)
{
    using namespace cc;

    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            IoStream::output().write(File{argv[i]}.map());
        }
    }
    else {
        IoStream::input().transferTo(IoStream::output());
    }

    return 0;
}

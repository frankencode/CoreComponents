/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/File>
#include <cc/stdio>

int main(int argc, char *argv[])
{
    using namespace cc;

    try {
        if (argc > 1) {
            for (int i = 1; i < argc; ++i) {
                stdOutput().write(File{argv[i]}.map().reversed());
            }
        }
        else {
            Bytes data = stdInput().readAll();
            data.reverse();
            stdOutput().write(data);
        }
    }
    catch (IoExhaustion &)
    {}

    return 0;
}

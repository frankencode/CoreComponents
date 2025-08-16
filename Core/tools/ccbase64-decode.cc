/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/IoStream>
#include <cc/Base64>
#include <cc/stdio>

int main(int argc, char *argv[])
{
    using namespace cc;

    String data = stdInput().readAll().trimmed();
    stdOutput().write(base64(data));

    return 0;
}

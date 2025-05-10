/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/IoStream>
#include <cc/base64>
#include <cc/DEBUG>

int main(int argc, char *argv[])
{
    using namespace cc;

    String data = stdInput().readAll().trimmed();
    stdOutput().write(base64Decode(data));

    return 0;
}

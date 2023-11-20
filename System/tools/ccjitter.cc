/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/JitterSource>
#include <cc/IoStream>
#include <cc/str>

int main()
{
    using namespace cc;

    JitterSource source { 16 };

    for (Bytes data { Dim<>{1} }; source.read(&data, 1);)
    {
        IoStream::output().write(data);
    }

    return 0;
}

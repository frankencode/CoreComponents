/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
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
        stdOutput().write(data);
    }

    return 0;
}

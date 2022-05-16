/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/IoStream>
#include <cc/base64>
#include <cc/DEBUG>

int main(int argc, char *argv[])
{
    using namespace cc;

    String data = IoStream::input().readAll().trimmed();
    IoStream::output().write(base64Decode(data));

    return 0;
}

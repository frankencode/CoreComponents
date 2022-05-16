/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/AesEntropySource>
#include <cc/IoStream>

int main()
{
    using namespace cc;

    AesEntropySource{}.transferTo(IoStream::output());

    return 0;
}

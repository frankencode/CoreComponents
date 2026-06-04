/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/AesEntropySource>
#include <cc/IoStream>

int main()
{
    using namespace cc;

    AesEntropySource{}.transferTo(stdOutput());

    return 0;
}

/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
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

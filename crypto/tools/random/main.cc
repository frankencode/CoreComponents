/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/crypto/RandomSource>
#include <cc/File>
#include <cc/stdio>

int main()
{
    using namespace cc;
    using namespace cc::crypto;

    Ref<File> random = File::open("/dev/random");
    Ref<Stream> source = RandomSource::open();

    String buffer = ByteArray::allocate(0x4000);
    while (true) {
        source->read(mutate(buffer));
        stdOut()->write(buffer);
    }

    return 0;
}

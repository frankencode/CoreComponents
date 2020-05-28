/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/System>
#include <cc/Map>
#include <cc/Thread>
#include <cc/can/CanSocket>
#include <cc/can/CanLogger>

using namespace cc;
using namespace cc::can;

int main(int argc, char **argv)
{
    String interface = "can0";
    if (argc >= 2) interface = argv[1];
    auto media = CanSocket::open(interface);
    media->enableErrorFrames(true);

    auto logger = CanLogger::create(media);

    while (true) Thread::sleep(1);

    return 0;
}

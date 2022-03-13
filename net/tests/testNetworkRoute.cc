/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/NetworkRoute>
#include <cc/Format>
#include <cc/testing>
#include <cc/stdio>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "NetworkQueryRouteAll",
        []{
            List<NetworkRoute> list = NetworkRoute::queryAll();

            for (const NetworkRoute &route: list) {
                Format line;
                if (route.destinationMask() == 0 && route.sourceMask() == 0) line << "default";
                if (route.gateway()) line << "via" << route.gateway();
                if (route.destination())
                    line << String(Format{"%%/%%"} << route.destination() << route.destinationMask());
                if (route.outputInterface() != "")
                    line << "out" << route.outputInterface();
                if (route.inputInterface() != "")
                    line << "in" << route.inputInterface();
                if (route.source())
                    line << "src" << String(Format{"%%/%%"} << route.source() << route.sourceMask());
                line << route.priority();
                fout() << line.join(' ') << nl;
            }
        }
    };

    return TestSuite{argc, argv}.run();
}

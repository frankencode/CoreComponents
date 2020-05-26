/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/Format>
#include <cc/stdio>
#include <cc/net/RouteInfo>

using namespace cc;
using namespace cc::testing;
using namespace cc::net;

class QueryRouteInfo: public TestCase
{
public:
    void run()
    {
        Ref<RouteInfoList> list = RouteInfo::queryTable();

        for (int i = 0; i < list->count(); ++i) {
            RouteInfo *info = list->at(i);
            Format line;
            if (info->destinationMask() == 0 && info->sourceMask() == 0) line << "default";
            if (info->gateway()) line << "via" << info->gateway();
            if (info->destination())
                line << string(Format{"%%/%%"} << info->destination() << info->destinationMask());
            if (info->outputInterface() != "")
                line << "out" << info->outputInterface();
            if (info->inputInterface() != "")
                line << "in" << info->inputInterface();
            if (info->source())
                line << "src" << string(Format{"%%/%%"} << info->source() << info->sourceMask());
            line << info->priority();
            fout() << line->join(" ") << nl;
        }
    }
};

int main(int argc, char** argv)
{
    CC_TESTSUITE_ADD(QueryRouteInfo);

    return TestSuite::instance()->run(argc, argv);
}

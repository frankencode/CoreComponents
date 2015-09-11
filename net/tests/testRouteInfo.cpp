/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/testing/TestSuite>
#include <flux/Format>
#include <flux/stdio>
#include <flux/net/RouteInfo>

using namespace flux;
using namespace flux::testing;
using namespace flux::net;

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
                line << String(Format("%%/%%") << info->destination() << info->destinationMask());
            if (info->outputInterface() != "")
                line << "out" << info->outputInterface();
            if (info->inputInterface() != "")
                line << "in" << info->inputInterface();
            if (info->source())
                line << "src" << String(Format("%%/%%") << info->source() << info->sourceMask());
            line << info->priority();
            fout() << line->join(" ") << nl;
        }
    }
};

int main(int argc, char** argv)
{
    FLUX_TESTSUITE_ADD(QueryRouteInfo);

    return testSuite()->run(argc, argv);
}

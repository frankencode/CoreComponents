/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/colors>
#include <cc/stdio>
#include <cc/str>
#include <cc/testing/TestSuite>

using namespace cc;
using namespace cc::testing;

class HsvTest: public TestCase
{
    void run() {
        struct TestData { double h,s,v, r,g,b; };
        TestData testData[] = {
            {   0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
            { 240.0, 1.0, 1.0, 0.0, 0.0, 1.0 },
            { 120.0, 1.0, 1.0, 0.0, 1.0, 0.0 },
            { 180.0, 1.0, 1.0, 0.0, 1.0, 1.0 },
            {   0.0, 1.0, 1.0, 1.0, 0.0, 0.0 },
            { 300.0, 1.0, 1.0, 1.0, 0.0, 1.0 },
            {  60.0, 1.0, 1.0, 1.0, 1.0, 0.0 },
            {   0.0, 0.0, 1.0, 1.0, 1.0, 1.0 }
        };

        for (auto &d: testData)
        {
            double r = 0, g = 0, b = 0;
            hsvToRgb(d.h, d.s, d.v, &r, &g, &b);
            fout()
                << "HSV(" << fixed(d.h, 1) << " " << fixed(d.s, 1) << " " << fixed(d.v, 1) << ") => "
                << "RGB(" << fixed(r, 2) << " " << fixed(g, 2) << " " << fixed(b, 2) << ")" << nl;
            CC_VERIFY(r == d.r && g == d.g && b == d.b);
        }

        for (auto &d: testData)
        {
            double h = 0, s = 0, v = 0;
            rgbToHsv(d.r, d.g, d.b, &h, &s, &v);
            fout()
                << "RGB(" << fixed(d.r, 1) << " " << fixed(d.g, 1) << " " << fixed(d.b, 1) << ") => "
                << "HSV(" << fixed(h, 2) << " " << fixed(s, 2) << " " << fixed(v, 2) << ")" << nl;
             CC_VERIFY((h == d.h) && s == d.s && v == d.v);
        }
    }
};

class HslTest: public TestCase
{
    void run() {
        struct TestData { double h,s,l, r,g,b; };
        TestData testData[] = {
            {   0.0, 1.0, 0.0, 0.0, 0.0, 0.0 },
            { 240.0, 1.0, 0.5, 0.0, 0.0, 1.0 },
            { 120.0, 1.0, 0.5, 0.0, 1.0, 0.0 },
            { 180.0, 1.0, 0.5, 0.0, 1.0, 1.0 },
            {   0.0, 1.0, 0.5, 1.0, 0.0, 0.0 },
            { 360.0, 1.0, 0.5, 1.0, 0.0, 0.0 },
            { 300.0, 1.0, 0.5, 1.0, 0.0, 1.0 },
            {  60.0, 1.0, 0.5, 1.0, 1.0, 0.0 },
            {   0.0, 1.0, 1.0, 1.0, 1.0, 1.0 }
        };

        for (auto &d: testData)
        {
            double r = 0, g = 0, b = 0;
            hslToRgb(d.h, d.s, d.l, &r, &g, &b);
            fout()
                << "HSL(" << fixed(d.h, 1) << " " << fixed(d.s, 1) << " " << fixed(d.l, 1) << ") => "
                << "RGB(" << fixed(r, 2) << " " << fixed(g, 2) << " " << fixed(b, 2) << ")" << nl;
            CC_VERIFY(r == d.r && g == d.g && b == d.b);
        }

        for (auto &d: testData)
        {
            double h = 0, s = 0, l = 0;
            rgbToHsl(d.r, d.g, d.b, &h, &s, &l);
            fout()
                << "RGB(" << fixed(d.r, 1) << " " << fixed(d.g, 1) << " " << fixed(d.b, 1) << ") => "
                << "HSL(" << fixed(h, 2) << " " << fixed(s, 2) << " " << fixed(l, 2) << ")" << nl;
            if (d.r == d.g && d.g == d.b)
                CC_VERIFY(l == d.l);
            else
                CC_VERIFY((h == d.h || h == d.h - 360.) && s == d.s && l == d.l);
        }
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(HsvTest);
    CC_TESTSUITE_ADD(HslTest);

    return TestSuite::instance()->run(argc, argv);
}

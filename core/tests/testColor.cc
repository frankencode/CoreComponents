/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cmath>
#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/Color>

using namespace cc;
using namespace cc::testing;

class GrayShadesOnPaper: public TestCase
{
    void run() {
        Color b = Color(0xFF, 0xFF, 0xFF);
        for (int i = 0; i <= 0xFF; ++i) {
            Color a = Color(0x00, 0x00, 0x00, i);
            Color c = Color::blend(a, b);
            Color d = Color(0xFF - i, 0xFF - i, 0xFF - i);
            if (c != d) fout("%%, %%\n") << c << d;
            CC_VERIFY(c == d);
        }
    }
};

class GrayShadesOnGlass: public TestCase
{
    void run()
    {
        Color b = Color(0xFF, 0xFF, 0xFF, 0x80);
        for (int i = 0; i <= 0xFF; ++i) {
            Color a = Color(0x00, 0x00, 0x00, i);
            Color c = Color::blend(a, b);
            Color d = GrayShadesOnGlass::blend(a, b);
            if (c != d) fout("%% over %% => %% (%%)\n") << a << b << c << d;
            CC_VERIFY(differLess(c, d, 1, 1));
        }
    }

    inline static int delta(int a, int b) { return a < b ? b - a : a - b; }

    static bool differLess(Color a, Color b, int maxColorDelta, int maxOpacityDelta)
    {
        return
            delta(Color::redComponent(a),   Color::redComponent(b)  ) <= maxColorDelta &&
            delta(Color::greenComponent(a), Color::greenComponent(b)) <= maxColorDelta &&
            delta(Color::blueComponent(a),  Color::blueComponent(b) ) <= maxColorDelta &&
            delta(Color::alphaComponent(a), Color::alphaComponent(b)) <= maxOpacityDelta;
    }

    static double blendComponent(double alpha, double a, double beta, double b) {
        return
            (alpha * a + (1 - alpha) * beta * b)
            /
            (alpha + (1 - alpha) * beta);
    }

    static double blendAlpha(double alpha, double beta) {
        return alpha + (1 - alpha) * beta;
    }

    static Color blend(Color a, Color b) {
        double a_r   = Color::redComponent(a)   / 255.;
        double a_g   = Color::greenComponent(a) / 255.;
        double a_b   = Color::blueComponent(a)  / 255.;
        double alpha = Color::alphaComponent(a) / 255.;
        double b_r   = Color::redComponent(b)   / 255.;
        double b_g   = Color::greenComponent(b) / 255.;
        double b_b   = Color::blueComponent(b)  / 255.;
        double beta  = Color::alphaComponent(b) / 255.;
        return Color(
            ::round(blendComponent(alpha, a_r, beta, b_r) * 255),
            ::round(blendComponent(alpha, a_g, beta, b_g) * 255),
            ::round(blendComponent(alpha, a_b, beta, b_b) * 255),
            ::round(blendAlpha(alpha, beta) * 255)
        );
    }
};

class NormalizationTest: public TestCase
{
    void run() {
        {
            Color c { 0x80, 0x80, 0x80, 0x80 }; // 50% white premultiplied
            Color::normalize(&c);
            fout() << c << nl;
            CC_VERIFY(c == Color(0xFF, 0xFF, 0xFF, 0x80));
        }
        {
            Color c { 0x40, 0x00, 0x00, 0x40 }; // 25% red premultiplied
            Color::normalize(&c);
            fout() << c << nl;
            CC_VERIFY(c == Color(0xFF, 0x00, 0x00, 0x40));
        }
        {
            Color c { 0x00, 0x00, 0x10, 0x20 }; // 12.5% half-blue premultiplied
            Color::normalize(&c);
            fout() << c << nl;
            CC_VERIFY(
                c == Color(0x00, 0x00, 0x7F, 0x20) ||
                c == Color(0x00, 0x00, 0x80, 0x20)
            );
        }
        {
            Color c { 0x00, 0x01, 0x00, 0x01 }; // 1/255-th green premultiplied
            Color::normalize(&c);
            fout() << c << nl;
            CC_VERIFY(c == Color(0x00, 0xFF, 0x00, 0x01));
        }
        {
            Color c { 0x00, 0x00, 0x00, 0x00 }; // transparent
            Color::normalize(&c);
            fout() << c << nl;
            CC_VERIFY(c == Color(0x00, 0x00, 0x00, 0x00));
        }
        {
            Color c { 0x00, 0x00, 0xFF, 0xFF }; // opaque blue
            Color::normalize(&c);
            fout() << c << nl;
            CC_VERIFY(c == Color(0x00, 0x00, 0xFF, 0xFF));
        }
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(GrayShadesOnPaper);
    CC_TESTSUITE_ADD(GrayShadesOnGlass);
    CC_TESTSUITE_ADD(NormalizationTest);

    return TestSuite::instance()->run(argc, argv);
}

#include <cc/Color>
#include <cc/testing>
#include <cmath>

namespace test {

using namespace cc;

int delta(int a, int b) { return a < b ? b - a : a - b; }

bool differLess(Color a, Color b, int maxColorDelta, int maxOpacityDelta)
{
    return
        delta(a.red(),   b.red()  ) <= maxColorDelta &&
        delta(a.green(), b.green()) <= maxColorDelta &&
        delta(a.blue(),  b.blue() ) <= maxColorDelta &&
        delta(a.alpha(), b.alpha()) <= maxOpacityDelta;
}

double blendComponent(double alpha, double a, double beta, double b) {
    return
        (alpha * a + (1 - alpha) * beta * b)
        /
        (alpha + (1 - alpha) * beta);
}

double blendAlpha(double alpha, double beta) {
    return alpha + (1 - alpha) * beta;
}

Color blend(Color a, Color b) {
    double a_r   = a.red()   / 255.;
    double a_g   = a.green() / 255.;
    double a_b   = a.blue()  / 255.;
    double alpha = a.alpha() / 255.;
    double b_r   = b.red()   / 255.;
    double b_g   = b.green() / 255.;
    double b_b   = b.blue()  / 255.;
    double beta  = b.alpha() / 255.;
    return Color{
        std::round(blendComponent(alpha, a_r, beta, b_r) * 255),
        std::round(blendComponent(alpha, a_g, beta, b_g) * 255),
        std::round(blendComponent(alpha, a_b, beta, b_b) * 255),
        std::round(blendAlpha(alpha, beta) * 255)
    };
}

} // namespace test

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "ColorFormatting",
        []{
            CC_CHECK(str(Color{0x8080FF}) == "#8080FF");
        }
    };

    TestCase {
        "GrayShadesOnPaper",
        []{
            Color b = Color{0xFF, 0xFF, 0xFF};
            for (int i = 0; i <= 0xFF; ++i) {
                Color a = Color{0x00, 0x00, 0x00, i};
                Color c = Color::blend(a, b);
                Color d = Color{0xFF - i, 0xFF - i, 0xFF - i};
                CC_CHECK_EQUALS(c, d);
            }
        }
    };

    TestCase {
        "GrayShadesOnGlass",
        []{
            Color b = Color{0xFF, 0xFF, 0xFF, 0x80};
            for (int i = 0; i <= 0xFF; ++i) {
                Color a = Color{0x00, 0x00, 0x00, i};
                Color c = Color::blend(a, b);
                Color d = test::blend(a, b);
                if (c != d) fout("%% over %% => %% (%%)\n") << a << b << c << d;
                CC_VERIFY(test::differLess(c, d, 1, 1));
            }
        }
    };

    TestCase {
        "PremultipliedAlpha",
        []{
            CC_CHECK((Color{0x80, 0x80, 0x80, 0x80}.normalized() == Color{0xFF, 0xFF, 0xFF, 0x80})); // 50% white premultiplied
            CC_CHECK((Color{0x40, 0x00, 0x00, 0x40}.normalized() == Color{0xFF, 0x00, 0x00, 0x40})); // 25% red premultiplied
            CC_CHECK((Color{0x00, 0x01, 0x00, 0x01}.normalized() == Color{0x00, 0xFF, 0x00, 0x01})); // 1/255-th green premultiplied
            CC_CHECK((Color{0x00, 0x00, 0x00, 0x00}.normalized() == Color{0x00, 0x00, 0x00, 0x00})); // transparent
            CC_CHECK((Color{0x00, 0x00, 0xFF, 0xFF}.normalized() == Color{0x00, 0x00, 0xFF, 0xFF})); // opaque blue
            CC_CHECK((
                Color{0x00, 0x00, 0x10, 0x20}.normalized() == Color{0x00, 0x00, 0x7F, 0x20} ||
                Color{0x00, 0x00, 0x10, 0x20}.normalized() == Color{0x00, 0x00, 0x80, 0x20}
            )); // 12.5% half-blue premultiplied
        }
    };

    TestCase {
        "ColorSpaceHsv",
        []{
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

    TestCase {
        "ColorSpaceHsl",
        []{
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

    return TestSuite{argc, argv}.run();
}

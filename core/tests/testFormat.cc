/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>

using namespace cc;
using namespace cc::testing;

class FloatingPointLiterals: public TestCase
{
    void run()
    {
        const double test[] = {
            3.3, 1., 0.1, 1.1, 0., 1.234e10, 1e-308, cc::nan(), cc::inf(),
            -1./3., -0.55, 0.49, 15, -1.5, 1.1111111111,
            1.1111111111111111111111111111111,
            1e-16
        };

        for (int i = 0, n = sizeof(test) / sizeof(test[0]); i < n; ++i) {
            double x = test[i];
            bool ok = true;
            fout() << x << " => " << String(Format() << x)->toNumber<float64_t>(&ok) << " (" << ok << ")" << nl;
            fout() << (String(Format() << x)->toNumber<float64_t>() - x) << nl;
            uint64_t a = union_cast<float64_t>(x);
            uint64_t b = union_cast<float64_t>(String(Format() << x)->toNumber<float64_t>());
            fout() << "0x" << hex(a) << nl;
            fout() << "0x" << hex(b) << nl;
            fout() << oct(1, 3) << nl;
            fout() << nl;
            CC_VERIFY(a == b);
        }

        fout() << fixed(1.23, 3) << nl;
        fout() << fixed(-10.01, 4) << nl;
        fout() << fixed(0.01, 2) << nl;
        CC_VERIFY(fixed(1.23, 3) == "1.230");
        CC_VERIFY(fixed(-10.01, 4) == "-10.0100");
        CC_VERIFY(fixed(0.01, 2) == "0.01");
    }
};

class PrintingVariants: public TestCase
{
    void run()
    {
        Variant a = 1, b = true, c = "abc", d = 3.2;
        String s = Format() << a << ", " << b << ", " << c << ", " << dec(d);
        fout() << s << nl;
        CC_VERIFY(s == "1, true, abc, 3.2");
    }
};

class IntegerLiterals: public TestCase
{
    void run()
    {
        fout("%% = 0x%% = 0%% = 0b%%\n") << 123 << hex(123) << oct(123) << bin(123);
        CC_VERIFY(String(dec(123))->toNumber<int>() == 123);
        CC_VERIFY(String("0x" + hex(123))->toNumber<int>() == 123);
        CC_VERIFY(String("0" + oct(123))->toNumber<int>() == 123);
        CC_VERIFY(String("0b" + bin(123))->toNumber<int>() == 123);
    }
};

class ColorLiterals: public TestCase
{
    void run()
    {
        Color c = Color::tone(0x8080FF);
        fout() << c << nl;
        CC_VERIFY(str(c) == "#8080FFFF");
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(FloatingPointLiterals);
    CC_TESTSUITE_ADD(PrintingVariants);
    CC_TESTSUITE_ADD(IntegerLiterals);
    CC_TESTSUITE_ADD(ColorLiterals);

    return TestSuite::instance()->run(argc, argv);
}

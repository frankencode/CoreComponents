/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio>
#include <flux/check>

using namespace flux;

int main()
{
    {
        fout() << "Testing floating point numerals..." << nl;

        const double test[] = {
            3.3, 1., 0.1, 1.1, 0., 1.234e10, 1e-308, nan, inf,
            -1./3., -0.55, 0.49, 15, -1.5, 1.1111111111,
            1.1111111111111111111111111111111,
            1e-16
        };

        for (int i = 0, n = sizeof(test) / sizeof(test[0]); i < n; ++i) {
            double x = test[i];
            bool ok = true;
            fout() << x << " => " << String(Format() << x)->toFloat64(&ok) << " (" << ok << ")" << nl;
            fout() << (String(Format() << x)->toFloat64() - x) << nl;
            uint64_t a = union_cast<uint64_t>(x);
            uint64_t b = union_cast<uint64_t>(String(Format() << x)->toFloat64());
            fout() << "0x" << hex(a) << nl;
            fout() << "0x" << hex(b) << nl;
            fout() << oct(1, 3) << nl;
            fout() << nl;
            check(a == b);
        }

        fout() << fixed(1.23, 3) << nl;
        fout() << fixed(-10.01, 4) << nl;
        fout() << fixed(0.01, 2) << nl;
        check(fixed(1.23, 3) == "1.230");
        check(fixed(-10.01, 4) == "-10.0100");
        check(fixed(0.01, 2) == "0.01");
    }
    {
        fout() << "Testing variants..." << nl;
        Variant a = 1, b = true, c = "abc", d = 3.2;
        String s = Format() << a << ", " << b << ", " << c << ", " << dec(d);
        fout() << s << nl;
        check(s == "1, true, abc, 3.2");
    }
    {
        fout() << "Testing hexadecimal, octal and binary numerals..." << nl;
        fout("%% = 0x%% = 0%% = 0b%%\n") << 123 << hex(123) << oct(123) << bin(123);
        check(String(dec(123))->toInt() == 123);
        check(String("0x" + hex(123))->toInt() == 123);
        check(String("0" + oct(123))->toInt() == 123);
        check(String("0b" + bin(123))->toInt() == 123);
    }
    {
        fout() << "Testing formatting colors..." << nl;
        Color c = Color::parse("teal");
        fout() << c << nl;
        check(str(c) == "#008080");
    }
    return 0;
}

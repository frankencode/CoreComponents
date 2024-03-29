/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/str>
#include <cc/Queue>
#include <cc/Utf8Sink>
#include <cc/bits>
#include <cc/math>

namespace cc {

String hex(const Bytes &data, char a)
{
    assert(a == 'A' || a == 'a');

    long n = data.count();
    if (n == 0) return String{};

    String s2 = String::allocate(2 * n);
    long j = 0;
    for (long i = 0; i < n; ++i) {
        unsigned char ch = data.item<unsigned char>(i);
        int d0 = (ch >> 4) & 0xf;
        int d1 = ch & 0xf;
        if ((0 <= d0) && (d0 < 10)) s2[j++] = d0 + '0';
        else s2[j++] = (d0 - 10) + a;
        if ((0 <= d1) && (d1 < 10)) s2[j++] = d1 + '0';
        else s2[j++] = (d1 - 10) + a;
    }

    return s2;
}

uint8_t readHexDigit(char ch)
{
    uint8_t digit = 0;
    if ('0' <= ch && ch <= '9') digit = ch - '0';
    else if ('A' <= ch && ch <= 'F') digit = ch - 'A' + 10;
    else if ('a' <= ch && ch <= 'f') digit = ch - 'a' + 10;
    return digit;
}

Bytes readHex(const String &hex)
{
    String s = hex;

    long l = s.startsWith("0x") ? 2 : 0;
    long n = (hex.count() - l) >> 1;

    Bytes data;
    if (n <= 0) return data;

    data = Bytes::allocate(n);
    for (long i = 0; i < n; ++i) {
        data[i] = (readHexDigit(hex[l]) << 4) | readHexDigit(hex[l + 1]);
        l += 2;
    }

    return data;
}

String fnum(double x, int precision, int base, int screen)
{
    /// \todo make use of frexp

    using std::log;
    using std::pow;

    static_assert(sizeof(double) == 8);

    std::uint64_t xi = union_cast<std::uint64_t>(x);
    std::uint64_t f = (xi << 12) >> 12; // fraction
    int e = int((xi << 1) >> 53); // exponent
    int s = int(xi >> 63); // sign

    String text;
    int i = 0;

    if ((e == 0x7FF) && (f != 0)) // NaN
    {
        return "nan";
    }
    else if ((e == 0x7FF) && (f == 0)) // infinite
    {
        return s ? "-inf" : "inf";
    }
    else if ((e == 0) && (f == 0)) // zero
    {
        return "0";
    }
    else // if (((0 < e) && (e < 0x7FF)) || ((e == 0) && (f != 0))) // normalized or denormalized number
    {
        Queue<char> digits;

        bool normalized = (0 < e) && (e < 0x7FF);
        int eb = e - 1023 + int(!normalized); // exponent with bias applied

        int eba = int(roundNearestOrIn(log(pow(double(2.), eb)) / log(double(base)))); // exponent in user defined base
        double cba = pow(double(2.),double(eb)) / pow(double(base),double(eba)); // correction factor of user defined base

        uint64_t m = (uint64_t(normalized) << 52) | f; // mantissa
        const uint64_t q = uint64_t((uint64_t(1) << 52) / cba); // quotient

        // workaround to prevent leading zeros
        if (m < q) {
            m *= base;
            --eba;
        }

        int ni = 1; // number of digits of integral part
        if ((-screen <= eba) && (eba <= screen))
        {
            if (eba > 0) {
                ni += eba;
                eba = 0;
            }
            else {
                while (eba < 0) {
                    digits << 0;
                    ++eba;
                }
            }
        }

        while (digits.count() < precision) {
            int d = int(m / q);
            digits << d;
            m -= d * q;
            m *= base;
        }

        int ns = 0; // number of significiant digits
        for (auto pos = digits.head(); pos; ++pos) {
            if (digits.at(pos) != 0) ns = +pos + 1;
        }

        int wi = ni + s;
        int wf = (ns > ni) ? ns - ni : 0;

        int ne = 0; // number of exponent digits
        {
            for (int h = eba; h != 0; h /= base) ++ne;
        }

        text = String::allocate(wi + int(wf != 0) + wf + int(ne != 0) * (1 + int(eba < 0) + ne), ' ');

        if (s == 1)
            text.at(i++) = '-';

        const char *fig = "0123456789abcdef";

        auto di = digits.head(); // digit iterator

        for (int l = 0; l < ni; ++l)
            text.at(i++) = fig[int(digits.at(di++))];

        if (wf != 0)
        {
            text.at(i++) = '.';
            for (int l = 0; l < wf; ++l)
            {
                if (!di)
                    text.at(i++) = '0';
                else
                    text.at(i++) = fig[int(digits.at(di++))];
            }
        }

        if (ne != 0)
        {
            text.at(i++) = 'e';
            if (eba < 0) { text.at(i++) = '-'; eba = -eba; }
            for (int l = ne-1, h = eba; l >= 0; --l, h /= base)
                text.at(i+l) = fig[h % base];
            i += ne;
        }
    }

    return text;
}

String fixed(double x, int nf)
{
    return fixed(x, 0, nf);
}

String fixed(double x, int ni, int nf)
{
    if (x != x) return "nan";
    if (x == +1.0/0.0) return "inf";
    if (x == -1.0/0.0) return "-inf";

    double ip;
    double fp = std::modf(x, &ip);
    if (fp < 0) fp = -fp;

    {
        double q = 1;
        for (int i = 0; i < nf; ++i) q *= 10;
        fp = std::round(q * fp);
        if (fp >= q) {
            if (ip < 0) --ip;
            else ++ip;
            fp -= q;
        }
    }

    String sip = inum(std::int64_t(ip));
    if (ni > sip.count()) sip = sip.alignedRight(ni);
    if (nf <= 0) return sip;

    String s = String::allocate(sip.count() + 1 + nf, '.');
    sip.copyTo(&s);
    inum(std::uint64_t(fp)).alignedRight(nf, '0').copyToRange(&s, sip.count() + 1, s.count());
    return s;
}

String str(const void *x)
{
    if (sizeof(void *) == 8)
        return dec(union_cast<std::uint64_t>(x));

    return dec(union_cast<std::uint32_t>(x));
}

String str(char32_t ch)
{
    String s = String::allocate(4);
    Utf8Sink sink{s};
    sink.write(ch);
    s.truncate(sink.currentOffset());
    return s;
}

} // namespace cc

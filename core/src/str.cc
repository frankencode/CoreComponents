/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Stack>
#include <cc/rounding>
#include <cc/Utf8Sink>
#include <cc/str>

namespace cc {

String fnum(float64_t x, int precision, int base, int screen)
{
    Ref< Stack<int> > digits = Stack<int>::create(2 * screen < 128 ? 128 : 2 * screen /*save bet*/);

    uint64_t xi = union_cast<uint64_t>(x);
    uint64_t f = (xi << 12) >> 12; // fraction
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
        bool normalized = (0 < e) && (e < 0x7FF);
        int eb = e - 1023 + int(!normalized); // exponent with bias applied

        int eba = int(roundToZero(log(pow(float64_t(2.), eb)) / log(float64_t(base)))); // exponent in user defined base
        float64_t cba = pow(float64_t(2.),float64_t(eb)) / pow(float64_t(base),float64_t(eba)); // correction factor of user defined base

        uint64_t m = (uint64_t(normalized) << 52) | f; // mantissa
        const uint64_t q = uint64_t((uint64_t(1) << 52) / cba); // quotient

        // workaround to prevent leading zeros
        if (m < q) {
            m *= base;
            --eba;
        }

        digits->deplete();

        int ni = 1; // number of digits of integral part
        if ((-screen <= eba) && (eba <= screen))
        {
            if (eba > 0) {
                ni += eba;
                eba = 0;
            }
            else {
                while (eba < 0) {
                    digits->push(0);
                    ++eba;
                }
            }
        }

        while (digits->count() < precision) {
            int d = int(m / q);
            digits->push(d);
            m -= d * q;
            m *= base;
        }

        int ns = 0; // number of significiant digits
        {
            for (int i = 0; i < digits->count(); ++i)
                if (digits->bottom(i) != 0)
                    ns = i + 1;
        }

        int wi = ni + s;
        int wf = (ns > ni) ? ns - ni : 0;

        int ne = 0; // number of exponent digits
        {
            for (int h = eba; h != 0; h /= base) ++ne;
        }

        text = String(wi + int(wf != 0) + wf + int(ne != 0) * (1 + int(eba < 0) + ne), ' ');

        if (s == 1)
            mutate(text)->at(i++) = '-';

        const char *fig = "0123456789abcdef";
        int k = 0; // digit index

        for (int l = 0; l < ni; ++l)
            mutate(text)->at(i++) = fig[digits->bottom(k++)];

        if (wf != 0)
        {
            mutate(text)->at(i++) = '.';
            for (int l = 0; l < wf; ++l)
            {
                if (digits->count() <= k)
                    mutate(text)->at(i++) = '0';
                else
                    mutate(text)->at(i++) = fig[digits->bottom(k++)];
            }
        }

        if (ne != 0)
        {
            mutate(text)->at(i++) = 'e';
            if (eba < 0) { mutate(text)->at(i++) = '-'; eba = -eba; }
            for (int l = ne-1, h = eba; l >= 0; --l, h /= base)
                mutate(text)->at(i+l) = fig[h % base];
            i += ne;
        }
    }

    return text;
}

String fixed(float64_t x, int nf)
{
    // FIXME: the following 3 compares are slightly inefficient
    if (x != x) return "nan";
    if (x == +1.0/0.0) return "inf";
    if (x == -1.0/0.0) return "-inf";
    double ip;
    double fp = modf(x, &ip);
    String sip = inum(int64_t(ip));
    if (nf <= 0) return sip;
    String s(sip->count() + 1 + nf, '.');
    mutate(s)->write(sip);
    if (fp < 0) fp = -fp;
    for (int i = 0; i < nf; ++i) fp *= 10;
    fp = round(fp);
    mutate(s)->write(right(inum(uint64_t(fp)), nf, '0'), sip->count() + 1, s->count());
    return s;
}

String dec(const Variant &x, int n)
{
    return (Variant::type(x) == Variant::FloatType) ?
        dec(float(x), n > 0 ? n : 7) :
        dec(int(x), n);
}

String str(uchar_t ch)
{
    String s(4);
    Ref<Utf8Sink> sink = Utf8Sink::open(mutate(s));
    sink->write(ch);
    mutate(s)->truncate(sink->byteSink()->currentOffset());
    return s;
}

String str(void *x)
{
    if (sizeof(void *) == sizeof(uint64_t))
        return dec(union_cast<uint64_t>(x));

    return dec(union_cast<uint32_t>(x));
}

String left(const String &s, int w, char blank)
{
    if (s->count() > w) return s;
    else return s + String(w - s->count(), blank);
}

String right(const String &s, int w, char blank)
{
    if (s->count() > w) return s;
    else return String(w - s->count(), blank) + s;
}

} // namespace cc

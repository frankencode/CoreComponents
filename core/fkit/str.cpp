/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "Stack.h"
#include "rounding.h"
#include "str.h"

namespace fkit
{

String str(const Variant &x)
{
	if (type(x) == Variant::UndefType) return "undef";
	else if (type(x) == Variant::BoolType) return str(bool(x));
	else if (type(x) == Variant::IntType) return str(int(x));
	else if (type(x) == Variant::FloatType) return str(float(x));
	else if (type(x) == Variant::StringType) return String(x);
	/*else if (type(x) == Variant::ObjectType)*/ return str((void *)cast<Object>(x));
}

String fnum(float64_t x, int precision, int base, int screen)
{
	Ref< Stack<int> > digits = Stack<int>::create(screen < 128 ? 128 : 2 * screen /*save bet*/);

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

		digits->clear();

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

		while (digits->fill() < precision) {
			int d = int(m / q);
			digits->push(d);
			m -= d * q;
			m *= base;
		}

		int ns = 0; // number of significiant digits
		{
			for (int i = 0; i < digits->fill(); ++i)
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
			text->set(i++, '-');

		const char *fig = "0123456789abcdef";
		int k = 0; // digit index

		for (int l = 0; l < ni; ++l)
			text->set(i++, fig[digits->bottom(k++)]);

		if (wf != 0)
		{
			text->set(i++, '.');
			for (int l = 0; l < wf; ++l)
			{
				if (digits->fill() <= k)
					text->set(i++, '0');
				else
					text->set(i++, fig[digits->bottom(k++)]);
			}
		}

		if (ne != 0)
		{
			text->set(i++, 'e');
			if (eba < 0) { text->set(i++, '-'); eba = -eba; }
			for (int l = ne-1, h = eba; l >= 0; --l, h /= base)
				text->set(i+l, fig[h % base]);
			i += ne;
		}
	}

	return text;
}

String dec(const Variant &x, int m)
{
	return (type(x) == Variant::FloatType) ?
		dec(float(x), m > 0 ? m : 7) :
		dec(int(x), m);
}

String str(void *x)
{
	if (sizeof(void *) == sizeof(uint64_t))
		return dec(union_cast<uint64_t>(x));

	return dec(union_cast<uint32_t>(x));
}

String left(const String &s, int w, char blank)
{
	if (s->length() > w) return s;
	else return s + String(w - s->length(), blank);
}

String right(const String &s, int w, char blank)
{
	if (s->length() > w) return s;
	else return String(w - s->length(), blank) + s;
}

} // namespace fkit

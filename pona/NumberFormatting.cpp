/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstraction Library.
**
** Copyright (C) 2007-2009  Frank Mertens
**
** This file is part of a free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License,
** or (at your option) any later version.
**
** The library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this libary.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "Stack.hpp"
#include "Rounding.hpp"
#include "SyntaxFactory.hpp"
#include "BooleanLiteral.hpp"
#include "IntegerLiteral.hpp"
#include "FloatLiteral.hpp"
#include "NumberFormatting.hpp"

namespace pona
{

bool parseBool(String text, int index, int* length)
{
	bool value = false;
	int i1 = 0;
	
	if (syntaxFactory()->booleanLiteral()->match(text, index, &i1, &value))
	{
		if (length) *length = i1 - index;
	}
	else
	{
		if (length) *length = 0;
	}
	
	return value;
}

String printInteger(uint64_t value, int sign, int base, int width)
{
	Stack<int> digits(256);
	
	if (value == 0)
	{
		digits.push(0);
	}
	else
	{
		uint64_t v = value;
		for (int i = 0; v > 0; ++i)
		{
			if (i == digits.size())
				PONA_THROW(FormattingException, "Printing integer exceeds maximum number of digits");
			digits.push(int(v % base));
			v /= base;
		}
	}
	
	if (sign == -1)
		digits.push(16);
	
	if (width > 0)
		if (digits.fill() > width)
			PONA_THROW(FormattingException, "Printing integer exceeds width");
	
	String s;
	s->push(0, width > digits.fill() ? width : digits.fill());
	int k = 0;
	
	for (int i = digits.fill(); i < width; ++i)
		s->set(k++, ' ');
	
	while (digits.fill() > 0)
	{
		const char* letters = "0123456789ABCDEF-";
		int digit = digits.pop();
		s->set(k++, letters[digit]);
	}
	
	return s;
}

String printFloatingPoint(float64_t x, int base, int wi, int wf, int w, Char fill)
{
	String text;
	
	if ( ! ( ((2 <= base) && (base <= 16)) &&
	         ((0 <= wi) && (wi <= 16)) &&
	         ((0 <= wf) && (wf <= 16)) &&
	         ((0 <= w) && (w <= 64)) &&
	         (fill != 0) ) )
		PONA_THROW(FormattingException, "Invalid argument");
	
	const char* digits = "0123456789ABCDEF";
	
	uint64_t xi = union_cast<uint64_t>(x);
	uint64_t f = (xi << 12) >> 12; // fraction
	int e = int((xi << 1) >> 53); // exponent
	int s = int(xi >> 63); // sign
	
	if ((e == 0x7FF) && (f != 0)) // NaN
	{
		if (w == 0) w = 3;
		text = w * Char(' ');
		const char* nan = "NaN";
		for (int k = 0; k < 3; ++k)
			text->set(k, nan[k]);
	}
	else if ((e == 0x7FF) && (f == 0)) // infinite
	{
		if (w == 0) w = 8 + s;
		text = w * Char(' ');
		const char* inf = "INFINITE";
		int i = 0;
		if (s == 1) text->set(i++, '-');
		for (int k = 0; k < 8; ++k)
			text->set(i++, inf[k]);
	}
	else if (((0 < e) && (e < 0x7FF)) || ((e == 0) && (f != 0))) // normalized or denormalized number
	{
		// ---------------------------------------------------------------
		// convert significiant to a series of digits
		// ---------------------------------------------------------------
		
		bool normalized = (0 < e) && (e < 0x7FF);
		int eb = e - 1023 + int(!normalized); // exponent with bias applied
		
		int eba = int(roundToZero(log(pow(float64_t(2.), eb)) / log(float64_t(base)))); // exponent in user defined base
		float64_t cba = pow(float64_t(2.),float64_t(eb)) / pow(float64_t(base),float64_t(eba)); // correction factor of user defined base
		
		uint64_t m = (uint64_t(normalized) << 52) | f; // mantissa
		const uint64_t q = uint64_t((uint64_t(1) << 52) / cba); // !open issue: remainder unchecked
		
		if (m < q) // workaround to prevent leading zeros
		{
			m *= base;
			--eba;
		}
		
		int ned = int(ceilToInf(log(float64_t((uint64_t(1)<<52)))/log(float64_t(base))) - 1); // number of exact digits
		
		Stack<int> stack(ned);
		
		while (!stack.isFull())
		{
			int d = int(m / q);
			stack.push(d);
			m -= d * q;
			m *= base;
		}
		
		// ---------------------------------------------------------------
		// output digits
		// ---------------------------------------------------------------
		
		int ns = 0; // number of significiant digits
		{
			for (int i = 0; i < stack.fill(); ++i)
				if (stack.bottom(i) != 0) ns = i + 1;
		}
		
		int ni = 1;
		if (eba >= 0) {
			ni += eba;
			eba = 0;
		}
		
		if (wi < ni + s) wi = ni + s;
		if (wf == 0) wf = (ns > ni) ? ns - ni : 0;
		
		int ne = 0; // number of exponent digits
		{
			for (int h = eba; h != 0; h /= base) ++ne;
		}
		
		int h = wi + int(wf != 0) + wf + int(ne != 0) * (1 + int(eba < 0) + ne);
		if (w < h) w = h; // w too small
		
		text = w * fill;

		int i = 0; // output index
		int k = 0; // digit index
		
		// DEBUG
		//printf("ned = %d, w = %d, wi = %d, wf = %d, ni = %d, ne = %d, eba = %d\n", ned, w, wi, wf, ni, ne, eba);
		
		for (int l = 0; l < wi-ni-s; ++l)
			text->set(i++, fill);
		
		if (s == 1)
			text->set(i++, '-');
		
		for (int l = 0; l < ni; ++l)
			text->set(i++, digits[stack.bottom(k++)]);
		
		if (wf != 0)
		{
			text->set(i++, '.');
			for (int l = 0; l < wf; ++l)
			{
				if (stack.isEmpty())
					text->set(i++, '0');
				else
					text->set(i++, digits[stack.bottom(k++)]);
			}
		}
		
		if (ne != 0)
		{
			text->set(i++, 'e');
			if (eba < 0) { text->set(i++, '-'); eba = -eba; }
			for (int l = ne-1, h = eba; l >= 0; --l, h /= 10)
				text->set(i+l, digits[h % 10]);
			i += ne;
		}
	}
	else // if ((e == 0) && (f == 0)) // zero
	{
		if (wi == 0) wi = 1;
		if (w < wi) w = wi;
		text = w * Char(' ');
		text->set(wi-1, '0');
	}
	
	return text;
}

void parseInteger(uint64_t* value, int* sign, String text, int index, int* length)
{
	int i1 = 0;
	
	if (syntaxFactory()->integerLiteral()->match(text, index, &i1, value, sign))
	{
		if (length) *length = i1 - index;
	}
	else
	{
		if (length) *length = 0;
	}
}

int toInt(String text, bool* ok)
{
	uint64_t value; int sign;
	int length = 0;
	parseInteger(&value, &sign, text, 0, &length);
	
	if (length == 0) {
		if (ok) *ok = false;
	}
	else if (value > uint64_t(intMax)) {
		value = 0;
		if (ok) *ok = false;
	}
	
	return sign * int(value);
}

float64_t parseFloatingPoint(String text, int index, int* length)
{
	float64_t value = 0.;
	int i1 = 0;
	
	if (syntaxFactory()->floatingPointLiteral()->match(text, index, &i1, &value))
	{
		if (length) *length = i1 - index;
	}
	else
	{
		if (length) *length = 0;
	}
	
	return value;
}

} // namespace pona

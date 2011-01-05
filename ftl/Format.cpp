/*
 * Format.cpp -- formated strings
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include "FormatSyntax.hpp"
#include "FormatSpecifier.hpp"
#include "Format.hpp"

namespace ftl
{

Format::Format(String format)
	: Super(new StringList)
{
	Ref<PlaceHolder> lastPlaceHolder;
	
	if (format->contains('%'))
	{
		Ref<FormatSpecifier> specifier = formatSyntax()->formatSpecifier();
		int i0Saved = 0, i0 = 0, i1 = 0;
		
		int nph = 0; // number of placeholders
		
		while (true) {
			Ref<PlaceHolder, Owner> ph = new PlaceHolder;
			
			if (!specifier->find(format, &i0, &i1, &ph->w_, &ph->wi_, &ph->wf_, &ph->base_, &ph->exp_, &ph->blank_)) break;
			
			if (i0 != i0Saved)
				get()->append(format->copy(i0Saved, i0));
			else
				get()->append(String());
			
			ph->j_ = get()->length() + nph;
			ph->check();
			if (lastPlaceHolder)
				lastPlaceHolder->next_ = ph;
			else
				placeHolders_ = ph;
			lastPlaceHolder = ph;
			++nph;
			
			i0 = i1;
			i0Saved = i0;
		}
		if (i0Saved < format->size())
			get()->append(format->copy(i0Saved, format->size()));
	}
	else if (!format->isEmpty()) {
		get()->append(format);
	}
	
	// add default placeholder
	if (lastPlaceHolder)
		lastPlaceHolder->next_ = new PlaceHolder;
	else
		placeHolders_ = new PlaceHolder;
}

Format::Format(const Format& b)
	: Super(b.get()),
	  placeHolders_(b.placeHolders_)
{}

Format& Format::operator=(const Format& b)
{
	set(b.get());
	placeHolders_ = b.placeHolders_;
	return *this;
}

Ref<Format::PlaceHolder, Owner> Format::nextPlaceHolder()
{
	Ref<PlaceHolder, Owner> ph = placeHolders_;
	if (placeHolders_->next_)
		placeHolders_ = placeHolders_->next_;
	else
		ph->j_ = get()->length();
	return ph;
}

Format& Format::print(const Variant& x)
{
	if (x.type() == Variant::UndefType)
		print("undef");
	else if (x.type() == Variant::BoolType)
		print(bool(x));
	else if (x.type() == Variant::IntType)
		print(int(x));
	else if (x.type() == Variant::FloatType)
		print(double(x));
	else if (x.type() == Variant::StringType)
		print(String(x));
	else if (x.type() == Variant::RefType)
		print(Ref<>(x).get());
	return *this;
}

void Format::printInt(uint64_t x, int sign)
{
	Ref<PlaceHolder, Owner> ph = nextPlaceHolder();
	
	int buf[MaxDigits];
	Stack<int> digits(buf, MaxDigits);
	
	digits.clear();
	if (x == 0) {
		digits.push(0);
	}
	else {
		uint64_t v = x;
		for (int i = 0; v > 0; ++i) {
			digits.push(int(v % ph->base_));
			v /= ph->base_;
		}
	}
	if (sign == -1)
		digits.push(16);
	
	int wi = (ph->wi_ > 0) ? ph->wi_ : 1;
	int wf = ph->wf_;
	int w = (sign == -1) + wi + (wf != 0) + wf;
	if (w < ph->w_) w = ph->w_;
	if (wi < digits.fill()) wi = digits.fill();
	if (w < wi) w = wi;
	
	String text = String::initialized(w, ph->blank_);
	int i = 0;
	
	int wb = 0; // width of blank
	if (wi != 0)
		wb = wi - digits.fill();
	else if (wf != 0)
		wb = w - digits.fill() - wf - 1;
	else
		wb = w - digits.fill();
	
	if (wb > 0) i += wb;
	
	while (digits.fill() > 0) {
		const char* letters = "0123456789ABCDEF-";
		int d = digits.pop();
		text->set(i++, letters[d]);
	}
	
	get()->insert(get()->index(ph->j_), text);
}

void Format::printFloat(float64_t x)
{
	Ref<PlaceHolder, Owner> ph = nextPlaceHolder();
	
	int buf[MaxDigits];
	Stack<int> digits(buf, MaxDigits);
	
	int wi = (ph->wi_ == 0) ? 1 : ph->wi_;
	int wf = ph->wf_;
	int w = wi + (wf != 0) + wf;
	if (w < ph->w_) w = ph->w_;
	
	uint64_t xi = union_cast<uint64_t>(x);
	uint64_t f = (xi << 12) >> 12; // fraction
	int e = int((xi << 1) >> 53); // exponent
	int s = int(xi >> 63); // sign
	
	String text;
	int i = 0;
	
	if ((e == 0x7FF) && (f != 0)) // NaN
	{
		if (w < 3) w = 3;
		text = String::initialized(w, ph->blank_);
		int wb = w - 3;
		if (wb > 0)
			i += wb;
		const char* nan = "nan";
		while (*nan) {
			text->set(i++, *nan);
			++nan;
		}
	}
	else if ((e == 0x7FF) && (f == 0)) // infinite
	{
		if (w < 3 + s) w = 3 + s;
		text = String::initialized(w, ph->blank_);
		int wb = w - 3 - s;
		if (wb > 0)
			i += wb;
		if (s == 1) text->set(i++, '-');
		const char* inf = "inf";
		while (*inf) {
			text->set(i++, *inf);
			++inf;
		}
	}
	else if (((0 < e) && (e < 0x7FF)) || ((e == 0) && (f != 0))) // normalized or denormalized number
	{
		bool normalized = (0 < e) && (e < 0x7FF);
		int eb = e - 1023 + int(!normalized); // exponent with bias applied
		
		int eba = int(roundToZero(log(pow(float64_t(2.), eb)) / log(float64_t(ph->base_)))); // exponent in user defined base
		float64_t cba = pow(float64_t(2.),float64_t(eb)) / pow(float64_t(ph->base_),float64_t(eba)); // correction factor of user defined base
		
		uint64_t m = (uint64_t(normalized) << 52) | f; // mantissa
		const uint64_t q = uint64_t((uint64_t(1) << 52) / cba); // quotient
		
		// workaround to prevent leading zeros
		if (m < q) {
			m *= ph->base_;
			--eba;
		}
		
		int ned = int(ceilToInf(log(float64_t((uint64_t(1)<<52)))/log(float64_t(ph->base_))) - 1); // number of exact digits
		
		digits.clear();
		
		int ni = 1; // number of digits of integral part
		if (!ph->exp_) {
			if ((!ph->exp_) && (-ExpAutoLimit <= eba) && (eba <= ExpAutoLimit))
			{
				if (eba > 0) {
					ni += eba;
					eba = 0;
				}
				else {
					while (eba < 0) {
						digits.push(0);
						++eba;
					}
				}
			}
		}
		
		while (digits.fill() < ned) {
			int d = int(m / q);
			digits.push(d);
			m -= d * q;
			m *= ph->base_;
		}
		
		int ns = 0; // number of significiant digits
		{
			for (int i = 0; i < digits.fill(); ++i)
				if (digits.bottom(i) != 0)
					ns = i + 1;
		}
		
		if (wi < ni + s) wi = ni + s;
		if (wf == 0) wf = (ns > ni) ? ns - ni : 0;
		
		int ne = 0; // number of exponent digits
		{
			for (int h = eba; h != 0; h /= ph->base_) ++ne;
		}
		
		int h = wi + int(wf != 0) + wf + int(ne != 0) * (1 + int(eba < 0) + ne);
		if (w < h) w = h; // w too small
		
		text = String::initialized(w, ph->blank_);
		
		int wb = wi - ni - s;
		if (wb > 0) i += wb;
		
		if (s == 1)
			text->set(i++, '-');
		
		const char* letters = "0123456789ABCDEF";
		int k = 0; // digit index
		
		for (int l = 0; l < ni; ++l)
			text->set(i++, letters[digits.bottom(k++)]);
		
		if (wf != 0)
		{
			text->set(i++, '.');
			for (int l = 0; l < wf; ++l)
			{
				if (digits.fill() <= k)
					text->set(i++, '0');
				else
					text->set(i++, letters[digits.bottom(k++)]);
			}
		}
		
		if (ne != 0)
		{
			text->set(i++, 'e');
			if (eba < 0) { text->set(i++, '-'); eba = -eba; }
			for (int l = ne-1, h = eba; l >= 0; --l, h /= ph->base_)
				text->set(i+l, letters[h % ph->base_]);
			i += ne;
		}
	}
	else // if ((e == 0) && (f == 0)) // zero
	{
		text = String::initialized(w, ph->blank_);
		text->set(i + wi - 1, '0');
	}
	
	get()->insert(get()->index(ph->j_), text);
}

} // namespace ftl

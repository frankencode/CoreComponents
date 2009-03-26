#include "Rounding.hpp"
#include "SyntaxFactory.hpp"
#include "FormatSpecifier.hpp"
#include "Format.hpp"

namespace pona
{

Format::Format()
	: defaultPlaceHolder_(new PlaceHolder),
	  digits_(128 /* safe guess, 65 + ExpAutoLimit should be sufficient ? */ ),
	  n0_(0)
{}

Format::Format(String templateText)
	: String(templateText),
	  defaultPlaceHolder_(new PlaceHolder),
	  digits_(128),
	  n0_(get()->length())
{
	Ref<FormatSpecifier> specifier = syntaxFactory()->formatSpecifier();
	int i0Saved = 0, i0 = 0, i1 = 0;
	int m = 0; // accumulated width of place holders
	
	Ref<PlaceHolder> tail;
	String text;
	
	while (true)
	{
		Ref<PlaceHolder, Owner> ph = new PlaceHolder;
		
		if (!specifier->find(*this, &i0, &i1, &ph->w_, &ph->wi_, &ph->wf_, &ph->base_, &ph->exp_, &ph->blank_)) break;
		
		ph->i_ = i0 - m;
		ph->check();
		
		if (i0 != i0Saved)
			text->append(get()->range(i0Saved, i0 - i0Saved));
		if (tail)
			tail->next_ = ph;
		else
			placeHolders_ = ph;
		tail = ph;
		m += i1 - i0;
		i0 = i1;
		i0Saved = i0;
	}
	
	if (i0Saved != n0_)
		text->append(get()->range(i0Saved, n0_ - i0Saved));
	
	set(text);
	n0_ -= m;
	defaultPlaceHolder_->i_ = n0_;
}

Ref<Format::PlaceHolder, Owner> Format::nextPlaceHolder()
{
	Ref<PlaceHolder, Owner> ph;
	if (placeHolders_) {
		ph = placeHolders_;
		placeHolders_ = placeHolders_->next_;
	}
	else {
		ph = defaultPlaceHolder_;
	}
	return ph;
}

Format& Format::print(Variant x)
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
	String text = *this;
	
	digits_.clear();
	if (x == 0) {
		digits_.push(0);
	}
	else {
		uint64_t v = x;
		for (int i = 0; v > 0; ++i) {
			digits_.push(int(v % ph->base_));
			v /= ph->base_;
		}
	}
	if (sign == -1)
		digits_.push(16);
	
	int wi = (ph->wi_ > 0) ? ph->wi_ : 1;
	int wf = ph->wf_;
	int w = (sign == -1) + wi + (wf != 0) + wf;
	if (w < ph->w_) w = ph->w_;
	if (wi < digits_.fill()) wi = digits_.fill();
	if (w < wi) w = wi;
	
	int i = ph->i_ + text->length() - n0_;
	text->push(i, w);
	text->fill(i, w, ph->blank_);
	
	if (wi != 0) {
		int wb = wi - digits_.fill();
		if (wb > 0)
			i += wb;
	}
	else if (wf != 0) {
		int wb = w - digits_.fill() - wf - 1;
		if (wb > 0)
			i += wb;
	}
	else {
		int wb = w - digits_.fill();
		if (wb > 0)
			i += wb;
	}
	
	while (digits_.fill() > 0) {
		const char* letters = "0123456789ABCDEF-";
		int d = digits_.pop();
		text->set(i++, letters[d]);
	}
}

void Format::printFloat(float64_t x)
{
	Ref<PlaceHolder, Owner> ph = nextPlaceHolder();
	String text = *this;
	
	int wi = (ph->wi_ == 0) ? 1 : ph->wi_;
	int wf = ph->wf_;
	int w = wi + (wf != 0) + wf;
	if (w < ph->w_) w = ph->w_;
	
	int i = ph->i_ + text->length() - n0_; // output index

	uint64_t xi = union_cast<uint64_t>(x);
	uint64_t f = (xi << 12) >> 12; // fraction
	int e = int((xi << 1) >> 53); // exponent
	int s = int(xi >> 63); // sign
	
	if ((e == 0x7FF) && (f != 0)) // NaN
	{
		if (w < 3) w = 3;
		text->push(i, w);
		int wb = w - 3;
		if (wb > 0) {
			text->fill(i, wb, ph->blank_);
			i += wb;
		}
		const char* nan = "nan";
		while (*nan) {
			text->set(i++, *nan);
			++nan;
		}
	}
	else if ((e == 0x7FF) && (f == 0)) // infinite
	{
		if (w < 3 + s) w = 3 + s;
		text->push(i, w);
		int wb = w - 3 - s;
		if (wb > 0) {
			text->fill(i, wb, ph->blank_);
			i += wb;
		}
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
		
		digits_.clear();
		
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
						digits_.push(0);
						++eba;
					}
				}
			}
		}
		
		while (digits_.fill() < ned) {
			int d = int(m / q);
			digits_.push(d);
			m -= d * q;
			m *= ph->base_;
		}
		
		int ns = 0; // number of significiant digits
		{
			for (int i = 0; i < digits_.fill(); ++i)
				if (digits_.bottom(i) != 0)
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
		
		text->push(i, w);
		text->fill(i, w, ph->blank_);
		
		int wb = wi - ni - s;
		if (wb > 0) i += wb;
		
		if (s == 1)
			text->set(i++, '-');
		
		const char* letters = "0123456789ABCDEF";
		int k = 0; // digit index
		
		for (int l = 0; l < ni; ++l)
			text->set(i++, letters[digits_.bottom(k++)]);
		
		if (wf != 0)
		{
			text->set(i++, '.');
			for (int l = 0; l < wf; ++l)
			{
				if (digits_.fill() <= k)
					text->set(i++, '0');
				else
					text->set(i++, letters[digits_.bottom(k++)]);
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
		text->push(i, w);
		text->fill(i, w, ph->blank_);
		text->set(i + wi - 1, '0');
	}
}

} // namespace pona

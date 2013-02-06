 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_FORMAT_HPP
#define FTL_FORMAT_HPP

#include "string.hpp"
#include "variant.hpp"
#include "rounding.hpp"
#include "Stack.hpp"

namespace ftl
{

FTL_EXCEPTION(FormatException, Exception);

class format: public Ref<StringList>
{
public:
	typedef Ref<StringList> Super;

	format(string format = "");

	format(const format &b);
	format &operator=(const format &b);

	format &operator<<(const string &s);
	inline format &operator<<(const Ref<ByteArray> &s) { return *this << string(s); }
	inline format &operator<<(ByteArray *s) { return *this << string(s); }
	inline format &operator<<(const char *s) { return *this << string(s); }
	inline format &operator<<(char *s) { return *this << string(s); }
	inline format &operator<<(char ch) { return *this << string(&ch, 1); }

	inline format &operator<<(unsigned char x) { printInt(x); return *this; }
	inline format &operator<<(unsigned short x) { printInt(x); return *this; }
	inline format &operator<<(unsigned int x) { printInt(x); return *this; }
	inline format &operator<<(unsigned long x) { printInt(x); return *this; }
	inline format &operator<<(unsigned long long x) { printInt(x); return *this; }

	inline format &operator<<(short x) { int s = sign(x); printInt(uint64_t(s*x), s); return *this; }
	inline format &operator<<(int x) { int s = sign(x); printInt(uint64_t(s*x), s); return *this; }
	inline format &operator<<(long x) { int s = sign(x); printInt(uint64_t(s*x), s); return *this; }
	inline format &operator<<(long long x) { int s = sign(x); printInt(uint64_t(s*x), s); return *this; }

	inline format &operator<<(float x) { printFloat(x, true); return *this; }
	inline format &operator<<(double x) { printFloat(x); return *this; }

	inline format &operator<<(bool x) { return *this << (x ? "true" : "false"); }
	inline format &operator<<(void *x) {
		if (sizeof(void *) == sizeof(uint32_t))
			printInt(union_cast<uint32_t>(x));
		else if (sizeof(void *) == sizeof(uint64_t))
			printInt(union_cast<uint64_t>(x)); return *this;
		return *this;
	}

	format &operator<<(const variant &x);

	template<class T>
	inline format &operator<<(const T &x) { return *this << string(x); }

	template<class T>
	inline format &operator<<(const Ref<T> &x) { return *this << string(*x); }

	inline format &operator<<(const Ref<StringList> &x) { return *this << x->join(""); }

	template<class T>
	inline format &operator<<(T *x) { return *this << (void *)x; }

private:
	enum { ExpAutoLimit = 6 };
	enum { MaxDigits = 128 }; // safe guess, 65 + ExpAutoLimit should be sufficient

	void init();

	class PlaceHolder: public Instance {
	public:
		PlaceHolder()
			: j_(0), w_(0), wi_(0), wf_(0), base_(10), exp_(false), blank_(' ')
		{}

		void check() {
			if ( ! ( ((2 <= base_) && (base_ <= 16)) &&
			         ((0 <= wi_) && (wi_ <= 16)) &&
			         ((0 <= wf_) && (wf_ <= 16)) &&
			         (0 <= w_) && (blank_ >= 32) ) )
				FTL_THROW(FormatException, "Invalid format specifier");
		}

		int j_; // insertion point
		int w_, wi_, wf_, base_;
		bool exp_;
		char blank_;
		Ref<PlaceHolder> next_;
	};

	Ref<PlaceHolder> nextPlaceHolder();

	void printInt(uint64_t x, int sign = 1);
	void printFloat(float64_t x, bool half = false);

	Ref<PlaceHolder> placeHolders_;
};

} // namespace ftl

#endif // F

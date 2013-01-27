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

#include "atoms"
#include "String.hpp"
#include "Variant.hpp"
#include "rounding.hpp"
#include "Stack.hpp"

namespace ftl
{

FTL_EXCEPTION(FormatException, Exception);

class Format: public O<StringList>
{
public:
	typedef O<StringList> Super;

	Format(String format = "");

	Format(const Format &b);
	Format &operator=(const Format &b);

	Format &operator<<(const String &s);
	inline Format &operator<<(const O<ByteArray> &s) { return *this << String(s); }
	inline Format &operator<<(const char *s) { return *this << String(s); }
	inline Format &operator<<(char *s) { return *this << String(s); }
	inline Format &operator<<(char ch) { return *this << String(&ch, 1); }

	inline Format &operator<<(unsigned char x) { printInt(x); return *this; }
	inline Format &operator<<(unsigned short x) { printInt(x); return *this; }
	inline Format &operator<<(unsigned int x) { printInt(x); return *this; }
	inline Format &operator<<(unsigned long x) { printInt(x); return *this; }
	inline Format &operator<<(unsigned long long x) { printInt(x); return *this; }

	inline Format &operator<<(short x) { int s = sign(x); printInt(uint64_t(s*x), s); return *this; }
	inline Format &operator<<(int x) { int s = sign(x); printInt(uint64_t(s*x), s); return *this; }
	inline Format &operator<<(long x) { int s = sign(x); printInt(uint64_t(s*x), s); return *this; }
	inline Format &operator<<(long long x) { int s = sign(x); printInt(uint64_t(s*x), s); return *this; }

	inline Format &operator<<(float x) { printFloat(x, true); return *this; }
	inline Format &operator<<(double x) { printFloat(x); return *this; }

	inline Format &operator<<(bool x) { return *this << (x ? "true" : "false"); }
	inline Format &operator<<(void *x) {
		if (sizeof(void *) == sizeof(uint32_t))
			printInt(union_cast<uint32_t>(x));
		else if (sizeof(void *) == sizeof(uint64_t))
			printInt(union_cast<uint64_t>(x)); return *this;
		return *this;
	}

	Format &operator<<(const Variant &x);

	template<class T>
	inline Format &operator<<(const T &x) { return *this << x.toString(); }

	template<class T>
	inline Format &operator<<(const O<T> &x) { return *this << x->toString(); }

	inline Format &operator<<(const O<StringList> &x) { return *this << x->join(""); }

	template<class T>
	inline Format &operator<<(T *x) { return *this << (void *)x; }

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
		O<PlaceHolder> next_;
	};

	O<PlaceHolder> nextPlaceHolder();

	void printInt(uint64_t x, int sign = 1);
	void printFloat(float64_t x, bool half = false);

	O<PlaceHolder> placeHolders_;
};

} // namespace ftl

#endif // FTL_FORMAT_HPP

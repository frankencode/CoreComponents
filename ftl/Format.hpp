/*
 * Format.hpp -- formated strings
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
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

class Format: public Ref<StringList, Owner>
{
public:
	typedef Ref<StringList, Owner> Super;
	
	Format(String format = "");
	
	Format(const Format& b);
	Format& operator=(const Format& b);
	
	inline Format& print(String s) { get()->insert(get()->index(nextPlaceHolder()->j_), s); return *this; }
	inline Format& print(Path p) { return print(p.toString()); }
	inline Format& print(const char* s) { return print(String(s)); }
	
	inline Format& print(char ch) { get()->insert(get()->index(nextPlaceHolder()->j_), String(&ch, 1)); return *this; }
	
	inline Format& print(unsigned char x) { printInt(x); return *this; }
	inline Format& print(unsigned short x) { printInt(x); return *this; }
	inline Format& print(unsigned int x) { printInt(x); return *this; }
	inline Format& print(unsigned long x) { printInt(x); return *this; }
	inline Format& print(unsigned long long x) { printInt(x); return *this; }
	
	inline Format& print(short x) { int s = sign(x); printInt(uint64_t(s*x), s); return *this; }
	inline Format& print(int x) { int s = sign(x); printInt(uint64_t(s*x), s); return *this; }
	inline Format& print(long x) { int s = sign(x); printInt(uint64_t(s*x), s); return *this; }
	inline Format& print(long long x) { int s = sign(x); printInt(uint64_t(s*x), s); return *this; }
	
	inline Format& print(float x) { printFloat(x); return *this; }
	inline Format& print(double x) { printFloat(x); return *this; }
	
	inline Format& print(bool x) { return print(x ? "true" : "false"); }
	inline Format& print(void* x) {
		if (sizeof(void*) == sizeof(uint32_t))
			printInt(union_cast<uint32_t>(x));
		else if (sizeof(void*) == sizeof(uint64_t))
			printInt(union_cast<uint64_t>(x)); return *this;
		return *this;
	}
	
	Format& print(const Variant& x);
	
	template<class T>
	inline Format& operator<<(const T& x) { return print(x); }
	
	// inline operator String() const { return String::join(*this); }
	
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
		Ref<PlaceHolder, Owner> next_;
	};
	
	Ref<PlaceHolder, Owner> nextPlaceHolder();
	
	void printInt(uint64_t x, int sign = 1);
	void printFloat(float64_t x);
	
	Ref<PlaceHolder, Owner> placeHolders_;
};

} // namespace ftl

#endif // FTL_FORMAT_HPP

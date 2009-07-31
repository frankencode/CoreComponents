/*
 * Format.hpp -- formated strings
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_FORMAT_HPP
#define PONA_FORMAT_HPP

#include "atoms"
#include "String.hpp"
#include "Variant.hpp"
#include "Stack.hpp"

namespace pona
{

PONA_EXCEPTION(FormatException, Exception);

class Format: public String
{
public:
	typedef String::Element Element;
	typedef String::Media Media;
	
	Format();
	Format(const Char& ch);
	Format(const char* utf8);
	Format(Ref<Media, Owner> media);
	
	inline Format& print(String s) { get()->insert(nextPlaceHolder()->i_ + get()->length() - n0_, s); return *this; }
	inline Format& print(const char* s) { return print(String(s)); }
	
	inline Format& print(Char ch) { get()->insert(nextPlaceHolder()->i_ + get()->length() - n0_, ch); return *this; }
	inline Format& print(char ch) { return print(Char(ch)); }
	
	inline Format& print(uint8_t x) { printInt(x); return *this; }
	inline Format& print(uint16_t x) { printInt(x); return *this; }
	inline Format& print(uint32_t x) { printInt(x); return *this; }
	inline Format& print(uint64_t x) { printInt(x); return *this; }
	
	inline Format& print(int16_t x) { int s = sgn(x); printInt(uint64_t(s*x), s); return *this; }
	inline Format& print(int32_t x) { int s = sgn(x); printInt(uint64_t(s*x), s); return *this; }
	inline Format& print(int64_t x) { int s = sgn(x); printInt(uint64_t(s*x), s); return *this; }
	
	inline Format& print(float32_t x) { printFloat(x); return *this; }
	inline Format& print(float64_t x) { printFloat(x); return *this; }
	
	inline Format& print(bool x) { return print(x ? "true" : "false"); }
	inline Format& print(void* x) { printInt(uint64_t(x)); return *this; }
	
	Format& print(Variant x);
	
	template<class T>
	inline Format& operator<<(T x) { return print(x); }
	
private:
	enum { ExpAutoLimit = 6 };
	enum { MaxDigits = 128 }; // safe guess, 65 + ExpAutoLimit should be sufficient ?
	
	void init();
	
	class PlaceHolder: public Instance {
	public:
		PlaceHolder()
			: i_(0), w_(0), wi_(0), wf_(0), base_(10), exp_(false), blank_(' ')
		{}
		
		void check() {
			if ( ! ( ((2 <= base_) && (base_ <= 16)) &&
			         ((0 <= wi_) && (wi_ <= 16)) &&
			         ((0 <= wf_) && (wf_ <= 16)) &&
			         (0 <= w_) && (blank_ >= 32) ) )
				PONA_THROW(FormatException, "Invalid format specifier");
		}
		
		int i_;
		int w_, wi_, wf_, base_;
		bool exp_;
		Char blank_;
		Ref<PlaceHolder, Owner> next_;
	};
	
	Ref<PlaceHolder, Owner> nextPlaceHolder();
	
	template<class T>
	inline static int sgn(T x) { return (x >= 0) - (x < 0); }
	
	void printInt(uint64_t x, int sign = 1);
	void printFloat(float64_t x);
	
	Ref<PlaceHolder, Owner> defaultPlaceHolder_;
	Ref<PlaceHolder, Owner> placeHolders_;
	
	Stack<int> digits_;
	
	int n0_; // length of template text minus all place holders
};

} // namespace pona

#endif // PONA_FORMAT_HPP

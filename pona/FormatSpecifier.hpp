/*
 * FormatSpecifier.hpp -- syntax of "%%"-placeholders inside 'Format' strings
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_FORMATSPECIFIER_HPP
#define PONA_FORMATSPECIFIER_HPP

#include "Syntax.hpp"
#include "String.hpp"

namespace pona
{

class FormatSpecifier: public Syntax<String::Media>::Definition
{
public:
	FormatSpecifier();
	bool find(Ref<String::Media> text, int* i0, int* i1, int* w, int* wi, int* wf, int* base, bool* exp, char* blank);
	
private:
	int width_;
	int integerWidth_;
	int fractionWidth_;
	int base_;
	int exp_;
	int blank_;
	int format_;
};

} // namespace pona

#endif // PONA_FORMATSPECIFIER_HPP

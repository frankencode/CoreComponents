/*
 * FormatSpecifier.hpp -- syntax of "%%"-placeholders inside 'Format' strings
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_FORMATSPECIFIER_HPP
#define PONA_FORMATSPECIFIER_HPP

#include "Syntax.hpp"

namespace pona
{

class FormatSpecifier: public Syntax<String::Media>::Definition
{
public:
	FormatSpecifier();
	bool find(Ref<String::Media> text, int* i0, int* i1, int* w, int* wi, int* wf, int* base, bool* exp, Char* blank);
	
private:
	RULE width_;
	RULE integerWidth_;
	RULE fractionWidth_;
	RULE base_;
	RULE exp_;
	RULE blank_;
	RULE format_;
};

} // namespace pona

#endif // PONA_FORMATSPECIFIER_HPP

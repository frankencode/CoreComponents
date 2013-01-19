 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_FORMATSPECIFIER_HPP
#define FTL_FORMATSPECIFIER_HPP

#include "SyntaxDefinition.hpp"
#include "Singleton.hpp"
#include "ByteArray.hpp"

namespace ftl
{

class FormatSpecifier: public SyntaxDefinition, public Singleton<FormatSpecifier>
{
public:
	bool find(Ref<ByteArray> text, int *i0, int *i1, int *w, int *wi, int *wf, int *base, bool *exp, char *blank);

protected:
	friend class Singleton<FormatSpecifier>;

	FormatSpecifier();

	int width_;
	int integerWidth_;
	int fractionWidth_;
	int base_;
	int exp_;
	int blank_;
	int format_;
};

inline Ref<FormatSpecifier> formatSpecifier() { return FormatSpecifier::instance(); }

} // namespace ftl

#endif // FTL_FORMATSPECIFIER_HPP

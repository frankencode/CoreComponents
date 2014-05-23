/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXTOKI_LANGUAGE_H
#define FLUXTOKI_LANGUAGE_H

#include <flux/String.h>
#include <flux/SyntaxDefinition.h>

namespace fluxtoki
{

using namespace flux;

class Language: public Object
{
public:
	virtual bool detect(String path, String content) const = 0;

	inline String name() const { return name_; }
	inline SyntaxDefinition *syntax() const { return syntax_; }

protected:
	Language(SyntaxDefinition *syntax)
		: name_(syntax->name()),
		  syntax_(syntax)
	{}

	String name_;
	Ref<SyntaxDefinition> syntax_;
};

} // namespace fluxtoki

#endif // FLUXTOKI_LANGUAGE_H

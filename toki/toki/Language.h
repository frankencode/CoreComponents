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

#include <flux/Pattern.h>
#include <flux/SyntaxDefinition.h>

namespace fluxtoki
{

using namespace flux;

class Language: public Object
{
public:
	inline String displayName() const { return displayName_; }
	inline String name() const { return name_; }
	inline Pattern pathPattern() const { return pathPattern_; }
	inline const SyntaxDefinition *highlightingSyntax() const { return highlightingSyntax_; }
	inline const SyntaxDefinition *discoverySyntax() const { return discoverySyntax_; }
	inline const SyntaxDefinition *foldingSyntax() const { return foldingSyntax_; }

protected:
	Language(
		String displayName,
		Pattern pathPattern,
		const SyntaxDefinition *highlightingSyntax,
		const SyntaxDefinition *discoverySyntax = 0,
		const SyntaxDefinition *foldingSyntax = 0
	);

private:
	Language();

	String displayName_;
	String name_;
	Pattern pathPattern_;
	Ref<const SyntaxDefinition> highlightingSyntax_;
	Ref<const SyntaxDefinition> discoverySyntax_;
	Ref<const SyntaxDefinition> foldingSyntax_;
};

} // namespace fluxtoki

#endif // FLUXTOKI_LANGUAGE_H

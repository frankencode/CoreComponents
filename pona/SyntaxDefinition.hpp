/*
 * SyntaxDefinition.hpp -- alias for 'Syntax::Definition'
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_SYNTAXDEFINITION_HPP
#define PONA_SYNTAXDEFINITION_HPP

#include "Syntax.hpp"

namespace pona
{

template<class Media>
class PONA_API SyntaxDefinition: public Syntax<Media>::Definition
{
public:
	SyntaxDefinition(int languageId = -1)
		: Syntax<Media>::Definition(languageId)
	{}
};

} // namespace pona

#endif // PONA_SYNTAXDEFINITION_HPP


/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXXML_XMLSYNTAX_H
#define FLUXXML_XMLSYNTAX_H

#include <flux/SyntaxDefinition.h>

namespace flux {

template<class> class Singleton;

namespace xml {

class XmlSyntax: public SyntaxDefinition
{
protected:
	friend class Singleton<XmlSyntax>;
	XmlSyntax();
};

XmlSyntax *xmlSyntax();

}} // namespace flux::xml

#endif // FLUXXML_XMLSYNTAX_H

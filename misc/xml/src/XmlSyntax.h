/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXXML_XMLSYNTAX_H
#define FLUXXML_XMLSYNTAX_H

#include <flux/syntax/SyntaxDefinition>

namespace flux { template<class> class Singleton; }

namespace flux {
namespace xml {

using namespace flux::syntax;

class XmlSyntax: public SyntaxDefinition
{
protected:
    friend class Singleton<XmlSyntax>;
    XmlSyntax();
};

XmlSyntax *xmlSyntax();

}} // namespace flux::xml

#endif // FLUXXML_XMLSYNTAX_H

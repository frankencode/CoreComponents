/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/syntax/SyntaxDefinition>

namespace cc { template<class> class Singleton; }

namespace cc {
namespace xml {

using namespace cc::syntax;

class XmlSyntax: public SyntaxDefinition
{
protected:
    friend class Singleton<XmlSyntax>;
    XmlSyntax();
};

XmlSyntax *xmlSyntax();

}} // namespace cc::xml


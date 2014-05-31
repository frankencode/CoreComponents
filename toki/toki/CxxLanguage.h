/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXTOKI_CXXLANGUAGE_H
#define FLUXTOKI_CXXLANGUAGE_H

#include "Registry.h"

namespace fluxtoki
{

class CxxLanguage: public Language
{
private:
	friend class Registration<CxxLanguage>;
	CxxLanguage();
};

namespace { Registration<CxxLanguage> registration; }

} // namespace fluxtoki

#endif // FLUXTOKI_CXXLANGUAGE_H

/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXTOKI_YASONLANGUAGE_H
#define FLUXTOKI_YASONLANGUAGE_H

#include "Registry.h"

namespace fluxtoki
{

class YasonLanguage: public Language
{
private:
	friend class Registration<YasonLanguage>;
	YasonLanguage();
};

namespace { Registration<YasonLanguage> registration; }

} // namespace fluxtoki

#endif // FLUXTOKI_YASONLANGUAGE_H

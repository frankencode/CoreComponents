/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXTOKI_REGISTRY_H
#define FLUXTOKI_REGISTRY_H

#include <flux/Singleton.h>
#include <flux/SyntaxDefinition.h>

namespace fluxtoki
{

using namespace flux;

class Registry: public Object, public Singleton<Registy>
{
public:


private:
	friend class Singleton<Registry>;
};

} // namespace fluxtoki

#endif // FLUXTOKI_REGISTRY_H

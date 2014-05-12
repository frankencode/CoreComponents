/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXDOC_GENERATOR_H
#define FLUXDOC_GENERATOR_H

#include <flux/yason.h>

namespace fluxdoc
{

using namespace flux;

class Generator: public Object
{
public:
	inline String name() const { return name_; }
	inline YasonObject *prototype() const { return prototype_; }

	virtual void run() = 0;

protected:
	Generator(String name, YasonObject *prototype);

	String name_;
	Ref<YasonObject> prototype_;
};

} // namespace fluxdoc

#endif // FLUXDOC_GENERATOR_H

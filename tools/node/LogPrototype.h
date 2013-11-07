/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXNODE_LOGPROTOTYPE_H
#define FLUXNODE_LOGPROTOTYPE_H

#include <flux/Yason.h>
#include <flux/Date.h>

namespace fluxnode
{

using namespace flux;

class LogPrototype: public YasonObject
{
public:
	static Ref<LogPrototype> create() { return new LogPrototype; }

private:
	LogPrototype()
		: YasonObject("Log")
	{
		insert("path", "");
		insert("level", "");
		insert("retention", days(30));
		insert("rotation", days(1));
	}
};

} // namespace fluxnode

#endif // FLUXNODE_LOGPROTOTYPE_H

/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXNODE_CONFIGPROTOCOL_H
#define FLUXNODE_CONFIGPROTOCOL_H

#include <flux/Singleton.h>
#include <flux/Yason.h>

namespace fluxnode
{

using namespace flux;

class ConfigProtocol: public YasonProtocol, public Singleton<ConfigProtocol>
{
public:
	void registerService(YasonObject *configPrototype);

private:
	friend class Singleton<ConfigProtocol>;
	ConfigProtocol();
	Ref<YasonProtocol> nodeProtocol_;
};

inline ConfigProtocol *configProtocol() { return ConfigProtocol::instance(); }

} // namespace fluxnode

#endif // FLUXNODE_CONFIGPROTOCOL_H

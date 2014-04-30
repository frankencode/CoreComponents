/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <unistd.h> // sysconf
#include "Exception.h"
#include "Group.h"

namespace flux
{

Group::Group(gid_t id)
{
	int bufSize = 1024;
	#ifdef _SC_GETGR_R_SIZE_MAX
	int h = sysconf(_SC_GETGR_R_SIZE_MAX);
	if (h != -1) bufSize = h;
	#endif
	String buf(bufSize);
	struct group space;
	memclr(&space, sizeof(struct group));
	struct group *entry = 0;
	if (::getgrgid_r(id, &space, buf->chars(), buf->size(), &entry) != 0)
		FLUX_SYSTEM_EXCEPTION;
	load(entry);
}

Group::Group(const char *name)
{
	int bufSize = sysconf(_SC_GETGR_R_SIZE_MAX);
	if (bufSize == -1)
		FLUX_SYSTEM_EXCEPTION;
	String buf(bufSize);
	struct group space;
	memclr(&space, sizeof(struct group));
	struct group *entry = 0;
	if (::getgrnam_r(name, &space, buf->chars(), buf->size(), &entry) != 0)
		FLUX_SYSTEM_EXCEPTION;
	load(entry);
}

void Group::load(struct group *entry)
{
	id_ = entry->gr_gid;
	name_ = entry->gr_name;
	members_ = StringList::create();
	char **pcs = entry->gr_mem;
	while (*pcs) {
		members_->append(*pcs);
		++pcs;
	}
}

} // namespace flux

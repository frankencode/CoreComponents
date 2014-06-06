/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <unistd.h> // sysconf
#include "errors.h"
#include "Group.h"

namespace flux
{

Group::Group(gid_t id)
{
	int bufSize = sysconf(_SC_GETGR_R_SIZE_MAX);
	if (bufSize == -1) FLUX_SYSTEM_DEBUG_ERROR(errno);
	String buf(bufSize);
	struct group space;
	memclr(&space, sizeof(struct group));
	struct group *entry = 0;
	int ret = ::getgrgid_r(id, &space, buf->chars(), buf->size(), &entry);
	if ((!entry) && ret == 0) ret = ENOENT;
	if (ret != 0) FLUX_SYSTEM_DEBUG_ERROR(ret);
	load(entry);
}

Group::Group(const char *name)
{
	int bufSize = sysconf(_SC_GETGR_R_SIZE_MAX);
	if (bufSize == -1) FLUX_SYSTEM_DEBUG_ERROR(errno);
	String buf(bufSize);
	struct group space;
	memclr(&space, sizeof(struct group));
	struct group *entry = 0;
	int ret = ::getgrnam_r(name, &space, buf->chars(), buf->size(), &entry);
	if ((!entry) && ret == 0) ret = ENOENT;
	if (ret != 0) FLUX_SYSTEM_RESOURCE_ERROR(ret, name);
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

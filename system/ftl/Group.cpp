/*
 * Group.cpp -- reading group information
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include <unistd.h> // sysconf
#include "Group.hpp"

namespace ftl
{

Group::Group(gid_t id)
{
	int bufSize = 1024;
	#ifdef _SC_GETGR_R_SIZE_MAX
	int h = sysconf(_SC_GETGR_R_SIZE_MAX);
	if (h != -1) bufSize = h;
	#endif
	Ref<ByteArray, Owner> buf = ByteArray::newInstance(bufSize, '\0');
	struct group space;
	mem::clr(&space, sizeof(struct group));
	struct group* entry = 0;
	if (::getgrgid_r(id, &space, buf->data(), buf->size(), &entry) != 0)
		FTL_SYSTEM_EXCEPTION;
	load(entry);
}

Group::Group(const char* name)
{
	int bufSize = sysconf(_SC_GETGR_R_SIZE_MAX);
	if (bufSize == -1)
		FTL_SYSTEM_EXCEPTION;
	Ref<ByteArray, Owner>  buf = ByteArray::newInstance(bufSize, '\0');
	struct group space;
	mem::clr(&space, sizeof(struct group));
	struct group* entry = 0;
	if (::getgrnam_r(name, &space, buf->data(), buf->size(), &entry) != 0)
		FTL_SYSTEM_EXCEPTION;
	load(entry);
}

void Group::load(struct group* entry)
{
	id_ = entry->gr_gid;
	name_ = entry->gr_name;
	members_ = new StringList;
	char** pcs = entry->gr_mem;
	while (*pcs) {
		members_->append(*pcs);
		++pcs;
	}
}

} // namespace ftl

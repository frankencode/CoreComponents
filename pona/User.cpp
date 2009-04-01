/*
 * User.cpp -- reading user information
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include <unistd.h> // sysconf
#include "User.hpp"

namespace pona
{

User::User(uid_t id)
{
	int bufSize = sysconf(_SC_GETPW_R_SIZE_MAX);
	if (bufSize == -1)
		PONA_SYSTEM_EXCEPTION;
	CString buf(bufSize);
	struct passwd space;
	struct passwd* entry = 0;
	if (::getpwuid_r(id, &space, buf, bufSize, &entry) != 0)
		PONA_SYSTEM_EXCEPTION;
	load(entry);
}

User::User(String name)
{
	int bufSize = sysconf(_SC_GETPW_R_SIZE_MAX);
	if (bufSize == -1)
		PONA_SYSTEM_EXCEPTION;
	CString buf(bufSize);
	struct passwd space;
	struct passwd* entry = 0;
	if (::getpwnam_r(name.utf8(), &space, buf, bufSize, &entry) != 0)
		PONA_SYSTEM_EXCEPTION;
	load(entry);
}

void User::load(struct passwd* entry)
{
	if (entry) {
		exists_ = true;
		id_ = entry->pw_uid;
		groupId_ = entry->pw_gid;
		name_ = entry->pw_name;
		realName_ = entry->pw_gecos;
		home_ = entry->pw_dir;
		shell_ = entry->pw_shell;
	}
	else {
		exists_ = false;
		id_ = 0;
		groupId_ = 0;
	}
}

} // namespace pona

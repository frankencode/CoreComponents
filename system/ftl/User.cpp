/*
 * User.cpp -- reading user information
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include <unistd.h> // sysconf
#include "User.hpp"

namespace ftl
{

User::User(uid_t id)
{
	int bufSize = 1024;
	#ifdef _SC_GETPW_R_SIZE_MAX
	int h = sysconf(_SC_GETPW_R_SIZE_MAX);
	if (h != -1) bufSize = h;
	#endif
	ByteArray buf(bufSize);
	struct passwd space;
	struct passwd* entry = 0;
	if (::getpwuid_r(id, &space, buf, bufSize, &entry) != 0)
		FTL_SYSTEM_EXCEPTION;
	load(entry);
}

User::User(const char* name)
{
	int bufSize = sysconf(_SC_GETPW_R_SIZE_MAX);
	if (bufSize == -1)
		FTL_SYSTEM_EXCEPTION;
	ByteArray buf(bufSize);
	struct passwd space;
	struct passwd* entry = 0;
	if (::getpwnam_r(name, &space, buf, bufSize, &entry) != 0)
		FTL_SYSTEM_EXCEPTION;
	load(entry);
}

void User::load(struct passwd* entry)
{
	if (entry) {
		exists_ = true;
		id_ = entry->pw_uid;
		groupId_ = entry->pw_gid;
		loginName_ = entry->pw_name;
		fullName_ = entry->pw_gecos;
		if (fullName_->length() > 0)
			if ((fullName_->get(0) == ',') || (fullName_->get(-1) == ',')) {
				fullName_ = loginName_->copy();
				if (isLower(fullName_->at(0)))
					fullName_->set(0, toUpper(fullName_->at(0)));
				// fullName_ << " Anonymous";
			}
		home_ = entry->pw_dir;
		shell_ = entry->pw_shell;
	}
	else {
		exists_ = false;
		id_ = 0;
		groupId_ = 0;
	}
}

} // namespace ftl

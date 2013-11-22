/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <unistd.h> // sysconf
#include "User.h"

namespace flux
{

User::User(uid_t id)
{
	int bufSize = 1024;
	#ifdef _SC_GETPW_R_SIZE_MAX
	int h = sysconf(_SC_GETPW_R_SIZE_MAX);
	if (h != -1) bufSize = h;
	#endif
	String buf(bufSize);
	struct passwd space;
	struct passwd *entry = 0;
	if (::getpwuid_r(id, &space, buf->chars(), buf->size(), &entry) != 0)
		FLUX_SYSTEM_EXCEPTION;
	load(entry);
}

User::User(const char *name)
{
	int bufSize = sysconf(_SC_GETPW_R_SIZE_MAX);
	if (bufSize == -1)
		FLUX_SYSTEM_EXCEPTION;
	String buf(bufSize);
	struct passwd space;
	struct passwd *entry = 0;
	if (::getpwnam_r(name, &space, buf->chars(), buf->size(), &entry) != 0)
		FLUX_SYSTEM_EXCEPTION;
	load(entry);
}

void User::load(struct passwd *entry)
{
	if (entry) {
		exists_ = true;
		id_ = entry->pw_uid;
		groupId_ = entry->pw_gid;
		loginName_ = entry->pw_name;
		fullName_ = entry->pw_gecos;
		if (fullName_->size() > 0)
			if ((fullName_->at(0) == ',') || (fullName_->at(fullName_->size() - 1) == ',')) {
				fullName_ = loginName_->copy();
				if (isLower(fullName_->at(0)))
					fullName_->at(0) = downcase(fullName_->at(0));
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

} // namespace flux

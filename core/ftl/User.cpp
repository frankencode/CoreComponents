 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
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
	Ref<ByteArray, Owner> buf = ByteArray::create(bufSize);
	struct passwd space;
	struct passwd *entry = 0;
	if (::getpwuid_r(id, &space, buf->data(), buf->size(), &entry) != 0)
		FTL_SYSTEM_EXCEPTION;
	load(entry);
}

User::User(const char *name)
{
	int bufSize = sysconf(_SC_GETPW_R_SIZE_MAX);
	if (bufSize == -1)
		FTL_SYSTEM_EXCEPTION;
	Ref<ByteArray, Owner> buf = ByteArray::create(bufSize);
	struct passwd space;
	struct passwd *entry = 0;
	if (::getpwnam_r(name, &space, buf->data(), buf->size(), &entry) != 0)
		FTL_SYSTEM_EXCEPTION;
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

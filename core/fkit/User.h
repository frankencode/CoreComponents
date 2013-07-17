/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_USER_H
#define FKIT_USER_H

#include <sys/types.h>
#include <pwd.h>
#include "String.h"

namespace fkit
{

/** \brief system user information
  * \see Group
  */
class User: public Object
{
public:
	inline static Ref<User> lookup(uid_t id) {
		return new User(id);
	}
	inline static Ref<User> lookup(const char *name) {
		return new User(name);
	}

	inline bool exists() const { return exists_; }

	inline uid_t id() const { return id_; }
	inline gid_t groupId() const { return groupId_; }
	inline String name() const { return loginName_; }
	inline String loginName() const { return loginName_; }
	inline String fullName() const { return fullName_; }
	inline String home() const { return home_; }
	inline String shell() const { return shell_; }

private:
	User(uid_t id);
	User(const char *name);

	void load(struct passwd *entry);
	bool exists_;
	uid_t id_;
	gid_t groupId_;
	String loginName_;
	String fullName_;
	String home_;
	String shell_;
};

} // namespace fkit

#endif // FKIT_USER_H

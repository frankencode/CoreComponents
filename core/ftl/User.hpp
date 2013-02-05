 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_USER_HPP
#define FTL_USER_HPP

#include <sys/types.h>
#include <pwd.h>
#include "string.hpp"

namespace ftl
{

/** \brief system user information
  * \see Group
  */
class User: public Instance
{
public:
	inline static hook<User> lookup(uid_t id) {
		return new User(id);
	}
	inline static hook<User> lookup(const char *name) {
		return new User(name);
	}

	inline bool exists() const { return exists_; }

	inline uid_t id() const { return id_; }
	inline gid_t groupId() const { return groupId_; }
	inline string name() const { return loginName_; }
	inline string loginName() const { return loginName_; }
	inline string fullName() const { return fullName_; }
	inline string home() const { return home_; }
	inline string shell() const { return shell_; }

private:
	User(uid_t id);
	User(const char *name);

	void load(struct passwd *entry);
	bool exists_;
	uid_t id_;
	gid_t groupId_;
	string loginName_;
	string fullName_;
	string home_;
	string shell_;
};

} // namespace ftl

#endif // FTL_USER_HPP

/*
 * User.hpp -- reading user information
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_USER_HPP
#define FTL_USER_HPP

#include <sys/types.h>
#include <pwd.h>
#include "atoms"
#include "String.hpp"

namespace ftl
{

class User: public Instance
{
public:
	User(uid_t id);
	User(const char* name);
	
	inline bool exists() const { return exists_; }
	
	inline uid_t id() const { return id_; }
	inline gid_t groupId() const { return groupId_; }
	inline String name() const { return loginName_; }
	inline String loginName() const { return loginName_; }
	inline String fullName() const { return fullName_; }
	inline String home() const { return home_; }
	inline String shell() const { return shell_; }
	
	// inline static isSuperUser
	
private:
	void load(struct passwd* entry);
	bool exists_;
	uid_t id_;
	gid_t groupId_;
	String loginName_;
	String fullName_;
	String home_;
	String shell_;
};

} // namespace ftl

#endif // FTL_USER_HPP

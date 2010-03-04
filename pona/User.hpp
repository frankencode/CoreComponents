/*
 * User.hpp -- reading user information
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_USER_HPP
#define PONA_USER_HPP

#include <sys/types.h>
#include <pwd.h>
#include "atoms"
#include "UString.hpp"

namespace pona
{

class User: public Instance
{
public:
	User(uid_t id);
	User(const char* name);
	
	inline bool exists() const { return exists_; }
	
	inline uid_t id() const { return id_; }
	inline gid_t groupId() const { return groupId_; }
	inline UString loginName() const { return loginName_; }
	inline UString fullName() const { return fullName_; }
	inline UString home() const { return home_; }
	inline UString shell() const { return shell_; }
	
	// inline static isSuperUser
	
private:
	void load(struct passwd* entry);
	bool exists_;
	uid_t id_;
	gid_t groupId_;
	UString loginName_;
	UString fullName_;
	UString home_;
	UString shell_;
};

} // namespace pona

#endif // PONA_USER_HPP

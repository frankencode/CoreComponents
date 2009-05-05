/*
 * User.hpp -- reading user information
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_USER_HPP
#define PONA_USER_HPP

#include <sys/types.h>
#include <pwd.h>
#include "atom"
#include "String.hpp"

namespace pona
{

class User: public Instance
{
public:
	User(uid_t id);
	User(String name);
	
	inline bool exists() const { return exists_; }
	
	inline uid_t id() const { return id_; }
	inline gid_t groupId() const { return groupId_; }
	inline String name() const { return name_; }
	inline String realName() const { return realName_; }
	inline String home() const { return home_; }
	inline String shell() const { return shell_; }
	
private:
	void load(struct passwd* entry);
	bool exists_;
	uid_t id_;
	gid_t groupId_;
	String name_;
	String realName_;
	String home_;
	String shell_;
};

} // namespace pona

#endif // PONA_USER_HPP

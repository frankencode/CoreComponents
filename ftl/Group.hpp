/*
 * Group.hpp -- reading group information
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_GROUP_HPP
#define FTL_GROUP_HPP

#include <sys/types.h>
#include <grp.h>
#include "atoms"
#include "String.hpp"

namespace ftl
{

class Group: public Instance
{
public:
	Group(gid_t id);
	Group(const char* name);
	
	inline bool exists() const { return exists_; }
	
	inline gid_t id() const { return id_; }
	inline String name() const { return name_; }
	inline Ref<StringList> members() const { return members_; }
	
private:
	void load(struct group* entry);
	bool exists_;
	gid_t id_;
	String name_;
	Ref<StringList, Owner> members_;
};

} // namespace ftl

#endif // FTL_GROUP_HPP

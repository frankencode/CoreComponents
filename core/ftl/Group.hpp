/*
 * Group.hpp -- reading group information
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_GROUP_HPP
#define FTL_GROUP_HPP

#include <sys/types.h>
#include <grp.h>
#include "atoms"
#include "String.hpp"

namespace ftl
{

/** \brief system group information
  * \see User
  */
class Group: public Instance
{
public:
	inline static Ref<Group, Owner> lookup(gid_t id) {
		return new Group(id);
	}
	inline static Ref<Group, Owner> lookup(const char *name) {
		return new Group(name);
	}

	inline bool exists() const { return exists_; }

	inline gid_t id() const { return id_; }
	inline String name() const { return name_; }
	inline Ref<StringList> members() const { return members_; }

private:
	Group(gid_t id);
	Group(const char *name);

	void load(struct group *entry);
	bool exists_;
	gid_t id_;
	String name_;
	Ref<StringList, Owner> members_;
};

} // namespace ftl

#endif // FTL_GROUP_HPP

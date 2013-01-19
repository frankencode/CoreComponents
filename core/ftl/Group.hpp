 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
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

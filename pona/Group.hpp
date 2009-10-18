/*
 * Group.hpp -- reading group information
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_GROUP_HPP
#define PONA_GROUP_HPP

#include <sys/types.h>
#include <grp.h>
#include "atoms"
#include "String.hpp"

namespace pona
{

class PONA_API Group: public Instance
{
public:
	Group(gid_t id);
	Group(String name);
	
	inline bool exists() const { return exists_; }
	
	inline gid_t id() const { return id_; }
	inline String name() const { return name_; }
	inline Ref<StringList> members() const { return members_; }
	
private:
	PONA_INTERN void load(struct group* entry);
	bool exists_;
	gid_t id_;
	String name_;
	Ref<StringList, Owner> members_;
};

} // namespace pona

#endif // PONA_GROUP_HPP

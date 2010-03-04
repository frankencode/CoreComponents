/*
 * Group.hpp -- reading group information
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_GROUP_HPP
#define PONA_GROUP_HPP

#include <sys/types.h>
#include <grp.h>
#include "atoms"
#include "UString.hpp"

namespace pona
{

class Group: public Instance
{
public:
	Group(gid_t id);
	Group(const char* name);
	
	inline bool exists() const { return exists_; }
	
	inline gid_t id() const { return id_; }
	inline UString name() const { return name_; }
	inline Ref<UStringList> members() const { return members_; }
	
private:
	void load(struct group* entry);
	bool exists_;
	gid_t id_;
	UString name_;
	Ref<UStringList, Owner> members_;
};

} // namespace pona

#endif // PONA_GROUP_HPP

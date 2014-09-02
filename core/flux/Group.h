/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_GROUP_H
#define FLUX_GROUP_H

#include <sys/types.h>
#include <grp.h>
#include "String.h"

namespace flux
{

/** \brief system group information
  * \see User
  */
class Group: public Object
{
public:
	inline static Ref<Group> lookup(gid_t id) {
		return new Group(id);
	}
	inline static Ref<Group> lookup(const char *name) {
		return new Group(name);
	}

	inline bool exists() const { return exists_; }

	inline gid_t id() const { return id_; }
	inline String name() const { return name_; }
	inline StringList *members() const { return members_; }

private:
	Group(gid_t id);
	Group(const char *name);

	void load(struct group *entry);
	bool exists_;
	gid_t id_;
	String name_;
	Ref<StringList> members_;
};

} // namespace flux

#endif // FLUX_GROUP_H

/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <sys/types.h>
#include <grp.h>
#include <cc/String>

namespace cc {

class User;

/** \class Group Group.h cc/Group
  * \brief %System group information
  * \see User
  */
class Group: public Object
{
public:
    inline static Ref<Group> lookup(gid_t id) {
        return new Group(id);
    }
    inline static Ref<Group> lookup(String name) {
        return new Group(name);
    }

    inline bool exists() const { return exists_; }

    inline gid_t id() const { return id_; }
    inline String name() const { return name_; }
    inline StringList *otherMembers() const { return otherMembers_; }

    bool checkMembership(User *user) const;

private:
    Group(gid_t id);
    Group(String name);

    void load(struct group *entry);
    bool exists_;
    gid_t id_;
    String name_;
    Ref<StringList> otherMembers_;
};

} // namespace cc

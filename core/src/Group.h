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
  * \ingroup sysinfo
  * \brief %System group information
  * \see User
  */
class Group: public Object
{
public:
    /** Lookup group information by group ID
      * \param id system group ID
      * \return new object instance
      */
    inline static Ref<Group> lookup(gid_t id) {
        return new Group(id);
    }

    /** Lookup group information by group name
      * \param name system group name
      * \return new object instance
      */
    inline static Ref<Group> lookup(String name) {
        return new Group(name);
    }

    /// True if queried group exists on the system
    inline bool isValid() const { return isValid_; }

    inline gid_t id() const { return id_; } ///< system group ID
    inline String name() const { return name_; } ///< group name

    /// Return list of user logins, which are in this group, but for which this is not their primary group
    inline StringList *otherMembers() const { return otherMembers_; }

    /** Check group membership
      * \param user user information object
      * \return true if the user is member of this group, false otherwise
      */
    bool checkMembership(User *user) const;

private:
    Group(gid_t id);
    Group(String name);

    void load(struct group *entry);
    bool isValid_;
    gid_t id_;
    String name_;
    Ref<StringList> otherMembers_;
};

} // namespace cc

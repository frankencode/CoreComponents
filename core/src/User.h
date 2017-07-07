/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <sys/types.h>
#include <pwd.h>
#include <cc/String>

namespace cc {

/** \class User User.h cc/User
  * \ingroup sysinfo
  * \brief Query information about system users
  * \see Group
  */
class User: public Object
{
public:
    /** Lookup user information by user ID
      * \param id system user ID
      * \return new object instance
      */
    inline static Ref<User> lookup(uid_t id) {
        return new User(id);
    }

    /** Lookup user information by user name
      * \param name login name
      * \return new object instance
      */
    inline static Ref<User> lookup(String name) {
        return new User(name);
    }

    /// True if queried user exists on the system
    inline bool isValid() const { return isValid_; }

    inline uid_t id() const { return id_; } ///< user ID
    inline gid_t groupId() const { return groupId_; } ///< primary group ID
    inline String name() const { return loginName_; } ///< login name
    inline String loginName() const { return loginName_; } ///< login name
    inline String fullName() const { return fullName_; } ///< real name
    inline String home() const { return home_; } ///< home directory path
    inline String shell() const { return shell_; } ///< login shell of the user

private:
    User(uid_t id);
    User(String name);

    void load(struct passwd *entry);
    bool isValid_;
    uid_t id_;
    gid_t groupId_;
    String loginName_;
    String fullName_;
    String home_;
    String shell_;
};

} // namespace cc

/*
 * Copyright (C) 2007-2016 Frank Mertens.
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

/** \brief %System user information
  * \see Group
  */
class User: public Object
{
public:
    inline static Ref<User> lookup(uid_t id) {
        return new User(id);
    }
    inline static Ref<User> lookup(String name) {
        return new User(name);
    }

    inline bool exists() const { return exists_; }

    inline uid_t id() const { return id_; }
    inline gid_t groupId() const { return groupId_; }
    inline String name() const { return loginName_; }
    inline String loginName() const { return loginName_; }
    inline String fullName() const { return fullName_; }
    inline String home() const { return home_; }
    inline String shell() const { return shell_; }

private:
    User(uid_t id);
    User(String name);

    void load(struct passwd *entry);
    bool exists_;
    uid_t id_;
    gid_t groupId_;
    String loginName_;
    String fullName_;
    String home_;
    String shell_;
};

} // namespace cc

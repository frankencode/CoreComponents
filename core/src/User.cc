/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <unistd.h> // sysconf
#include <cc/exceptions>
#include <cc/User>

namespace cc {

User::User(uid_t id)
{
    int bufSize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (bufSize == -1) CC_SYSTEM_DEBUG_ERROR(errno);

    String buf(bufSize);
    struct passwd space;
    struct passwd *entry = 0;
    int ret = ::getpwuid_r(id, &space, mutate(buf)->chars(), buf->count(), &entry);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
    load(entry);
}

User::User(String name)
{
    int bufSize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (bufSize == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    String buf(bufSize);
    struct passwd space;
    struct passwd *entry = 0;
    int ret = ::getpwnam_r(name, &space, mutate(buf)->chars(), buf->count(), &entry);
    if (ret != 0) CC_SYSTEM_RESOURCE_ERROR(ret, name);
    load(entry);
}

void User::load(struct passwd *entry)
{
    if (entry) {
        isValid_ = true;
        id_ = entry->pw_uid;
        groupId_ = entry->pw_gid;
        loginName_ = entry->pw_name;
        fullName_ = entry->pw_gecos;
        if (fullName_->count() > 0)
            if ((fullName_->at(0) == ',') || (fullName_->at(fullName_->count() - 1) == ',')) {
                fullName_ = loginName_->copy();
                if (isLower(fullName_->at(0)))
                    mutate(fullName_)->at(0) = toLower(fullName_->at(0));
                // fullName_ << " Anonymous";
            }
        home_ = entry->pw_dir;
        shell_ = entry->pw_shell;
    }
    else {
        isValid_ = false;
        id_ = 0;
        groupId_ = 0;
    }
}

} // namespace cc

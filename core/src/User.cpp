/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <unistd.h> // sysconf
#include <flux/exceptions>
#include <flux/User>

namespace flux {

User::User(uid_t id)
{
    int bufSize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (bufSize == -1) FLUX_SYSTEM_DEBUG_ERROR(errno);

    String buf(bufSize);
    struct passwd space;
    struct passwd *entry = 0;
    int ret = ::getpwuid_r(id, &space, buf->chars(), buf->count(), &entry);
    if ((!entry) && ret == 0) ret = ENOENT;
    if (ret != 0) FLUX_SYSTEM_DEBUG_ERROR(ret);
    load(entry);
}

User::User(const char *name)
{
    int bufSize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (bufSize == -1) FLUX_SYSTEM_DEBUG_ERROR(errno);
    String buf(bufSize);
    struct passwd space;
    struct passwd *entry = 0;
    int ret = ::getpwnam_r(name, &space, buf->chars(), buf->count(), &entry);
    if ((!entry) && ret == 0) ret = ENOENT;
    if (ret != 0) FLUX_SYSTEM_RESOURCE_ERROR(ret, name);
    load(entry);
}

void User::load(struct passwd *entry)
{
    if (entry) {
        exists_ = true;
        id_ = entry->pw_uid;
        groupId_ = entry->pw_gid;
        loginName_ = entry->pw_name;
        fullName_ = entry->pw_gecos;
        if (fullName_->count() > 0)
            if ((fullName_->at(0) == ',') || (fullName_->at(fullName_->count() - 1) == ',')) {
                fullName_ = loginName_->copy();
                if (isLower(fullName_->at(0)))
                    fullName_->at(0) = downcase(fullName_->at(0));
                // fullName_ << " Anonymous";
            }
        home_ = entry->pw_dir;
        shell_ = entry->pw_shell;
    }
    else {
        exists_ = false;
        id_ = 0;
        groupId_ = 0;
    }
}

} // namespace flux

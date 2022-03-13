/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/GroupInfo>
#include <cc/UserInfo>
#include <sys/types.h>
#include <unistd.h> // sysconf
#include <grp.h>

namespace cc {

GroupInfo::State::State(const struct group &info):
    id{static_cast<int>(info.gr_gid)},
    name{info.gr_name}
{
    for (char **pcs = info.gr_mem; *pcs; ++pcs)
        members << String{*pcs};
}

GroupInfo::GroupInfo(gid_t id)
{
    int bufSize = ::sysconf(_SC_GETGR_R_SIZE_MAX);
    if (bufSize == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    String buf = String::allocate(bufSize);
    struct group space;
    struct group *info = nullptr;
    int ret = ::getgrgid_r(id, &space, buf, +buf, &info);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
    if (info) Object::me = new State{*info};
}

GroupInfo::GroupInfo(const String &name)
{
    int bufSize = ::sysconf(_SC_GETGR_R_SIZE_MAX);
    if (bufSize == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    String buf = String::allocate(bufSize);
    struct group space;
    struct group *info = nullptr;
    int ret = ::getgrnam_r(name, &space, buf, +buf, &info);
    if (ret != 0) CC_SYSTEM_RESOURCE_ERROR(ret, name);
    if (info) Object::me = new State{*info};
}

bool GroupInfo::checkMembership(const UserInfo &user) const
{
    return user.groupId() == me().id || me().members.find(user.loginName());
}

} // namespace cc

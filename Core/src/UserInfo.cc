/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/UserInfo>
#include <sys/types.h>
#include <unistd.h> // sysconf
#include <pwd.h>

namespace cc {

UserInfo::State::State(const struct passwd &info):
    id{static_cast<int>(info.pw_uid)},
    groupId{static_cast<int>(info.pw_gid)},
    loginName{info.pw_name},
    fullName{info.pw_gecos},
    home{info.pw_dir},
    shell{info.pw_shell}
{
    if (fullName.startsWith(',') || fullName.endsWith(',')) {
        fullName = loginName.copy();
        fullName[0] = toUpper(fullName[0]);
    }
}

UserInfo::UserInfo(int id)
{
    int bufSize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (bufSize == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    String buf = String::allocate(bufSize);
    struct passwd space;
    struct passwd *info = nullptr;
    int ret = ::getpwuid_r(id, &space, buf, +buf, &info);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
    if (info) Object::me = new State{*info};
}

UserInfo::UserInfo(const String &name)
{
    int bufSize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (bufSize == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    String buf = String::allocate(bufSize);
    struct passwd space;
    struct passwd *info = nullptr;
    int ret = ::getpwnam_r(name, &space, buf, +buf, &info);
    if (ret != 0) CC_SYSTEM_RESOURCE_ERROR(ret, name);
    if (info) Object::me = new State{*info};
}

} // namespace cc

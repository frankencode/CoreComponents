/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <unistd.h> // sysconf
#include <cc/exceptions>
#include <cc/User>
#include <cc/Group>

namespace cc {

Group::Group(gid_t id)
{
    int bufSize = sysconf(_SC_GETGR_R_SIZE_MAX);
    if (bufSize == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    String buf = ByteArray::allocate(bufSize);
    struct group space;
    memclr(&space, sizeof(struct group));
    struct group *entry = 0;
    int ret = ::getgrgid_r(id, &space, buf->chars(), buf->count(), &entry);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
    load(entry);
}

Group::Group(String name)
{
    int bufSize = sysconf(_SC_GETGR_R_SIZE_MAX);
    if (bufSize == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    String buf = ByteArray::allocate(bufSize);
    struct group space;
    memclr(&space, sizeof(struct group));
    struct group *entry = 0;
    int ret = ::getgrnam_r(name, &space, buf->chars(), buf->count(), &entry);
    if (ret != 0) CC_SYSTEM_RESOURCE_ERROR(ret, name);
    load(entry);
}

bool Group::checkMembership(User *user) const
{
    return user->groupId() == id_ || otherMembers_->contains(user->name());
}

void Group::load(struct group *entry)
{
    if (entry) {
        exists_ = true;
        id_ = entry->gr_gid;
        name_ = entry->gr_name;
        otherMembers_ = StringList::create();
        char **pcs = entry->gr_mem;
        while (*pcs) {
            otherMembers_->append(*pcs);
            ++pcs;
        }
    }
    else {
        exists_ = false;
        id_ = 0;
    }
}

} // namespace cc

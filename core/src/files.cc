/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */


#include <cc/files>

namespace cc {

String str(Access access)
{
    String s;
    if (+access) {
        auto l = StringList::create();
        if (+(access & Access::Read))    l << "Access::Read";
        if (+(access & Access::Write))   l << "Access::Write";
        if (+(access & Access::Execute)) l << "Access::Execute";
        if (+(access & Access::Exists))  l << "Access::Exists";
        s = l->join("|");
    }
    else {
        s = "Access::None";
    }
    return s;
}

String str(FileMode mode)
{
    String s;
    if (+mode) {
        auto l = StringList::create();
        if (+(mode & FileMode::SetUserId))  l << "FileMode::SetUserId";
        if (+(mode & FileMode::SetGroupId)) l << "FileMode::SetGroupId";
        if (+(mode & FileMode::StickyBit))  l << "FileMode::StickyBit";
        if (+(mode & FileMode::UserRead))   l << "FileMode::UserRead";
        if (+(mode & FileMode::UserWrite))  l << "FileMode::UserWrite";
        if (+(mode & FileMode::UserExec))   l << "FileMode::UserExec";
        if (+(mode & FileMode::GroupRead))  l << "FileMode::GroupRead";
        if (+(mode & FileMode::GroupWrite)) l << "FileMode::GroupWrite";
        if (+(mode & FileMode::GroupExec))  l << "FileMode::GroupExec";
        if (+(mode & FileMode::OtherRead))  l << "FileMode::OtherRead";
        if (+(mode & FileMode::OtherWrite)) l << "FileMode::OtherWrite";
        if (+(mode & FileMode::OtherExec))  l << "FileMode::OtherExec";
        s = l->join("|");
    }
    else {
        s = "FileMode::None";
    }
    return s;
}

String str(FileType type)
{
    String s;
    switch (type) {
        case FileType::Regular    : s = "FileType::Regular"; break;
        case FileType::Directory  : s = "FileType::Directory"; break;
        case FileType::CharDevice : s = "FileType::CharDevice"; break;
        case FileType::BlockDevice: s = "FileType::BlockDevice"; break;
        case FileType::Fifo       : s = "FileType::Fifo"; break;
        case FileType::Symlink    : s = "FileType::Symlink"; break;
        case FileType::Socket     : s = "FileType::Socket"; break;
    };
    return s;
}

String str(OpenMode mode)
{
    String s;
    if (+mode) {
        auto l = StringList::create();
        if (+(mode & OpenMode::ReadOnly))  l << "OpenMode::ReadOnly";
        if (+(mode & OpenMode::WriteOnly)) l << "OpenMode::WriteOnly";
        if (+(mode & OpenMode::ReadWrite)) l << "OpenMode::ReadWrite";
        if (+(mode & OpenMode::Append))    l << "OpenMode::Append";
        if (+(mode & OpenMode::Create))    l << "OpenMode::Create";
        if (+(mode & OpenMode::Truncate))  l << "OpenMode::Truncate";
        if (+(mode & OpenMode::Virgin))    l << "OpenMode::Virgin";
        s = l->join("|");
    }
    else {
        s = "OpenMode::None";
    }
    return s;
}

String str(Seek method)
{
    String s;
    switch (method) {
        case Seek::Begin  : s = "Seek::Begin"; break;
        case Seek::Current: s = "Seek::Current"; break;
        case Seek::End    : s = "Seek::End"; break;
    };
    return s;
}

} // namespace cc

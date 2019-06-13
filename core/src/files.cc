/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */


#include <cc/files>

namespace cc {

String str(FileAccess flags)
{
    String s;
    if (+flags) {
        auto l = StringList::create();
        if (+(flags & FileAccess::Read))    l << "FileAccess::Read";
        if (+(flags & FileAccess::Write))   l << "FileAccess::Write";
        if (+(flags & FileAccess::Execute)) l << "FileAccess::Execute";
        if (+(flags & FileAccess::Exists))  l << "FileAccess::Exists";
        s = l->join("|");
    }
    else {
        s = "FileAccess::None";
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

String str(FileOpen mode)
{
    String s;
    if (+mode) {
        auto l = StringList::create();
        if (+(mode & FileOpen::ReadOnly))  l << "FileOpen::ReadOnly";
        if (+(mode & FileOpen::WriteOnly)) l << "FileOpen::WriteOnly";
        if (+(mode & FileOpen::ReadWrite)) l << "FileOpen::ReadWrite";
        if (+(mode & FileOpen::Append))    l << "FileOpen::Append";
        if (+(mode & FileOpen::Create))    l << "FileOpen::Create";
        if (+(mode & FileOpen::Truncate))  l << "FileOpen::Truncate";
        if (+(mode & FileOpen::Virgin))    l << "FileOpen::Virgin";
        s = l->join("|");
    }
    else {
        s = "FileOpen::None";
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

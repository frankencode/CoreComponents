/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/File>
#include <cc/MountTable>

namespace cc {

Ref<MountTable> MountTable::open(const String &path)
{
    String text;
    {
        Ref<File> file;
        if (path == String{}) {
            #ifdef __linux
            file = File::tryOpen("/proc/self/mounts");
            if (!file)
            #endif
            file = File::open("/etc/mtab");
        }
        else
            file = File::open(path);

        text = file->readAll(); // FIXME: file->map()
    }
    return Object::create<MountTable>(text);
}

MountTable::MountTable(const String &text):
    lines_{StringList::create()},
{
    for (String line: text->split("\n")) {
        line->simplifyInsitu();
        if (line->count() == 0) continue;
        if (line->at(0) == '#') continue;
        lines_->append(line);
    }
}

Entry::Entry(const String &line):
    parts_(line->split(" "))
{
    while (parts_->count() < 4) parts_->append(String{});
}

} // namespace cc

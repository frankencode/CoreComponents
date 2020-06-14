/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/MountTable>
#include <cc/File>

namespace cc {

Ref<MountTable> MountTable::open(const String &path)
{
    String text;
    {
        File file;
        if (path == String{}) {
            #ifdef __linux
            try { file = File{"/proc/self/mounts"}; }
            catch (...)
            #endif
            {
                file = File{"/etc/mtab"};
            }
        }
        else
            file = File{path};

        text = file->readAll();
    }
    return new MountTable{text};
}

Ref<MountTable> MountTable::parse(const String &text)
{
    return new MountTable{text};
}

MountTable::MountTable(const String &text)
{
    for (String line: text->split("\n")) {
        mutate(line)->simplifyInsitu();
        if (line->count() == 0) continue;
        if (line->at(0) == '#') continue;
        lines_->append(line);
    }
}

MountTable::Entry::Entry(const String &line):
    parts_{line->split(" ")}
{
    while (parts_->count() < 4) parts_->append(String{});
}

} // namespace cc

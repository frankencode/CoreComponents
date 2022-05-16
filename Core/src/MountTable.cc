/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/MountTable>
#include <cc/File>
#include <cc/DEBUG>

namespace cc {

MountPoint::MountPoint(const String &line):
    parts_{line.split(' ')}
{
    while (parts_.count() < 4) parts_.append(String{});
}

MountTable::MountTable(const String &path)
{
    readLines(loadText(path));
}

MountTable MountTable::read(const String &text)
{
    MountTable table;
    table.readLines(text);
    return table;
}

String MountTable::loadText(const String &path)
{
    String text;
    {
        File file;
        if (path == "") {
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

        text = file.readAll();
    }
    return text;
}

void MountTable::readLines(const String &text)
{
    for (String line: text.split('\n')) {
        line.simplify();
        if (line.count() == 0) continue;
        if (line.at(0) == '#') continue;
        (*this) << MountPoint{line};
    }
}

String MountTable::toString() const
{
    List<String> lines;
    for (const auto &entry: *this)
        lines << entry.toString();
    return lines;
}

} // namespace cc

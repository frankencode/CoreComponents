/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/LineSource>
#include <cc/Heap>

using namespace cc;

int main()
{
    StringList list;
    Ref<LineSource> source = LineSource::open(stdIn());
    for (String item; source->read(&item);)
        list->append(item);

    list = list->sort();

    for (const String &item: list)
        fout() << item << nl;

    return 0;
}

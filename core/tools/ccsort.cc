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
    Ref<StringList> list = StringList::create();
    Ref<LineSource> source = LineSource::open(stdIn());
    for (string item; source->read(&item);)
        list->append(item);

    list = list->sort();

    for (int i = 0; i < list->count(); ++i)
        fout() << list->at(i) << nl;

    return 0;
}

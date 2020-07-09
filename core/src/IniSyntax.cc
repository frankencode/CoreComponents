/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/IniSyntax>
#include <cc/LineSource>

namespace cc {

void IniSyntax::parse(const String &text)
{
    LineSource source{text};

    for (String line; source->read(&line);) {
        mutate(line)->trimInsitu(" \t", " \t");
        if (line->count() == 0) {
            // ignore empty line
        }
        else if (line->startsWith('[')) {
            mutate(line)->trimInsitu("[ \t", "] \t");
            enterSection(line);
        }
        else if (line->startsWith(';')) {
            // ignore comment
        }
        else {
            int i = 0;
            if (line->find('=', &i)) {
                String key = line->copy(0, i);
                String value = line->copy(i + 1, line->count());
                mutate(key)->trimInsitu("", " \t");
                mutate(value)->trimInsitu(" \t", "");
                establish(key, value);
            }
            else {
                handleError(source->offset(), line);
            }
        }
    }
}

} // namespace cc

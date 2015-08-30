/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio>
#include <flux/check>
#include <flux/String>
#include <flux/Unicode>

using namespace flux;

int main()
{
    {
        String s = "Übertragung";
        check(s != "123");
        check("X" < s);

        fout("s = \"%%\"\n") << s;
        fout("Unicode::open(s)->count() = %%\n") << Unicode::open(s)->count();
        fout("s->size() = %%\n") << s->count();
        fout("s->copy() = \"%%\"\n") << s->copy();

        Ref<StringList> parts = s->split("a");
        fout("s.split(\"a\") = [\n");
        for (int i = 0; i < parts->count(); ++i)
            fout("  \"%%\"\n") << parts->at(i);
        fout("]\n");
    }
    {
        String s = "bin/testPath";
        // fout("s = \"%%\"\n") << s;
        fout("s->find(\"/\") = %%\n") << s->find("/");
        Ref<StringList> parts = s->split("/");
        fout("s.split(\"/\") = [\n");
        for (int i = 0; i < parts->count(); ++i)
            fout("  \"%%\"\n") << parts->at(i);
        fout("]\n");
    }
    {
        String s = "Hallo!, \n\\u041F\\u0440\\u0438\\u0432\\u0435\\u0442!, \\ud834\\udd22, Hello!";
        fout("s = \"%%\"\n") << s;
        String se = s->unescape();
        fout("Unicode::open(se)->at(17) = 0x%%\n") << hex(Unicode::open(se)->at(17), 2);
        fout("se = \"%%\"\n") << se;
    }
    {
        String s = "..Привет, Привет!";
        s->replaceInsitu("Привет", "Hallo");
        fout("s = \"%%\"\n") << s;
    }
    return 0;
}

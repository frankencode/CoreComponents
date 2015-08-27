/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio>
#include <flux/Process>

using namespace flux;

int main(int argc, char **argv)
{
    fout("argv[0] = \"%%\"\n") << argv[0];
    fout("String(argv[0])->fileName() = \"%%\"\n") << String(argv[0])->fileName();
    fout("String(argv[0])->isAbsolutePath() = %%\n") << String(argv[0])->isAbsolutePath();
    fout("String(argv[0])->absolutePathRelativeTo(\"%%\") = \"%%\"\n") << Process::cwd() << String(argv[0])->absolutePathRelativeTo(Process::cwd());
    {
        String path = String(argv[0])->absolutePathRelativeTo(Process::cwd());
        while (path != "/") {
            path = path->reducePath();
            fout("path1->reduce() = \"%%\"\n") << path;
        }
    }
    {
        String path = argv[0];
        while (path->contains('/')) {
            path = path->reducePath();
            fout("path2->reduce() = \"%%\"\n") << path;
        }
    }
    return 0;
}

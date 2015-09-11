/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/testing/TestSuite>
#include <flux/stdio>
#include <flux/Process>

using namespace flux;
using namespace flux::testing;

class PathManipulation: public TestCase
{
    void run()
    {
        String execPath = testSuite()->argv()[0];
        fout("argv[0] = \"%%\"\n") << execPath;
        fout("String(argv[0])->fileName() = \"%%\"\n") << execPath->fileName();
        fout("String(argv[0])->isAbsolutePath() = %%\n") << execPath->isAbsolutePath();
        fout("String(argv[0])->absolutePathRelativeTo(\"%%\") = \"%%\"\n") << Process::cwd() << execPath->absolutePathRelativeTo(Process::cwd());
        {
            String path = execPath->absolutePathRelativeTo(Process::cwd());
            while (path != "/") {
                path = path->reducePath();
                fout("path1->reduce() = \"%%\"\n") << path;
            }
        }
        {
            String path = execPath;
            while (path->contains('/')) {
                path = path->reducePath();
                fout("path2->reduce() = \"%%\"\n") << path;
            }
        }
    }
};

int main(int argc, char **argv)
{
    FLUX_TESTSUITE_ADD(PathManipulation);

    return testSuite()->run(argc, argv);
}

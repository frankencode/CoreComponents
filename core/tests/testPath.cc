/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/Process>

using namespace cc;
using namespace cc::testing;

class PathManipulation: public TestCase
{
    void run()
    {
        String execPath = TestSuite::instance()->execPath();
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
        fout() << "String(\"/\")->expandPath(\"/usr\") = \"" << String("/")->expandPath("/usr") << "\"" << nl;
        CC_VERIFY(String("/")->expandPath("/usr") == "/usr");
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(PathManipulation);

    return TestSuite::instance()->run(argc, argv);
}

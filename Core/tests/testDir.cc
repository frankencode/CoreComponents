#include <cc/Dir>
#include <cc/DirWalk>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "CwdIterate",
        []{
            for (auto name: Dir{"."})
                fout() << name << nl;
        }
    };

    TestCase {
        "CwdTraverse",
        []{
            for (auto name: DirWalk{"."})
                fout() << name << nl;
        }
    };

    return TestSuite{argc, argv}.run();
}

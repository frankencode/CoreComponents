#include <cc/testing>
#include <cc/Glob>

using namespace cc;

int main(int argc, char *argv[])
{
    TestCase {
        "Glob",
        []{
            Glob glob{"*.(so|dll)"};
            for (auto &s: glob)
                fout() << s << nl;

            CC_INSPECT(glob.explain());
        }
    };

    return TestSuite{argc, argv}.run();
}

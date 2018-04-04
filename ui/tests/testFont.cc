#include <cc/debug>
#include <cc/testing/TestSuite>
#include <cc/ui/Font>

using namespace cc;
using namespace cc::ui;
using namespace cc::testing;

class FontTest: public TestCase
{
    void run() override
    {
        CC_INSPECT(sizeof(Font));
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(FontTest);

    return TestSuite::instance()->run(argc, argv);
}

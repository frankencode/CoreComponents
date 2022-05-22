#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "IntegerSizes",
        []{
            CC_INSPECT(sizeof(long));
        }
    };

    return TestSuite{argc, argv}.run();
};

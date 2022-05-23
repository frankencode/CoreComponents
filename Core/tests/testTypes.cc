#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "IntegerSizes",
        []{
            CC_INSPECT(sizeof(int));
            CC_INSPECT(sizeof(long));
            CC_INSPECT(sizeof(double));
        }
    };

    return TestSuite{argc, argv}.run();
};

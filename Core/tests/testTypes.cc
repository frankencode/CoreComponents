#include <cc/blist/Tree>
#include <cc/blist/Vector>
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

    TestCase {
        "BlistTypes",
        []{
            CC_INSPECT(sizeof(blist::Tree<16>::Node));
            CC_INSPECT(sizeof(blist::Tree<16>::Branch));
            CC_INSPECT(sizeof(blist::Vector<void*, 16>::Leaf));
            CC_INSPECT(sizeof(blist::Tree<32>::Node));
            CC_INSPECT(sizeof(blist::Tree<32>::Branch));
            CC_INSPECT(sizeof(blist::Vector<void*, 32>::Leaf));
        }
    };

    return TestSuite{argc, argv}.run();
};

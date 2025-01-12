#include <cc/buckets/BucketTree>
#include <cc/buckets/BucketVector>
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
        "BucketTypes",
        []{
            CC_INSPECT(sizeof(cc::BucketTree<16>::Node));
            CC_INSPECT(sizeof(cc::BucketTree<16>::Branch));
            CC_INSPECT(sizeof(cc::BucketVector<void*, 16>::Leaf));
            CC_INSPECT(sizeof(cc::BucketTree<32>::Node));
            CC_INSPECT(sizeof(cc::BucketTree<32>::Branch));
            CC_INSPECT(sizeof(cc::BucketVector<void*, 32>::Leaf));
        }
    };

    return TestSuite{argc, argv}.run();
};

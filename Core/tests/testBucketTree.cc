#include <cc/buckets/BucketTree>
#include <cc/Locator>
#include <cc/DEBUG>

namespace cc::buckets { template class BucketTree<long>; };

int main(int argc, char *argv[])
{
    using namespace cc::buckets;
    using namespace cc;

    CC_INSPECT(sizeof(BucketTree<long>::Branch));
    CC_INSPECT(sizeof(BucketTree<long>::Leaf));
    CC_INSPECT(sizeof(BucketTree<long>));
    CC_INSPECT(sizeof(Locator));

    BucketTree<long> numbers;

    numbers.contains(0);

    return 0;
}

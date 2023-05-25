#include <cc/buckets/BucketPrefixTree>
#include <cc/SetNext>
#include <cc/String>
#include <cc/Random>
#include <cc/testing>

namespace cc::buckets { template class BucketPrefixTree<long, 8>; }
namespace cc::buckets { template class BucketPrefixTree<cc::String, 16>; }

int main(int argc, char *argv[])
{
    using namespace cc::buckets;
    using namespace cc;

    TestCase {
        "SetSizes",
        []{
            CC_INSPECT(sizeof(BucketPrefixTree<long, 16>::Branch));
            CC_INSPECT(sizeof(BucketPrefixTree<long, 16>::Leaf));
        }
    };

    TestCase {
        "SetInsertAsc",
        []{
            const int n = 256;
            SetNext<int, 8> set;
            for (int i = 0; i < n; ++i) {
                set.insert(i);
            }
            CC_CHECK(set.count() == n);
            for (int i = 0; i < n; ++i) {
                CC_VERIFY(set.contains(i));
            }
            CC_INSPECT(set.tree().Granularity);
            CC_INSPECT(set.tree().height_);
        }
    };

    TestCase {
        "SetInsertDsc",
        []{
            const int n = 256;
            SetNext<int, 8> set;
            for (int i = 0; i < n; ++i) {
                set.insert(n - i - 1);
            }
            CC_CHECK(set.count() == n);
            for (int i = 0; i < n; ++i) {
                CC_VERIFY(set.contains(i));
            }
            CC_INSPECT(set.tree().Granularity);
            CC_INSPECT(set.tree().height_);
        }
    };

    TestCase {
        "SetInsertRnd",
        []{
            const int n = 65536;
            Array<int> pad = Array<int>::allocate(n);
            for (int i = 0; i < n; ++i) {
                pad[i] = i;
            }
            Random{0}.shuffle(pad);
            SetNext<int, 8> set;
            for (int i = 0; i < n; ++i) {
                set.insert(pad[i]);
            }
            CC_CHECK(set.count() == n);
            for (int i = 0; i < n; ++i) {
                CC_VERIFY(set.contains(i));
            }
            CC_INSPECT(set.tree().Granularity);
            CC_INSPECT(set.tree().height_);
            CC_INSPECT(set.tree().checkNodeFills());

            #if 0
            set.tree().checkBranches([](auto *branch){
                if (branch->fill_ < 6) {
                    CC_DEBUG;
                    CC_INSPECT((void *)branch);
                    CC_INSPECT(branch->fill_);
                    CC_INSPECT((void *)branch->succ_);
                    CC_INSPECT((void *)branch->pred_);
                }
                return true;
            });
            #endif

            #if 0
            set.tree().checkLeafs([](auto *leaf){
                if (leaf->
                CC_CHECK(
                CC_DEBUG;
                CC_INSPECT((void *)leaf);
                for (unsigned k = 0; k < leaf->fill_; ++k) CC_INSPECT(leaf->at(k));
                return true;
            });
            #endif
        }
    };

    return TestSuite{argc, argv}.run();

    #if 0
    using namespace cc::buckets;
    using namespace cc;

    BucketPrefixTree<long> tree;

    CC_INSPECT(sizeof(BucketPrefixTree<long, 16>::Branch));
    CC_INSPECT(sizeof(BucketPrefixTree<long, 16>::Leaf));

    SetNext<int> set;
    // for (int i = 0; i < 256; ++i) set.insert(i);
    for (int i = 0; i < 256; ++i) {
        CC_INSPECT(255-i);
        set.insert(255-i);
    }

    for (int i = 0; i < 256; ++i) {
        CC_INSPECT(i);
        CC_INSPECT(set.contains(i));
    }

    #if 0
    set.forEach([](int x){
        CC_INSPECT(x);
    });
    #endif

    set.tree_.checkLeafs([](auto *leaf){
        CC_DEBUG;
        CC_INSPECT((void *)leaf);
        for (unsigned k = 0; k < leaf->fill_; ++k) CC_INSPECT(leaf->at(k));
        return true;
    });
    #endif

    return 0;
}

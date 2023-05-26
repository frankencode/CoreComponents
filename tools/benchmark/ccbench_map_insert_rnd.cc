#include <cc/Array>
#include <cc/Map>
#include <cc/System>
#include <cc/Random>
#include <cc/File>
#include <cc/stdio>
#include <cc/DEBUG>
#include <cmath>

int main(int argc, char *argv[])
{
    using namespace cc;

    const int n = argc > 1 ? String{argv[1]}.toInt() : 100000;

    Array<long> items = Array<long>::allocate(n);

    fout() << "Test preparation: generate shuffled random numbers... ";
    {
        double t = System::now();

        Random random { 0 };

        for (int i = 0; i < n; ++i) {
            items[i] = random();
        }

        for (int i = 0; i < 3; ++i) {
            random.shuffle(items);
        }

        t = System::now() - t;
        fout() << std::round(t * 1000) << " ms\n";
    }

    Map<long> map;

    fout() << n << " random insertions into cc::Map<long>... ";
    {
        double t = System::now();

        for (int i = 0; i < n; ++i) {
            map.insert(items[i], i);
        }

        t = System::now() - t;
        fout() << std::round(t * 1000) << " ms\n";
    }

    fout() << n << " random lookups... ";
    {
        double t = System::now();

        for (int i = 0; i < n; ++i) {
            if (!map.contains(items[i])) {
                ferr() << "FAILED" << nl;
                return 1;
            }
        }

        t = System::now() - t;
        fout() << std::round(t * 1000) << " ms\n";
    }

    #if 0 // FIXME: move to BucketIndexTree
    std::function checkFill {[](const buckets::BucketIndexTree::Node *node) -> bool {
        // CC_INSPECT(node->fill_);
        /*if (node->fill_ < 12) {
            CC_INSPECT(node->fill_);
            CC_INSPECT(node->parent_ != nullptr);
            CC_INSPECT(node->pred_ != nullptr);
            CC_INSPECT(node->succ_ != nullptr);
            CC_DEBUG;
        }*/
        return node->fill_ >= (3 * buckets::BucketIndexTree::Capacity / 4) || !node->parent_  || !node->pred_ || !node->succ_;
    }};

    CC_INSPECT(map.tree().check(checkFill));
    #endif

    return 0;
}

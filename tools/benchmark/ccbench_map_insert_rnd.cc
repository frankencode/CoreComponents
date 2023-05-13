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

    Map<int> numbers;

    fout() << n << " random insertions into cc::bucket::Map<int>... ";
    {
        double t = System::now();
        Random random { 0 };
        for (int i = 0; i < n; ++i) {
            int r = random.get();
            numbers.insert(r, i);
        }
        t = System::now() - t;
        fout() << std::round(t * 1000) << " ms\n";
    }

    std::function checkFill {[](const buckets::BucketIndexTree::Node *node) -> bool {
        /*if (node->fill_ < 12) {
            CC_INSPECT(node->fill_);
            CC_INSPECT(node->parent_ != nullptr);
            CC_INSPECT(node->pred_ != nullptr);
            CC_INSPECT(node->succ_ != nullptr);
            CC_DEBUG;
        }*/
        return node->fill_ >= 12 || !node->parent_  || !node->pred_ || !node->succ_;
    }};

    CC_INSPECT(numbers.tree().check(checkFill));

    fout() << n << " random lookups... ";
    {
        Random random { 0 };
        double t = System::now();
        for (int i = 0; i < n; ++i) {
            int r = random();
            if (!numbers.contains(r)) {
                ferr() << "FAILED" << nl;
                return 1;
            }
        }
        t = System::now() - t;
        fout() << std::round(t * 1000) << " ms\n";
    }

    return 0;
}

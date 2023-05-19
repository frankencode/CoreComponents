#include <cc/Map>
#include <cc/System>
#include <cc/Random>
#include <cc/stdio>
#include <cc/DEBUG>
#include <cmath>

int main(int argc, char *argv[])
{
    using namespace cc;

    const int n = argc > 1 ? String{argv[1]}.toInt() : 1000000;

    Map<int> numbers;

    fout() << "sizeof(buckets::Branch) = " << sizeof(buckets::BucketIndexTree::Branch) << nl;
    fout() << "sizeof(buckets::BucketVector<KeyValue<int,int>>::Leaf) = " << sizeof(buckets::BucketVector<KeyValue<int, int>>::Leaf) << nl;
    fout() << "sizeof(buckets::BucketVector<KeyValue<long,long>>::Leaf) = " << sizeof(buckets::BucketVector<KeyValue<long, long>>::Leaf) << nl;

    fout() << "Ascending insertion into cc::Map<int>... ";
    {
        double t = System::now();
        for (int i = 0; i < n; ++i) {
            numbers.insert(i, i);
        }
        t = System::now() - t;
        fout() << std::round(t * 1000) << " ms\n";
    }

    fout() << n << " lookups... ";
    {
        double t = System::now();
        for (int i = 0; i < n; ++i) {
            if (!numbers.contains(i)) return 1;
        }
        t = System::now() - t;
        fout() << std::round(t * 1000) << " ms\n";
    }

    CC_INSPECT(numbers.tree().isDense());

    return 0;
}

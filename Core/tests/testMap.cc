#include <cc/Map>
#include <cc/testing>

namespace cc { template class Map<int>; }

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "InsertionOperator",
        []{
            Map<int> m;
            m(0) = 1;
            m(1) = 2;
            CC_CHECK(m.count() == 2);
            CC_CHECK(m(0) == 1);
            CC_CHECK(m(1) == 2);
        }
    };

    TestCase {
        "InsertionRemoval",
        []{
            Map<int> map;
            const int n = 20;
            {
                Random random{0};
                for (int i = 0; i < n; ++i) {
                    int key = random.get();
                    int value = random.get();
                    map.insert(key, value);
                }
            }
            CC_CHECK(map.count() == 20);
            {
                Random random{0};
                for (int i = 0; i < n; ++i) {
                    int key = random.get();
                    int value = random.get();
                    CC_VERIFY(map.value(key) == value);
                }
            }
            {
                Random random{0};
                for (int i = 0; i < n; ++i) {
                    int j = random.get(0, map.count() - 1);
                    map.remove(map.at(j).key());
                }
            }
            CC_CHECK(map.count() == 0);
        }
    };

    TestCase {
        "MorphToList",
        []{
            Map<int> map;
            const int n = 5;
            {
                Random random { 0 };
                for (int i = 0; i < n; ++i) {
                    int key = random.get();
                    int value = random.get();
                    map.insert(key, value);
                }
            }

            List<KeyValue<int>> list { std::move(map) };
            {
                Random random { 0 };
                for (int i = 0; i < n; ++i) {
                    int key = random.get();
                    int value = random.get();
                    CC_CHECK(list.find(KeyValue{key, value}));
                }
            }
        }
    };

    return TestSuite{argc, argv}.run();
}

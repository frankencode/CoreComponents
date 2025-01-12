#include <cc/MultiMap>
#include <cc/testing>

namespace cc { template class MultiMap<int>; }

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "Insertion",
        []{
            MultiMap<int> m;
            m.insert(0, 1);
            m.insert(0, 2);
            m.insert(1, 3);
            m.insert(1, 4);
            CC_CHECK(m.at(0).key() == 0);
            CC_CHECK(m.at(0).value() == 1);
            CC_CHECK(m.at(1).key() == 0);
            CC_CHECK(m.at(1).value() == 2);
            CC_CHECK(m.at(2).key() == 1);
            CC_CHECK(m.at(2).value() == 3);
            CC_CHECK(m.at(3).key() == 1);
            CC_CHECK(m.at(3).value() == 4);
        }
    };

    TestCase {
        "InsertionRemoval",
        []{
            MultiMap<int> map;
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
                    Locator pos;
                    CC_VERIFY(map.find(key, &pos));
                    CC_VERIFY(map.at(pos).value() == value);
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
            MultiMap<int> map;
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
                    CC_CHECK(list.find(KeyValue{random.get(), random.get()}));
                }
            }
        }
    };

    return TestSuite{argc, argv}.run();
}

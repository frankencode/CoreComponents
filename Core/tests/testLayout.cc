#include <cc/Layout>
#include <cc/Random>
#include <cc/testing>

namespace cc {

template class Layout<None, double>;

void testLayout(const auto &layout)
{
    #ifndef NDEBUG
    CC_VERIFY(layout->testStructure());
    CC_VERIFY(layout->testIteration());
    CC_VERIFY(layout->testBalance());
    CC_VERIFY(layout->testWeights());
    #endif
}

} // namespace cc

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "LayoutBasicTest",
        []{
            using TestLayout = Layout<String, int>;

            TestLayout layout;
            layout.pushBack("Item 1", 2);
            layout.pushBack("Item 2", 3);
            layout.pushBack("Item 3", 1);
            layout.pushBack("Item 4", 4);
            layout.pushBack("Item 5", 1);

            testLayout(layout);

            CC_CHECK(layout.count() == 5);


            {
                for (long i = 0; i < layout.count(); ++i) {
                    auto stop = layout[i];
                    fout() << stop.item() << " (" << stop.extent() << "): " << stop.pos() << ".." << stop.pos() + stop.extent() - 1  << nl;
                }
                fout() << nl;
            }

            {
                for (auto stop: layout(3, 10)) {
                    fout()
                        << stop.item() << " (" << stop.extent() << "): "
                        << stop.pos() << ".." << stop.pos() + stop.extent() - 1 << nl;
                }
                fout() << nl;
            }
        }
    };

    TestCase {
        "LayoutTreeTest",
        []{
            Layout<int, int> layout;

            const int n = 32;
            Random random;
            for (int i = 0; i < n; ++i) {
                layout.pushBack(i, random.get(1, 10));
                CC_INSPECT(layout.count());
                testLayout(layout);
            }
        }
    };

    return TestSuite{argc, argv}.run();
}

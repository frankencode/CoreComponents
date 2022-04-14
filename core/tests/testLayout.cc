#include <cc/Layout>
#include <cc/Random>
#include <cc/testing>

namespace cc {

template class Layout<None, double>;

void testLayout(const auto &layout)
{
    CC_VERIFY(layout->testStructure());
    CC_VERIFY(layout->testIteration());
    CC_VERIFY(layout->testBalance());
    CC_VERIFY(layout->testWeights());
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
            layout.append("Item 1", 2);
            layout.append("Item 2", 3);
            layout.append("Item 3", 1);
            layout.append("Item 4", 4);
            layout.append("Item 5", 1);

            testLayout(layout);

            CC_CHECK(layout.count() == 5);

            {
                for (long i = 0; i < layout.count(); ++i) {
                    String item;
                    int extent = 0;
                    int pos = layout.getPosAt(i);
                    layout.getItemExtentAt(i, &item, &extent);
                    fout() << item << " (" << extent << "): " << pos << ".." << pos + extent - 1  << nl;
                }
                fout() << nl;
            }

            {
                long i0 = 0, i1 = 0;
                int pos = 0;
                layout.getView(3, 10, &i0, &i1, &pos);
                CC_INSPECT(i0);
                CC_INSPECT(i1);
                CC_CHECK(i0 == 1 && i1 == 4);
                for (long i = i0; i < i1; ++i) {
                    String item;
                    int extent = 0;
                    layout.getItemExtentAt(i, &item, &extent);
                    fout() << item << " (" << extent << "): " << pos << ".." << pos + extent - 1 << nl;
                    pos += extent;
                }
                fout() << nl;
            }

            {
                for (const auto &stop: layout.view(3, 10)) {
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
                layout.append(i, random.get(1, 10));
                CC_INSPECT(layout.count());
                testLayout(layout);
            }
        }
    };

    return TestSuite{argc, argv}.run();
}

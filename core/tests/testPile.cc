#include <cc/Pile>
#include <cc/testing>

namespace cc { template class Pile<None, double>; }

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "SimplePileTest",
        []{
            using TestPile = Pile<String, int>;

            TestPile layout;
            layout.append("Item 1", 2);
            layout.append("Item 2", 3);
            layout.append("Item 3", 1);
            layout.append("Item 4", 4);
            layout.append("Item 5", 1);

            CC_VERIFY(layout.count() == 5);

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
                CC_VERIFY(i0 == 1 && i1 == 4);
                for (long i = i0; i < i1; ++i) {
                    String item;
                    int extent = 0;
                    layout.getItemExtentAt(i, &item, &extent);
                    fout() << item << " (" << extent << "): " << pos << ".." << pos + extent - 1 << nl;
                    pos += extent;
                }
                fout() << nl;
            }
        }
    };

    return TestSuite{argc, argv}.run();
}

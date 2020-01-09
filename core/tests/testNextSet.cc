#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/debug>
#include <cc/NextSet>

using namespace cc;
using namespace cc::testing;

void print(const NextSet<int> *set)
{
    Format f;
    for (int i = 0; i < set->count(); ++i)
        f << set->at(i);
    ferr() << "(" << f->join(", ") << ")" << nl;
}

int main(int argc, char **argv)
{
    Local<NextSet<int>> set;
    print(set);
    for (int i = 63; i >= 0; --i) {
        set->insert(i);
        print(set);
    }

    return 0;
}

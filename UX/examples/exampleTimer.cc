#include <cc/Timer>
#include <cc/System>
#include <cc/stdio>
#include <cmath>

int main()
{
    using namespace cc;

    return
        Timer{1}
        .onTimeout([=]{
            double t = System::now();
            fout() << fixed(t, 6) << nl;
        })
        .startAt(std::ceil(System::now()))
        .run();
}

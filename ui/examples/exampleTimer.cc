#include <cc/Application>
#include <cc/System>
#include <cc/stdio>
#include <cmath>

int main()
{
    using namespace cc;

    Timer timer{1};
    timer.onTimeout([=]{
        double t = System::now();
        fout() << fixed(t, 6) << nl;
    });
    timer.startAt(std::ceil(System::now()));

    return Application{}.run();
}

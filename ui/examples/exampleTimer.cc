#include <cc/ui/Application>
#include <cc/System>
#include <cc/stdio>
#include <cmath>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    Timer timer{1};
    timer.onTimeout([=]{
        double t = System::now();
        fout() << fixed(t, 6) << nl;
    });
    timer.startAt(std::ceil(System::now()));

    return Application{}.run();
}

#include <cmath>
#include <cc/stdio>
#include <cc/System>
#include <cc/str>
#include <cc/ui/Application>
#include <cc/ui/Timer>

using namespace cc;
using namespace cc::ui;

int main(int argc, char **argv)
{
    auto app = Application::open(argc, argv);

    Timer::startAt(std::ceil(System::now()), 1, []{
        fout() << fixed(System::now(), 6) << nl;
    });

    return app->run();
}

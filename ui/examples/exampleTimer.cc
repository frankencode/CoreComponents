#include <cc/ui/Application>
#include <cc/ui/Timer>
#include <cc/System>
#include <cc/stdio>
#include <cc/str>

using namespace cc;
using namespace cc::ui;

int main(int argc, char **argv)
{
    Timer::startAt(std::ceil(System::now()), 1, []{
        fout() << fixed(System::now(), 6) << nl;
    });

    return Application{}->run();
}

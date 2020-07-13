#include <cc/ui/Application>
#include <cc/ui/Timer>
#include <cc/System>
#include <cc/stdio>
#include <cc/str>

using namespace cc;
using namespace cc::ui;

int main(int argc, char **argv)
{
    Timer{1, []{
        fout() << fixed(System::now(), 6) << nl;
    }}->startAt(std::ceil(System::now()));

    return Application{}->run();
}

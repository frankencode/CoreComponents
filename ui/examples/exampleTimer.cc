#include <cc/ui/Application>
#include <cc/ui/Timer>
#include <cc/System>
#include <cc/stdio>

int main(int argc, char **argv)
{
    using namespace cc;
    using namespace cc::ui;

    Timer timer{1};
    timer->timeout >>[=]{
        fout() << fixed(System::now(), 6) << nl;
    };
    timer->startAt(std::ceil(System::now()));

    return Application{}->run();
}

#include <cc/ui/TextField>

using namespace cc;
using namespace cc::ui;

class MainView: public View
{
    friend class Object;

    MainView()
    {
        size = Size { 640, 480 };
        inheritPaper();

        add<TextField>("First name")
            ->centerInParent();
    }
};

int main(int argc, char **argv)
{
    Window::open<MainView>("Hello, world!", WindowMode::Accelerated|WindowMode::VSync);
    return Application{}->run();
}

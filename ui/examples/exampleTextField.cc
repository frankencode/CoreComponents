#include <cc/ui/Application>
#include <cc/ui/TextField>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    View view{640, 480};
    view->inheritPaper();

    TextField field{"First name"};
    field->centerInParent();

    view << field;

    Window::open(view, "Hello, world!", WindowMode::Accelerated|WindowMode::VSync);
    return Application{}->run();
}

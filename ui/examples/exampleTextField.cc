#include <cc/ui/Application>
#include <cc/ui/TextField>
#include <cc/DEBUG>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    View view{640, 480};
    view->inheritPaper();

    TextField field{"First name"};
    field->centerInParent();
    field->accepted >>[=]{
        CC_INSPECT(field->inputText());
    };
    view << field;

    Window::open(view, "Hello, world!", WindowMode::Accelerated|WindowMode::VSync);

    return Application{}->run();
}

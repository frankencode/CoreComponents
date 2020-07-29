#include <cc/ui/Application>
#include <cc/ui/PasswordField>
#include <cc/DEBUG>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    View view{640, 480};
    view->inheritPaper();

    PasswordField field{"Enter password"};
    field->centerInParent();
    field->accepted >>[=]{
        CC_INSPECT(field->password());
    };
    view << field;

    Window::open(view, "Hello, world!", WindowMode::Accelerated|WindowMode::VSync);

    return Application{}->run();
}

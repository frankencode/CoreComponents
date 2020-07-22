#include <cc/ui/PasswordField>
#include <cc/DEBUG>

using namespace cc;
using namespace cc::ui;

class MainView: public View
{
    friend class Object;

    MainView()
    {
        size = Size { 640, 480 };
        inheritPaper();

        auto passwordField = add<PasswordField>("Enter password");
        passwordField->centerInParent();
        passwordField->accepted->connect([=]{
            CC_INSPECT(passwordField->password());
        });
    }
};

int main(int argc, char **argv)
{
    Window::open<MainView>("Hello, world!", WindowMode::Accelerated|WindowMode::VSync);
    return Application{}->run();
}

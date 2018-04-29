#include <cc/debug>
#include <cc/ui/Application>
#include <cc/ui/TextField>

using namespace cc;
using namespace cc::ui;

class MainView: public View
{
    friend class Object;

    MainView()
    {
        size = Size { 640, 480 };
        inheritColor();

        TextField *textField = add<TextField>("First name");
        textField->centerInParent();
        textField->hover->connect([=]{ CC_INSPECT(textField->hover()); });
        textField->pressed->connect([=]{ CC_INSPECT(textField->pressed()); });
        textField->focus->connect([=]{ CC_INSPECT(textField->focus()); });
    }
};

int main(int argc, char **argv)
{
    Application::open(argc, argv);
    Window::open<MainView>("Hello, world!");
    return Application::instance()->run();
}

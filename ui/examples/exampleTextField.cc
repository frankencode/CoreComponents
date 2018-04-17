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
        size = Size{640, 480};
        color = Color{"#EDEDED"};

        Ref<TextField> textField = TextField::create(this, "First name");
        textField->centerInParent();

        CC_INSPECT(textField->size());
    }
};

int main(int argc, char **argv)
{
    Application::open(argc, argv);
    Window::open(Object::create<MainView>(), "Hello, world!");
    return Application::instance()->run();
}

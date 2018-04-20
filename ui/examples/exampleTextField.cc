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
        color->bind([=]{ return style()->theme()->backgroundColor(); });

        Ref<TextField> textField = TextField::create(this, "First name");
        textField->centerInParent();
        textField->hover->connect([=]{ CC_INSPECT(textField->hover()); });
        textField->pressed->connect([=]{ CC_INSPECT(textField->pressed()); });

        CC_INSPECT(textField->size());
    }
};

int main(int argc, char **argv)
{
    Application::open(argc, argv);
    Window::open(Object::create<MainView>(), "Hello, world!");
    return Application::instance()->run();
}

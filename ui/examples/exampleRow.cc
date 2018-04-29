#include <cc/debug>
#include <cc/ui/Application>
#include <cc/ui/RowLayout>
#include <cc/ui/ColumnLayout>
#include <cc/ui/StylePlugin>
#include <cc/ui/Label>

using namespace cc;
using namespace cc::ui;

class Item: public Label
{
    friend class Object;

    Item(View *parent, const String &text):
        Label(parent, text)
    {
        margin = dp(20);
    }
};

class MainView: public View
{
    friend class Object;

    MainView()
    {
        size = Size{640, 480};
        color = Color{"#FFFFFF"};

        View *box = View::create(this);
        box->color = Color{"#D0D0FF"};
        box->centerInParent();

        RowLayout::setup(box);

        Object::create<Item>(box, "• Item 1");
        Object::create<Item>(box, "• Item 2");

        View *subBox = View::create(box);
        subBox->color = Color{"#D0FFD0"};

        ColumnLayout::setup(subBox);
        Object::create<Item>(subBox, "• Item A");
        Object::create<Item>(subBox, "• Item B"); // ◦
    }
};

int main(int argc, char **argv)
{
    Application *app = Application::open(argc, argv);
    Window::open(Object::create<MainView>(), "Hello, world!");
    return app->run();
}

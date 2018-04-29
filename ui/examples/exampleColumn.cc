#include <cc/ui/Application>
#include <cc/ui/ColumnLayout>
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
        color = 0xFFFFFF;

        View *box = View::create(this);
        box->color = 0xD0D0FF;
        box->centerInParent();

        ColumnLayout::setup(box);

        Object::create<Item>(box, "• Item 1");
        Object::create<Item>(box, "• Item 2");
        Object::create<Item>(box, "• Item 3");

        {
            View *subBox = View::create(box);
            subBox->color = 0xD0FFD0;

            ColumnLayout::setup(subBox)->indent->bind([=]{
                return style()->defaultFont()->size();
            });

            Object::create<Item>(subBox, "• Item A");
            Object::create<Item>(subBox, "• Item B");
            Object::create<Item>(subBox, "• Item C"); // ◦
        }
    }
};

int main(int argc, char **argv)
{
    Application *app = Application::open(argc, argv);
    Window::open(Object::create<MainView>(), "Hello, world!");
    return app->run();
}

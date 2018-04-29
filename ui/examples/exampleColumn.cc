#include <cc/ui/Application>
#include <cc/ui/Column>
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

        View *box = add<View>();
        box->color = 0xD0D0FF;
        box->centerInParent();

        box->organize<Column>();

        box->add<Item>("• Item 1");
        box->add<Item>("• Item 2");
        box->add<Item>("• Item 3");

        {
            View *subBox = box->add<View>();
            subBox->color = 0xD0FFD0;

            subBox->organize<Column>()
                ->indent->bind([=]{
                    return style()->defaultFont()->size();
                });

            subBox->add<Item>("• Item A");
            subBox->add<Item>("• Item B");
            subBox->add<Item>("• Item C"); // ◦
        }
    }
};

int main(int argc, char **argv)
{
    Application *app = Application::open(argc, argv);
    Window::open<MainView>("Hello, world!");
    return app->run();
}

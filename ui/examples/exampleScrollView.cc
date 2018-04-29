#include <cc/debug>
#include <cc/ui/Application>
#include <cc/ui/ScrollView>
#include <cc/ui/ColumnLayout>
#include <cc/ui/Label>

using namespace cc;
using namespace cc::ui;

class MainView: public View
{
    friend class Object;

    MainView()
    {
        size = Size{640, 480};
        inheritColor();

        ScrollView *scroll = ScrollView::create(this);

        ColumnLayout::setup(scroll->carrier());
        // ColumnLayout::setup(scroll); // TODO

        for (int i = 0; i < 50; ++i) {
            Label *label = Label::create(scroll, "Item " + str(i + 1));
            label->margin = Size{20};
        }
    }
};

int main(int argc, char **argv)
{
    Application *app = Application::open(argc, argv);
    Window::open(Object::create<MainView>(), "Hello, world!");
    return app->run();
}

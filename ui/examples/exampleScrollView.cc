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
        color = Color{"#FFFFFF"};

        ScrollView *scroll = ScrollView::create(this);

        ColumnLayout::setup(scroll->carrier());
        // ColumnLayout::setup(scroll); // TODO

        for (int i = 0; i < 50; ++i)
            Label::create(scroll, "Item " + str(i));
    }
};

int main(int argc, char **argv)
{
    Application *app = Application::open(argc, argv);
    Window::open(Object::create<MainView>(), "Hello, world!", WindowMode::Default|WindowMode::Accelerated);
    return app->run();
}

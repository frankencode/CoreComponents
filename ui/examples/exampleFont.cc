#include <cc/Format>
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

        for (int i = 16; i <= 64; i += 8) {
            String text = Format("A small brown fox... (%%)") << i;
            Label::create(scroll, text, Font(i));
            Label::create(scroll, text, Font(i) << Slant::Italic);
            Label::create(scroll, text, Font(i) << Weight::Bold);
            Label::create(scroll, text, Font(i) << Slant::Italic << Weight::Bold);
        }
    }
};

int main(int argc, char **argv)
{
    Application *app = Application::open(argc, argv);
    Window::open(Object::create<MainView>(), "Hello, world!");
    return app->run();
}

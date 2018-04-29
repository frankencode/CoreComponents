#include <cc/ui/Application>
#include <cc/ui/ScrollView>
#include <cc/ui/Column>
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
        scroll->carrier()->organize<Column>();

        for (int size = 16; size <= 64; size += 8) {
            String text = "A small brown fox... (" + str(size) + ")";
            scroll->add<Label>(text, Font(size));
            scroll->add<Label>(text, Font(size) << Slant::Italic);
            scroll->add<Label>(text, Font(size) << Weight::Bold);
            scroll->add<Label>(text, Font(size) << Slant::Italic << Weight::Bold);
        }
    }
};

int main(int argc, char **argv)
{
    Application *app = Application::open(argc, argv);
    Window::open<MainView>("Hello, world!");
    return app->run();
}

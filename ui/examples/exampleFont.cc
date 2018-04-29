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
        color = style()->theme()->windowColor();

        ScrollView *scroll = ScrollView::create(this);
        scroll->carrier()->organize<Column>();

        for (int i = 16; i <= 64; i += 8) {
            String text = "A small brown fox... (" + str(i) + ")";
            scroll->add<Label>(text, Font(i));
            scroll->add<Label>(text, Font(i) << Slant::Italic);
            scroll->add<Label>(text, Font(i) << Weight::Bold);
            scroll->add<Label>(text, Font(i) << Slant::Italic << Weight::Bold);
        }
    }
};

int main(int argc, char **argv)
{
    Application *app = Application::open(argc, argv);
    Window::open<MainView>("Hello, world!");
    return app->run();
}

#include <cc/ui/Application>
#include <cc/ui/View>

using namespace cc;
using namespace cc::ui;

class MainView: public View
{
    friend class Object;

    MainView()
    {
        size = Size{640, 480};
        inheritPaper();

        // iconVisual_ = style()->getIconVisual(Icon::Videocam);
        iconVisual_ = style()->getIconVisual(Icon::LibraryAdd);
    }

    void paint() override
    {
        Painter p(this);

        p->translate(Point{dp(12)});

        iconVisual_->paint(p);
    }

    Ref<IconVisual> iconVisual_;
};

int main(int argc, char **argv)
{
    Application *app = Application::open(argc, argv);
    Window *window = Window::open(Object::create<MainView>(), "Hello, world!");
    window->title->bind([=]{ return "Test window " + str(window->size()) + " at " + str(window->pos()); });
    return app->run();
}

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

        iconVisual_ = style()->getIconVisual(Icon::Video);
        // iconVisual_ = style()->getIconVisual(Icon::LibraryAdd);
        // iconVisual_ = style()->getIconVisual(Icon::Folder);
        // iconVisual_ = style()->getIconVisual(Icon::FolderOpen);
        /*iconVisual_ = style()->getIconVisual(Icon::Alert);
        iconVisual_->ink->bind([=]{ return theme()->alertColor(); });*/
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
    auto app = Application::open(argc, argv);
    auto window = Window::open<MainView>("Hello, world!");
    window->title->bind([=]{ return "Test window " + str(window->size()) + " at " + str(window->pos()); });
    return app->run();
}

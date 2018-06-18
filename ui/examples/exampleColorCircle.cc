#include <algorithm>
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
        paper = 0xFFFFFF;
    }

    void paint() override
    {
        Painter p(this);
        Point center = size() / 2;
        const double step = 10;
        const double radius = std::min(size()[0], size()[1]) / 3;
        for (double angle = 0; angle < 360; angle += step) {
            p->moveTo(center);
            p->arcNegative(center, radius, -degree(angle), -degree(angle + step));
            p->setSource(Color::fromHue(angle));
            p->fill();
        }
    }
};

int main(int argc, char **argv)
{
    Application *app = Application::open(argc, argv);
    Window::open(Object::create<MainView>());
    return app->run();
}

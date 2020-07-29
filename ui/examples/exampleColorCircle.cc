#include <cc/ui/Application>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    View view{640, 480};
    view->paper = Color::White;
    view->paint <<[=]{
        Painter p{view};
        Point center = view->size() / 2;
        const double step = 10;
        const double radius = (view->size()[0] < view->size()[1] ? view->size()[0] : view->size()[1]) / 3;
        for (double angle = 0; angle < 360; angle += step) {
            p->moveTo(center);
            p->arcNegative(center, radius, -degree(angle), -degree(angle + step));
            p->setSource(Color::fromHue(angle));
            p->fill();
        }
    };

    Window::open(view);

    return Application{}->run();
}

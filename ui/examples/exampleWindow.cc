#include <cc/ui/Application>

int main(int argc, char **argv)
{
    using namespace cc;
    using namespace cc::ui;

    View view;
    view->size = Size{640, 480};
    view->paper = Color::White;
    view->paint >>[=]{
        Painter p{view};

        p->translate(view->center());

        p->setSource(Color{"#FF000080"});
        p->circle(-Point{30, 0}, 60);
        p->fill();

        p->setSource(Color{"#00FF0080"});
        p->circle(Point{0, 0}, 60);
        p->fill();

        p->setSource(Color{"#0000FF80"});
        p->circle(Point{30, 0}, 60);
        p->fill();
    };

    auto window = Window::open(view);
    window->title <<[=]{
        return "Test window " + str(window->size()) + " at " + str(window->pos());
    };

    return Application{}->run();
}

#include <cc/ui/Application>
#include <cc/ui/Painter>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    View view;

    Window{
        View{&view}
        .size(640, 480)
        .paper(Color::White)
        .paint([=]{
            Painter p{view};
            Point center = view.size() / 2;
            const double step = 10;
            const double radius = view.size().min() / 3;
            for (double angle = 0; angle < 360; angle += step) {
                p.moveTo(center);
                p.arcNegative(center, radius, -degree(angle), -degree(angle + step));
                p.setPen(Color::fromHue(angle));
                p.fill();
            }
        })
    }.show();

    return Application{}.run();
}

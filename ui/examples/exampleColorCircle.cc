#include <cc/Application>
#include <cc/Painter>
#include <cc/DEBUG>

int main()
{
    using namespace cc;

    View view;

    Window{
        View{&view}
        .size(640, 480)
        .paper(Color::White)
        .paint([view]{
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

    CC_INSPECT(view.isWeak());

    return Application{}.run();
}

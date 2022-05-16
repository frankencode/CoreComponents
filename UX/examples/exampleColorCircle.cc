#include <cc/View>

int main()
{
    using namespace cc;

    View view;

    return
        View{sp(500), sp(500)}
        .associate(&view)
        .paper(Color::White)
        .paint([=]{
            Painter painter{view};
            Point center = view.size() / 2;
            const double step = 10;
            const double radius = view.size().min() / 3;
            for (double angle = 0; angle < 360; angle += step) {
                painter
                .moveTo(center)
                .arcNegative(center, radius, -degree(angle), -degree(angle + step))
                .setPen(Color::fromHue(angle))
                .fill();
            }
        })
        .run();
}

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
            painter.translate(view.size() / 2);
            painter.scale({1, -1});
            const double step = 10;
            const double radius = view.size().min() / 3;
            for (double angle = 0; angle < 360; angle += step) {
                painter
                .moveTo({0, 0})
                .arc({0, 0}, radius, rad(angle), rad(angle + step))
                .setPen(Color::fromHue(angle))
                .fill();
            }
        })
        .run();
}

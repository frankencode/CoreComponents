#include <cc/View>

int main()
{
    using namespace cc;

    View view;

    return
        View{sp(500), sp(500)}
        .associate(&view)
        .paper(Color::Black)
        .paint([=]{
            Painter painter{view};

            painter
            .translate(view.size() / 2)
            .rotate(rad(30))
            .moveTo({50, 0})
            .arcAround({0, 0}, rad(300))
            .lineTo({0, 0})
            .lineTo({50, 0})
            .setPen(0xFFFF00)
            .fill()
            .rotate(rad(-30))
            .moveTo({-5, -17})
            .arcAround({-5, -23}, rad(360))
            .setPen(Color::Black)
            .fill();

            for (int i = 1; i < 10; ++i) {
                Point a { 50. * i, 0 };
                painter
                .moveTo(a)
                .arcAround(a + Point{ 7, 0 }, rad(360))
                .setPen(Color::Blue)
                .fill();
            }

            for (int i = 2; i < 10; ++i) {
                Point a { -50. * i, 0 };
                painter
                .moveTo(a)
                .arcAround(a + Point{ 7, 0 }, rad(360))
                .setPen(Color::Blue)
                .stroke();
            }
        })
        .run();
}

#include <cc/View>
#include <cc/Path>

int main()
{
    using namespace cc;

    View view;

    return
        View{sp(500), sp(500)}
        .associate(&view)
        .paper(Color::White)
        .paint([=]{
            Painter{view}
            .translate(view.size()/2)
            .addPath(
                Path{{-200, -200}}
                .lineTo({200, -200})
                .lineTo({200, 200})
                .lineTo({-200, 200})
                .lineTo({-200, -200})
            )
            .setPen(0xE0E0E0)
            .fill()
            .addPath(
                Path{{0, 0}}
                .curveTo({50, -100}, {150, 0}, {0, 150})
                .curveTo({-150, 0}, {-50, -100}, {0, 0})
            )
            .setPen(0x008000)
            .fill();
        })
        .run();
}

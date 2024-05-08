#include <cc/Plot>

int main()
{
    using namespace cc;

    Plot{gu(80), gu(60)}
    .scaling(Plot::log10)
    .points({
        Point{0,   0},
        Point{25,  10},
        Point{50,  10000},
        Point{75,  100000},
        Point{100, 100000000}
    })
    .pen(
        Pen{}
        .color({0x00, 0x00, 0x80})
        .lineWidth(10)
        .lineCap(LineCap::Round)
        .lineJoin(LineJoin::Round)
    )
    .run();
}

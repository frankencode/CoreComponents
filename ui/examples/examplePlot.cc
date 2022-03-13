#include <cc/ui/Application>
#include <cc/ui/Plot>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    Window{
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
            .lineWidth(50)
            .lineCap(LineCap::Round)
            .lineJoin(LineJoin::Round)
        )
    }.show();

    return Application{}.run();
}

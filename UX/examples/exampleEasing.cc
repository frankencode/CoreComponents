#include <cc/Control>

int main()
{
    using namespace cc;

    Control box;

    return
        View{sp(500), sp(500)}
        .paper(Color::White)
        .add(
            Control{}
            .associate(&box)
            .onClicked([=] mutable {
                box.angle(box.angle() + 45);
            })
            .size(sp(200), sp(200))
            .paper("dodgerblue")
            .angleEasing(Easing::Bezier{0.5, -0.4, 0.5, 1.4}, 0.5)
            .centerInParent()
        )
        .run();
}

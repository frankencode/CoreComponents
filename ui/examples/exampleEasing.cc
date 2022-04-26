#include <cc/Label>

int main()
{
    using namespace cc;

    View box;

    return
        View{sp(500), sp(500)}
        .paper(Color::White)
        .add(
            View{&box}
            .size(sp(200), sp(200))
            .centerInParent()
            .paper("dodgerblue")
            .angleEasing(Easing::Bezier{0.5, -0.4, 0.5, 1.4}, 0.5)
            .onPointerReleased([=](auto) mutable {
                box.angle(box.angle() + 45);
                return true;
            })
            .add(
                Label{"Press me!"}
                .color(Color::White)
                .centerInParent()
            )
        )
        .run();
}

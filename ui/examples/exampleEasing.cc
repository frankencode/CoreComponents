#include <cc/Label>
#include <cc/Control>
#include <cc/math>

int main()
{
    using namespace cc;

    Control box;
    Property<double> targeAngle;

    return
        View{sp(500), sp(500)}
        .paper(Color::White)
        .add(
            Control{&box}
            .onClicked([=]() mutable {
                box.angle((targeAngle += 45)());
            })
            .size(sp(200), sp(200))
            .centerInParent()
            .paper("dodgerblue")
            .angleEasing(Easing::Bezier{0.5, -0.4, 0.5, 1.4}, 0.5)
            .add(
                Label{"Press me!"}
                .color(Color::White)
                .centerInParent()
            )
        )
        .run();
}

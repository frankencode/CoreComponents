#include <cc/Label>
#include <cc/Timer>
#include <cc/System>
#include <cc/Date>

int main()
{
    using namespace cc;

    Property<double> time = System::now();
    Label label;

    return
        View{sp(500), sp(500)}
        .paper(Color::White)
        .add(
            Label{&label}
            .font(Font{Pitch::Fixed, sp(40)})
            .text([=]{
                Date date{time};
                return dec(date.hour(), 2) + "∶" + dec(date.minutes(), 2) + "∶" + dec(date.seconds(), 2);
            })
            .margin(sp(20))
            .color(Color::Black)
            .paper(0xD0D0FF)
            .angleEasing(Easing::Bezier{0.5, -0.4, 0.5, 1.4}, 0.5)
            .centerInParent()
        )
        .attach(
            Timer{1}
            .onTimeout([=]() mutable {
                time = System::now();
                label.angle(label.angle() + 45);
            })
            .startAt(std::ceil(time))
        )
        .run();
}

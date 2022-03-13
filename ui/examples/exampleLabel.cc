#include <cc/ui/Application>
#include <cc/ui/Label>
#include <cc/ui/Timer>
#include <cc/ui/Easing>
#include <cc/System>
#include <cc/Date>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    Property<double> time = System::now();
    Label label;

    Window{
        View{640, 480}
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
    }.show();

    Timer timer{1};
    timer.onTimeout([&time, &label]{
        time = System::now();
        label.angle(label.angle() + 45);
    });
    timer.startAt(std::ceil(time));

    return Application{}.run();
}

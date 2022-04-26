#include <cc/Application>
#include <cc/Easing>
#include <cc/DEBUG>

int main()
{
    using namespace cc;

    View box;

    Window{
        View{640, 480}
        .paper("white")
        .add(
            View{&box}
            .size(200, 200)
            .centerInParent()
            .paper("dodgerblue")
            .angleEasing(Easing::Bezier{0.5, -0.4, 0.5, 1.4}, 0.5)
            .onPointerReleased([=](auto) mutable {
                box.angle(box.angle() + 45);
                return true;
            })
        )
    }
    .title("Click on the blue square!")
    .show();

    return Application{}.run();
}

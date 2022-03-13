#include <cc/ui/Application>
#include <cc/ui/Box>
#include <cc/ui/Shadow>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    Window{
        View{640, 480}
        .paper(Color::White)
        .add(
            Box{}
            .radius(50)
            .color(0x008090)
            .size({100, 100})
            .centerInParent()
            .decorate(
                Shadow{}
                .color(0x80008090)
                .blurRadius(sp(10))
                .offset({0, sp(5)})
            )
        )
    }.show();

    return Application{}.run();
}

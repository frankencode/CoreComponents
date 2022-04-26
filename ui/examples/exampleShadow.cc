#include <cc/Application>
#include <cc/Box>
#include <cc/Shadow>

int main()
{
    using namespace cc;

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

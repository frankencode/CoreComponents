#include <cc/Box>
#include <cc/Shadow>

int main()
{
    using namespace cc;

    View{640, 480}
    .paper(Color::White)
    .add(
        Box{}
        .radius(50)
        .color(0x008090)
        .size({100, 100})
        .centerInParent()
        .decoration(
            Shadow{}
            .color(0x80004048)
            .blurRadius(8)
            .offset({0, 8})
        )
    )
    .run();
}

#include <cc/View>

int main()
{
    using namespace cc;

    return
        View{500, 500}
        .add(
            View{500, 500}
            .pos(0, 0)
            .paper(Color::Green)
        )
        .add(
            View{500, 500}
            .pos(500, 0)
            .paper(Color::Blue)
        )
        .add(
            View{500, 500}
            .pos(1000, 0)
            .paper(Color::Red)
        )
        .pos(-250, 0)
        .run();
}

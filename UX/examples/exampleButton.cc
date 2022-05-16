#include <cc/Button>
#include <cc/Format>

int main()
{
    using namespace cc;

    Button button;

    return
        View{sp(640), sp(480)}
        .add(
            Button{}
            .associate(&button)
            .icon(Ideographic::Face, sp(28))
            .text("Press me!")
            .onClicked([=]() mutable {
                static int n = 1;
                button.text(Format{"Pressed (%%)"}.arg(n++));
            })
            .centerInParent()
        )
        .run();
}

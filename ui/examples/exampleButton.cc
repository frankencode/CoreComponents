#include <cc/Application>
#include <cc/Button>
#include <cc/DEBUG>

int main()
{
    using namespace cc;

    Button button;

    Window{
        View{sp(640), sp(480)}
        .add(
            Button{&button}
            .icon(Ideographic::Face, sp(28))
            .text("Press me!")
            .centerInParent()
            .attach([button]{
                CC_INSPECT(button.height());
            })
        )
    }.show();

    return Application{}.run();
}

#include <cc/ui/Application>
#include <cc/ui/Button>
#include <cc/DEBUG>

int main()
{
    using namespace cc;
    using namespace cc::ui;

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

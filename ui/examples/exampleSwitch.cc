#include <cc/ui/Application>
#include <cc/ui/Switch>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    Switch onOff;

    Window{
        View{sp(640), sp(480)}
        .add(
            Switch{&onOff}
            .centerInParent()
        )
    }.show();

    return Application{}.run();
}

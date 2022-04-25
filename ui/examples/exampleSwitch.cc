#include <cc/ui/Application>
#include <cc/ui/Switch>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    Window{
        View{sp(640), sp(480)}
        .add(
            Switch{}
            .centerInParent()
        )
    }.show();

    return Application{}.run();
}

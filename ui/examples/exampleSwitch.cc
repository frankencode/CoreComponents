#include <cc/Application>
#include <cc/Switch>

int main()
{
    using namespace cc;

    Window{
        View{sp(640), sp(480)}
        .add(
            Switch{}
            .centerInParent()
        )
    }.show();

    return Application{}.run();
}

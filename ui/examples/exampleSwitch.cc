#include <cc/Switch>

int main()
{
    using namespace cc;

    return
        View{sp(640), sp(480)}
        .add(
            Switch{}
            .centerInParent()
        )
        .run();
}

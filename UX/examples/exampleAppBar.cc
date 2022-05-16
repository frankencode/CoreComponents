#include <cc/AppBar>
#include <cc/DEBUG>

int main()
{
    using namespace cc;

    AppBar appBar;

    return
        View{sp(500), sp(500)}
        .add(
            AppBar{}
            .associate(&appBar)
            .title("Hello, world!")
            .onDismissed([]{ ferr() << "Back!" << nl; })
            // .onNavigate([]{ ferr() << "Navigate!" << nl; })
        )
        .run();
}

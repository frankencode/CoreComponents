#include <cc/AppBar>
#include <cc/DEBUG>

int main()
{
    using namespace cc;

    AppBar appBar;

    return
        View{sp(360), sp(600)}
        .add(
            AppBar{}
            .associate(&appBar)
            .title("Hello, world!")
            .onDismissed([]{
                ferr() << "Back!" << nl;
            })
            .addAction(
                Action{}
                .icon(Ideographic::Information)
                .onTriggered([]{
                    ferr() << "Info!" << nl;
                })
            )
            // .onNavigate([]{ ferr() << "Navigate!" << nl; })
        )
        .run();
}

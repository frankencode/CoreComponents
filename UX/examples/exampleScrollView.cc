#include <cc/ScrollView>
#include <cc/Label>

int main()
{
    using namespace cc;

    return
        ScrollView{
            sp(500), sp(500),
            Pane{}
            .size(sp(900), sp(900))
            .add(
                View{sp(400), sp(400)}
                .centerInParent()
                .paper(Color::Red)
            )
            .add(
                Label{"Test 1, 2, 3"}
                .pos(sp(100), sp(100))
            )
        }.run();
}

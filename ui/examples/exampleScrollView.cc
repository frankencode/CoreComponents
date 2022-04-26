#include <cc/Application>
#include <cc/ScrollView>
#include <cc/Label>
#include <cc/DEBUG>

int main()
{
    using namespace cc;

    ScrollView scrollView{640, 480};
    scrollView.pane()
        .size(Size{1280, 960})
        .add(
            View{400, 400}
            .centerInParent()
            .paper(Color::Red)
        )
        .add(
            Label{"Test 1, 2, 3"}
            .pos(100, 100)
        );

    Window{scrollView}.show();

    return Application{}.run();
}

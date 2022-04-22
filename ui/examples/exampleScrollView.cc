#include <cc/ui/Application>
#include <cc/ui/ScrollView>
#include <cc/ui/Label>
#include <cc/DEBUG>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    ScrollView scrollView{640, 480};

    scrollView.pane().size(Size{1280, 960});
    scrollView.pane().add(
        View{400, 400}
        .centerInParent()
        .paper(Color::Red)
    );

    scrollView.pane().add(
        Label{"Test 1, 2, 3"}
        .pos(100, 100)
    );

    Window{scrollView}.show();

    return Application{}.run();
}

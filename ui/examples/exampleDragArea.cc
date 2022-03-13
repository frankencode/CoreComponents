#include <cc/ui/Application>
#include <cc/ui/DragArea>
#include <cc/ui/Label>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    DragArea dragArea{640, 480};

    const int n = 10;

    for (int i = 0; i < n; ++i) {
        dragArea.add(
            Label{"Drag me!"}
            .margin(dp(40))
            .paper(Color::fromHsv(360. * i / n, 0.4, 1.))
        );
    }

    Window{dragArea}.show();

    return Application{}.run();
}

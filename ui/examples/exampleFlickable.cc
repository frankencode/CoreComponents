#include <cc/ui/Application>
#include <cc/ui/Flickable>
#include <cc/ui/ColumnLayout>
#include <cc/ui/Label>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    const double gap = Font::defaultSize();

    Flickable pane{640, 480};

    pane.layout(
        ColumnLayout{}
        .spacing(gap)
        .margin(gap)
    );

    for (int i = 0; i < 100; ++i) {
        pane.add(Label{"Item " + str(i + 1)});
    }

    Window{pane}.show();

    return Application{}.run();
}

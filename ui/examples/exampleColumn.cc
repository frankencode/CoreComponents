#include <cc/ui/Application>
#include <cc/ui/ColumnLayout>
#include <cc/ui/Label>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    View view{640, 480};
    view->paper = Color::White;

    View box;
    box->paper = 0xD0D0FF;
    box->size = Size{50, 50};
    ColumnLayout{box};
    for (String s: StringList{
        "• Item 1",
        "• Item 2",
        "• Item 3"
    }) {
        Label label{s};
        label->margin = dp(20);
        label->paper = Color::Green;
        box << label;
    }

    View subBox;
    subBox->paper = 0xD0FFD0;
    ColumnLayout{subBox}
        ->indent = dp(20);
    for (String s: StringList{
        "◦ Item A",
        "◦ Item B",
        "◦ Item C"
    }) {
        Label label{s};
        label->margin = dp(20);
        label->paper = Color::Blue;
        subBox << label;
    }
    box << subBox;

    view << box;
    box->centerInParent();

    Window::open(view, "Hello, world!", WindowMode::Accelerated|WindowMode::VSync);

    return Application{}->run();
}

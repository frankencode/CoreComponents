#include <cc/ui/Application>
#include <cc/ui/ScrollView>
#include <cc/ui/ColumnLayout>
#include <cc/ui/Label>

int main(int argc, char **argv)
{
    using namespace cc;
    using namespace cc::ui;

    View view{640, 480};
    view->inheritPaper();

    ScrollView scroll;
    ColumnLayout{scroll->carrier()};

    for (int i = 0; i < 50; ++i) {
        Label label{"Item" + str(i + 1)};
        label->margin = Size{label->font()->size()};
        scroll->carrier() << label;
    }
    view << scroll;

    Window::open(view, "Hello, world!", WindowMode::Accelerated|WindowMode::VSync);

    return Application{}->run();
}

#include <cc/ui/Application>
#include <cc/ui/ScrollView>
#include <cc/ui/ColumnLayout>
#include <cc/ui/Label>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    ScrollView view{640, 480};
    ColumnLayout{view};

    for (double size = 16; size <= 64; size += 8) {
        String text = "A small brown fox... (" + str(size) + ")";
        view << Label{text, Font{size}};
        view << Label{text, Font{size} << Slant::Italic};
        view << Label{text, Font{size} << Weight::Bold};
        view << Label{text, Font{size} << Slant::Italic << Weight::Bold};
    }

    Window::open(view, "Hello, world!", WindowMode::Accelerated|WindowMode::VSync);

    return Application{}->run();
}

#include <cc/ui/Application>
#include <cc/ui/ColumnLayout>
#include <cc/ui/RowLayout>
#include <cc/ui/Label>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    class MyLabel: public Label {
    public:
        MyLabel(const String &text):
            Label{text}
        {
            (*this)->margin = (*this)->font()->size();
        }
    };

    View view{640, 480};
    view->paper = Color::White;

    View box;
    box->centerInParent();
    box->paper = Color{0xD0D0FF};
    ColumnLayout{box};
    box << MyLabel{"• Item 1"};
    box << MyLabel{"• Item 2"};
    box << MyLabel{"• Item 3"};
    {
        View subBox;
        subBox->paper = Color{0xD0FFD0};
        RowLayout{subBox};
        subBox << MyLabel{"◦ Item A"};
        subBox << MyLabel{"◦ Item B"};
        subBox << MyLabel{"◦ Item C"};
        box << subBox;
    }
    view << box;

    Window::open(view, "Hello, world!", WindowMode::Accelerated|WindowMode::VSync);

    return Application{}->run();
}

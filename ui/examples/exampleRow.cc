#include <cc/debug>
#include <cc/ui/Application>
#include <cc/ui/RowLayout>
#include <cc/ui/ColumnLayout>
#include <cc/ui/StylePlugin>
#include <cc/ui/Label>

using namespace cc;
using namespace cc::ui;

class MainView: public View
{
    friend class Object;

    MainView()
    {
        size = Size{640, 480};
        color = Color{"#FFFFFF"};

        View *box = View::create(this);
        box->color = Color{"#D0D0FF"};
        box->centerInParent();

        RowLayout::setup(box);

        Label::create(box, "• Item 1");
        Label::create(box, "• Item 2");

        View *subBox = View::create(box);
        subBox->color = Color{"#D0FFD0"};

        ColumnLayout::setup(subBox);
        Label::create(subBox, "◦ Item A");
        Label::create(subBox, "◦ Item B");
    }

    bool hasKeyInput() const override { return true; }

    bool onKeyPressed(const KeyEvent *event)
    {
        if (+(event->modifiers() & KeyModifier::Control))
        {
            if (event->keyCode() == '+')
                Application::instance()->textZoom += 4;
            else if (event->keyCode() == '-')
                Application::instance()->textZoom -= 4;
        }

        return true;
    }
};

int main(int argc, char **argv)
{
    Application *app = Application::open(argc, argv);
    app->textZoom = 20;
    Window::open(Object::create<MainView>(), "Hello, world!");
    return app->run();
}

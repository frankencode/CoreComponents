#include <cc/debug>
#include <cc/ui/Application>
#include <cc/ui/Column>
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

        Column *box = Column::create(this);
        box->color = Color{"#D0D0FF"};
        box->centerInParent();

        Label::create(box, "• Item 1");
        Label::create(box, "• Item 2");
        Label::create(box, "• Item 3");

        {
            Column *subBox = Column::create(box);
            subBox->color = Color{"#D0FFD0"};
            subBox->indent->bind([=]{
                return (
                    StylePlugin::instance()->defaultTextStyle()->font()->size() +
                    Application::instance()->textZoom()
                );
            });

            Label::create(subBox, "◦ Item A");
            Label::create(subBox, "◦ Item B");
            Label::create(subBox, "◦ Item C");
        }

        keyPressed->connect([=]{
            if (+(key()->modifiers() & KeyModifier::Control)) {
                if (key()->keyCode() == '+') {
                    Application::instance()->textZoom += 4;
                }
                else if (key()->keyCode() == '-') {
                    Application::instance()->textZoom -= 4;
                }
            }
        });
    }
};

int main(int argc, char **argv)
{
    Application *app = Application::open(argc, argv);
    app->textZoom = 20;
    Window::open(Object::create<MainView>(), "Hello, world!");
    return app->run();
}

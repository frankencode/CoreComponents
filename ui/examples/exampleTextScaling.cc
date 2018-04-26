#include <cc/ui/Application>
#include <cc/ui/Transition>
#include <cc/ui/Label>

using namespace cc;
using namespace cc::ui;

class MainView: public View
{
    friend class Object;

    MainView()
    {
        size = Size{640, 480};
        color = style()->theme()->windowColor();

        label_ = Label::create(this, "Press + | - (zoom in/out)");
        label_->font = Font { 40 };
        label_->ink = Color::Black;
        label_->margin = Size{ 2 };
        label_->centerInParent();

        easeOn(label_->font, 1, easing::Bezier(0.5, -0.4, 0.5, 1.4));
    }

    bool isStatic() const override { return false; }

    bool onKeyPressed(const KeyEvent *event)
    {
        if (+(event->modifiers() & KeyModifier::Control))
        {
            if (event->keyCode() == '+')
                label_->font *= 2;
            else if (event->keyCode() == '-')
                label_->font /= 2;
        }

        return true;
    }

    Label *label_;
};

int main(int argc, char **argv)
{
    Application *app = Application::open(argc, argv);
    Window::open(Object::create<MainView>(), "Hello, world!", WindowMode::Accelerated);
    return app->run();
}

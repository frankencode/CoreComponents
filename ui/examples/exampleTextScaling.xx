#include <cc/ui/Label>
#include <cc/ui/Transition>

using namespace cc;
using namespace cc::ui;

class MainView: public View
{
    friend class Object;

    MainView()
    {
        size = Size{640, 480};
        inheritPaper();

        label_ = add<Label>("Press + | - (zoom in/out)");
        label_->font = Font { 40 };
        label_->ink = Color::Black;
        // label_->centerInParent();

        easeOn(label_->font, 1, easing::Bezier(0.5, -0.4, 0.5, 1.4));
    }

    bool isStatic() const override { return false; }

    bool onKeyPressed(const KeyEvent *event) override
    {
        if (event->keyCode() == '+')
            label_->font *= 2;
        else if (event->keyCode() == '-')
            label_->font /= 2;

        return true;
    }

    Label *label_;
};

int main(int argc, char **argv)
{
    Window::open<MainView>("Hello, world!", WindowMode::Accelerated|WindowMode::VSync);
    return Application{}->run();
}

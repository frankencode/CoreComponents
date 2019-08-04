#include <cc/debug>
#include <cc/File>
#include <cc/ui/TextEdit>

using namespace cc;
using namespace cc::ui;

String path;

class MainView: public View
{
    friend class Object;

    MainView()
    {
        // size = Size { 640, 480 };
        size = Size { 1920, 1080 };
        inheritPaper();

        auto edit = add<TextEdit>();
        edit->showLineNumbers = false;
        // edit->font->bind([=] { return app()->defaultFixedFont(); }); // FIXME: Why does this not work?
        // edit->font = Font{"DejaVu Sans Mono", 14};
        // app()->defaultFixedFont();
        edit->setText(
            File::open(path)->map()
        );
        // CC_INSPECT(edit->getText());
        // CC_INSPECT(size());
    }
};

int main(int argc, char **argv)
{
    Application::open(argc, argv);
    if (argc > 1) path = argv[1];
    else path = __FILE__;
    Window::open<MainView>("Hello, world!", WindowMode::Accelerated|WindowMode::Fullscreen|WindowMode::VSync);
    return Application::instance()->run();
}

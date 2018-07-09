#include <cc/debug>
#include <cc/ui/ItemView>
#include <cc/ui/ItemCarrier>

using namespace cc;
using namespace cc::ui;

class MainView: public View
{
    friend class Object;

    MainView()
    {
        size = Size{640, 480};
        inheritPaper();

        auto root = Item::create();
        for (int i = 0; i < 20; ++i)
            root->add<Item>();

        for (int i = 0; i < 3; ++i)
            root->at(2)->add<Item>();

        add<ItemView>(root);
        // add<ItemCarrier>(root);
    }
};

int main(int argc, char **argv)
{
    auto app = Application::open(argc, argv);
    Window::open<MainView>();
    return app->run();
}

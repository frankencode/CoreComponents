#include <cc/debug>
#include <cc/ui/Application>
#include <cc/ui/View>
#include <cc/ui/Label>
#include <cc/ui/ItemView>
#include <cc/ui/SingleLineItemDelegate>

using namespace cc;
using namespace cc::ui;

class IconItem: public Item
{
    friend class Object;

public:
    Icon icon() const { return icon_; }

private:
    IconItem(Icon icon): icon_{icon} {}

    Icon icon_;
};

class IconModel: public Item
{
    friend class Object;

    IconModel()
    {
        /*Icon icons[] = { Icon::Video, Icon::Folder, Icon::FolderOpen, Icon::Alert };
        for (Icon icon: icons)
            add<IconItem>(icon);*/
        for (int i = static_cast<int>(Icon::FirstIcon); i <= static_cast<int>(Icon::LastIcon); ++i)
            add<IconItem>(static_cast<Icon>(i));
    }
};

class IconView: public ItemView
{
    friend class Object;

    IconView(View *parent, IconModel *model):
        ItemView{parent, model}
    {}

    View *addDelegate(View *parent, Item *item) override
    {
        IconItem *iconItem = static_cast<IconItem *>(item);
        return add<SingleLineItemDelegate>(str(iconItem->icon()), style()->getIconVisual(iconItem->icon()));
        // return add<Label>(str(iconItem->icon()));
    }
};

class MainView: public View
{
    friend class Object;

    MainView()
    {
        size = Size{640, 480};
        inheritPaper();

        auto model = Object::create<IconModel>();
        auto iconView = add<IconView>(model);

        CC_INSPECT(iconView->carrier()->size());

        #if 0
        Icon icons[] = { Icon::Video, Icon::Folder, Icon::FolderOpen, Icon::Alert };

        for (auto icon: icons)
            add<SingleLineItemDelegate>(str(icon), style()->getIconVisual(icon));

        organize<Column>();
        #endif
    }
};

int main(int argc, char **argv)
{
    auto app = Application::open(argc, argv);
    Window::open<MainView>();
    return app->run();
}

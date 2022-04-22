#include <cc/ui/Application>
#include <cc/ui/Picture>
#include <cc/ui/ListMenu>
#include <cc/ui/Label>
#include <cc/Format>
#include <cc/DEBUG>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    ListMenu menu{640, 480};
    // menu.header(Label{"Test 1, 2, 3, ..."}.margin(sp(16)));
    // menu.footer(Label{"Fin."}.margin(sp(16)));

    for (int i = 0; i < 32; ++i)
    {
        ListItem item;
        menu.pane().add(
            ListItem{&item}
            .icon(Picture{Ideographic::AccessPoint, 28})
            .text(Format{"Access point %%"} << i)
            .onClicked([i]{
                CC_INSPECT(i);
            })
            #if 0
            .attach([i,item]{
                ferr() << i << ": " << (item.visible() ? "show" : "hide") << nl;
            })
            .onEndOfLife([i]{
                ferr() << i << ": EOL" << nl;
            })
            #endif
        );
    }

    Window{menu}.show();

    return Application{}.run();
}

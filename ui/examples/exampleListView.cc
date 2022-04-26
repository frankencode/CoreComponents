#include <cc/Application>
#include <cc/Picture>
#include <cc/ListView>
#include <cc/ListItem>
#include <cc/Label>
#include <cc/Format>
#include <cc/DEBUG>

int main()
{
    using namespace cc;

    ListView listView{640, 480};
    // listView.header(Label{"Test 1, 2, 3, ..."}.margin(sp(16)));
    // listView.footer(Label{"Fin."}.margin(sp(16)));

    for (int i = 0; i < 32; ++i)
    {
        ListItem item;

        listView.pane().add(
            ListItem{&item}
            .icon(Picture{Ideographic::AccessPoint, 28})
            .text(Format{"Access point %%"} << i)
            .onClicked([=]{
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

    Window{listView}.show();

    return Application{}.run();
}

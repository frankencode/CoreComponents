#include <cc/ui/Application>
#include <cc/ui/Picture>
#include <cc/ui/ListView>
#include <cc/ui/ListItem>
#include <cc/ui/Label>
#include <cc/Format>
#include <cc/DEBUG>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    ListView listView{640, 480};
    // menu.header(Label{"Test 1, 2, 3, ..."}.margin(sp(16)));
    // menu.footer(Label{"Fin."}.margin(sp(16)));

    for (int i = 0; i < 32; ++i)
    {
        listView.add(
            ListItem{}
            .icon(Picture{Ideographic::AccessPoint, 28})
            .text(Format{"Access point %%"} << i)
            .onClicked([=]{
                CC_INSPECT(i);
            })
        );
    }

    Window{listView}.show();

    return Application{}.run();
}

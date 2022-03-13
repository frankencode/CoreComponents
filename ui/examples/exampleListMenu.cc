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
        menu.add(
            ListItem{}
            .icon(Picture{Ideographic::AccessPoint, 28})
            .text(Format{"Access point %%"} << i)
            .onClicked([=]{
                CC_INSPECT(i);
            })
        );
    }

    Window{menu}.show();

    return Application{}.run();
}

#include <cc/ListMenu>
#include <cc/Picture>
#include <cc/Label>
#include <cc/Format>
#include <cc/debugging>

int main()
{
    using namespace cc;

    ListMenu menu;

    return
        ListMenu{sp(500), sp(500)}
        .associate(&menu)
        .header(
            Label{}
            .margin(sp(16))
            .text([=]{
                return Format{"%% networks available"}.arg(menu.carrier().count());
            })
        )
        .footer(
            Label{"End of list"}
            .margin(sp(16))
        )
        .populate([](auto target){
            for (int i = 0; i < 32; ++i) {
                target.carrier().add(
                    ListItem{}
                    .icon(Picture{Icon::AccessPoint, 28})
                    .title(Format{"Access point %%"}.arg(i))
                    .onClicked([i]{
                        CC_INSPECT(i);
                    })
                );
            }
        })
        .run();
}

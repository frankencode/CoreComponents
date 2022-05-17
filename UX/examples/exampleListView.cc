#include <cc/ListView>
#include <cc/ListItem>
#include <cc/Picture>
#include <cc/Label>
#include <cc/Format>
#include <cc/DEBUG>

int main()
{
    using namespace cc;

    ListView menu;

    return
        ListView{sp(500), sp(500)}
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
        .populate([](auto target) {
            for (int i = 0; i < 32; ++i) {
                target.carrier().add(
                    ListItem{}
                    .icon(Picture{Ideographic::AccessPoint, 28})
                    .title(Format{"Access point %%"}.arg(i))
                    .onClicked([=]{
                        CC_INSPECT(i);
                    })
                );
            }
        })
        .run();
}

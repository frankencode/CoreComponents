#include <cc/StackView>
#include <cc/ListMenu>
#include <cc/Picture>
#include <cc/Label>
#include <cc/DEBUG>

int main()
{
    using namespace cc;

    StackView stack;
    Label screen;
    ListMenu menu;

    return
        StackView{sp(500), sp(500), &stack}
        .push(
            ListMenu{&menu}
            .populate([](auto target) {
                for (int i = 0; i < 32; ++i) {
                    target.pane().add(
                        ListItem{}
                        .icon(Picture{Ideographic::AccessPoint, 28})
                        .text(Format{"Item %%"}.arg(i))
                        .onClicked([i]() {
                            CC_INSPECT(i);
                            // stack.push(View{}.add(Label{"Hello"}));
                        })
                    );
                }
            })
        )
        .attach([=]{
            CC_INSPECT(menu.id());
            CC_INSPECT(stack.size());
            CC_INSPECT(stack.carrier().size());
            CC_INSPECT(stack.carrier().pos());
        })
        .run();
}

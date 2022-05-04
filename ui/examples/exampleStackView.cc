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
    Control blueScreen;

    return
        StackView{sp(500), sp(500), &stack}
        /*.push(
            Control{}
            .onClicked([=]() mutable{
                stack.push(
                    Control{&blueScreen}
                    .paper(Color::Blue)
                );
            })
            .paper(Color::Green)
        )*/
        .push(
            ListMenu{&menu}
            .populate([=](auto target) mutable {
                for (int i = 0; i < 32; ++i) {
                    target.pane().add(
                        ListItem{}
                        .icon(Picture{Ideographic::AccessPoint, 28})
                        .text(Format{"Item %%"}.arg(i))
                        .onClicked([=]() mutable {
                            CC_INSPECT(i);
                            stack.push(
                                Control{}
                                .onClicked([=]() mutable {
                                    stack.pop();
                                })
                                .add(Label{"Hello"}.centerInParent())
                                .add(View{100, 100}.pos(10, 10).paper(Color::Red))
                                .add(View{100, 100}.pos(120, 10).paper(Color::Green))
                                .add(View{100, 100}.pos(230, 10).paper(Color::Blue))
                                .add(View{100, 100}.pos(340, 10).paper(Color::Black))
                            );
                        })
                    );
                }
            })
        )
        #if 0
        .attach([=]{
            // CC_INSPECT(menu.id());
            CC_INSPECT(stack.carrier().id());
            CC_INSPECT(stack.size());
            CC_INSPECT(stack.carrier().size());
            CC_INSPECT(stack.carrier().pos());
        })
        #endif
        .run();
}

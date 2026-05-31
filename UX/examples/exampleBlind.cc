#include <cc/Blind>
#include <cc/TonalButton>
#include <cc/debugging>

int main()
{
    using namespace cc;

    View view;
    Blind blind;

    return
        View{sp(640), sp(480), &view}
        (
            TonalButton{"Open"}
            .onClicked([=]() mutable {
                view.push(
                    Blind{&blind}
                    (
                        TonalButton{"Close"}
                        .onClicked([=]() mutable {
                            view.pop();
                        })
                        .centerInParent()
                    )
                    .onEndOfLife([]{
                        CC_DEBUG << "EOL";
                    })
                );
            })
            .centerInParent()
        )
        (
            TonalButton{"Test 123"}
            .onClicked([]{ CC_DEBUG; })
            .pos({sp(48), sp(48)})
        )
        .run();
}

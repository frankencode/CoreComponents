#include <cc/Blind>
#include <cc/TonalButton>
#include <cc/DEBUG>

int main()
{
    using namespace cc;

    Blind blind;
    View view;

    return
        View{sp(640), sp(480)}
        .associate(&view)
        .add(
            TonalButton{"Open"}
            .onClicked([=]() mutable {
                view.push(
                    Blind{}
                    .associate(&blind)
                    .add(
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
        .add(
            TonalButton{"Test 123"}
            .onClicked([]{ CC_DEBUG; })
            .pos({sp(48), sp(48)})
        )
        .run();
}

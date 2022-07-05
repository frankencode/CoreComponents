#include <cc/CustomDialog>
#include <cc/LineEdit>
#include <cc/Text>
#include <cc/TonalButton>
#include <cc/DEBUG>

int main()
{
    using namespace cc;

    CustomDialog dialog;
    View view;

    return
        View{sp(360), sp(360)}
        .associate(&view)
        .add(
            TonalButton{"Continue"}
            ([=]() mutable {
                CustomDialog{}
                .associate(&dialog)
                .addContent(
                    LineEdit{}
                    .title("Enter name")
                )
                .addAction(
                    Action{"Cancel"}
                    ([=]() mutable {
                        CC_DEBUG << "Cancel";
                        dialog.close();
                    })
                )
                .addAction(
                    Action{"OK"}
                    ([=]() mutable {
                        CC_DEBUG << "OK";
                        dialog.close();
                    })
                )
                .open()
                .onEndOfLife([]{
                    CC_DEBUG << "EOL";
                });
            })
            .centerInParent()
        )
        .run();
}

#include <cc/TonalButton>
#include <cc/AlertDialog>
#include <cc/DEBUG>

int main()
{
    using namespace cc;

    AlertDialog dialog;
    View view;

    return
        View{sp(360), sp(360)}
        .associate(&view)
        .add(
            TonalButton{"Test"}
            ([=]() mutable {
                AlertDialog{"Discard draft?"}
                .associate(&dialog)
                .addAction(
                    Action{"Cancel"}
                    ([=]() mutable {
                        CC_DEBUG << "Cancel";
                        dialog.close();
                    })
                )
                .addAction(
                    Action{"Discard"}
                    ([=]() mutable {
                        CC_DEBUG << "Discard";
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

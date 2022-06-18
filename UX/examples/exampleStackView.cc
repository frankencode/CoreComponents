#include <cc/StackView>
#include <cc/AppBar>
#include <cc/ListMenu>
#include <cc/Picture>
#include <cc/Flickable>
#include <cc/ColumnLayout>
#include <cc/LineEdit>
#include <cc/NumberEdit>
#include <cc/PasswordEdit>
#include <cc/Slider>
#include <cc/Switch>
#include <cc/Checkbox>
#include <cc/Divider>
#include <cc/DEBUG>

int main()
{
    using namespace cc;

    StackView stack;
    ListMenu menu;

    PasswordEdit passwordField;
    Checkbox dessert;

    AppBar appBar;
    View screen;

    return
        StackView{sp(480), sp(800)}
        .associate(&stack)
        .push(
            View{}
            .add(
                AppBar{}
                .associate(&appBar)
                .title("User Manager")
                .onNavigate([]{ ferr() << "Navigate!" << nl; })
            )
            .addBelow(
                ListMenu{}
                .associate(&menu)
                .populate([=](auto target) mutable {
                    for (int i = 0; i < 32; ++i) {
                        target.carrier().add(
                            ListItem{}
                            .icon(Picture{Icon::AccessPoint, 28})
                            .title(Format{"User %%"}.arg(i))
                            .onClicked([=]() mutable {
                                CC_INSPECT(i);
                                stack.push(
                                    View{}
                                    .associate(&screen)
                                    .add(
                                        Flickable{
                                            Pane{}
                                            .layout(ColumnLayout{})
                                            .add(LineEdit{"First name"})
                                            .add(LineEdit{"Family name"})
                                            .add(
                                                NumberEdit{"Age"}
                                                .value(-1)
                                                .min(0)
                                                .max(1000)
                                                //! \todo allow to step focus, when left empty
                                            )
                                            .add(Divider{})
                                            .add(Checkbox{"Dessert"}.associate(&dessert))
                                            .add(Checkbox{"Apple"}.groupUnder(dessert))
                                            .add(Checkbox{"Banana"}.groupUnder(dessert))
                                            .add(Divider{})
                                            .add(
                                                PasswordEdit{"Enter password"}
                                                .associate(&passwordField)
                                                .onAccepted([=]{
                                                    CC_INSPECT(passwordField.password());
                                                })
                                            )
                                            .add(PasswordEdit{"Repeat password"})
                                            .add(Switch{"Super user"})
                                            .add(Switch{"Tutorial mode"})
                                            .add(LineEdit{"Street address"})
                                            .add(LineEdit{"Zip code"})
                                            .add(
                                                Slider{}
                                                .leading(Icon::VolumeLow)
                                                .trailing(Icon::VolumeHigh)
                                            )
                                            .add(LineEdit{"Country"})
                                        }
                                        .size([=]{
                                            return screen.size() - Size{0, appBar.height()};
                                        })
                                        .pos(0, appBar.height())
                                    )
                                    .add(
                                        AppBar{}
                                        .title(Format{"Edit User %%"}.arg(i))
                                        .onDismissed([=]() mutable { stack.pop(); })
                                    )
                                    .onEndOfLife([]{ ferr() << "screen destroyed" << nl; })
                                );
                            })
                        );
                    }
                })
                .pos(0, appBar.height())
                .size([=]{ return stack.size() - Size{0, appBar.height()}; })
            )
        )
        .onEndOfLife([]{ ferr() << "stack destroyed" << nl; })
        .run();
}

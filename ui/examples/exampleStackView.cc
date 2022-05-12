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
#include <cc/Icon>
#include <cc/Divider>
#include <cc/Button>
#include <cc/DEBUG>

int main()
{
    using namespace cc;

    const double gap = sp(16);
    const double screenWidth = sp(480);
    const double screenHeight = sp(800);
    const double appBarHeight = AppBar::height();

    StackView stack;
    ListMenu menu;

    PasswordEdit passwordField;
    Checkbox dessert;

    AppBar appBar;
    View screen;
    LineEdit edit;

    return
        StackView{screenWidth, screenHeight, &stack}
        .push(
            View{}
            .add(
                ListMenu{&menu}
                .populate([=](auto target) mutable {
                    for (int i = 0; i < 32; ++i) {
                        target.pane().add(
                            ListItem{}
                            .icon(Picture{Ideographic::AccessPoint, 28})
                            .text(Format{"User %%"}.arg(i))
                            .onClicked([=]() mutable {
                                CC_INSPECT(i);
                                stack.push(
                                    View{&screen}
                                    .add(
                                        Flickable{
                                            screenWidth, screenHeight - appBarHeight,
                                            Pane{}
                                            .add(LineEdit{"First name", &edit})
                                            .add(LineEdit{"Family name"})
                                            .add(
                                                NumberEdit{"Age"}
                                                .value(-1)
                                                .min(0)
                                                .max(1000)
                                                //! \todo allow to step focus, when left empty
                                            )
                                            .add(Divider{})
                                            .add(Checkbox{"Dessert", &dessert})
                                            .add(Checkbox{"Apple"}.groupUnder(dessert))
                                            .add(Checkbox{"Banana"}.groupUnder(dessert))
                                            .add(Divider{})
                                            .add(
                                                PasswordEdit{"Enter password", &passwordField}
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
                                                .leading(Icon{Ideographic::VolumeLow})
                                                .trailing(Icon{Ideographic::VolumeHigh})
                                            )
                                            .add(LineEdit{"Country"})
                                            .layout(
                                                ColumnLayout{}
                                                .margin(gap) //! \todo should have a sensible non-zero default
                                                .spacing(gap)
                                            )
                                        }
                                        .size([=,&screen]() { //! \todo FIXME not using "&screen" causes memory bleed?!
                                            return screen.size() - Size{0, appBarHeight};
                                        })
                                        .pos(0, appBarHeight)
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
                .clip(true)
                .pos(0, appBarHeight)
                .size([=]{ return stack.size() - Size{0, appBarHeight}; })
            )
            .add(
                AppBar{&appBar}
                .title("User Manager")
                .onNavigate([]{ ferr() << "Navigate!" << nl; })
            )
        )
        .onEndOfLife([]{ ferr() << "stack destroyed" << nl; })
        .run();
}

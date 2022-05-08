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

    screen
    .add(
        Flickable{
            screenWidth, screenHeight - appBarHeight,
            Pane{}
            .layout(
                ColumnLayout{}
                .margin(gap) //! \todo should have a sensible non-zero default
                .spacing(gap)
            )
            // .add(Button{}.text("OK").onClicked([&]() mutable { stack.pop(); }))
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
        }
        .size([&]{ return screen.size() - Size{0, appBarHeight}; })
        .pos(0, appBarHeight)
    )
    .add(
        AppBar{}
        .title("Edit user")
        .onDismissed([&]{ stack.pop(); })
    )
    .onEndOfLife([]{ CC_DEBUG << "FIN.2" << nl; });

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
                            .onClicked([=]() mutable { // FIXME: [stack, i, &screen]
                                CC_INSPECT(i);
                                stack.push(screen);
                                /*stack.push( //! \todo resolve issue with this screen, clicking on the red square causes issues
                                    View{}
                                    .add(
                                        Control{}
                                        .pos(20, 20)
                                        .size(100, 100)
                                        .paper("darkred")
                                        .onEndOfLife([]{ CC_DEBUG << "FIN.1.2" << nl; })
                                    )
                                    .onPointerReleased([=](auto) mutable{
                                        stack.pop();
                                        return true;
                                    })
                                    .onEndOfLife([]{ CC_DEBUG << "FIN.1" << nl; })
                                );*/
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
        .run();
}

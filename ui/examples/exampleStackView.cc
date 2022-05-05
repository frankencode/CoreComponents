#include <cc/StackView>
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

    StackView stack;
    ListMenu menu;

    PasswordEdit passwordField;
    Checkbox dessert;

    Flickable screen {
        sp(500), sp(500),
        Pane{}
        .layout(
            ColumnLayout{}
            .margin(gap) //! \todo should have a sensible non-zero default
            .spacing(gap)
        )
        .add(Button{}.text("OK").onClicked([&]() mutable { stack.pop(); }))
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
    };

    return
        StackView{sp(500), sp(500), &stack}
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
                            stack.push(screen);
                            /*stack.push(
                                Control{}
                                .onClicked([=]() mutable {
                                    stack.pop();
                                })
                                .paper("aliceblue")
                            );*/
                        })
                    );
                }
            })
        )
        .run();
}

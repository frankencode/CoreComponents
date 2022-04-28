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
#include <cc/DEBUG>

int main()
{
    using namespace cc;

    const double gap = sp(16);

    PasswordEdit passwordField;
    Checkbox dessert;

    return
        Flickable{
            sp(400), sp(600),
            Pane{}
            .layout(
                ColumnLayout{}
                .margin(gap)
                .spacing(gap)
            )
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
        .run();
}

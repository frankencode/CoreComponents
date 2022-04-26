#include <cc/Flickable>
#include <cc/ColumnLayout>
#include <cc/TextField>
#include <cc/NumberField>
#include <cc/PasswordField>
#include <cc/SliderField>
#include <cc/SwitchField>
#include <cc/Icon>
#include <cc/Divider>
#include <cc/DEBUG>

int main()
{
    using namespace cc;

    const double gap = sp(16);

    PasswordField passwordField;

    return
        Flickable{
            sp(400), sp(500),
            Pane{}
            .layout(
                ColumnLayout{}
                .margin(gap)
                .spacing(gap)
            )
            .add(TextField{"First name"})
            .add(TextField{"Middle name"})
            .add(TextField{"Last name"})
            .add(
                NumberField{"Age"}
                .value(-1)
                .min(0)
                .max(1000)
            )
            .add(Divider{})
            .add(
                PasswordField{"Enter password", &passwordField}
                .onAccepted([=]{
                    CC_INSPECT(passwordField.password());
                })
            )
            .add(PasswordField{"Repeat password"})
            .add(SwitchField{}.text("Super user"))
            .add(SwitchField{}.text("Tutorial mode"))
            .add(TextField{"Street address"})
            .add(TextField{"Zip code"})
            .add(
                SliderField{}
                .leading(Icon{Ideographic::VolumeLow})
                .trailing(Icon{Ideographic::VolumeHigh})
            )
            .add(TextField{"Country"})
        }
        .run();
}

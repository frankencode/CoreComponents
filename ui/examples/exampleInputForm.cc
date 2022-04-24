#include <cc/ui/Application>
#include <cc/ui/Flickable>
#include <cc/ui/ColumnLayout>
#include <cc/ui/TextField>
#include <cc/ui/NumberField>
#include <cc/ui/PasswordField>
#include <cc/ui/SliderField>
#include <cc/ui/SwitchField>
#include <cc/ui/Icon>
#include <cc/ui/Divider>
#include <cc/DEBUG>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    const double gap = sp(16);

    PasswordField passwordField;

    Window{
        Flickable{
            sp(400), sp(500),
            Pane{}
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
                .onAccepted([&]{
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
            .layout(
                ColumnLayout{}
                .margin(gap)
                .spacing(gap)
            )
        }
    }.show();

    return Application{}.run();
}

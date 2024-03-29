#include <cc/Flickable>
#include <cc/ColumnLayout>
#include <cc/LineEdit>
#include <cc/NumberEdit>
#include <cc/PasswordEdit>
#include <cc/Slider>
#include <cc/Switch>
#include <cc/Checkbox>
#include <cc/RadioButton>
#include <cc/Divider>
#include <cc/debugging>

int main()
{
    using namespace cc;

    PasswordEdit passwordField;
    Checkbox dessert;

    RadioButton::Group userType;

    return
        Flickable{
            sp(400), sp(600),
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
            .add(Divider{})
            .add(
                RadioButton{"Admin"}
                .join(&userType)
                .onSelected([]{ ferr() << "admin" << nl; })
            )
            .add(
                RadioButton{"User"}
                .join(&userType)
                .onSelected([]{ ferr() << "user" << nl; })
            )
            .add(
                RadioButton{"Guest"}
                .join(&userType)
                .onSelected([]{ ferr() << "guest" << nl; })
            )
            .add(Divider{})
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
        .run();
}

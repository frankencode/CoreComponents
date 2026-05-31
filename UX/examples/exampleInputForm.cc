#include <cc/Flickable>
#include <cc/LineEdit>
#include <cc/NumberEdit>
#include <cc/PasswordEdit>
#include <cc/Slider>
#include <cc/Switch>
#include <cc/Checkbox>
#include <cc/RadioButton>
#include <cc/Divider>
#include <cc/layout>
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
            Pane{ColumnLayout{}}
            (LineEdit{"First name"})
            (LineEdit{"Family name"})
            (
                NumberEdit{"Age"}
                .value(-1)
                .min(0)
                .max(1000)
                //! \todo allow to step focus, when left empty
            )
            (Divider{})
            (Checkbox{"Dessert"}.associate(&dessert))
            (Checkbox{"Apple"}.groupUnder(dessert))
            (Checkbox{"Banana"}.groupUnder(dessert))
            (Divider{})
            (
                PasswordEdit{"Enter password"}
                .associate(&passwordField)
                .onAccepted([=]{
                    CC_INSPECT(passwordField.password());
                })
            )
            (PasswordEdit{"Repeat password"})
            (Divider{})
            (
                RadioButton{"Admin"}
                .join(&userType)
                .onSelected([]{ ferr() << "admin" << nl; })
            )
            (
                RadioButton{"User"}
                .join(&userType)
                .onSelected([]{ ferr() << "user" << nl; })
            )
            (
                RadioButton{"Guest"}
                .join(&userType)
                .onSelected([]{ ferr() << "guest" << nl; })
            )
            (Divider{})
            (Switch{"Tutorial mode"})
            (LineEdit{"Street address"})
            (LineEdit{"Zip code"})
            (
                Slider{}
                .leading(Icon::VolumeLow)
                .trailing(Icon::VolumeHigh)
            )
            (LineEdit{"Country"})
        }
        .run();
}

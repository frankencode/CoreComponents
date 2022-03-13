#include <cc/ui/Application>
#include <cc/ui/TextField>
#include <cc/ui/Ideographic>
#include <cc/ui/Icon>
#include <cc/Format>
#include <cc/DEBUG>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    // Application{}.fontSmoothing(FontSmoothing::RgbSubpixel);

    TextField field;

    Window{
        View{640, 480}
        .add(
            TextField{&field}
            .title("Username")
            .icon(Ideographic::AccountPlus)
            .alertIcon()
            .placeholder("JohnDoe")
            .help("Allowed characters include uppercase letters, lowercase letters, minus and underscore")
            .status([&]{ return dec(field.text().count(), 2) + "/20"; })
            .accept([&]{
                if (field.text().count() == 0) {
                    field.error("The username cannot be empty");
                    return false;
                }
                for (char ch: field.text()) {
                    if ('a' <= ch && ch <= 'z') continue;
                    if ('A' <= ch && ch <= 'Z') continue;
                    if (ch == '-' || ch == '_') continue;
                    field.error(Format{"Character '%%' is not allowed"} << ch);
                    return false;
                }
                field.error("");
                return true;
            })
            .onAccepted([&]{
                CC_INSPECT(field.text());
            })
            .pos(dp(16), dp(16))
        )
    }.show();

    return Application{}.run();
}

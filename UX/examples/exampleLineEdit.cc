#include <cc/LineEdit>
#include <cc/Icon>
#include <cc/Format>
#include <cc/DEBUG>

int main()
{
    using namespace cc;

    LineEdit edit;

    return
        View{sp(640), sp(480)}
        .add(
            LineEdit{}
            .associate(&edit)
            .title("Username")
            .text("Nobody123")
            .icon(Icon::AccountPlus)
            .alertIcon()
            .placeholder("JohnDoe")
            .help("Allowed characters include uppercase letters, lowercase letters, minus and underscore")
            .status([=]{ return dec(edit.text().count(), 2) + "/20"; })
            .accept([=]() mutable {
                if (edit.text().count() == 0) {
                    edit.error("The username cannot be empty");
                    return false;
                }
                for (char ch: edit.text()) {
                    if ('a' <= ch && ch <= 'z') continue;
                    if ('A' <= ch && ch <= 'Z') continue;
                    if (ch == '-' || ch == '_') continue;
                    edit.error(Format{"Character '%%' is not allowed"} << ch);
                    return false;
                }
                edit.error("");
                return true;
            })
            .onAccepted([=]{
                CC_INSPECT(edit.text());
            })
            .pos(dp(16), dp(16))
        )
        .run();
}

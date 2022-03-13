#include <cc/ui/Application>
#include <cc/ui/Picture>
#include <cc/ui/ListItem>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    Window{
        ListItem{}
        .icon(Picture{Ideographic::AccessPoint, 28})
        .text("Access point")
        .paper(Color::White)
    }.show();

    return Application{}.run();
}

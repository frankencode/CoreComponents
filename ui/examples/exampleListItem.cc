#include <cc/Application>
#include <cc/Picture>
#include <cc/ListItem>

int main()
{
    using namespace cc;

    Window{
        ListItem{}
        .icon(Picture{Ideographic::AccessPoint, 28})
        .text("Access point")
        .paper(Color::White)
    }.show();

    return Application{}.run();
}

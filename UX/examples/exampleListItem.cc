#include <cc/ListItem>
#include <cc/Picture>

int main()
{
    using namespace cc;

    return
        ListItem{}
        .icon(Picture{Ideographic::AccessPoint, 28})
        .title("Access point")
        .paper(Color::White)
        .run();
}

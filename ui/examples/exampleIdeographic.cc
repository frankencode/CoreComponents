#include <cc/Picture>

int main()
{
    using namespace cc;

    return
        Picture{Ideographic::AccountPlus, sp(24)}
        .paper("white")
        .run();
}

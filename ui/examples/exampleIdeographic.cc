#include <cc/Application>
#include <cc/Picture>

int main()
{
    using namespace cc;

    Window{
        // Picture{Ideographic::AccessPoint, 50}
        Picture{Ideographic::AccountPlus, sp(24)}
        .paper("white")
    }.show();

    return Application{}.run();
}

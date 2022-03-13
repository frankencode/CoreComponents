#include <cc/ui/Application>
#include <cc/ui/Picture>

int main()
{
    using namespace cc::ui;

    Window{
        // Picture{Ideographic::AccessPoint, 50}
        Picture{Ideographic::AccountPlus, sp(24)}
        .paper("white")
    }.show();

    return Application{}.run();
}

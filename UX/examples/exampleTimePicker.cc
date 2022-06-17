#include <cc/TimePicker>

int main()
{
    using namespace cc;

    return
        TimePicker{}
        .size(sp(320), sp(320))
        .run();
}

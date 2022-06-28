#include <cc/TimePicker>
#include <cc/Application>
#include <cc/DEBUG>

int main()
{
    using namespace cc;

    TimePicker picker;

    return
        TimePicker{}
        .associate(&picker)
        .onAccepted([=]{
            CC_INSPECT(picker.hour());
            CC_INSPECT(picker.minute());
            Application{}.quit();
        })
        .onRejected([=]{
            Application{}.quit();
        })
        .run();
}

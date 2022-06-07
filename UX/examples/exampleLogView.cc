#include <cc/LogView>
#include <cc/Timer>
#include <cc/System>
#include <cc/FlickableIndicator>
#include <cc/Date>

int main()
{
    using namespace cc;

    LogView log;

    return
        LogView{sp(500), sp(500)}
        .associate(&log)
        .add(FlickableIndicator{})
        .attach(
            Timer{0.5}
            .onTimeout([log] mutable {
                log.appendText(Date::now());
            })
            .start()
        )
        .run();
}

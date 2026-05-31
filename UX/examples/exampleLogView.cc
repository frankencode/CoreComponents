#include <cc/LogView>
#include <cc/Timer>
#include <cc/System>
#include <cc/FlickableIndicator>
#include <cc/Date>
#include <cc/debugging>

/** \todo Fix example, LogView seems to not show newly appended lines.
  */
int main()
{
    using namespace cc;

    LogView log;

    return
        LogView{sp(500), &log}
        .add(FlickableIndicator{})
        .attach(
            Timer{0.5}
            .onTimeout([log]() mutable {
                String text = Date::now();
                log.appendText(text);
                CC_INSPECT(text);
            })
            .start()
        )
        .run();
}

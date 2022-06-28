#include <cc/ClockFace>
#include <cc/DEBUG>

int main() {
    using namespace cc;
    ClockFace clock;
    return
        ClockFace{}
        .associate(&clock)
        .pm(true)
        //.mode(ClockFace::Mode::SetHour)
        //.mode(ClockFace::Mode::SetMinute)
        .mode(ClockFace::Mode::SetSecond)
        .onHourChanged([=]{
            CC_INSPECT(clock.hour());
        })
        .run();
}

#include <cc/Timezone>
#include <cc/Date>
#include <cc/System>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "LocalTime",
        []{
            Timezone timezone = Timezone::local();
            for (auto info: timezone) {
                CC_INSPECT(Date{info.startTime()});
                CC_INSPECT(info.offset());
                CC_INSPECT(info.isDaylightSaving());
                CC_INSPECT(info.timezone());
                CC_DEBUG;
            }

            double time = (uint64_t(1)<<32)-1;
            CC_INSPECT(Date{time});

            TimeInfo info = timezone.infoForTime(time);
            CC_INSPECT(Date{info.startTime()});
            CC_INSPECT(info.offset());
            CC_INSPECT(info.isDaylightSaving());
            CC_INSPECT(info.timezone());
        }
    };

    return TestSuite{argc, argv}.run();
}

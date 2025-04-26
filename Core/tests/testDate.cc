#include <cc/Date>
#include <cc/System>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "BasicDateFormatting",
        []{
            double t = System::now();
            fout() << static_cast<uint64_t>(t) << nl;
            Date d1{Date{t}.time()};
            Date d2{t};

            fout() << d1.dayName() << ", " << d1 << nl;
            fout() << d2.dayName() << ", " << d2 << nl;

            CC_VERIFY(str(d1) == str(d2));
        }
    };

    TestCase {
        "BeginOfEpoch",
        []{
            Date date{0};

            fout() << date << nl;
            fout() << "year: " << date.year() << nl;
            fout() << "month: " << date.month() << nl;
            fout() << "day: " << date.day() << nl;

            CC_VERIFY(date.isValid());
            CC_VERIFY(date.year() == 1970);
            CC_VERIFY(date.month() == 1);
            CC_VERIFY(date.day() == 1);
        }
    };

    TestCase {
        "CalendarDates",
        []{
            struct TestData { int day; int month; int year; int hour; int minutes; int seconds; };
            TestData testData[] {
                {  1,  1, 2250,  0,  0,  0 },
                { 12,  9, 2013,  0,  0,  0 },
                {  3,  9, 2012, 11, 22, 33 },
                {  1,  7, 1981,  0,  0,  0 },
                {  1,  1, 1970,  0,  0,  0 },
                { 24,  7, 1969, 16, 50, 35 },
                {  1,  1, 1968,  0,  0,  0 },
                {  1,  1, 1967,  0,  0,  0 },
                {  1,  7, 1967,  0,  0,  0 },
                {  1,  1, 1900,  0,  0,  0 },
                {  1,  1,    1,  0,  0,  0 }
            };
            for (auto &d: testData) {
                Date date0{d.year, d.month, d.day, d.hour, d.minutes, d.seconds};
                Date date1{date0.time()};
                fout() << int64_t(date0.time()) << nl;
                fout() << "  " << date0.dayName() << ", " << date0 << " == " << date1.dayName() << ", " << date1 << nl;
                CC_VERIFY(date0.time() == date1.time());
                CC_VERIFY(date0.toString() == date1.toString());
            }
        }
    };

    TestCase {
        "Conversions",
        []{
            Date date { 2025, 3, 2, 18, 29, 56 };
            fout() << "time = " << static_cast<int64_t>(date.time()) << nl;
            CC_VERIFY(static_cast<int64_t>(date.time()) == 1740940196);
        }
    };

    return TestSuite{argc, argv}.run();
}

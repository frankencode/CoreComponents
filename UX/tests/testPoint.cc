#include <cc/Point>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "AngleOfPoint",
        []{
            CC_CHECK(deg(angle(Point{1, 0})) == 0);
            CC_CHECK(deg(angle(Point{1, 1})) == 45);
            CC_CHECK(deg(angle(Point{0, 1})) == 90);
            CC_CHECK(deg(angle(Point{-1, 1})) == 135);
            CC_CHECK(deg(angle(Point{-1, 0})) == 180);
            CC_CHECK(deg(angle(Point{-1, -1})) == 225);
            CC_CHECK(deg(angle(Point{0, -1})) == 270);
            CC_CHECK(deg(angle(Point{1, -1})) == 315);
        }
    };

    return TestSuite{argc, argv}.run();
}

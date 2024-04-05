#include <cc/CanFrame>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "StringifyAndParseFrame",
        []{
            CanFrame frame{0x123};
            {
                for (int i = 0; i < frame.size(); ++i) {
                    frame[i] = i + 1;
                }
            }
            CC_INSPECT(frame);

            CanFrame frame2;
            CC_CHECK(CanFrame::read(frame.toString(), &frame2));

            CC_INSPECT(frame2);
            CC_CHECK(frame == frame2);
        }
    };

    return TestSuite{argc, argv}.run();
}

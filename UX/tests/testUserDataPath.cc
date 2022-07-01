#include <cc/Application>
#include <cc/Dir>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "UserDataPath",
        [=]{
            Application app{argc, argv};
            CC_INSPECT(app.userDataPath());
            CC_CHECK(Dir::exists(app.userDataPath()));
        }
    };

    return TestSuite{argc, argv}.run();
}

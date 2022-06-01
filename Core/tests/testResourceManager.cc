#include <cc/ResourceManager>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "BasicResourceLookup",
        []{
            ResourceManager{}.addDirectory("/usr/share/hello/icons");
            ResourceManager{}.addDirectory("/usr/share/hello/icons", "hello/icons");
            CC_CHECK(ResourceManager{}.realPath(":/icons/app.webp") == "/usr/share/hello/icons/app.webp");
            CC_CHECK(ResourceManager{}.realPath(":/hello/icons/app.webp") == "/usr/share/hello/icons/app.webp");
        }
    };

    return TestSuite{argc, argv}.run();
}

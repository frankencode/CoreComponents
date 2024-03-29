#include <cc/YasonWriter>
#include <cc/CaptureSink>
#include <cc/Version>
#include <cc/Color>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "ComplexMessage",
        []{
            MetaObject person{"Person"};
            person("version") = Version{1, 0, 0};
            person("favourite color") = Color{0, 0xFF, 0xFF};
            person("name") = "Hans Mustermann";
            person("age") = 17.5;
            person("hobbies") = { "sky diving", "mountain biking", "poetry" };
            person("picture")("uri") = "http://www.hans-mustermann.de/photo.jpg";
            person("picture")("width") = 400;
            person("picture")("height") = 300;

            CaptureSink sink;
            YasonWriter{sink}.write(person);
            String message = sink.collect();

            String idealMessage =
                "Person {\n"
                "  age: 17.5\n"
                "  \"favourite color\": #00FFFF\n"
                "  hobbies: [ \"sky diving\", \"mountain biking\", \"poetry\" ]\n"
                "  name: \"Hans Mustermann\"\n"
                "  picture: {\n"
                "    height: 300\n"
                "    uri: \"http://www.hans-mustermann.de/photo.jpg\"\n"
                "    width: 400\n"
                "  }\n"
                "  version: 1.0.0\n"
                "}";

            CC_INSPECT(message);
            CC_CHECK(message == idealMessage);
        }
    };

    return TestSuite{argc, argv}.run();
}

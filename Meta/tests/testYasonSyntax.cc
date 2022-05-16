#include <cc/YasonSyntax>
#include <cc/Color>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "SimpleMessage",
        []{
            String message = "{ text: Hello, world! }";

            Variant value = YasonSyntax{}.parse(message);

            CC_CHECK(value.is<MetaObject>());

            MetaObject object = value.to<MetaObject>();

            CC_CHECK_EQUALS(object.members().value("text"), "Hello, world!");
        }
    };

    TestCase {
        "ComplexMessage",
        []{
            String message =
                "Person {\n"
                "  version: 1.0.0\n"
                "  name: Hans Mustermann\n"
                "  age: 17\n"
                "  hobbies: [ Sky Diving, Mountain Biking, Poetry ]\n"
                "  picture: {\n"
                "    uri: \"http://www.hans-mustermann.de/photo.jpg\"\n"
                "    width: 400; height: 300\n"
                "  }\n"
                "  home: {\n"
                "    latitude: 12.34\n"
                "    longitude: 123.4\n"
                "  }\n"
                "  favouriteColor: #FF8000\n"
                "  favouriteNumbers: [ 2, 5, 7 ]\n"
                "  \"Is a super hero?\": true\n"
                "  motto:\n"
                "    \"What I cannot create,\\n\"\n"
                "    \"I do not understand.\"\n"
                "  smiley:<<EOI\n"
                "******\n"
                "* ** *\n"
                "******\n"
                "*    *\n"
                "******\n"
                "EOI\n"
                "  Person {\n"
                "    name: Jane\n"
                "  }\n"
                "  Person {\n"
                "    name: John\n"
                "  }\n"
                "}";

            auto person = YasonSyntax{}.parse(message);
            CC_CHECK(person.is<MetaObject>());

            CC_CHECK(person("version") == "1.0.0");
            CC_CHECK(person("name") == "Hans Mustermann");
            CC_CHECK(person("age").to<long>() == 17);
            CC_CHECK((person("hobbies").to<List<Variant>>() == List<Variant>{"Sky Diving", "Mountain Biking", "Poetry"}));

            auto picture = person("picture");
            CC_CHECK(picture("uri").to<String>().startsWith("http"));
            CC_CHECK(picture("width").to<long>() == 400);
            CC_CHECK(picture("height").to<long>() == 300);

            auto home = person("home");
            CC_CHECK(home("latitude").to<double>() == 12.34);
            CC_CHECK(home("longitude").to<double>() == 123.4);

            CC_CHECK(person("favouriteColor") == "#FF8000");
            CC_CHECK((person("favouriteNumbers").to<List<Variant>>() == List<Variant>{2, 5, 7}));

            CC_CHECK(person("motto") == "What I cannot create,\nI do not understand.");
            CC_CHECK(person("smiley") ==
                "******\n"
                "* ** *\n"
                "******\n"
                "*    *\n"
                "******\n"
            );

            CC_CHECK(person.count() == 2);
            CC_CHECK(person[0]("name") == "Jane");
            CC_CHECK(person[1]("name") == "John");

            // CC_INSPECT(person);
        }
    };

    return TestSuite{argc, argv}.run();
}

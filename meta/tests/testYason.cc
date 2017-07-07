/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/meta/yason>

using namespace cc;
using namespace cc::meta;
using namespace cc::testing;

class ObjectSerialization: public TestCase
{
    void testData(MetaObject *object)
    {
        CC_VERIFY(object->className() == "Person");
        CC_VERIFY(
            object->contains("name") &&
            object->contains("age") &&
            object->contains("hobbies") &&
            object->contains("picture") &&
            object->contains("home") &&
            object->contains("favouriteNumbers") &&
            object->contains("Is a super hero?") &&
            object->contains("motto")
        );

        CC_VERIFY(String(object->value("name")) == "Hans Mustermann");
        CC_VERIFY(int(object->value("age")) == 17);

        VariantList *hobbies = Variant::cast<VariantList *>(object->value("hobbies"));
        CC_VERIFY(String(hobbies->at(0)) == "Sky Diving");
        CC_VERIFY(String(hobbies->at(1)) == "Mountain Biking");
        CC_VERIFY(String(hobbies->at(2)) == "Poetry");

        MetaObject *picture = Variant::cast<MetaObject *>(object->value("picture"));
        ferr() << "Variant::type(object->value(\"picture\")) = " << Variant::type(object->value("picture")) << nl;
        CC_VERIFY(String(picture->value("uri")) == "http://www.hans-mustermann.de/photo.jpg");
        CC_VERIFY(int(picture->value("width")) == 400);
        CC_VERIFY(int(picture->value("height")) == 300);

        MetaObject *home = Variant::cast<MetaObject *>(object->value("home"));
        CC_VERIFY(float(home->value("latitude")) == float(12.34));
        CC_VERIFY(float(home->value("longitude")) == float(123.4));

        VariantList *numbers = Variant::cast<VariantList *>(object->value("favouriteNumbers"));
        CC_VERIFY(int(numbers->at(0)) == 2);
        CC_VERIFY(int(numbers->at(1)) == 5);
        CC_VERIFY(int(numbers->at(2)) == 7);

        CC_VERIFY(object->value("Is a super hero?"));
        CC_VERIFY(
            String(object->value("motto")) ==
            String(
                "What I cannot create,\n"
                "I do not understand."
            )
        );
        CC_VERIFY(
            String(object->value("smiley")) ==
            String(
                "******\n"
                "* ** *\n"
                "******\n"
                "*    *\n"
                "******\n"
            )
        );
        CC_VERIFY(object->children()->count() == 2);
        CC_VERIFY(String(object->children()->at(0)->value("name")) == "Jane");
        CC_VERIFY(String(object->children()->at(1)->value("name")) == "John");
    }

    void run()
    {
        String text =
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

        Ref<MetaObject> object = yason::parse(text);
        testData(object);

        String text2 = object->toString();
        fout() << text2;
        Ref<MetaObject> object2 = yason::parse(text2);
        testData(object2);
    }
};

int main(int argc, char** argv)
{
    CC_TESTSUITE_ADD(ObjectSerialization);

    return TestSuite::instance()->run(argc, argv);
}

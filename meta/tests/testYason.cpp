/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/testing/TestSuite>
#include <flux/stdio>
#include <flux/meta/yason>

using namespace flux;
using namespace flux::meta;
using namespace flux::testing;

class ObjectSerialization: public TestCase
{
    void testData(MetaObject *object)
    {
        FLUX_VERIFY(object->className() == "Person");
        FLUX_VERIFY(
            object->contains("name") &&
            object->contains("age") &&
            object->contains("hobbies") &&
            object->contains("picture") &&
            object->contains("home") &&
            object->contains("favouriteNumbers") &&
            object->contains("Is a super hero?") &&
            object->contains("motto")
        );

        FLUX_VERIFY(String(object->value("name")) == "Hans Mustermann");
        FLUX_VERIFY(int(object->value("age")) == 17);

        VariantList *hobbies = cast<VariantList>(object->value("hobbies"));
        FLUX_VERIFY(String(hobbies->at(0)) == "Sky Diving");
        FLUX_VERIFY(String(hobbies->at(1)) == "Mountain Biking");
        FLUX_VERIFY(String(hobbies->at(2)) == "Poetry");

        MetaObject *picture = cast<MetaObject>(object->value("picture"));
        FLUX_VERIFY(String(picture->value("uri")) == "http://www.hans-mustermann.de/photo.jpg");
        FLUX_VERIFY(int(picture->value("width")) == 400);
        FLUX_VERIFY(int(picture->value("height")) == 300);

        MetaObject *home = cast<MetaObject>(object->value("home"));
        FLUX_VERIFY(float(home->value("latitude")) == float(12.34));
        FLUX_VERIFY(float(home->value("longitude")) == float(123.4));

        VariantList *numbers = cast<VariantList>(object->value("favouriteNumbers"));
        FLUX_VERIFY(int(numbers->at(0)) == 2);
        FLUX_VERIFY(int(numbers->at(1)) == 5);
        FLUX_VERIFY(int(numbers->at(2)) == 7);

        FLUX_VERIFY(object->value("Is a super hero?"));
        FLUX_VERIFY(
            String(object->value("motto")) ==
            String(
                "What I cannot create,\n"
                "I do not understand."
            )
        );
        FLUX_VERIFY(
            String(object->value("smiley")) ==
            String(
                "******\n"
                "* ** *\n"
                "******\n"
                "*    *\n"
                "******\n"
            )
        );
        FLUX_VERIFY(object->children()->count() == 2);
        FLUX_VERIFY(String(object->children()->at(0)->value("name")) == "Jane");
        FLUX_VERIFY(String(object->children()->at(1)->value("name")) == "John");
    }

    void run()
    {
        String text =
            "Person {\n"
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
    FLUX_TESTSUITE_ADD(ObjectSerialization);

    return testSuite()->run(argc, argv);
}

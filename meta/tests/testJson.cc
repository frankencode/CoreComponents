/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/meta/json>
#include "ObjectSerializationTest.h"

class JsonSerializationTest: public ObjectSerializationTest
{
    void run()
    {
        Ref<MetaObject> object = json::parse(testMessage());
        testData(object, true);

        string reproduction = json::stringify(object);
        fout() << reproduction;
        Ref<MetaObject> object2 = json::parse(reproduction);
        testData(object2, false);
    }
};

int main(int argc, char** argv)
{
    CC_TESTSUITE_ADD(JsonSerializationTest);

    return TestSuite::instance()->run(argc, argv);
}

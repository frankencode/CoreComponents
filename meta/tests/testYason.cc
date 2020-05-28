/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/meta/yason>
#include "ObjectSerializationTest.h"

class YasonSerializationTest: public ObjectSerializationTest
{
    void run()
    {
        Ref<MetaObject> object = yason::parse(testMessage());
        testData(object);

        String reproduction = yason::stringify(object);
        fout() << reproduction;
        Ref<MetaObject> object2 = yason::parse(reproduction);
        testData(object2);
    }
};

int main(int argc, char** argv)
{
    CC_TESTSUITE_ADD(YasonSerializationTest);

    return TestSuite::instance()->run(argc, argv);
}

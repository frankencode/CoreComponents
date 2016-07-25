/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/net/base64>

using namespace cc;
using namespace cc::testing;
using namespace cc::net;

class SymmetryExamples: public TestCase
{
    void run() {
        String test[] = { "Man", "Hello world", "", "1" };
        const int testCount = sizeof(test) / sizeof(test[0]);

        for (int i = 0; i < testCount; ++i) {
            String a = test[i], b = base64::encode(a);
            fout("base64(\"%%\") = \"%%\"\n") << a << b;
            CC_VERIFY(base64::decode(b) == a);
        }
    }
};

int main(int argc, char** argv)
{
    CC_TESTSUITE_ADD(SymmetryExamples);

    return TestSuite::instance()->run(argc, argv);
}

/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/debug>
#include <cc/entropy/HuffmanCodec>

using namespace cc;
using namespace cc::entropy;
using namespace cc::testing;

class OneBufferTest: public TestCase
{
public:
    void run() override
    {
        {
            String m = "abrakadabra";
            CC_INSPECT(m);
            String e = HuffmanCodec::encode(m);
            fout() << nl << e->hexDump() << nl;
            String m2 = HuffmanCodec::decode(e);
            CC_INSPECT(m2);
            CC_VERIFY(m == m2);
        }
        {
            String m = "Hello, world!";
            CC_INSPECT(m);
            String e = HuffmanCodec::encode(m);
            fout() << nl << e->hexDump() << nl;
            String m2 = HuffmanCodec::decode(e);
            CC_INSPECT(m2);
            CC_VERIFY(m == m2);
        }
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(OneBufferTest);
    return TestSuite::instance()->run(argc, argv);
}

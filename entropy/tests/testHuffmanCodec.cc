/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/debug>
#include <cc/RandomBytes>
#include <cc/CaptureSink>
#include <cc/ReplaySource>
#include <cc/entropy/HuffmanCodec>

using namespace cc;
using namespace cc::entropy;
using namespace cc::testing;

class OneBufferTest: public TestCase
{
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

class NoiseTest: public TestCase
{
    void run() override
    {
        const off_t dataSize = 10000;
        const int byteMin = 0x20;
        const int byteMax = 0x3F;
        const int seed = 0;

        String message = RandomBytes{seed, dataSize, byteMin, byteMax}->readAll();

        String encodedMessage; {
            CaptureSink capture;
            {
                ReplaySource source{message};
                BitSink sink{capture};
                for (HuffmanCodec huffman; huffman->encode(source, sink););
            }
            encodedMessage = capture->collect();
        }

        ferr() << encodedMessage->head(80)->hexDump() << "..." << nl;

        CC_INSPECT(dataSize);
        CC_INSPECT(encodedMessage->count());
        CC_INSPECT(double(encodedMessage->count()) / dataSize);

        String decodedMessage; {
            CaptureSink capture;
            {
                ReplaySource replay{encodedMessage};
                BitSource source{replay};
                for (HuffmanCodec huffman; huffman->decode(source, capture););
            }
            decodedMessage = capture->collect();
        }

        CC_VERIFY(message == decodedMessage);
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(OneBufferTest);
    CC_TESTSUITE_ADD(NoiseTest);
    return TestSuite::instance()->run(argc, argv);
}

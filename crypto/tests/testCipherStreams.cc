/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/math>
#include <cc/Random>
#include <cc/MemoryStream>
#include <cc/NullStream>
#include <cc/crypto/CipherSink>
#include <cc/crypto/CipherSource>
#include <cc/crypto/AesCipher>

using namespace cc;
using namespace cc::testing;
using namespace cc::crypto;

class TestSmth: public TestCase
{
protected:
    static string testCycle(BlockCipher *cipher, CharArray *text)
    {
        string buffer = CharArray::allocate(roundUpToNext(cipher->blockSize(), text->count()));
        {
            Ref<Stream> stream = MemoryStream::open(mutate(buffer));
            Ref<CipherSink> sink = CipherSink::open(cipher, stream, NullStream::instance());
            sink->write(text);
        }
        string text2 = string::create(text->count());
        {
            Ref<Stream> stream = MemoryStream::open(mutate(buffer));
            Ref<CipherSource> source = CipherSource::open(cipher, stream);
            source->read(mutate(text2));
        }
        return text2;
    }

};

class TestBlockBoundary: public TestSmth
{
    void run()
    {
        string key = "\x2b\x7e\x15\x16\x28\xae\xd2\xa6\xab\xf7\x15\x88\x09\xcf\x4f\x3c";
        Ref<BlockCipher> cipher = AesCipher::create(key);
        string text = "0123456789,0123456789,0123456789";
        for (int n = 1; n <= text->count(); ++n) {
            string text1 = text->copy(0, n);
            string text2 = testCycle(cipher, mutate(text1));
            fout() << text1 << " >> " << text2 << nl;
            CC_VERIFY(text1 == text2);
        }
    }
};

class TestBufferBoundary: public TestSmth
{
    void run()
    {
        string key = "\x2b\x7e\x15\x16\x28\xae\xd2\xa6\xab\xf7\x15\x88\x09\xcf\x4f\x3c";
        Ref<BlockCipher> cipher = AesCipher::create(key);
        string text = string::create(0x1001);
        Ref<Random> random = Random::open();
        for (int i = 0; i < text->count(); ++i) mutate(text)->byteAt(i) = random->get() & 0xFF;
        {
            string text1 = text->copy(0, text->count() - 1);
            string text2 = testCycle(cipher, mutate(text1));
            CC_VERIFY(text1 == text2);
        }
        {
            string text1 = text;
            string text2 = testCycle(cipher, mutate(text1));
            CC_VERIFY(text1 == text2);
        }
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(TestBlockBoundary);
    CC_TESTSUITE_ADD(TestBufferBoundary);

    return TestSuite::instance()->run(argc, argv);
}

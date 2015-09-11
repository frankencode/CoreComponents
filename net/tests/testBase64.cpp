/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/testing/TestSuite>
#include <flux/stdio>
#include <flux/net/base64>

using namespace flux;
using namespace flux::testing;
using namespace flux::net;

class SymmetryExamples: public TestCase
{
    void run() {
        String test[] = { "Man", "Hello world", "", "1" };
        const int testCount = sizeof(test) / sizeof(test[0]);

        for (int i = 0; i < testCount; ++i) {
            String a = test[i], b = base64::encode(a);
            fout("base64(\"%%\") = \"%%\"\n") << a << b;
            FLUX_VERIFY(base64::decode(b) == a);
        }
    }
};

int main(int argc, char** argv)
{
    FLUX_TESTSUITE_ADD(SymmetryExamples);

    return testSuite()->run(argc, argv);
}

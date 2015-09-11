/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/testing/TestSuite>
#include <flux/stdio>
#include <flux/net/Uri>

using namespace flux;
using namespace flux::testing;
using namespace flux::net;

class BreakDownUri: public TestCase
{
    void run() {
        Ref<Uri> uri = Uri::create("http://john@example.com:8000/%7ejohn/123.php?say=hello#part1");
        fout("uri->scheme() = \"%%\"\n") << uri->scheme();
        fout("uri->userInfo() = \"%%\"\n") << uri->userInfo();
        fout("uri->host() = \"%%\"\n") << uri->host();
        fout("uri->port() = %%\n") << uri->port();
        fout("uri->path() = \"%%\"\n") << uri->path();
        fout("uri->query() = \"%%\"\n") << uri->query();
        fout("uri->fragment() = \"%%\"\n") << uri->fragment();
        FLUX_VERIFY(uri->scheme() == "http");
        FLUX_VERIFY(uri->userInfo() == "john");
        FLUX_VERIFY(uri->host() == "example.com");
        FLUX_VERIFY(uri->port() == 8000);
        FLUX_VERIFY(uri->path() == "/~john/123.php");
        FLUX_VERIFY(uri->query() == "say=hello");
        FLUX_VERIFY(uri->fragment() == "part1");
    }
};

int main(int argc, char** argv)
{
    FLUX_TESTSUITE_ADD(BreakDownUri);

    return testSuite()->run(argc, argv);
}

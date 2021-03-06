/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/net/Uri>

using namespace cc;
using namespace cc::testing;
using namespace cc::net;

class BreakDownUri: public TestCase
{
    void run() {
        Uri uri{"http://john@example.com:8000/%7ejohn/123.php?say=hello#part1"};
        fout("uri->scheme() = \"%%\"\n") << uri->scheme();
        fout("uri->userInfo() = \"%%\"\n") << uri->userInfo();
        fout("uri->host() = \"%%\"\n") << uri->host();
        fout("uri->port() = %%\n") << uri->port();
        fout("uri->path() = \"%%\"\n") << uri->path();
        fout("uri->query() = \"%%\"\n") << uri->query();
        fout("uri->fragment() = \"%%\"\n") << uri->fragment();
        CC_VERIFY(uri->scheme() == "http");
        CC_VERIFY(uri->userInfo() == "john");
        CC_VERIFY(uri->host() == "example.com");
        CC_VERIFY(!uri->hostIsNumeric());
        CC_VERIFY(uri->port() == 8000);
        CC_VERIFY(uri->path() == "/~john/123.php");
        CC_VERIFY(uri->query() == "say=hello");
        CC_VERIFY(uri->fragment() == "part1");
    }
};

class BreakDownSimpleUri: public TestCase
{
    void run() {
        Uri uri{"localhost:8080"};
        fout("uri->host() = \"%%\"\n") << uri->host();
        fout("uri->port() = %%\n") << uri->port();
        CC_VERIFY(uri->host() == "localhost");
        CC_VERIFY(!uri->hostIsNumeric());
        CC_VERIFY(uri->port() == 8080);
    }
};

class HostIsNumeric: public TestCase
{
    void run() {
        Uri uri{"127.0.0.1:80"};
        fout("uri->host() = \"%%\"\n") << uri->host();
        fout("uri->port() = %%\n") << uri->port();
        CC_VERIFY(uri->host() == "127.0.0.1");
        CC_VERIFY(uri->hostIsNumeric());
        CC_VERIFY(uri->port() == 80);
    }
};

class Inet6Address: public TestCase
{
    void run() {
        Uri uri{"[::1]:80"};
        fout("uri->host() = \"%%\"\n") << uri->host();
        fout("uri->port() = %%\n") << uri->port();
        CC_VERIFY(uri->host() == "::1");
        CC_VERIFY(uri->hostIsNumeric());
        CC_VERIFY(uri->port() == 80);
    }
};

int main(int argc, char** argv)
{
    CC_TESTSUITE_ADD(BreakDownUri);
    CC_TESTSUITE_ADD(BreakDownSimpleUri);
    CC_TESTSUITE_ADD(HostIsNumeric);
    CC_TESTSUITE_ADD(Inet6Address);

    return TestSuite::instance()->run(argc, argv);
}

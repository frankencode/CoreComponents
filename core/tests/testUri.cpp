#include <flux/stdio.h>
#include <flux/check.h>
#include <flux/Uri.h>

using namespace flux;

int main()
{
	Ref<Uri> uri = Uri::create("http://john@example.com:8000/%7ejohn/123.php?say=hello#part1");
	fout("uri->scheme() = \"%%\"\n") << uri->scheme();
	fout("uri->userInfo() = \"%%\"\n") << uri->userInfo();
	fout("uri->host() = \"%%\"\n") << uri->host();
	fout("uri->port() = %%\n") << uri->port();
	fout("uri->path() = \"%%\"\n") << uri->path();
	fout("uri->query() = \"%%\"\n") << uri->query();
	fout("uri->fragment() = \"%%\"\n") << uri->fragment();
	check(uri->scheme() == "http");
	check(uri->userInfo() == "john");
	check(uri->host() == "example.com");
	check(uri->port() == 8000);
	check(uri->path() == "/~john/123.php");
	check(uri->query() == "say=hello");
	check(uri->fragment() == "part1");
	return 0;
}

#include <ftl/stdio>
#include <ftl/misc>

namespace ftl
{

int main()
{
	auto uri = Uri::create("http://john@example.com:8000/%7ejohn/123.php?say=hello#part1");
	print("uri->scheme() = \"%%\"\n", uri->scheme());
	print("uri->userInfo() = \"%%\"\n", uri->userInfo());
	print("uri->host() = \"%%\"\n", uri->host());
	print("uri->port() = %%\n", uri->port());
	print("uri->path() = \"%%\"\n", uri->path());
	print("uri->query() = \"%%\"\n", uri->query());
	print("uri->fragment() = \"%%\"\n", uri->fragment());
	return 0;
}

} // namespace ftl

int main()
{
	return ftl::main();
}

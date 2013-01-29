#include <ftl/stdio>
#include <ftl/Base64.hpp>

int main() {
	using namespace ftl;
	print("string(\"Man\")->base64() = \"%%\"\n", string("Man")->base64());
	print("string(\"Hello world\")->base64() = \"%%\"\n", string("Hello world")->base64());
	print("string(\"\")->base64() = \"%%\"\n", string("")->base64());
	print("s1 = \"%%\"\n", Base64::decode(Base64::encode(string("Man"))));
	print("s2 = \"%%\"\n", Base64::decode(Base64::encode(string("Hello world"))));
	print("s3 = \"%%\"\n", Base64::decode(Base64::encode(string("1"))));
	print("s4 = \"%%\"\n", Base64::decode(Base64::encode(string(""))));
	return 0;
}

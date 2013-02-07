#include <ftl/PrintDebug.hpp>
#include <ftl/Base64.hpp>

int main() {
	using namespace ftl;
	print("String(\"Man\")->base64() = \"%%\"\n", String("Man")->base64());
	print("String(\"Hello world\")->base64() = \"%%\"\n", String("Hello world")->base64());
	print("String(\"\")->base64() = \"%%\"\n", String("")->base64());
	print("s1 = \"%%\"\n", Base64::decode(Base64::encode(String("Man"))));
	print("s2 = \"%%\"\n", Base64::decode(Base64::encode(String("Hello world"))));
	print("s3 = \"%%\"\n", Base64::decode(Base64::encode(String("1"))));
	print("s4 = \"%%\"\n", Base64::decode(Base64::encode(String(""))));
	return 0;
}

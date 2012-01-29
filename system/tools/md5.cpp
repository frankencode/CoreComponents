#include <ftl/stdio>
#include <ftl/binary>

int main()
{
	using namespace ftl;
	
	ByteArray buf(FTL_DEFAULT_BUF_CAPA);
	Md5 md5;
	int nr = 0;
	while ((nr = rawInput()->readAvail(buf.data(), buf.size())) > 0)
		md5.feed(buf.data(), nr);
	uint8_t sum[16];
	mem::clr(sum, 16);
	md5.finish(sum);
	for (int i = 0; i < 16; ++i)
		print("%hex:2.:'0'%", sum[i]);
	print("\n");
	buf.clear();
	
	return 0;
}

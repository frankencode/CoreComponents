#include <ftl/stdio>

int main()
{
	using namespace ftl;
	
	for (int i = 0; i < 256; ++i)
	{
		uint32_t c = i;
		for (int k = 0; k < 8; ++k)
		{
			if (c & 1)
				c = 0xedb88320L ^ (c >> 1);
			else
				c >>= 1;
		}
		print("0x%hex:8:8.:'0'%", c);
		if (i != 255) print(",");
		if ((i+1)%8 == 0) print("\n");
		else print(" ");
	}
	return 0;
}

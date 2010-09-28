#include "types.hpp"
#include "Buffer.hpp"
#include "Stream.hpp"

namespace ftl
{

void Stream::read(void* buf, int bufCapa)
{
	uint8_t* buf2 = (uint8_t*)buf;
	int n = bufCapa;
	while (n > 0)
	{
		int dn = readAvail(buf2, n);
		if (dn == 0)
			FTL_THROW(StreamIoException, "Reading beyond end of input");
		buf2 += dn;
		n -= dn;
	}
}

Ref<ByteArray, Owner> Stream::readAll()
{
	Buffer buffer;
	while (true) {
		Ref<Block> block = buffer.allocate();
		block->setFill(readAvail(block->data(), buffer.blockSize()));
		if (block->fill() == 0) break;
	}
	return buffer.join();
}

} // namespace ftl

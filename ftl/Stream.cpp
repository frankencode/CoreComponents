#include "types.hpp"
#include "Buffer.hpp"
#include "Stream.hpp"

namespace ftl
{

int Stream::readAvail(Ref<ByteArray> buf)
{
	return readAvail(buf->data(), buf->size());
}

void Stream::read(void* buf, int bufFill)
{
	uint8_t* p = (uint8_t*)buf;
	int n = bufFill;
	while (n > 0) {
		int dn = readAvail(p, n);
		if (dn == 0)
			FTL_THROW(StreamIoException, "Reading beyond end of input");
		p += dn;
		n -= dn;
	}
}

void Stream::read(Ref<ByteArray> buf)
{
	read(buf->data(), buf->size());
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

void Stream::write(Ref<ByteArray> buf) {
	write(buf->data(), buf->size());
}

} // namespace ftl

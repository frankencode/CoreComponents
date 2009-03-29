#include "MemoryMapping.hpp"

namespace pona
{

MemoryMapping::MemoryMapping(Ref<File> file, off_t offset, size_t length, int type)
	: length_(length)
{
	if (!file->isOpen())
		PONA_THROW(StreamSemanticException, "File needs to be opened, before it can be mapped.");
	
	int prot = 0;
	if ((file->openFlags() & File::Read) != 0)
		prot |= Read;
	if ((file->openFlags() & File::Write) != 0)
		prot |= Write;
	if ((file->openFlags() & File::Execute) != 0)
		prot |= Execute;
	
	start_ = ::mmap(NULL, length, prot, type, file->fd(), offset);
	if (start_ == MAP_FAILED)
		PONA_SYSTEM_EXCEPTION;
}

MemoryMapping::MemoryMapping(size_t length, int prot, int type)
	: length_(length)
{
	start_ = ::mmap(NULL, length, prot, type, 0, 0);
	if (start_ == MAP_FAILED)
		PONA_SYSTEM_EXCEPTION;
}

MemoryMapping::~MemoryMapping()
{
	if (::munmap(start_, length_) == -1)
		PONA_SYSTEM_EXCEPTION;
}

} // namespace pona

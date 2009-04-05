#include <unistd.h>
#include "Pipe.hpp"

namespace pona
{

Pipe::Pipe()
{
	int fd[2];
	if (::pipe(fd) == -1)
		PONA_SYSTEM_EXCEPTION;
	rawInput_ = new SystemStream(fd[0]);
	rawOutput_ = new SystemStream(fd[1]);
}

Ref<SystemStream> Pipe::rawInput() { return rawInput_; }
Ref<SystemStream> Pipe::rawOutput() { return rawOutput_; }

} // namespace pona

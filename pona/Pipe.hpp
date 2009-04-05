#ifndef PONA_PIPE_HPP
#define PONA_PIPE_HPP

#include "atoms"
#include "SystemStream.hpp"

namespace pona
{

class Pipe: public Instance
{
public:
	Pipe();
	Ref<SystemStream> rawInput();
	Ref<SystemStream> rawOutput();
	
private:
	Ref<SystemStream, Owner> rawInput_;
	Ref<SystemStream, Owner> rawOutput_;
};

} // namespace pona

#endif // PONA_PIPE_HPP

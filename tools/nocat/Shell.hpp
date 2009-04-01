#ifndef NOCAT_SHELL_HPP
#define NOCAT_SHELL_HPP

#include <pona/stdio>
#include "Options.hpp"

namespace nocat
{

using namespace pona;

class Shell: public Instance
{
public:
	Shell(Ref<Options> options);
	void session(Ref<Stream> stream);
	void transfer(Ref<Stream> stream);
	
private:
	Ref<Options, Owner> options_;
};

} // namespace nocat

#endif // NOCAT_SHELL_HPP

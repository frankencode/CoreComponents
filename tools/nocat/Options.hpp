#ifndef NOCAT_OPTIONS_HPP
#define NOCAT_OPTIONS_HPP

#include <pona/context>
#include <pona/network>

namespace nocat
{

using namespace pona;

class Options: public ToolOptions
{
public:
	Options(int argc, char** argv);
	Ref<SocketAddress, Owner> address() const;
	
	Variant server_, client_, help_;
	Variant host_, port_, inet6_;
	Variant exec_;
	Variant pool_, backlog_, ioUnit_;
	Variant loop_, repeat_;
	Variant timeout_;
	Variant bench_;
	Ref<StringList, Owner> files_;
};

} // namespace nocat

#endif // NOCAT_OPTIONS_HPP

#ifndef RGET_OPTIONS_HPP
#define RGET_OPTIONS_HPP

#include <pona/context>
#include <pona/thread>
#include <pona/network>

namespace rget
{

using namespace pona;

class Options: public ToolOptions
{
	PONA_SHARED
	
public:
	static Ref<Options> instance();
	
	void read(int argc, char** argv);
	
	Ref<SocketAddress, Owner> address() const;
	
	Variant server_, client_, help_;
	Variant host_, port_, inet6_, backlog_;
	Variant canon_, editor_, eol_, ioUnit_;
	Variant logging_, logDir_;
	Variant exec_, tunnel_;
	Variant loop_, repeat_;
	Ref<StringList, Owner> files_;
	
	String editorPath_;
	int loggingFlags_;
	
private:
	static Mutex lock_;
	Options();
};

inline Ref<Options> options() { return Options::instance(); }

} // namespace rget

#endif // RGET_OPTIONS_HPP

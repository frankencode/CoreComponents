#ifndef NOCAT_CLIENT_HPP
#define NOCAT_CLIENT_HPP

#include <pona/stdio>
#include <pona/network>
#include "Options.hpp"

namespace nocat
{

using namespace pona;

class Client: public Instance
{
public:
	Client(Ref<Options> options);
	int connect();
	
private:
	Ref<Options, Owner> options_;
};

} // namespace nocat

#endif // NOCAT_CLIENT_HPP

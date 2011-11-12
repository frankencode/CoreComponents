#include <signal.h>
#include "debug.hpp"

namespace ftl
{

void abort()
{
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGABRT);
	pthread_sigmask(SIG_UNBLOCK, &set, 0);
	raise(SIGABRT);
}

} // namespace ftl

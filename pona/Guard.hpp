#ifndef PONA_GUARD_HPP
#define PONA_GUARD_HPP

#include "Mutex.hpp"

namespace pona
{

class PONA_API Guard: public Mutex
{
public:
	Guard() { acquire(); }
	~Guard() { release(); }
};

} // namespace pona

#endif // PONA_GUARD_HPP

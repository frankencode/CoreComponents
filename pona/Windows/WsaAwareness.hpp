#ifndef PONA_WSAAWARENESS_HPP
#define PONA_WSAAWARENESS_HPP

#include "../Atoms.hpp"

namespace pona
{

class WsaAwareness: public Instance
{
public:
	static Ref<WsaAwareness, RefSetNull> instance();
	
private:
	WsaAwareness();
	~WsaAwareness();
};

inline Ref<WsaAwareness> wsaAwareness() { return WsaAwareness::instance(); }

} // namespace pona

#endif // PONA_WSAAWARENESS_HPP


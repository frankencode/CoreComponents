#ifndef PONA_CSTRING_HPP
#define PONA_CSTRING_HPP

#include "atoms"
#include "Vector.hpp"

namespace pona
{

class CString: public Ref<Vector<char>, Owner>
{
public:
	CString() {}
	CString(int length): Ref<Vector<char>, Owner>(new Vector<char>(length + 1)) { get()->set(length, 0); }
	inline operator char*() const { return get()->at(0); }
	inline operator void*() const { return (void*)get()->at(0); }
	inline operator uint8_t*() const { return (uint8_t*)get()->at(0); }
};

} // namespace pona

#endif // PONA_CSTRING_HPP

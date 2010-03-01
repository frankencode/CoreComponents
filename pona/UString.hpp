#ifndef PONA_USTRING_HPP
#define PONA_USTRING_HPP

#include <assert.h>
#include "atoms"
#include "UStringIndex.hpp"
#include "Array.hpp"
#include "List.hpp"

namespace pona
{

class UString;

typedef List<UString> UStringList;
typedef Array<uint8_t, DeepCopyZeroTerminatedArray> UStringMedia;

class UString: public Ref<UStringMedia, Owner>
{
public:
	typedef UStringMedia Media;
	typedef Ref<Media, Owner> Super;
	typedef UStringIndex Index;
	
	UString(): Super(new Media) {}
	UString(int size): Super(new Media(size)) {}
	UString(int size, char zero): Super(new Media(size, zero)) {
		assert((0 <= zero) && (zero <= 127));
	}
	UString(const char* data, int size = -1) {
		if (size < 0) size = pona::strlen(data);
		validate(data, size);
		assign(data, size);
	}
	UString(const UString& b) { assign(b.data(), b.size()); }
	UString(const Index& index0, const Index& index1) {
		assign(index0.pos(), index1.pos() - index0.pos());
	}
	UString(Ref<UStringList> parts, const char* glue = "") { assign(parts, glue); }
	~UString() { reset(); }
	
	inline void reset() { Super::get()->reset(); }
	
	inline UString& operator=(const char* data) {
		int size = pona::strlen(data);
		validate(data, size);
		reset();
		assign(data, size);
		return *this;
	}
	
	inline UString& operator=(const UString& b) {
		reset();
		assign(b.data(), b.size());
		return *this;
	}
	
	inline UString& operator=(Ref<UStringList> parts) {
		reset();
		assign(parts);
		return *this;
	}
	
	inline Index first() const { return empty() ? Index() : Index(data()); }
	inline Index last() const { return empty() ? Index() : eoi() - 1; }
	inline Index eoi() const { return empty() ? Index() : Index(data(), data() + size()); }
	
	inline int size() const { return Super::get()->size(); }
	inline int empty() const { return Super::get()->empty(); }
	
	inline bool def(const Index& index) const {
		assert(!empty());
		assert(index.data() == data());
		return index.valid();
	}
	inline uchar_t get(const Index& index) const {
		assert(!empty());
		assert(index.data() == data());
		return index.getChar();
	}
	inline uchar_t operator[](const Index& index) { return get(index); }
	
	inline char* data() const { return reinterpret_cast<char*>(Super::get()->data()); }
	inline operator char*() const { return reinterpret_cast<char*>(Super::get()->data()); }
	inline void validate() const { validate(data(), size()); }
	inline bool valid() const {
		try { validate(); }
		catch (StreamIoException&) { return false; }
		return true;
	}
	
	Index find(const Index& index, const char* pattern) const;
	Ref<UStringList, Owner> split(const char* pattern) const;
	
	inline Index find(const char* pattern) const { return find(first(), pattern); }
	inline bool contains(const char* pattern) const { return find(first(), pattern).valid(); }
	
	inline bool operator< (const UString& b) const { return pona::strcmp(data(), b.data()) <  0; }
	inline bool operator==(const UString& b) const { return pona::strcmp(data(), b.data()) == 0; }
	inline bool operator> (const UString& b) const { return pona::strcmp(data(), b.data()) >  0; }
	inline bool operator!=(const UString& b) const { return pona::strcmp(data(), b.data()) != 0; }
	inline bool operator<=(const UString& b) const { return pona::strcmp(data(), b.data()) <= 0; }
	inline bool operator>=(const UString& b) const { return pona::strcmp(data(), b.data()) >= 0; }
	
	/*int toInt(bool* ok = 0);
	double toFloat(bool* ok = 0);
	int64_t toInt64(bool* ok = 0);
	uint64_t toUInt64(bool* ok = 0);
	float64_t toFloat64(bool* ok = 0);*/
	
private:
	static void validate(const char* data, int size = -1);
	inline void assign(const char* data, int size = -1) {
		if (size < 0) size = pona::strlen(data);
		set(new Media(reinterpret_cast<const uint8_t*>(data), size));
	}
	void assign(Ref<UStringList> parts, const char* glue = "");
};

Ref<UStringList, Owner> operator+(const UString& a, const UString& b);
Ref<UStringList, Owner> operator+(const UString& a, const char* b);
Ref<UStringList, Owner> operator+(const char* a, const UString& b);
Ref<UStringList, Owner> operator+(Ref<UStringList, Owner> list, const UString& b);

} // namespace pona

#endif // PONA_USTRING_HPP

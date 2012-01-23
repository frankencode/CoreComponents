#ifndef FTL_XCLIENT_HPP
#define FTL_XCLIENT_HPP

#include <ftl/Exception.hpp>
#include <ftl/String.hpp>
#include <ftl/Mutex.hpp>

namespace ftl
{

class StreamSocket;

FTL_EXCEPTION(XException, Exception);

class XClient: public Instance
{
public:
	XClient(String host = "", int display = 0, int screen = 0);
	
	inline int majorVersion() const { return majorVersion_; }
	inline int minorVersion() const { return minorVersion_; }
	inline uint32_t releaseNumber() const { return releaseNumber_; }
	inline String vendor() const { return vendor_; }
	
private:
	Ref<StreamSocket, Owner> socket_;
	
	int majorVersion_;
	int minorVersion_;
	uint32_t releaseNumber_;
	String vendor_;
	
	uint32_t allocateResourceId();
	void freeResourceId(uint32_t id);
	Mutex resourceIdMutex_;
	uint32_t resourceIdBase_;
	uint32_t resourceIdMask_;
	uint32_t nextResourceId_;
	Ref<List<uint32_t>, Owner> freeResourceIds_;
	
	int imageEndian_;
};

} // namespace ftl

#endif // FTL_XCLIENT_HPP

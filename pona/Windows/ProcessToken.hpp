#ifndef PONA_PROCESSTOKEN_HPP
#define PONA_PROCESSTOKEN_HPP

#include <Aclapi.h>

namespace pona
{

class ProcessToken
{
public:
	ProcessToken();
	~ProcessToken();

	PSID ownerSid() const;
	PSID groupSid() const;
	PACL defaultDacl() const;

	wchar_t* owner() const;
	wchar_t* group() const;
	wchar_t* domain() const;

	SECURITY_ATTRIBUTES securityAttributes(int mask);

private:
	PSID ownerSid_;
	PSID groupSid_;
	PACL defaultDacl_;
	
	LPVOID bufOwner_;
	LPVOID bufGroup_;
	LPVOID bufDefaultDacl_;
	wchar_t* owner_;
	wchar_t* group_;
	wchar_t* domain_;
};

} // namespace pona

#endif // PONA_PROCESSTOKEN_HPP

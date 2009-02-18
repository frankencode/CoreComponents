/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstraction Library.
**
** Copyright (C) 2007-2009  Frank Mertens
**
** This file is part of a free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License,
** or (at your option) any later version.
**
** The library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this libary.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include <Windows.h>
#include "../Exception.hpp"
#include "ProcessToken.hpp"

namespace pona
{

ProcessToken::ProcessToken()
	: ownerSid_(0),
	  groupSid_(0),
	  defaultDacl_(0),
	  bufOwner_(0),
	  bufGroup_(0),
	  bufDefaultDacl_(0),
	  owner_(0),
	  group_(0),
	  domain_(0)
{
	HANDLE processToken = 0;

	{
		BOOL ok = OpenProcessToken(GetCurrentProcess(), TOKEN_READ, &processToken);
		if (!ok)
			PONA_WINDOWS_EXCEPTION;
	}

	{
		DWORD bufLen = 0, retLen = 0;
		GetTokenInformation(processToken, TokenOwner, 0, 0, &bufLen);
		bufOwner_ = LocalAlloc(LPTR, bufLen);
		BOOL ok = GetTokenInformation(processToken, TokenOwner, bufOwner_, bufLen, &retLen);
		if (!ok)
			PONA_WINDOWS_EXCEPTION;
		TOKEN_OWNER* uglyCast = (TOKEN_OWNER*)bufOwner_;
		ownerSid_ = uglyCast->Owner;
	}

	{
		DWORD bufLen = 0, retLen = 0;
		GetTokenInformation(processToken, TokenPrimaryGroup, 0, 0, &bufLen);
		bufGroup_ = LocalAlloc(LPTR, bufLen);
		BOOL ok = GetTokenInformation(processToken, TokenPrimaryGroup, bufGroup_, bufLen, &retLen);
		if (!ok)
			PONA_WINDOWS_EXCEPTION;
		TOKEN_PRIMARY_GROUP* uglyCast = (TOKEN_PRIMARY_GROUP*)bufGroup_;
		groupSid_ = uglyCast->PrimaryGroup;
	}

	{
		DWORD bufLen = 0, retLen = 0;
		GetTokenInformation(processToken, TokenDefaultDacl, 0, 0, &bufLen);
		bufDefaultDacl_ = LocalAlloc(LPTR, bufLen);
		BOOL ok = GetTokenInformation(processToken, TokenDefaultDacl, bufDefaultDacl_, bufLen, &retLen);
		if (!ok)
			PONA_WINDOWS_EXCEPTION;
		TOKEN_DEFAULT_DACL* uglyCast = (TOKEN_DEFAULT_DACL*)bufDefaultDacl_;
		defaultDacl_ = uglyCast->DefaultDacl;
	}

	CloseHandle(processToken);

	{
		DWORD ownerLen = 0;
		DWORD groupLen = 0;
		DWORD domainLen = 0;
		SID_NAME_USE use;
		BOOL ok;

		ownerLen = domainLen = 0;
		LookupAccountSid(0, ownerSid_, 0, &ownerLen, 0, &domainLen, &use);
		owner_ = new wchar_t[ownerLen+1];
		domain_ = new wchar_t[domainLen+1];
		ok = LookupAccountSid(0, ownerSid_, owner_, &ownerLen, domain_, &domainLen, &use);
		if (!ok)
			PONA_WINDOWS_EXCEPTION;
		owner_[ownerLen] = 0;
		domain_[domainLen] = 0;

		if (IsWellKnownSid(groupSid_, WinAccountDomainUsersSid))
		{
			groupLen = domainLen + 1;
			group_ = new wchar_t[groupLen+1];
			group_[0] = '@';
			for (DWORD i = 0; i < domainLen; ++i)
				group_[i+1] = domain_[i];
			group_[groupLen] = 0;
		}
		else
		{
			groupLen = domainLen = 0;
			LookupAccountSid(0, groupSid_, 0, &groupLen, 0, &domainLen, &use);
			group_ = new wchar_t[groupLen+1];
			wchar_t* buf = new wchar_t[domainLen+1];
			BOOL ok = LookupAccountSid(0, groupSid_, group_, &groupLen, buf, &domainLen, &use);
			delete[] buf;
			if (!ok)
				PONA_WINDOWS_EXCEPTION;
			group_[groupLen] = 0;
		}
	}
}

ProcessToken::~ProcessToken()
{
	if (bufOwner_ != 0)
	{
		LocalFree(bufOwner_);
		bufOwner_ = 0;
	}
	if (bufGroup_ != 0)
	{
		LocalFree(bufGroup_);
		bufGroup_ = 0;
	}
	if (bufDefaultDacl_ != 0)
	{
		LocalFree(bufDefaultDacl_);
		bufDefaultDacl_ = 0;
	}
	if (owner_ != 0)
	{
		delete[] owner_;
		owner_ = 0;
	}
	if (group_ != 0)
	{
		delete[] group_;
		group_ = 0;
	}
	if (domain_ != 0)
	{
		delete[] domain_;
		domain_ = 0;
	}
}

PSID ProcessToken::ownerSid() const { return ownerSid_; }
PSID ProcessToken::groupSid() const { return groupSid_; }
PACL ProcessToken::defaultDacl() const { return defaultDacl_; }

wchar_t* ProcessToken::owner() const { return owner_; }
wchar_t* ProcessToken::group() const { return group_; }
wchar_t* ProcessToken::domain() const { return domain_; }

SECURITY_ATTRIBUTES ProcessToken::securityAttributes(int mask)
{
	PSID worldSid;
	SID_IDENTIFIER_AUTHORITY authority = SECURITY_WORLD_SID_AUTHORITY;
	AllocateAndInitializeSid(&authority, 1, 0, 0, 0, 0, 0, 0, 0, 0, &worldSid);

	const int eaCount = 3;
	EXPLICIT_ACCESS ea[eaCount];
	
	memset(ea, 0, sizeof(ea));

	ea[0].grfAccessPermissions =
		(((mask & 01) != 0) * GENERIC_EXECUTE) |
		(((mask & 02) != 0) * GENERIC_WRITE) |
		(((mask & 04) != 0) * GENERIC_READ);
	ea[0].grfAccessMode = GRANT_ACCESS;
	ea[0].grfInheritance= NO_INHERITANCE;
	ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	ea[0].Trustee.ptstrName = (LPTSTR)worldSid;

	ea[1].grfAccessPermissions =
		(((mask & 010) != 0) * GENERIC_EXECUTE) |
		(((mask & 020) != 0) * GENERIC_WRITE) |
		(((mask & 040) != 0) * GENERIC_READ);
	ea[1].grfAccessMode = GRANT_ACCESS;
	ea[1].grfInheritance= NO_INHERITANCE;
	ea[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[1].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea[1].Trustee.ptstrName = (LPTSTR)groupSid();

	ea[2].grfAccessPermissions =
		(((mask & 0100) != 0) * GENERIC_EXECUTE) |
		(((mask & 0200) != 0) * GENERIC_WRITE) |
		(((mask & 0400) != 0) * GENERIC_READ);
	ea[2].grfAccessMode = GRANT_ACCESS;
	ea[2].grfInheritance= NO_INHERITANCE;
	ea[2].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[2].Trustee.TrusteeType = TRUSTEE_IS_USER;
	ea[2].Trustee.ptstrName = (LPTSTR)ownerSid();

	PACL acl = 0;
	DWORD ret = SetEntriesInAcl(eaCount, ea, 0, &acl);
	if (ret != ERROR_SUCCESS) PONA_WINDOWS_EXCEPTION;

	PSECURITY_DESCRIPTOR sd = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
	InitializeSecurityDescriptor(sd, SECURITY_DESCRIPTOR_REVISION);
	if (!SetSecurityDescriptorDacl(sd, TRUE, acl, FALSE)) PONA_WINDOWS_EXCEPTION;

	SECURITY_ATTRIBUTES sa;
	memset(&sa, 0, sizeof(sa));
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = sd;
	sa.bInheritHandle = TRUE;

	return sa;
}

} // namespace pona

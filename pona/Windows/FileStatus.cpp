/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstractions Library.
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

#include <Aclapi.h>
#include "../FileStatus.hpp"

namespace pona
{

void FileStatus::update()
{
	exists_ = false;
	mode_ = 0;
	size_ = -1;
	owner_ = String();
	group_ = String();
	
	//-------------------------------------------------------------------
	// Retrieve ownership and access information
	//-------------------------------------------------------------------
	
	PSID ownerSid = 0, groupSid = 0;
	PACL dacl, sacl;
	PSECURITY_DESCRIPTOR sd;
	
	wchar_t* pathWc = path_.wcsdup();
	DWORD ret = GetNamedSecurityInfo(
		/* pObjectName = */ pathWc,
		/* ObjectType = */ SE_FILE_OBJECT,
		/* SecurityInfo =  */
		OWNER_SECURITY_INFORMATION |
		GROUP_SECURITY_INFORMATION |
		DACL_SECURITY_INFORMATION,
		&ownerSid,
		&groupSid,
		&dacl,
		&sacl,
		&sd
	);
	
	if (ret != ERROR_SUCCESS) {
		free(pathWc);
		return;
	}
	
	exists_ = true;
	
	//-------------------------------------------------------------------
	// Translate DACL of file to UNIX-style file mode
	//-------------------------------------------------------------------
	
	{
		ACCESS_MASK mask;
		{
			TRUSTEE trustee;
			ZeroMemory(&trustee, sizeof(trustee));
			trustee.TrusteeForm = TRUSTEE_IS_SID;
			trustee.TrusteeType = TRUSTEE_IS_USER;
			trustee.ptstrName = (LPTSTR)ownerSid;
			DWORD ret = GetEffectiveRightsFromAcl(dacl, &trustee, &mask);
			if (ret != ERROR_SUCCESS)
				PONA_WINDOWS_EXCEPTION;
		}

		mode_ |= 0400 * ((mask & FILE_READ_DATA) != 0);
		mode_ |= 0200 * ((mask & FILE_WRITE_DATA) != 0);
		mode_ |= 0100 * ((mask & FILE_EXECUTE) != 0);
	}

	{
		ACCESS_MASK mask;
		{
			TRUSTEE trustee;
			ZeroMemory(&trustee, sizeof(trustee));
			trustee.TrusteeForm = TRUSTEE_IS_SID;
			trustee.TrusteeType = TRUSTEE_IS_GROUP;
			trustee.ptstrName = (LPTSTR)groupSid;
			DWORD ret = GetEffectiveRightsFromAcl(dacl, &trustee, &mask);
			if (ret != ERROR_SUCCESS)
				PONA_WINDOWS_EXCEPTION;
		}
		
		mode_ |= 040 * ((mask & FILE_READ_DATA) != 0);
		mode_ |= 020 * ((mask & FILE_WRITE_DATA) != 0);
		mode_ |= 010 * ((mask & FILE_EXECUTE) != 0);
	}

	{
		PSID worldSid;
		SID_IDENTIFIER_AUTHORITY authority = SECURITY_WORLD_SID_AUTHORITY;
		AllocateAndInitializeSid(&authority, 1, 0, 0, 0, 0, 0, 0, 0, 0, &worldSid);
		
		ACCESS_MASK mask;
		{
			TRUSTEE trustee;
			ZeroMemory(&trustee, sizeof(trustee));
			trustee.TrusteeForm = TRUSTEE_IS_SID;
			trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
			trustee.ptstrName = (LPTSTR)worldSid;
			DWORD ret = GetEffectiveRightsFromAcl(dacl, &trustee, &mask);
			if (ret != ERROR_SUCCESS)
				PONA_WINDOWS_EXCEPTION;
		}
		
		mode_ |= 04 * ((mask & FILE_READ_DATA) != 0);
		mode_ |= 02 * ((mask & FILE_WRITE_DATA) != 0);
		mode_ |= 01 * ((mask & FILE_EXECUTE) != 0);
	}

	//-------------------------------------------------------------------
	// Retrieve owner and group name
	//-------------------------------------------------------------------
	
	while (true)
	{
		DWORD ownerLen = 0;
		DWORD groupLen = 0;
		DWORD domainLen = 0;
		SID_NAME_USE use;
		BOOL ok;

		ownerLen = domainLen = 0;
		LookupAccountSid(0, ownerSid, 0, &ownerLen, 0, &domainLen, &use);
		wchar_t* ownerWc = new wchar_t[ownerLen+1];
		wchar_t* groupWc = 0;
		wchar_t* domainWc = new wchar_t[domainLen+1];
		ok = LookupAccountSid(0, ownerSid, ownerWc, &ownerLen, domainWc, &domainLen, &use);
		if (!ok)
			break;
		ownerWc[ownerLen] = 0;
		domainWc[domainLen] = 0;

		if (IsWellKnownSid(groupSid, WinAccountDomainUsersSid))
		{
			groupLen = domainLen + 1;
			groupWc = new wchar_t[groupLen+1];
			groupWc[0] = '@';
			for (DWORD i = 0; i < domainLen; ++i)
				groupWc[i+1] = domainWc[i];
			groupWc[groupLen] = 0;
		}
		else
		{
			groupLen = domainLen = 0;
			LookupAccountSid(0, groupSid, 0, &groupLen, 0, &domainLen, &use);
			groupWc = new wchar_t[groupLen+1];
			wchar_t* buf = new wchar_t[domainLen+1];
			BOOL ok = LookupAccountSid(0, groupSid, groupWc, &groupLen, buf, &domainLen, &use);
			delete[] buf;
			if (!ok)
				break;
			groupWc[groupLen] = 0;
		}
		
		owner_ = ownerWc;
		group_ = groupWc;

		delete[] ownerWc;
		delete[] groupWc;

		break;
	}

	//-------------------------------------------------------------------
	// Retrieve file size
	//-------------------------------------------------------------------
	
	{
		WIN32_FIND_DATA findData;
		
		HANDLE handle = FindFirstFile(pathWc, &findData);
		if (handle != INVALID_HANDLE_VALUE)
		{
			size_ = findData.nFileSizeHigh;
			size_ <<= (sizeof(findData.nFileSizeLow) * 8);
			size_ |= findData.nFileSizeLow;
		}
	}

	free(pathWc);
}

} // namespace pona

//---------------------------------------------------------------------------
// $RCSfile: wxGUID.cpp $
// $Source: contrib/src/common/wxGUID.cpp $
// $Revision: 1.6 $
// $Date: Mar 30, 2005 9:22:56 PM $
//---------------------------------------------------------------------------
// Author:      Jorgen Bodde
// Copyright:   (c) Jorgen Bodde
// License:     wxWidgets License
//---------------------------------------------------------------------------

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifdef __LINUX__
#include <uuid/uuid.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#include "wxGUID.h"
#include <stdio.h>
#include <string.h>


//----------------------------------------------------------------------------
// wxGUID
//----------------------------------------------------------------------------

wxGUID::wxGUID(bool create)
{
	if(create)
		Generate();
	else
		Clear();
}

wxGUID::wxGUID(const wxGUID &value)
{
	Copy(value);
}

wxGUID::~wxGUID()
{
	// TODO: Enter destructor code
}

bool wxGUID::operator==(const wxGUID &value) const     
{
    return IsEqual(value); 
}

bool wxGUID::operator!=(const wxGUID &value) const 
{ 
    return !IsEqual(value); 
}

void wxGUID::operator=(const wxGUID &value) 
{
    Copy(value); 
}

bool wxGUID::IsEqual(const wxGUID &value) const
{
	// returns true if both guid's are equal

	for(int i = 0; i < WX_GUID_LEN; i++)
	{
		if(guid[i] != value.guid[i])
			return false;
	}

	return true;
}

bool wxGUID::Generate()
{
	bool value = false;
	
#ifdef _WIN32
	unsigned char *ptr;
	HRESULT res;
	GUID uuid;

	res = CoCreateGuid(&uuid);
	if(res == S_OK)
	{
		// copy the values
		ptr = (unsigned char *)&uuid.Data1;
		for(int i = 0; i < 4; i++)
			guid[i] = ptr[i];

		guid[4] = uuid.Data2 & 0xff;
		guid[5] = (uuid.Data2 >> 8) & 0xff;

		guid[6] = uuid.Data3 & 0xff;
		guid[7] = (uuid.Data3 >> 8) & 0xff;

		for(int j = 0; j < 8; j++)
			guid[8+j] = uuid.Data4[j];

		value = true;
	}

#endif
#ifdef __LINUX__
	uuid_t uid;
	uuid_generate(uid);

	wxASSERT_MSG(WX_GUID_LEN == sizeof(uuid_t), 
	             wxT("wxGUID::Generate() - Size of WX_GUID_LEN is not equal to uuid_t struct"));
	
	for(int i = 0; i < 16; i++)
		guid[i] = uid[i];

	// expect it always to be true
	value = true;
#endif

	return value;
}

void wxGUID::Clear()
{
	// clear GUID (initialise)

	for(int i = 0; i < WX_GUID_LEN; i++)
		guid[i] = 0;
}

void wxGUID::GuidToStr(char* s) const
{
	sprintf(s, "{%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
			   guid[0], guid[1], guid[2],  guid[3],  guid[4],  guid[5],  guid[6],  guid[7],
			   guid[8], guid[9], guid[10], guid[11], guid[12], guid[13], guid[14], guid[15]);
}

void wxGUID::Copy(const wxGUID &value)
{
	for(int i = 0; i < WX_GUID_LEN; i++)
		guid[i] = value.guid[i];
}

bool wxGUID::StrToGuid(const char *value)
{
	unsigned char number;

	Clear();

	size_t len = strlen(value);
	if (len < 38)
		return false;

	if(value[0] != '{' && value[len - 1] != '}')
		return false;

	value++;

	for(int i = 0; i < 16; i++)
	{
		if(GetStringByte(value, number))
			guid[i] = number;
		else
		{
			Clear();
			return false;
		}
	}

	return true;
}

const unsigned char *wxGUID::GetData() const 
{
     return guid;
}


bool wxGUID::GetStringByte(const char*& str, unsigned char &result)
{
	unsigned char high, low;

	if (str[0] == 0  ||  str[1] == 0)
		return false;		// no room

	if (str[0] == '-')
	{
		str++;
	}

	if (GetNibble(str[0], high) && GetNibble(str[1], low))
	{
		// two succeeding nibbles
		result = ((high << 4) & 0xf0) + low;
		str = str + 2;
		return true;
	}

	return false;
}

bool wxGUID::GetNibble(unsigned char nibble, unsigned char &value)
{
	bool result = false;

	if(nibble >= 'a' && nibble <= 'f')
	{
		value = nibble - 87;
		result = true;
	}
	else if(nibble >= 'A' && nibble <= 'F')
	{
		value = nibble - 45;
		result = true;
	}
	else if(nibble >= '0' && nibble <= '9')
	{
		value = nibble - 48;
		result = true;
	}

	return result;
}

void wxGUID::SetData(const unsigned char *data)
{
	if(data)
	{
		// copy the data in the buffer
		for(int i = 0; i < WX_GUID_LEN; i++)
			guid[i] = data[i];
	}
	else
		Clear();
}

bool wxGUID::IsBlank() const
{
	// returns wether every GUID number is 0
	// this is not possible (even statistically seen)

	for(int i = 0; i < WX_GUID_LEN; i++)
	{
		if(guid[i] != 0)
			return false;
	}

	return true;
}

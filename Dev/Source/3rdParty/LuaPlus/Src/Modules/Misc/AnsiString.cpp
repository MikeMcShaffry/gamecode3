#include "Misc_InternalPch.h"
#include "AnsiString.h"
#include "FixedSizeAllocator.h"
#include <stdarg.h>
#include <ctype.h>
#include "trio.h"

namespace Misc {

#define ROUND(x,y) (((x)+(y-1))&~(y-1))
#define ROUND4(x) ROUND(x, 4)
static FixedSizeAllocator poolAllocator64(ROUND4(65*sizeof(char)+12), 100);
static FixedSizeAllocator poolAllocator128(ROUND4(129*sizeof(char)+12), 100);
static FixedSizeAllocator poolAllocator256(ROUND4(257*sizeof(char)+12), 100);
static FixedSizeAllocator poolAllocator512(ROUND4(513*sizeof(char)+12), 100);

void AnsiStringData::Free()
{
    if (this->maxSize == 64)
        poolAllocator64.Free(this);
    else if (this->maxSize == 128)
        poolAllocator128.Free(this);
    else if (this->maxSize == 256)
        poolAllocator256.Free(this);
    else  if (this->maxSize == 512)
        poolAllocator512.Free(this);
    else
    {
        assert(this->maxSize > 512);
        delete[] (unsigned char*)this;
    }
}



char nilAnsiCharacter = '\0';


static const struct
{
    AnsiStringData data;
    char dummy;
} g_strEmpty = { {-1, 0, 0}, 0 };

extern const char* anEmptyAnsiString = &g_strEmpty.dummy;

static int CountStream(trio_pointer_t p, int)
{
	int& count = *(int*)p;
	count++;
	return 0;
}


AnsiString AnsiString::Format(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	int len = 0;
	trio_vcprintf(CountStream, &len, format, args);

	AnsiString dest;
	dest.AllocBuffer(len);
	trio_vsnprintf(dest.m_string, len + 1, format, args);

	va_end(args);
	return dest;
}


AnsiString AnsiString::FormatV(const char* format, va_list args)
{
	int len = 0;
	trio_vcprintf(CountStream, &len, format, args);

	AnsiString dest;
	dest.AllocBuffer(len);
	trio_vsnprintf(dest.m_string, len + 1, format, args);
	return dest;
}


const AnsiString& AnsiString::operator=(const AnsiString& stringSrc)
{
    if (m_string == stringSrc.m_string)
		return *this;

	AnsiStringData* srcStringData = stringSrc.GetStringData();
	AnsiStringData* stringData = GetStringData();

	if (srcStringData->stringLen == 0)
	{
		Clear();
		return *this;
	}

    if ((stringData->refCount < 0  &&  stringData != &g_strEmpty.data) || srcStringData->refCount < 0)
    {
		AllocBeforeWrite(srcStringData->stringLen);
		memcpy(m_string, stringSrc.m_string, srcStringData->stringLen * sizeof(char));
		GetStringData()->stringLen = srcStringData->stringLen;
		m_string[srcStringData->stringLen] = 0;
		return *this;
	}

	GetStringData()->Unref();
    m_string = stringSrc.m_string;
    GetStringData()->Ref();
	return *this;
}


const AnsiString& AnsiString::operator=(const char* srcString)
{
	size_t srcStringLen = StrLen(srcString);
	if (srcStringLen == 0)
	{
		Clear();
		return *this;
	}

	AllocBeforeWrite(srcStringLen);
	memcpy(m_string, srcString, srcStringLen * sizeof(char));
	GetStringData()->stringLen = srcStringLen;
	m_string[srcStringLen] = 0;
    return *this;
}


const AnsiString& AnsiString::operator=(char ch)
{
	AllocBeforeWrite(1);
	m_string[0] = ch;
	GetStringData()->stringLen = 1;
	m_string[1] = 0;
    return *this;
}


void AnsiString::Clear()
{
	AnsiStringData* pData = GetStringData();

	if (pData->stringLen == 0)
		return;
	if (pData->refCount >= 0)
	{
		pData->Unref();
		m_string = (char*)anEmptyAnsiString;
	}
	else
		*this = &nilAnsiCharacter;
}


AnsiString AnsiString::Sub(size_t startPos) const
{
	size_t stringLen = GetStringData()->stringLen;
	if (startPos >= stringLen)
		return AnsiString();
	return Sub(startPos, stringLen - startPos);
}


AnsiString AnsiString::Sub(size_t startPos, size_t count) const
{
	size_t stringLen = GetStringData()->stringLen;
	if (startPos > stringLen  ||  count == 0)
		return AnsiString();
	if (startPos + count > stringLen)
		count = stringLen - startPos;

	AnsiString dest;
	dest.AllocBuffer(count);
	memcpy(dest.m_string, m_string + startPos, count * sizeof(char));
	return dest;
}


AnsiString AnsiString::Upper() const
{
	AnsiString dest = *this;
	dest.CopyBeforeWrite();
	char* ptr = dest.m_string;
	while (*ptr != 0)
	{
		*ptr = toupper(*ptr);
		ptr++;
	}
	return dest;
}


AnsiString AnsiString::Lower() const
{
	AnsiString dest = *this;
	dest.CopyBeforeWrite();
	char* ptr = dest.m_string;
	while (*ptr != 0)
	{
		*ptr = tolower(*ptr);
		ptr++;
	}
	return dest;
}


AnsiString AnsiString::TrimLeft(char ch) const
{
	AnsiString dest;

	const char* str = m_string;
	if (ch == 0)
	{
		while (isspace(*str))
			str++;
	}
	else
	{
		while (*str == ch)
			str++;
	}

	size_t count = GetStringData()->stringLen - (str - m_string);
	if (count > 0)
	{
		dest.AllocBuffer(count);
		memcpy(dest.m_string, str, count);
	}

	return dest;
}


AnsiString AnsiString::TrimRight(char ch) const
{
	AnsiString dest;

	size_t stringLen = GetStringData()->stringLen;
	char* str = (char*)m_string + stringLen - 1;
	if (ch == 0)
	{
		while (str != m_string  &&  isspace(*str))
			str--;
	}
	else
	{
		while (str != m_string  &&  *str == ch)
			str++;
	}

	size_t count = str - m_string + 1;
	dest.AllocBuffer(count);
	memcpy(dest.m_string, m_string, count);
	return dest;
}


int AnsiString::Find(char ch, size_t start) const
{
	if (start >= GetStringData()->stringLen)
		return -1;

	const char* found = strchr(m_string + start, ch);
	return found ? (int)(found - m_string) : -1;
}


int AnsiString::Find(const char* strSearch, size_t start) const
{
	if (start >= GetStringData()->stringLen)
		return -1;

	const char* found = strstr(m_string + start, strSearch);
	return found ? (int)(found - m_string) : -1;
}


int AnsiString::ReverseFind(char ch) const
{
	const char* found = strrchr(m_string, ch);
	return found ? (int)(found - m_string) : -1;
}


AnsiString AnsiString::BeforeLast(char ch) const
{
	AnsiString str;
	int pos = ReverseFind(ch);
	if (pos == -1)
		str = *this;
	else
		str = AnsiString(m_string, pos);

	return str;
}


AnsiString AnsiString::AfterLast(char ch) const
{
	AnsiString str;
	int pos = ReverseFind(ch);
	if (pos == -1)
		str = *this;
	else
		str = m_string + pos + 1;

	return str;
}


AnsiString AnsiString::Replace(char oldCh, char newCh) const
{
	AnsiString dest;
	dest.AllocBuffer(GetStringData()->stringLen);

	char* destPtr = dest.m_string;
	for (char* srcPtr = m_string; *srcPtr; ++srcPtr, ++destPtr)
	{
		char ch = *srcPtr;
		if (ch == oldCh)
			*destPtr = newCh;
		else
			*destPtr = ch;
	}

	return dest;
}


char* AnsiString::GetBuffer(size_t nLen)
{
	AllocBeforeWrite(nLen);
	return m_string;
}

void AnsiString::ReleaseBuffer()
{
	GetStringData()->stringLen = StrLen(m_string);
}

void AnsiString::ReleaseBuffer(size_t nLen)
{
	m_string[nLen] = 0;
	GetStringData()->stringLen = nLen;
}


void AnsiString::AllocBuffer(size_t nLen)
{
	if (nLen == 0)
		m_string = (char*)anEmptyAnsiString;
	else
	{
		AnsiStringData* pData;
		if (nLen <= 64)
		{
			pData = (AnsiStringData*)poolAllocator64.Alloc(64);
			pData->maxSize = 64;
		}
		else if (nLen <= 128)
		{
			pData = (AnsiStringData*)poolAllocator128.Alloc(128);
			pData->maxSize = 128;
		}
		else if (nLen <= 256)
		{
			pData = (AnsiStringData*)poolAllocator256.Alloc(256);
			pData->maxSize = 256;
		}
		else if (nLen <= 512)
		{
			pData = (AnsiStringData*)poolAllocator512.Alloc(512);
			pData->maxSize = 512;
		}
		else
		{
			pData = (AnsiStringData*)
				new unsigned char[sizeof(AnsiStringData) + (nLen+1)*sizeof(char)];
			pData->maxSize = nLen;
		}
		pData->refCount = 1;
		pData->stringLen = nLen;
		m_string = pData->data();
		m_string[nLen] = '\0';
	}
}


void AnsiString::CopyBeforeWrite()
{
	AnsiStringData* stringData = GetStringData();
	if (!stringData->IsShared())
		return;
	stringData->Unref();
	size_t stringLen = stringData->stringLen;
	AllocBuffer(stringLen);
	memcpy(m_string, stringData->data(), (stringLen + 1) * sizeof(char));
}


void AnsiString::AllocBeforeWrite(size_t len)
{
	AnsiStringData* stringData = GetStringData();
	if (stringData->IsShared()  ||  len > stringData->maxSize)
	{
		stringData->Unref();
		AllocBuffer(len);
	}
}


AnsiString& AnsiString::ConcatCopy(size_t leftLen, const char* leftStr, size_t rightLen, const char* rightStr)
{
	size_t totalLen = leftLen + rightLen;
	if (totalLen == 0)
		return *this;

	AllocBuffer(totalLen);
	memcpy(m_string, leftStr, leftLen * sizeof(char));
	memcpy(m_string + leftLen, rightStr, rightLen * sizeof(char));
	return *this;
}


AnsiString& AnsiString::ConcatInPlace(int rightStringLen, const char* rightString)
{
	if (rightStringLen == 0)
		return *this;

	AnsiStringData* origStringData = GetStringData();
	size_t origStringLen = origStringData->stringLen;
	if (origStringData->refCount > 1  ||  origStringLen + rightStringLen > origStringData->maxSize)
	{
		const char* origString = m_string;
		AllocBuffer(origStringData->stringLen + rightStringLen);
		memcpy(m_string, origString, origStringLen * sizeof(char));
		origStringData->Unref();
		memcpy(m_string + origStringLen, rightString, rightStringLen * sizeof(char));
		return *this;
	}

	memcpy(m_string + origStringLen, rightString, rightStringLen * sizeof(char));
	origStringLen += rightStringLen;
	origStringData->stringLen = origStringLen;
	m_string[origStringLen] = 0;
	return *this;
}

} // namespace Misc


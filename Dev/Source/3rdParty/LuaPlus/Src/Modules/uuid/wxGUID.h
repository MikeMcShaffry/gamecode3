//---------------------------------------------------------------------------
// $RCSfile: wxGUID.h $
// $Source: contrib/include/wx/common/wxGUID.h $
// $Revision: 1.6 $
// $Date: Mar 30, 2005 9:22:56 PM $
//---------------------------------------------------------------------------
// Author:      Jorgen Bodde
// Copyright:   (c) Jorgen Bodde
// License:     wxWidgets License
//---------------------------------------------------------------------------

#ifndef __WXGUID_HPP_
#define __WXGUID_HPP_

#define WX_GUID_LEN 16

//----------------------------------------------------------------------------
// wxGUID
//----------------------------------------------------------------------------

class wxGUID
{
public:
    wxGUID(bool create = true);
	wxGUID(const wxGUID &value);
    virtual ~wxGUID();

    bool operator==(const wxGUID &value) const;
    bool operator!=(const wxGUID &value) const;
    void operator=(const wxGUID &value);

	bool IsEqual(const wxGUID &value) const;
	void Copy(const wxGUID &value);
	bool Generate();
	void Clear();
	void GuidToStr(char* buffer) const;
	bool StrToGuid(const char *value);
	
    const unsigned char *GetData() const;
	
    void SetData(const unsigned char *data);

	bool IsBlank() const;

private:
	bool GetNibble(unsigned char nibble, unsigned char &value);
	bool GetStringByte(const char*& str, unsigned char &result);
	unsigned char guid[WX_GUID_LEN];
};


#endif

/////////////////////////////////////////////////////////////////////////////
// Purpose:     wxImage
// Author:      J Winwood
// Created:     14/11/2001
// Copyright:   (c) 2001-2002 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

%enum wxBitmapType
    wxBITMAP_TYPE_INVALID
    wxBITMAP_TYPE_BMP
    wxBITMAP_TYPE_BMP_RESOURCE
    wxBITMAP_TYPE_RESOURCE
    wxBITMAP_TYPE_BMP_RESOURCE
    wxBITMAP_TYPE_ICO
    wxBITMAP_TYPE_ICO_RESOURCE
    wxBITMAP_TYPE_CUR
    wxBITMAP_TYPE_CUR_RESOURCE
    wxBITMAP_TYPE_XBM
    wxBITMAP_TYPE_XBM_DATA
    wxBITMAP_TYPE_XPM
    wxBITMAP_TYPE_XPM_DATA
    wxBITMAP_TYPE_TIF
    wxBITMAP_TYPE_TIF_RESOURCE
    wxBITMAP_TYPE_GIF
    wxBITMAP_TYPE_GIF_RESOURCE
    wxBITMAP_TYPE_PNG
    wxBITMAP_TYPE_PNG_RESOURCE
    wxBITMAP_TYPE_JPEG
    wxBITMAP_TYPE_JPEG_RESOURCE
    wxBITMAP_TYPE_PNM
    wxBITMAP_TYPE_PNM_RESOURCE
    wxBITMAP_TYPE_PCX
    wxBITMAP_TYPE_PCX_RESOURCE
    wxBITMAP_TYPE_PICT
    wxBITMAP_TYPE_PICT_RESOURCE
    wxBITMAP_TYPE_ICON
    wxBITMAP_TYPE_ICON_RESOURCE
    wxBITMAP_TYPE_MACCURSOR
    wxBITMAP_TYPE_MACCURSOR_RESOURCE
    wxBITMAP_TYPE_ANY
%end

///////////////////////////////////////////////////////////////////////////////
// wxImage

%if wxLUA_USE_wxImage

%include "wx/image.h"

%class %delete wxImage, wxObject
	~%wxver22 %define %object wxNullImage

	wxImage(const wxImage& image)
    %constructor wxDefaultImage()
    ~%wxver22 %constructor wxImageFromBitmap(const wxBitmap& bitmap)
    %constructor wxEmptyImage(int width, int height)
    %constructor wxImageFromData(int width, int height, unsigned char* data, bool static_data = FALSE)
    %constructor wxImageFromFile(const wxString& name, long type = wxBITMAP_TYPE_ANY)
    static void AddHandler(wxImageHandler* handler)
    static void CleanUpHandlers()
    //wxBitmap ConvertToBitmap() const
    //wxImage  ConvertToMono() const
    wxImage Copy() const
    void Create(int width, int height)
    void Destroy()
    unsigned char GetBlue(int x, int y) const
    //unsigned char* GetData() const
    unsigned char GetGreen(int x, int y) const
    unsigned char GetRed(int x, int y) const
    static wxList& GetHandlers()
    int GetHeight() const
    unsigned char GetMaskBlue() const
    unsigned char GetMaskGreen() const
    unsigned char GetMaskRed() const
    //const wxPalette& GetPalette() const
    wxImage GetSubImage(const wxRect& rect) const
    int GetWidth() const
    ~%wxver22 wxString GetOption(const wxString &name) const
    ~%wxver22 int GetOptionInt(const wxString &name) const
    ~%wxver22 int HasOption(const wxString &name) const
    bool HasMask() const
    static void InitStandardHandlers()
    static void InsertHandler(wxImageHandler* handler)
    bool LoadFile(const wxString& name, long type = wxBITMAP_TYPE_ANY)
    %rename LoadMimeFile bool LoadFile(const wxString& name, const wxString& mimetype)
    bool Ok() const
    static bool RemoveHandler(const wxString& name)
    bool SaveFile(const wxString& name, int type)
    %rename SaveMimeFile bool SaveFile(const wxString& name, const wxString& mimetype)
    wxImage Mirror(bool horizontally = TRUE) const
    void Replace(unsigned char r1, unsigned char g1, unsigned char b1, unsigned char r2, unsigned char g2, unsigned char b2)
    // wxImage& Rescale(int width, int height)
    wxImage Rotate(double angle, const wxPoint& rotationCentre, bool interpolating = TRUE, wxPoint* offsetAfterRotation = NULL)
    wxImage Rotate90(bool clockwise = TRUE) const
    wxImage Scale(int width, int height) const
    void SetData(unsigned char *data)
    void SetMask(bool hasMask = TRUE)
    //void SetMaskColour(unsigned char red, unsigned char blue, unsigned char green)
    void SetMaskColour(unsigned char red, unsigned char blue, unsigned char green)
    ~%wxver22 void SetOption(const wxString &name, const wxString &value)
    ~%wxver22 %rename SetOptionInt void SetOption(const wxString &name, int value)
    //void SetPalette(const wxPalette& palette)
    void SetRGB(int x, int y, unsigned char red, unsigned char green, unsigned char blue)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxImageHandler and derived classes

%class %noclassinfo wxImageHandler, wxObject
    wxString GetName() const
    wxString GetExtension() const
    long GetType() const
    wxString GetMimeType() const
    void SetName(const wxString& name)
    void SetExtension(const wxString& extension)
    void SetMimeType(const wxString& mimetype)
    void SetType(long type)
    %property=Name, read, write
    %property=Extension, read, write
    %property=MimeType, read, write
    %property=Type, read, write
%endclass

%class wxBMPHandler, wxImageHandler
    %include "wx/imagbmp.h"
    wxBMPHandler()
%endclass

%class wxPNGHandler, wxImageHandler
    %include "wx/imagpng.h"
    wxPNGHandler()
%endclass

%class wxJPEGHandler, wxImageHandler
    %include "wx/imagjpeg.h"
    wxJPEGHandler()
%endclass

%class wxGIFHandler, wxImageHandler
    %include "wx/imaggif.h"
    wxGIFHandler()
%endclass

%class wxPNMHandler, wxImageHandler
    %include "wx/imagpnm.h"
    wxPNMHandler()
%endclass

%class wxPCXHandler, wxImageHandler
    %include "wx/imagpcx.h"
    wxPCXHandler()
%endclass

%class wxTIFFHandler, wxImageHandler
    %include "wx/imagtiff.h"
    wxTIFFHandler()
%endclass

%wxver22 %skip
%class wxXPMHandler, wxImageHandler
    %include "wx/imagxpm.h"
    wxXPMHandler()
%endclass

%wxver22 %skip
%class %noclassinfo wxICOHandler, wxBMPHandler
    %include "wx/imagbmp.h"
    wxICOHandler()
%endclass

%endif wxLUA_USE_wxImage

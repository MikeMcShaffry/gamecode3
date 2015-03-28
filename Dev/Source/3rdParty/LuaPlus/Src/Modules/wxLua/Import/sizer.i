/////////////////////////////////////////////////////////////////////////////
// Purpose:     wxSizers and wxLayoutConstraints
// Author:      J Winwood
// Created:     14/11/2001
// Copyright:   (c) 2001-2002 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

%if wxLUA_USE_wxSizer

///////////////////////////////////////////////////////////////////////////////
// wxSizer

%class wxSizer, wxObject
    %rename AddWindow void Add(wxWindow* window, int option = 0,int flag = 0, int border = 0, wxObject* userData = NULL)
    %rename AddSizer void Add(wxSizer* sizer, int option = 0, int flag = 0, int border = 0, wxObject* userData = NULL)
    void Add(int width, int height, int option = 0, int flag = 0, int border = 0, wxObject* userData = NULL)
    wxSize CalcMin()
    void Fit(wxWindow* window)
    wxSize GetSize()
    %property=Size, read
    wxPoint GetPosition()
    %property=Position, read
    wxSize GetMinSize()
    %property=MinSize, read
    void Layout()
    %rename PrependWindow void Prepend(wxWindow* window, int option = 0, int flag = 0, int border = 0, wxObject* userData = NULL)
    %rename PrependSizer void Prepend(wxSizer* sizer, int option = 0, int flag = 0, int border = 0, wxObject* userData = NULL)
    void Prepend(int width, int height, int option = 0, int flag = 0, int border= 0, wxObject* userData = NULL)
    void RecalcSizes()
    <wxver23 %rename RemoveWindow bool Remove(wxWindow* window)
    %rename RemoveSizer bool Remove(wxSizer* sizer)
    bool Remove(int nth)
    void SetDimension(int x, int y, int width, int height)
    void SetMinSize(int width, int height)
    // %rename SetMinSizeSize void SetMinSize(wxSize size)
    %rename SetWindowMinSize void SetItemMinSize(wxWindow* window, int width, int height)
    %rename SetSizerMinSize void SetItemMinSize(wxSizer* sizer, int width, int height)
    void SetItemMinSize(int pos, int width, int height)
    void SetSizeHints(wxWindow* window)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxBoxSizer

%class wxBoxSizer, wxSizer
    wxBoxSizer(int orient)
    void RecalcSizes()
    const wxSize CalcMin()
    int GetOrientation()
    %property=Orientation, read
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxGridSizer

%class wxGridSizer, wxSizer
    wxGridSizer(int cols, int rows, int vgap, int hgap)
    // wxGridSizer(int cols, int vgap = 0, int hgap = 0)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxFlexGridSizer

%class wxFlexGridSizer, wxGridSizer
    wxFlexGridSizer(int rows, int cols, int vgap, int hgap)
    // wxFlexGridSizer(int cols, int vgap = 0, int hgap = 0)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxNotebookSizer

%class wxNotebookSizer, wxSizer
    wxNotebookSizer(wxNotebook* notebook)
    wxNotebook* GetNotebook()
    %property=Notebook, read
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxStaticBoxSizer

%class wxStaticBoxSizer, wxBoxSizer
    wxStaticBoxSizer(wxStaticBox* box, int orient)
    wxStaticBox* GetStaticBox()
    %property=StaticBox, read
%endclass

%endif wxLUA_USE_wxSizer

///////////////////////////////////////////////////////////////////////////////
// wxLayoutConstraints

%if wxLUA_USE_wxLayoutConstraints

%include "wx/layout.h"

%enum wxRelationship
    wxUnconstrained
    wxAsIs
    wxPercentOf
    wxAbove
    wxBelow
    wxLeftOf
    wxRightOf
    wxSameAs
    wxAbsolute
%end

%enum wxEdge
    wxLeft
    wxTop
    wxRight
    wxBottom
    wxWidth
    wxHeight
    wxCentre
    wxCenter
    wxCentreX
    wxCentreY
%end

%class wxLayoutConstraints, wxObject
    wxLayoutConstraints()
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxIndividualLayoutConstraint

%class wxIndividualLayoutConstraint, wxObject
    wxIndividualLayoutConstraint()
    void Above(wxWindow *otherWin, int margin = 0)
    void Absolute(int value)
    void AsIs()
    void Below(wxWindow *otherWin, int margin = 0)
    void Unconstrained()
    void LeftOf(wxWindow *otherWin, int margin = 0)
    void PercentOf(wxWindow *otherWin, wxEdge edge, int per)
    void RightOf(wxWindow *otherWin, int margin = 0)
    void SameAs(wxWindow *otherWin, wxEdge edge, int margin = 0)
    void Set(wxRelationship rel, wxWindow *otherWin, wxEdge otherEdge, int value = 0, int margin = 0)
%endclass

%endif wxLUA_USE_wxLayoutConstraints


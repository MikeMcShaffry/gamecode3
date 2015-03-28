/////////////////////////////////////////////////////////////////////////////
// Name:        wxLuaDebugIO.cpp
// Purpose:     Debugging I/O functions for wxLua
// Author:      J. Winwood
// Created:     May 2002
// Copyright:   (c) 2002 Lomtick Software. All rights reserved.
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "wxLuaDebugIO.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wxLuaDebugIO.h"
#include "wxLuaInternals.h"
#include "wxLuaDebug.h"
#include "wxLuaSocket.h"

// ----------------------------------------------------------------------------
// wxLuaDebugIO
// ----------------------------------------------------------------------------

unsigned char  wxLuaDebugIO::ReadByte(wxLuaSocket *pSocket)
{
    unsigned char value;
    pSocket->Read((char *) &value, sizeof(value));
    return value;
}

short wxLuaDebugIO::ReadWord(wxLuaSocket *pSocket)
{
    short value;
    pSocket->Read((char *)&value, sizeof(value));
    return value;
}

unsigned short wxLuaDebugIO::ReadUWord(wxLuaSocket *pSocket)
{
    unsigned short value;
    pSocket->Read((char *)&value, sizeof(value));
    return value;
}

int wxLuaDebugIO::ReadInt(wxLuaSocket *pSocket)
{
    int value;
    pSocket->Read((char *) &value, sizeof(value));
    return value;
}

unsigned int wxLuaDebugIO::ReadUInt(wxLuaSocket *pSocket)
{
    unsigned int value;
    pSocket->Read((char *) &value, sizeof(value));
    return value;
}

long wxLuaDebugIO::ReadLong(wxLuaSocket *pSocket)
{
    long value;
    pSocket->Read((char *) &value, sizeof(value));
    return value;
}

unsigned long wxLuaDebugIO::ReadULong(wxLuaSocket *pSocket)
{
    unsigned long value;
    pSocket->Read((char *) &value, sizeof(value));
    return value;
}

wxString wxLuaDebugIO::ReadString(wxLuaSocket *pSocket)
{
    wxString value;
    unsigned int length = ReadInt(pSocket);
    if (length > 0)
    {
        char *buffer = new char[length + 1];
        pSocket->Read(buffer, length);
        buffer[length] = 0;
        value = lua2wx(buffer);
        delete[] buffer;
    }
    return value;
}

wxLuaDebugData *wxLuaDebugIO::ReadDebugData(wxLuaSocket *pSocket)
{
    wxLuaDebugData *pSortedList = new wxLuaDebugData();

    try
    {
        size_t idx, idxMax;
        pSocket->Read((char *) &idxMax, sizeof(idxMax));

        for (idx = 0; idx < idxMax; ++idx)
        {
            int bufferLength;
            pSocket->Read((char *) &bufferLength, sizeof(bufferLength));
            if (bufferLength > 0)
            {
                char *pBuffer = new char[bufferLength];
                char *pMemory = pBuffer;
                pSocket->Read(pMemory, bufferLength);
                int nReference = *(int *) pMemory;
                pMemory += sizeof(int);
                int nIndex = *(int *) pMemory;
                pMemory += sizeof(int);
                bool fExpanded = (0 != *pMemory++);

                const char *pNamePtr = pMemory;
                pMemory += strlen(pNamePtr) + 1;
                const char *pTypePtr = pMemory;
                pMemory += strlen(pTypePtr) + 1;
                const char *pValuePtr = pMemory;

                wxLuaDebugDataItem *pItem = new wxLuaDebugDataItem(lua2wx(pNamePtr),
                                                 lua2wx(pTypePtr),
                                                 lua2wx(pValuePtr),
                                                 wxEmptyString, // FIXME! What goes here!?
                                                 nReference,
                                                 nIndex,
                                                 fExpanded);
                pSortedList->Add(pItem);

                delete[] pBuffer;
            }
        }
    }
    catch(wxLuaSocketException & /*e*/)
    {
        delete pSortedList;
        pSortedList = NULL;
    }

    return pSortedList;
}

bool wxLuaDebugIO::WriteByte(wxLuaSocket *pSocket, unsigned char value)
{
    bool result = false;
    try
    {
        pSocket->Write((const char *) &value, sizeof(value));
        result = true;
    }
    catch(wxLuaSocketException & /*e*/)
    {
    }
    return result;
}

bool wxLuaDebugIO::WriteWord(wxLuaSocket *pSocket, short value)
{
    bool result = false;
    try
    {
        pSocket->Write((const char *) &value, sizeof(value));
        result = true;
    }
    catch(wxLuaSocketException & /*e*/)
    {
    }
    return result;
}

bool wxLuaDebugIO::WriteUWord(wxLuaSocket *pSocket, unsigned short value)
{
    bool result = false;
    try
    {
        pSocket->Write((const char *) &value, sizeof(value));
        result = true;
    }
    catch(wxLuaSocketException & /*e*/)
    {
    }
    return result;
}

bool wxLuaDebugIO::WriteInt(wxLuaSocket *pSocket, int value)
{
    bool result = false;
    try
    {
        pSocket->Write((const char *) &value, sizeof(value));
        result = true;
    }
    catch(wxLuaSocketException & /*e*/)
    {
    }
    return result;
}

bool wxLuaDebugIO::WriteUInt(wxLuaSocket *pSocket, unsigned int value)
{
    bool result = false;
    try
    {
        pSocket->Write((const char *) &value, sizeof(value));
        result = true;
    }
    catch(wxLuaSocketException & /*e*/)
    {
    }
    return result;
}

bool wxLuaDebugIO::WriteLong(wxLuaSocket *pSocket, long value)
{
    bool result = false;
    try
    {
        pSocket->Write((const char *) &value, sizeof(value));
        result = true;
    }
    catch(wxLuaSocketException & /*e*/)
    {
    }
    return result;
}

bool wxLuaDebugIO::WriteULong(wxLuaSocket *pSocket, unsigned long value)
{
    bool result = false;
    try
    {
        pSocket->Write((const char *) &value, sizeof(value));
        result = true;
    }
    catch(wxLuaSocketException & /*e*/)
    {
    }
    return result;
}

bool wxLuaDebugIO::WriteString(wxLuaSocket *pSocket, const wxString &value)
{
    bool result = FALSE;
    try
    {
        LuaCharBuffer buf(value);
        int buflen = buf.Length();
        pSocket->Write((const char *) &buflen, sizeof(buflen));
        if (buflen > 0)
        {
            pSocket->Write(buf.GetData(), buflen);
        }
        result = TRUE;
    }
    catch(wxLuaSocketException & /*e*/)
    {
    }
    return result;
}

bool wxLuaDebugIO::WriteDebugData(wxLuaSocket *pSocket, const wxLuaDebugData *pSortedList)
{
    bool result = false;
    size_t idx, idxMax = pSortedList->Count();

    pSocket->Write((const char *) &idxMax, sizeof(idxMax));

    for (idx = 0; idx < idxMax; ++idx)
    {
        try
        {
            const wxLuaDebugDataItem *item = pSortedList->Item(idx);

            int nameLength = item->GetName().Length() + 1;
            int typeLength = item->GetType().Length() + 1;
            int valueLength = item->GetValue().Length() + 1;

            int bufferLength = (2 * sizeof(int)) +
                                sizeof(unsigned char) +
                                nameLength +
                                typeLength +
                                valueLength;

            unsigned char *pBuffer = new unsigned char[bufferLength];
            unsigned char *pMemory = pBuffer;

            pSocket->Write((const char *) &bufferLength, sizeof(bufferLength));

            *(int *) pMemory = item->GetReference();
            pMemory += sizeof(int);

            *(int *) pMemory = item->GetIndex();
            pMemory += sizeof(int);

            *pMemory++ = item->IsExpanded();

            memcpy(pMemory, wx2lua(item->GetName()), nameLength);
            pMemory += nameLength;

            memcpy(pMemory, wx2lua(item->GetType()), typeLength);
            pMemory += typeLength;

            memcpy(pMemory, wx2lua(item->GetValue()), valueLength);

            pSocket->Write((const char *) pBuffer, bufferLength);

            delete[] pBuffer;
            result = true;
        }
        catch(wxLuaSocketException & /*e*/)
        {
        }

        if (result == false)
            break;
    }
    return result;
}

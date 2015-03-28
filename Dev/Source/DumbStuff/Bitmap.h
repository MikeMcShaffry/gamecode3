#pragma once
//========================================================================
// Bitmap.h : Converts 8- 16- or 32-bit bitmap formats to a DirectDraw surface
//
// Part of the GameCode Application
//
// GameCode is the sample application that encapsulates much of the source code
// discussed in "Game Coding Complete" by Mike McShaffry, published by
// Paraglyph Press. ISBN: 1-932111-75-1
//
// If this source code has found it's way to you, and you think it has helped you
// in any way, do the author a favor and buy a new copy of the book - there are 
// detailed explanations in it that compliment this code well. Buy a copy at Amazon.com
// by clicking here: http://www.amazon.com/exec/obidos/ASIN/1932111751/gamecodecompl-20/
//
// There's also a companion web site at http://www.mcshaffry.com/GameCode/portal.php
//
// (c) Copyright 2003 Michael L. McShaffry
//
// This work is licensed under the Creative Commons Attribution-ShareAlike License. 
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/1.0/ 
// or send a letter to:
//      Creative Commons
//      559 Nathan Abbott Way
//      Stanford, California 94305, USA.
//
//========================================================================


#include "..\Graphics-DX7\DDutil.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
#define _555_SOURCES


//////////////////////////////////////////////////////////////////////
// Bitmap
//////////////////////////////////////////////////////////////////////

class Bitmap
{
public:

	// Describes the pixel format of destination for conversion
	enum PixelFormat
	{
		PIXELFORMAT_RGB_16_565,
		PIXELFORMAT_RGB_16_555,
		PIXELFORMAT_RGB_32,
		PIXELFORMAT_16,		// Non-optimal - (non RGB formats)
		PIXELFORMAT_32,		// i.e. BGR...
		PIXELFORMAT_UNKNOWN
	};

	static const char* MakeSingleBitmap(const char* inputdata, int startFrame, int numFrames, int totalFrames, int *datasize );

	// Make surface from bitmap data
	static bool FillSurfaceFromBitmap( const char* pBMPHeader, DDSURFACEDESC2* pDesc, BYTE** pExtra );

	// Retrieves a single frame out of an IPAC file, decoding RLE if necessary,
	// as well as DWORD aligning the new bitmap
	static int GetFrameSize( const char* inputdata, int iFrame );
	static void GetSingleFrame( const char* inputdata, int iFrame, char* destbuffer, int datasize );

	// Get information from bitmap
	static void GetBitmapInfo(const char *inputData, int &width, int &height, int &bpp );
	static void GetBitmapBits(const char *inputData, const char** bits );

	// RLE
	static long GetRLEBitSize(const char* inputdata);

private:

	// Sets the pixel format for conversion purposes
	static PixelFormat GetPixelFormat(LPDDPIXELFORMAT pixelformat);
	
	// 8 bit conversion helper
	static bool GeneratePaletteConversionTable( DDSURFACEDESC2* pDesc, const char* bitmap, BYTE** table );
	
	// RLE
	static void DecodeRLEBits(const char* inputdata, char* outputdata);
	
	// Standard copy, no conversion needed
	static bool CopyBitmapBits( const char* pBMPHeader, DDSURFACEDESC2* pDesc );
	
public:

	// =============
	// 8 bit sources
	// =============
	
	static bool ConvertBitmap_8( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight, BYTE* pExtra );

	// ==============
	// 16 bit sources
	// ==============

#ifdef _555_SOURCES
	// 555 sources
	static bool ConvertBitmap_16_555_16_555_RGB( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight );
	static bool ConvertBitmap_16_555_16_565_RGB( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight );
	static bool ConvertBitmap_16_555_32_RGB( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight );
#else
	// 565 sources
	static bool ConvertBitmap_16_565_16_555_RGB( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight );
	static bool ConvertBitmap_16_565_16_565_RGB( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight );
	static bool ConvertBitmap_16_565_32_RGB( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight );
#endif

	// ==============
	// 24 bit sources
	// ==============
	
	static bool ConvertBitmap_24_16_555_RGB( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight );
	static bool ConvertBitmap_24_16_565_RGB( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight );
	static bool ConvertBitmap_24_32_RGB( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight );

	// ==============
	// 32 bit sources
	// ==============
	
	static bool ConvertBitmap_32_16_555_RGB( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight );
	static bool ConvertBitmap_32_16_565_RGB( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight );
	static bool ConvertBitmap_32_32_RGB( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight );

	// ========================
	// Generic - 16 bit sources
	// ========================

#ifdef _555_SOURCES
	// 555 sources
	static bool ConvertBitmap_16_555_16_XXX( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight );
	static bool ConvertBitmap_16_555_32_XXX( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight );
#else
	// 565 sources
	static bool ConvertBitmap_16_565_16_XXX( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight );
	static bool ConvertBitmap_16_565_32_XXX( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight );
#endif

	// ========================
	// Generic - 24 bit sources
	// ========================

	static bool ConvertBitmap_24_16_XXX( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight );
	static bool ConvertBitmap_24_32_XXX( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight );

	// ========================
	// Generic - 32 bit sources
	// ========================

	static bool ConvertBitmap_32_16_XXX( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight );
	static bool ConvertBitmap_32_32_XXX( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight );
};


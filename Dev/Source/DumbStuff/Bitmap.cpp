//========================================================================
// Bitmap.cpp : Converts 8- 16- or 32-bit bitmap formats to a DirectDraw surface
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


//========================================================================
// Bitmap Utilities
//
// This code isn't discussed anywhere in the book, but you use it to 
// take classic BMP files and copy/convert their bits to a direct draw
// surface.
//
//========================================================================

#include "stdafx.h"
#include "Bitmap.h"


//-----------------------------------------------------------------------------
// Name: Bitmap::GetRLEBitSize()
// Desc: Get the size of a decoded RLE bitmap
//-----------------------------------------------------------------------------
long Bitmap::GetRLEBitSize(const char* inputdata)
{
	BITMAPFILEHEADER *bmf=(BITMAPFILEHEADER *)inputdata;
	BITMAPINFOHEADER *bmpInfoHdr=(BITMAPINFOHEADER *)(&bmf[1]);

	int width = bmpInfoHdr->biWidth;
	int height = bmpInfoHdr->biHeight;

	//
	// Manually decode RLE encoded 8 bit bitmaps.
	//

	//
	// Account for DWORD aligned scan lines
	//

	int offset=4-(width%4);
	if(offset==4)
	{
		offset=0;
	}

	//
	// Hande right side up bitmaps
	//

	int lineCount=height;
	if(lineCount<0)
	{
		lineCount=-lineCount;
	}

	//
	// Decode as follows:
	// If the first byte isn't 0, there's a run of that many bytes that
	// have a value of the next byte.
	//
	// If the first byte is 0, read the next byte.
	// If that byte is 0, it's the end of a scan line, advance to the next
	// scan line.
	// If that byte is 1, it's the end of the bitmap.
	// If that byte is 2, it's a forward jump into the bitmap. The next byte
	// is the horizontal jump, the byte after that is the vertial jump.
	//
	// If that byte is 3 or more is a list of literal byte values.
	// Copy that many bytes to the new scan line. These literal bytes
	// sections are word aligned so if the list ends mid-word, it's
	// padded with a 0.
	//

	return lineCount*(width+offset);
}

//-----------------------------------------------------------------------------
// Name: Bitmap::DecodeRLEBits()
// Desc: Decodes a RLE bitmap
//-----------------------------------------------------------------------------
void Bitmap::DecodeRLEBits(const char* inputdata, char* outputdata)
{
	BITMAPFILEHEADER *bmf=(BITMAPFILEHEADER *)inputdata;

	BITMAPINFOHEADER *bmpInfoHdr=(BITMAPINFOHEADER *)(&bmf[1]);
	BITMAPINFO *bitmapInfo=(BITMAPINFO *)bmpInfoHdr;

	const char *bitmapBits=((const char *)bmf)+bmf->bfOffBits;

	int width = bmpInfoHdr->biWidth;
	int height = bmpInfoHdr->biHeight;

	//
	// Manually decode RLE encoded 8 bit bitmaps.
	//

	//
	// Account for DWORD aligned scan lines
	//

	int offset=4-(width % 4);
	if(offset==4)
	{
		offset=0;
	}

	//
	// Hande right side up bitmaps
	//

	int lineCount=height;
	if(lineCount<0)
	{
		lineCount=-lineCount;
	}

	//
	// Decode as follows:
	// If the first byte isn't 0, there's a run of that many bytes that
	// have a value of the next byte.
	//
	// If the first byte is 0, read the next byte.
	// If that byte is 0, it's the end of a scan line, advance to the next
	// scan line.
	// If that byte is 1, it's the end of the bitmap.
	// If that byte is 2, it's a forward jump into the bitmap. The next byte
	// is the horizontal jump, the byte after that is the vertial jump.
	//
	// If that byte is 3 or more is a list of literal byte values.
	// Copy that many bytes to the new scan line. These literal bytes
	// sections are word aligned so if the list ends mid-word, it's
	// padded with a 0.
	//

	long size=lineCount*(width+offset);
	char *newBitmapBits=outputdata;
	const char *source=bitmapBits;
	char *dest=newBitmapBits;
	char *lineStart=dest;
	while(lineCount>0)
	{
		char code=*source++;
		if(code!=0)
		{
			const char value=*source++;
			
			/*for(int f=0;f<code;f++)
			{
				*dest++=value;
			}*/
			//4 bytes at a time!
			memset(dest, value, code);
			dest += code;
		}
		else
		{
			code=*source++;

			if (code==1)
			{
				break;
			}

			switch(code)
			{
			case 0:
				lineStart+=width+offset;
				dest=lineStart;
				lineCount--;
				break;
			case 1:
				break;

			case 2:
				{
					BYTE horz=*source++;
					BYTE vert=*source++;

					if(vert!=0)
					{
						int offset=(int)(dest-lineStart);
						lineStart+=(width+offset)*vert;
						dest=lineStart+offset;
						lineCount-=vert;
					}
					dest+=horz;
				}
				break;
			default:
				{
					/*for(int f=0;f<code;f++)
					{
						*dest++=*source++;
					}*/
					//4 bytes at a time!
					memcpy(dest, source, code);
					dest += code;
					source += code;

					if(code&1)
					{
						source++;
					}
				}
				break;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Name: Bitmap::GetFrameSize()
// Desc: Retrieves the size needed for a buffer to hold a particular frame.
//       Remember that multiple frames of animation are stored in a bitmap
//       like a filmstrip. Since all the bitmaps for the animation are exactly 
//       the same, only one BITMAPINFOHEADER is stored, and some offsets to the
//       beginning of the bitmaps are stored in a lookup table at the beginning
//       of the inputdata.
//
//       YES...this is non-standard. It's stored this way because there really 
//       isn't a standard method to store multi-frame bitmaps.
//
//       Incidentally, this is how IPAC stores graphic data.
//
//-----------------------------------------------------------------------------
int Bitmap::GetFrameSize( const char* inputdata, int iFrame )
{
	int datasize( 0 );

	// MLM 5-19-2001 This needed some defensive coding - found a weakness here
	// during a bug fix for #821
	assert(iFrame>=0 && _T("Frames must be > 0"));
	if (iFrame<0)
		return NULL;
	// MLM 5-19-2001 End
	
	// The image is at the start of the buffer + the frame offset
	int* pOffset = (int*) inputdata;
	const char* pImage = inputdata + pOffset[iFrame];

	BITMAPFILEHEADER* bmf=(BITMAPFILEHEADER *) pImage;
	BITMAPINFOHEADER* bmpInfoHdr=(BITMAPINFOHEADER *)(&bmf[1]);
	BITMAPINFO* bitmapInfo=(BITMAPINFO *)bmpInfoHdr;
	
	int headersize = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER );
	
	datasize = headersize;
	
	long frameSize;
	long pitch;
	
	bool _8bit = bmpInfoHdr->biBitCount == 8;
	bool _RLE = _8bit && bmpInfoHdr->biCompression==BI_RLE8;
	
	// Calc size needed
	if ( _RLE )
	{
		frameSize = GetRLEBitSize( pImage );
	}
	else
	{
		pitch = ((((bmpInfoHdr->biWidth * bmpInfoHdr->biBitCount)+31)&~31)>>3);
		frameSize = pitch*bmpInfoHdr->biHeight;
	}
	datasize += frameSize;
	
	// 8 bit has RGB quad data
	if ( _8bit )
		datasize += sizeof( RGBQUAD ) * 256;

	return datasize;
}

//-----------------------------------------------------------------------------
// Name: Bitmap::GetSingleFrame()
// Desc: Retrieves a single out of an IPAC file, decompressing it if necessary
//		 into the destination buffer.
//
// See comments for GetFrameSize to find out what the hell this does.
// It is NOT normal for bitmaps to store multiple frames, and this routine reads
// an extension of the BMP file format that is non-standard.
//-----------------------------------------------------------------------------
void Bitmap::GetSingleFrame( const char* inputdata, int iFrame, char* destbuffer, int datasize )
{
	// MLM 5-19-2001 This needed some defensive coding - found a weakness here
	// during a bug fix for #821
	assert(iFrame>=0 && _T("Frames must be > 0"));
	if (iFrame<0)
		return;
	// MLM 5-19-2001 End
	
	// The image is at the start of the buffer + the frame offset
	int* pOffset = (int*) inputdata;
	const char* pImage = inputdata + pOffset[iFrame];

	BITMAPFILEHEADER* bmf=(BITMAPFILEHEADER *) pImage;
	BITMAPINFOHEADER* bmpInfoHdr=(BITMAPINFOHEADER *)(&bmf[1]);
	BITMAPINFO* bitmapInfo=(BITMAPINFO *)bmpInfoHdr;
	
	/*
	int headersize = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER );
	
	*datasize = headersize;
	
	long frameSize;
	long pitch;
	*/

	bool _8bit = bmpInfoHdr->biBitCount == 8;
	bool _RLE = _8bit && bmpInfoHdr->biCompression==BI_RLE8;
	
	/*
	// Calc size needed
	if ( _RLE )
	{
		frameSize = GetRLEBitSize( pImage );
	}
	else
	{
		pitch = ((((bmpInfoHdr->biWidth * bmpInfoHdr->biBitCount)+31)&~31)>>3);
		frameSize = pitch*bmpInfoHdr->biHeight;
	}
	*datasize += frameSize;
	
	// 8 bit has RGB quad data
	if ( _8bit )
		*datasize += sizeof( RGBQUAD ) * 256;
	*/

	// Alloc new block
	char* outputdata = destbuffer;
	char* pNewBits;
	
	// Copy the first frame's file header
	memcpy( outputdata, pImage, bmf->bfOffBits );
	
	// Set new header info
	BITMAPFILEHEADER* _bmf=(BITMAPFILEHEADER *) outputdata;
	BITMAPINFOHEADER* _bmpInfoHdr=(BITMAPINFOHEADER *)(&_bmf[1]);
	
	// Now decoded, no RLE decoding is needed
	if ( _RLE )
		_bmpInfoHdr->biCompression = BI_RGB;

	// Location for destination bits
	pNewBits = outputdata + bmf->bfOffBits;
		
	if ( _RLE )
	{
		DecodeRLEBits( pImage, pNewBits );
	}
	else
	{
		const char* pImageBits = pImage + bmf->bfOffBits;
		memcpy( pNewBits, pImageBits, bmpInfoHdr->biSizeImage );
	}
}


//-----------------------------------------------------------------------------
// Name: Bitmap::CopyBitmapBits()
// Desc: Copies the bits bits to the destination surface
//-----------------------------------------------------------------------------
bool Bitmap::CopyBitmapBits( const char* pBMPHeader, DDSURFACEDESC2* pDesc )
{
	const char* pBitmap = NULL;
	BYTE* pSurface = (BYTE*) pDesc->lpSurface;
	
	// Get info
	int width, height, bpp;
	Bitmap::GetBitmapInfo( pBMPHeader, width, height, bpp );
	Bitmap::GetBitmapBits( pBMPHeader, &pBitmap );

	// Pitches
	long  surface_pitch = pDesc->lPitch;
	long  bmp_pitch=((((width*bpp)+31)&~31)>>3);
	
	// Set index
	const char* pBits = pBitmap + ( (height-1) * bmp_pitch );
	
	// Copy
	for ( int i = 0; i < height; i++ )
	{
		memcpy( pSurface, pBits, bmp_pitch );

		// Move pointers to next line
		pBits -= bmp_pitch;
		pSurface += surface_pitch;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Name: Bitmap::FillSurfaceFromBitmap()
// Desc: Make surface from bitmap data
//-----------------------------------------------------------------------------
bool Bitmap::FillSurfaceFromBitmap( const char* pBMPHeader, DDSURFACEDESC2* pDesc, BYTE** pExtra )
{
	// Bitmap info
	const char* pBits = NULL;
	Bitmap::GetBitmapBits( pBMPHeader, &pBits );
	int	iBitmapWidth, iBitmapHeight, iBitmapDepth;
	Bitmap::GetBitmapInfo( pBMPHeader, iBitmapWidth, iBitmapHeight, iBitmapDepth );
	
	// Pitch
	int iSurfacePitch = (int) pDesc->lPitch;

	// Set the pixel format
	LPDDPIXELFORMAT ddpixelformat = &(pDesc->ddpfPixelFormat);
	PixelFormat pixelformat = GetPixelFormat( ddpixelformat );

	// Do the conversion
	switch( iBitmapDepth )
	{
	case 8 :
		// =============
		// 8 bit sources
		// =============
		
		// Lookup table for conversion, if necessary
		if ( (*pExtra) == NULL )
		{
			Bitmap::GeneratePaletteConversionTable( pDesc, pBMPHeader, pExtra );
			//TRACE( _T("Loading color table for resource\n" ) );
		}

		return ConvertBitmap_8( pDesc, pBits, iBitmapWidth, iBitmapHeight, (*pExtra) );
		
	case 16 :

#ifdef _555_SOURCES
		// ==============
		// 16 bit sources
		// ==============
		
		switch( pixelformat )
		{
		case PIXELFORMAT_RGB_16_555 :
			// 16 bit to 16 bit (555) - copy directly
			return CopyBitmapBits( pBMPHeader, pDesc );
			
		case PIXELFORMAT_RGB_16_565: // (tested)
			// 16 bit to 16 bit (565)
			return ConvertBitmap_16_555_16_565_RGB( pDesc, pBits, iBitmapWidth, iBitmapHeight );

		case PIXELFORMAT_RGB_32: // (tested)
			// 16 bit to 32 bit
			return ConvertBitmap_16_555_32_RGB( pDesc, pBits, iBitmapWidth, iBitmapHeight );

		case PIXELFORMAT_16:
			// 16 bit to any bit type
			return ConvertBitmap_16_555_16_XXX( pDesc, pBits, iBitmapWidth, iBitmapHeight );

		case PIXELFORMAT_32: // (tested)
			// 16 bit to any 32 bit type
			return ConvertBitmap_16_555_32_XXX( pDesc, pBits, iBitmapWidth, iBitmapHeight );
			
		default :
			assert( 0 && _T("Mode is not handled.") );
			return false;
		}
		break;
#else
		// ==============
		// 16 bit sources
		// ==============
		switch( pixelformat )
		{
		case PIXELFORMAT_RGB_16_565:
			// 16 bit to 16 bit (565) - copy directly
			return CopyBitmapBits( pBMPHeader, pDesc );

		case PIXELFORMAT_RGB_16_555 :
			// 16 bit to 16 bit (555)
			return ConvertBitmap_16_565_16_555_RGB( pDesc, pBits, iBitmapWidth, iBitmapHeight );
						
		case PIXELFORMAT_RGB_32: // (tested)
			// 16 bit to 32 bit
			return ConvertBitmap_16_565_32_RGB( pDesc, pBits, iBitmapWidth, iBitmapHeight );

		case PIXELFORMAT_16:
			// 16 bit to any bit type
			return ConvertBitmap_16_565_16_XXX( pDesc, pBits, iBitmapWidth, iBitmapHeight );

		case PIXELFORMAT_32: // (tested)
			// 16 bit to any 32 bit type
			return ConvertBitmap_16_565_32_XXX( pDesc, pBits, iBitmapWidth, iBitmapHeight );
			
		default :
			assert( 0 && _T("Mode is not handled.") );
			return false;
		}
		break;
#endif

	case 24:
		// ==============
		// 24 bit sources
		// ==============

		switch( pixelformat )
		{
		case PIXELFORMAT_RGB_16_555 :
			// 24 bit to 16 bit (555)
			return ConvertBitmap_24_16_555_RGB( pDesc, pBits, iBitmapWidth, iBitmapHeight );
		
		case PIXELFORMAT_RGB_16_565 :
			// 24 bit to 16 bit (565)
			return ConvertBitmap_24_16_565_RGB( pDesc, pBits, iBitmapWidth, iBitmapHeight );
			
		case PIXELFORMAT_RGB_32 :
			// 24 bit to 32 bit
			return ConvertBitmap_24_32_RGB( pDesc, pBits, iBitmapWidth, iBitmapHeight );
		
		case PIXELFORMAT_16 :
			// 24 bit to any 16 type
			return ConvertBitmap_24_16_XXX( pDesc, pBits, iBitmapWidth, iBitmapHeight );

		case PIXELFORMAT_32 : 
			// 24 bit to any 24 type
			return ConvertBitmap_24_32_XXX( pDesc, pBits, iBitmapWidth, iBitmapHeight );

		default : 
			assert( 0 && _T("Mode is not handled.") );
			return false;	
		}
	default :
		assert( 0 && _T("Mode is not handled.") );
		return false;
	}
	
	// Create bits as needed
	return true;	
}


//-----------------------------------------------------------------------------
// Name: Bitmap() 
// Desc: Gets the bitmap info
//-----------------------------------------------------------------------------
void Bitmap::GetBitmapInfo(const char *inputData, int &width, int &height, int &bpp )
{
	BITMAPFILEHEADER* bmf=(BITMAPFILEHEADER *) inputData;
	BITMAPINFOHEADER* bmpInfoHdr=(BITMAPINFOHEADER *)(&bmf[1]);
	//BYTE* pBits = inputData + bmf->bfOffBits;
	
	// Size
	width = bmpInfoHdr->biWidth;
	height = bmpInfoHdr->biHeight;
	bpp = bmpInfoHdr->biBitCount;
}

//-----------------------------------------------------------------------------
// Name: Bitmap() 
// Desc: Gets the bitmap bits
//-----------------------------------------------------------------------------
void Bitmap::GetBitmapBits(const char *inputData, const char** bits)
{
	BITMAPFILEHEADER* bmf=(BITMAPFILEHEADER *) inputData;
	BITMAPINFOHEADER* bmpInfoHdr=(BITMAPINFOHEADER *)(&bmf[1]);
	(*bits) = inputData + bmf->bfOffBits;
}


//-----------------------------------------------------------------------------
// Name: Bitmap() 
// Desc: Gets the pixel format from the surface
//-----------------------------------------------------------------------------
Bitmap::PixelFormat Bitmap::GetPixelFormat( LPDDPIXELFORMAT ddpixelformat )
{
	// Save the pixel format
	switch( ddpixelformat->dwRGBBitCount )
	{
	case 16 : 
		// 16 bit - 555 or 565
		if ( ddpixelformat->dwRBitMask == 0x7c00 &&  // 555 RGB
			 ddpixelformat->dwGBitMask == 0x03e0 && 
			 ddpixelformat->dwBBitMask == 0x1f )
		{
			return PIXELFORMAT_RGB_16_555;
		}
		else if ( ddpixelformat->dwRBitMask == 0xf800 && 
			ddpixelformat->dwGBitMask == 0x07e0 && 
			ddpixelformat->dwBBitMask == 0x1f ) // 565 RGB
		{
			return PIXELFORMAT_RGB_16_565;
		}
		
		//default
		return PIXELFORMAT_16;

	case 32 : 
		
		// 32 bit - 888
		if ( ddpixelformat->dwRBitMask == 0xff0000 &&  // 888 RGB
			ddpixelformat->dwGBitMask == 0x00ff00 && 
			ddpixelformat->dwBBitMask == 0x0000ff )
		{
			return PIXELFORMAT_RGB_32;
		}
		
		// default
		return PIXELFORMAT_32; 

	default : 
		assert( 0 && "Invalid pixel format for surface (8,16,32 bit only)\n" );
		break;
	}

	return PIXELFORMAT_UNKNOWN;
}

//-----------------------------------------------------------------------------
// Name: Bitmap() 
// Desc: Generates the bitmap lookup table to convert bitmap palette indexes
//		 to real color values native to the color mode of the surface.
//		 Use the failsafe way, slow conversion, for all formats possible
//		 looking at the mask of the pixel formats, num bits, etc...
//-----------------------------------------------------------------------------
bool Bitmap::GeneratePaletteConversionTable( DDSURFACEDESC2* pDesc, const char* bitmap, BYTE** table )
{
	BITMAPFILEHEADER* bmf=(BITMAPFILEHEADER *) bitmap;
	BITMAPINFOHEADER* bmpInfoHdr=(BITMAPINFOHEADER *)(&bmf[1]);

	// Is this a valid 8 bit bitmap?  If not, bail
	if ( bmpInfoHdr->biBitCount != 8 )
		return false;

	// Get the colors
	RGBQUAD* colors =(RGBQUAD *)(((BYTE*)bmpInfoHdr)+bmpInfoHdr->biSize);

	// Declare conversion table
	int iBpp = pDesc->ddpfPixelFormat.dwRGBBitCount;
	int new_size = ( iBpp >> 3 ) << 8;
	BYTE* lookuptable = new BYTE[ new_size ];
		
	// Amount to shift
	int iRShift = 0, iBShift = 0, iGShift = 0;
	int i;
	
	// Convert the colors
	switch( iBpp )
	{
	case 16 : // 16 bit conversion
		{
			int iRBits = 0, iBBits = 0, iGBits = 0;
			WORD temp;

			// Find amount to shift for red
			for ( i = 0, temp = 1; i < iBpp; i++ )
			{
				if ( temp & pDesc->ddpfPixelFormat.dwRBitMask ) 
				{ iRShift = i; break; }
				temp <<= 1;
			}
			for ( ; i < iBpp; i++ )
			{
				if ( ! ( temp & pDesc->ddpfPixelFormat.dwRBitMask ) )
					break;
				iRBits++;
				temp <<= 1;
			}
			
			// Find amount to shift for green
			for ( i = 0, temp = 1; i < iBpp; i++ )
			{
				if ( temp & pDesc->ddpfPixelFormat.dwGBitMask ) 
				{ iGShift = i; break; }
				temp <<= 1;
			}
			for ( ; i < iBpp; i++ )
			{
				if ( ! ( temp & pDesc->ddpfPixelFormat.dwGBitMask ) )
					break;
				iGBits++;
				temp <<= 1;
			}
			
			// Find amount to shift for blue
			for ( i = 0, temp = 1; i < iBpp; i++ )
			{
				if ( temp & pDesc->ddpfPixelFormat.dwBBitMask ) 
				{ iBShift = i; break; }
				temp <<= 1;
			}
			for ( ; i < iBpp; i++ )
			{
				if ( ! ( temp & pDesc->ddpfPixelFormat.dwBBitMask ) )
					break;
				iBBits++;
				temp <<= 1;
			}
			
			// Convert!
			WORD* index = (WORD*) lookuptable;
			for ( int i = 0; i < 256; i++ )
			{
				index[ i ] = 
					( colors[ i ].rgbRed   >> ( 8 - iRBits ) << iRShift ) | // RED
					( colors[ i ].rgbGreen >> ( 8 - iGBits ) << iGShift ) | // GREEN
					( colors[ i ].rgbBlue  >> ( 8 - iBBits ) << iBShift ); // BLUE
			}
		}
		break;

	case 32 : // 32 bit conversion
		{
			DWORD temp;
			
			// Find amount to shift for red
			for ( i = 0, temp = 1; i < iBpp; i++ )
			{
				if ( temp & (DWORD) pDesc->ddpfPixelFormat.dwRBitMask ) 
				{ iRShift = i; break; }
				temp <<= 1;
			}
			
			// Find amount to shift for green
			for ( i = 0, temp = 1; i < iBpp; i++ )
			{
				if ( temp & (DWORD) pDesc->ddpfPixelFormat.dwGBitMask ) 
				{ iGShift = i; break; }
				temp <<= 1;
			}
			
			// Find amount to shift for blue
			for ( i = 0, temp = 1; i < iBpp; i++ )
			{
				if ( temp & (DWORD) pDesc->ddpfPixelFormat.dwBBitMask ) 
				{ iBShift = i; break; }
				temp <<= 1;
			}

			// Convert
			DWORD* index = (DWORD*) lookuptable;
			for ( int i = 0; i < 256; i++ )
			{
				index[ i ] = 
					( colors[ i ].rgbRed   << iRShift ) | // RED
					( colors[ i ].rgbGreen << iGShift ) | // GREEN
					( colors[ i ].rgbBlue  << iBShift ); // BLUE
			}
		}
		break;

	default : 
		assert( 0 && "Couldn't build palette, surface doesn't contain valid bpp info" );
		break;
	}

	// Return data
	(*table) = lookuptable;

	return true;
}

// =============
// 8 bit sources
// =============

bool Bitmap::ConvertBitmap_8( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight, BYTE* pLookup )
{
	// 8 bit to 16 bit, using the color translation table
	BYTE* pBitmap = (BYTE*) bitmapBits;
	BYTE* pSurface = (BYTE*) pDesc->lpSurface;
	assert( iBitmapHeight > 0  && "Bottom-up bitmap expected" );
	
	// Pitches
	long bmp_pitch=((((iBitmapWidth*8)+31)&~31)>>3);
	long surface_pitch = pDesc->lPitch;
	
	// Adjust for upside-down, start at last line of bitmap
	pBitmap += bmp_pitch * ( iBitmapHeight - 1 );
	
	// Temp pointers for easy color reading
	BYTE* pSource = (BYTE*) pBitmap;

	// 16 bit conversion
	switch( pDesc->ddpfPixelFormat.dwRGBBitCount )
	{
	case 16 :
		{
			WORD* pDest = (WORD*) pSurface;
			WORD* pTable = (WORD*) pLookup;

			for ( int i = 0; i < iBitmapHeight; i++ )
			{
				for ( int j = 0; j < iBitmapWidth; j++ )
				{
					pDest[ j ] = pTable[ pBitmap[ j ] ];
				}
				
				// Move bitmap pointer to next line
				pBitmap -= bmp_pitch;
				
				// Move surface pointer to next line
				pSurface += surface_pitch;
				pDest = (WORD*) pSurface;
			}
		}
		break;

	case 32 :
		{
			DWORD* pDest = (DWORD*) pSurface;
			DWORD* pTable = (DWORD*) pLookup;
			
			for ( int i = 0; i < iBitmapHeight; i++ )
			{
				for ( int j = 0; j < iBitmapWidth; j++ )
				{
					pDest[ j ] = pTable[ pSource[ j ] ];
				}
				
				// Move bitmap pointer to next line
				pSource -= bmp_pitch;
				
				// Move surface pointer to next line
				pSurface += surface_pitch;
				pDest = (DWORD*) pSurface;
			}
			break;
		}

	default : // failure
		assert( 0 && "You should never get here" );
		break;
	}

	return true;
}

// ==============
// 16 bit sources
// ==============

#ifdef _555_SOURCES

bool Bitmap::ConvertBitmap_16_555_16_555_RGB( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight )
{
	// 16 bit to 16 bit (555) - copy directly
	return false;
}

bool Bitmap::ConvertBitmap_16_555_16_565_RGB( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight )
{
	// 16 bit 555 to 16 bit 565 RGB
	BYTE* pBitmap = (BYTE*) bitmapBits;
	BYTE* pSurface = (BYTE*) pDesc->lpSurface;
	assert( iBitmapHeight > 0  && "Bottom-up bitmap expected" );

	// Pitches
	long bmp_pitch=((((iBitmapWidth*16)+31)&~31)>>3);
	long surface_pitch = pDesc->lPitch;

	// Adjust for upside-down, start at last line of bitmap
	pBitmap += bmp_pitch * ( iBitmapHeight - 1 );
		
	// Temp pointers for easy color reading
	WORD* pSource = (WORD*) pBitmap;
	WORD* pDest = (WORD*) pSurface;
	
	/*
	========================================
	RGB 555 to RGB 565
	========================================
	RED		5 to 5 bits, << 1
	GREEN	5 to 6 bits, << 1
	BLUE	5 to 5 bits,
	========================================
	*/

	const int ndoubles = iBitmapWidth >> 1;
	const int nsingles = iBitmapWidth % ndoubles;

	for ( int i = 0; i < iBitmapHeight; i++ )
	{
		DWORD * pDWordDest = (DWORD *)pDest;
		DWORD * pDWordSrc = (DWORD *)pSource;
		DWORD * pDestEnd = pDWordDest + ndoubles;

		while (pDWordDest < pDestEnd)
		{
			*pDWordDest = 
				( ( *pDWordSrc & 0x7fe07fe0 ) << 1 ) | 
				( *pDWordSrc & 0x001f001f );

			pDWordDest++;
			pDWordSrc++;
		}

		if ( nsingles )
		{
			WORD* pWordDest = (WORD*) pDWordDest;
			WORD* pWordSrc = (WORD*) pDWordSrc;
			*pWordDest = 
				( ( *pWordSrc & 0x7fe0 ) << 1 ) | 
				( *pWordSrc & 0x001f );
		}

		/* Original
		
		//Must be WORD aligned...
		for (int j = 0; j < iBitmapWidth; j++)
		{
			pDest[ j ] = 
				( ( pSource[ j ] & 0x7fe0 ) << 1 ) | 
				( pSource[ j ] & 0x1f );
		}
		
		*/

		// Move bitmap pointer to next line
		pBitmap -= bmp_pitch;
		pSource = (WORD*) pBitmap;
		
		// Move surface pointer to next line
		pSurface += surface_pitch;
		pDest = (WORD*) pSurface;
	}

	return true;
}

bool Bitmap::ConvertBitmap_16_555_32_RGB( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight )
{
	// 16 bit 555 to 32 bit RGB
	BYTE* pBitmap = (BYTE*) bitmapBits;
	BYTE* pSurface = (BYTE*) pDesc->lpSurface;
	assert( iBitmapHeight > 0  && "Bottom-up bitmap expected" );

	// Pitches
	long bmp_pitch=((((iBitmapWidth*16)+31)&~31)>>3);
	long surface_pitch = pDesc->lPitch;

	// Adjust for upside-down, start at last line of bitmap
	pBitmap += bmp_pitch * ( iBitmapHeight - 1 );
		
	// Temp pointers for easy color reading
	WORD* pSource = (WORD*) pBitmap;
	DWORD* pDest = (DWORD*) pSurface;

	/*
	========================================
	RGB 555 to RGB 0888
	========================================
	RED		5 to 8 bits, << 16 + 3 - 10
	GREEN	5 to 8 bits, << 8 + 3 - 5
	BLUE	5 to 8 bits, << 3

	This leaves three empty bits that need
	to be filled with something. We will
	fill those with the three most
	significant bits of the 5 bit value.
	Added 4-4-02 -JES
	
	So for those 3 bits:
	RED		<< 16 - 2 - 10
	GREEN	<< 8  - 2 - 5
	BLUE	<< 0  - 2 - 0
	========================================
	*/
	DWORD color, temp;

	for ( int i = 0; i < iBitmapHeight; ++i )
	{
		for ( int j = 0; j < iBitmapWidth; ++j )
		{
			color = pSource[ j ];
			
			/* Original
			temp = (color & 0x7c00) << 9;	//Set high blue bits from all blue bits...
			temp |= (color & 0x7000) << 4;	//Set low blue bits from high blue bits...

			temp |= ( color & 0x3e0 ) << 6;	//Set high green bits from all green bits...
			temp |= ( color & 0x380 ) << 1;	//Set low green bits from high green bits...

			temp |= ( color & 0x1f ) << 3;	//Set high red bits from all red bits...
			temp |= ( color & 0x1c ) >> 2;	//Set low red bits from high red bits...

			/*Alternate #1...
			temp = (color & 0x7c00) << 9;	//Set high blue bits from all blue bits...
			temp |= (color & 0x1FE0) << 5;	//Set low blue bits from low blue bits,
											//Set high green bits from all green bits...
			temp |= (color & 0x00FF) << 3;	//Set low green bits from low green bits,
											//Set high red bits from all red bits...
			temp |= (color & 0x0007);		//Set low red bits from low red bits.
			*/

			/*Alternate #2...*/
			temp = ((color & 0x7c00) << 9)	//Set blue bits...
				|  ((color & 0x03E0) << 6)	//Set green bits...
				|  ((color & 0x001F) << 3);	//Set red bits...

			//Replicate the high bits into the low bits...
			temp |= (temp & 0x00E0E0E0) >> 5;	//Set the low bits of each color from
												//the high bits.
			

			pDest[ j ] = temp;
		}
		
		// Move bitmap pointer to next line
		pBitmap -= bmp_pitch;
		pSource = (WORD*) pBitmap;
		
		// Move surface pointer to next line
		pSurface += surface_pitch;
		pDest = (DWORD*) pSurface;
	}
	return true;
}

// Non-optimal

bool Bitmap::ConvertBitmap_16_555_16_XXX( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight )
{
	// 16 bit 555 to 16 bit 565 RGB
	BYTE* pBitmap = (BYTE*) bitmapBits;
	BYTE* pSurface = (BYTE*) pDesc->lpSurface;
	assert( iBitmapHeight > 0  && "Bottom-up bitmap expected" );

	// Pitches
	long bmp_pitch=((((iBitmapWidth*16)+31)&~31)>>3);
	long surface_pitch = pDesc->lPitch;

	// Adjust for upside-down, start at last line of bitmap
	pBitmap += bmp_pitch * ( iBitmapHeight - 1 );
		
	// Temp pointers for easy color reading
	WORD* pSource = (WORD*) pBitmap;
	WORD* pDest = (WORD*) pSurface;
	WORD temp;

	// Amount to shift
	int iRShift = 0, iBShift = 0, iGShift = 0;
	int iRBits = 0, iBBits = 0, iGBits = 0;
	int i;
	
	// Find amount to shift for red
	for ( i = 0, temp = 1; i < 16; i++ )
	{
		if ( temp & pDesc->ddpfPixelFormat.dwRBitMask ) 
		{ iRShift = i; break; }
		temp <<= 1;
	}
	for ( ; i < 16; i++ )
	{
		if ( ! ( temp & pDesc->ddpfPixelFormat.dwRBitMask ) )
			break;
		iRBits++;
		temp <<= 1;
	}
	
	// Find amount to shift for green
	for ( i = 0, temp = 1; i < 16; i++ )
	{
		if ( temp & pDesc->ddpfPixelFormat.dwGBitMask ) 
		{ iGShift = i; break; }
		temp <<= 1;
	}
	for ( ; i < 16; i++ )
	{
		if ( ! ( temp & pDesc->ddpfPixelFormat.dwGBitMask ) )
			break;
		iGBits++;
		temp <<= 1;
	}
	
	// Find amount to shift for blue
	for ( i = 0, temp = 1; i < 16; i++ )
	{
		if ( temp & pDesc->ddpfPixelFormat.dwBBitMask ) 
		{ iBShift = i; break; }
		temp <<= 1;
	}
	for ( ; i < 16; i++ )
	{
		if ( ! ( temp & pDesc->ddpfPixelFormat.dwBBitMask ) )
			break;
		iBBits++;
		temp <<= 1;
	}
	
	/*
	=============================================
	RGB 555 to xxx xxx
	=============================================
	RED		5 to R bits, >> 10 << R shift
	GREEN	5 to G bits, >> 5 << G shift
	BLUE	5 to B bits, >> 0 << B shift
	=============================================
	*/
	
	// Account for converting from 5 to 6 bits
	iRShift += iRBits - 5;
	iGShift += iGBits - 5;
	iBShift += iBBits - 5;

	for ( i = 0; i < iBitmapHeight; i++ )
	{
		for ( int j = 0; j < iBitmapWidth; j++ )
		{
			pDest[ j ] =
				( ( ( pSource[ j ] & 0x7c00 ) >> 10 ) << iRShift ) | // red
				( ( ( pSource[ j ] & 0x3e0 ) >> 5 ) << iGShift ) |	 // green
				( ( pSource[ j ] & 0x1f ) << iBShift );				 // blue
		}
		
		// Move bitmap pointer to next line
		pBitmap -= bmp_pitch;
		pSource = (WORD*) pBitmap;
		
		// Move surface pointer to next line
		pSurface += surface_pitch;
		pDest = (WORD*) pSurface;
	}
	return true;
}

bool Bitmap::ConvertBitmap_16_555_32_XXX( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight )
{
	// 16 bit 555 to 32 bit xxxx xxx
	BYTE* pBitmap = (BYTE*) bitmapBits;
	BYTE* pSurface = (BYTE*) pDesc->lpSurface;
	assert( iBitmapHeight > 0  && "Bottom-up bitmap expected" );

	// Pitches
	long bmp_pitch=((((iBitmapWidth*16)+31)&~31)>>3);
	long surface_pitch = pDesc->lPitch;

	// Adjust for upside-down, start at last line of bitmap
	pBitmap += bmp_pitch * ( iBitmapHeight - 1 );
		
	// Temp pointers for easy color reading
	WORD* pSource = (WORD*) pBitmap;
	DWORD* pDest = (DWORD*) pSurface;
	DWORD temp;
	
	// Amount to shift
	int iRShift(0), iBShift(0), iGShift(0); // added initialization values for iRShift & iBShift -taj
	int i;
	DWORD color;
	
	// Find amount to shift for red
	for ( i = 0, temp = 1; i < 32; i++ )
	{
		if ( temp & pDesc->ddpfPixelFormat.dwRBitMask ) 
		{ iRShift = i; break; }
		temp <<= 1;
	}
	
	// Find amount to shift for green
	for ( i = 0, temp = 1; i < 32; i++ )
	{
		if ( temp & pDesc->ddpfPixelFormat.dwGBitMask ) 
		{ iGShift = i; break; }
		temp <<= 1;
	}
	
	// Find amount to shift for blue
	for ( i = 0, temp = 1; i < 32; i++ )
	{
		if ( temp & pDesc->ddpfPixelFormat.dwBBitMask ) 
		{ iBShift = i; break; }
		temp <<= 1;
	}

	/*
	=============================================
	RGB 555 to 888 xxx
	=============================================
	RED		5 to R bits, >> 10 << R shift
	GREEN	5 to G bits, >> 5 << G shift
	BLUE	5 to B bits, >> 0 << B shift

	This leaves three empty bits that need
	to be filled with something. We will
	fill those with the three most
	significant bits of the 5 bit value.
	Added 4-4-02 -JES
	=============================================
	*/

	// Account for shifting of 5 bit source to 8 bit
	int const iRShift5To8 = iRShift + 3;
	int const iBShift5To8 = iBShift + 3;
	int const iGShift5To8 = iGShift + 3;

	for ( i = 0; i < iBitmapHeight; ++i )
	{
		for ( int j = 0; j < iBitmapWidth; ++j )
		{
			color = pSource[ j ];
			
			temp = ( (color & 0x7c00) >> 10 ) << iRShift5To8;
			temp |= ( (color & 0x7000) >> 12 ) << iRShift;

			temp |= ( ( color & 0x3e0 ) >> 5 ) << iGShift5To8;
			temp |= ( ( color & 0x380 ) >> 7 ) << iGShift;

			temp |= ( color & 0x1f ) << iBShift5To8;
			temp |= ( ( color & 0x1c ) >> 2 ) << iBShift;

			pDest[ j ] = temp;
		}
		
		// Move bitmap pointer to next line
		pBitmap -= bmp_pitch;
		pSource = (WORD*) pBitmap;
		
		// Move surface pointer to next line
		pSurface += surface_pitch;
		pDest = (DWORD*) pSurface;
	}
	return true;
}

#else

bool Bitmap::ConvertBitmap_16_565_16_555_RGB( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight )
{
	// 16 bit 555 to 16 bit 565 RGB
	BYTE* pBitmap = (BYTE*) bitmapBits;
	BYTE* pSurface = (BYTE*) pDesc->lpSurface;
	assert( iBitmapHeight > 0  && "Bottom-up bitmap expected" );

	// Pitches
	long bmp_pitch=((((iBitmapWidth*16)+31)&~31)>>3);
	long surface_pitch = pDesc->lPitch;

	// Adjust for upside-down, start at last line of bitmap
	pBitmap += bmp_pitch * ( iBitmapHeight - 1 );
		
	// Temp pointers for easy color reading
	WORD* pSource = (WORD*) pBitmap;
	WORD* pDest = (WORD*) pSurface;
	
	/*
	========================================
	RGB 565 to RGB 555
	========================================
	*/

	const int ndoubles = iBitmapWidth >> 1;
	const int nsingles = iBitmapWidth % ndoubles;

	for ( int i = 0; i < iBitmapHeight; i++ )
	{
		DWORD * pDWordDest = (DWORD *)pDest;
		DWORD * pDWordSrc = (DWORD *)pSource;
		DWORD * pDestEnd = pDWordDest + ndoubles;

		while (pDWordDest < pDestEnd)
		{
			*pDWordDest = 
				( ( *pDWordSrc & 0xffc0ffc0 ) >> 1 ) | 
				( *pDWordSrc & 0x001f001f );

			pDWordDest++;
			pDWordSrc++;
		}

		if ( nsingles )
		{
			WORD* pWordDest = (WORD*) pDWordDest;
			WORD* pWordSrc = (WORD*) pDWordSrc;
			*pWordDest = 
				( ( *pWordSrc & 0xffc0 ) >> 1 ) | 
				( *pWordSrc & 0x001f );
		}

		// Move bitmap pointer to next line
		pBitmap -= bmp_pitch;
		pSource = (WORD*) pBitmap;
		
		// Move surface pointer to next line
		pSurface += surface_pitch;
		pDest = (WORD*) pSurface;
	}

	return true;
}

	
bool Bitmap::ConvertBitmap_16_565_16_565_RGB( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight )
{
	// Copy directly
	return false;
}


bool Bitmap::ConvertBitmap_16_565_32_RGB( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight )
{
	assert( 0 && "Not implemented" );
	return false;
}


bool Bitmap::ConvertBitmap_16_565_16_XXX( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight )
{
	assert( 0 && "Not implemented" );
	return false;
}

	
bool Bitmap::ConvertBitmap_16_565_32_XXX( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight )
{
	assert( 0 && "Not implemented" );
	return false;
}

#endif

// ==============
// 24 bit sources
// ==============

bool Bitmap::ConvertBitmap_24_16_555_RGB( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight )
{
	// 24 bit to 16 bit 555 RGB
	BYTE* pBitmap = (BYTE*) bitmapBits;
	BYTE* pSurface = (BYTE*) pDesc->lpSurface;
	assert( iBitmapHeight > 0  && "Bottom-up bitmap expected" );
	
	// Pitches
	long bmp_pitch=((((iBitmapWidth* 24)+31)&~31)>>3);
	long surface_pitch = pDesc->lPitch;
	
	// Adjust for upside-down, start at last line of bitmap
	pBitmap += bmp_pitch * ( iBitmapHeight - 1 );
	
	// Temp pointers for easy color reading
	BYTE* pSource = (BYTE*) pBitmap;
	WORD* pDest = (WORD*) pSurface;
	
	/*
	========================================
	BGR 888 to RGB 555
	========================================
	RED		8 to 5 bits, >> 3 << 11 
	GREEN	8 to 5 bits, >> 3 << 5
	BLUE	8 to 5 bits, >> 3
	========================================
	*/

	const int doubles( iBitmapWidth / 2 );
	const int singles( iBitmapWidth % 2 );

	for ( int i = 0; i < iBitmapHeight; i++ )
	{
		WORD* pDestEnd = pDest + ( doubles << 1 );
		while ( pDest < pDestEnd )
		{
			pDest[ 0 ] =
				( ( pSource[ 2 ] & 0xf8 ) << 7 ) | // RED
				( ( pSource[ 1 ] & 0xf8 ) << 2 ) | // GREEN
				( ( pSource[ 0 ] & 0xf8 ) >> 3 );  // BLUE
			pDest[ 1 ] =
				( ( pSource[ 5 ] & 0xf8 ) << 7 ) | // RED
				( ( pSource[ 4 ] & 0xf8 ) << 2 ) | // GREEN
				( ( pSource[ 3 ] & 0xf8 ) >> 3 );  // BLUE
			pSource += 6;
			pDest+=2;
		}

		if ( singles )
		{
			pDest[ 0 ] =
				( ( pSource[ 2 ] & 0xf8 ) << 7 ) | // RED
				( ( pSource[ 1 ] & 0xf8 ) << 2 ) | // GREEN
				( ( pSource[ 0 ] & 0xf8 ) >> 3 );  // BLUE
		}
		
		
		/*
		for ( int j = 0; j < iBitmapWidth; j++ )
		{
			pDest[ j ] =
				( ( pSource[ 2 ] & 0xf8 ) << 7 ) | // RED
				( ( pSource[ 1 ] & 0xf8 ) << 2 ) | // GREEN
				( ( pSource[ 0 ] & 0xf8 ) >> 3 );  // BLUE
			pSource += 3;
		}
		*/

		// Move bitmap pointer to next line
		pBitmap -= bmp_pitch;
		pSource = pBitmap;
		
		// Move surface pointer to next line
		pSurface += surface_pitch;
		pDest = (WORD*) pSurface;
	}

	return true;
}

bool Bitmap::ConvertBitmap_24_16_565_RGB( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight )
{
	// 24 bit to 16 bit 565 RGB
	BYTE* pBitmap = (BYTE*) bitmapBits;
	BYTE* pSurface = (BYTE*) pDesc->lpSurface;
	assert( iBitmapHeight > 0  && "Bottom-up bitmap expected" );

	// Pitches
	long bmp_pitch=((((iBitmapWidth* 24)+31)&~31)>>3);
	long surface_pitch = pDesc->lPitch;

	// Adjust for upside-down, start at last line of bitmap
	pBitmap += bmp_pitch * ( iBitmapHeight - 1 );
		
	// Temp pointers for easy color reading
	BYTE* pSource = (BYTE*) pBitmap;
	WORD* pDest = (WORD*) pSurface;
	
	
	/*
	========================================
	BGR 888 to RGB 565
	========================================
	RED		8 to 5 bits, >> 3 << 11 
	GREEN	8 to 6 bits, >> 2 << 5
	BLUE	8 to 5 bits, >> 3
	========================================
	*/

	for ( int i = 0; i < iBitmapHeight; i++ )
	{
		for ( int j = 0; j < iBitmapWidth; j++ )
		{
			pDest[ j ] =
				( ( pSource[ 2 ] & 0xf8 ) << 8 ) | 
				( ( pSource[ 1 ] & 0xfc ) << 3 ) |
				( ( pSource[ 0 ] & 0xf8 ) >> 3 );
			pSource += 3;
		}

		// Move bitmap pointer to next line
		pBitmap -= bmp_pitch;
		pSource = pBitmap;
		
		// Move surface pointer to next line
		pSurface += surface_pitch;
		pDest = (WORD*) pSurface;
	}

	return true;
}

bool Bitmap::ConvertBitmap_24_32_RGB( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight )
{
	// 24 bit to 32 bit 0888 RGB
	BYTE* pBitmap = (BYTE*) bitmapBits;
	BYTE* pSurface = (BYTE*) pDesc->lpSurface;
	assert( iBitmapHeight > 0  && "Bottom-up bitmap expected" );

	// Pitches
	long bmp_pitch=((((iBitmapWidth* 24)+31)&~31)>>3);
	long surface_pitch = pDesc->lPitch;

	// Adjust for upside-down, start at last line of bitmap
	pBitmap += bmp_pitch * ( iBitmapHeight - 1 );
		
	// Temp pointers for easy color reading
	BYTE* pSource = (BYTE*) pBitmap;
	DWORD* pDest = (DWORD*) pSurface;
	
	/*
	========================================
	BGR 888 to RGB 0888
	========================================
	RED		8 bits << 16
	GREEN	8 bits << 8
	BLUE	8 bits
	========================================
	*/

	for ( int i = 0; i < iBitmapHeight; i++ )
	{
		for ( int j = 0; j < iBitmapWidth; j++ )
		{
			pDest[ j ] = 
			( pSource[ 2 ] << 16 ) |	// RED
			( pSource[ 1 ] << 8 ) |		// GREEN
			( pSource[ 0 ] );			// BLUE
			pSource += 3;
		}
		
		// Move bitmap pointer to next line
		pBitmap -= bmp_pitch;
		pSource = pBitmap;
		
		// Move surface pointer to next line
		pSurface += surface_pitch;
		pDest = (DWORD*) pSurface;
	}
	return true;
}

// Non-optimal

bool Bitmap::ConvertBitmap_24_16_XXX( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight )
{
	BYTE* pBitmap = (BYTE*) bitmapBits;
	BYTE* pSurface = (BYTE*) pDesc->lpSurface;
	assert( iBitmapHeight > 0  && "Bottom-up bitmap expected" );
	
	// Pitches
	long bmp_pitch=((((iBitmapWidth* 24)+31)&~31)>>3);
	long surface_pitch = pDesc->lPitch;
	
	// Adjust for upside-down, start at last line of bitmap
	pBitmap += bmp_pitch * ( iBitmapHeight - 1 );
	
	// Temp pointers for easy color reading
	BYTE* pSource = (BYTE*) pBitmap;
	WORD* pDest = (WORD*) pSurface;
	WORD temp;
	
	// Amount to shift
	int iRShift = 0, iBShift = 0, iGShift = 0;
	int iRBits = 0, iBBits = 0, iGBits = 0;
	int i;
	
	// Find amount to shift for red
	for ( i = 0, temp = 1; i < 16; i++ )
	{
		if ( temp & pDesc->ddpfPixelFormat.dwRBitMask ) 
		{ iRShift = i; break; }
		temp <<= 1;
	}
	for ( ; i < 16; i++ )
	{
		if ( ! ( temp & pDesc->ddpfPixelFormat.dwRBitMask ) )
			break;
		iRBits++;
		temp <<= 1;
	}
	
	// Find amount to shift for green
	for ( i = 0, temp = 1; i < 16; i++ )
	{
		if ( temp & pDesc->ddpfPixelFormat.dwGBitMask ) 
		{ iGShift = i; break; }
		temp <<= 1;
	}
	for ( ; i < 16; i++ )
	{
		if ( ! ( temp & pDesc->ddpfPixelFormat.dwGBitMask ) )
			break;
		iGBits++;
		temp <<= 1;
	}
	
	// Find amount to shift for blue
	for ( i = 0, temp = 1; i < 16; i++ )
	{
		if ( temp & pDesc->ddpfPixelFormat.dwBBitMask ) 
		{ iBShift = i; break; }
		temp <<= 1;
	}
	for ( ; i < 16; i++ )
	{
		if ( ! ( temp & pDesc->ddpfPixelFormat.dwBBitMask ) )
			break;
		iBBits++;
		temp <<= 1;
	}
		  
	/*
	=============================================
	BGR 888 to xxx xxx
	=============================================
	RED		8 to R bits, >> 8 - R bits << R shift
	GREEN	8 to G bits, >> 8 - G bits << G shift
	BLUE	8 to B bits, >> 8 - B bits << B shift
	=============================================
	*/

	for ( i = 0; i < iBitmapHeight; i++ )
	{
		for ( int j = 0; j < iBitmapWidth; j++ )
		{
			pDest[ j ] =
				( pSource[ 2 ] >> ( 8 - iRBits ) << iRShift ) | // RED
				( pSource[ 1 ] >> ( 8 - iGBits ) << iGShift ) | // GREEN
				( pSource[ 0 ] >> ( 8 - iBBits ) << iBShift ); // BLUE
			pSource += 3;
		}
		
		// Move bitmap pointer to next line
		pBitmap -= bmp_pitch;
		pSource = pBitmap;
		
		// Move surface pointer to next line
		pSurface += surface_pitch;
		pDest = (WORD*) pSurface;
	}
	return true;
}

bool Bitmap::ConvertBitmap_24_32_XXX( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight )
{
	BYTE* pBitmap = (BYTE*) bitmapBits;
	BYTE* pSurface = (BYTE*) pDesc->lpSurface;
	assert( iBitmapHeight > 0  && "Bottom-up bitmap expected" );
	
	// Pitches
	long bmp_pitch=((((iBitmapWidth*24)+31)&~31)>>3);
	long surface_pitch = pDesc->lPitch;
	
	// Adjust for upside-down, start at last line of bitmap
	pBitmap += bmp_pitch * ( iBitmapHeight - 1 );
	
	// Temp pointers for easy color reading
	BYTE* pSource = (BYTE*) pBitmap;
	DWORD* pDest = (DWORD*) pSurface;
	DWORD temp;
	
	// Amount to shift
	int iRShift = 0, iBShift = 0, iGShift = 0;
	int iRBits = 0, iBBits = 0, iGBits = 0;
	int i;
	
	// Find amount to shift for red
	for ( i = 0, temp = 1; i < 32; i++ )
	{
		if ( temp & pDesc->ddpfPixelFormat.dwRBitMask ) 
		{ iRShift = i; break; }
		temp <<= 1;
	}
	for ( ; i < 32; i++ )
	{
		if ( ! ( temp & pDesc->ddpfPixelFormat.dwRBitMask ) )
			break;
		iRBits++;
		temp <<= 1;
	}
	
	// Find amount to shift for green
	for ( i = 0, temp = 1; i < 32; i++ )
	{
		if ( temp & pDesc->ddpfPixelFormat.dwGBitMask ) 
		{ iGShift = i; break; }
		temp <<= 1;
	}
	for ( ; i < 32; i++ )
	{
		if ( ! ( temp & pDesc->ddpfPixelFormat.dwGBitMask ) )
			break;
		iGBits++;
		temp <<= 1;
	}
	
	// Find amount to shift for blue
	for ( i = 0, temp = 1; i < 32; i++ )
	{
		if ( temp & pDesc->ddpfPixelFormat.dwBBitMask ) 
		{ iBShift = i; break; }
		temp <<= 1;
	}
	for ( ; i < 32; i++ )
	{
		if ( ! ( temp & pDesc->ddpfPixelFormat.dwBBitMask ) )
			break;
		iBBits++;
		temp <<= 1;
	}
		  
	/*
	=============================================
	BGR 888 to xxx 0888
	=============================================
	RED		8 to 8 bits, << R shift
	GREEN	8 to 8 bits, << G shift
	BLUE	8 to 8 bits, << B shift
	=============================================
	*/

	for ( i = 0; i < iBitmapHeight; i++ )
	{
		for ( int j = 0; j < iBitmapWidth; j++ )
		{
			pDest[ j ] =
				( pSource[ 2 ] << iRShift ) | // RED
				( pSource[ 1 ] << iGShift ) | // GREEN
				( pSource[ 0 ] << iBShift ); // BLUE
			pSource += 3;
		}
		
		// Move bitmap pointer to next line
		pBitmap -= bmp_pitch;
		pSource = pBitmap;
		
		// Move surface pointer to next line
		pSurface += surface_pitch;
		pDest = (DWORD*) pSurface;
	}
	return true;
}


// ==============
// 32 bit sources
// ==============

bool Bitmap::ConvertBitmap_32_16_555_RGB( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight )
{
	assert(0 && _T("This is untested."));

	// 32 bit to 16 bit 555 RGB
	BYTE* pBitmap = (BYTE*) bitmapBits;
	BYTE* pSurface = (BYTE*) pDesc->lpSurface;
	assert( iBitmapHeight > 0  && "Bottom-up bitmap expected" );
	
	// Pitches
	long bmp_pitch=((((iBitmapWidth*32)+31)&~31)>>3);
	long surface_pitch = pDesc->lPitch;
	
	// Adjust for upside-down, start at last line of bitmap
	pBitmap += bmp_pitch * ( iBitmapHeight - 1 );
	
	// Temp pointers for easy color reading
	BYTE* pSource = (BYTE*) pBitmap;
	WORD* pDest = (WORD*) pSurface;
	
	/*
	========================================
	ABGR 8888 to RGB 555
	========================================
	RED		8 to 5 bits, >> 3 << 11 
	GREEN	8 to 5 bits, >> 3 << 5
	BLUE	8 to 5 bits, >> 3
	========================================
	*/

	for ( int i = 0; i < iBitmapHeight; i++ )
	{
		for ( int j = 0; j < iBitmapWidth; j++ )
		{
			pDest[ j ] =
				( ( pSource[ 3 ] & 0xf8 ) << 7 ) | // RED
				( ( pSource[ 2 ] & 0xf8 ) << 2 ) | // GREEN
				( ( pSource[ 1 ] & 0xf8 ) >> 3 );  // BLUE
			pSource += 4;
		}
		
		// Move bitmap pointer to next line
		pBitmap -= bmp_pitch;
		pSource = pBitmap;
		
		// Move surface pointer to next line
		pSurface += surface_pitch;
		pDest = (WORD*) pSurface;
	}
	return true;
}

bool Bitmap::ConvertBitmap_32_16_565_RGB( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight )
{
	assert(0 && _T("This is untested."));

	// 32 bit to 16 bit 565 RGB
	BYTE* pBitmap = (BYTE*) bitmapBits;
	BYTE* pSurface = (BYTE*) pDesc->lpSurface;
	assert( iBitmapHeight > 0  && "Bottom-up bitmap expected" );

	// Pitches
	long bmp_pitch=((((iBitmapWidth*32)+31)&~31)>>3);
	long surface_pitch = pDesc->lPitch;

	// Adjust for upside-down, start at last line of bitmap
	pBitmap += bmp_pitch * ( iBitmapHeight - 1 );
		
	// Temp pointers for easy color reading
	BYTE* pSource = (BYTE*) pBitmap;
	WORD* pDest = (WORD*) pSurface;
	
	/*
	========================================
	ABGR 8888 to RGB 565
	========================================
	RED		8 to 5 bits, >> 3 << 11 
	GREEN	8 to 6 bits, >> 2 << 5
	BLUE	8 to 5 bits, >> 3
	========================================
	*/

	for ( int i = 0; i < iBitmapHeight; i++ )
	{
		for ( int j = 0; j < iBitmapWidth; j++ )
		{
			pDest[ j ] =
			( ( pSource[ 3 ] & 0xf8 ) << 8 ) | // RED
			( ( pSource[ 2 ] & 0xfc ) << 3 ) | // GREEN
			( ( pSource[ 1 ] & 0xf8 ) >> 3 );  // BLUE
			pSource += 4;
		}
		
		// Move bitmap pointer to next line
		pBitmap -= bmp_pitch;
		pSource = pBitmap;
		
		// Move surface pointer to next line
		pSurface += surface_pitch;
		pDest = (WORD*) pSurface;
	}
	return true;
}

bool Bitmap::ConvertBitmap_32_32_RGB( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight )
{
	assert(0 && _T("This is untested."));

	// 32 bit to 32 bit 0888 RGB
	BYTE* pBitmap = (BYTE*) bitmapBits;
	BYTE* pSurface = (BYTE*) pDesc->lpSurface;
	assert( iBitmapHeight > 0  && "Bottom-up bitmap expected" );

	// Pitches
	long bmp_pitch=((((iBitmapWidth*32)+31)&~31)>>3);
	long surface_pitch = pDesc->lPitch;

	// Adjust for upside-down, start at last line of bitmap
	pBitmap += bmp_pitch * ( iBitmapHeight - 1 );
		
	// Temp pointers for easy color reading
	BYTE* pSource = (BYTE*) pBitmap;
	DWORD* pDest = (DWORD*) pSurface;
	
	/*
	========================================
	ABGR 8888 to RGB 0888
	========================================
	RED		8 bits << 16
	GREEN	8 bits << 8
	BLUE	8 bits
	========================================
	*/

	for ( int i = 0; i < iBitmapHeight; i++ )
	{
		for ( int j = 0; j < iBitmapWidth; j++ )
		{
			pDest[ j ] = 
			( pSource[ 3 ] << 16 ) |	// RED
			( pSource[ 2 ] << 8 ) |		// GREEN
			( pSource[ 1 ] );			// BLUE
			pSource += 4;
		}
		
		// Move bitmap pointer to next line
		pBitmap -= bmp_pitch;
		pSource = pBitmap;
		
		// Move surface pointer to next line
		pSurface += surface_pitch;
		pDest = (DWORD*) pSurface;
	}
	return true;
}

// Non-optimal

bool Bitmap::ConvertBitmap_32_16_XXX( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight )
{

	BYTE* pBitmap = (BYTE*) bitmapBits;
	BYTE* pSurface = (BYTE*) pDesc->lpSurface;
	assert( iBitmapHeight > 0  && "Bottom-up bitmap expected" );
	
	// Pitches
	long bmp_pitch=((((iBitmapWidth*24)+31)&~31)>>3);
	long surface_pitch = pDesc->lPitch;
	
	// Adjust for upside-down, start at last line of bitmap
	pBitmap -= bmp_pitch * iBitmapHeight ;
	
	// Temp pointers for easy color reading
	BYTE* pSource = (BYTE*) pBitmap;
	WORD* pDest = (WORD*) pSurface;
	WORD temp;
	
	// Amount to shift
	int iRShift = 0, iBShift = 0, iGShift = 0;
	int iRBits = 0, iBBits = 0, iGBits = 0;
	int i;
	
	// Find amount to shift for red
	for ( i = 0, temp = 1; i < 16; i++ )
	{
		if ( temp & pDesc->ddpfPixelFormat.dwRBitMask ) 
		{ iRShift = i; break; }
		temp <<= 1;
	}
	for ( ; i < 16; i++ )
	{
		if ( ! ( temp & pDesc->ddpfPixelFormat.dwRBitMask ) )
			break;
		iRBits++;
		temp <<= 1;
	}
	
	// Find amount to shift for green
	for ( i = 0, temp = 1; i < 16; i++ )
	{
		if ( temp & pDesc->ddpfPixelFormat.dwGBitMask ) 
		{ iGShift = i; break; }
		temp <<= 1;
	}
	for ( ; i < 16; i++ )
	{
		if ( ! ( temp & pDesc->ddpfPixelFormat.dwGBitMask ) )
			break;
		iGBits++;
		temp <<= 1;
	}
	
	// Find amount to shift for blue
	for ( i = 0, temp = 1; i < 16; i++ )
	{
		if ( temp & pDesc->ddpfPixelFormat.dwBBitMask ) 
		{ iBShift = i; break; }
		temp <<= 1;
	}
	for ( ; i < 16; i++ )
	{
		if ( ! ( temp & pDesc->ddpfPixelFormat.dwBBitMask ) )
			break;
		iBBits++;
		temp <<= 1;
	}
		  
	/*
	=============================================
	ABGR 8888 to xxx xxx
	=============================================
	RED		8 to R bits, >> 8 - R bits << R shift
	GREEN	8 to G bits, >> 8 - G bits << G shift
	BLUE	8 to B bits, >> 8 - B bits << B shift
	=============================================
	*/

	for ( i = 0; i < iBitmapHeight; i++ )
	{
		for ( int j = 0; j < iBitmapWidth; j++ )
		{
			pDest[ j ] =
				( pSource[ 3 ] >> ( 8 - iRBits ) << iRShift ) | // RED
				( pSource[ 1 ] >> ( 8 - iGBits ) << iGShift ) | // GREEN
				( pSource[ 2 ] >> ( 8 - iBBits ) << iBShift ); // BLUE
			pSource += 3;
		}
		
		// Move bitmap pointer to next line
		pBitmap += bmp_pitch;
		pSource = pBitmap;
		
		// Move surface pointer to next line
		pSurface += surface_pitch;
		pDest = (WORD*) pSurface;
	}
	return true;
}

bool Bitmap::ConvertBitmap_32_32_XXX( DDSURFACEDESC2* pDesc, const char* bitmapBits, int iBitmapWidth, int iBitmapHeight )
{
	BYTE* pBitmap = (BYTE*) bitmapBits;
	BYTE* pSurface = (BYTE*) pDesc->lpSurface;
	assert( iBitmapHeight > 0  && "Bottom-up bitmap expected" );
	
	// Pitches
	long bmp_pitch=((((iBitmapWidth*24)+31)&~31)>>3);
	long surface_pitch = pDesc->lPitch;
	
	// Adjust for upside-down, start at last line of bitmap
	pBitmap -= bmp_pitch * iBitmapHeight;
	
	// Temp pointers for easy color reading
	BYTE* pSource = (BYTE*) pBitmap;
	DWORD* pDest = (DWORD*) pSurface;
	DWORD temp;
	
	// Amount to shift
	int iRShift = 0, iBShift = 0, iGShift = 0;
	int iRBits = 0, iBBits = 0, iGBits = 0;
	int i;
	
	// Find amount to shift for red
	for ( i = 0, temp = 1; i < 32; i++ )
	{
		if ( temp & pDesc->ddpfPixelFormat.dwRBitMask ) 
		{ iRShift = i; break; }
		temp <<= 1;
	}
	for ( ; i < 32; i++ )
	{
		if ( ! ( temp & pDesc->ddpfPixelFormat.dwRBitMask ) )
			break;
		iRBits++;
		temp <<= 1;
	}
	
	// Find amount to shift for green
	for ( i = 0, temp = 1; i < 32; i++ )
	{
		if ( temp & pDesc->ddpfPixelFormat.dwGBitMask ) 
		{ iGShift = i; break; }
		temp <<= 1;
	}
	for ( ; i < 32; i++ )
	{
		if ( ! ( temp & pDesc->ddpfPixelFormat.dwGBitMask ) )
			break;
		iGBits++;
		temp <<= 1;
	}
	
	// Find amount to shift for blue
	for ( i = 0, temp = 1; i < 32; i++ )
	{
		if ( temp & pDesc->ddpfPixelFormat.dwBBitMask ) 
		{ iBShift = i; break; }
		temp <<= 1;
	}
	for ( ; i < 32; i++ )
	{
		if ( ! ( temp & pDesc->ddpfPixelFormat.dwBBitMask ) )
			break;
		iBBits++;
		temp <<= 1;
	}
		  
	/*
	=============================================
	ABGR 8888 to xxx 0888
	=============================================
	RED		8 to 8 bits, << R shift
	GREEN	8 to 8 bits, << G shift
	BLUE	8 to 8 bits, << B shift
	=============================================
	*/

	for ( i = 0; i < iBitmapHeight; i++ )
	{
		for ( int j = 0; j < iBitmapWidth; j++ )
		{
			pDest[ j ] =
				( pSource[ 3 ] << iRShift ) | // RED
				( pSource[ 1 ] << iGShift ) | // GREEN
				( pSource[ 2 ] << iBShift ); // BLUE
			pSource += 3;
		}
		
		// Move bitmap pointer to next line
		pBitmap += bmp_pitch;
		pSource = pBitmap;
		
		// Move surface pointer to next line
		pSurface += surface_pitch;
		pDest = (DWORD*) pSurface;
	}
	return true;
}

//-----------------------------------------------------------------------------
// Name: Bitmap::MakeSingleBitmap()
// Desc: Make a single bitmap given a string of frames
//		 and allocating a new buffer in ram to store the new bitmap.
//		 This returns the pointer to the new ram and also sets datasize
//		 to the size of the new buffer
//-----------------------------------------------------------------------------
const char* Bitmap::MakeSingleBitmap(const char* inputdata, int startFrame, int numFrames, int totalFrames, int *datasize )
{
	// The image is at the start of the buffer + the frame offset
	int* pOffset = (int*) inputdata;
	const char* pImage = inputdata + ( ( totalFrames == 1 ) ? 0 : pOffset[startFrame] );

	BITMAPFILEHEADER* bmf=(BITMAPFILEHEADER *) pImage;
	BITMAPINFOHEADER* bmpInfoHdr=(BITMAPINFOHEADER *)(&bmf[1]);
	BITMAPINFO* bitmapInfo=(BITMAPINFO *)bmpInfoHdr;

	int infosize= bmpInfoHdr->biSize;
	assert( infosize == sizeof( BITMAPINFOHEADER ) );

	int headersize = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER );

	*datasize = headersize;

	long frameSize;
	long pitch;

	bool _8bit = bmpInfoHdr->biBitCount == 8;
	bool _RLE = _8bit && bmpInfoHdr->biCompression==BI_RLE8;

	// If the total #frames is one, then
	// there is no need to crate a copy of this
	if (totalFrames == 1 && !_RLE )
	{
		// Just return the original
		return inputdata;
	}

	// Calc size needed
	if ( _RLE )
	{
		frameSize = GetRLEBitSize( pImage );
	}
	else
	{
		pitch = ((((bmpInfoHdr->biWidth * bmpInfoHdr->biBitCount)+31)&~31)>>3);
		frameSize = pitch*bmpInfoHdr->biHeight;
	}
	*datasize += frameSize * numFrames;
	
	// 8 bit has RGB quad data
	if ( _8bit )
		*datasize += sizeof( RGBQUAD ) * 256;
	
	// Alloc new block
	char* outputdata = new char[ *datasize ];
	const char* pImageBits;
	char* pNewBits = outputdata;
	
	// Copy the first frame's file header
	memcpy( outputdata, pImage, bmf->bfOffBits );

	// Set new height of the bitmap
	BITMAPFILEHEADER* _bmf=(BITMAPFILEHEADER *) outputdata;
	_bmf->bfSize = headersize + frameSize * numFrames;
	BITMAPINFOHEADER* _bmpInfoHdr=(BITMAPINFOHEADER *)(&_bmf[1]);
	_bmpInfoHdr->biHeight *= numFrames;
	_bmpInfoHdr->biSizeImage = frameSize * numFrames;
		
	// Now decoded, no RLE decoding is needed
	if ( _RLE )
		_bmpInfoHdr->biCompression = BI_RGB;

	BITMAPFILEHEADER* imgHeader;

	// Set new image pointer
	pNewBits = outputdata + bmf->bfOffBits;

	// Loop for each frame
	int endFrame = startFrame + numFrames - 1;
	for ( int i = endFrame; i >= startFrame; i-- )
	{
		pImage = inputdata + ( ( totalFrames == 1 ) ? 0 : pOffset[i] );

		if ( _RLE )
		{
			DecodeRLEBits( pImage, pNewBits );
		}
		else
		{
			imgHeader = (BITMAPFILEHEADER *) pImage;
			pImageBits = pImage + imgHeader->bfOffBits;
			memcpy( pNewBits, pImageBits, bmpInfoHdr->biSizeImage );
		}

		pNewBits += frameSize;
	}

	return outputdata;
}


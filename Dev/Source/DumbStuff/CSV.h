#pragma once
//========================================================================
// CSV.h : CSV is a class to parse CSV files, used to store game data
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
//  Content References in Game Coding Complete
// 
//  Not described in the book, but a nice little parser for grabbing 
//  data out of CSV files.
//
//========================================================================

class CSV
{
public:
	CSV(const TCHAR* filename);
	~CSV();

	void ParseItem(char const * const line, int item, char *output) const;
	void ReadLine(char const * const section, char const * const entry, char *output) const;
	int const GetInt(char const * const entry, const int& item=0) const;
	int const GetInt(char const * const section, char const * const entry, const int& item) const;
	void GetString(char const * const entry, char *outputString) const;
	bool const GetBool(char const * const entry) const;
	int GetEnum(char const * const entry) const;

protected:
	char *m_orig_file;
	int m_length;
};


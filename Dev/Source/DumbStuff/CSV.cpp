//========================================================================
// CSV.cpp : CSV is a class to parse CSV files, used to store game data
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
// class CSV			- 
//
//========================================================================

#include "StdAfx.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "CSV.h"


CSV::~CSV()
{
	if (m_orig_file)
	{
		delete m_orig_file;
		m_orig_file = NULL;
	}
}


CSV::CSV(const TCHAR* filename)
{
	m_orig_file = NULL;
	m_length = 0;

	FILE *csvFile = fopen(filename, "r");
	assert(csvFile);

	// Grab the length of the file and allocate a buffer
	fseek(csvFile, 0, SEEK_END);
	m_length = ftell(csvFile);
	fseek(csvFile, 0, SEEK_SET);
	
	// Allocate a buffer and read the file
	m_orig_file = new char[m_length];
	assert(m_orig_file);
	fread(m_orig_file, m_length, 1, csvFile);
	fclose(csvFile);
}

void CSV::ParseItem(char const * const line, int const _item, char *output) const
{
	int item = _item;
	char buffer[256];
	char *str;
	char seps[]   = " ,\t\r\n=";

	strcpy(buffer, line);
	str = strtok(buffer, seps);
	while (item > 0)
	{
		str = strtok(NULL, seps);
		--item;
	}
	strcpy(output, str);
}


void CSV::ReadLine(char const * const section, char const * const line, char *output) const
{
	char *buffer = new char[m_length];
	assert(buffer);

	char *tmpBuffer;
	char seps[]   = " ,\t\r\n=";
	char newLine[] = "\r\n";
	size_t len;

	memcpy(buffer, m_orig_file, m_length);
	tmpBuffer = strtok( buffer, newLine );
	len = strlen(section);

	// Find the section header.
	// Assumption: Section will be found or loop won't end.
	while ((strlen(tmpBuffer) < len) || (_strnicmp(tmpBuffer, section, len)))
	{
		tmpBuffer = strtok(NULL, newLine);
		assert(tmpBuffer);
	}

	// Get the first line in the section.
	tmpBuffer = strtok(NULL, newLine);

	bool success = false;

	while (tmpBuffer)
	{
		// Ignore commented lines and blank lines.
		if ((tmpBuffer[0] == ';') || (tmpBuffer[0] == ' '))
		{
			// Non match. Continue search.
			tmpBuffer = strtok( NULL, newLine );
			continue;
		}

		len = strlen(line);
		if ((strlen(tmpBuffer) > len) && (_strnicmp(tmpBuffer, line, len) == 0) &&
				((tmpBuffer[len] == ',') ||
				(tmpBuffer[len] == '\t') ||
				(tmpBuffer[len] == '\r') ||
				(tmpBuffer[len] == '\n') ||
				(tmpBuffer[len] == '='))
			)
		{
			// Found entry. Exit loop.
			tmpBuffer = strtok( tmpBuffer, seps );
			tmpBuffer = strtok( NULL, newLine );
			strcpy(output, tmpBuffer);
			tmpBuffer = NULL;
			success = true;
		}
		else
		{
			// Non match. Continue search.
			tmpBuffer = strtok( NULL, newLine );
		}
	}

	assert(success && "The entry was not found");

	delete buffer;
}


// Return an integer stored in the file.
int const CSV::GetInt(char const * const entry, const int& item) const
{
	return GetInt( "[Integer]", entry, item );
}//end GetInt


// Return boolean
bool const CSV::GetBool(char const * const entry) const
{
	return ( GetInt( "[Bool]", entry, 0 ) != 0 );
}

// Return an integer stored in the file in a section other than the Integer section
int const CSV::GetInt(char const * const section, char const * const entry, const int& item) const
{
	int retVal;

	char buffer[256];
	char str[256];
	ReadLine(section, entry, buffer);
	ParseItem(buffer, item, str);

	// Check for an enum if it's a string.
	if ((str[0] >= '0' && str[0] <= '9') || (str[0] == '-'))
	{
		retVal = atoi(str);
	}
	else
	{
		retVal = GetEnum( str );
	}

	return retVal;
}//end GetInt



// Return a string stored in the file.
// Commas in string are not used as separators.
void CSV::GetString(char const * const entry, char *outputString) const
{
	char intSection[] = "[String]";

	char buffer[256];
	ReadLine(intSection, entry, buffer);
	strcpy(outputString, buffer);
}


int CSV::GetEnum(char const * const entry) const
{
	char intSection[] = "[Enum]";

	char buffer[256];
	ReadLine(intSection, entry, buffer);
	return atoi(buffer);
}


// tLuaVector.h: interface for the tLuaVector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TLUAVECTOR_H__BAD115E6_A282_11D4_9DF6_0000B45D6611__INCLUDED_)
#define AFX_TLUAVECTOR_H__BAD115E6_A282_11D4_9DF6_0000B45D6611__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#include <iostream.h>

extern "C"
{
#include "lua.h"
}

#include "tLuaCOM.h"
#include "luabeans.h"


class tLuaVector  
{
public:
	long size(void);
  enum Type {NOTYPE, NUMBER, STRING, USERDATA, OTHER, MANY};

	void InitVectorFromTable(stkIndex table);
  void InitVectorFromStack(stkIndex first_elem);
  void InitVectorFromDimensions(
    long *dimensions,
    unsigned long num_dimensions);
	
	void setindex(
    stkIndex luaval,
    long *dimensions,
    unsigned long size,
    const SAFEARRAYBOUND* bounds);

	bool CreateTable(void);

	Type lua2Type(stkIndex luaval);
	Type getType(void);
  unsigned long getLength(void);
	stkIndex getindex(long *indices, unsigned long size);
	unsigned long get_Nth_Dimension(unsigned long n);
	unsigned long get_Dimensions(void);
	tLuaVector(LuaBeans *lbeans);
	virtual ~tLuaVector();

protected:
	LuaBeans* lbeans;
	lua_State* L;
  enum {UNKNOWN, VECTOR, SCALAR} elem_type;

	void freeData(void);

	unsigned long max_length;
	unsigned long length;
	stkIndex*  luavals;
	tLuaVector** vectors;
	bool initialized;

  Type array_type;
};

#endif // !defined(AFX_TLUAVECTOR_H__BAD115E6_A282_11D4_9DF6_0000B45D6611__INCLUDED_)

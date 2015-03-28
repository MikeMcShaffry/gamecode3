
// tLuaCOMTypeHandler.h: interface for the tLuaCOMTypeHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TLUACOMTYPEHANDLER_H__2CAF122C_E09B_11D3_BD2F_444553540000__INCLUDED_)
#define AFX_TLUACOMTYPEHANDLER_H__2CAF122C_E09B_11D3_BD2F_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <ole2.h>

#include "luabeans.h"
#include "tLuaObjList.h"
#include "LuaAux.h"

class tLuaCOM;

class tLuaCOMTypeHandler  
{
public:
  void setOutValues(FUNCDESC* pFuncDesc, DISPPARAMS* pDispParams, stkIndex outvalue);
  void pushLuaArgs(
    DISPPARAMS* pDispParams,
    const ELEMDESC* pElemDesc
    );

  void fillDispParams(
    DISPPARAMS& rDispParams,
    FUNCDESC *pfuncdesc,
    tLuaObjList& params,
    int invkind
    );

  void releaseVariants(DISPPARAMS *pDispParams);

  int pushOutValues(const DISPPARAMS& dispparams);

  bool setRetval(const FUNCDESC *funcdesc, stkIndex luaval, VARIANTARG *pvarg);

  void lua2com(stkIndex luaval, VARIANTARG& varg);

  void com2lua(VARIANTARG varg);
  tLuaCOMTypeHandler(ITypeInfo *ptypeinfo, LuaBeans *lbeans);
  virtual ~tLuaCOMTypeHandler();

	static TYPEDESC processTYPEDESC(
    ITypeInfo* typeinfo,
    TYPEDESC tdesc);

protected:
  long * dimensionsFromBounds(SAFEARRAYBOUND* bounds, long num_bounds);
  void put_in_array(SAFEARRAY* safearray,
                    VARIANT var_value,
                    long* indices,
                    VARTYPE vt);

  void inc_indices(long *indices,
                   SAFEARRAYBOUND *bounds,
                   unsigned long dimensions);

  SAFEARRAYBOUND* getRightOrderedBounds(SAFEARRAYBOUND *bounds, 
                                        unsigned long num_dimensions);

	void Coerce(VARIANTARG& dest, VARIANTARG src, VARTYPE vt);
	long VariantSize(VARTYPE vt);
	static TYPEDESC processSAFEARRAY(ITypeInfo* typeinfo, TYPEDESC& tdesc);
	void toByRefParam(VARIANT& var_source, VARIANTARG * pvarg_dest);
	void initByRefParam(VARIANTARG* pvarg, VARTYPE vt);
	void pushIUnknown(IUnknown *punk);
	bool isIUnknown(stkIndex value);
	static TYPEDESC processAliases(
    ITypeInfo* typeinfo,
    const TYPEDESC& tdesc);
	void releaseVariant(VARIANTARG *pvarg, bool release_memory=true);
	static TYPEDESC processUSERDEFINED(
    ITypeInfo* typeinfo,
    const TYPEDESC& tdesc);
	static TYPEDESC processPTR(
    ITypeInfo* typeinfo,
    const TYPEDESC& tdesc);
  lua_State *L;
  tLuaCOM * from_lua(int index);
  stkIndex get_from_array(
    SAFEARRAY* safearray,
    long *indices,
    const VARTYPE& vt);

  void safearray_com2lua(VARIANTARG& varg);

  void safearray_lua2com(
    stkIndex luaval,
    VARIANTARG& varg, VARTYPE vt,
    bool from_stack = false
    );

  void string2safearray(const char* str, long len, VARIANTARG& varg);
  void safearray2string(VARIANTARG & varg);

  ITypeInfo * m_typeinfo;
  LuaBeans *lbeans;
};

#define LUACOM_IUNKNOWN_TAGNAME "_LuaCOM_IUnknown_tag"

#endif // !defined(AFX_TLUACOMTYPEHANDLER_H__2CAF122C_E09B_11D3_BD2F_444553540000__INCLUDED_)

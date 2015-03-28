/*
 * tLuaCOM.cpp
 *
 *  Implementacao da classe tLuaCOM
 *
 * Renato Cerqueira
 * Vinicius Almendra
 */

// RCS Info
static char *rcsid = "$Id: tLuaCOM.cpp,v 1.35 2004/02/09 18:49:23 almendra Exp $";
static char *rcsname = "$Name:  $";

#include "tLuaCOM.h"
#include "tLuaDispatch.h"
#include "LuaAux.h"
#include "tUtil.h"
#include "tLuaCOMException.h"
#include "tCOMUtil.h"

#include "luacompat.h"

long tLuaCOM::NEXT_ID = 0;

tLuaCOM::tLuaCOM(IDispatch *pdisp_arg,
                 ITypeInfo *ptinfo_arg,
                 LuaBeans *lbeans,
                 CLSID coclass
                 )
{
  HRESULT hr = S_OK;

  // initialization
  connection_point        = NULL;
  connection_point_cookie = NULL;
  plib_tcomp              = NULL;
  ptcomp                  = NULL;
  clsid                   = IID_NULL;
  lock_count              = 0;

  pdisp = pdisp_arg;
  pdisp->AddRef(); 

  ptinfo = ptinfo_arg;

  if(ptinfo)
  {
    // gets ITypeComp interface. If it's not
    // available, frees typeinfo, as we only
    // use ITypeComp

    hr = ptinfo->GetTypeComp(&ptcomp);

    if(SUCCEEDED(hr))
    {
      ptinfo->AddRef();

      // tries to get typecomp for type library
      // (useful to bind to constants)

      ITypeLib* ptlib = NULL;
      unsigned int dumb = 0;

      hr = ptinfo->GetContainingTypeLib(&ptlib, &dumb);

      if(SUCCEEDED(hr))
      {
        ptlib->GetTypeComp(&plib_tcomp);
        COM_RELEASE(ptlib);
      }
    }
    else
    {
      ptinfo->Release();
      ptinfo = NULL;
    }
  }

  typehandler = new tLuaCOMTypeHandler(ptinfo_arg, lbeans);

  // cleans FuncInfo array
  int i;
  for(i = 0; i < MAX_FUNCINFOS; i++)
    pFuncInfo[i].name = NULL;

  ID = tLuaCOM::NEXT_ID++;

#ifdef VERBOSE
  {
    char msg[100];
    lua_Debug ar;
    lua_State* L = lbeans->getLuaState();

    if(lua_getstack(L, 1, &ar))
    {
      lua_getinfo(L, "lnS", &ar);

      sprintf(msg, "%.4d:created:LUAINFO=(%s,%d,%s,%s)", ID, ar.short_src,
        ar.currentline, ar.name, ar.what);
    }
    else if(lua_getstack(L, 0, &ar))
    {
      lua_getinfo(L, "lnS", &ar);
      sprintf(msg, "%.4d:created:LUAINFO=(%s,%d,%s,%s)", ID, ar.short_src,
        ar.currentline, ar.name, ar.what);
    }
    else
    {
      sprintf(msg, "%.4d:created:LUAINFO=(not available)", ID);
    }

    tUtil::log_verbose("tLuaCOM",msg);
  }
#endif
}

tLuaCOM::~tLuaCOM()
{
  if(connection_point != NULL)
    releaseConnection();

  // frees funcinfos
  {
    long counter = 0;

    while(pFuncInfo[counter].name != NULL &&
      counter < MAX_FUNCINFOS)
    {
      free(pFuncInfo[counter].name);
      pFuncInfo[counter].name = NULL;

      ReleaseFuncDesc(pFuncInfo[counter].propget);
      ReleaseFuncDesc(pFuncInfo[counter].propput);
      ReleaseFuncDesc(pFuncInfo[counter].func);

      counter++;
    }
  }


  COM_RELEASE(ptcomp);
  COM_RELEASE(plib_tcomp);
  COM_RELEASE(ptinfo);
  COM_RELEASE(pdisp); 


  delete typehandler;
  typehandler = NULL;

  tUtil::log_verbose("tLuaCOM", "%.4d:destroyed", ID);
}

bool tLuaCOM::getFUNCDESC(const char *name, FuncInfo& funcinfo)
{
  // First, tries to see we have the FUNCDESC's cached

  long counter = 0;

  for(counter = 0; counter < MAX_FUNCINFOS; counter++)
  {
    // when .name is NULL, there is no further information
    if(pFuncInfo[counter].name == NULL)
      break;

    if(strcmp(name, pFuncInfo[counter].name) == 0)
      break;
  }

  // checks whether funcinfo was found
  if(counter < MAX_FUNCINFOS && pFuncInfo[counter].name != NULL)
  {
    funcinfo = pFuncInfo[counter];
    return true;
  }

  // did not find, so gets type information through
  // ITypeComp

  HRESULT hr = S_OK;
  BINDPTR bindptr;
  DESCKIND desckind;
  BSTR wName;
  ITypeInfo *info = NULL;

  unsigned int dumb = 0;

  wName = tUtil::string2bstr(name);

  unsigned long lhashval = LHashValOfName(LOCALE_SYSTEM_DEFAULT, wName);

  hr = ptcomp->Bind(wName, lhashval, INVOKE_PROPERTYGET,
    &info, &desckind, &bindptr);
  
  if(FAILED(hr) || desckind == DESCKIND_NONE)
    funcinfo.propget = NULL;
  else
  {
    funcinfo.propget = bindptr.lpfuncdesc;
    info->Release();
  }

  hr = ptcomp->Bind(wName, lhashval, INVOKE_FUNC,
    &info, &desckind, &bindptr);
  
  if(FAILED(hr) || desckind == DESCKIND_NONE)
    funcinfo.func = NULL;
  else
  {
    funcinfo.func = bindptr.lpfuncdesc;
    info->Release();
  }


  hr = ptcomp->Bind(wName, lhashval, INVOKE_PROPERTYPUT,
    &info, &desckind, &bindptr);
  
  if(FAILED(hr) || desckind == DESCKIND_NONE)
    funcinfo.propput = NULL;
  else
  {
    funcinfo.propput = bindptr.lpfuncdesc;
    info->Release();
  }

  // if there is not propertyput, then tries propputref

  if(funcinfo.propput == NULL)
  {
    hr = ptcomp->Bind(wName, lhashval, INVOKE_PROPERTYPUTREF,
      &info, &desckind, &bindptr);

    if(FAILED(hr) || desckind == DESCKIND_NONE)
      funcinfo.propput = NULL;
    else
    {
      funcinfo.propput = bindptr.lpfuncdesc;
      info->Release();
    }
  }

  SysFreeString(wName);

  // If no type information found, returns NULL
  if(!funcinfo.propget && !funcinfo.propput && !funcinfo.func)
    return false;
  else if(counter < MAX_FUNCINFOS)
  {
    CHECKPRECOND(pFuncInfo[counter].name == NULL);

    pFuncInfo[counter].name = tUtil::strdup(name);

    pFuncInfo[counter].propget  = funcinfo.propget;
    pFuncInfo[counter].propput  = funcinfo.propput;
    pFuncInfo[counter].func     = funcinfo.func;

    return true;
  }
  else
    return true;
}


//
// Tries to find a constant in the type library and
// pushes its value to Lua
//
bool tLuaCOM::getConstant(const char* name)
{
  if(plib_tcomp == NULL)
    return false;

  HRESULT hr = S_OK;
  BINDPTR bindptr;
  DESCKIND desckind;
  BSTR wName;
  ITypeInfo *info = NULL;
  bool result = false;

  unsigned int dumb = 0;

  wName = tUtil::string2bstr(name);

  unsigned long lhashval = LHashValOfName(LOCALE_SYSTEM_DEFAULT, wName);

  hr = plib_tcomp->Bind(wName, lhashval, INVOKE_PROPERTYGET,
    &info, &desckind, &bindptr);

  SysFreeString(wName);

  if(SUCCEEDED(hr) 
    && desckind == DESCKIND_VARDESC 
    && bindptr.lpvardesc->varkind == VAR_CONST)
  {
    typehandler->com2lua(*bindptr.lpvardesc->lpvarValue);
    result = true;
  }
  else
    result = false;

  COM_RELEASE(info);

  return result;
}

bool tLuaCOM::getDISPID(const char* name, DISPID* dispid)
{
   HRESULT hr;
   wchar_t* w_name = (wchar_t*) malloc( (strlen(name) + 1) * sizeof(wchar_t));
   mbstowcs(w_name,name,strlen(name)+1);

   hr = pdisp->GetIDsOfNames(IID_NULL, &w_name, 1,
                          LOCALE_SYSTEM_DEFAULT,dispid);
   free(w_name);
   
   return SUCCEEDED(hr);
}

int tLuaCOM::call(DISPID dispid,
                  int invkind,
                  FUNCDESC *pfuncdesc,
                  tLuaObjList& params)
{ 
  tUtil::log_verbose("tLuaCOM.call", "about to call DISPID 0x%.8x", dispid);

   HRESULT hr             = 0;
   unsigned int i         = 0;
   UINT ArgErr            = 0;
   
   // number of return values (required to interface with Lua)
   int num_retvals        = 0;

   DISPPARAMS dispparams;
   VARIANTARG result;
   EXCEPINFO excepinfo;
      
   VariantInit(&result);

   // fills DISPPARAMS structure, converting lua arguments
   // to COM parameters
   typehandler->fillDispParams(dispparams, pfuncdesc, params, invkind);

   hr = pdisp->Invoke(
     dispid,
     IID_NULL, 
     LOCALE_SYSTEM_DEFAULT,
     invkind,
     &dispparams,
     &result,
     &excepinfo,
     &ArgErr);

   if(SUCCEEDED(hr))
   {
     try
     {
       // pushes first return value (if any)
       // if there is no type information, assumes that
       // all functions have return value
       if((pfuncdesc && pfuncdesc->elemdescFunc.tdesc.vt != VT_VOID) ||
           !pfuncdesc)
       {
        // To ease programming, methods of untyped objects
        // always return values to Lua. If the COM method does
        // not return nothing, then LuaCOM pushes nil.
        // (This avoids confusion with out params)

         typehandler->com2lua(result);
         num_retvals++;
       }

       // pushes out values
       if(invkind & INVOKE_FUNC)
         num_retvals += typehandler->pushOutValues(dispparams);
     }
     catch(class tLuaCOMException& e)
     {
       UNUSED(e);

       typehandler->releaseVariants(&dispparams);
       VariantClear(&result);

       throw;
     }

     typehandler->releaseVariants(&dispparams);
     VariantClear(&result);

     return num_retvals;
   }
   else // Houve erro
   {
     // Limpa parametros
     typehandler->releaseVariants(&dispparams);

     if(hr == DISP_E_EXCEPTION) // excecoes
     {
       if(excepinfo.bstrDescription != NULL)
         COM_EXCEPTION(tUtil::bstr2string(excepinfo.bstrDescription));
       else if(excepinfo.wCode != 0)
         COM_EXCEPTION(tUtil::GetErrorMessage(excepinfo.wCode));
       else if(excepinfo.scode != 0)
         COM_EXCEPTION(tUtil::GetErrorMessage(excepinfo.scode));
       else
         COM_EXCEPTION("Unknown exception");
     }
     else // Erros comuns
       COM_ERROR(tUtil::GetErrorMessage(hr));
   }
}

bool tLuaCOM::addConnection(tLuaCOM *server)
{
  if(!server->hasTypeInfo())
    return false;

  HRESULT hr;
  IDispatch *pdisp_server = server->GetIDispatch();

  IConnectionPointContainer *pcpc = NULL;

  if(connection_point != NULL)
    return false;

  hr = pdisp->QueryInterface
    (
      IID_IConnectionPointContainer, (void **) &pcpc
    );

  if(FAILED(hr))
  {
    return false;
  }

  {
    IID guid;

    server->GetIID(&guid);

    hr = pcpc->FindConnectionPoint(guid, &connection_point);
  }

  pcpc->Release();
  pcpc = NULL;

  if(FAILED(hr))
  {
    return false;
  }

  hr = connection_point->Advise
    (
      (IUnknown *) pdisp_server, &connection_point_cookie
    );

  if(FAILED(hr))
  {
    connection_point->Release();
    connection_point = NULL;

    return false;
  }

  return true;
}

void tLuaCOM::releaseConnection(void)
{
  CHK_LCOM_ERR(connection_point, "No connections established.");

  connection_point->Unadvise(connection_point_cookie);
  connection_point->Release();
  connection_point = NULL;
}

//
//  isMember
//
//    Informa se existe algum metodo ou propriedade com
//    o nome passado como parametro
//

bool tLuaCOM::isMember(const char * name)
{
  HRESULT hr;
  DISPID dumb_dispid;
  
  wchar_t* w_name = (wchar_t*) malloc( (strlen(name) + 1) * sizeof(wchar_t));

  assert(w_name);
  if(!w_name)
    return false;

  mbstowcs(w_name, name, strlen(name)+1);

  hr = pdisp->GetIDsOfNames(IID_NULL, &w_name, 1,
                        LOCALE_SYSTEM_DEFAULT, &dumb_dispid);
  free(w_name);
  w_name = NULL;

  if(!FAILED(hr))
    return true;
  else
    return false;
}


void tLuaCOM::getHelpInfo(char **ppHelpFile, unsigned long *pHelpContext)
{
  if(!hasTypeInfo())
  {
    *ppHelpFile = NULL;
    return;
  }


  ITypeLib *typelib = NULL;
  BSTR helpfile;
  HRESULT hr = S_OK;
  
  hr = ptinfo->GetDocumentation(-1, NULL, NULL, pHelpContext, &helpfile);

  if(FAILED(hr) || helpfile == NULL)
  {
    *ppHelpFile = NULL;
    return;
  }

  // Se nao conseguiu help contextna propria interface, tenta obte-lo
  // na type library
  if(*pHelpContext == 0)
  {
    unsigned int dumb_index = 0;
    unsigned long typelib_help_context = 0;
    BSTR helpfile_typelib;

    hr = ptinfo->GetContainingTypeLib(&typelib, &dumb_index);

    if(!FAILED(hr))
    {
      hr = typelib->GetDocumentation(-1, NULL, NULL,
        &typelib_help_context, &helpfile_typelib);

      if(!FAILED(hr))
      {
        SysFreeString(helpfile);

        helpfile = helpfile_typelib;
        *pHelpContext = typelib_help_context;
      }
    }
  }

  int str_size = SysStringLen(helpfile);
  *ppHelpFile = (char *) malloc((str_size + 1)*sizeof(wchar_t));
  wcstombs(*ppHelpFile, helpfile, str_size+1);

  SysFreeString(helpfile);
}



//
// CreateLuaCOM
//


tLuaCOM * tLuaCOM::CreateLuaCOM(IDispatch * pdisp,
                                LuaBeans *lbeans,
                                const CLSID& coclass,
                                ITypeInfo* typeinfo,
                                bool untyped
                                )
{
  HRESULT hr = S_OK;

  CHECKPARAM(pdisp && lbeans);
  CHECKPARAM(!untyped || !typeinfo);

  if(!untyped)
  {
    if(!typeinfo)
      typeinfo = tCOMUtil::GetDispatchTypeInfo(pdisp);
    else
      typeinfo->AddRef();
  }

  tLuaCOM *lcom = 
    new tLuaCOM(pdisp, typeinfo, lbeans, coclass);

  COM_RELEASE(typeinfo);

  // We have one reference (the pointer), so we lock the object
  lcom->Lock();

  return lcom;
}

ITypeInfo * tLuaCOM::GetDefaultEventsInterface()
{
  CLSID clsid = GetCLSID();

  if(clsid == IID_NULL)
    return NULL;
  
  ITypeInfo* coclassinfo = tCOMUtil::GetCoClassTypeInfo(pdisp, clsid);

  if(!coclassinfo)
    return NULL;

  return tCOMUtil::GetDefaultInterfaceTypeInfo(coclassinfo, true);
}

void tLuaCOM::ReleaseFuncDesc(FUNCDESC * pfuncdesc)
{
  CHECKPRECOND(ptinfo);

  if(pfuncdesc)
    ptinfo->ReleaseFuncDesc(pfuncdesc);
}

IDispatch * tLuaCOM::GetIDispatch()
{
  return pdisp;
}

void tLuaCOM::GetIID(IID * piid)
{
  CHECKPRECOND(ptinfo);

  TYPEATTR *ptypeattr;

  ptinfo->GetTypeAttr(&ptypeattr);
  *piid = ptypeattr->guid;
  ptinfo->ReleaseTypeAttr(ptypeattr);
}


CLSID tLuaCOM::GetCLSID()
{
  HRESULT hr = S_OK;

  if(clsid != IID_NULL)
    return clsid;

  // tries to find the CLSID using IProvideClassInfo
  ITypeInfo* coclassinfo = tCOMUtil::GetCoClassTypeInfo(pdisp);

  if(coclassinfo)
  {
    clsid = tCOMUtil::GetCLSID(coclassinfo);
    COM_RELEASE(coclassinfo);

    if(clsid != IID_NULL)
      return clsid;
  }

  // Now searches the type library seeking the coclass to which
  // this interface belongs

  clsid = tCOMUtil::FindCLSID(ptinfo);

  return clsid;
}


ITypeInfo* tLuaCOM::GetTypeInfo()
{
  return ptinfo;
}

int tLuaCOM::Lock()
{
  return ++lock_count;
}

int tLuaCOM::Unlock()
{
  if(--lock_count == 0)
  {
    delete this;
    return 0;
  }
  else
    return lock_count;
}


bool tLuaCOM::hasTypeInfo(void)
{
  if(ptinfo)
    return true;
  else
    return false;
}


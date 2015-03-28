// tLuaCOMClassFactory.cpp: implementation of the tLuaCOMClassFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "tLuaCOMClassFactory.h"
#include "tLuaCOMException.h"
#include "tUtil.h"


STDMETHODIMP
tLuaCOMClassFactory::QueryInterface(REFIID iid, void** ppv) 
{   
  *ppv = NULL;

  if (iid == IID_IUnknown || iid == IID_IClassFactory)
    *ppv = this;
  else 
    return E_NOINTERFACE; 

  AddRef();
  return NOERROR;    
}


STDMETHODIMP_(ULONG)
tLuaCOMClassFactory::AddRef(void)
{ 
  return ++m_cRef;
}


STDMETHODIMP_(ULONG)
tLuaCOMClassFactory::Release(void)
{ 
  if(--m_cRef == 0)
  {
    delete this;
    return 0;
  }

  return m_cRef;
}

/*
 * tLuaCOMClassFactory::CreateInstance, LockServer
 *
 * Purpose:
 *  Implements IClassFactory::CreateInstance, LockServer
 *
 */
STDMETHODIMP
tLuaCOMClassFactory::CreateInstance(IUnknown* punkOuter,
                         REFIID riid,
                         void** ppv)
{
  HRESULT hr = E_NOTIMPL;
    
  *ppv = NULL;
    
  // This implementation does'nt allow aggregation
  if (punkOuter)
    return CLASS_E_NOAGGREGATION;
    
  // This is REGCLS_SINGLEUSE class factory, so CreateInstance will be
  // called atmost once. An application objects has a REGCLS_SINGLEUSE class
  // factory. The global application object has already been created when 
  // CreateInstance is called. A REGCLS_MULTIPLEUSE class factory's 
  // CreateInstance would be called multiple times and would create a new 
  // object each time. An MDI application would have a REGCLS_MULTIPLEUSE 
   // class factory for it's document objects.
  hr = object->QueryInterface(riid, ppv); 

  return hr;
}

STDMETHODIMP
tLuaCOMClassFactory::LockServer(BOOL fLock)
{    
  CoLockObjectExternal(object, fLock, TRUE); 
  return NOERROR;       
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

tLuaCOMClassFactory::tLuaCOMClassFactory(IDispatch* object_arg)
{
  CHECKPARAM(object_arg);

  m_cRef = 0;

  object = object_arg;
  object->AddRef();
}

tLuaCOMClassFactory::~tLuaCOMClassFactory()
{
  object->Release();
}

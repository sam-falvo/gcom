#include "IClassFactory_impl.h"
#include "IDisplayText_impl.h"
#include "CLSID_Example1.h"

#include <stdlib.h>

/************************************************************************/
/* Prototypes								*/
/************************************************************************/

static HRESULT QueryInterface( IClassFactory *, REFIID, void ** );
static uint32 AddRef( IClassFactory * );
static uint32 Release( IClassFactory * );
static HRESULT CreateInstance(
			      IClassFactory *,
			      const IUnknown *,
			      REFIID,
			      void **
			     );
static HRESULT LockServer( IClassFactory *, Bool );

extern void InitializeDisplayTextObject( IDisplayText_impl * );

/************************************************************************/
/* Static Structure Definitions						*/
/************************************************************************/

static IClassFactoryVtbl vtbl =
{
   &QueryInterface,
   &AddRef,
   &Release,
   &CreateInstance,
   &LockServer
};

IClassFactory_impl g_ClassFactory =
{
   &vtbl,
   0,
   0,
   0
};

/************************************************************************/
/* IClassFactory Method Implementations					*/
/************************************************************************/

		       /* IUnknown required methods */

static HRESULT QueryInterface(
			      IClassFactory *pcf,
			      REFIID riid,
			      void **ppvResult
			     )
{
   HRESULT hr = E_NOINTERFACE;
   
   *ppvResult = NULL;
   
   if( IsEqualIID( IID_IUnknown, riid ) ||
       IsEqualIID( IID_IClassFactory, riid ) )
   {
      *ppvResult = (void *)pcf;
      AddRef( pcf );
      hr = S_OK;
   }
	   
   return hr;
}
	
static uint32 AddRef( IClassFactory *pcf )
{
   (((IClassFactory_impl *)pcf) -> referenceCount)++;
   return 1;
}
	
static uint32 Release( IClassFactory *pcf )
{
   IClassFactory_impl *self = (IClassFactory_impl *)pcf;
   
   if( self -> referenceCount != 0 )
   	self -> referenceCount--;
}

			 /* IClassFactory methods */

static HRESULT CreateInstance(
			      IClassFactory *pcf,
			      const IUnknown *punkOuter,
			      REFIID riid,
			      void **ppvResult
			     )
{
   HRESULT hr = E_OUTOFMEMORY;
   IDisplayText_impl *pdtImpl;

   *ppvResult = NULL;

   /*
    * Aggregation is discussed in another tutorial.  For now,
    * just to keep things simple, we don't support aggregation.
    */
   
   if( punkOuter != NULL )
   	return E_NOAGGREGATION;

   pdtImpl = (IDisplayText_impl *)
   	     ( malloc( sizeof( IDisplayText_impl ) ) );
   if( pdtImpl != NULL )
   {
      InitializeDisplayTextObject( pdtImpl );
      hr = pdtImpl -> lpVtbl ->
      		QueryInterface(
			       (IDisplayText *)pdtImpl,
			       riid,
			       ppvResult
			      );
      if( FAILED(hr) )
         pdtImpl -> lpVtbl -> Release( (IDisplayText *)pdtImpl );
      else
         g_ClassFactory.objectCount++;
   }
  
   return hr;
}
	
static HRESULT LockServer( IClassFactory *pcf, Bool reallyLock )
{
   IClassFactory_impl *self = (IClassFactory_impl *)pcf;

   if( reallyLock == TRUE )
      g_ClassFactory.lockCount++;
   else
   {
      if( g_ClassFactory.lockCount != 0 )
         g_ClassFactory.lockCount--;
   }

   return S_OK;
}

/************************************************************************/
/* Component Library's Interface to the COM Run-time Environment	*/
/************************************************************************/

HRESULT DllCanUnloadNow( void )
{
   if( g_ClassFactory.objectCount != 0 )
   	return S_FALSE;

   if( g_ClassFactory.referenceCount != 0 )
   	return S_FALSE;

   if( g_ClassFactory.lockCount != 0 )
   	return S_FALSE;

   return S_OK;
}

HRESULT DllGetClassObject(
			  REFCLSID rclsid,
			  REFIID riid,
			  void **ppvResult
			 )
{
   HRESULT hr = E_CLASSNOTREG;
   
   if( IsEqualIID( CLSID_Example1, rclsid ) )
   {
      hr = QueryInterface(
      			  (IClassFactory *)&g_ClassFactory,
			  riid,
			  ppvResult
			 );
   }
   
   return hr;
}

#include "IDisplayText_impl.h"
#include "IClassFactory_impl.h"

#include <stdio.h>
#include <stdlib.h>

/************************************************************************/
/* Prototypes								*/
/************************************************************************/

static HRESULT QueryInterface( IDisplayText *, REFIID, void ** );
static uint32 AddRef( IDisplayText * );
static uint32 Release( IDisplayText * );
static HRESULT SetText( IDisplayText *, char *, uint32 );
static HRESULT GetText( IDisplayText *, char *, uint32 );
static HRESULT Display( IDisplayText * );

/************************************************************************/
/* Static Data Structures						*/
/************************************************************************/

static IDisplayTextVtbl vtbl =
{
   &QueryInterface,
   &AddRef,
   &Release,
   &SetText,
   &GetText,
   &Display
};

/************************************************************************/
/* Object Initialization -- called by class factory object!		*/
/************************************************************************/

void InitializeDisplayTextObject( IDisplayText_impl *pdtImpl )
{
   memset( pdtImpl, 0, sizeof( IDisplayText_impl ) );
   pdtImpl -> lpVtbl = &vtbl;
}

/************************************************************************/
/* IDisplayText Methods							*/
/************************************************************************/

		       /* IUnknown required methods */

static HRESULT QueryInterface(
			      IDisplayText *pdt,
			      REFIID riid,
			      void **ppvResult
			     )
{
   HRESULT hr = E_NOINTERFACE;

   *ppvResult = NULL;
   
   if( IsEqualIID( IID_IUnknown, riid ) ||
       IsEqualIID( IID_IDisplayText, riid ) )
   {
      *ppvResult = (void *)pdt;
      AddRef( pdt );
      hr = S_OK;
   }
	   
   return hr;
}

static uint32 AddRef( IDisplayText *pdt )
{
   (((IDisplayText_impl *)pdt) -> referenceCount)++;
   return 1;
}

static uint32 Release( IDisplayText *pdt )
{
   IDisplayText_impl *self = (IDisplayText_impl *)pdt;
   extern IClassFactory_impl g_ClassFactory;

   if( self -> referenceCount != 0 )
   	self -> referenceCount--;

   if( self -> referenceCount == 0 )
   {
      free( self );	/* Allocated by malloc() in class factory */
      g_ClassFactory.objectCount--;
   }
	   
   return 0;
}

		     /* IDisplayText-specific methods */

static HRESULT SetText( IDisplayText *pdt, char *buf, uint32 size )
{
   HRESULT hr = S_OK;
   IDisplayText_impl *self = (IDisplayText_impl *)pdt;

   if( size > 128 )
   {
      hr = S_FALSE;	/* Just letting the client know... */
      size = 128;	/* that his buffer was too big to fit. */
   }

   strncpy( self -> buffer, buf, size );
   self -> buffer[127] = 0;	/* Just in case... */

   return hr;
}

static HRESULT GetText( IDisplayText *pdt, char *buf, uint32 size )
{
   HRESULT hr = S_OK;
   IDisplayText_impl *self = (IDisplayText_impl *)pdt;
   uint32 realSize = strlen( self -> buffer );
	   
   if( realSize > size )
   {
      realSize = size;
      hr = S_FALSE;
   }
	   
   strncpy( buf, self -> buffer, realSize );
   return hr;
}

static HRESULT Display( IDisplayText *pdt )
{
   IDisplayText_impl *self = (IDisplayText_impl *)pdt;
	   
   printf(
   	  "IDisplayText::Display() says, \"%s\"\n",
	  self -> buffer
	 );
   return S_OK;
}

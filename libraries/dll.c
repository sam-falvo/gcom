/*
 * dll.c
 * GCOM Release 0.3
 * 
 * Copyright (c) 1999, 2000 Samuel A. Falvo II
 *
 * This software is provided 'as-is', without any implied or express warranty.
 * In no event shall the authors be held liable for damages arising from the
 * use this software.
 *
 * Permission is granted for anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in a
 *    product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 
 * 3. This notice may not be removed or altered from any source
 *    distribution.
 */

#include <gcom/gcom.h>
#include <util/lists.h>
#include "gcom-config.h"

/************************************************************************/
/* Internal data structures needed to keep track of which DLLs we've	*/
/* loaded.								*/
/************************************************************************/

static uint32 initCount = 0;
static List libraryList;

typedef struct
{
   Node		node;
   void *	pDLL;
   wchar *	name;
   uint32	loadCount;
} LibNode;

/**
 * This function creates a new LibNode structure.  This structure is used
 * to remember which libraries have already been loaded by the GCOM library.
 * Although the native OS already does something similar, GCOM needs
 * to keep track of its loaded libraries to prevent duplicating its
 * internal data structures needlessly.
 * 
 * @param name
 * The Unicode path and filename of the library to load.
 * 
 * @param ppln
 * Pointer to the LibNode pointer that will hold the resulting structure.
 * 
 * @results
 * S_OK if all went well; E_OUTOFMEMORY otherwise.
 */

HRESULT NewLibNodeFromName( wchar *name, LibNode **ppln )
{
   HRESULT hr;
   LibNode *pln;

   *ppln = NULL;
   
   pln = CoTaskMemAlloc( sizeof( LibNode ) );
   if( pln != NULL )
   {
      pln -> pDLL = NULL;
      pln -> name = NULL;
      pln -> loadCount = 0;
      
      hr = gCoUnicodeStringDuplicate( name, &pln -> name );
      if( SUCCEEDED( hr ) )
      {
	 *ppln = pln;
	 return S_OK;
      }
      
      CoTaskMemFree( pln );
   }

   return E_OUTOFMEMORY;
}

/**
 * This function disposes of a LibNode structure.
 * 
 * @param pln
 * Pointer to the LibNode to deallocate.
 *
 * @returns Nothing.
 */

void DisposeLibNode( LibNode *pln )
{
   if( pln -> name )
		   CoTaskMemFree( pln -> name );
   
   CoTaskMemFree( pln );
}

/************************************************************************/
/* Coherency helpers to aid in thread safety of this code.		*/
/************************************************************************/

void LockLibList( void )
{
#warning Insert locking code to make this thread safe.
}

void UnlockLibList( void )
{
#warning Insert unlocking code to make this thread safe.
}

/************************************************************************/
/* Initialization Functions						*/
/************************************************************************/

HRESULT DLLInitialize( void )
{
   /* How do we make this thread safe? */
   initCount++;
   
   if( initCount == 1 )
   {
      ListInitialize( &libraryList );

      return S_OK;
   }
   
   return S_FALSE;
}

HRESULT DLLUninitialize( void )
{
   if( initCount != 0 )
	initCount--;

   return S_OK;
}

/************************************************************************/
/* GCOM Services used by higher level code.				*/
/************************************************************************/

/**
 * This function is internal to the dll.c GCOM source module.  Its
 * job is to determine if we've already loaded the specified DLL.
 * Although the native OS can (hopefully) handle multiple library
 * opens, without this function we'd be occupying memory with each
 * gCoLoadDLL() function call.
 * 
 * The string comparison used is implementation defined.  For UNIX,
 * it's case sensitive.  For Microsoft/Amiga operating systems,
 * it's case insensitive.  It all depends on the underlying filesystem.
 * 
 * @param libName
 * Unicode string of the library's filename, including path.
 * 
 * @param pln
 * Pointer to a LibNode structure.  See dll.c for more info on that
 * structure.
 * 
 * @results
 * S_OK if we've found a library; S_FALSE if not.
 */

static HRESULT gCoFindDLL( wchar *dllName, LibNode **ppln )
{
   LibNode *pln;
   HRESULT result = S_FALSE;
   
   LockLibList();
   *ppln = NULL;
   
   for(
       pln = (LibNode *)( libraryList.head );
       ( result == S_FALSE ) && ( pln -> node.next );
       pln = (LibNode *)( pln -> node.next )
      )
   {
      if( gCoUnicodeStringCompare( dllName, pln -> name ) == 0 )
      {
	 result = S_OK;
	 *ppln = pln;
      }
   }
   UnlockLibList();
   
   return result;
}

/**
 * This function loads the indicated library into memory for use.
 * The function GCOMDLLInit() is called within the DLL to ensure that
 * it is properly initialized.  GCOMDLLInit() is called once, and once
 * only, for the lifetime of the library.  GCOMDLLInit() must return
 * a successful HRESULT code in order to let GCOM use the library.
 * 
 * @param libName
 * The name of the library to load, expressed as a NULL-terminated,
 * Unicode string.  This parameter must not be NULL.
 * 
 * @param phDLL
 * This is a pointer to a variable of type HDLL, which is used to
 * hold the handle to the library.  This parameter must not be NULL.
 * 
 * @returns
 * S_OK if everything worked perfectly.  E_INVALIDARG is returned if
 * there was some kind of problem with converting the Unicode filename
 * into the native OS' representation.  E_DLLNOTFOUND is returned if
 * the library couldn't be loaded for some reason.  Otherwise, it will
 * return anything the GCOMDLLInit() function returns.
 *
 * @see gCoUnloadDLL
 * @see gCoGetDLLSymbol
 */

HRESULT gCoLoadDLL( wchar *libName, HDLL *phdll )
{
   HRESULT hr;
   char asciiLibName[MAX_PATH_LEN];
   LibNode *pln;

   *phdll = (HDLL)0;

   hr = gCoFindDLL( libName, &pln );
   if( hr == S_OK )
   {
      *phdll = (HDLL)pln;
      pln -> loadCount++;
      hr = S_OK;
   }
   else
   {
      hr = gCoUnicodeStringToAscii( libName, asciiLibName, MAX_PATH_LEN );
      if( SUCCEEDED( hr ) )
      {
         hr = NewLibNodeFromName( libName, &pln );
         if( SUCCEEDED( hr ) )
         {
	    LockLibList();

	    pln -> pDLL = dlopen( asciiLibName, RTLD_LAZY );
	    if( dlerror() == NULL )
	    {
	       ListAddTail( &libraryList, (Node *)pln );
	       *phdll = (HDLL)pln;
	       pln -> loadCount = 1;

	       hr = gCoGCOMDLLInit( (HDLL)pln );
	       if( FAILED( hr ) )
	       {
		  dlclose( pln -> pDLL );
		  DisposeLibNode( pln );
	       }
	    }
	    else
	    {
	       hr = E_DLLNOTFOUND;
	       DisposeLibNode( pln );
	    }
	       
	    UnlockLibList();
	 }
      }
   }

   return hr;
}

/**
 * This function forcibly unloads the indicated DLL, *even if it's not
 * ready to be unloaded!*  Use this function with extreme care.
 * The DLL's GCOMDLLExpunge() is called automatically in the process.
 * 
 * @param hdll
 * The handle to the library as returned by a previous call of
 * gCoLoadDLL().  This parameter cannot be zero or NULL.
 * 
 * @returns
 * Currently, this function always returns S_OK.  However, it is unwise
 * to depend on this behavior.  Future versions of this function
 * may refuse to unload the library if it's not ready to be unloaded.
 * 
 * @see gCoLoadDLL
 */

HRESULT gCoUnloadDLL( HDLL hdll )
{
   LibNode *pln = (LibNode *)hdll;
   
   LockLibList();

   if( pln -> loadCount > 0 )
		   pln -> loadCount--;

   if( pln -> loadCount == 0 )
   {
      gCoGCOMDLLExpunge( hdll );
      NodeRemove( (Node *)pln );
      if( pln -> pDLL )		dlclose( pln -> pDLL );
      DisposeLibNode( pln );
   }

   UnlockLibList();
   return S_OK;
}

/**
 * This function queries a DLL for an exposed symbol name.
 * Some operating systems do not support this function,
 * especially those which use static jump-tables (such as AmigaOS
 * and the original a.out libraries under Unix).
 * 
 * @param hdll
 * Handle to the DLL to query.  This is as returned by gCoLoadDLL().
 * 
 * @param symbol
 * The Unicode string containing the symbol name to query for.
 * 
 * @param ppv
 * A pointer to a variable which will point to the symbol requested.
 * 
 * @returns
 * S_OK if the symbol is exported and publicly available.
 * E_NOINTERFACE if the symbol is not supported by the DLL.
 * E_NOTSUPPORTED if the DLL doesn't support this operation.
 */

HRESULT gCoGetDLLSymbol( HDLL hdll, wchar *symbol, void **ppv )
{
   HRESULT hr;
   LibNode *pln = (LibNode *)hdll;
   char asciiSymbol[ MAX_SYMBOL_LEN ];

   hr = gCoUnicodeStringToAscii( symbol, asciiSymbol, MAX_SYMBOL_LEN );
   if( SUCCEEDED( hr ) )
   {
      LockLibList();	/* Because dlsym() isn't thread safe */
      *ppv = dlsym( pln -> pDLL, asciiSymbol );
      if( dlerror() == NULL )
		      hr = S_OK;
      else
		      hr = E_NOINTERFACE;
      UnlockLibList();
   }
   
   return hr;
}

/**
 * This function attempts to return a COM class object of some kind
 * from the indicated DLL.  This DLL can be an inproc server or handler.
 * 
 * @param hdll
 * The handle to the library which presumably contains the class object
 * desired.  This is as returned from gCoLoadDLL().
 * 
 * @param rclsid
 * The CLSID of the class object to return.
 * 
 * @param riid
 * The IID of the class object to return.  Normally, this would be
 * IID_IClassFactory, but it need not be.
 * 
 * @param ppv
 * The address of the variable holding the interface pointer requested.
 * 
 * @returns
 * S_OK if everything went OK.
 * 
 * E_NOINTERFACE if the class object doesn't support the requested interface.
 * 
 * E_OUTOFMEMORY if no memory is available to instantiate the class object.
 * 
 * E_CLASSNOTREG if the class is not recognized by the DLL's
 * DllGetClassObject() function.
 * 
 * E_SYMBOLNOTFOUND if the DLL doesn't export the DllGetClassObject()
 * function.
 * 
 * Any other error returned by the DLL's implementation of DllGetClassObject.
 */

HRESULT gCoDLLGetClassObject(
			     HDLL hdll,
			     REFCLSID rclsid,
			     REFIID riid,
			     void **ppv
			    )
{
   HRESULT hr;
   HRESULT (*getClassObject)( REFCLSID, REFIID, void ** );
   
   *ppv = NULL;		/* Just in case... */

   hr = gCoGetDLLSymbol( hdll, L"DllGetClassObject", (void *)&getClassObject );
   if( SUCCEEDED( hr ) )
   {
      hr = (*getClassObject)( rclsid, riid, ppv );
   }
   else
   {
      hr = E_SYMBOLNOTFOUND;
   }
   
   return hr;
}

/**
 * This function calls the DLL's implementation of
 * DllCanUnloadNow().
 * 
 * @param hdll
 * The handle to the DLL to query.
 * 
 * @returns
 * S_OK if the DLL is safe to unload.  Otherwise, S_FALSE is
 * returned.  If the DLL doesn't export the DllCanUnloadNow()
 * function, S_FALSE is returned, and the library will be unloaded
 * when the COM library uninitializes (as per existing COM specifications).
 */

HRESULT gCoDLLCanUnloadNow( HDLL hdll )
{
   HRESULT hr;
   HRESULT (*canUnloadNow)( void );
   
   hr = gCoGetDLLSymbol( hdll, L"DllCanUnloadNow", (void *)&canUnloadNow );
   if( SUCCEEDED( hr ) )
   {
      hr = (*canUnloadNow)();
   }
   else
   {
      hr = S_FALSE;
   }
   
   return hr;
}

/**
 * Invokes the library's GCOMDLLInit() function for library initialization.
 * If this function isn't exported by the library, then it assumes a
 * successful initialization.
 * 
 * @param hdll
 * The handle to the library to initialize.
 * 
 * @returns
 * Any successful HRESULT code if the library initialized successfully.
 * Otherwise, any failure code is returned.
 * 
 * If the library doesn't expose the GCOMDLLInit() function, then
 * S_OK is returned, assuming the library doesn't need any initialization.
 */

HRESULT gCoGCOMDLLInit( HDLL hdll )
{
   HRESULT hr;
   HRESULT (*init)( void );

   hr = gCoGetDLLSymbol( hdll, WSTR_DLLINIT, (void *)&init );
   if( SUCCEEDED( hr ) )
   {
      hr = (*init)();
   }
   else
		   hr = S_OK;
   
   return hr;
}

/**
 * Invokes the DLL's GCOMDLLExpunge() function, if it's exposed.
 * 
 * @param hdll
 * Handle to the library to attempt to cleanly shutdown.
 * 
 * @returns Nothing.
 */

void gCoGCOMDLLExpunge( HDLL hdll )
{
   HRESULT hr;
   void (*expunge)( void );
   
   hr = gCoGetDLLSymbol( hdll, WSTR_DLLEXPUNGE, (void *)&expunge );
   if( SUCCEEDED( hr ) )
		   (*expunge)();
}

/**
 * Cycles through each of the libraries loaded by GCOM, and queries
 * each of them to see if it's safe to unload them.  If it is safe to
 * unload the library, the deed is done.  Otherwise, the library remains
 * for cleanup later on.
 * 
 * @returns Nothing.
 */

void CoFreeUnusedLibraries( void )
{
   HRESULT hr;
   LibNode *pln;
   
   LockLibList();
   
   pln = (LibNode *)( libraryList.head );
   while( pln -> node.next )
   {
      hr = gCoDLLCanUnloadNow( (HDLL)pln );
      if( hr == S_OK )
         gCoUnloadDLL( (HDLL)pln );
   }
   
   UnlockLibList();
}

/*
 * init.c
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
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include "gcom-config.h"

/************************************************************************/
/* Library Private Data							*/
/************************************************************************/

typedef enum
{
   GCOMIT_SERVER,
   GCOMIT_HANDLER
} GCOMIT;

/************************************************************************/
/* Public COM Library Functions						*/
/************************************************************************/

/**
 * This function is called before any other GCOM library functions are
 * called.  This function must be called for *each thread* which is
 * running in your application.
 * 
 * @param unused
 * This parameter is reserved for future use.  Leave this argument
 * NULL.
 * 
 * @returns
 * S_OK if initialization was successful.  Otherwise, an implementation-
 * defined error code.
 */

HRESULT CoInitialize( void *___unused )
{
   HRESULT hr;

   hr = TaskMallocInitialize();
   if( SUCCEEDED( hr ) )
   {
      hr = DLLInitialize();
   }

   return hr;
}

/**
 * This function is called before exiting the application.  The number
 * of CoInitialize() function calls must be matched with the number
 * of CoUninitialize() functions.
 * 
 * @returns Nothing.
 */

void CoUninitialize( void )
{
   DLLUninitialize();
   TaskMallocUninitialize();
}

/**
 * Returns the current version of the GCOM library.
 *
 * @returns
 * A 32-bit unsigned integer, which is broken into two sub-fields.
 * Bits 31-16 contain the library's version number, and bits 15-0
 * contain the library's revision number.
 * 
 * Compatibility between library versions is guaranteed only between
 * libraries with the *same* version number (the library could hold
 * any revision).
 */

uint32 CoBuildVersion( void )
{
   return MAKE_GCOMVERSION( GCOM_CURRENT_VERSION, GCOM_CURRENT_REVISION );
}

/**
 * This obtains a class object from an in-process object
 * of some kind, as indicated in the "inprocType" parameter.
 * This function is internal to the GCOM library, and should
 * never be called by the application.
 * 
 * @param inprocType
 * This is an enumeration which contains the following
 * values:
 * 
 * GCOMIT_SERVER	Look for an in-proc server,
 * GCOMIT_HANDLER	Look for an in-proc handler.
 * 
 * @param rclsid
 * The ID of the class object to acquire.
 * 
 * @param riid
 * The interface to query from the class object.
 * 
 * @param ppv
 * Where to store the queried interface.
 * 
 * @returns
 * S_OK if everything was successful.
 *
 * @see gCoGetClassData
 */

static HRESULT gCoGetInprocClassObject(
				       GCOMIT inprocType,
				       REFCLSID rclsid,
				       REFIID riid,
				       void **ppv
				      )
{
   HRESULT hr;
   char asciiFilename[ MAX_PATH_LEN ], *pch;
   wchar wFilename[ MAX_PATH_LEN ];
   wchar awchClassID[ MAX_GUIDSTRING_LEN ];
   char achClassID[ MAX_GUIDSTRING_LEN ];
   char datumPath[ MAX_PATH_LEN ];
   HDLL hdll;
   int fh;
   CLSID actualCLSID;

   *ppv = NULL;

   /*
    * Resolve the given CLSID, which may be redirected to another via a
    * chain of Treat-As relationships.  See CoTreatAsClass() and friends.
    */

   hr = gCoResolveTreatAsClass( rclsid, &actualCLSID );
   if( FAILED( hr ) )
	return hr;      

   /* 
    * Determine the name of the DLL purportedly containing the class
    * implementation we're looking for.
    */

   gCoGUIDToString( &actualCLSID, awchClassID );
   gCoUnicodeStringToAscii( awchClassID, achClassID, MAX_GUIDSTRING_LEN );

#warning There needs to be a better way of doing this.  This chunk of code
#warning can be abused, deliberately or inadvertently, by putting oversized
#warning strings into the registry entries.  For now, however, I am just
#warning doing this to get things to work.  --saf2
   
   strcpy( datumPath, STR_REGISTRYHOME );
   strcat(
	  datumPath,
	  (inprocType == GCOMIT_SERVER) ? STR_INPROCSERVERS
					: STR_INPROCHANDLERS
	 );
   strcat( datumPath, achClassID );

   fh = open( datumPath, O_RDONLY );
   if( fh < 0 )
   {
      return E_READREGDB;
   }
   else
   {
      int sz;

      sz = read( fh, asciiFilename, MAX_PATH_LEN-1 );
      asciiFilename[ sz ] = 0;

      close( fh );
   }

   /*
    * Strip the ASCII name of all whitespace.  Note that if there is
    * any white space in front of the filename, then unpredictable
    * results can occur.  On my computer, dlopen() just chokes and returns
    * an error value.  (as it should.)
    */

   for(
	pch = &asciiFilename[0];
	*pch != 0;
	pch++
      )
   {
      if( isspace( *pch ) )    *pch = 0;
   }

   /*
    * gCoLoadDLL() will only load a particular DLL once, for if a DLL of
    * a given name is already loaded, it will simply return a handle to that
    * DLL.  If, and only if, a DLL with the given name isn't found will it
    * load the DLL from backing storage.
    */

   hr = gCoAsciiStringToUnicode( asciiFilename, wFilename, MAX_PATH_LEN );
   if( FAILED(hr) )
	return hr;

   hr = gCoLoadDLL( wFilename, &hdll );
   if( SUCCEEDED( hr ) )
   {
      hr = gCoDLLGetClassObject(
				hdll,
				rclsid,
				riid,
				ppv
			       );

      if( FAILED( hr ) )
      {
	 HRESULT temphr;

	 /* 
	  * We don't just forcibly unload the DLL, as something else may
	  * be using it for other purposes.
	  */

	 temphr = gCoDLLCanUnloadNow( hdll );
         if( temphr == S_OK )
	    gCoUnloadDLL( hdll );
      }
   }

   return hr;
}

/**
 * This function is called to obtain the class object associated with a
 * given class ID.
 * 
 * @param rclsid
 * The ID of the specific class object to obtain.
 * 
 * @param context
 * This parameter specifies the exact type of class object to
 * acquire.  This is a bit-wise OR of the following flags:
 * 
 * CLSCTX_INPROC_SERVER
 *    This specifies that the class object desired will
 *    create real in-process objects.
 * 
 * CLSCTX_INPROC_HANDLER
 *    This specifies that the class object desired will
 *    create in-process *handler* objects.  These are
 *    objects which are partially implemented in the
 *    current process, and partly implemented elsewhere.
 * 
 * CLSCTX_LOCAL_SERVER
 *    This specifies that the class object desired will
 *    create local server objects.  These are objects
 *    that are implemented in another address space,
 *    but on the local machine.
 * 
 * CLSCTX_REMOTE_SERVER
 *    This specifies that the class object desired will
 *    create objects on another computer.
 * 
 * Several commonly used flag combinations have been
 * assigned their own names:
 * 
 * CLSCTX_INPROC
 *    Same as specifying CLSCTX_INPROC_SERVER and
 *    CLSCTX_INPROC_HANDLER.
 * 
 * CLSCTX_SERVER
 *    Same as specifying CLSCTX_INPROC_SERVER,
 *    CLSCTX_LOCAL_SERVER, and CLSCTX_REMOTE_SERVER.
 * 
 * CLSCTX_ALL
 *    All flags.
 * 
 * @param serverInfo
 * This parameter is used only when CLSCTX_REMOTE_SERVER
 * is specified.  This is a pointer to a structure of
 * the following form:
 * 
 * 	typedef struct
 * 	{
 * 	   OLECHAR *	pszRemoteSCMBindingHandle;
 * 	} COMSERVERINFO;
 * 
 * The string specified in the structure is used to give
 * information on where the remote object implementation
 * might reside.  If this information isn't given
 * explicitly, then the information is either taken
 * from the registry database, is searched for on the
 * network, or the operation fails (depending on the
 * CLSID and its settings).
 * 
 * @param riid
 * This specifies the desired interface to acquire from
 * the class object.  In most cases, IID_IClassFactory
 * is specified for this parameter, but need not be the
 * case.
 * 
 * @param ppv
 * This is a pointer to a variable which is to hold the
 * reference to the indicated interface, if successful.
 * 
 * @returns
 * S_OK is returned on success.  E_CLASSNOTREG is returned
 * if the class implementation could not be found or is
 * not registered in the registry.  E_OUTOFMEMORY is
 * returned if there wasn't enough memory to complete the
 * operation.  For in-process servers or handlers,
 * anything returned by DllGetClassObject() is also returned
 * here as well.  E_UNEXPECTED is returned otherwise.
 */

HRESULT CoGetClassObject(
			 REFCLSID rclsid,
			 CLSCTX ctx,
			 COMSERVERINFO *serverInfo,
			 REFIID riid,
			 void **ppv
			)
{
   HRESULT hr;
   
   *ppv = NULL;		/* Just in case ctx == 0 */

   if( ctx & CLSCTX_INPROC_SERVER )
   {
      hr = gCoGetInprocClassObject( GCOMIT_SERVER, rclsid, riid, ppv );
      if( SUCCEEDED( hr ) )
	      return hr;
   }

   if( ctx & CLSCTX_INPROC_HANDLER )
   {
      hr = gCoGetInprocClassObject( GCOMIT_HANDLER, rclsid, riid, ppv );
      if( SUCCEEDED( hr ) )
	      return hr;
   }

   /*
    * We do not currently support local and remote servers,
    * but you get the idea as to how to implement them.
    */

   return E_CLASSNOTREG;
}

/**
 * This function creates a single object of class *clsid*.
 * It's really a convenient helper function that
 * calls CoGetClassObject() and
 * IClassFactory::CreateInstance() for you.
 * 
 * @param rclsid
 * The ID of the class to make the object.
 * 
 * @param punkOuter
 * The outer or controlling IUnknown interface used when
 * aggregating objects.
 * 
 * @param ctx
 * The context within which the object should be created.
 * These are the same values as used in
 * IClassFactory::CreateInstance.
 * 
 * @param riid
 * The initial interface ID used to reference the object.
 * Usually this is some kind of initialization interface.
 * 
 * @param ppv
 * A pointer to the variable that will hold a pointer
 * to the requested interface.
 * 
 * @returns
 * S_OK if the object was created successfully.
 * Otherwise, any error that can be returned from
 * CoGetClassObject or IClassFactory::CreateInstance().
 *
 * @see CoGetClassObject
 * @see CoCreateInstanceEx
 */

HRESULT CoCreateInstance(
			 REFCLSID rclsid,
			 const IUnknown *punkOuter,
			 CLSCTX ctx,
			 REFIID riid,
			 void **ppv
			)
{
   HRESULT hr;
   IClassFactory *pcf;
   
   hr = CoGetClassObject( rclsid, ctx, NULL, IID_IClassFactory, (void **)&pcf );
   if( SUCCEEDED( hr ) )
   {
      hr = pcf -> lpVtbl -> CreateInstance( pcf, punkOuter, riid, ppv );
      pcf -> lpVtbl -> Release( pcf );
   }
   
   return hr;
}

/**
 * Creates an instance of the class *rclsid* on a specific machine,
 * asking for a set of interfaces on the object.
 * 
 * @param rclsid
 * The class of which an instance is desired.
 * 
 * @param punkOuter
 * The controlling IUnknown, if any.
 * 
 * @param ctx
 * The context of the objects to be created.  See CoGetClassObject()
 * for more details.
 * 
 * @param serverInfo
 * Identifies the machine on which to activate the executable
 * code.  See CoGetClassObject() for more information.
 * 
 * @param count
 * The number of interfaces in the *multiQI* array.
 * 
 * @param multiQI
 * Pointer to an array of MULTI_QI structures, which must be
 * at least *count* elements in size.
 * 
 * @returns
 * S_OK if successful.  S_NOTALLINTERFACES is returned if not
 * all interfaces is supported by the object.  Examine
 * individual pvObj members of the MULTI_QI array to
 * determine exactly which interfaces were returned.
 * Otherwise, any error from CoCreateInstance() can be
 * returned.
 * 
 * @see CoGetClassObject
 * @see CoCreateInstance
 */

HRESULT CoCreateInstanceEx(
			   REFCLSID rclsid,
			   const IUnknown *punkOuter,
			   CLSCTX ctx,
			   COMSERVERINFO *serverInfo,
			   uint32 count,
			   MULTI_QI *multi_QI
			  )
{
   HRESULT hr;
   IClassFactory *pcf;
   IUnknown *punk;
   uint32 i, successfulQueries;

   hr = CoGetClassObject(
			 rclsid,
			 ctx,
			 serverInfo,
			 IID_IClassFactory,
			 (void **)&pcf
			);
   if( SUCCEEDED( hr ) )
   {
      hr = pcf -> lpVtbl -> CreateInstance(
					   pcf,
					   punkOuter,
					   IID_IUnknown,
					   (void **)&punk
					  );
      if( SUCCEEDED( hr ) )
      {
	 for( i = 0; i < count; i++ )
	 {
	    multi_QI[i].hr =
		    punk -> lpVtbl -> QueryInterface(
						     punk,
						     multi_QI[i].riid,
						     &multi_QI[i].pvObj
						    );
	    if( SUCCEEDED( multi_QI[i].hr ) )
		    successfulQueries++;
	 }
	 
	 punk -> lpVtbl -> Release( punk );

	 if( successfulQueries == 0 )
		 hr = E_NOINTERFACE;
	 else if( successfulQueries < count )
		 hr = S_NOTALLINTERFACES;
	 else if( successfulQueries == count )
		 hr = S_OK;
      }
   }

   return hr;
}

/*

Copyright (c) 1999, 2000 Samuel A. Falvo II

This software is provided 'as-is', without any implied or express warranty.
In no event shall the authors be held liable for damages arising from the
use this software.

Permission is granted for anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software in a
   product, an acknowledgment in the product documentation would be
   appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

3. This notice may not be removed or altered from any source
   distribution.

*/

#ifndef GCOM_CLASS_H
#define GCOM_CLASS_H

/*
 * gcom/class.h
 * GCOM Release 0.1
 */

#include <gcom/types.h>
#include <gcom/errors.h>
#include <gcom/interface.h>
#include <gcom/unicode.h>

/************************************************************************/
/* Miscellanious Class Definitions					*/
/************************************************************************/

#define MAX_GUIDSTRING_LEN		(39) /* Includes NULL terminator */

/*
DECLARE_CLSID(
	      NULL,
	      0x00000000,
	      0x0000, 0x0000, 0x0000,
	      0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	     )
*/

extern REFCLSID CLSID_NULL;

/************************************************************************/
/* IClassFactory and object creation definitions			*/
/************************************************************************/

/*
DECLARE_IID(
	    IClassFactory,
	    0x00000001,
	    0x0000, 0x0000, 0xC000,
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x46
	   )
*/

extern REFIID IID_IClassFactory;

BEGIN_INTERFACE( IClassFactory )
   HRESULT (*CreateInstance)(
			     IClassFactory *,
			     const IUnknown *,
			     REFIID,
			     void **
			    );
   HRESULT (*LockServer)( IClassFactory *, Bool );
END_INTERFACE( IClassFactory )

enum CLSCTX
{
   CLSCTX_INPROC_SERVER			= 1,
   CLSCTX_INPROC_HANDLER		= 2,
   CLSCTX_LOCAL_SERVER			= 4,
   CLSCTX_REMOTE_SERVER			= 16,
	   
   CLSCTX_INPROC			= CLSCTX_INPROC_SERVER
	   				| CLSCTX_INPROC_HANDLER,
	   
   CLSCTX_SERVER			= CLSCTX_INPROC_SERVER
	   				| CLSCTX_LOCAL_SERVER
	   				| CLSCTX_REMOTE_SERVER,
	   
   CLSCTX_ALL				= CLSCTX_INPROC_SERVER
	   				| CLSCTX_INPROC_HANDLER
	   				| CLSCTX_LOCAL_SERVER
	   				| CLSCTX_REMOTE_SERVER
};
typedef enum CLSCTX CLSCTX;

enum REGCLS
{
   REGCLS_SINGLEUSE			= 0,
   REGCLS_MULTIPLEUSE			= 1,
   REGCLS_MULTI_SEPARATE		= 2
};
typedef enum REGCLS REGCLS;

typedef struct COMSERVERINFO COMSERVERINFO;
struct COMSERVERINFO
{
   wchar *szRemoteSCMBindingHandle;
};

typedef struct MULTI_QI MULTI_QI;
struct MULTI_QI
{
   REFIID	riid;
   void *	pvObj; 		/* Note that it's not a void ** */
   HRESULT	hr;
};

/* For CoRegisterClassObject() and CoRevokeClassObject().  COM hardcodes the
 * return value from these functions to a DWORD (uint32 in GCOM).  However,
 * in operating systems other than Windows, a simple integer may not be
 * the most efficient way of returning the results of CoRegisterClassObject().
 * So we define a new return type, just for those functions.
 */
typedef uint32 GCOMREGTOKEN;

/**** PROTOTYPES ****/

HRESULT	CoRegisterClassObject(
			      REFCLSID,
			      const IUnknown *,
			      CLSCTX,
			      REGCLS,
			      GCOMREGTOKEN *
			     );

HRESULT	CoRevokeClassObject( GCOMREGTOKEN * );

HRESULT	CoGetClassObject(
			 REFCLSID,
			 CLSCTX,
			 COMSERVERINFO *,
			 REFIID,
			 void **
			);

HRESULT	CoCreateInstance(
			 REFCLSID,
			 const IUnknown *,
			 CLSCTX,
			 REFIID,
			 void **
			);

HRESULT	CoCreateInstanceEx(
			   REFCLSID,
			   const IUnknown *,
			   CLSCTX,
			   COMSERVERINFO *,
			   uint32,
			   MULTI_QI *
			  );

HRESULT	CoTreatAsClass( REFCLSID, REFCLSID );
HRESULT	CoGetTreatAsClass( REFCLSID, CLSID * );

#endif

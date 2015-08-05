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

#ifndef GCOM_ERRORS_H
#define GCOM_ERRORS_H

/*
 * gcom/errors.h
 * GCOM Release 0.1
 */

#include <gcom/types.h>

/************************************************************************/
/* Error Definitions							*/
/************************************************************************/

typedef uint32 HRESULT;

#define FACILITY_NULL		(0)
#define FACILITY_RPC		(1)
#define FACILITY_DISPATCH	(2)
#define FACILITY_STORAGE	(3)
#define FACILITY_ITF		(4)
#define FACILITY_WIN32		(7)	/* Never used by GCOM */
#define FACILITY_WINDOWS	(8)	/* Never used by GCOM */
#define FACILITY_CONTROL	(10)	/* Never used by GCOM */
#define FACILITY_AMIGA		(11)	/* TODO: Get official number */

#define SEVERITY_SUCCESS	(0)
#define SEVERITY_ERROR		(1)

#define HRESULT_SEVERITY(hr)	(((hr)>>31)&1)
#define HRESULT_FACILITY(hr)	(((hr)>>16)&0x1FFF)
#define HRESULT_CODE(hr)	((hr)&0xFFFF)

#define SUCCEEDED(hr)		(HRESULT_SEVERITY((hr))==SEVERITY_SUCCESS)
#define FAILED(hr)		(HRESULT_SEVERITY((hr))==SEVERITY_ERROR)

#define MAKE_HRESULT(sev,fac,code)		\
   ((HRESULT)((((uint32)(sev))<<31)|(((uint32)(fac))<<16)|((uint32)(code))))

#define S_OK		MAKE_HRESULT( SEVERITY_SUCCESS, FACILITY_NULL, 0 )
#define S_FALSE		MAKE_HRESULT( SEVERITY_SUCCESS, FACILITY_NULL, 1 )

/* TODO: GET OFFICIAL WIN32 ERROR VALUES! */

#define S_NOTALLINTERFACES \
       MAKE_HRESULT( SEVERITY_SUCCESS, FACILITY_NULL, 2 )

#define E_UNEXPECTED	MAKE_HRESULT( SEVERITY_ERROR, FACILITY_NULL, 0x00 )
#define E_INVALIDARG	MAKE_HRESULT( SEVERITY_ERROR, FACILITY_NULL, 0x01 )
#define E_OUTOFMEMORY	MAKE_HRESULT( SEVERITY_ERROR, FACILITY_NULL, 0x02 )
#define E_READREGDB	MAKE_HRESULT( SEVERITY_ERROR, FACILITY_NULL, 0x03 )
#define E_WRITEREGDB	MAKE_HRESULT( SEVERITY_ERROR, FACILITY_NULL, 0x04 )
#define E_DLLNOTFOUND	MAKE_HRESULT( SEVERITY_ERROR, FACILITY_NULL, 0x05 )
#define E_SYMBOLNOTFOUND MAKE_HRESULT( SEVERITY_ERROR, FACILITY_NULL, 0x06 )

#define E_NOAGGREGATION	MAKE_HRESULT( SEVERITY_ERROR, FACILITY_NULL, 0x10 )
#define E_CLASSNOTREG	MAKE_HRESULT( SEVERITY_ERROR, FACILITY_NULL, 0x11 )
#define E_OBJISREG	MAKE_HRESULT( SEVERITY_ERROR, FACILITY_NULL, 0x12 )
#define E_NOINTERFACE	MAKE_HRESULT( SEVERITY_ERROR, FACILITY_NULL, 0x13 )

/* Amiga Specific Errors */

#define E_NOPERMISSION	MAKE_HRESULT( SEVERITY_ERROR, FACILITY_AMIGA, 0x00 )

/* Synonyms for Amiga errors */

#define AMIGA_E_NOPERMISSION

/* The following definitions are for source code compatibility with the
 * COM 0.9 specification as published on Microsoft's website (see
 * http://www.microsoft.com/com/).
 */

#define CO_S_NOTALLINTERFACES		S_NOTALLINTERFACES
#define REGDB_E_CLASSNOTREG		E_CLASSNOTREG
#define CO_E_OBJISREG			E_OBJISREG
#define CO_E_CLASSNOTREG		E_CLASSNOTREG
#define CO_E_READREGDB			E_READREGDB
#define CO_E_WRITEREGDB			E_WRITEREGDB
#define CLASS_E_NOAGGREGATION		E_NOAGGREGATION

#endif

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

#ifndef GCOM_GUID_H
#define GCOM_GUID_H

/*
 * gcom/guid.h
 * GCOM Release 0.1
 */

#include <gcom/types.h>
#include <gcom/unicode.h>

/************************************************************************/
/* Globally Unique Identifiers are used throughout GCOM to identify	*/
/* a variety of things, ranging from individual interfaces, to classes,	*/
/* to ... whatever.  :-)						*/
/************************************************************************/

#define GCOM_DECLARE_ID_TYPE(id)		\
   typedef struct GUID id;			\
   typedef const id * REF##id ;

struct GUID
{
   uint32	Data1;
   uint16	Data2;
   uint16	Data3;
   uint8	Data4[8];
};

GCOM_DECLARE_ID_TYPE( GUID );
GCOM_DECLARE_ID_TYPE( UUID );
GCOM_DECLARE_ID_TYPE( IID );
GCOM_DECLARE_ID_TYPE( CLSID );

/* This butt-ugly macro allows us to easily create UUIDs in C source. */

#define DECLARE_GUID(name, a,b,c,d,e,f,g,h,i,j)		\
   static struct GUID ___##name = {			\
      a, b, c,						\
      {							\
	 ((d)>>8),					\
         ((d)&0xFF),					\
	 e,f,g,h,i,j					\
      }							\
   };							\
   /*static*/ const GUID * name = & ___##name ;

#define DECLARE_UUID(name, a,b,c,d,e,f,g,h,i,j)		\
   DECLARE_GUID(name, a,b,c,d,e,f,g,h,i,j)

/* Note that this macro automatically prepends IID_ to the ID's name
 * you're defining.
 */

#define DECLARE_IID(name, a,b,c,d,e,f,g,h,i,j)		\
   DECLARE_GUID( IID_##name , a,b,c,d,e,f,g,h,i,j)

/* Note that this macro automatically prepends CLSID_ to the ID's name
 * you're defining.
 */

#define DECLARE_CLSID(name, a,b,c,d,e,f,g,h,i,j)	\
   DECLARE_GUID( CLSID_##name , a,b,c,d,e,f,g,h,i,j)

Bool IsEqualIID( REFIID, REFIID );
void gCoGUIDToString( REFGUID, wchar * );
HRESULT gCoStringToGUID( wchar *, GUID * );

#endif

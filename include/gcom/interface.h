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

#ifndef GCOM_INTERFACE_H
#define GCOM_INTERFACE_H

/*
 * gcom/interface.h
 * GCOM Release 0.1
 */

#include <gcom/types.h>
#include <gcom/guid.h>

/************************************************************************/
/* Interface definition macros for the C programming language, to help  */
/* automate the process of creating new interfaces in C.  C++ users do  */
/* not need this kind of support, as it's already integral to the	*/
/* compiler itself.							*/
/************************************************************************/

#define BEGIN_INTERFACE_(i)				\
   typedef struct i##Vtbl i##Vtbl;			\
   typedef struct i i;					\
   struct i						\
   {							\
      i##Vtbl * lpVtbl;					\
   };							\
   struct i##Vtbl					\
   {

#define END_INTERFACE_(i)				\
   };

#define IUNKNOWN(i)					\
   HRESULT (*QueryInterface)( i *, REFIID, void ** );	\
   uint32 (*AddRef)( i * );				\
   uint32 (*Release)( i * );


#define BEGIN_INTERFACE(i)				\
   BEGIN_INTERFACE_(i)					\
   IUNKNOWN(i)

#define END_INTERFACE(i)				\
   END_INTERFACE_(i)

/* Declare the IUnknown interface structures. */

/*
DECLARE_IID(
	    IUnknown,
	    0x00000000,
	    0x0000, 0x0000, 0xC000,
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x46
	   )
*/

extern REFIID IID_IUnknown;

BEGIN_INTERFACE( IUnknown )
END_INTERFACE( IUnknown )

#endif

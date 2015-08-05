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

#ifndef GCOM_ALLOC_H
#define GCOM_ALLOC_H

/*
 * gcom/alloc.h
 * GCOM Release 0.1
 */

#include <gcom/types.h>
#include <gcom/errors.h>
#include <gcom/interface.h>

/************************************************************************/
/* IMalloc definitions							*/
/************************************************************************/

/*
DECLARE_IID(
	    IMalloc,
	    0x00000002,
	    0x0000, 0x0000, 0xC000,
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x46
	   )
*/

extern REFIID IID_IMalloc;

BEGIN_INTERFACE( IMalloc )
   void *	(*Alloc)( IMalloc *, uint32 );
   void *	(*Realloc)( IMalloc *, void *, uint32 );
   void		(*Free)( IMalloc *, void * );
   uint32	(*GetSize)( IMalloc *, void * );
   int		(*DidAlloc)( IMalloc *, void * );
   void		(*HeapMinimize)( IMalloc * );
END_INTERFACE( IMalloc )

enum MEMCTX
{
   MEMCTX_TASK 		= 1,
   MEMCTX_SHARED	= 2,
   MEMCTX_MACSYSTEM	= 3,	/* Unsupported in Andromeda */
   MEMCTX_UNKNOWN	= -1,	/* Unsupported in Andromeda */
   MEMCTX_SAME		= -2,	/* Unsupported in Andromeda */
};
typedef enum MEMCTX MEMCTX;

HRESULT	CoGetMalloc( MEMCTX, IMalloc ** );
void *	CoTaskMemAlloc( uint32 );
void *	CoTaskMemRealloc( void *, uint32 );
void	CoTaskMemFree( void * );

#endif

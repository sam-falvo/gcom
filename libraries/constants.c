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

/*
 * constants.c
 * GCOM Release 0.3
 */

#include <gcom/gcom.h>

/************************************************************************/
/* Interface ID Constants						*/
/************************************************************************/


DECLARE_IID(
	    IUnknown,
	    0x00000000,
	    0x0000, 0x0000, 0xC000,
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x46
	   )

DECLARE_IID(
	    IMalloc,
	    0x00000002,
	    0x0000, 0x0000, 0xC000,
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x46
	   )

DECLARE_IID(
	    IClassFactory,
	    0x00000001,
	    0x0000, 0x0000, 0xC000,
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x46
	   )

/************************************************************************/
/* Class ID Constants							*/
/************************************************************************/

DECLARE_CLSID(
	      NULL,
	      0x00000000,
	      0x0000, 0x0000, 0x0000,
	      0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	     )

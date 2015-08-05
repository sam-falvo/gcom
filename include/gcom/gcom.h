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

#ifndef GCOM_GCOM_H
#define GCOM_GCOM_H

/*
 * gcom/gcom.h
 * GCOM Release 0.1
 */

#include <gcom/types.h>
#include <gcom/unicode.h>
#include <gcom/guid.h>
#include <gcom/errors.h>
#include <gcom/interface.h>
#include <gcom/class.h>
#include <gcom/alloc.h>
#include <gcom/dll.h>
#include <gcom/registry.h>

/************************************************************************/
/* GCOM Version Information -- GCOM specific.  Microsoft COM hardcoded  */
/* version datatypes to 32-bit unsigned integers.  This is fine, but it */
/* isn't as portable as a typedef or two.				*/
/************************************************************************/

typedef uint32		GCOMVERSION;

#define GCOM_VERSION_MAJOR(v)	((uint16)(((v)>>16))&0xFFFF)
#define GCOM_VERSION_MINOR(v)	((uint16)((v)&0xFFFF))
#define MAKE_GCOMVERSION(v,r)	((uint32)((uint16)(v)<<16)|((uint16)(r)))

#define GCOM_CURRENT_VERSION	(0)
#define GCOM_CURRENT_REVISION	(3)

GCOMVERSION	CoBuildVersion( void );

/************************************************************************/
/* Miscellanious GCOM functions						*/
/************************************************************************/

HRESULT		CoInitialize( void * );
void		CoUninitialize( void );

#endif

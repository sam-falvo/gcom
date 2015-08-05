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

#ifndef GCOM_TYPES_H
#define GCOM_TYPES_H

/*
 * gcom/types.h
 * GCOM Release 0.3
 */

/************************************************************************/
/* Generic Type Definitions						*/
/************************************************************************/

typedef char			Bool;
typedef unsigned int		uint;

/************************************************************************/
/* Platform Specific Type Definitions					*/
/************************************************************************/

/* TAO/Elate using VP2 */

#if defined( __ELATE__ ) && defined( __GNUC__ )
typedef char			int8;
typedef short			int16;
typedef int			int32;
typedef long			int64;

typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned int		uint32;
typedef unsigned long		uint64;

/* Linux using GNU C/C++ */

#elif defined( __LINUX__ ) && defined( __GNUC__ )
typedef char			int8;
typedef short			int16;
typedef long			int32;
typedef long long		int64;

typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned long		uint32;
typedef unsigned long long	uint64;

/* Haven't a clue; define your specific platform here. */

#else
#error Define a set of datatypes for your particular platform here.
#endif

/************************************************************************/
/* Unicode Type Support							*/
/************************************************************************/

typedef uint8			UTF8;
typedef uint16			Rune;

#if defined( __ANSI__ )
#include <wchar.h>
typedef wchar_t			wchar;
#else	/* Assume UCS-4 encoding */
typedef uint32			wchar;
#endif

/************************************************************************/
/* Constants								*/
/************************************************************************/

#if defined( __GNUC__ )
#ifdef NULL
#undef NULL
#endif
#endif
#define NULL			(0L)

#ifndef FALSE
#define FALSE			(0L)
#endif

#ifndef TRUE
#define TRUE			(!FALSE)
#endif

/************************************************************************/
/* Nice construct for defining flags variables and such.  For any given */
/* flag ABC, in the family of DEF, you'll get two definitions:		*/
/*									*/
/* DEFB_ABC = bit							*/
/* DEFF_ABC = (1L<<bit)							*/
/*									*/
/* They are created as enumerations.					*/
/************************************************************************/

#define DEFINE_FLAG( fam, flg, val )	\
	enum { fam##B_##flg = (val), fam##F_##flg = (1L<<(val)) };

#endif

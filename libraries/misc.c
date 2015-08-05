/*
 * misc.c
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

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <gcom/gcom.h>
#include "gcom-config.h"

/**
 * Compare two UUIDs for equality.  Note that the name of this function
 * was chosen for two reasons:
 * 1) The most common use case for this function is to compare two interface
 * IDs, and
 * 2) it retains some degree of source code compatibility with Windows
 * COM implementations.
 *
 * @param riid1
 * The first interface ID (or any GUID for that matter) to compare.
 * 
 * @param riid2
 * The second interface ID (or any GUID for that matter) to compare.
 *
 * @returns
 * TRUE if they are both bit-wise identical.  FALSE otherwise.
 */

Bool IsEqualIID( REFIID riid1, REFIID riid2 )
{
   return memcmp( riid1, riid2, sizeof( GUID ) ) == 0;
}

/**
 * This function creates a Unicode string representation of a class ID.
 * The resulting string is NULL-terminated.  The textual representation
 * of a class ID is {01234567-89AB-CDEF-FEDC-BA9876543210}, including
 * the opening and closing braces.
 * 
 * @param rclsid
 * The class ID to express as a string.
 * 
 * @param buffer
 * Pointer to a buffer which is at least MAX_GUIDSTRING_LEN characters in
 * length (long enough to handle the string itself, plus the terminating
 * NULL).  MAX_GUIDSTRING_LEN is currently defined to 40.
 * 
 * @returns Nothing.
 *
 * @see gCoStringToGUID
 */

void gCoGUIDToString( REFCLSID rclsid, wchar *buffer )
{
   char asciiBuffer[ MAX_GUIDSTRING_LEN ];

   sprintf(
	   asciiBuffer,
	   STR_GUIDSTRING_TEMPLATE,
	   rclsid -> Data1,
	   rclsid -> Data2, rclsid -> Data3,
	   rclsid -> Data4[0], rclsid -> Data4[1],
	   rclsid -> Data4[2], rclsid -> Data4[3],
	   rclsid -> Data4[4], rclsid -> Data4[5],
	   rclsid -> Data4[6], rclsid -> Data4[7]
	  );

   gCoAsciiStringToUnicode( asciiBuffer, buffer, MAX_GUIDSTRING_LEN );
}

/**
 * Converts a single hexadecimal character into its equivalent 4-bit
 * quantity.
 * 
 * @param pstr
 * Pointer to the character to convert.
 * 
 * @returns
 * The equivalent numeric value.
 */

static uint8 ConvertNybble( char *pstr )
{
   uint8 v = (uint8)( toupper( *pstr ) ) - '0';
   
   if( v > 9 )
	   v -= 7;
   
   return v;
}

/**
 * Converts two characters of a string into a byte value in binary.
 * 
 * @param pstr
 * Pointer to the string of two characters to convert.  The string need
 * NOT be NULL-terminated.
 * 
 * @returns
 * The byte represented by the string.
 * 
 * @see gCoStringToGUID
 */

static uint8 ConvertByte( char *pstr )
{
   uint8 v = 0;
   
   v = ConvertNybble( pstr++ );
   v = ( v << 4 ) + ConvertNybble( pstr );
   
   return v;
}

/**
 * This function converts a string representation of a GUID into
 * a real GUID.  The textual representation of a class ID is
 * {01234567-89AB-CDEF-FEDC-BA9876543210}, including the opening and
 * closing braces.
 * 
 * @param buffer
 * Pointer to the start of a Unicode string buffer containing the
 * GUID in string form.
 * 
 * @param pclsid
 * Pointer to a CLSID which is to contain the resulting GUID in binary
 * form.
 * 
 * @returns
 * S_OK if successful.  E_INVALIDARG if the supplied textual form of the
 * GUID is not valid (e.g., contains characters other than {, }, -, and
 * the hexadecimal digits).
 *
 * @see gCoGUIDToString
 */

HRESULT gCoStringToGUID( wchar *buffer, CLSID *pclsid )
{
   char asciiGUID[ MAX_GUIDSTRING_LEN ];
   int i;
   
   gCoUnicodeStringToAscii( buffer, asciiGUID, MAX_GUIDSTRING_LEN );
   asciiGUID[ MAX_GUIDSTRING_LEN-1 ] = 0;	/* Ensure NULL termination */
  
   memset( pclsid, 0, sizeof( GUID ) );

   /* Sanity checks */
   
   if( asciiGUID[0] != '{' )		return E_INVALIDARG;
   if( asciiGUID[9] != '-' )		return E_INVALIDARG;
   if( asciiGUID[14] != '-' )		return E_INVALIDARG;
   if( asciiGUID[19] != '-' )		return E_INVALIDARG;
   if( asciiGUID[24] != '-' )		return E_INVALIDARG;
   if( asciiGUID[37] != '}' )		return E_INVALIDARG;
   
   for( i = 1; i < 9; i++ )
	   if( !( isxdigit( asciiGUID[i] ) ) )
		   return E_INVALIDARG;
   
   for( i = 10; i < 14; i++ )
	   if( !( isxdigit( asciiGUID[i] ) ) )
		   return E_INVALIDARG;
   
   for( i = 15; i < 19; i++ )
	   if( !( isxdigit( asciiGUID[i] ) ) )
		   return E_INVALIDARG;
   
   for( i = 20; i < 24; i++ )
	   if( !( isxdigit( asciiGUID[i] ) ) )
		   return E_INVALIDARG;
   
   for( i = 25; i < 37; i++ )
	   if( !( isxdigit( asciiGUID[i] ) ) )
		   return E_INVALIDARG;
   
   /* Now let's delimit the strings so that we can convert them to
    * binary.
    */
   
   asciiGUID[9] =
   asciiGUID[14] =
   asciiGUID[19] =
   asciiGUID[24] =
   asciiGUID[37] = 0;
   
   /* Now we actually begin the conversion process. */
   
   pclsid -> Data1 = strtoul( &asciiGUID[1], NULL, 16 );
   pclsid -> Data2 = strtoul( &asciiGUID[10], NULL, 16 );
   pclsid -> Data3 = strtoul( &asciiGUID[15], NULL, 16 );
   pclsid -> Data4[0] = ConvertByte( &asciiGUID[20] );
   pclsid -> Data4[1] = ConvertByte( &asciiGUID[22] );
   
   for( i = 0; i < 6; i++ )
	   pclsid -> Data4[i+2] = ConvertByte( &asciiGUID[ 25 + ( i * 2 ) ] );

   return S_OK;
}

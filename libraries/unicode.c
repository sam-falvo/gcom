/*
 * unicode.c
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

#include <stdio.h>
#include <stdlib.h>
#include <gcom/gcom.h>

/**
 * This function converts a Unicode string into an ASCII/UTF-8 string.
 * 
 * @param pstr1
 * This is the Unicode string to convert.  The string must be NULL-
 * terminated.
 * 
 * @param pstr2
 * This is the ASCII/UTF-8 string buffer to store the resulting string
 * into.
 * 
 * @param bufSize
 * This is the maximum number of bytes in the output buffer pointed to by
 * pstr2.  This function converts up to (bufSize-1) bytes of textual data,
 * thus guaranteeing at least one NULL byte at the very end of the buffer.
 * 
 * @returns
 * S_OK if the string converted correctly.  Otherwise, E_INVALIDARG is
 * returned if an illegal character is returned.
 * 
 * @see gCoAsciiStringToUnicode
 */

HRESULT gCoUnicodeStringToAscii( wchar *pstr1, char *pstr2, uint32 bufSize )
{
   memset( pstr2, 0, bufSize );
   if( wcstombs( pstr2, pstr1, bufSize ) == -1 )
	   return E_INVALIDARG;
   
   return S_OK;
}

/**
 * This function converts an ASCII/UTF-8 string into a Unicode string.
 * 
 * @param pstr1
 * This is the ASCII/UTF-8 string buffer to store the resulting string
 * into.
 * 
 * @param pstr2
 * This is the Unicode string to convert.  The string must be NULL-
 * terminated.
 * 
 * @param chars
 * This is the maximum number of Unicode characters in the output buffer.
 * This function actually converts (chars-1) characters, thus guaranteeing
 * enough space in the output buffer for the terminating NULL character.
 * 
 * @returns
 * S_OK if the string converted correctly.  Otherwise, E_INVALIDARG is
 * returned if an illegal character is returned.
 * 
 * @see gCoUnicodeStringToAscii
 */

HRESULT gCoAsciiStringToUnicode( char *pstr1, wchar *pstr2, uint32 chars )
{
   memset( pstr2, 0, chars * sizeof( wchar ) );
   if( mbstowcs( pstr2, pstr1, chars ) == -1 )
	   return E_INVALIDARG;
   
   return S_OK;
}

/**
 * This function returns the length of a Unicode string.
 * 
 * @param str
 * Pointer to a NULL-terminated Unicode string.
 * 
 * @returns
 * The number of characters (not bytes!) in the string is returned.
 * Semantics are largely similar to that or strlen().
 * 
 * @see strlen
 */

uint32 gCoUnicodeStringLength( wchar *pstr )
{
   return wcslen( pstr );
}

/**
 * This function returns the size of a Unicode string in bytes.
 * 
 * @param str
 * Pointer to a NULL-terminated Unicode string.
 * 
 * @returns
 * The number of bytes (not characters!) in the string is returned.
 * 
 * @see gCoUnicodeStringLength
 */

uint32 gCoUnicodeStringSizeInBytes( wchar *pstr )
{
   return wcslen( pstr ) * sizeof( wchar );
}

/**
 * This function copies one Unicode string to another.  It is assumed
 * that the receiving buffer is big enough to hold the incoming string.
 * 
 * @param str1
 * The source string.
 * 
 * @param str2
 * Pointer to the destination buffer, which will hold the string.
 * 
 * @returns Nothing.
 */

void gCoUnicodeStringCopy( wchar *ps, wchar *pd )
{
   wcscpy( pd, ps );
}

/**
 * This function duplicates a string.  The CALLER is
 * responsible for freeing the memory via the task's
 * allocator object or via CoTaskMemFree().
 * 
 * @param pSource
 * The string to duplicate.
 * 
 * @param ppDest
 * The pointer to a variable of type wchar *.  After this function
 * completes, that pointer will refer to the duplicate string.
 * 
 * @result
 * S_OK if the string was successfully duplicated.  E_OUTOFMEMORY
 * if not enough memory could be gathered to complete the operation.
 */

HRESULT gCoUnicodeStringDuplicate( wchar *ps, wchar **ppd )
{
   HRESULT hr;
   uint32 length;
   
   length = ( gCoUnicodeStringLength( ps ) + 1 ) * sizeof( wchar );
   *ppd = (wchar *)CoTaskMemAlloc( length );
   if( *ppd )
   {
      gCoUnicodeStringCopy( ps, *ppd );
      hr = S_OK;
   }
   else
      hr = E_OUTOFMEMORY;
   
   return hr;
}

/**
 * Compares two strings, and returns their collating order.
 * 
 * @param str1
 * The first string to compare.
 * 
 * @param str2
 * The second string to compare.
 * 
 * @returns
 * An integer indicating the collating order of the two strings
 * is returned.  -1 is returned if string 1 is "less than" string 2
 * (that is, it would appear before string 2 in a dictionary or index),
 * 0 if they are equal, or +1 if string 1 is "greater than" string 2.
 */

int gCoUnicodeStringCompare( wchar *ps, wchar *pd )
{
   return wcscmp( ps, pd );
}

/**
 * Concatenates one unicode string after another.  The programmer
 * must ensure that there is gCoUnicodeStringLength(s1) +
 * gCoUnicodeStringLength(s2) + 1 characters available in s1.
 *
 * @param s1
 * The string to which s2 is tacked onto the end of.
 *
 * @param s2
 * The string to tack onto the end of s1.
 *
 * @returns
 * Nothing.
 */

void gCoUnicodeStringConcatenate( wchar *s1, wchar *s2 )
{
   wcscat( s1, s2 );
}

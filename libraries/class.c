/*
 * class.c
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

#include <gcom/gcom.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include "gcom-config.h"

/************************************************************************/
/* Library Functions                                                    */
/************************************************************************/

/**
 * This function tells the COM environment to treat *rclsidOld*
 * as *rclsidNew*.  This has many useful applications, including things
 * like patching existing software (including native operating system
 * without having to close the applications, or reboot the operating
 * system.
 * 
 * Changes take effect immediately upon return from this function, and
 * are system-wide.
 * 
 * **NOTE**  It's perfectly legal to "redirect" a class to an emulated
 * class.  That is, consider the following registry database setup:
 * 
 * [InprocServers]
 *      {DEADBEEF-FEED-FACE-0000-0123456789AB} = /com/inproc/servers/foo.so
 * 
 * [Emulations]
 *      {00010203-0405-0607-0809-0A0B0C0D0E0F} =
 *              {0F0E0D0C-0B0A-0908-0706-050403020100}
 *      {0F0E0D0C-0B0A-0908-0706-050403020100} =
 *              {00112233-4455-6677-8899-AABBCCDDEEFF}
 *      {00112233-4455-6677-8899-AABBCCDDEEFF} =
 *              {DEADBEEF-FEED-FACE-0000-0123456789AB}
 * 
 * If we try to obtain the class factory for {00010203-0405-0607-0809-
 * 0A0B0C0D0E0F}, we'll actually get the class factory for
 * {DEADBEEF-FEED-FACE-0000-0123456789AB}.
 * 
 * @param rclsidOld
 * The class which is to be emulated.
 * 
 * @param rclsidNew
 * The class which is to perform the emulation.  If CLSID_NULL is specified,
 * the emulation mapping for rclsidOld is removed.
 * 
 * @returns
 * S_OK if successful.  E_WRITEREGDB is returned if the registry database
 * couldn't be updated.  E_NOPERMISSION if the caller doesn't have
 * suitable permissions for establishing a treat-as relationship.
 */

HRESULT CoTreatAsClass( REFCLSID rclsidOld, REFCLSID rclsidNew )
{
   HRESULT hr = E_WRITEREGDB;
   char fullFilename[ MAX_PATH_LEN ];
   char achClassID[ MAX_GUIDSTRING_LEN ];
   wchar awchClassID[ MAX_GUIDSTRING_LEN ];
   char achNewClassID[ MAX_GUIDSTRING_LEN ];
   wchar awchNewClassID[ MAX_GUIDSTRING_LEN ];
   int fh;

   if( IsEqualIID( rclsidNew, CLSID_NULL ) )
   {
      gCoGUIDToString( rclsidOld, awchClassID );
      gCoUnicodeStringToAscii( awchClassID, achClassID, MAX_GUIDSTRING_LEN );
      strcpy( fullFilename, STR_REGISTRYHOME );
      strcat( fullFilename, STR_TREATAS );
      strcat( fullFilename, achClassID );

      if( ( unlink( fullFilename ) ) < 0 )
      {
         switch( errno )
         {
            case EACCES:
            case EPERM:
               hr = E_NOPERMISSION;
               break;

            default:
               hr = E_WRITEREGDB;
               break;
         }
      }
      else
         hr = S_OK;
   }
   else
   {
      gCoGUIDToString( rclsidOld, awchClassID );
      gCoUnicodeStringToAscii( awchClassID, achClassID, MAX_GUIDSTRING_LEN );
      strcpy( fullFilename, STR_REGISTRYHOME );
      strcat( fullFilename, STR_TREATAS );
      strcat( fullFilename, achClassID );

      printf( "Writing file %s\n", fullFilename );

      gCoGUIDToString( rclsidNew, awchNewClassID );

      hr = gCoUnicodeStringToAscii( awchNewClassID, achNewClassID, MAX_GUIDSTRING_LEN );
      if( FAILED(hr) )  return hr;

      fh = open( fullFilename, O_WRONLY | O_CREAT | O_TRUNC, 0666 );
      if( fh > 0 )
      {
         write( fh, achNewClassID, MAX_GUIDSTRING_LEN );
         close( fh );

         hr = S_OK;
      }
      else
      {
         switch( errno )
         {
            case EACCES:
            case EPERM:
               hr = E_NOPERMISSION;
               break;

            default:
               hr = E_WRITEREGDB;
               break;
         }
      }
   }
   
   return hr;
}

/**
 * Retrieves the current emulation status of a particular class.
 * 
 * @param rclsidOld
 * This is the class ID for which we want to get emulation information
 * on.
 * 
 * @param pclsidNew
 * This is a pointer to a GUID which will hold the emulated class.
 * 
 * @returns
 * E_READREGDB is returned if the registration database couldn't be
 * accessed, *or* if no treat-as relationship exists for the class.
 * E_NOPERMISSION is returned if the caller doesn't have enough
 * permission to obtain a class' treat-as relationship.  S_FALSE is
 * returned if the class is emulating itself.  S_OK otherwise.
 *
 * @see CoTreatAsClass
 */

HRESULT CoGetTreatAsClass( REFCLSID rclsidOld, CLSID *pclsidNew )
{
   HRESULT hr;
   wchar awchClassID[ MAX_GUIDSTRING_LEN ];
   char achClassID[ MAX_GUIDSTRING_LEN ];
   wchar awchReadClassID[ MAX_GUIDSTRING_LEN ];
   char achReadClassID[ MAX_GUIDSTRING_LEN  ];
   char datumName[ MAX_PATH_LEN ];
   CLSID readClassID;
   int fh;

   /*
    * Convert class ID to string
    */

   gCoGUIDToString( rclsidOld, awchClassID );
   hr = gCoUnicodeStringToAscii( awchClassID, achClassID, MAX_GUIDSTRING_LEN );
   if( FAILED(hr) )
      return hr;

   /*
    * From the string form of the class ID, build the registry
    * entry's filename.
    */

   strcpy( datumName, STR_REGISTRYHOME );
   strcat( datumName, STR_TREATAS );
   strcat( datumName, achClassID );

   /*
    * Read registry entry's value.
    */

   fh = open( datumName, O_RDONLY );
   if( fh < 0 )
   {
      return E_READREGDB;
   }
   else
   {
      read( fh, &achReadClassID, MAX_GUIDSTRING_LEN );
      close( fh );

      achReadClassID[ MAX_GUIDSTRING_LEN-1 ] = 0;
   }

   /*
    * Convert registry entry's textual value into a CLSID
    */

   hr = gCoAsciiStringToUnicode( achReadClassID, awchReadClassID, MAX_GUIDSTRING_LEN );
   if( FAILED(hr) )
        return hr;

   hr = gCoStringToGUID( awchReadClassID, &readClassID );
   if( SUCCEEDED( hr ) )
   {
      if( IsEqualIID( &readClassID, rclsidOld ) )
         hr = S_FALSE;
      else
         hr = S_OK;

      memcpy( pclsidNew, &readClassID, sizeof( CLSID ) );
   }
   else
   {
      memset( pclsidNew, 0, sizeof( CLSID ) );
   }
   
   return hr;
}

/**
 * This function iteratively resolves class emulations, set up with the
 * CoTreatAsClass() function.  This function is semantically equivalent
 * to CoGetTreatAsClass(), but it continues to resolve emulations as long
 * as emulations exist.
 * 
 * @param rclsidOld See CoGetTreatAsClass().
 * @param pclsidNew See CoGetTreatAsClass().
 * 
 * @returns See CoGetTreatAsClass().
 */

HRESULT gCoResolveTreatAsClass( REFCLSID rclsidOld, CLSID *pclsidNew )
{
   HRESULT hr;
   CLSID clsid;
   
   memcpy( &clsid, rclsidOld, sizeof( CLSID ) );
   hr = CoGetTreatAsClass( &clsid, &clsid );
   while( SUCCEEDED( hr ) )
   {
      /* These two conditionals exist to prevent deadlock on
       * cases where rclsidOld is emulated to itself, but through
       * several other classes (e.g., class A -> class B -> 
       * class C -> class A).
       */

      if( hr == S_FALSE )
              break;
      
      if( IsEqualIID( rclsidOld, &clsid ) )
      {
         hr = S_FALSE;
         break;
      }

      hr = CoGetTreatAsClass( &clsid, &clsid );
   }

   memcpy( pclsidNew, &clsid, sizeof( CLSID ) );
   return S_OK;
}

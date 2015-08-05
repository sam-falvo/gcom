#include <gcom/gcom.h>
#include <stdio.h>

#include "IDisplayText.h"
#include "CLSID_Example1.h"

#define MSG		"Hello World!!"
#define MSGLEN		(strlen(MSG)+1)

int main( void )
{
   HRESULT hr;
   IDisplayText *pdt;

   hr = CoInitialize( NULL );
   if( FAILED(hr) )
   {
      printf( "Couldn't initialize the COM library: $%08X\n", hr );
      return 1;
   }

   hr = CoCreateInstance(
			 CLSID_Example1,
			 NULL,
			 CLSCTX_SERVER,
			 IID_IDisplayText,
			 (void *)&pdt
			);
   if( SUCCEEDED(hr) )
   {
      pdt -> lpVtbl -> SetText( pdt, MSG, MSGLEN );
      pdt -> lpVtbl -> Display( pdt );

      pdt -> lpVtbl -> Release( pdt );
   }
   else
      printf( "Couldn't create object of example1.so: $%08X\n", hr );

   CoUninitialize();
   return 0;
}

#ifndef IDisplayText_impl__H
#define IDisplayText_impl__H

/* in IDisplayText_impl.h */
	
#include "IDisplayText.h"
	
typedef struct IDisplayText_impl IDisplayText_impl;
struct IDisplayText_impl
{
   IDisplayTextVtbl *	lpVtbl;
   uint32		referenceCount;
   char			buffer[128];
};

#endif

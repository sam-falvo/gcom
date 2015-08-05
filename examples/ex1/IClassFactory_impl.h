#ifndef IClassFactory__H
#define IClassFactory__H

#include <gcom/gcom.h>
	
typedef struct IClassFactory_impl IClassFactory_impl;
struct IClassFactory_impl
{
   IClassFactoryVtbl *lpVtbl;	/* Same as IClassFactory */
   
   uint32	objectCount;
   uint32	lockCount;
   uint32	referenceCount;
};

#endif

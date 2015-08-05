/*
 * alloc.c
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
#include <gcom/gcom.h>
#include <util/lists.h>

/************************************************************************/
/* Prototypes								*/
/************************************************************************/

static HRESULT IMalloc_QueryInterface( IMalloc *, REFIID, void ** );
static uint32  IMalloc_AddRef        ( IMalloc * );
static uint32  IMalloc_Release       ( IMalloc * );
static void *  IMalloc_Alloc         ( IMalloc *, uint32 );
static void *  IMalloc_Realloc       ( IMalloc *, void *, uint32 );
static void    IMalloc_Free          ( IMalloc *, void * );
static uint32  IMalloc_GetSize       ( IMalloc *, void * );
static int     IMalloc_DidAlloc      ( IMalloc *, void * );
static void    IMalloc_HeapMinimize  ( IMalloc * );

/************************************************************************/
/* Coherency helper functions.						*/
/************************************************************************/

static void LockAllocList( void )
{
#warning Insert locking code to make thread-safe.
}

static void UnlockAllocList( void )
{
#warning Insert unlocking code to make thread-safe.
}

static void LockInitCount( void )
{
#warning Insert locking code to make thread-safe.
}

static void UnlockInitCount( void )
{
#warning Insert unlocking code to make thread-safe.
}

/************************************************************************/
/* The process' allocator object.  Yes, we're a real COM (singleton)	*/
/* object.								*/
/************************************************************************/

/*
 * This structure is used to keep track of which memory chunks we've
 * allocated.  With some minor changes to this source code, it can
 * also be used to help detect memory leaks.
 */

typedef struct
{
   Node		node;
   uint32	size;
} AllocNode;

static List allocList;

/*
 * The IUnknown methods, which all COM objects must support in one
 * way or another.
 */

static
HRESULT IMalloc_QueryInterface( IMalloc *self, REFIID riid, void **ppv )
{
   HRESULT hr;
   
   *ppv = NULL;
   hr = E_NOINTERFACE;
   
   if( IsEqualIID( riid, IID_IUnknown ) || IsEqualIID( riid, IID_IMalloc ) )
   {
      *ppv = self;
      IMalloc_AddRef( self );
      hr = S_OK;
   }

   return hr;
}

static
uint32 IMalloc_AddRef( IMalloc *self )
{
   /* Normal COM objects will implement reference counting here.  However,
    * since we're always present in the address space of the COM client,
    * there is no real reason to maintain a reference count.
    */

   return 1;	/* Client sees that we always have one reference count */
}

static
uint32 IMalloc_Release( IMalloc *self )
{
   /* Normal COM objects will implement reference counting here.  However,
    * since we're always present in the address space of the COM client,
    * there is no real reason to maintain a reference count.
    */

   return 0;	/* Client sees us as no longer having any references */
}

/*
 * The IMalloc methods.
 */

static
void *IMalloc_Alloc( IMalloc *self, uint32 cBytes )
{
   AllocNode *an;

   /* IMalloc requires us to implement the DidAlloc() function,
    * which returns non-zero if we allocated a particular chunk
    * of memory.
    * 
    * In order to properly implement that functionality, this
    * code allocates the requested memory, and adds that memory
    * to a doubly-linked list, maintained internally as part of
    * the object's state.  This way, we have a record of which
    * chunks of memory we did and did not allocate.
    */

   an = (AllocNode *)malloc( cBytes + sizeof( AllocNode ) );
   if( an != NULL )
   {
      /* Fill in some bookkeeping information. */

      an -> size = cBytes;	/* For the IMalloc::GetSize() function */
      LockAllocList();
      ListAddTail( &allocList, (Node *)an );
      UnlockAllocList();
      
      /* Now advance "an" to point to the memory requested by the client */
      an++;
   }

   return (void *)an;
}

static
void *IMalloc_Realloc( IMalloc *self, void *pv, uint32 cBytes )
{
   AllocNode *an1 = ( (AllocNode *)pv ) - 1;
   AllocNode *an2;

   /*
    * Unlike the other memory allocation functions, we must lock the
    * list for the entire duration of the realloc operation.  This is
    * because we must, essentially, perform a "read-modify-write"
    * operation on the list, and it must be atomic.
    * 
    * The reason is realloc() *could* relocate a chunk of data, rather
    * than extending a chunk in-place.  As a result, we must remove the
    * allocated node from the list, realloc() it, and then place it back
    * onto the list.
    */

   LockAllocList();

   NodeRemove( (Node *)an1 );
   an2 = (AllocNode *)realloc( an1, cBytes + sizeof( AllocNode ) );
   
   if( an2 != NULL )
   {
      an2 -> size = cBytes;
      ListAddTail( &allocList, (Node *)an2 );
      UnlockAllocList();
      
      return (void *)( an2 + 1 );
   }
   else
   {
      ListAddTail( &allocList, (Node *)an1 );
      UnlockAllocList();

      return NULL;
   }
}

static
void IMalloc_Free( IMalloc *self, void *pv )
{
   AllocNode *an = ( (AllocNode *)pv ) - 1;
   
   LockAllocList();
   NodeRemove( (Node *)an );
   UnlockAllocList();
   
   free( an );
}

static
uint32 IMalloc_GetSize( IMalloc *self, void *pv )
{
   AllocNode *an = ( (AllocNode *)pv ) - 1;
   
   return an -> size;
}

static
int IMalloc_DidAlloc( IMalloc *self, void *pv )
{
   AllocNode *anGiven = ( (AllocNode *)pv ) - 1;
   AllocNode *an;
   int result = FALSE;

   LockAllocList();

   for(		/* NOTE: THIS IS A BRUTE-FORCE IMPLEMENTATION.  It's slow! */
       an = (AllocNode *)allocList.head;
       ( result == FALSE ) && ( an -> node.next );
       an = (AllocNode *)an -> node.next
      )
   {
      if( an == anGiven )
	      result = TRUE;
   }

   UnlockAllocList();

   return result;
}

static
void IMalloc_HeapMinimize( IMalloc *self )
{
   /* Do nothing -- ANSI C provides no counterpart. */
}

/************************************************************************/
/* Component Initialization and Uninitialization Functions		*/
/************************************************************************/

static const IMallocVtbl TaskMallocVtbl =
{
   &IMalloc_QueryInterface,
   &IMalloc_AddRef,
   &IMalloc_Release,
   &IMalloc_Alloc,
   &IMalloc_Realloc,
   &IMalloc_Free,
   &IMalloc_GetSize,
   &IMalloc_DidAlloc,
   &IMalloc_HeapMinimize
};

static const IMalloc TaskMalloc =
{
   &TaskMallocVtbl
};

static IMalloc *pTaskMalloc = &TaskMalloc;
static uint32 initCount = 0;

/*
 * TaskMallocInitialize() and TaskMallocUninitialize() are called
 * by CoInitialize() and CoUninitialize(), respectively.  Threads should
 * never, ever call these functions directly.
 */

HRESULT TaskMallocInitialize( void )
{
   initCount++;
   if( initCount == 1 )
   {
      ListInitialize( &allocList );
      return S_OK;
   }
   
   return S_FALSE;
}

HRESULT TaskMallocUninitialize( void )
{
   AllocNode *an, *nan;

   if( initCount != 1 )
   {
      initCount--;
      return S_FALSE;
   }

   LockAllocList();
   an = (AllocNode *)( allocList.head );
   while( an -> node.next )
   {
      nan = (AllocNode *)an -> node.next;
      NodeRemove( (Node *)an );
      free( an );

      an = nan;
   }
   UnlockAllocList();

   initCount--;
   return S_OK;
}

/************************************************************************/
/* Convenient Wrappers for the process' IMalloc object.			*/
/************************************************************************/

/**
 * This function returns an allocator object exposing the IMalloc
 * interface.  There are different types of allocators which can be
 * requested, though support for all allocators except one is optional.
 * For more information on memory allocators, please refer to the Microsoft
 * COM 0.9 specifications document.
 *
 * NOTE FOR AMIGA USERS: Elate operates without memory protection; as a
 * result, the task allocator is simultaneously also the shared memory
 * allocator.  As a result, MEMCTX_SHARED and MEMCTX_TASK both return
 * the same object.  Please note, however, that this can change in future
 * releases of either Elate or Ami environments.  Do not depend on this
 * behavior.
 *
 * @param memctx
 * This parameter specifies the memory allocator desired.  There are
 * currently five types of allocators specified in the COM specification.
 * However, all but one is an optional feature, and are platform dependent
 * in nature.  Currently MEMCTX_TASK and MEMCTX_SHARED are supported.
 * 
 * @param ppMalloc
 * This parameter points to a variable used to hold the resulting interface
 * pointer for the requested IMalloc interface.
 * 
 * @returns
 * S_OK if MEMCTX_TASK or MEMCTX_SHARED was specified for the memory context.
 * 
 * E_INVALIDARG if the specified memory context isn't supported.  Currently
 * only MEMCTX_TASK and MEMCTX_SHARED are supported.
 * 
 * E_NOMEMORY if there wasn't enough memory to create the memory allocator
 * object.  This implementation of GCOM never returns this specific error,
 * but do not depend on this behavior.
 *
 * @see CoTaskMemAlloc
 * @see CoTaskMemRealloc
 * @see CoTaskMemFree
 */

HRESULT CoGetMalloc( MEMCTX memctx, IMalloc **ppMalloc )
{
   *ppMalloc = NULL;
   
   if( ( memctx == MEMCTX_TASK ) || ( memctx == MEMCTX_SHARED ) )
   {
      *ppMalloc = pTaskMalloc;
      IMalloc_AddRef( pTaskMalloc );
      return S_OK;
   }
   
   return E_INVALIDARG;
}

/**
 * This function is a wrapper, semantically equivalent to the following:
 * 
 * 	IMalloc *pmalloc;
 * 	HRESULT hr;
 * 	void *result;
 * 
 * 	hr = CoGetMalloc( MEMCTX_TASK, &pmalloc );
 * 	if( SUCCEEDED( hr ) )
 * 	{
 * 	   result = pmalloc -> lpVtbl -> Alloc( pmalloc, cBytes );
 * 	   pmalloc -> lpVtbl -> Release( pmalloc );
 * 	}
 * 
 * 	return result;
 * 
 * @param cBytes
 * This parameter specifies the size of the memory chunk to allocate.
 * 
 * @returns
 * NULL if the memory couldn't be allocated for some reason (e.g.,
 * out of memory, whatever).  Otherwise, a pointer to the memory chunk
 * is returned.
 * 
 * @see IMalloc::Alloc
 */

void *CoTaskMemAlloc( uint32 cBytes )
{
   /*
    * Note that we don't go through the riff-raff specified above.
    * We know that the allocator object is /always/ present in the
    * process, since we never unload it (regardless of its reference
    * count).  Therefore, there's no point in wasting CPU cycles on it.
    */

   if( initCount != 0 )
	   return IMalloc_Alloc( pTaskMalloc, cBytes );
   else
	   return NULL;
}

/**
 * This function is a wrapper, semantically equivalent to the following:
 * 
 * 	IMalloc *pmalloc;
 * 	HRESULT hr;
 * 	void *result, *pvMem;
 * 
 * 	hr = CoGetMalloc( MEMCTX_TASK, &pmalloc );
 * 	if( SUCCEEDED( hr ) )
 * 	{
 * 	   result = pmalloc -> lpVtbl -> Realloc( pmalloc, pvMem, cBytes );
 * 	   pmalloc -> lpVtbl -> Release( pmalloc );
 * 	}
 * 
 * 	return result;
 * 
 * @param pvMem
 * This is a pointer to a block of memory, as returned by CoTaskMemAlloc(),
 * or the process' IMalloc::Alloc() method.
 * 
 * @param cBytes
 * This parameter specifies the new size of the memory chunk.
 * 
 * @returns
 * NULL if the memory couldn't be allocated for some reason (e.g.,
 * out of memory, whatever).  Otherwise, a pointer to the memory chunk
 * is returned.
 * 
 * @see IMalloc::Alloc
 * @see CoGetMalloc
 */

void *CoTaskMemRealloc( void *pvMem, uint32 cb )
{
   /*
    * Note that we don't go through the riff-raff specified above.
    * We know that the allocator object is /always/ present in the
    * process, since we never unload it (regardless of its reference
    * count).  Therefore, there's no point in wasting CPU cycles on it.
    */

   if( initCount != 0 )
	   return IMalloc_Realloc( pTaskMalloc, pvMem, cb );
   else
	   return NULL;
}

/**
 * This function is a wrapper function, semantically equivalent to
 * the following:
 * 
 * 	HRESULT hr;
 * 	void *pvMem;
 * 	IMalloc *pmalloc;
 * 
 * 	hr = CoGetMalloc( MEMCTX_TASK, &pmalloc );
 * 	if( SUCCEEDED( hr ) )
 * 	{
 *	   pmalloc -> lpVtbl -> Free( pmalloc, pvMem );
 * 	}
 * 
 * This function frees a chunk of memory previously allocated by the
 * CoTaskMemAlloc() function, or by the *task's* IMalloc::Alloc() method.
 * 
 * @param pvMem
 * This is a pointer to the memory block to deallocate.
 * 
 * @returns Nothing.
 */

void CoTaskMemFree( void *pvMem )
{
   if( initCount != 0 )
	   IMalloc_Free( pTaskMalloc, pvMem );
}

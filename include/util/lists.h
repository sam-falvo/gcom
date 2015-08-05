#ifndef DOLPHIN_LISTS_H
#define DOLPHIN_LISTS_H

/*
 * util/lists.h
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

#include <gcom/types.h>

/*** The different types of nodes...
 * 
 * Note that there are two types of lists (both use the same structure).
 * The first kind stores nodes in no particular order, and are often used
 * to represent sets, queues, deques, etc.  Such lists can contain
 * either type of node.
 * 
 * The other kind contains nodes which are in a 'priority' order.  This
 * kind of list is used extensively by the task dispatcher, where the
 * priority of the task is simply the node's priority.  This type of
 * list can contain *ONLY* PriorityNode structures.
 */

typedef struct _Node
{
   struct _Node *	next;
   struct _Node *	previous;
} Node;

typedef struct _PriorityNode
{
   struct _PriorityNode *	next;
   struct _PriorityNode *	previous;
   int16			priority;	/* -32768 to 32767 */
   uint16			padding;	/* DO NOT USE. */
} PriorityNode;

/*** System-wide list ***/

typedef struct 
{
   Node *	head;
   void *	nullPointer;	/* Must always be NULL */
   Node *	tail;
} List;

/* Conditions that any given list can be in... */

typedef enum _ListState
{
   LS_EMPTY		= 0,
   LS_NONEMPTY,
   LS_BAD
} ListState;

/*** PROTOTYPES ***/

void ListInitialize( List * );
ListState ListGetState( List * );

void NodeInsert( Node *, Node * );
void NodeRemove( Node * );
void ListPriorityInsert( List *, PriorityNode * );

void ListAddHead( List *, Node * );
Node *ListPeekHead( List * );
Node *ListRemoveHead( List * );

void ListAddTail( List *, Node * );
Node *ListPeekTail( List * );
Node *ListRemoveTail( List * );

#endif

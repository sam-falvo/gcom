/*
 * lists.c
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
#include <util/lists.h>

/****** ListInitialize ***************************************************
 * 
 * NAME
 * 	ListInitialize
 * 
 * SYNOPSIS
 * 	#include <exo/lists.h>
 * 
 * 	ListInitialize( list );
 * 
 * 	List *list;
 * 
 * FUNCTION
 * 	Prepares the list for use.  Whatever nodes that were on that
 * 	list can no longer be accessed from the list.  This function
 * 	does NOT deallocate the nodes on the list.
 * 
 * INPUTS
 * 	list		The list to initialize.
 * 
 * RESULT
 * 
 * BUGS
 * 
 * SEE ALSO
 * 	ListState
 * 
 ************************************************************************/

void ListInitialize( List *list )
{
   list -> head = (Node *)( &list -> nullPointer );
   list -> nullPointer = NULL;
   list -> tail = (Node *)list;
}

/****** ListGetState *****************************************************
 * 
 * NAME
 * 	ListGetState
 * 
 * SYNOPSIS
 * 	#include <exo/lists.h>
 * 
 * 	state = ListGetState( list );
 * 
 * 	ListState	state;
 * 	List 		*list;
 * 
 * FUNCTION
 * 	Determines the current state of the list.  You'd use this function
 * 	to test if the list was empty or not, of if it's in an uninitialized
 * 	state.
 * 
 * INPUTS
 * 	list		The list to query the state of.
 * 
 * RESULT
 * 	LS_EMPTY	The list is initialized, but empty.
 * 	LS_NONEMPTY	The list is initialized, but non-empty.
 * 	LS_BAD		The list is not initialized, or in an erroneous
 * 			state.
 * 
 * BUGS
 *
 * SEE ALSO
 * 	ListInitialize
 * 
 ************************************************************************/

ListState ListGetState( List *list ) 
{
   Node *headNode, *tailNode;
   
   headNode = list -> head;
   tailNode = list -> tail;

   if(
         ( headNode == (Node *)( &list -> nullPointer ) )
      && ( list -> nullPointer == NULL )
      && ( tailNode == (Node *)list )
     )
      return LS_EMPTY;
   
   if(
         ( headNode -> previous == (Node *)list )
      && ( tailNode -> next == (Node *)( &list -> nullPointer ) )
     )
      return LS_NONEMPTY;

   return LS_BAD;
}

/****** ListAddHead ******************************************************
 * 
 * NAME
 * 	ListAddHead
 * 
 * SYNOPSIS
 * 	#include <exo/lists.h>
 * 
 * 	ListAddHead( list, node );
 * 
 * 	List *list;
 * 	Node *node;
 * 
 * FUNCTION
 * 	Adds node to the head of the list.
 * 
 * INPUTS
 * 	list		The list to add the node to.
 * 	node		The node to add.
 * 
 * RESULT
 * 
 * BUGS
 * 
 * SEE ALSO
 * 	ListAddTail, ListRemoveHead, ListRemoveTail, ListPeekHead,
 *	ListPeekTail
 * 
 ************************************************************************/

void ListAddHead( List *list, Node *node )
{
   NodeInsert( (Node *)list, node );
}

/****** ListAddTail ******************************************************
 * 
 * NAME
 * 	ListAddTail
 * 
 * SYNOPSIS
 * 	#include <exo/lists.h>
 * 
 * 	ListAddTail( list, node )
 * 
 * 	List *list;
 * 	Node *node;
 * 
 * FUNCTION
 * 	Adds the given node to the tail of the list.
 * 
 * INPUTS
 * 	list	The list to add the node to.
 * 	node	The node to add.
 * 
 * RESULT
 * 
 * BUGS
 * 
 * SEE ALSO
 * 	ListAddHead, ListRemoveHead, ListRemoveTail, ListPeekHead,
 *	ListPeekTail
 * 
 ************************************************************************/

void ListAddTail( List *list, Node *node )
{
   NodeInsert( list -> tail, node );
}

/****** ListPeekHead *****************************************************
 * 
 * NAME
 * 	ListPeekHead
 * 
 * SYNOPSIS
 * 	#include <exo/lists.h>
 * 
 * 	node = ListPeekHead( list );
 * 
 * 	Node *node;
 * 	List *list;
 * 
 * FUNCTION
 * 	Obtains the first node on the list, but DOES NOT remove it.
 * 	To actually remove the node, either call ListRemoveHead(), or
 * 	NodeRemove().
 * 
 * INPUTS
 * 	list		The list which potentially contains the desired
 * 			node.
 * 
 * RESULT
 * 	A pointer to the head node if there is one, or NULL if no nodes
 * 	are available on the list.
 * 
 * BUGS
 * 
 * SEE ALSO
 * 	ListAddHead, ListRemoveHead, ListAddTail, ListRemoveTail
 * 
 ************************************************************************/

Node *ListPeekHead( List *list )
{
   ListState condition = ListGetState( list );
   
   if( condition == LS_NONEMPTY )
      return list -> head;
   
   return NULL;
}

/****** ListPeekTail *****************************************************
 * 
 * NAME
 * 	ListPeekTail
 * 
 * SYNOPSIS
 * 	#include <exo/lists.h>
 * 
 * 	node = ListPeekTail( list );
 * 
 * 	Node *node;
 * 	List *list;
 * 
 * FUNCTION
 * 	Obtains the tail node on the list, but DOES NOT remove it.
 * 	To actually remove the node, either call ListRemoveTail(), or
 * 	NodeRemove().
 * 
 * INPUTS
 * 	list		The list which potentially contains the desired
 * 			node.
 * 
 * RESULT
 * 	A pointer to the tail node if there is one, or NULL if no nodes
 * 	are available on the list.
 * 
 * BUGS
 * 
 * SEE ALSO
 * 	ListAddHead, ListRemoveHead, ListAddTail, ListRemoveTail
 * 
 ************************************************************************/

Node *ListPeekTail( List *list )
{
   ListState condition = ListGetState( list );
   
   if( condition == LS_NONEMPTY )
      return list -> tail;

   return NULL;
}

/****** ListRemoveHead ***************************************************
 * 
 * NAME
 * 	ListRemoveHead
 * 
 * SYNOPSIS
 * 	#include <exo/lists.h>
 * 
 * 	node = ListRemoveHead( list );
 * 
 * 	Node *node;
 * 	List *list;
 * 
 * FUNCTION
 * 	Obtains the first node on the list, and removes it from the list.
 * 
 * INPUTS
 * 	list		The list which potentially contains the desired
 * 			node.
 * 
 * RESULT
 * 	A pointer to the head node if there is one, or NULL if no nodes
 * 	are available on the list.
 * 
 * BUGS
 * 
 * SEE ALSO
 * 	ListAddHead, ListPeekHead, ListAddTail, ListPeekTail
 * 
 ************************************************************************/

Node *ListRemoveHead( List *list )
{
   Node *node = ListPeekHead( list );
   
   if( node )
   {
      NodeRemove( node );
   }

   return node;
}

/****** ListRemoveTail ***************************************************
 * 
 * NAME
 * 	ListRemoveTail
 * 
 * SYNOPSIS
 * 	#include <exo/lists.h>
 * 
 * 	node = ListRemoveTail( list );
 * 
 * 	Node *node;
 * 	List *list;
 * 
 * FUNCTION
 *	Obtains the tail node on the list, and removes it from that
 * 	list.
 * 
 * INPUTS
 * 	list		The list which potentially contains the desired
 * 			node.
 * 
 * RESULT
 * 	A pointer to the tail node if there is one, or NULL if no nodes
 * 	are available on the list.
 * 
 * BUGS
 * 
 * SEE ALSO
 * 	ListAddHead, ListPeekHead, ListAddTail, ListPeekTail
 * 
 ************************************************************************/

Node *ListRemoveTail( List *list )
{
   Node *node = ListPeekTail( list );
   
   if( node )
   {
      NodeRemove( node );
   }

   return node;
}

/****** NodeInsert *******************************************************
 * 
 * NAME
 * 	NodeInsert
 * 
 * SYNOPSIS
 * 	#include <exo/lists.h>
 * 
 * 	NodeInsert( node1, node2 );
 * 
 * 	Node *node1, *node2;
 * 
 * FUNCTION
 * 	Inserts node2 AFTER node1.
 * 
 * INPUTS
 * 	node1		The node that is already in a list.
 * 	node2		The node to insert after node1.
 * 
 * RESULT
 * 
 * BUGS
 * 
 * SEE ALSO
 * 	NodeRemove
 * 
 ************************************************************************/

void NodeInsert( Node *node1, Node *node2 )
{
   Node *node3 = node1 -> next;
   
   node1 -> next = node2;
   node2 -> next = node3;
   node3 -> previous = node2;
   node2 -> previous = node1;
}

/****** NodeRemove *******************************************************
 * 
 * NAME
 * 	NodeRemove
 * 
 * SYNOPSIS
 * 	#include <exo/lists.h>
 * 
 * 	NodeRemove( node );
 * 
 * 	Node *node;
 * 
 * FUNCTION
 * 	Removes the node from whatever list it is currently on.
 * 
 * INPUTS
 * 	node		The node to remove from its list.
 * 
 * RESULT
 *
 * BUGS
 * 
 * SEE ALSO
 * 	NodeInsert
 * 
 ************************************************************************/

void NodeRemove( Node *node2 )
{
   Node *node1 = node2 -> previous;
   Node *node3 = node2 -> next;
   
   node1 -> next = node3;
   node3 -> previous = node1;
   
   node2 -> next = NULL;
   node2 -> previous = NULL;
}

/****** ListPriorityInsert ***********************************************
 * 
 * NAME
 * 	ListPriorityInsert
 * 
 * SYNOPSIS
 * 	ListPriorityInsert( list, node );
 * 
 * 	List *list;
 * 	PriorityNode *node;
 * 
 * FUNCTION
 * 	Inserts `node' into `list', based on its priority.  The higher
 * 	the priority, the closer to the head of the list it is placed.
 * 	If there are nodes on the list which have equal priority to `node',
 * 	then `node' is inserted after those nodes, thus forming a
 * 	prioritized first-in-first-out queue.
 * 
 * INPUTS
 * 	list		The list which contains PriorityNodes.
 * 	node		The PriorityNode to insert.  Unlike other List
 * 			functions, regular Node structures WILL NOT WORK.
 * 
 * RESULT
 * 
 * BUGS
 * 
 * SEE ALSO
 * 	ListAddHead, ListAddTail, ListRemoveHead, ListRemoveTail, NodeInsert
 * 
 ************************************************************************/

void ListPriorityInsert( List *list, PriorityNode *node2 )
{
   PriorityNode *node1;
   
   /* Step 1: Find out where we're going to insert the node
    * 
    * Unless you've implemented a function like this before, this is a pretty
    * mind-bending algorithm, despite its simplicity.  At least it is to me...
    * 
    * The idea is to start at the head of the list.  If the priority of the
    * current node in the list is greater than or equal to the supplied node's
    * priority, then we move on to the next node.  We abort this loop on two
    * conditions: we reach the end of the list (node1 -> next == NULL), or
    * we find a node where the priority of the list's node is lower than that
    * of the supplied node.
    * 
    * Note that the algorithm is designed to keep node1 set to the node
    * immediately AFTER the one we're really interested in.  Although this
    * looks like a brain-dead way of doing things, it's actually quite fast
    * and eliminates two special-cases from the algorithm, which shrinks the
    * final code size by a factor of 2 (!!).  The special cases are
    * (a) checking to see if the list is empty, and (b) checking to see if
    * we've reached the end of the list without inserting node2.
    * 
    * This algorithm tends to be hard for me to understand -- I usually need
    * to sit down with pen and paper to visualize how it works.
    */

   node1 = (PriorityNode *)( list -> head );
   while( node1 -> next )
   {
      if( node1 -> priority < node2 -> priority )
	      break;

      node1 = (PriorityNode *)( node1 -> next );
   }
   
   /* Step 2: Insert the new node accordingly */
   NodeInsert( (Node *)( node1 -> previous ), (Node *)node2 );
}

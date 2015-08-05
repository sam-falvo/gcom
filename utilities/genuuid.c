/*
 * genuuid.c
 * Dolphin Release 0.5.0
 * 
 * Copyright (c) 1999 Samuel A. Falvo II
 * All Rights Reserved.
 *
 * This program generates UUIDs according to the method documented in the
 * IETF draft-standard on UUIDs.  Please refer to
 * draft-leach-uuids-guids-01.txt.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

/* This is the offset from the time reference between gettimeofday()
 * (Jan 1 1970 UTC), and that required for UUIDs (Jan 1, 1582 UTC).
 */

const unsigned long long time_offset = 122443488000000000LL;

/* For now, we hardcode the developers Ethernet address into this node
 * structure.	In the future, we will need to fetch this either automatically
 * or via a configuration file.
 */

static char node[6] = { 0x00, 0x00, 0x01, 0x13, 0x56, 0x27 };

typedef struct
{
	unsigned long	time_lo;
	unsigned short	time_mid;
	unsigned short	time_hi;	/* and version */
	unsigned char	clockseq_hi;	/* I'd SURE like to know why... */
	unsigned char	clockseq_lo;	/* this is big-endian. */
	unsigned char	node[6];
} UUID;

UUID id;

void main( void )
{
	int i;
	struct timeval tv;
	unsigned long long hugeTime;

	/* Get all the required information. */

	gettimeofday( &tv, NULL );
	hugeTime = ( tv.tv_sec * 1000000LL + tv.tv_usec ) * 10LL;
	hugeTime += time_offset;

	/* Start filling out the UUID */
	
	id.time_lo  = (unsigned long)( hugeTime & 0x00000000FFFFFFFF );
	id.time_mid = (unsigned short)( ( hugeTime >> 32 ) & 0x0000FFFF );
	id.time_hi  = (unsigned short)( ( hugeTime >> 48 ) & 0x0000FFFF );
	id.time_hi  = ( id.time_hi & 0x0FFF ) | 0x1000;	/* Sets version */

	id.clockseq_hi &= 0x1F;
	id.clockseq_hi |= 0x80;	/* Sets UUID variant */
	
	memcpy( &id.node[0], node, 6 );
	
	/* Print out the resulting UUID */
	
	printf
	(
		"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}\n",
		id.time_lo, id.time_mid, id.time_hi,
		id.clockseq_hi, id.clockseq_lo,
		id.node[0], id.node[1], id.node[2],
		id.node[3], id.node[4], id.node[5]
	);
}

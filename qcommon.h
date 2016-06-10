/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
// qcommon.h -- definitions common between client and server, but not game.or ref modules
#ifndef _QCOMMON_H_
#define _QCOMMON_H_

typedef struct {
	qboolean	allowoverflow;	// if false, do a Com_Error
	qboolean	overflowed;		// set to true if the buffer size failed (with allowoverflow set)
	qboolean	oob;			// set to true if the buffer size failed (with allowoverflow set)
	byte	*data;
	int		maxsize;
	int		cursize;
	int		readcount;
	int		bit;				// for bitwise reads and writes
} msg_t;

void MSG_initHuffman( void );

void MSG_Init (msg_t *buf, byte *data, int length);
void MSG_InitOOB( msg_t *buf, byte *data, int length );
void MSG_Clear (msg_t *buf);
void MSG_WriteData (msg_t *buf, const void *data, int length);
void MSG_Bitstream( msg_t *buf );

// TTimo
// copy a msg_t in case we need to store it as is for a bit
// (as I needed this to keep an msg_t from a static var for later use)
// sets data buffer as MSG_Init does prior to do the copy
void MSG_Copy(msg_t *buf, byte *data, int length, msg_t *src);

void MSG_WriteBits( msg_t *msg, int value, int bits );

void MSG_WriteChar (msg_t *sb, int c);
void MSG_WriteByte (msg_t *sb, int c);
void MSG_WriteShort (msg_t *sb, int c);
void MSG_WriteLong (msg_t *sb, int c);
void MSG_WriteFloat (msg_t *sb, float f);
void MSG_WriteString (msg_t *sb, const char *s);
void MSG_WriteBigString (msg_t *sb, const char *s);
void MSG_WriteAngle (msg_t *sb, float f);
void MSG_WriteAngle16 (msg_t *sb, float f);
int MSG_HashKey(const char *string, int maxlen);

void	MSG_BeginReading (msg_t *sb);
void	MSG_BeginReadingOOB(msg_t *sb);

int		MSG_ReadBits( msg_t *msg, int bits );

int		MSG_ReadChar (msg_t *sb);
int		MSG_ReadByte (msg_t *sb);
int		MSG_ReadShort (msg_t *sb);
int		MSG_ReadLong (msg_t *sb);
float	MSG_ReadFloat (msg_t *sb);
char	*MSG_ReadString (msg_t *sb);
char	*MSG_ReadBigString (msg_t *sb);
char	*MSG_ReadStringLine (msg_t *sb);
float MSG_ReadAngle( msg_t *sb );
float	MSG_ReadAngle16 (msg_t *sb);
void	MSG_ReadData (msg_t *sb, void *buffer, int size);
int		MSG_LookaheadByte (msg_t *msg);

void MSG_WriteDelta( msg_t *msg, int oldV, int newV, int bits );
int MSG_ReadDelta( msg_t *msg, int oldV, int bits );
void MSG_WriteDeltaFloat( msg_t *msg, float oldV, float newV );
float MSG_ReadDeltaFloat( msg_t *msg, float oldV );

void MSG_WriteDeltaKey( msg_t *msg, int key, int oldV, int newV, int bits );
int MSG_ReadDeltaKey( msg_t *msg, int key, int oldV, int bits );
void MSG_WriteDeltaKeyFloat( msg_t *msg, int key, float oldV, float newV );
float MSG_ReadDeltaKeyFloat( msg_t *msg, int key, float oldV );

#define MAX_MSGLEN        16384   // max length of a message, which may
                      // be fragmented into multiple packets

/* This is based on the Adaptive Huffman algorithm described in Sayood's Data
 * Compression book.  The ranks are not actually stored, but implicitly defined
 * by the location of a node within a doubly-linked list */

#define NYT HMAX          /* NYT = Not Yet Transmitted */
#define INTERNAL_NODE (HMAX+1)

typedef struct nodetype {
  struct  nodetype *left, *right, *parent; /* tree structure */
  struct  nodetype *next, *prev; /* doubly-linked list */
  struct  nodetype **head; /* highest ranked node in block */
  int   weight;
  int   symbol;
} node_t;

#define HMAX 256 /* Maximum symbol */

typedef struct {
  int     blocNode;
  int     blocPtrs;

  node_t*   tree;
  node_t*   lhead;
  node_t*   ltail;
  node_t*   loc[HMAX+1];
  node_t**  freelist;

  node_t    nodeList[768];
  node_t*   nodePtrs[768];
} huff_t;

typedef struct {
  huff_t    compressor;
  huff_t    decompressor;
} huffman_t;

void  Huff_Compress(msg_t *buf, int offset);
void  Huff_Decompress(msg_t *buf, int offset);
void  Huff_Init(huffman_t *huff);
void  Huff_addRef(huff_t* huff, byte ch);
int   Huff_Receive (node_t *node, int *ch, byte *fin);
void  Huff_transmit (huff_t *huff, int ch, byte *fout);
void  Huff_offsetReceive (node_t *node, int *ch, byte *fin, int *offset);
void  Huff_offsetTransmit (huff_t *huff, int ch, byte *fout, int *offset);
void  Huff_putBit( int bit, byte *fout, int *offset);
int   Huff_getBit( byte *fout, int *offset);

// don't use if you don't know what you're doing.
int   Huff_getBloc(void);
void  Huff_setBloc(int _bloc);

#endif // _QCOMMON_H_

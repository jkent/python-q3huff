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
#include <string.h>
#include "q_shared.h"
#include "qcommon.h"

static huffman_t		msgHuff;

static qboolean			msgInit = qfalse;

/*
==============================================================================

			MESSAGE IO FUNCTIONS

Handles byte ordering and avoids alignment errors
==============================================================================
*/

void MSG_Init( msg_t *buf, byte *data, int length ) {
	if (!msgInit) {
		MSG_initHuffman();
	}
	memset (buf, 0, sizeof(*buf));
	buf->data = data;
	buf->maxsize = length;
}

void MSG_InitOOB( msg_t *buf, byte *data, int length ) {
	if (!msgInit) {
		MSG_initHuffman();
	}
	memset (buf, 0, sizeof(*buf));
	buf->data = data;
	buf->maxsize = length;
	buf->oob = qtrue;
}

void MSG_Clear( msg_t *buf ) {
	buf->cursize = 0;
	buf->overflowed = qfalse;
	buf->bit = 0;					//<- in bits
}


void MSG_Bitstream( msg_t *buf ) {
	buf->oob = qfalse;
}

void MSG_BeginReading( msg_t *msg ) {
	msg->readcount = 0;
	msg->bit = 0;
	msg->oob = qfalse;
}

void MSG_BeginReadingOOB( msg_t *msg ) {
	msg->readcount = 0;
	msg->bit = 0;
	msg->oob = qtrue;
}

void MSG_Copy(msg_t *buf, byte *data, int length, msg_t *src)
{
	if (length<src->cursize) {
		return; //Com_Error( ERR_DROP, "MSG_Copy: can't copy into a smaller msg_t buffer");
	}
	memcpy(buf, src, sizeof(msg_t));
	buf->data = data;
	memcpy(buf->data, src->data, src->cursize);
}

/*
=============================================================================

bit functions
  
=============================================================================
*/

// negative bit values include signs
void MSG_WriteBits( msg_t *msg, int value, int bits ) {
	int	i;
//	FILE*	fp;

	// this isn't an exact overflow check, but close enough
	if ( msg->maxsize - msg->cursize < 4 ) {
		msg->overflowed = qtrue;
		return;
	}

	if ( bits == 0 || bits < -31 || bits > 32 ) {
		return; //Com_Error( ERR_DROP, "MSG_WriteBits: bad bits %i", bits );
	}

	if ( bits < 0 ) {
		bits = -bits;
	}
	if (msg->oob) {
		if(bits==8)
		{
			msg->data[msg->cursize] = value;
			msg->cursize += 1;
			msg->bit += 8;
		}
		else if(bits==16)
		{
			short temp = value;
			
			CopyLittleShort(&msg->data[msg->cursize], &temp);
			msg->cursize += 2;
			msg->bit += 16;
		}
		else if(bits==32)
		{
			CopyLittleLong(&msg->data[msg->cursize], &value);
			msg->cursize += 4;
			msg->bit += 32;
		}
		else 
			return; //Com_Error(ERR_DROP, "can't write %d bits", bits);
	} else {
//		fp = fopen("c:\\netchan.bin", "a");
		value &= (0xffffffff>>(32-bits));
		if (bits&7) {
			int nbits;
			nbits = bits&7;
			for(i=0;i<nbits;i++) {
				Huff_putBit((value&1), msg->data, &msg->bit);
				value = (value>>1);
			}
			bits = bits - nbits;
		}
		if (bits) {
			for(i=0;i<bits;i+=8) {
//				fwrite(bp, 1, 1, fp);
				Huff_offsetTransmit (&msgHuff.compressor, (value&0xff), msg->data, &msg->bit);
				value = (value>>8);
			}
		}
		msg->cursize = (msg->bit>>3)+1;
//		fclose(fp);
	}
}

int MSG_ReadBits( msg_t *msg, int bits ) {
	int			value;
	int			get;
	qboolean	sgn;
	int			i, nbits;
//	FILE*	fp;

	value = 0;

	if ( bits < 0 ) {
		bits = -bits;
		sgn = qtrue;
	} else {
		sgn = qfalse;
	}

	if (msg->oob) {
		if(bits==8)
		{
			value = msg->data[msg->readcount];
			msg->readcount += 1;
			msg->bit += 8;
		}
		else if(bits==16)
		{
			short temp;
			
			CopyLittleShort(&temp, &msg->data[msg->readcount]);
			value = temp;
			msg->readcount += 2;
			msg->bit += 16;
		}
		else if(bits==32)
		{
			CopyLittleLong(&value, &msg->data[msg->readcount]);
			msg->readcount += 4;
			msg->bit += 32;
		}
		else
			return 0; //Com_Error(ERR_DROP, "can't read %d bits", bits);
	} else {
		nbits = 0;
		if (bits&7) {
			nbits = bits&7;
			for(i=0;i<nbits;i++) {
				value |= (Huff_getBit(msg->data, &msg->bit)<<i);
			}
			bits = bits - nbits;
		}
		if (bits) {
//			fp = fopen("c:\\netchan.bin", "a");
			for(i=0;i<bits;i+=8) {
				Huff_offsetReceive (msgHuff.decompressor.tree, &get, msg->data, &msg->bit);
//				fwrite(&get, 1, 1, fp);
				value |= (get<<(i+nbits));
			}
//			fclose(fp);
		}
		msg->readcount = (msg->bit>>3)+1;
	}
	if ( sgn && bits > 0 && bits < 32 ) {
		if ( value & ( 1 << ( bits - 1 ) ) ) {
			value |= -1 ^ ( ( 1 << bits ) - 1 );
		}
	}

	return value;
}



//================================================================================

//
// writing functions
//

void MSG_WriteChar( msg_t *sb, int c ) {
#ifdef PARANOID
	if (c < -128 || c > 127)
		return; //Com_Error (ERR_FATAL, "MSG_WriteChar: range error");
#endif

	MSG_WriteBits( sb, c, 8 );
}

void MSG_WriteByte( msg_t *sb, int c ) {
#ifdef PARANOID
	if (c < 0 || c > 255)
		return; //Com_Error (ERR_FATAL, "MSG_WriteByte: range error");
#endif

	MSG_WriteBits( sb, c, 8 );
}

void MSG_WriteData( msg_t *buf, const void *data, int length ) {
	int i;
	for(i=0;i<length;i++) {
		MSG_WriteByte(buf, ((byte *)data)[i]);
	}
}

void MSG_WriteShort( msg_t *sb, int c ) {
#ifdef PARANOID
	if (c < ((short)0x8000) || c > (short)0x7fff)
		return; //Com_Error (ERR_FATAL, "MSG_WriteShort: range error");
#endif

	MSG_WriteBits( sb, c, 16 );
}

void MSG_WriteLong( msg_t *sb, int c ) {
	MSG_WriteBits( sb, c, 32 );
}

void MSG_WriteFloat( msg_t *sb, float f ) {
	floatint_t dat;
	dat.f = f;
	MSG_WriteBits( sb, dat.i, 32 );
}

void MSG_WriteString( msg_t *sb, const char *s ) {
	if ( !s ) {
		MSG_WriteData (sb, "", 1);
	} else {
		int		l,i;
		char	string[MAX_STRING_CHARS];

		l = strlen( s );
		if ( l >= MAX_STRING_CHARS ) {
			//Com_Printf( "MSG_WriteString: MAX_STRING_CHARS" );
			MSG_WriteData (sb, "", 1);
			return;
		}
		strncpy( string, s, sizeof( string ) );
		string[sizeof(string)-1] = '\0';

		// get rid of 0x80+ and '%' chars, because old clients don't like them
		for ( i = 0 ; i < l ; i++ ) {
			if ( ((byte *)string)[i] > 127 /*|| string[i] == '%'*/ ) {
				string[i] = '.';
			}
		}

		MSG_WriteData (sb, string, l+1);
	}
}

void MSG_WriteBigString( msg_t *sb, const char *s ) {
	if ( !s ) {
		MSG_WriteData (sb, "", 1);
	} else {
		int		l,i;
		char	string[BIG_INFO_STRING];

		l = strlen( s );
		if ( l >= BIG_INFO_STRING ) {
			//Com_Printf( "MSG_WriteString: BIG_INFO_STRING" );
			MSG_WriteData (sb, "", 1);
			return;
		}
		strncpy( string, s, sizeof( string ) );
		string[sizeof(string) - 1] = '\0';

		// get rid of 0x80+ and '%' chars, because old clients don't like them
		for ( i = 0 ; i < l ; i++ ) {
			if ( ((byte *)string)[i] > 127 /*|| string[i] == '%'*/ ) {
				string[i] = '.';
			}
		}

		MSG_WriteData (sb, string, l+1);
	}
}

void MSG_WriteAngle( msg_t *sb, float f ) {
	MSG_WriteByte (sb, (int)(f*256/360) & 255);
}

void MSG_WriteAngle16( msg_t *sb, float f ) {
	MSG_WriteShort (sb, ANGLE2SHORT(f));
}


//============================================================

//
// reading functions
//

// returns -1 if no more characters are available
int MSG_ReadChar (msg_t *msg ) {
	int	c;
	
	c = (signed char)MSG_ReadBits( msg, 8 );
	if ( msg->readcount > msg->cursize ) {
		c = -1;
	}	
	
	return c;
}

int MSG_ReadByte( msg_t *msg ) {
	int	c;
	
	c = (unsigned char)MSG_ReadBits( msg, 8 );
	if ( msg->readcount > msg->cursize ) {
		c = -1;
	}	
	return c;
}

int MSG_LookaheadByte( msg_t *msg ) {
	const int bloc = Huff_getBloc();
	const int readcount = msg->readcount;
	const int bit = msg->bit;
	int c = MSG_ReadByte(msg);
	Huff_setBloc(bloc);
	msg->readcount = readcount;
	msg->bit = bit;
	return c;
}

int MSG_ReadShort( msg_t *msg ) {
	int	c;
	
	c = (short)MSG_ReadBits( msg, 16 );
	if ( msg->readcount > msg->cursize ) {
		c = -1;
	}	

	return c;
}

int MSG_ReadLong( msg_t *msg ) {
	int	c;
	
	c = MSG_ReadBits( msg, 32 );
	if ( msg->readcount > msg->cursize ) {
		c = -1;
	}	
	
	return c;
}

float MSG_ReadFloat( msg_t *msg ) {
	floatint_t dat;
	
	dat.i = MSG_ReadBits( msg, 32 );
	if ( msg->readcount > msg->cursize ) {
		dat.f = -1;
	}	
	
	return dat.f;	
}

char *MSG_ReadString( msg_t *msg ) {
	static char	string[MAX_STRING_CHARS];
	int		l,c;
	
	l = 0;
	do {
		c = MSG_ReadByte(msg);		// use ReadByte so -1 is out of bounds
		if ( c == -1 || c == 0 ) {
			break;
		}
		// translate all fmt spec to avoid crash bugs
		if ( c == '%' ) {
			c = '.';
		}
		// don't allow higher ascii values
		if ( c > 127 ) {
			c = '.';
		}

		string[l] = c;
		l++;
	} while (l < sizeof(string)-1);
	
	string[l] = 0;
	
	return string;
}

char *MSG_ReadBigString( msg_t *msg ) {
	static char	string[BIG_INFO_STRING];
	int		l,c;
	
	l = 0;
	do {
		c = MSG_ReadByte(msg);		// use ReadByte so -1 is out of bounds
		if ( c == -1 || c == 0 ) {
			break;
		}
		// translate all fmt spec to avoid crash bugs
		/*if ( c == '%' ) {
			c = '.';
		}*/
		// don't allow higher ascii values
		if ( c > 127 ) {
			c = '.';
		}

		string[l] = c;
		l++;
	} while (l < sizeof(string)-1);
	
	string[l] = 0;
	
	return string;
}

char *MSG_ReadStringLine( msg_t *msg ) {
	static char	string[MAX_STRING_CHARS];
	int		l,c;

	l = 0;
	do {
		c = MSG_ReadByte(msg);		// use ReadByte so -1 is out of bounds
		if (c == -1 || c == 0 || c == '\n') {
			break;
		}
		// translate all fmt spec to avoid crash bugs
		/*if ( c == '%' ) {
			c = '.';
		}*/
		// don't allow higher ascii values
		if ( c > 127 ) {
			c = '.';
		}

		string[l] = c;
		l++;
	} while (l < sizeof(string)-1);
	
	string[l] = 0;
	
	return string;
}

float MSG_ReadAngle( msg_t *msg ) {
  return (MSG_ReadByte(msg) * (360.0/256));
}

float MSG_ReadAngle16( msg_t *msg ) {
	return SHORT2ANGLE(MSG_ReadShort(msg));
}

void MSG_ReadData( msg_t *msg, void *data, int len ) {
	int		i;

	for (i=0 ; i<len ; i++) {
		((byte *)data)[i] = MSG_ReadByte (msg);
	}
}

// a string hasher which gives the same hash value even if the
// string is later modified via the legacy MSG read/write code
int MSG_HashKey(const char *string, int maxlen) {
	int hash, i;

	hash = 0;
	for (i = 0; i < maxlen && string[i] != '\0'; i++) {
		if (string[i] & 0x80 || string[i] == '%')
			hash += '.' * (119 + i);
		else
			hash += string[i] * (119 + i);
	}
	hash = (hash ^ (hash >> 10) ^ (hash >> 20));
	return hash;
}

/*
=============================================================================

delta functions
  
=============================================================================
*/

//#define	LOG(x) if( cl_shownet && cl_shownet->integer == 4 ) { Com_Printf("%s ", x ); };
#define LOG(x)

void MSG_WriteDelta( msg_t *msg, int oldV, int newV, int bits ) {
	if ( oldV == newV ) {
		MSG_WriteBits( msg, 0, 1 );
		return;
	}
	MSG_WriteBits( msg, 1, 1 );
	MSG_WriteBits( msg, newV, bits );
}

int	MSG_ReadDelta( msg_t *msg, int oldV, int bits ) {
	if ( MSG_ReadBits( msg, 1 ) ) {
		return MSG_ReadBits( msg, bits );
	}
	return oldV;
}

void MSG_WriteDeltaFloat( msg_t *msg, float oldV, float newV ) {
	floatint_t fi;
	if ( oldV == newV ) {
		MSG_WriteBits( msg, 0, 1 );
		return;
	}
	fi.f = newV;
	MSG_WriteBits( msg, 1, 1 );
	MSG_WriteBits( msg, fi.i, 32 );
}

float MSG_ReadDeltaFloat( msg_t *msg, float oldV ) {
	if ( MSG_ReadBits( msg, 1 ) ) {
		floatint_t fi;

		fi.i = MSG_ReadBits( msg, 32 );
		return fi.f;
	}
	return oldV;
}

/*
=============================================================================

delta functions with keys
  
=============================================================================
*/

int kbitmask[32] = {
	0x00000001, 0x00000003, 0x00000007, 0x0000000F,
	0x0000001F,	0x0000003F,	0x0000007F,	0x000000FF,
	0x000001FF,	0x000003FF,	0x000007FF,	0x00000FFF,
	0x00001FFF,	0x00003FFF,	0x00007FFF,	0x0000FFFF,
	0x0001FFFF,	0x0003FFFF,	0x0007FFFF,	0x000FFFFF,
	0x001FFFFf,	0x003FFFFF,	0x007FFFFF,	0x00FFFFFF,
	0x01FFFFFF,	0x03FFFFFF,	0x07FFFFFF,	0x0FFFFFFF,
	0x1FFFFFFF,	0x3FFFFFFF,	0x7FFFFFFF,	0xFFFFFFFF,
};

void MSG_WriteDeltaKey( msg_t *msg, int key, int oldV, int newV, int bits ) {
	if ( oldV == newV ) {
		MSG_WriteBits( msg, 0, 1 );
		return;
	}
	MSG_WriteBits( msg, 1, 1 );
	MSG_WriteBits( msg, newV ^ key, bits );
}

int	MSG_ReadDeltaKey( msg_t *msg, int key, int oldV, int bits ) {
	if ( MSG_ReadBits( msg, 1 ) ) {
		return MSG_ReadBits( msg, bits ) ^ (key & kbitmask[ bits - 1 ]);
	}
	return oldV;
}

void MSG_WriteDeltaKeyFloat( msg_t *msg, int key, float oldV, float newV ) {
	floatint_t fi;
	if ( oldV == newV ) {
		MSG_WriteBits( msg, 0, 1 );
		return;
	}
	fi.f = newV;
	MSG_WriteBits( msg, 1, 1 );
	MSG_WriteBits( msg, fi.i ^ key, 32 );
}

float MSG_ReadDeltaKeyFloat( msg_t *msg, int key, float oldV ) {
	if ( MSG_ReadBits( msg, 1 ) ) {
		floatint_t fi;

		fi.i = MSG_ReadBits( msg, 32 ) ^ key;
		return fi.f;
	}
	return oldV;
}

static int msg_hData[256] = {
250315, 41193,  6292,   7106,   3730,   3750,   6110,   23283,
33317,  6950,   7838,   9714,   9257,   17259,  3949,   1778,
8288,   1604,   1590,   1663,   1100,   1213,   1238,   1134,
1749,   1059,   1246,   1149,   1273,   4486,   2805,   3472,
21819,  1159,   1670,   1066,   1043,   1012,   1053,   1070,
1726,   888,    1180,   850,    960,    780,    1752,   3296,
10630,  4514,   5881,   2685,   4650,   3837,   2093,   1867,
2584,   1949,   1972,   940,    1134,   1788,   1670,   1206,
5719,   6128,   7222,   6654,   3710,   3795,   1492,   1524,
2215,   1140,   1355,   971,    2180,   1248,   1328,   1195,
1770,   1078,   1264,   1266,   1168,   965,    1155,   1186,
1347,   1228,   1529,   1600,   2617,   2048,   2546,   3275,
2410,   3585,   2504,   2800,   2675,   6146,   3663,   2840,
14253,  3164,   2221,   1687,   3208,   2739,   3512,   4796,
4091,   3515,   5288,   4016,   7937,   6031,   5360,   3924,
4892,   3743,   4566,   4807,   5852,   6400,   6225,   8291,
23243,  7838,   7073,   8935,   5437,   4483,   3641,   5256,
5312,   5328,   5370,   3492,   2458,   1694,   1821,   2121,
1916,   1149,   1516,   1367,   1236,   1029,   1258,   1104,
1245,   1006,   1149,   1025,   1241,   952,    1287,   997,
1713,   1009,   1187,   879,    1099,   929,    1078,   951,
1656,   930,    1153,   1030,   1262,   1062,   1214,   1060,
1621,   930,    1106,   912,    1034,   892,    1158,   990,
1175,   850,    1121,   903,    1087,   920,    1144,   1056,
3462,   2240,   4397,   12136,  7758,   1345,   1307,   3278,
1950,   886,    1023,   1112,   1077,   1042,   1061,   1071,
1484,   1001,   1096,   915,    1052,   995,    1070,   876,
1111,   851,    1059,   805,    1112,   923,    1103,   817,
1899,   1872,   976,    841,    1127,   956,    1159,   950,
7791,   954,    1289,   933,    1127,   3207,   1020,   927,
1355,   768,    1040,   745,    952,    805,    1073,   740,
1013,   805,    1008,   796,    996,    1057,   11457,  13504,
};

void MSG_initHuffman( void ) {
	int i,j;

	msgInit = qtrue;
	Huff_Init(&msgHuff);
	for(i=0;i<256;i++) {
		for (j=0;j<msg_hData[i];j++) {
			Huff_addRef(&msgHuff.compressor,	(byte)i);			// Do update
			Huff_addRef(&msgHuff.decompressor,	(byte)i);			// Do update
		}
	}
}

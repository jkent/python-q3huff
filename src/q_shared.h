#ifndef __Q_SHARED_H
#define __Q_SHARED_H

#include <stddef.h>

#define Q3_LITTLE_ENDIAN

//endianness
void CopyShortSwap (void *dest, void *src);
void CopyLongSwap (void *dest, void *src);
short ShortSwap (short l);
int LongSwap (int l);
float FloatSwap (const float *f);

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

#define CopyLittleShort(dest, src) CopyShortSwap(dest, src)
#define CopyLittleLong(dest, src) CopyLongSwap(dest, src)
#define LittleShort(x) ShortSwap(x)
#define LittleLong(x) LongSwap(x)
#define LittleFloat(x) FloatSwap(&x)
#define BigShort
#define BigLong
#define BigFloat

#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

#define CopyLittleShort(dest, src) memcpy(dest, src, 2)
#define CopyLittleLong(dest, src) memcpy(dest, src, 4)
#define LittleShort
#define LittleLong
#define LittleFloat
#define BigShort(x) ShortSwap(x)
#define BigLong(x) LongSwap(x)
#define BigFloat(x) FloatSwap(&x)

#else
#error "Byte order not supported"
#endif

typedef unsigned char 		byte;

typedef enum {qfalse, qtrue}	qboolean;

typedef union {
  float f;
  int i;
  unsigned int ui;
} floatint_t;

#define ARRAY_LEN(x)      (sizeof(x) / sizeof(*(x)))
#define STRARRAY_LEN(x)     (ARRAY_LEN(x) - 1)

#define MAX_STRING_CHARS  1024  // max length of a string passed to Cmd_TokenizeString
#define BIG_INFO_STRING   8192  // used for system info key only

typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef vec_t vec5_t[5];

// usercmd_t is sent to the server each client frame
typedef struct usercmd_s {
  int       serverTime;
  int       angles[3];
  int       buttons;
  byte      weapon;           // weapon
  signed char forwardmove, rightmove, upmove;
} usercmd_t;

typedef enum {
  TR_STATIONARY,
  TR_INTERPOLATE,       // non-parametric, but interpolate between snapshots
  TR_LINEAR,
  TR_LINEAR_STOP,
  TR_SINE,          // value = base + sin( time / duration ) * delta
  TR_GRAVITY
} trType_t;

typedef struct {
  trType_t  trType;
  int   trTime;
  int   trDuration;     // if non 0, trTime + trDuration = stop time
  vec3_t  trBase;
  vec3_t  trDelta;      // velocity, etc
} trajectory_t;

typedef struct entityState_s {
  int   number;     // entity index
  int   eType;      // entityType_t
  int   eFlags;

  trajectory_t  pos;  // for calculating position
  trajectory_t  apos; // for calculating angles

  int   time;
  int   time2;

  vec3_t  origin;
  vec3_t  origin2;

  vec3_t  angles;
  vec3_t  angles2;

  int   otherEntityNum; // shotgun sources, etc
  int   otherEntityNum2;

  int   groundEntityNum;  // ENTITYNUM_NONE = in air

  int   constantLight;  // r + (g<<8) + (b<<16) + (intensity<<24)
  int   loopSound;    // constantly loop this sound

  int   modelindex;
  int   modelindex2;
  int   clientNum;    // 0 to (MAX_CLIENTS - 1), for players and corpses
  int   frame;

  int   solid;      // for client side prediction, trap_linkentity sets this properly

  int   event;      // impulse events -- muzzle flashes, footsteps, etc
  int   eventParm;

  // for players
  int   powerups;   // bit flags
  int   weapon;     // determines weapon and flash model, etc
  int   legsAnim;   // mask off ANIM_TOGGLEBIT
  int   torsoAnim;    // mask off ANIM_TOGGLEBIT

  int   generic1;
} entityState_t;

/*
========================================================================

  ELEMENTS COMMUNICATED ACROSS THE NET

========================================================================
*/

#define ANGLE2SHORT(x)  ((int)((x)*65536/360) & 65535)
#define SHORT2ANGLE(x)  ((x)*(360.0/65536))

#define SNAPFLAG_RATE_DELAYED 1
#define SNAPFLAG_NOT_ACTIVE   2 // snapshot used during connection and for zombies
#define SNAPFLAG_SERVERCOUNT  4 // toggled every map_restart so transitions can be detected

//
// per-level limits
//
#define MAX_CLIENTS     64    // absolute limit
#define MAX_LOCATIONS   64

#define GENTITYNUM_BITS   10    // don't need to send any more
#define MAX_GENTITIES   (1<<GENTITYNUM_BITS)

// entitynums are communicated with GENTITY_BITS, so any reserved
// values that are going to be communcated over the net need to
// also be in this range
#define ENTITYNUM_NONE    (MAX_GENTITIES-1)
#define ENTITYNUM_WORLD   (MAX_GENTITIES-2)
#define ENTITYNUM_MAX_NORMAL  (MAX_GENTITIES-2)


#define MAX_MODELS      256   // these are sent over the net as 8 bits
#define MAX_SOUNDS      256   // so they cannot be blindly increased


#define MAX_CONFIGSTRINGS 1024

// these are the only configstrings that the system reserves, all the
// other ones are strictly for servergame to clientgame communication
#define CS_SERVERINFO   0   // an info string with all the serverinfo cvars
#define CS_SYSTEMINFO   1   // an info string for server system to client system configuration (timescale, etc)

#define RESERVED_CONFIGSTRINGS  2 // game can't modify below this, only the system can

#define MAX_GAMESTATE_CHARS 16000
typedef struct {
  int     stringOffsets[MAX_CONFIGSTRINGS];
  char    stringData[MAX_GAMESTATE_CHARS];
  int     dataCount;
} gameState_t;

//=========================================================

// bit field limits
#define MAX_STATS       16
#define MAX_PERSISTANT      16
#define MAX_POWERUPS      16
#define MAX_WEAPONS       16

#define MAX_PS_EVENTS     2

#define PS_PMOVEFRAMECOUNTBITS  6

// playerState_t is the information needed by both the client and server
// to predict player motion and actions
// nothing outside of pmove should modify these, or some degree of prediction error
// will occur

// you can't add anything to this without modifying the code in msg.c

// playerState_t is a full superset of entityState_t as it is used by players,
// so if a playerState_t is transmitted, the entityState_t can be fully derived
// from it.
typedef struct playerState_s {
  int     commandTime;  // cmd->serverTime of last executed command
  int     pm_type;
  int     bobCycle;   // for view bobbing and footstep generation
  int     pm_flags;   // ducked, jump_held, etc
  int     pm_time;

  vec3_t    origin;
  vec3_t    velocity;
  int     weaponTime;
  int     gravity;
  int     speed;
  int     delta_angles[3];  // add to command angles to get view direction
                  // changed by spawns, rotating objects, and teleporters

  int     groundEntityNum;// ENTITYNUM_NONE = in air

  int     legsTimer;    // don't change low priority animations until this runs out
  int     legsAnim;   // mask off ANIM_TOGGLEBIT

  int     torsoTimer;   // don't change low priority animations until this runs out
  int     torsoAnim;    // mask off ANIM_TOGGLEBIT

  int     movementDir;  // a number 0 to 7 that represents the relative angle
                // of movement to the view angle (axial and diagonals)
                // when at rest, the value will remain unchanged
                // used to twist the legs during strafing

  vec3_t    grapplePoint; // location of grapple to pull towards if PMF_GRAPPLE_PULL

  int     eFlags;     // copied to entityState_t->eFlags

  int     eventSequence;  // pmove generated events
  int     events[MAX_PS_EVENTS];
  int     eventParms[MAX_PS_EVENTS];

  int     externalEvent;  // events set on player from another source
  int     externalEventParm;
  int     externalEventTime;

  int     clientNum;    // ranges from 0 to MAX_CLIENTS-1
  int     weapon;     // copied to entityState_t->weapon
  int     weaponstate;

  vec3_t    viewangles;   // for fixed views
  int     viewheight;

  // damage feedback
  int     damageEvent;  // when it changes, latch the other parms
  int     damageYaw;
  int     damagePitch;
  int     damageCount;

  int     stats[MAX_STATS];
  int     persistant[MAX_PERSISTANT]; // stats that aren't cleared on death
  int     powerups[MAX_POWERUPS]; // level.time that the powerup runs out
  int     ammo[MAX_WEAPONS];

  int     generic1;
  int     loopSound;
  int     jumppad_ent;  // jumppad entity hit this frame

  // not communicated over the net at all
  int     ping;     // server to game info for scoreboard
  int     pmove_framecount;
  int     jumppad_frame;
  int     entityEventSequence;
} playerState_t;

typedef struct
{
  byte  b0;
  byte  b1;
  byte  b2;
  byte  b3;
  byte  b4;
  byte  b5;
  byte  b6;
  byte  b7;
} qint64;

#endif	// __Q_SHARED_H

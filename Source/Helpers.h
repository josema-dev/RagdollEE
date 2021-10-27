#ifndef __HELPERS_H__
#define __HELPERS_H__

#include "stdafx.h"
#include "@@headers.h"

enum ACTOR_FLAG
{
	ACTOR_FREEZE_POS_X = 1 << 0,
	ACTOR_FREEZE_POS_Y = 1 << 1,
	ACTOR_FREEZE_POS_Z = 1 << 2,
	ACTOR_FREEZE_ROT_X = 1 << 3,
	ACTOR_FREEZE_ROT_Y = 1 << 4,
	ACTOR_FREEZE_ROT_Z = 1 << 5,
	ACTOR_KINEMATIC = 1 << 6,
	ACTOR_GRAVITY = 1 << 7,
	ACTOR_RAY = 1 << 8,
	ACTOR_COLLISION = 1 << 9,
	ACTOR_TRIGGER = 1 << 10,
	ACTOR_SLEEP = 1 << 11,
	ACTOR_CCD = 1 << 12,
};

T1(TYPE) C TYPE& Unaligned(C TYPE& src) { return src; }
T1(TYPE)   void  Unaligned(TYPE& dest, C TYPE& src) { dest = src; }
T1(TYPE)   void _Unaligned(Byte& dest, C TYPE& src) { dest = src; }
T1(TYPE)   void _Unaligned(UShort& dest, C TYPE& src) { dest = src; }
T1(TYPE)   void _Unaligned(Int& dest, C TYPE& src) { dest = src; }
T1(TYPE)   void _Unaligned(UInt& dest, C TYPE& src) { dest = src; }

enum ACTOR_GROUPS // actor groups (for example in the game you can specify following groups: door, item, character, ..)
{
	GROUP_BACKGROUND, // background
	GROUP_OBJ, // objects
};

#endif //__HELPERS_H__

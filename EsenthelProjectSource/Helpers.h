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

#if (defined _M_IX86 || defined __i386__) || (defined _M_X64 || defined __x86_64__) || (ARM && X64) || WEB // x86 32/64 and ARM 64 can do unaligned reads. When using WebAssembly (WASM) for WEB platform, unaligned access is supported, however when executed on platforms without native unaligned access support (Arm32) it will be extremely slow, however since Arm32 is in extinction then it's better to enable unaligned access to get better performance on majority of platforms that support it.
   T1(TYPE) C TYPE& Unaligned(              C TYPE &src) {return src;}
   T1(TYPE)   void  Unaligned(TYPE   &dest, C TYPE &src) {  dest=src;}
   T1(TYPE)   void _Unaligned(Byte   &dest, C TYPE &src) {  dest=src;}
   T1(TYPE)   void _Unaligned(UShort &dest, C TYPE &src) {  dest=src;}
   T1(TYPE)   void _Unaligned(Int    &dest, C TYPE &src) {  dest=src;}
   T1(TYPE)   void _Unaligned(UInt   &dest, C TYPE &src) {  dest=src;}
#else
   T1(TYPE)   TYPE  Unaligned(              C TYPE &src) {if(SIZE(TYPE)==1)return src;else{TYPE temp; CopyFast(Ptr(&temp), CPtr(&src), SIZE(TYPE)); return temp;}} // !! these functions must casted to 'Ptr', because without it, compiler may try to inline the 'memcpy' when it detects that both params are of the same type and in that case it will assume that they are memory aligned and crash will occur !!
   T1(TYPE)   void  Unaligned(TYPE   &dest, C TYPE &src) {if(SIZE(TYPE)==1)  dest=src;else{           CopyFast(Ptr(&dest), CPtr(&src), SIZE(TYPE));             }} // !! these functions must casted to 'Ptr', because without it, compiler may try to inline the 'memcpy' when it detects that both params are of the same type and in that case it will assume that they are memory aligned and crash will occur !!
   T1(TYPE)   void _Unaligned(Byte   &dest, C TYPE &src) {                   dest=Unaligned(src) ;                                                               }
   T1(TYPE)   void _Unaligned(UShort &dest, C TYPE &src) {Unaligned(dest, (UShort)Unaligned(src));                                                               }
   T1(TYPE)   void _Unaligned(Int    &dest, C TYPE &src) {Unaligned(dest, (Int   )Unaligned(src));                                                               }
   T1(TYPE)   void _Unaligned(UInt   &dest, C TYPE &src) {Unaligned(dest, (UInt  )Unaligned(src));                                                               }
#endif

enum ACTOR_GROUPS // actor groups (for example in the game you can specify following groups: door, item, character, ..)
{
	GROUP_BACKGROUND, // background
	GROUP_OBJ, // objects
};

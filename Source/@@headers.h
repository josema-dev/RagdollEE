/******************************************************************************/
#pragma once

#define bool Bool // boolean value (8-bit)

#define char8 Char8 //  8-bit character
#define char  Char  // 16-bit character

#define sbyte  I8  //  8-bit   signed integer
#define  byte  U8  //  8-bit unsigned integer
#define  short I16 // 16-bit   signed integer
#define ushort U16 // 16-bit unsigned integer
#define  int   I32 // 32-bit   signed integer
#define uint   U32 // 32-bit unsigned integer
#define  long  I64 // 64-bit   signed integer
#define ulong  U64 // 64-bit unsigned integer

#define flt Flt // 32-bit floating point
#define dbl Dbl // 64-bit floating point

#define  ptr  Ptr // universal pointer
#define cptr CPtr // universal pointer to const data

#define cchar8 CChar8 // const Char8
#define cchar  CChar  // const Char16

#define  intptr  IntPtr //   signed integer capable of storing full memory address
#define uintptr UIntPtr // unsigned integer capable of storing full memory address

#define class struct // Esenthel Script "class" is a C++ "struct"
/******************************************************************************/
// DEFINES
#define STEAM   0
#define OPEN_VR 0
/******************************************************************************/
/******************************************************************************/
// CLASSES
/******************************************************************************/
class ClientInfo;
/******************************************************************************/
// ENUMS
/******************************************************************************/
enum CLIENT_SERVER_COMMANDS
{
   CS_WELCOME     , // welcome message
   CS_LOGIN       , // login account
   CS_POSITION    , // exchange player position
   CS_DEL_NEIGHBOR, // remove information about another player
   CS_ADD_NEIGHBOR, // add    information about another player that we've come into contact
};
/******************************************************************************/
enum DRAW_GROUP_CHR
{
   DG_CHR_DEFAULT,
   DG_CHR_HEAD   ,
};
/******************************************************************************/
enum ITEM_TYPE : byte
{
   ITEM_MISC  ,
   ITEM_WEAPON,
   ITEM_ARMOR ,
};
/******************************************************************************/
enum OBJ_TYPE // Object Class Types
{
   OBJ_CHR         ,
   OBJ_DESTRUCTIBLE,
   OBJ_ITEM        ,
   OBJ_LIGHT_POINT ,
   OBJ_PARTICLES   ,
};
/******************************************************************************/
// CONSTANTS
/******************************************************************************/
const int  EE_APP_BUILD        =1;
/******************************************************************************/
/******************************************************************************/
// CLASSES
/******************************************************************************/
#include "@ClientInfo.h"
/******************************************************************************/
// CPP
/******************************************************************************/
#include "Auto.h"
#include "Shared/Commands.h"
/******************************************************************************/
// INLINE, TEMPLATES
/******************************************************************************/
/******************************************************************************/

#ifndef __ZCK_RECORD_H__
#define __ZCK_RECORD_H__

#pragma once

#include "../_CommonDefine/FlyffCommonDefine.h"
#include <cstring>

#ifdef __ZCK_RECORD


enum ZCK_RECORD_TYPE
{
    ZCK_RECORD_STR = 0,
    ZCK_RECORD_DEX,
    ZCK_RECORD_STA,
    ZCK_RECORD_INT,
    ZCK_RECORD_DPS,
    ZCK_RECORD_DUNGEON,
    ZCK_RECORD_COUNT
};

enum
{
    ZCK_RECORD_TOP_COUNT = 10
};

struct ZCK_RECORD_ENTRY
{
    u_long  dwPlayerId;
    char    szName[MAX_NAME];
    int     nLevel;
    int     nJob;
    __int64 nValue;
    DWORD   dwDungeonWorldId;
    DWORD   dwDungeonLevel;

    ZCK_RECORD_ENTRY()
        : dwPlayerId( 0 )
        , nLevel( 0 )
        , nJob( 0 )
        , nValue( 0 )
        , dwDungeonWorldId( 0 )
        , dwDungeonLevel( 0 )
    {
        memset( szName, 0, sizeof( szName ) );
    }
};

#endif // __ZCK_RECORD

#endif // __ZCK_RECORD_H__
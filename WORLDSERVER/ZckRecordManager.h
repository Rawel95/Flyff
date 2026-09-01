#ifndef __ZCK_RECORD_MANAGER_H__
#define __ZCK_RECORD_MANAGER_H__

#pragma once

#ifdef __ZCK_RECORD

#include "../_Common/ZckRecord.h"
#include <map>
#include <string>
#include <vector>

class FLWSUser;

class CZckRecordManager
{
private:
    struct PLAYER_RECORD
    {
        u_long  dwPlayerId;
        char    szName[MAX_NAME];
        int     nLevel;
        int     nJob;
        __int64 nStats[4];
        __int64 nBestDps;
        DWORD   dwBestDungeonTime;
        DWORD   dwDungeonWorldId;
        DWORD   dwDungeonLevel;

        PLAYER_RECORD();
    };

    struct DPS_SESSION
    {
        OBJID   objTarget;
        __int64 nTotalDamage;
        DWORD   dwStartTick;
        DWORD   dwLastHitTick;

        DPS_SESSION();
    };

    typedef std::map<u_long, PLAYER_RECORD> RECORD_MAP;
    typedef std::map<u_long, DPS_SESSION> DPS_MAP;

    RECORD_MAP  m_records;
    DPS_MAP     m_dpsSessions;
    __int64     m_nWeekStart;
    DWORD       m_dwVersion;
    DWORD       m_dwLastSaveTick;
    bool        m_bLoaded;
    bool        m_bDirty;

private:
    CZckRecordManager();
    ~CZckRecordManager();

    CZckRecordManager( const CZckRecordManager& );
    CZckRecordManager& operator=( const CZckRecordManager& );

    void EnsureLoaded();
    __int64 GetCurrentWeekStart() const;
    std::string GetDataFilePath() const;
    bool CheckWeeklyReset();
    void MarkDirty();
    bool UpdateIdentity( PLAYER_RECORD& record, FLWSUser* pUser );
    PLAYER_RECORD& GetOrCreateRecord( FLWSUser* pUser );
    void BuildTop10( ZCK_RECORD_TYPE type, std::vector<ZCK_RECORD_ENTRY>& out ) const;
    void BuildSnapshot( CAr& ar ) const;

public:
    static CZckRecordManager* GetInstance();

    bool Load();
    bool Save();
    bool Reload();
    void Process();

    void UpdateStats( FLWSUser* pUser );
    void AddDamage( FLWSUser* pAttacker, OBJID objTarget, __int64 nDamage );
    void UpdateDungeonRecord( FLWSUser* pUser, DWORD dwCompleteTime, DWORD dwWorldId, DWORD dwDungeonLevel );

    void SendSnapshot( FLWSUser* pUser );
    void BroadcastSnapshot();
};

#endif // __ZCK_RECORD

#endif // __ZCK_RECORD_MANAGER_H__
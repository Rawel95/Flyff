#pragma once

#ifdef __ZCK_INVASIONS

#include <map>
#include <vector>

class CMover;
#ifdef __WORLDSERVER
class FLWSUser;
#endif // __WORLDSERVER

enum class ZckInvasionPacketAction : int
{
	Add = 0,
	Notify,
	RefreshAll,
	End
};

struct ZckInvasionOpenTime
{
	int nDay;
	int nHour;
	int nMinute;

	ZckInvasionOpenTime()
		: nDay( 0 )
		, nHour( 0 )
		, nMinute( 0 )
	{
	}
};

struct ZckInvasionReward
{
	int nMinRank;
	int nMaxRank;
	DWORD dwItemID;
	int nItemCount;
	bool bBind;

	ZckInvasionReward()
		: nMinRank( 0 )
		, nMaxRank( 0 )
		, dwItemID( NULL_ID )
		, nItemCount( 0 )
		, bBind( false )
	{
	}
};

struct ZckInvasionMonster
{
	DWORD dwMonsterID;
	int nMonsterCount;
	bool bAggressive;

	ZckInvasionMonster()
		: dwMonsterID( NULL_ID )
		, nMonsterCount( 0 )
		, bAggressive( false )
	{
	}
};

struct ZckInvasionWave
{
	int nPercentBeforeNext;
	std::vector<ZckInvasionMonster> vMonsters;

	ZckInvasionWave()
		: nPercentBeforeNext( 100 )
	{
	}
};

struct ZckInvasionDefinition
{
	DWORD dwWorldID;
	int nDifficulty;
	D3DXVECTOR3 vStartingPos;
	float fSpawnRadius;
	int nMaxTimeMinutes;
	std::vector<ZckInvasionWave> vWaves;
	std::vector<ZckInvasionReward> vRewards;

	ZckInvasionDefinition()
		: dwWorldID( NULL_ID )
		, nDifficulty( 0 )
		, vStartingPos( 0.0f, 0.0f, 0.0f )
		, fSpawnRadius( 0.0f )
		, nMaxTimeMinutes( 0 )
	{
	}
};

struct ZckCurrentInvasion
{
	DWORD dwInvasionID;
	int nMonstersAlive;
	int nCurrentWave;
	int nCurrentPercent;
	__int64 nStartingTimeMs;
	std::map<u_long, __int64> mPlayerDamage;
#ifdef __WORLDSERVER
	std::vector<OBJID> vCurrentWaveMonsters;
	std::vector<OBJID> vSpawnedMonsters;
#endif // __WORLDSERVER

	ZckCurrentInvasion()
		: dwInvasionID( NULL_ID )
		, nMonstersAlive( 0 )
		, nCurrentWave( -1 )
		, nCurrentPercent( 0 )
		, nStartingTimeMs( 0 )
	{
	}
};

class CZckInvasions
{
private:
	int m_nMaxInvasions;
	std::vector<ZckInvasionOpenTime> m_vOpenTimes;
	std::vector<ZckInvasionDefinition> m_vDefinitions;
	std::vector<ZckCurrentInvasion> m_vCurrentInvasions;

#ifdef __WORLDSERVER
	DWORD m_dwLastNotifyTick;
	ULONGLONG m_ullLastScheduleKey;

	ULONGLONG BuildScheduleKey( const CTime& ctime ) const;
	ZckCurrentInvasion* FindCurrentInvasion( DWORD dwInvasionID );
	const ZckCurrentInvasion* FindCurrentInvasion( DWORD dwInvasionID ) const;
	bool IsDefinitionActive( DWORD dwInvasionID ) const;
	bool IsFinished( ZckCurrentInvasion& current ) const;
	bool IsWaveComplete( ZckCurrentInvasion& current );
	void SetNextWave( ZckCurrentInvasion& current );
	void ProcessCurrentInvasions();
	void EndInvasion( ZckCurrentInvasion& current );
	void CleanupSpawnedMonsters( ZckCurrentInvasion& current );
	void SendUserRewards( ZckCurrentInvasion& current );
	void NotifyNearbyUsers( const ZckCurrentInvasion& current );
	void SendSnapshot( FLWSUser* pUser, ZckInvasionPacketAction action, const ZckCurrentInvasion* pCurrent );
	void SendSnapshotToAll( ZckInvasionPacketAction action, const ZckCurrentInvasion* pCurrent );
	void SerializeCurrent( CAr& ar, const ZckCurrentInvasion& current ) const;
#endif // __WORLDSERVER

#ifdef __CLIENT
	bool m_bInitialRefreshRequested;
#endif // __CLIENT

public:
	CZckInvasions();
	~CZckInvasions();

	static CZckInvasions* GetInstance();

	BOOL LoadScript();

	const std::vector<ZckInvasionDefinition>& GetDefinitions() const { return m_vDefinitions; }
	const std::vector<ZckCurrentInvasion>& GetCurrentInvasions() const { return m_vCurrentInvasions; }
	const ZckInvasionDefinition* GetDefinition( DWORD dwInvasionID ) const;

#ifdef __WORLDSERVER
	void Process();
	BOOL CreateInvasion( DWORD dwInvasionID, CString* pFailureReason = NULL );
	void AddUserDamage( CMover* pAttacker, CMover* pDefender, int nDamage );
	void SendRefreshAll( FLWSUser* pUser );
#endif // __WORLDSERVER

#ifdef __CLIENT
	void ProcessClientSync();
	bool GetLocalInvasionID( DWORD& dwInvasionID ) const;
	void ClientUpsertInvasion( const ZckCurrentInvasion& current );
	void ClientReplaceAll( const std::vector<ZckCurrentInvasion>& invasions );
	void ClientDeleteInvasion( DWORD dwInvasionID );
#endif // __CLIENT
};

#endif // __ZCK_INVASIONS
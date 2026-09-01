#pragma once

#ifdef __ZCK_WORLD_BOSS

class CMover;
class CModelObject;

#ifdef __WORLDSERVER
class FLWSUser;

struct WorldBossReward
{
	DWORD dwItemID;
	int nItemCount;
	bool bBinded;

	WorldBossReward()
		: dwItemID( NULL_ID )
		, nItemCount( 0 )
		, bBinded( false )
	{
	}

	bool IsValid() const
	{
		return dwItemID != NULL_ID && nItemCount > 0;
	}
};

struct WorldBossDetails
{
	int nHour;
	int nMinute;
	std::vector<DWORD> vecPossibleMonsters;
	DWORD dwWorldID;
	D3DXVECTOR3 vPos;
	DWORD dwWorldIDTeleport;
	D3DXVECTOR3 vPosTeleport;
	std::vector<WorldBossReward> vecRewardTop;
	WorldBossReward rewardInvolved;
	__int64 n64MinDamageReq;

	WorldBossDetails()
		: nHour( -1 )
		, nMinute( -1 )
		, dwWorldID( NULL_ID )
		, vPos( 0.0f, 0.0f, 0.0f )
		, dwWorldIDTeleport( NULL_ID )
		, vPosTeleport( 0.0f, 0.0f, 0.0f )
		, n64MinDamageReq( 0 )
	{
	}
};

class CWorldBossManager
{
private:
	std::vector<WorldBossDetails> m_arrayWorldBosses[7];
	DWORD m_dwWorldBossId;
	WorldBossDetails m_currentWbd;
	DWORD m_dwLastWorldBossInfoTick;
	time_t m_tExpirationTime;
	ULONGLONG m_ullLastScheduleKey;
	bool m_bEnabled;

private:
	void ResetActiveWorldBoss();
	ULONGLONG BuildScheduleKey( const CTime& ctime ) const;
	bool BuildWorldBossInfo( DWORD& dwMonsterID, int& nHPPercent, DWORD& dwRemainingSeconds );
	void SendWorldBossSnapshot( FLWSUser* pUser, DWORD dwMonsterID, int nHPPercent, DWORD dwRemainingSeconds );

public:
	CWorldBossManager();
	~CWorldBossManager();

	static CWorldBossManager* GetInstance();

	void SetEnabled( bool bEnabled ) { m_bEnabled = bEnabled; }
	bool IsEnabled() const { return m_bEnabled; }

	BOOL LoadScript();
	void Process();
	void ProcessExpire();
	bool SpawnWorldBoss( const WorldBossDetails& refWbd, CString* pFailureReason = NULL );
	BOOL SpawnWorldBossByCommand( int nDayOfWeek, int nIndex, CString* pFailureReason = NULL );
	void FinishWorldBoss( CMover* pMover );

	void SendWorldBossInfo( FLWSUser* pUser );
	void SendWorldBossInfoToAllUsers();
	void TeleportUserToWorldBoss( FLWSUser* pUser );
};
#endif // __WORLDSERVER

#ifdef __CLIENT
class CWorldBossManager
{
private:
	int m_nHPPercent;
	CModelObject* m_pModelWorldBoss;
	float m_fModelRadius;
	time_t m_tExpirationTime;

public:
	CWorldBossManager();
	~CWorldBossManager();

	static CWorldBossManager* GetInstance();

	void Clear();
	void Process();

	int GetHPPercent() const { return m_nHPPercent; }
	CModelObject* GetModel() const { return m_pModelWorldBoss; }
	float GetModelRadius() const { return m_fModelRadius; }
	time_t GetExpirationTime() const { return m_tExpirationTime; }

	void UpdateWorldBossInfo( DWORD dwMonsterID, int nHPPercent, DWORD dwRemainingSeconds );
};
#endif // __CLIENT

#endif // __ZCK_WORLD_BOSS
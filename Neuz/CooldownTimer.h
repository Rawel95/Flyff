#pragma once

#ifdef __COOLDOWN_TIMER

typedef struct tagDUNGEON_COOLTIME
{
	DWORD dwWorldId;
	DWORD dwCoolTime;
} DUNGEON_COOLTIME;

class CCoolTimeMgr
{
private:
	std::vector<DUNGEON_COOLTIME*> m_vecDungeonCoolTime;
	DWORD m_dwNextGuildWar;

	CCoolTimeMgr();
	~CCoolTimeMgr();
public:
	static CCoolTimeMgr* Instance();

	void Clear();

	HANDLE FirstDCD(LPDWORD pWorldId, LPDWORD pCoolTime);
	bool NextDCD(HANDLE hFindDungeon, LPDWORD pWorldId, LPDWORD pCoolTime);
	void FindClose(HANDLE hFindDungeon);

	DWORD GetNextGuildWarTime();
	void Serialize(CAr& ar);

	void UpdateDungeonCoolTime(DWORD dwWorldId, DWORD dwCoolTime);
	void UpdateGuildWarTime(DWORD dwNextGuildWar);
};

#endif // __COOLDOWN_TIMER
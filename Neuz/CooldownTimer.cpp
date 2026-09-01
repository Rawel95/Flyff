#include "stdafx.h"

#ifdef __COOLDOWN_TIMER
#include "../../LibrarySource/network/FLAr.h"
#include "CooldownTimer.h"

CCoolTimeMgr::CCoolTimeMgr()
{
	m_vecDungeonCoolTime.clear();
	m_dwNextGuildWar = 0;
	//	m_dwNextPlayerSiege = 0;
}

CCoolTimeMgr::~CCoolTimeMgr()
{
	Clear();
}

CCoolTimeMgr* CCoolTimeMgr::Instance()
{
	static CCoolTimeMgr ins;
	return &ins;
}

void CCoolTimeMgr::Clear()
{
	while (!m_vecDungeonCoolTime.empty())
	{
		SAFE_DELETE(m_vecDungeonCoolTime.back());
		m_vecDungeonCoolTime.pop_back();
	}
}

HANDLE CCoolTimeMgr::FirstDCD(LPDWORD pWorldId, LPDWORD pCoolTime)
{
	if (m_vecDungeonCoolTime.empty())
		return INVALID_HANDLE_VALUE;

	std::vector<DUNGEON_COOLTIME*>::iterator* pIterator = new std::vector<DUNGEON_COOLTIME*>::iterator();
	*pIterator = m_vecDungeonCoolTime.begin();
	*pWorldId = (**pIterator)->dwWorldId;
	*pCoolTime = (**pIterator)->dwCoolTime;
	return (HANDLE)pIterator;
}

bool CCoolTimeMgr::NextDCD(HANDLE hFindDungeon, LPDWORD pWorldId, LPDWORD pCoolTime)
{
	std::vector<DUNGEON_COOLTIME*>::iterator* pIterator = (std::vector<DUNGEON_COOLTIME*>::iterator*)hFindDungeon;
	if (++(*pIterator) == m_vecDungeonCoolTime.end())
		return false;
	*pWorldId = (**pIterator)->dwWorldId;
	*pCoolTime = (**pIterator)->dwCoolTime;
	return true;
}

void CCoolTimeMgr::FindClose(HANDLE hFindDungeon)
{
	std::vector<DUNGEON_COOLTIME*>::iterator* pIterator = (std::vector<DUNGEON_COOLTIME*>::iterator*)hFindDungeon;
	SAFE_DELETE(pIterator);
}

DWORD CCoolTimeMgr::GetNextGuildWarTime()
{
	return m_dwNextGuildWar;
}

#ifdef __FREE_FOR_ALL
DWORD CCoolTimeMgr::GetNextPlayerSiegeTime()
{
	return m_dwNextPlayerSiege;
}
#endif // __FREE_FOR_ALL

void CCoolTimeMgr::Serialize(CAr& ar)
{
	Clear();
	char chSize;
	ar >> chSize;
	DUNGEON_COOLTIME* pCT = NULL;
	for (char c = 0; c < chSize; ++c)
	{
		pCT = new DUNGEON_COOLTIME();
		ar >> pCT->dwWorldId >> pCT->dwCoolTime;
		pCT->dwCoolTime += g_tmCurrent;
		m_vecDungeonCoolTime.push_back(pCT);
	}
	ar >> m_dwNextGuildWar;
	m_dwNextGuildWar += g_tmCurrent;
#ifdef __FREE_FOR_ALL
	ar >> m_dwNextPlayerSiege;
	m_dwNextPlayerSiege += g_tmCurrent;
#endif // __FREE_FOR_ALL

}

void CCoolTimeMgr::UpdateDungeonCoolTime(DWORD dwWorldId, DWORD dwCoolTime)
{
	for (unsigned i = 0; i < m_vecDungeonCoolTime.size(); ++i)
	{
		if (m_vecDungeonCoolTime[i]->dwWorldId == dwWorldId)
		{
			m_vecDungeonCoolTime[i]->dwCoolTime = dwCoolTime;
			return;
		}
	}
	DUNGEON_COOLTIME* pCT = new DUNGEON_COOLTIME();
	pCT->dwWorldId = dwWorldId;
	pCT->dwCoolTime = dwCoolTime;
	m_vecDungeonCoolTime.push_back(pCT);
}

void CCoolTimeMgr::UpdateGuildWarTime(DWORD dwNextGuildWar)
{
	m_dwNextGuildWar = dwNextGuildWar;
}

#ifdef __FREE_FOR_ALL
void CCoolTimeMgr::UpdatePlayerSiegeTime(DWORD dwNextPlayerSiege)
{
	m_dwNextPlayerSiege = dwNextPlayerSiege;
}
#endif // __FREE_FOR_ALL

#endif // __COOLDOWN_TIMER
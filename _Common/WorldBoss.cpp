#include "stdafx.h"
#include "WorldBoss.h"
#include <algorithm>

#ifdef __ZCK_WORLD_BOSS

#ifdef __WORLDSERVER
#include "CreateObj.h"
#include "worldmng.h"
#include "playerdata.h"
#include "FLItemElem.h"
#include "post.h"
#include "../WORLDSERVER/User.h"
#include "../WORLDSERVER/DPDatabaseClient.h"
#include "../WORLDSERVER/DPCoreClient.h"

extern CWorldMng g_WorldMng;
extern CDPCoreClient g_DPCoreClient;
extern CDPDatabaseClient g_dpDBClient;

namespace
{
	const time_t WORLD_BOSS_LIFETIME_SECONDS = 3600;
	const DWORD WORLD_BOSS_INFO_INTERVAL = SEC( 10 );

	bool PrepareWorldBossReward( const WorldBossReward& reward, FLItemElem& itemElem )
	{
		if( reward.IsValid() == false )
			return false;

		const PT_ITEM_SPEC pItemSpec = g_xSpecManager->GetSpecItem( reward.dwItemID );
		if( pItemSpec == NULL )
			return false;

		itemElem.m_dwItemId = reward.dwItemID;
		itemElem.m_nItemNum = reward.nItemCount;
		itemElem.m_bCharged = pItemSpec->bCharged;
		if( reward.bBinded )
			itemElem.SetFlag( FLItemElem::binds );
		itemElem.SetSerialNumber();
		return true;
	}

	void SendWorldBossRewardMail( u_long idPlayer, FLItemElem& itemElem, const char* pszText )
	{
		char szTitle[MAX_MAILTITLE] = { 0, };
		char szText[MAX_MAILTEXT] = { 0, };
		FLSPrintf( szTitle, _countof( szTitle ), "Boss Mondial" );
		FLSPrintf( szText, _countof( szText ), "%s", pszText != NULL ? pszText : "" );
		g_dpDBClient.SendQueryPostMail( idPlayer, 0, itemElem, 0, szTitle, szText );
	}
}

CWorldBossManager::CWorldBossManager()
	: m_dwWorldBossId( NULL_ID )
	, m_dwLastWorldBossInfoTick( 0 )
	, m_tExpirationTime( 0 )
	, m_ullLastScheduleKey( 0 )
	, m_bEnabled( false )
{
}

CWorldBossManager::~CWorldBossManager()
{
}

CWorldBossManager* CWorldBossManager::GetInstance()
{
	static CWorldBossManager sWorldBossManager;
	return &sWorldBossManager;
}

void CWorldBossManager::ResetActiveWorldBoss()
{
	m_dwWorldBossId = NULL_ID;
	m_currentWbd = WorldBossDetails();
	m_tExpirationTime = 0;
	m_dwLastWorldBossInfoTick = 0;
}

ULONGLONG CWorldBossManager::BuildScheduleKey( const CTime& ctime ) const
{
	ULONGLONG ullKey = static_cast<ULONGLONG>( ctime.GetYear() );
	ullKey = ullKey * 13ULL + static_cast<ULONGLONG>( ctime.GetMonth() );
	ullKey = ullKey * 32ULL + static_cast<ULONGLONG>( ctime.GetDay() );
	ullKey = ullKey * 24ULL + static_cast<ULONGLONG>( ctime.GetHour() );
	ullKey = ullKey * 60ULL + static_cast<ULONGLONG>( ctime.GetMinute() );
	return ullKey;
}

BOOL CWorldBossManager::LoadScript()
{
	CScript s;
	if( !s.Load( "ZckWorldBoss.inc" ) )
	{
		TCHAR szCurrentDirectory[MAX_PATH] = { 0, };
		GetCurrentDirectory( _countof( szCurrentDirectory ), szCurrentDirectory );
		FLERROR_LOG(
			PROGRAM_NAME,
			_T( "CWorldBossManager::LoadScript() - Impossible de trouver ou charger ZckWorldBoss.inc. ResourceFolder courant: %s" ),
			szCurrentDirectory
		);
		return FALSE;
	}

	for( int i = 0; i < 7; ++i )
		m_arrayWorldBosses[i].clear();

	int nLoadedEntries = 0;

	s.GetToken();
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "AddWorldBossDay" ) )
		{
			const int nDayOfWeek = s.GetNumber();
			const bool bValidDay = nDayOfWeek >= 1 && nDayOfWeek <= 7;
			if( !bValidDay )
				FLERROR_LOG( PROGRAM_NAME, _T( "CWorldBossManager::LoadScript() invalid nDayOfWeek %d" ), nDayOfWeek );

			s.GetToken();
			s.GetToken();
			while( s.tok != FINISHED && *s.token != '}' )
			{
				if( s.Token == _T( "AddWorldBossTime" ) )
				{
					WorldBossDetails wbd;
					wbd.nHour = s.GetNumber();
					wbd.nMinute = s.GetNumber();

					s.GetToken();
					s.GetToken();
					while( s.tok != FINISHED && *s.token != '}' )
					{
						if( s.Token == _T( "Monster_List" ) )
						{
							s.GetToken();
							DWORD dwMonsterID = static_cast<DWORD>( s.GetNumber() );
							while( s.tok != FINISHED && *s.token != '}' )
							{
								MoverProp* pMoverProp = prj.GetMoverProp( dwMonsterID );
								if( pMoverProp == NULL || pMoverProp->dwClass != RANK_SUPER )
									FLERROR_LOG( PROGRAM_NAME, _T( "CWorldBossManager::LoadScript() invalid WorldBoss mover %u" ), dwMonsterID );
								else
									wbd.vecPossibleMonsters.push_back( dwMonsterID );
								dwMonsterID = static_cast<DWORD>( s.GetNumber() );
							}
						}
						else if( s.Token == _T( "SpawnLocation" ) )
						{
							s.GetToken();
							wbd.dwWorldID = static_cast<DWORD>( s.GetNumber() );
							wbd.vPos.x = static_cast<float>( s.GetNumber() );
							wbd.vPos.y = static_cast<float>( s.GetNumber() );
							wbd.vPos.z = static_cast<float>( s.GetNumber() );
						}
						else if( s.Token == _T( "TeleportLocation" ) )
						{
							s.GetToken();
							wbd.dwWorldIDTeleport = static_cast<DWORD>( s.GetNumber() );
							wbd.vPosTeleport.x = static_cast<float>( s.GetNumber() );
							wbd.vPosTeleport.y = static_cast<float>( s.GetNumber() );
							wbd.vPosTeleport.z = static_cast<float>( s.GetNumber() );
						}
						else if( s.Token == _T( "Rewards_Top" ) )
						{
							s.GetToken();
							WorldBossReward reward;
							reward.dwItemID = static_cast<DWORD>( s.GetNumber() );
							while( s.tok != FINISHED && *s.token != '}' )
							{
								reward.nItemCount = s.GetNumber();
								reward.bBinded = s.GetNumber() != 0;
								if( reward.IsValid() && g_xSpecManager->GetSpecItem( reward.dwItemID ) != NULL )
									wbd.vecRewardTop.push_back( reward );
								else
									FLERROR_LOG( PROGRAM_NAME, _T( "CWorldBossManager::LoadScript() invalid top reward item %u count %d" ), reward.dwItemID, reward.nItemCount );
								reward = WorldBossReward();
								reward.dwItemID = static_cast<DWORD>( s.GetNumber() );
							}
						}
						else if( s.Token == _T( "Reward_Involved" ) )
						{
							s.GetToken();
							wbd.rewardInvolved.dwItemID = static_cast<DWORD>( s.GetNumber() );
							wbd.rewardInvolved.nItemCount = s.GetNumber();
							wbd.rewardInvolved.bBinded = s.GetNumber() != 0;
						}
						else if( s.Token == _T( "MinDamageReq" ) )
						{
							s.GetToken();
							wbd.n64MinDamageReq = s.GetInt64();
							if( wbd.n64MinDamageReq < 0 )
								wbd.n64MinDamageReq = 0;
						}

						s.GetToken();
					}

					if( wbd.nHour < 0 || wbd.nHour > 23 || wbd.nMinute < 0 || wbd.nMinute > 59 )
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "CWorldBossManager::LoadScript() invalid time %d:%d" ), wbd.nHour, wbd.nMinute );
					}
					else if( wbd.dwWorldID == NULL_ID || wbd.vecPossibleMonsters.empty() )
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "CWorldBossManager::LoadScript() incomplete WorldBoss entry day %d time %d:%d" ), nDayOfWeek, wbd.nHour, wbd.nMinute );
					}
					else if( bValidDay )
					{
						if( wbd.rewardInvolved.IsValid() && g_xSpecManager->GetSpecItem( wbd.rewardInvolved.dwItemID ) == NULL )
						{
							FLERROR_LOG( PROGRAM_NAME, _T( "CWorldBossManager::LoadScript() invalid involved reward item %u" ), wbd.rewardInvolved.dwItemID );
							wbd.rewardInvolved = WorldBossReward();
						}
						m_arrayWorldBosses[nDayOfWeek - 1].push_back( wbd );
						++nLoadedEntries;
					}
				}

				s.GetToken();
			}
		}

		s.GetToken();
	}

	TCHAR szCurrentDirectory[MAX_PATH] = { 0, };
	GetCurrentDirectory( _countof( szCurrentDirectory ), szCurrentDirectory );

	if( nLoadedEntries <= 0 )
	{
		FLERROR_LOG(
			PROGRAM_NAME,
			_T( "CWorldBossManager::LoadScript() - ZckWorldBoss.inc charge mais aucune entree valide. ResourceFolder courant: %s" ),
			szCurrentDirectory
		);
		return FALSE;
	}

	FLINFO_LOG(
		PROGRAM_NAME,
		_T( "CWorldBossManager::LoadScript() - ZckWorldBoss.inc charge avec succes: %d entree(s). ResourceFolder courant: %s" ),
		nLoadedEntries,
		szCurrentDirectory
	);
	return TRUE;
}

void CWorldBossManager::Process()
{
	if( m_bEnabled == false )
		return;

	ProcessExpire();

	if( m_dwWorldBossId != NULL_ID )
	{
		if( m_dwLastWorldBossInfoTick == 0 || static_cast<DWORD>( g_tmCurrent - m_dwLastWorldBossInfoTick ) >= WORLD_BOSS_INFO_INTERVAL )
			SendWorldBossInfoToAllUsers();
		return;
	}

	const CTime ctime = CTime::GetCurrentTime();
	const int nDayOfWeek = ctime.GetDayOfWeek();
	if( nDayOfWeek < 1 || nDayOfWeek > 7 )
		return;

	const ULONGLONG ullScheduleKey = BuildScheduleKey( ctime );
	const std::vector<WorldBossDetails>& vecWorldBosses = m_arrayWorldBosses[nDayOfWeek - 1];
	for( std::vector<WorldBossDetails>::const_iterator it = vecWorldBosses.begin(); it != vecWorldBosses.end(); ++it )
	{
		if( it->nHour == ctime.GetHour() && it->nMinute == ctime.GetMinute() )
		{
			if( m_ullLastScheduleKey == ullScheduleKey )
				break;

			if( SpawnWorldBoss( *it ) )
				m_ullLastScheduleKey = ullScheduleKey;
			break;
		}
	}
}

void CWorldBossManager::ProcessExpire()
{
	if( m_dwWorldBossId == NULL_ID )
		return;

	CMover* pMoverWB = prj.GetMover( m_dwWorldBossId );
	if( IsValidObj( pMoverWB ) == FALSE || pMoverWB->m_bWorldBoss == false )
	{
		ResetActiveWorldBoss();
		SendWorldBossInfoToAllUsers();
		return;
	}

	if( g_tCurrent < m_tExpirationTime )
		return;

	pMoverWB->m_bWorldBoss = false;
	pMoverWB->m_mapAttackersDamage.clear();
	ResetActiveWorldBoss();
	pMoverWB->Delete();

	SendWorldBossInfoToAllUsers();
	g_DPCoreClient.SendSystem( CString( _T( "Le Boss Mondial n'a pas ete vaincu a temps." ) ) );
}

bool CWorldBossManager::SpawnWorldBoss( const WorldBossDetails& refWbd, CString* pFailureReason )
{
	if( pFailureReason != NULL )
		pFailureReason->Empty();

	if( m_dwWorldBossId != NULL_ID )
	{
		CMover* pExistingBoss = prj.GetMover( m_dwWorldBossId );
		if( IsValidObj( pExistingBoss ) && pExistingBoss->m_bWorldBoss )
		{
			if( pFailureReason != NULL )
				*pFailureReason = _T( "Un World Boss est deja actif sur ce WorldServer." );
			return false;
		}
		ResetActiveWorldBoss();
	}

	CWorld* pWorld = g_WorldMng.GetWorld( refWbd.dwWorldID );
	if( pWorld == NULL )
	{
		if( pFailureReason != NULL )
			pFailureReason->Format( _T( "Le monde configure pour le World Boss est introuvable. WorldID=%u." ), refWbd.dwWorldID );
		return false;
	}

	if( refWbd.vecPossibleMonsters.empty() )
	{
		if( pFailureReason != NULL )
			*pFailureReason = _T( "La Monster_List du World Boss est vide ou invalide." );
		return false;
	}

	const int nMonsterIndex = static_cast<int>( xRandom( 0, static_cast<DWORD>( refWbd.vecPossibleMonsters.size() ) ) );
	if( nMonsterIndex < 0 || static_cast<size_t>( nMonsterIndex ) >= refWbd.vecPossibleMonsters.size() )
	{
		if( pFailureReason != NULL )
			*pFailureReason = _T( "Index aleatoire World Boss invalide." );
		return false;
	}

	const DWORD dwMonsterID = refWbd.vecPossibleMonsters[nMonsterIndex];
	MoverProp* pMoverProp = prj.GetMoverProp( dwMonsterID );
	if( pMoverProp == NULL )
	{
		if( pFailureReason != NULL )
			pFailureReason->Format( _T( "Le monstre configure est introuvable dans PropMover. MonsterID=%u." ), dwMonsterID );
		return false;
	}

	if( pMoverProp->dwClass != RANK_SUPER )
	{
		if( pFailureReason != NULL )
			pFailureReason->Format( _T( "Le monstre configure n'est pas RANK_SUPER. MonsterID=%u, Rank=%u." ), dwMonsterID, pMoverProp->dwClass );
		return false;
	}

#ifdef __LAYER_1015
	CMover* pMoverWB = CreateMover( pWorld, pMoverProp->dwID, refWbd.vPos, nDefaultLayer );
#else
	CMover* pMoverWB = CreateMover( pWorld, pMoverProp->dwID, refWbd.vPos );
#endif
	if( IsValidObj( pMoverWB ) == FALSE )
	{
		if( pFailureReason != NULL )
			pFailureReason->Format(
				_T( "CreateMover a echoue pour le World Boss. MonsterID=%u WorldID=%u Pos=%.0f %.0f %.0f." ),
				dwMonsterID,
				refWbd.dwWorldID,
				refWbd.vPos.x,
				refWbd.vPos.y,
				refWbd.vPos.z
			);
		return false;
	}

	pMoverWB->m_bWorldBoss = true;
	pMoverWB->m_mapAttackersDamage.clear();

	m_dwWorldBossId = pMoverWB->GetId();
	m_currentWbd = refWbd;
	m_tExpirationTime = g_tCurrent + WORLD_BOSS_LIFETIME_SECONDS;
	m_dwLastWorldBossInfoTick = 0;

	SendWorldBossInfoToAllUsers();

	CString str;
	str.Format( _T( "[Boss Mondial] %s est apparu !" ), pMoverProp->szName );
	g_DPCoreClient.SendSystem( str );
	return true;
}

BOOL CWorldBossManager::SpawnWorldBossByCommand( int nDayOfWeek, int nIndex, CString* pFailureReason )
{
	if( pFailureReason != NULL )
		pFailureReason->Empty();

	if( m_bEnabled == false )
	{
		if( pFailureReason != NULL )
			*pFailureReason = _T( "Le token WORLDBOSS n'a pas ete lu par WorldServer::Script(). Verifiez le WorldServer.ini reel charge par ce processus." );
		return FALSE;
	}

	if( nDayOfWeek < 1 || nDayOfWeek > 7 || nIndex < 1 )
	{
		if( pFailureReason != NULL )
			*pFailureReason = _T( "Jour ou numero World Boss invalide." );
		return FALSE;
	}

	if( m_dwWorldBossId != NULL_ID )
	{
		CMover* pCurrentWorldBoss = prj.GetMover( m_dwWorldBossId );
		if( IsValidObj( pCurrentWorldBoss ) && pCurrentWorldBoss->m_bWorldBoss )
		{
			if( pFailureReason != NULL )
				*pFailureReason = _T( "Un World Boss est deja actif sur ce WorldServer." );
			return FALSE;
		}
		ResetActiveWorldBoss();
		SendWorldBossInfoToAllUsers();
	}

	if( static_cast<size_t>( nIndex ) > m_arrayWorldBosses[nDayOfWeek - 1].size() )
	{
		if( LoadScript() == FALSE )
		{
			if( pFailureReason != NULL )
				*pFailureReason = _T( "ZckWorldBoss.inc n'a pas pu etre charge par le WorldServer. Il doit exister physiquement dans le ResourceFolder serveur." );
			return FALSE;
		}
	}

	const std::vector<WorldBossDetails>& vecWorldBosses = m_arrayWorldBosses[nDayOfWeek - 1];
	if( static_cast<size_t>( nIndex ) > vecWorldBosses.size() )
	{
		if( pFailureReason != NULL )
			pFailureReason->Format(
				_T( "Aucune entree World Boss chargee pour le jour %d numero %d. Entrees disponibles pour ce jour: %u." ),
				nDayOfWeek,
				nIndex,
				static_cast<unsigned int>( vecWorldBosses.size() )
			);
		return FALSE;
	}

	if( SpawnWorldBoss( vecWorldBosses[nIndex - 1], pFailureReason ) == false )
		return FALSE;

	m_ullLastScheduleKey = BuildScheduleKey( CTime::GetCurrentTime() );
	return TRUE;
}

void CWorldBossManager::FinishWorldBoss( CMover* pMover )
{
	if( pMover == NULL )
		return;
	if( m_dwWorldBossId == NULL_ID || pMover->GetId() != m_dwWorldBossId || pMover->m_bWorldBoss == false )
		return;
	if( pMover->GetHitPoint() > 0 )
		return;

	const WorldBossDetails finishedWbd = m_currentWbd;
	std::vector<std::pair<u_long, __int64> > result( pMover->m_mapAttackersDamage.begin(), pMover->m_mapAttackersDamage.end() );
	std::sort( result.begin(), result.end(), []( const std::pair<u_long, __int64>& a, const std::pair<u_long, __int64>& b )
	{
		return a.second > b.second;
	} );

	CString strBestDamageDealer;
	if( !result.empty() )
	{
		FLWSUser* pTopUser = g_xWSUserManager->GetUserByPlayerID( result[0].first );
		if( IsValidObj( pTopUser ) )
			strBestDamageDealer = pTopUser->GetName();
		else
		{
			PlayerData* pData = CPlayerDataCenter::GetInstance()->GetPlayerData( result[0].first );
			if( pData != NULL )
				strBestDamageDealer = pData->szPlayer;
		}

		if( finishedWbd.rewardInvolved.IsValid() )
		{
			std::set<CString> setRewardedIP;
			for( std::vector<std::pair<u_long, __int64> >::const_iterator it = result.begin(); it != result.end(); ++it )
			{
				FLWSUser* pParticipant = g_xWSUserManager->GetUserByPlayerID( it->first );
				if( IsValidObj( pParticipant ) == FALSE )
					continue;

				if( it->second < finishedWbd.n64MinDamageReq )
				{
					pParticipant->AddText( _T( "Vous n'avez pas inflige suffisamment de degats pour recevoir une recompense." ) );
					continue;
				}

				const CString strIP = pParticipant->m_playAccount.lpAddr;
				if( setRewardedIP.find( strIP ) != setRewardedIP.end() )
				{
					pParticipant->AddText( _T( "Un seul de vos personnages peut recevoir une recompense de participation au Boss Mondial." ) );
					continue;
				}

				FLItemElem itemElem;
				if( PrepareWorldBossReward( finishedWbd.rewardInvolved, itemElem ) == false )
					continue;

				if( pParticipant->CreateItem( &itemElem ) == FALSE )
					SendWorldBossRewardMail(
						pParticipant->m_idPlayer,
						itemElem,
						"Felicitations ! Voici votre recompense de participation au Boss Mondial."
					);

				setRewardedIP.insert( strIP );
			}
		}

		size_t nTopCount = result.size();
		if( nTopCount > finishedWbd.vecRewardTop.size() )
			nTopCount = finishedWbd.vecRewardTop.size();

		for( size_t i = 0; i < nTopCount; ++i )
		{
			FLItemElem itemElem;
			if( PrepareWorldBossReward( finishedWbd.vecRewardTop[i], itemElem ) == false )
				continue;

			FLWSUser* pRankedUser = g_xWSUserManager->GetUserByPlayerID( result[i].first );
			if( IsValidObj( pRankedUser ) )
			{
				if( pRankedUser->CreateItem( &itemElem ) == FALSE )
					SendWorldBossRewardMail(
						pRankedUser->m_idPlayer,
						itemElem,
						"Felicitations ! Voici votre recompense de classement du Boss Mondial."
					);
			}
			else
			{
				SendWorldBossRewardMail(
					result[i].first,
					itemElem,
					"Felicitations ! Voici votre recompense de classement du Boss Mondial."
				);
			}
		}
	}

	pMover->m_bWorldBoss = false;
	pMover->m_mapAttackersDamage.clear();
	ResetActiveWorldBoss();
	SendWorldBossInfoToAllUsers();

	CString str;
	if( strBestDamageDealer.IsEmpty() )
		str = _T( "Le Boss Mondial est mort !" );
	else
		str.Format( _T( "Meilleur DPS : %s." ), static_cast<LPCTSTR>( strBestDamageDealer ) );
	g_DPCoreClient.SendSystem( str );
}

bool CWorldBossManager::BuildWorldBossInfo( DWORD& dwMonsterID, int& nHPPercent, DWORD& dwRemainingSeconds )
{
	dwMonsterID = NULL_ID;
	nHPPercent = 0;
	dwRemainingSeconds = 0;

	if( m_dwWorldBossId == NULL_ID )
		return false;

	CMover* pMoverWB = prj.GetMover( m_dwWorldBossId );
	if( IsValidObj( pMoverWB ) == FALSE || pMoverWB->m_bWorldBoss == false )
		return false;

	dwMonsterID = pMoverWB->GetIndex();
	nHPPercent = pMoverWB->GetHitPointPercent();
	if( nHPPercent < 0 )
		nHPPercent = 0;
	else if( nHPPercent > 100 )
		nHPPercent = 100;

	if( m_tExpirationTime > g_tCurrent )
	{
		const time_t tRemaining = m_tExpirationTime - g_tCurrent;
		dwRemainingSeconds = static_cast<DWORD>( tRemaining );
	}
	return true;
}

void CWorldBossManager::SendWorldBossSnapshot( FLWSUser* pUser, DWORD dwMonsterID, int nHPPercent, DWORD dwRemainingSeconds )
{
	if( IsValidObj( pUser ) == FALSE )
		return;

	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_WORLDBOSS_INFO;
	ar << dwMonsterID << nHPPercent << dwRemainingSeconds;
	u_long uBufSize = 0;
	LPBYTE lpBuf = ar.GetBuffer( &uBufSize );
	pUser->AddBlock( lpBuf, uBufSize );
}

void CWorldBossManager::SendWorldBossInfo( FLWSUser* pUser )
{
	DWORD dwMonsterID = NULL_ID;
	int nHPPercent = 0;
	DWORD dwRemainingSeconds = 0;
	BuildWorldBossInfo( dwMonsterID, nHPPercent, dwRemainingSeconds );
	SendWorldBossSnapshot( pUser, dwMonsterID, nHPPercent, dwRemainingSeconds );
}

void CWorldBossManager::SendWorldBossInfoToAllUsers()
{
	DWORD dwMonsterID = NULL_ID;
	int nHPPercent = 0;
	DWORD dwRemainingSeconds = 0;
	BuildWorldBossInfo( dwMonsterID, nHPPercent, dwRemainingSeconds );

	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_WORLDBOSS_INFO;
	ar << dwMonsterID << nHPPercent << dwRemainingSeconds;
	u_long uBufSize = 0;
	LPBYTE lpBuf = ar.GetBuffer( &uBufSize );
	g_xWSUserManager->AddBlock( lpBuf, uBufSize );
	m_dwLastWorldBossInfoTick = g_tmCurrent;
}

void CWorldBossManager::TeleportUserToWorldBoss( FLWSUser* pUser )
{
	if( IsValidObj( pUser ) == FALSE || m_dwWorldBossId == NULL_ID )
		return;

	CMover* pMoverWB = prj.GetMover( m_dwWorldBossId );
	if( IsValidObj( pMoverWB ) == FALSE || pMoverWB->m_bWorldBoss == false || pMoverWB->GetHitPoint() <= 0 )
		return;

	if( pUser->IsTeleportable() == FALSE )
	{
		pUser->AddText( _T( "Vous ne pouvez pas vous teleporter maintenant." ) );
		return;
	}

	if( m_currentWbd.dwWorldIDTeleport == NULL_ID || g_WorldMng.GetWorld( m_currentWbd.dwWorldIDTeleport ) == NULL )
		return;

#ifdef __LAYER_1015
	if( pUser->REPLACE( g_uIdofMulti, m_currentWbd.dwWorldIDTeleport, m_currentWbd.vPosTeleport, REPLACE_FORCE, nDefaultLayer ) == FALSE )
#else
	if( pUser->REPLACE( g_uIdofMulti, m_currentWbd.dwWorldIDTeleport, m_currentWbd.vPosTeleport, REPLACE_FORCE ) == FALSE )
#endif
		pUser->AddText( _T( "La teleportation vers le Boss Mondial a echoue." ) );
}
#endif // __WORLDSERVER

#ifdef __CLIENT
#include "ModelObject.h"

CWorldBossManager::CWorldBossManager()
	: m_nHPPercent( 0 )
	, m_pModelWorldBoss( NULL )
	, m_fModelRadius( 0.0f )
	, m_tExpirationTime( 0 )
{
}

CWorldBossManager::~CWorldBossManager()
{
	Clear();
}

CWorldBossManager* CWorldBossManager::GetInstance()
{
	static CWorldBossManager sWorldBossManager;
	return &sWorldBossManager;
}

void CWorldBossManager::Clear()
{
	m_nHPPercent = 0;
	SAFE_DELETE( m_pModelWorldBoss );
	m_fModelRadius = 0.0f;
	m_tExpirationTime = 0;
}

void CWorldBossManager::Process()
{
	if( m_pModelWorldBoss != NULL )
		m_pModelWorldBoss->FrameMove();
}

void CWorldBossManager::UpdateWorldBossInfo( DWORD dwMonsterID, int nHPPercent, DWORD dwRemainingSeconds )
{
	if( dwMonsterID == NULL_ID )
	{
		Clear();
		return;
	}

	if( nHPPercent < 0 )
		nHPPercent = 0;
	else if( nHPPercent > 100 )
		nHPPercent = 100;

	m_nHPPercent = nHPPercent;
	m_tExpirationTime = time_null() + static_cast<time_t>( dwRemainingSeconds );

	if( m_pModelWorldBoss != NULL && m_pModelWorldBoss->m_pModelElem != NULL && m_pModelWorldBoss->m_pModelElem->m_dwIndex == dwMonsterID )
		return;

	SAFE_DELETE( m_pModelWorldBoss );
	m_fModelRadius = 0.0f;

	m_pModelWorldBoss = static_cast<CModelObject*>( prj.m_modelMng.LoadModel( D3DDEVICE, OT_MOVER, dwMonsterID ) );
	if( m_pModelWorldBoss == NULL )
		return;

	prj.m_modelMng.LoadMotion( m_pModelWorldBoss, OT_MOVER, dwMonsterID, MTI_STAND );
	m_pModelWorldBoss->InitDeviceObjects( g_Neuz.GetDevice() );
	m_fModelRadius = m_pModelWorldBoss->GetRadius();
}
#endif // __CLIENT

#endif // __ZCK_WORLD_BOSS
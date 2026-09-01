#include "StdAfx.h"
#include "ZckInvasions.h"

#ifdef __ZCK_INVASIONS

#include <algorithm>
#include <chrono>
#include <limits>

#ifdef __WORLDSERVER
#include "CreateObj.h"
#include "worldmng.h"
#include "FLItemElem.h"
#include "post.h"
#include "../WORLDSERVER/User.h"
#include "../WORLDSERVER/DPDatabaseClient.h"
#include "../WORLDSERVER/DPCoreClient.h"

extern CWorldMng g_WorldMng;
extern CDPCoreClient g_DPCoreClient;
extern CDPDatabaseClient g_dpDBClient;
#endif // __WORLDSERVER

#ifdef __CLIENT
#include "../Neuz/DPClient.h"
extern CDPClient g_DPlay;
#endif // __CLIENT

namespace
{
	const DWORD ZCK_INVASION_NOTIFY_INTERVAL = SEC( 2 );

	__int64 GetSystemTimeMilliseconds()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch() ).count();
	}

	bool ResolveCurrentScriptNumber( CScript& script, int& nValue )
	{
		if( script.Token.IsEmpty() )
			return false;

		if( script.tokenType == NUMBER || script.tokenType == HEX )
		{
			nValue = static_cast<int>( _tcstol( static_cast<LPCTSTR>( script.Token ), NULL, 0 ) );
			return true;
		}

		return CScript::LookupDefine( script.Token, nValue ) == TRUE;
	}

	bool IsClosingBrace( const CScript& script )
	{
		return script.Token.GetLength() == 1 && script.Token[0] == _T( '}' );
	}

	bool ExpectToken( CScript& script, LPCTSTR pszExpected, LPCTSTR pszContext )
	{
		if( script.Token == pszExpected )
			return true;

		FLERROR_LOG(
			PROGRAM_NAME,
			_T( "CZckInvasions::LoadScript() - Token invalide dans %s. Attendu '%s', recu '%s'." ),
			pszContext,
			pszExpected,
			static_cast<LPCTSTR>( script.Token )
		);
		return false;
	}

#ifdef __WORLDSERVER
	bool PrepareInvasionReward( const ZckInvasionReward& reward, FLItemElem& itemElem )
	{
		if( reward.dwItemID == NULL_ID || reward.nItemCount <= 0 )
			return false;

		const PT_ITEM_SPEC pItemSpec = g_xSpecManager->GetSpecItem( reward.dwItemID );
		if( pItemSpec == NULL )
			return false;

		itemElem.m_dwItemId = reward.dwItemID;
		itemElem.m_nItemNum = reward.nItemCount;
		itemElem.m_bCharged = pItemSpec->bCharged;
		if( reward.bBind )
			itemElem.SetFlag( FLItemElem::binds );
		itemElem.SetSerialNumber();
		return true;
	}

	void SendInvasionRewardMail( u_long idPlayer, FLItemElem& itemElem, const char* pszText )
	{
		char szTitle[MAX_MAILTITLE] = { 0, };
		char szText[MAX_MAILTEXT] = { 0, };
		FLSPrintf( szTitle, _countof( szTitle ), "Invasion" );
		FLSPrintf( szText, _countof( szText ), "%s", pszText != NULL ? pszText : "" );
		g_dpDBClient.SendQueryPostMail( idPlayer, 0, itemElem, 0, szTitle, szText );
	}
#endif // __WORLDSERVER
}

CZckInvasions::CZckInvasions()
	: m_nMaxInvasions( 0 )
#ifdef __WORLDSERVER
	, m_dwLastNotifyTick( 0 )
	, m_ullLastScheduleKey( 0 )
#endif // __WORLDSERVER
#ifdef __CLIENT
	, m_bInitialRefreshRequested( false )
#endif // __CLIENT
{
}

CZckInvasions::~CZckInvasions()
{
}

CZckInvasions* CZckInvasions::GetInstance()
{
	static CZckInvasions sManager;
	return &sManager;
}

const ZckInvasionDefinition* CZckInvasions::GetDefinition( DWORD dwInvasionID ) const
{
	if( dwInvasionID == NULL_ID || dwInvasionID >= static_cast<DWORD>( m_vDefinitions.size() ) )
		return NULL;
	return &m_vDefinitions[dwInvasionID];
}

BOOL CZckInvasions::LoadScript()
{
#ifdef __WORLDSERVER
	if( m_vCurrentInvasions.empty() == false )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "CZckInvasions::LoadScript() - Rechargement refuse pendant une invasion active." ) );
		return FALSE;
	}
#endif // __WORLDSERVER

	CScript script;
	if( script.Load( _T( "ZckInvasions.inc" ) ) == FALSE )
	{
		TCHAR szCurrentDirectory[MAX_PATH] = { 0, };
		GetCurrentDirectory( _countof( szCurrentDirectory ), szCurrentDirectory );
		FLERROR_LOG(
			PROGRAM_NAME,
			_T( "CZckInvasions::LoadScript() - Impossible de trouver ou charger ZckInvasions.inc. ResourceFolder courant: %s" ),
			szCurrentDirectory
		);
		return FALSE;
	}

	std::vector<ZckInvasionOpenTime> openTimes;
	std::vector<ZckInvasionDefinition> definitions;
	int nMaxInvasions = 0;

	script.GetToken();
	if( script.tok == FINISHED || ExpectToken( script, _T( "MAX_INVASIONS" ), _T( "racine" ) ) == false )
		return FALSE;

	nMaxInvasions = script.GetNumber();
	if( nMaxInvasions <= 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "CZckInvasions::LoadScript() - MAX_INVASIONS doit etre superieur a 0." ) );
		return FALSE;
	}

	script.GetToken();
	while( script.tok != FINISHED )
	{
		if( script.Token == _T( "OPEN_DAYS" ) )
		{
			script.GetToken();
			if( ExpectToken( script, _T( "{" ), _T( "OPEN_DAYS" ) ) == false )
				return FALSE;

			script.GetToken();
			while( script.tok != FINISHED && IsClosingBrace( script ) == false )
			{
				int nDay = 0;
				if( ResolveCurrentScriptNumber( script, nDay ) == false )
				{
					FLERROR_LOG(
						PROGRAM_NAME,
						_T( "CZckInvasions::LoadScript() - Jour OPEN_DAYS invalide ligne %d: %s" ),
						script.GetLineNum(),
						static_cast<LPCTSTR>( script.Token )
					);
					return FALSE;
				}

				const int nHour = script.GetNumber();
				const int nMinute = script.GetNumber();
				if( nDay < 1 || nDay > 7 || nHour < 0 || nHour > 23 || nMinute < 0 || nMinute > 59 )
				{
					FLERROR_LOG(
						PROGRAM_NAME,
						_T( "CZckInvasions::LoadScript() - Horaire invalide ligne %d: jour=%d heure=%d minute=%d." ),
						script.GetLineNum(),
						nDay,
						nHour,
						nMinute
					);
					return FALSE;
				}

				ZckInvasionOpenTime openTime;
				openTime.nDay = nDay;
				openTime.nHour = nHour;
				openTime.nMinute = nMinute;
				openTimes.push_back( openTime );
				script.GetToken();
			}
		}
		else if( script.Token == _T( "INVASIONS" ) )
		{
			script.GetToken();
			if( ExpectToken( script, _T( "{" ), _T( "INVASIONS" ) ) == false )
				return FALSE;

			script.GetToken();
			while( script.tok != FINISHED && IsClosingBrace( script ) == false )
			{
				if( ExpectToken( script, _T( "NEW" ), _T( "INVASIONS" ) ) == false )
					return FALSE;

				ZckInvasionDefinition definition;
				script.GetToken();
				if( ExpectToken( script, _T( "{" ), _T( "NEW" ) ) == false )
					return FALSE;

				script.GetToken();
				while( script.tok != FINISHED && IsClosingBrace( script ) == false )
				{
					if( script.Token == _T( "MAP" ) )
					{
						script.GetToken();
						int nWorldID = 0;
						if( ResolveCurrentScriptNumber( script, nWorldID ) == false || nWorldID <= 0 )
						{
							FLERROR_LOG(
								PROGRAM_NAME,
								_T( "CZckInvasions::LoadScript() - MAP invalide ligne %d: %s" ),
								script.GetLineNum(),
								static_cast<LPCTSTR>( script.Token )
							);
							return FALSE;
						}
						definition.dwWorldID = static_cast<DWORD>( nWorldID );
					}
					else if( script.Token == _T( "STARTING_POS" ) )
					{
						script.GetToken();
						if( ExpectToken( script, _T( "(" ), _T( "STARTING_POS" ) ) == false )
							return FALSE;
						definition.vStartingPos.x = script.GetFloat();
						script.GetToken();
						definition.vStartingPos.y = script.GetFloat();
						script.GetToken();
						definition.vStartingPos.z = script.GetFloat();
						script.GetToken();
						if( ExpectToken( script, _T( ")" ), _T( "STARTING_POS" ) ) == false )
							return FALSE;
					}
					else if( script.Token == _T( "RADIUS" ) )
					{
						definition.fSpawnRadius = script.GetFloat();
					}
					else if( script.Token == _T( "MAX_TIME" ) )
					{
						definition.nMaxTimeMinutes = script.GetNumber();
					}
					else if( script.Token == _T( "DIFFICULTY" ) )
					{
						definition.nDifficulty = script.GetNumber();
					}
					else if( script.Token == _T( "WAVES" ) )
					{
						script.GetToken();
						if( ExpectToken( script, _T( "{" ), _T( "WAVES" ) ) == false )
							return FALSE;

						script.GetToken();
						while( script.tok != FINISHED && IsClosingBrace( script ) == false )
						{
							if( ExpectToken( script, _T( "ADD" ), _T( "WAVES" ) ) == false )
								return FALSE;

							ZckInvasionWave wave;
							wave.nPercentBeforeNext = script.GetNumber();
							if( wave.nPercentBeforeNext < 1 || wave.nPercentBeforeNext > 100 )
							{
								FLERROR_LOG(
									PROGRAM_NAME,
									_T( "CZckInvasions::LoadScript() - Pourcentage de vague invalide ligne %d: %d." ),
									script.GetLineNum(),
									wave.nPercentBeforeNext
								);
								return FALSE;
							}

							script.GetToken();
							if( ExpectToken( script, _T( "{" ), _T( "ADD" ) ) == false )
								return FALSE;

							script.GetToken();
							while( script.tok != FINISHED && IsClosingBrace( script ) == false )
							{
								const CString strMonsterToken = script.Token;
								int nMonsterID = 0;
								const bool bMonsterIDResolved = ResolveCurrentScriptNumber( script, nMonsterID );

								ZckInvasionMonster monster;
								if( bMonsterIDResolved && nMonsterID > 0 )
									monster.dwMonsterID = static_cast<DWORD>( nMonsterID );
								monster.nMonsterCount = script.GetNumber();
								monster.bAggressive = script.GetNumber() != 0;

								if( bMonsterIDResolved == false || nMonsterID <= 0 )
								{
									FLERROR_LOG(
										PROGRAM_NAME,
										_T( "CZckInvasions::LoadScript() - Monster ID non resolu ligne %d: %s. Entree ignoree." ),
										script.GetLineNum(),
										static_cast<LPCTSTR>( strMonsterToken )
									);
								}
								else if( monster.nMonsterCount <= 0 )
								{
									FLERROR_LOG(
										PROGRAM_NAME,
										_T( "CZckInvasions::LoadScript() - Quantite monstre invalide ligne %d: id=%u count=%d. Entree ignoree." ),
										script.GetLineNum(),
										monster.dwMonsterID,
										monster.nMonsterCount
									);
								}
								else
								{
									wave.vMonsters.push_back( monster );
								}

								script.GetToken();
							}

							if( wave.vMonsters.empty() )
							{
								FLERROR_LOG(
									PROGRAM_NAME,
									_T( "CZckInvasions::LoadScript() - Une vague ne contient aucun monstre exploitable ligne %d." ),
									script.GetLineNum()
								);
								return FALSE;
							}

							definition.vWaves.push_back( wave );
							script.GetToken();
						}
					}
					else if( script.Token == _T( "REWARDS" ) )
					{
						script.GetToken();
						if( ExpectToken( script, _T( "{" ), _T( "REWARDS" ) ) == false )
							return FALSE;

						script.GetToken();
						while( script.tok != FINISHED && IsClosingBrace( script ) == false )
						{
							if( ExpectToken( script, _T( "RANK" ), _T( "REWARDS" ) ) == false )
								return FALSE;

							ZckInvasionReward reward;
							script.GetToken();
							if( ExpectToken( script, _T( "(" ), _T( "RANK" ) ) == false )
								return FALSE;
							reward.nMinRank = script.GetNumber();
							script.GetToken();
							reward.nMaxRank = script.GetNumber();
							script.GetToken();
							if( ExpectToken( script, _T( ")" ), _T( "RANK" ) ) == false )
								return FALSE;

							script.GetToken();
							if( ExpectToken( script, _T( "REWARDS" ), _T( "RANK" ) ) == false )
								return FALSE;
							script.GetToken();
							if( ExpectToken( script, _T( "(" ), _T( "REWARDS" ) ) == false )
								return FALSE;
							reward.dwItemID = static_cast<DWORD>( script.GetNumber() );
							script.GetToken();
							reward.nItemCount = script.GetNumber();
							script.GetToken();
							script.GetToken();
							reward.bBind = ( script.Token.CompareNoCase( _T( "true" ) ) == 0 || _ttoi( script.Token ) != 0 );
							script.GetToken();
							if( ExpectToken( script, _T( ")" ), _T( "REWARDS" ) ) == false )
								return FALSE;

							if( reward.nMinRank <= 0 || reward.nMaxRank < reward.nMinRank || reward.dwItemID == 0 || reward.dwItemID == NULL_ID || reward.nItemCount <= 0 )
							{
								FLERROR_LOG(
									PROGRAM_NAME,
									_T( "CZckInvasions::LoadScript() - Recompense invalide ligne %d rank=%d-%d item=%u count=%d. Entree ignoree." ),
									script.GetLineNum(),
									reward.nMinRank,
									reward.nMaxRank,
									reward.dwItemID,
									reward.nItemCount
								);
							}
							else
							{
#ifdef __WORLDSERVER
								if( g_xSpecManager->GetSpecItem( reward.dwItemID ) == NULL )
								{
									FLERROR_LOG(
										PROGRAM_NAME,
										_T( "CZckInvasions::LoadScript() - ItemSpec %u absent au chargement. La recompense est conservee mais sera revalidee a la distribution." ),
										reward.dwItemID
									);
								}
#endif // __WORLDSERVER
								definition.vRewards.push_back( reward );
							}

							script.GetToken();
						}
					}
					else
					{
						FLERROR_LOG(
							PROGRAM_NAME,
							_T( "CZckInvasions::LoadScript() - Cle inconnue dans NEW ligne %d: %s" ),
							script.GetLineNum(),
							static_cast<LPCTSTR>( script.Token )
						);
						return FALSE;
					}

					script.GetToken();
				}

				if( definition.dwWorldID == NULL_ID || definition.fSpawnRadius <= 0.0f || definition.nMaxTimeMinutes <= 0 || definition.vWaves.empty() )
				{
					FLERROR_LOG(
						PROGRAM_NAME,
						_T( "CZckInvasions::LoadScript() - Definition d'invasion incomplete ligne %d." ),
						script.GetLineNum()
					);
					return FALSE;
				}

				definitions.push_back( definition );
				script.GetToken();
			}
		}
		else
		{
			FLERROR_LOG(
				PROGRAM_NAME,
				_T( "CZckInvasions::LoadScript() - Cle racine inconnue ligne %d: %s" ),
				script.GetLineNum(),
				static_cast<LPCTSTR>( script.Token )
			);
			return FALSE;
		}

		script.GetToken();
	}

	if( openTimes.empty() )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "CZckInvasions::LoadScript() - Aucun horaire OPEN_DAYS valide n'est charge." ) );
		return FALSE;
	}

	if( definitions.empty() )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "CZckInvasions::LoadScript() - Aucune invasion valide chargee." ) );
		return FALSE;
	}

	m_nMaxInvasions = nMaxInvasions;
	m_vOpenTimes.swap( openTimes );
	m_vDefinitions.swap( definitions );
#ifdef __WORLDSERVER
	m_dwLastNotifyTick = 0;
	m_ullLastScheduleKey = 0;
#endif // __WORLDSERVER
#ifdef __CLIENT
	m_vCurrentInvasions.clear();
	m_bInitialRefreshRequested = false;
#endif // __CLIENT

	TCHAR szCurrentDirectory[MAX_PATH] = { 0, };
	GetCurrentDirectory( _countof( szCurrentDirectory ), szCurrentDirectory );

	FLINFO_LOG(
		PROGRAM_NAME,
		_T( "CZckInvasions::LoadScript() - ZckInvasions.inc charge avec succes: %u invasion(s), %u horaire(s), maximum simultane=%d. ResourceFolder courant: %s" ),
		static_cast<unsigned int>( m_vDefinitions.size() ),
		static_cast<unsigned int>( m_vOpenTimes.size() ),
		m_nMaxInvasions,
		szCurrentDirectory
	);
	return TRUE;
}

#ifdef __WORLDSERVER
ULONGLONG CZckInvasions::BuildScheduleKey( const CTime& ctime ) const
{
	ULONGLONG ullKey = static_cast<ULONGLONG>( ctime.GetYear() );
	ullKey = ullKey * 13ULL + static_cast<ULONGLONG>( ctime.GetMonth() );
	ullKey = ullKey * 32ULL + static_cast<ULONGLONG>( ctime.GetDay() );
	ullKey = ullKey * 24ULL + static_cast<ULONGLONG>( ctime.GetHour() );
	ullKey = ullKey * 60ULL + static_cast<ULONGLONG>( ctime.GetMinute() );
	return ullKey;
}

ZckCurrentInvasion* CZckInvasions::FindCurrentInvasion( DWORD dwInvasionID )
{
	for( std::vector<ZckCurrentInvasion>::iterator it = m_vCurrentInvasions.begin(); it != m_vCurrentInvasions.end(); ++it )
	{
		if( it->dwInvasionID == dwInvasionID )
			return &(*it);
	}
	return NULL;
}

const ZckCurrentInvasion* CZckInvasions::FindCurrentInvasion( DWORD dwInvasionID ) const
{
	for( std::vector<ZckCurrentInvasion>::const_iterator it = m_vCurrentInvasions.begin(); it != m_vCurrentInvasions.end(); ++it )
	{
		if( it->dwInvasionID == dwInvasionID )
			return &(*it);
	}
	return NULL;
}

bool CZckInvasions::IsDefinitionActive( DWORD dwInvasionID ) const
{
	return FindCurrentInvasion( dwInvasionID ) != NULL;
}

bool CZckInvasions::IsFinished( ZckCurrentInvasion& current ) const
{
	const ZckInvasionDefinition* pDefinition = GetDefinition( current.dwInvasionID );
	if( pDefinition == NULL )
		return true;

	const __int64 nExpirationMs = current.nStartingTimeMs + static_cast<__int64>( pDefinition->nMaxTimeMinutes ) * 60LL * 1000LL;
	if( GetSystemTimeMilliseconds() >= nExpirationMs )
		return true;

	return current.nCurrentWave >= 0 && current.nCurrentWave >= static_cast<int>( pDefinition->vWaves.size() );
}

bool CZckInvasions::IsWaveComplete( ZckCurrentInvasion& current )
{
	const ZckInvasionDefinition* pDefinition = GetDefinition( current.dwInvasionID );
	if( pDefinition == NULL )
		return true;

	if( current.nCurrentWave < 0 )
		return true;

	if( current.nCurrentWave >= static_cast<int>( pDefinition->vWaves.size() ) )
		return false;

	if( current.vCurrentWaveMonsters.empty() )
	{
		current.nCurrentPercent = 100;
		current.nMonstersAlive = 0;
		return true;
	}

	int nDeadMonsterCount = 0;
	for( std::vector<OBJID>::const_iterator it = current.vCurrentWaveMonsters.begin(); it != current.vCurrentWaveMonsters.end(); ++it )
	{
		CMover* pMover = prj.GetMover( *it );
		if( IsValidObj( pMover ) == FALSE || pMover->IsDie() )
			++nDeadMonsterCount;
	}

	current.nMonstersAlive = static_cast<int>( current.vCurrentWaveMonsters.size() ) - nDeadMonsterCount;
	current.nCurrentPercent = static_cast<int>( ( 100LL * nDeadMonsterCount ) / static_cast<__int64>( current.vCurrentWaveMonsters.size() ) );
	return current.nCurrentPercent >= pDefinition->vWaves[current.nCurrentWave].nPercentBeforeNext;
}

void CZckInvasions::SetNextWave( ZckCurrentInvasion& current )
{
	const ZckInvasionDefinition* pDefinition = GetDefinition( current.dwInvasionID );
	if( pDefinition == NULL )
		return;

	CWorld* pWorld = g_WorldMng.GetWorld( pDefinition->dwWorldID );
	if( pWorld == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "CZckInvasions::SetNextWave() - World %u introuvable." ), pDefinition->dwWorldID );
		current.nCurrentWave = static_cast<int>( pDefinition->vWaves.size() );
		return;
	}

	current.vCurrentWaveMonsters.clear();
	++current.nCurrentWave;
	current.nCurrentPercent = 0;
	current.nMonstersAlive = 0;

	if( current.nCurrentWave >= static_cast<int>( pDefinition->vWaves.size() ) )
		return;

	const ZckInvasionWave& wave = pDefinition->vWaves[current.nCurrentWave];
	for( std::vector<ZckInvasionMonster>::const_iterator itMonster = wave.vMonsters.begin(); itMonster != wave.vMonsters.end(); ++itMonster )
	{
		if( prj.GetMoverProp( itMonster->dwMonsterID ) == NULL )
		{
			FLERROR_LOG(
				PROGRAM_NAME,
				_T( "CZckInvasions::SetNextWave() - Mover %u absent de PropMover. Groupe de %d monstre(s) ignore." ),
				itMonster->dwMonsterID,
				itMonster->nMonsterCount
			);
			continue;
		}

		for( int i = 0; i < itMonster->nMonsterCount; ++i )
		{
			D3DXVECTOR3 vPos = pDefinition->vStartingPos;
			vPos.x += xRandomF( pDefinition->fSpawnRadius ) - ( pDefinition->fSpawnRadius / 2.0f );
			vPos.z += xRandomF( pDefinition->fSpawnRadius ) - ( pDefinition->fSpawnRadius / 2.0f );

#ifdef __LAYER_1015
			CMover* pMover = CreateMover( pWorld, itMonster->dwMonsterID, vPos, nDefaultLayer );
#else
			CMover* pMover = CreateMover( pWorld, itMonster->dwMonsterID, vPos );
#endif // __LAYER_1015
			if( IsValidObj( pMover ) == FALSE )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "CZckInvasions::SetNextWave() - Echec spawn mover %u." ), itMonster->dwMonsterID );
				continue;
			}

			if( itMonster->bAggressive )
				pMover->m_bActiveAttack = true;

			const OBJID objid = pMover->GetId();
			current.vCurrentWaveMonsters.push_back( objid );
			current.vSpawnedMonsters.push_back( objid );
			++current.nMonstersAlive;
		}
	}

	if( current.nMonstersAlive <= 0 )
	{
		FLERROR_LOG(
			PROGRAM_NAME,
			_T( "CZckInvasions::SetNextWave() - invasion=%u vague=%d/%u: aucun monstre n'a pu etre spawne." ),
			current.dwInvasionID,
			current.nCurrentWave + 1,
			static_cast<unsigned int>( pDefinition->vWaves.size() )
		);
	}

	FLINFO_LOG(
		PROGRAM_NAME,
		_T( "CZckInvasions::SetNextWave() - invasion=%u vague=%d/%u monstres=%d." ),
		current.dwInvasionID,
		current.nCurrentWave + 1,
		static_cast<unsigned int>( pDefinition->vWaves.size() ),
		current.nMonstersAlive
	);
}

void CZckInvasions::CleanupSpawnedMonsters( ZckCurrentInvasion& current )
{
	for( std::vector<OBJID>::const_iterator it = current.vSpawnedMonsters.begin(); it != current.vSpawnedMonsters.end(); ++it )
	{
		CMover* pMover = prj.GetMover( *it );
		if( IsValidObj( pMover ) )
			pMover->Delete();
	}
	current.vCurrentWaveMonsters.clear();
	current.vSpawnedMonsters.clear();
	current.nMonstersAlive = 0;
}

void CZckInvasions::SendUserRewards( ZckCurrentInvasion& current )
{
	const ZckInvasionDefinition* pDefinition = GetDefinition( current.dwInvasionID );
	if( pDefinition == NULL )
		return;

	std::vector<std::pair<u_long, __int64> > ranking;
	for( std::map<u_long, __int64>::const_iterator it = current.mPlayerDamage.begin(); it != current.mPlayerDamage.end(); ++it )
		ranking.push_back( std::make_pair( it->first, it->second ) );

	std::sort(
		ranking.begin(),
		ranking.end(),
		[]( const std::pair<u_long, __int64>& left, const std::pair<u_long, __int64>& right )
		{
			if( left.second != right.second )
				return left.second > right.second;
			return left.first < right.first;
		}
	);

	for( size_t i = 0; i < ranking.size(); ++i )
	{
		const int nRank = static_cast<int>( i ) + 1;
		const u_long idPlayer = ranking[i].first;
		const __int64 nDamage = ranking[i].second;
		FLWSUser* pUser = g_xWSUserManager->GetUserByPlayerID( idPlayer );

		if( nDamage <= 0 )
		{
			if( IsValidObj( pUser ) )
				pUser->AddText( _T( "Vous n'avez malheureusement inflige aucun degat pendant cette invasion !" ) );
			continue;
		}

		for( std::vector<ZckInvasionReward>::const_iterator itReward = pDefinition->vRewards.begin(); itReward != pDefinition->vRewards.end(); ++itReward )
		{
			if( nRank < itReward->nMinRank || nRank > itReward->nMaxRank )
				continue;

			FLItemElem itemElem;
			if( PrepareInvasionReward( *itReward, itemElem ) == false )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "CZckInvasions::SendUserRewards() - Recompense invalide item=%u rank=%d." ), itReward->dwItemID, nRank );
				break;
			}

			CString strMessage;
			strMessage.Format( _T( "Bonjour ! Voici votre recompense d'invasion ! Votre place est %d !" ), nRank );

			if( IsValidObj( pUser ) )
			{
				if( pUser->CreateItem( &itemElem ) == FALSE )
					SendInvasionRewardMail( idPlayer, itemElem, static_cast<LPCTSTR>( strMessage ) );
				pUser->AddText( strMessage );
			}
			else
			{
				SendInvasionRewardMail( idPlayer, itemElem, static_cast<LPCTSTR>( strMessage ) );
			}
			break;
		}
	}
}

void CZckInvasions::SerializeCurrent( CAr& ar, const ZckCurrentInvasion& current ) const
{
	ar << current.dwInvasionID;
	ar << current.nStartingTimeMs;
	const DWORD dwRankingCount = static_cast<DWORD>( current.mPlayerDamage.size() );
	ar << dwRankingCount;
	for( std::map<u_long, __int64>::const_iterator it = current.mPlayerDamage.begin(); it != current.mPlayerDamage.end(); ++it )
		ar << it->first << it->second;
	ar << current.nCurrentWave;
	ar << current.nMonstersAlive;
}

void CZckInvasions::SendSnapshot( FLWSUser* pUser, ZckInvasionPacketAction action, const ZckCurrentInvasion* pCurrent )
{
	if( IsValidObj( pUser ) == FALSE )
		return;

	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_ZCK_INVASIONS;
	ar << static_cast<int>( action );

	if( action == ZckInvasionPacketAction::RefreshAll )
	{
		const DWORD dwCount = static_cast<DWORD>( m_vCurrentInvasions.size() );
		ar << dwCount;
		for( std::vector<ZckCurrentInvasion>::const_iterator it = m_vCurrentInvasions.begin(); it != m_vCurrentInvasions.end(); ++it )
			SerializeCurrent( ar, *it );
	}
	else if( action == ZckInvasionPacketAction::End )
	{
		if( pCurrent == NULL )
			return;
		ar << pCurrent->dwInvasionID;
	}
	else
	{
		if( pCurrent == NULL )
			return;
		SerializeCurrent( ar, *pCurrent );
	}

	u_long uBufSize = 0;
	LPBYTE lpBuf = ar.GetBuffer( &uBufSize );
	pUser->AddBlock( lpBuf, uBufSize );
}

void CZckInvasions::SendSnapshotToAll( ZckInvasionPacketAction action, const ZckCurrentInvasion* pCurrent )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_ZCK_INVASIONS;
	ar << static_cast<int>( action );

	if( action == ZckInvasionPacketAction::RefreshAll )
	{
		const DWORD dwCount = static_cast<DWORD>( m_vCurrentInvasions.size() );
		ar << dwCount;
		for( std::vector<ZckCurrentInvasion>::const_iterator it = m_vCurrentInvasions.begin(); it != m_vCurrentInvasions.end(); ++it )
			SerializeCurrent( ar, *it );
	}
	else if( action == ZckInvasionPacketAction::End )
	{
		if( pCurrent == NULL )
			return;
		ar << pCurrent->dwInvasionID;
	}
	else
	{
		if( pCurrent == NULL )
			return;
		SerializeCurrent( ar, *pCurrent );
	}

	u_long uBufSize = 0;
	LPBYTE lpBuf = ar.GetBuffer( &uBufSize );
	g_xWSUserManager->AddBlock( lpBuf, uBufSize );
}

void CZckInvasions::NotifyNearbyUsers( const ZckCurrentInvasion& current )
{
	const ZckInvasionDefinition* pDefinition = GetDefinition( current.dwInvasionID );
	if( pDefinition == NULL )
		return;

	CWorld* pWorld = g_WorldMng.GetWorld( pDefinition->dwWorldID );
	if( pWorld == NULL )
		return;

	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_ZCK_INVASIONS;
	ar << static_cast<int>( ZckInvasionPacketAction::Notify );
	SerializeCurrent( ar, current );
	u_long uBufSize = 0;
	LPBYTE lpBuf = ar.GetBuffer( &uBufSize );

	g_xWSUserManager->AddBlock( pWorld, pDefinition->vStartingPos, 100, lpBuf, uBufSize );
}

void CZckInvasions::EndInvasion( ZckCurrentInvasion& current )
{
	SendUserRewards( current );
	CleanupSpawnedMonsters( current );
	SendSnapshotToAll( ZckInvasionPacketAction::End, &current );
	FLINFO_LOG( PROGRAM_NAME, _T( "CZckInvasions::EndInvasion() - invasion=%u terminee." ), current.dwInvasionID );
}

void CZckInvasions::ProcessCurrentInvasions()
{
	const DWORD dwNow = GetTickCount();
	const bool bNotify = ( m_dwLastNotifyTick == 0 || dwNow - m_dwLastNotifyTick >= ZCK_INVASION_NOTIFY_INTERVAL );
	if( bNotify )
		m_dwLastNotifyTick = dwNow;

	for( std::vector<ZckCurrentInvasion>::iterator it = m_vCurrentInvasions.begin(); it != m_vCurrentInvasions.end(); )
	{
		if( IsFinished( *it ) )
		{
			EndInvasion( *it );
			it = m_vCurrentInvasions.erase( it );
			continue;
		}

		if( IsWaveComplete( *it ) )
			SetNextWave( *it );

		if( bNotify )
			NotifyNearbyUsers( *it );

		++it;
	}
}

void CZckInvasions::Process()
{
	ProcessCurrentInvasions();

	if( m_vDefinitions.empty() || m_vOpenTimes.empty() || m_nMaxInvasions <= 0 )
		return;
	if( m_vCurrentInvasions.size() >= static_cast<size_t>( m_nMaxInvasions ) )
		return;

	const CTime ctime = CTime::GetCurrentTime();
	bool bScheduledNow = false;
	for( std::vector<ZckInvasionOpenTime>::const_iterator it = m_vOpenTimes.begin(); it != m_vOpenTimes.end(); ++it )
	{
		if( ctime.GetDayOfWeek() == it->nDay && ctime.GetHour() == it->nHour && ctime.GetMinute() == it->nMinute )
		{
			bScheduledNow = true;
			break;
		}
	}

	if( bScheduledNow == false )
		return;

	const ULONGLONG ullScheduleKey = BuildScheduleKey( ctime );
	if( ullScheduleKey == m_ullLastScheduleKey )
		return;
	m_ullLastScheduleKey = ullScheduleKey;

	std::vector<DWORD> available;
	for( DWORD dwID = 0; dwID < static_cast<DWORD>( m_vDefinitions.size() ); ++dwID )
	{
		if( IsDefinitionActive( dwID ) == false )
			available.push_back( dwID );
	}

	if( available.empty() )
		return;

	const DWORD dwSelected = available[xRandom( static_cast<int>( available.size() ) )];
	CreateInvasion( dwSelected, NULL );
}

BOOL CZckInvasions::CreateInvasion( DWORD dwInvasionID, CString* pFailureReason )
{
	if( pFailureReason != NULL )
		pFailureReason->Empty();

	if( m_vDefinitions.empty() )
	{
		if( pFailureReason != NULL )
			*pFailureReason = _T( "Aucune invasion n'est chargee." );
		return FALSE;
	}

	if( dwInvasionID >= static_cast<DWORD>( m_vDefinitions.size() ) )
	{
		if( pFailureReason != NULL )
			pFailureReason->Format( _T( "ID invasion invalide. Plage: 0-%u." ), static_cast<unsigned int>( m_vDefinitions.size() - 1 ) );
		return FALSE;
	}

	if( m_vCurrentInvasions.size() >= static_cast<size_t>( m_nMaxInvasions ) )
	{
		if( pFailureReason != NULL )
			*pFailureReason = _T( "Le nombre maximum d'invasions simultanees est atteint." );
		return FALSE;
	}

	if( IsDefinitionActive( dwInvasionID ) )
	{
		if( pFailureReason != NULL )
			*pFailureReason = _T( "Cette invasion est deja active." );
		return FALSE;
	}

	const ZckInvasionDefinition& definition = m_vDefinitions[dwInvasionID];
	if( g_WorldMng.GetWorld( definition.dwWorldID ) == NULL )
	{
		if( pFailureReason != NULL )
			pFailureReason->Format( _T( "World %u introuvable." ), definition.dwWorldID );
		return FALSE;
	}

	ZckCurrentInvasion current;
	current.dwInvasionID = dwInvasionID;
	current.nStartingTimeMs = GetSystemTimeMilliseconds();
	m_vCurrentInvasions.push_back( current );
	ZckCurrentInvasion& created = m_vCurrentInvasions.back();

	g_DPCoreClient.SendSystem( "Une invasion a demarre !" );
	g_DPCoreClient.SendSystem( "Consultez votre carte (M) pour connaitre son emplacement !" );
	CString strMessage;
	strMessage.Format( _T( "Elle restera %d minutes ! Bonne chance !" ), definition.nMaxTimeMinutes );
	g_DPCoreClient.SendSystem( strMessage );

	SendSnapshotToAll( ZckInvasionPacketAction::Add, &created );
	FLINFO_LOG( PROGRAM_NAME, _T( "CZckInvasions::CreateInvasion() - invasion=%u world=%u maxTime=%d." ), dwInvasionID, definition.dwWorldID, definition.nMaxTimeMinutes );
	return TRUE;
}

void CZckInvasions::AddUserDamage( CMover* pAttacker, CMover* pDefender, int nDamage )
{
	if( IsValidObj( pAttacker ) == FALSE || IsValidObj( pDefender ) == FALSE || pAttacker->IsPlayer() == FALSE || nDamage <= 0 )
		return;

	const u_long idPlayer = pAttacker->m_idPlayer;
	if( idPlayer == 0 || idPlayer == NULL_ID )
		return;

	for( std::vector<ZckCurrentInvasion>::iterator itInvasion = m_vCurrentInvasions.begin(); itInvasion != m_vCurrentInvasions.end(); ++itInvasion )
	{
		if( std::find( itInvasion->vCurrentWaveMonsters.begin(), itInvasion->vCurrentWaveMonsters.end(), pDefender->GetId() ) == itInvasion->vCurrentWaveMonsters.end() )
			continue;

		__int64& nStoredDamage = itInvasion->mPlayerDamage[idPlayer];
		const __int64 nMax = ( std::numeric_limits<__int64>::max )();
		if( nStoredDamage > nMax - static_cast<__int64>( nDamage ) )
			nStoredDamage = nMax;
		else
			nStoredDamage += static_cast<__int64>( nDamage );
		break;
	}
}

void CZckInvasions::SendRefreshAll( FLWSUser* pUser )
{
	SendSnapshot( pUser, ZckInvasionPacketAction::RefreshAll, NULL );
}
#endif // __WORLDSERVER

#ifdef __CLIENT
void CZckInvasions::ProcessClientSync()
{
	if( g_pPlayer == NULL )
	{
		m_bInitialRefreshRequested = false;
		m_vCurrentInvasions.clear();
		return;
	}

	if( m_bInitialRefreshRequested == false )
	{
		g_DPlay.SendHdr( PACKETTYPE_ZCK_INVASIONS_REFRESH );
		m_bInitialRefreshRequested = true;
	}
}

bool CZckInvasions::GetLocalInvasionID( DWORD& dwInvasionID ) const
{
	dwInvasionID = NULL_ID;
	if( g_pPlayer == NULL || g_pPlayer->GetWorld() == NULL )
		return false;

	const DWORD dwPlayerWorldID = g_pPlayer->GetWorld()->GetID();
	for( std::vector<ZckCurrentInvasion>::const_iterator it = m_vCurrentInvasions.begin(); it != m_vCurrentInvasions.end(); ++it )
	{
		const ZckInvasionDefinition* pDefinition = GetDefinition( it->dwInvasionID );
		if( pDefinition == NULL || pDefinition->dwWorldID != dwPlayerWorldID )
			continue;
		if( g_pPlayer->IsRangeObj( pDefinition->vStartingPos, pDefinition->fSpawnRadius ) == false )
			continue;

		dwInvasionID = it->dwInvasionID;
		return true;
	}
	return false;
}

void CZckInvasions::ClientUpsertInvasion( const ZckCurrentInvasion& current )
{
	if( GetDefinition( current.dwInvasionID ) == NULL )
		return;

	for( std::vector<ZckCurrentInvasion>::iterator it = m_vCurrentInvasions.begin(); it != m_vCurrentInvasions.end(); ++it )
	{
		if( it->dwInvasionID == current.dwInvasionID )
		{
			*it = current;
			return;
		}
	}
	m_vCurrentInvasions.push_back( current );
}

void CZckInvasions::ClientReplaceAll( const std::vector<ZckCurrentInvasion>& invasions )
{
	m_vCurrentInvasions.clear();
	for( std::vector<ZckCurrentInvasion>::const_iterator it = invasions.begin(); it != invasions.end(); ++it )
	{
		if( GetDefinition( it->dwInvasionID ) != NULL )
			m_vCurrentInvasions.push_back( *it );
	}
}

void CZckInvasions::ClientDeleteInvasion( DWORD dwInvasionID )
{
	for( std::vector<ZckCurrentInvasion>::iterator it = m_vCurrentInvasions.begin(); it != m_vCurrentInvasions.end(); ++it )
	{
		if( it->dwInvasionID == dwInvasionID )
		{
			m_vCurrentInvasions.erase( it );
			return;
		}
	}
}
#endif // __CLIENT

#endif // __ZCK_INVASIONS
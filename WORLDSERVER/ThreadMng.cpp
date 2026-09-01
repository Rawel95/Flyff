#include "stdafx.h"
#include "ThreadMng.h"
#include <WorldMng.h>
#ifdef __ZCK_TICK_FIX
#include <World.h>
#endif // __ZCK_TICK_FIX
#include "User.h"
#include "WorldServer.h"
#include "GlobalTime.h"
#include <party.h>
#include <guildquest.h>

#include <eveschool.h>
extern	CGuildCombat	g_GuildCombatMng;

#include <SecretRoom.h>
#include <CreateMonster.h>
#include "rangda.h"
#include <RainbowRace.h>
#include <PCBang.h>
#include <InstanceDungeonParty.h>

#include <Quiz.h>

#include <spevent.h>
#include <guild.h>
#include <guildwar.h>

#include <serialnumber.h>
#include "NetSerialNumberRequest.h"

#include "FLRandomRespawnNPC.h"
#include "FLCooperativeContributions.h"

#include "FLEventArenaGlobal.h"

#ifdef __ZCK_RECORD
#include "ZckRecordManager.h"
#endif // __ZCK_RECORD

#ifdef __ZCK_WORLD_BOSS
#include "../_Common/WorldBoss.h"
#endif // __ZCK_WORLD_BOSS

#ifdef __ZCK_INVASIONS
#include "../_Common/ZckInvasions.h"
#endif // __ZCK_INVASIONS

#include <dump/FLDumpInstaller.h>
#include "../_CommonDefine/FLCommonDumper.h"

#include "FLShutdownRule_Mng.h"

#include "FLFlyffPieceDailyPayment.h"

extern	char				g_szCacheBindIP[16];
extern	char				g_szDBAddr[16];
extern	char				g_szCacheAddr[16];
extern	char				g_szCoreAddr[16];
extern	CGuildMng			g_GuildMng;
extern	CGuildWarMng		g_GuildWarMng;

extern	CWorldMng			g_WorldMng;
extern  DWORD				g_tmCurrent;
//extern	time_t	g_tCurrent;
extern	char				g_sHeartbeat[32];
extern	CPartyMng			g_PartyMng;

extern  BOOL LoadAIScript();		// aimonster2.cpp

void LogPerformance( DWORD dwCurTick );

#ifdef __ZCK_TICK_FIX
enum EZckTickBootstrapState
{
	ZCK_TICK_BOOT_WAIT_WORLD = 0,
	ZCK_TICK_BOOT_WORLD_LOADED,
	ZCK_TICK_BOOT_OBJECTS_FLUSHED,
	ZCK_TICK_BOOT_READY
};

static volatile LONG g_lZckTickBootstrapState = ZCK_TICK_BOOT_WAIT_WORLD;
static DWORD g_dwZckTickMaxPhaseMs = 0;
static DWORD g_dwZckTickMeasuredMs = 0;
static LPCTSTR g_lpszZckTickMaxPhase = _T( "None" );

void ZckTickFix_OnWorldLoadComplete()
{
	::InterlockedExchange( &g_lZckTickBootstrapState, ZCK_TICK_BOOT_WORLD_LOADED );
}

static LONG ZckTickFix_GetBootstrapState()
{
	return ::InterlockedCompareExchange( &g_lZckTickBootstrapState, 0, 0 );
}

static void ZckTickFix_BeginLoop()
{
	g_dwZckTickMaxPhaseMs = 0;
	g_dwZckTickMeasuredMs = 0;
	g_lpszZckTickMaxPhase = _T( "None" );
}

static void ZckTickFix_RecordPhase( LPCTSTR lpszPhase, DWORD dwStartTick )
{
	const DWORD dwElapsed = timeGetTime() - dwStartTick;
	g_dwZckTickMeasuredMs += dwElapsed;

	if( dwElapsed > g_dwZckTickMaxPhaseMs )
	{
		g_dwZckTickMaxPhaseMs = dwElapsed;
		g_lpszZckTickMaxPhase = lpszPhase;
	}
}

static int ZckTickFix_GetPendingAddObjCount()
{
	int nPending = 0;

	CWorld* pWorld = g_WorldMng.GetFirstActive();
	while( pWorld )
	{
		if( pWorld->m_cbAddObjs > 0 )
			nPending += pWorld->m_cbAddObjs;

		pWorld = pWorld->nextptr;
	}

	return nPending;
}
#endif // __ZCK_TICK_FIX

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CTimeout
////////////////////////////////////////////////////////////////////////////////////////////////////////

CTimeout::CTimeout( DWORD dwInterval, DWORD dwFirstTimeout )
{
	m_dwInterval		= dwInterval;
	m_endTime	= timeGetTime() + dwFirstTimeout;
}

DWORD CTimeout::Over( DWORD dwCurr )
{
	if( dwCurr > m_endTime )
		return ( dwCurr - m_endTime );
	return 0;
}
void CTimeout::Reset( DWORD dwCurr, DWORD dwTimeout )
{
	if( dwTimeout > m_dwInterval )
		m_endTime	= dwCurr;
	else
		m_endTime	= dwCurr + ( m_dwInterval - dwTimeout );
}
BOOL CTimeout::TimeoutReset( DWORD dwCurr )
{
	DWORD dwTimeout	= Over( dwCurr );
	if( dwTimeout > 0 )
	{
		Reset( dwCurr, dwTimeout );
		return TRUE; 
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogPerformance( DWORD dwCurTick )
{
	static int nFrame = 0;			
	static DWORD dwPrev = 0;
	static DWORD dwElapsed = 0;

#ifdef __ZCK_TICK_FIX
	const LONG lBootstrapState = ZckTickFix_GetBootstrapState();
	if( lBootstrapState != ZCK_TICK_BOOT_READY )
	{
		if( g_dwZckTickMaxPhaseMs >= 100 )
		{
			FLINFO_LOG( PROGRAM_NAME,
				_T( "[ZCK_TICK_FIX] Bootstrap loop - max phase:(%s), phase tick:(%u ms), measured:(%u ms), state:(%d)" ),
				g_lpszZckTickMaxPhase, g_dwZckTickMaxPhaseMs, g_dwZckTickMeasuredMs, lBootstrapState );
		}

		dwElapsed = 0;
		nFrame = 0;

		if( lBootstrapState == ZCK_TICK_BOOT_OBJECTS_FLUSHED )
		{
			::InterlockedExchange( &g_lZckTickBootstrapState, ZCK_TICK_BOOT_READY );
			FLINFO_LOG( PROGRAM_NAME,
				_T( "[ZCK_TICK_FIX] Bootstrap termine - surveillance des ticks gameplay active." ) );
		}
		dwPrev = timeGetTime();
		return;
	}
#endif // __ZCK_TICK_FIX

	++nFrame;
	DWORD dwTick = dwCurTick - dwPrev; 
	dwPrev = dwCurTick;
	dwElapsed += dwTick;

	if( dwTick > 1000 )
	{
		SetLogInfo( LOGTYPE_WARN1, "warning - tick:%d", dwTick );
#ifdef __ZCK_TICK_FIX
		const DWORD dwZckUnmeasuredMs =
			( dwTick > g_dwZckTickMeasuredMs ) ? ( dwTick - g_dwZckTickMeasuredMs ) : 0;

		FLWARNING_LOG( PROGRAM_NAME,
			_T( "[ZCK_TICK_FIX] warning - tick:(%u ms), max phase:(%s), phase tick:(%u ms), measured:(%u ms), unmeasured:(%u ms)" ),
			dwTick, g_lpszZckTickMaxPhase, g_dwZckTickMaxPhaseMs, g_dwZckTickMeasuredMs, dwZckUnmeasuredMs );
#else
		FLWARNING_LOG( PROGRAM_NAME, _T( "warning - tick:%u" ), dwTick );
#endif // __ZCK_TICK_FIX
	}

	if( dwElapsed > 1000 )
	{
//#ifdef _DEBUG
		//static int nHour = 0;
		//if( nHour != g_GameTimer.m_nHour )
		//{

		//	//OutputDebugStr( str );
		//	
		//}
		//TCHAR str[ 1024 ];
		////nHour	= g_GameTimer.m_nHour;
		//FLSPrintf( str, _countof( str ), _T("gametime : %04d %02d:%02d:%02d	\r\n"), g_GameTimer.m_nDay, g_GameTimer.m_nHour, g_GameTimer.m_nMin, g_GameTimer.m_nSec );
		//SetLogInfo( LOGTYPE_CCU, str );
//#else
		SetLogInfo( LOGTYPE_CCU, "CCU:%d", g_xWSUserManager->GetCount() );
//#endif //_DEBUG

		SetLogInfo( LOGTYPE_PERFOMANCE, "frame:%d tick:%d", nFrame, dwTick );
		SetLogInfo( LOGTYPE_REDRAW, "" );
		SetLogInfo( LOGTYPE_DBDISCONNECT, g_dpDBClient.GetAlive() ? "DbConnect" : "DbDisconnect" );
		SetLogInfo( LOGTYPE_COREDISCONNECT, g_DPCoreClient.GetAlive() ? "CoreConnect" : "CoreDisconnect" );

		dwElapsed = 0; // -= 1000
		nFrame = 0;
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CRunObject
////////////////////////////////////////////////////////////////////////////////////////////////////////
CRunObject::CRunObject()
{
	m_hRunObject	= (HANDLE)0;
	m_hClose	= (HANDLE)0;
}

CRunObject::~CRunObject()
{
	Close();
}

BOOL CRunObject::Init( void )
{
	if( LoadAIScript() == FALSE )
		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	// 시리얼 넘버 초기화
	if( CSerialNumber::GetInstance()->Init( CNetSerialNumberRequest::GetInstance() ) == false )
	{
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////

	if( !g_dpDBClient.Run( g_szDBAddr, PN_DBSRVR_1, TRUE ) )
	{
		FLTRACE_LOG( PROGRAM_NAME, _T( "Can't connect to database server." ) );
		return FALSE;
	}

// 
	m_hClose	= CreateEvent( NULL, FALSE, FALSE, NULL );
	DWORD dwThreadId;
	m_hRunObject	= chBEGINTHREADEX( NULL, 0, _Run, (LPVOID)this, 0, &dwThreadId ); 
//


	if( !g_DPSrvr.StartServer( g_szCacheBindIP, (u_short)( g_uKey + PN_WORLDSRVR ), FLNormalProtocol::GetInstance(), TRUE ) )
	{
		FLTRACE_LOG( PROGRAM_NAME, _T( "Can't start server." ) );
		return FALSE;
	}
	if( !g_DPCoreClient.Run( g_szCoreAddr, PN_CORESRVR + 0, g_uKey ) )
	{
		FLTRACE_LOG( PROGRAM_NAME, _T( "Can't connect to core server." ) );
		return FALSE;
	}

// 	if ( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) )
// 		g_dpDBClient.SendQueryGuildBank( g_uKey );
	if( g_eLocal.GetState( EVE_WORMON ) )
		g_dpDBClient.SendQueryGuildQuest();

	if( g_eLocal.GetState( EVE_RAINBOWRACE ) )
		g_dpDBClient.SendRainbowRaceReqLoad();

	g_dpDBClient.CalluspXXXMultiServer( g_uIdofMulti, NULL );
	if( !CXMasEvent::GetInstance()->LoadScript( "spevent.txt" ) )
	{
		FLTRACE_LOG( PROGRAM_NAME, _T( "can't read spevent.txt" ) );
		return FALSE;
	}
	if( g_eLocal.GetState( EVE_SCHOOL ) )
	{
#ifdef __IDC
		if( !CEveSchool::GetInstance()->LoadPos( "..\\script\\school.txt" ) )	//
#else	// __IDC
		if( !CEveSchool::GetInstance()->LoadPos( "school.txt" ) )
#endif	// __IDC
		{
			FLTRACE_LOG( PROGRAM_NAME, _T( "school.txt not found" ) );
			return FALSE;
		}
	}
	if( g_eLocal.GetState( EVE_GUILDCOMBAT ) && !g_GuildCombatMng.LoadScript( "GuildCombat.txt" ) )
	{
		FLTRACE_LOG( PROGRAM_NAME, _T( "GuildCombat.txt not found" ) );
		return FALSE;
	}

	return TRUE;
}

void CRunObject::Close( void )
{
	CLOSE_THREAD( m_hRunObject, m_hClose );
}

u_int CRunObject::_Run( LPVOID pParam )
{
	CRunObject* pRunObject	= (CRunObject*)pParam;
	pRunObject->Run();
	return 0;
}

void CRunObject::Run( void )
{
	FLDumpInstaller::InstallThreadExceptionHandler();

	HANDLE hHeartbeat = CreateEvent( NULL, FALSE, FALSE, g_sHeartbeat );
	if( !( hHeartbeat ) )
		hHeartbeat	= OpenEvent( EVENT_MODIFY_STATE, FALSE, g_sHeartbeat );
	if( NULL == hHeartbeat )
		FLERROR_LOG( PROGRAM_NAME, _T( "MAPI_E_NOT_ENOUGH_RESOURCES" ) );
	DWORD dwTickHearbeat = timeGetTime() + SEC( 2 );

	CTimeout	timeoutObject( 67, 0 );
	CTimeout	timeoutReadAgent( 67, 20 );
	CTimeout	timeoutReadCore( 67, 40 );
	CTimeout	timeoutReadTrans( 67, 50 );
	CTimeout	timeoutReadAccount( 67, 60 );
	CTimeout	timeoutRespawn( 1000, 200 );
	CTimeout	timeout( 1000, 600 );
	CTimeout	timeoutCallTheRoll( MIN( 1 ), 70 );

	while( WaitForSingleObject( m_hClose, 1 ) != WAIT_OBJECT_0 )
	{
#ifdef __ZCK_TICK_FIX
		ZckTickFix_BeginLoop();
#endif // __ZCK_TICK_FIX
		{
			g_tmCurrent		= timeGetTime();
			g_tCurrent	= time( NULL );

			if( g_tmCurrent > dwTickHearbeat )
			{
				if( SetEvent( hHeartbeat ) == FALSE )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "Heartbeat SetEvent failed, GetLastError: %d" ), ::GetLastError() );
				}

				dwTickHearbeat = g_tmCurrent + SEC( 2 );
			}

			if( timeoutObject.TimeoutReset( g_tmCurrent ) )
			{
#ifdef __ZCK_TICK_FIX
				const LONG lZckStateBeforeWorldProcess = ZckTickFix_GetBootstrapState();
				const int nZckPendingBefore =
					( lZckStateBeforeWorldProcess == ZCK_TICK_BOOT_WORLD_LOADED )
					? ZckTickFix_GetPendingAddObjCount()
					: -1;
				const DWORD dwZckPhaseStart = timeGetTime();
#endif // __ZCK_TICK_FIX

				g_WorldMng.Process();

#ifdef __ZCK_TICK_FIX
				ZckTickFix_RecordPhase( _T( "WorldMng.Process" ), dwZckPhaseStart );

				if( lZckStateBeforeWorldProcess == ZCK_TICK_BOOT_WORLD_LOADED )
				{
					const int nZckPendingAfter = ZckTickFix_GetPendingAddObjCount();

					FLINFO_LOG( PROGRAM_NAME,
						_T( "[ZCK_TICK_FIX] Bootstrap WorldMng.Process - tick:(%u ms), pending before:(%d), pending after:(%d)" ),
						timeGetTime() - dwZckPhaseStart, nZckPendingBefore, nZckPendingAfter );

					if( nZckPendingAfter == 0 )
						::InterlockedExchange( &g_lZckTickBootstrapState, ZCK_TICK_BOOT_OBJECTS_FLUSHED );
				}
#endif // __ZCK_TICK_FIX
			}

			if( timeoutReadAgent.TimeoutReset( g_tmCurrent ) )
			{
#ifdef __ZCK_TICK_FIX
				const DWORD dwZckPhaseStart = timeGetTime();
#endif // __ZCK_TICK_FIX
				g_DPSrvr.ReceiveMessage();
#ifdef __ZCK_TICK_FIX
				ZckTickFix_RecordPhase( _T( "DPSrvr.ReceiveMessage" ), dwZckPhaseStart );
#endif // __ZCK_TICK_FIX
			}

			if( timeoutReadCore.TimeoutReset( g_tmCurrent ) )
			{
#ifdef __ZCK_TICK_FIX
				const DWORD dwZckPhaseStart = timeGetTime();
#endif // __ZCK_TICK_FIX
				g_DPCoreClient.ReceiveMessage();
#ifdef __ZCK_TICK_FIX
				ZckTickFix_RecordPhase( _T( "DPCoreClient.ReceiveMessage" ), dwZckPhaseStart );
#endif // __ZCK_TICK_FIX
			}

			if( timeoutReadTrans.TimeoutReset( g_tmCurrent ) )
			{
#ifdef __ZCK_TICK_FIX
				const DWORD dwZckPhaseStart = timeGetTime();
#endif // __ZCK_TICK_FIX
				g_dpDBClient.ReceiveMessage();
#ifdef __ZCK_TICK_FIX
				ZckTickFix_RecordPhase( _T( "DPDatabaseClient.ReceiveMessage" ), dwZckPhaseStart );
#endif // __ZCK_TICK_FIX
			}
		
		#ifdef __EVENTLUA_KEEPCONNECT
#ifdef __ZCK_TICK_FIX
			if( timeoutReadAccount.TimeoutReset( g_tmCurrent ) )
			{
				const DWORD dwZckPhaseStart = timeGetTime();
				prj.m_EventLua.KeepConnectEventProcess();
				ZckTickFix_RecordPhase( _T( "EventLua.KeepConnect" ), dwZckPhaseStart );
			}
#else
			if( timeoutReadAccount.TimeoutReset( g_tmCurrent ) )
				prj.m_EventLua.KeepConnectEventProcess();
#endif // __ZCK_TICK_FIX
		#endif // __EVENTLUA_KEEPCONNECT

			if( timeoutRespawn.TimeoutReset( g_tmCurrent ) )
			{
#ifdef __ZCK_TICK_FIX
				const DWORD dwZckPhaseStart = timeGetTime();
#endif // __ZCK_TICK_FIX
				u_long uRespawned = g_WorldMng.Respawn();
//				if( uRespawned > 0 )
					SetLogInfo( LOGTYPE_RESPAWN, "Respawn:%d Object:%d", uRespawned, g_WorldMng.GetObjCount() );
#ifdef __ZCK_TICK_FIX
				ZckTickFix_RecordPhase( _T( "WorldMng.Respawn" ), dwZckPhaseStart );
#endif // __ZCK_TICK_FIX
			}

			if( timeout.TimeoutReset( g_tmCurrent ) )
			{
#ifdef __ZCK_TICK_FIX
				const DWORD dwZckPhaseStart = timeGetTime();
#endif // __ZCK_TICK_FIX
				g_GameTimer.Compute();
				CGuildQuestProcessor::GetInstance()->Process();
				CPartyQuestProcessor::GetInstance()->Process();

				if( g_eLocal.GetState( EVE_GUILDCOMBAT ) )
					g_GuildCombatMng.Process();

				if( g_eLocal.GetState( EVE_GUILDCOMBAT1TO1 ) )
					g_GuildCombat1to1Mng.Process();

				if( g_eLocal.GetState( EVE_SECRETROOM ) )
				{
					CSecretRoomMng::GetInstance()->Process();
				}

				if( g_eLocal.GetState( EVE_RAINBOWRACE ) )
				{
					CRainbowRaceMng::GetInstance()->Process();
				}
#ifdef __ZCK_WORLD_BOSS
				CWorldBossManager::GetInstance()->Process();
#endif // __ZCK_WORLD_BOSS
#ifdef __ZCK_INVASIONS
				CZckInvasions::GetInstance()->Process();
#endif // __ZCK_INVASIONS
#ifdef __ZCK_RECORD
				CZckRecordManager::GetInstance()->Process();
				g_xWSUserManager->ProcessZckRecordStats();
#endif // __ZCK_RECORD

				{
					CPCBang::GetInstance()->ProcessPCBang();
				}

				{
					CInstanceDungeonParty::GetInstance()->Process();
				}

				{
					if( CQuiz::GetInstance()->IsAutoMode() && CQuiz::GetInstance()->IsRun() )
						CQuiz::GetInstance()->Process();
				}

				if( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) )
					g_GuildMng.Process();

				if( g_eLocal.GetState( EVE_GUILDWAR ) )
					g_GuildWarMng.Process();

//				g_PartyMng.PartyMapInfo();

				// ¡¤¡I￠￥y AI¨￢¡I¨¡￠c ￠￢o¨o¨￢AI AE￠￥c 1E￠￢ ¨¡¨o
				CRangdaController::Instance()->OnTimer();

	#ifdef __EVENTLUA_SPAWN
				prj.m_EventLua.EventSpawnProcess();
	#endif // __EVENTLUA_SPAWN
#ifdef __ZCK_TICK_FIX
				ZckTickFix_RecordPhase( _T( "OneSecondSystems" ), dwZckPhaseStart );
#endif // __ZCK_TICK_FIX
			}

			if( timeoutCallTheRoll.TimeoutReset( g_tmCurrent ) )	// 1¨￢¨￠￠￥c 
			{
#ifdef __ZCK_TICK_FIX
				const DWORD dwZckPhaseStart = timeGetTime();
#endif // __ZCK_TICK_FIX
				CEventGeneric::GetInstance()->CallTheRoll();
				CCreateMonster::GetInstance()->ProcessRemoveMonster();

				using namespace nsFlyffPieceDailyPayment;
				g_pFlyffPieceDailyPayment->Process( g_tCurrent );
#ifdef __ZCK_TICK_FIX
				ZckTickFix_RecordPhase( _T( "MinuteSystems" ), dwZckPhaseStart );
#endif // __ZCK_TICK_FIX
			}

#ifdef __ZCK_TICK_FIX
			const DWORD dwZckAlwaysPhaseStart = timeGetTime();
#endif // __ZCK_TICK_FIX
#ifdef __EVENT_0117
			CEventGeneric::GetInstance()->Spawn();
#endif	// __EVENT_0117

			//neodeath ¨uE￠O¡A
			//DATETIME_ALARMCLOCK().Update( DATE_TIMER().GetCurrentTime() );
			PERIOD_REPETITION_ALARM_MNG().Update();

//			//Cu￥i￠?¡¾a¨￢I
			if( _GetContentState( CT_DONATION ) == CS_VER1 )
				COOPERATIVE_CONTRIBUTIONS().TryClean_ExpiredGlobalReward();				//￠￢￠￢¡¤a A¨uA¨I

			if( g_pEventArenaGlobal->IsArenaChannel() )
			{
				g_pEventArena->Run();
			}


			g_PartyMng.PartyMapInfo();


			//if( _GetContentState( CT_SHUTDOWN_RULE ) == CS_VER1 )
			if( g_xFlyffConfig->GetMainLanguage() == LANG_KOR )
				FLShutdownRule_Mng::GetInstance().TryShutdown();
#ifdef __ZCK_TICK_FIX
			ZckTickFix_RecordPhase( _T( "AlwaysSystems" ), dwZckAlwaysPhaseStart );
#endif // __ZCK_TICK_FIX
		}
		
		LogPerformance( timeGetTime() );
	
	}
	CLOSE_HANDLE( m_hClose );
	CLOSE_HANDLE( hHeartbeat );
}

CRunObject*	CRunObject::GetInstance( void )
{
	static CRunObject	sRunObject;
	return &sRunObject;
}

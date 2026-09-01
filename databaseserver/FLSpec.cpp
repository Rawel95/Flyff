#include "StdAfx.h"
#include "FLSpec.h"

#include "../_Common/file.h"
#include <MsgHdr.h>

#include "dptrans.h"


FLSpec::FLSpec()
: m_pCooperativeContributions_Buffer( NULL )
, m_nCooperativeContributions_BufferSize( 0 )
{

}

FLSpec::~FLSpec()
{
	SAFE_DELETE( m_pCooperativeContributions_Buffer );
}

FLSpec & FLSpec::GetInstance()
{
	static FLSpec inst;
	return inst;
}
#ifdef __ZCK_COOPERATIVE_CONTRIBUTIONS_FIX
BOOL FLSpec::Load_CooperativeContributions()
{
	if( _GetContentState( CT_DONATION ) != CS_VER1 )
		return FALSE;

	const char * pszFileName= "Event/propCooperativeContributions_Event.inc";

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), pszFileName );
	WIN32_FILE_ATTRIBUTE_DATA kFileData;
	ZeroMemory( &kFileData, sizeof( kFileData ) );

	if( ::GetFileAttributesEx( szFullPath, GetFileExInfoStandard, &kFileData ) == FALSE )
	{
		const DWORD dwError = ::GetLastError();
		FLERROR_LOG( PROGRAM_NAME,
			_T( "[ COOPERATIVE CONTRIBUTIONS SCRIPT NOT FOUND. FILE_NAME:(%s), ERROR_CODE:(%u) ]" ),
			szFullPath, dwError );
		return FALSE;
	}

	if( ( kFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
	{
		FLERROR_LOG( PROGRAM_NAME,
			_T( "[ COOPERATIVE CONTRIBUTIONS SCRIPT PATH IS A DIRECTORY. FILE_NAME:(%s) ]" ),
			szFullPath );
		return FALSE;
	}

	ULARGE_INTEGER ullFileSize;
	ullFileSize.HighPart = kFileData.nFileSizeHigh;
	ullFileSize.LowPart  = kFileData.nFileSizeLow;

	if( ullFileSize.QuadPart == 0 )
	{
		FLINFO_LOG( PROGRAM_NAME,
			_T( "[ EMPTY COOPERATIVE CONTRIBUTIONS SCRIPT - NO EVENT CONFIGURED. FILE_NAME:(%s) ]" ),
			szFullPath );
		return TRUE;
	}

	CFileIO kFile;
	//const CString kFileName = pszFileName;
	//kFileName.MakeLower();

	if( kFile.Open( szFullPath, "rb" ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME,
			_T( "[ FAILED OPEN NON-EMPTY COOPERATIVE CONTRIBUTIONS SCRIPT. FILE_NAME:(%s) ]" ),
			szFullPath );
		return FALSE;
	}

	if( m_pCooperativeContributions_Buffer != NULL || m_nCooperativeContributions_BufferSize != 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ ALREADY LOAD (%s) ]" ), szFullPath );
		return FALSE;
	}

	m_nCooperativeContributions_BufferSize	= kFile.GetLength();
	m_pCooperativeContributions_Buffer		= new BYTE[ m_nCooperativeContributions_BufferSize ];

	kFile.Read( m_pCooperativeContributions_Buffer, m_nCooperativeContributions_BufferSize );
	return TRUE;
}


void FLSpec::TrySend_CooperativeContributions( const DPID dpid )
{
	//OnBaseGameSettingReq ????
	if( _GetContentState( CT_DONATION ) != CS_VER1 )
		return;

	if( m_pCooperativeContributions_Buffer == NULL || m_nCooperativeContributions_BufferSize <= 0 )
		return;

	BEFORESENDDUAL( out, PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_SPEC_NOTI, DPID_UNKNOWN, DPID_UNKNOWN );
	out << m_nCooperativeContributions_BufferSize;
	out.Write( m_pCooperativeContributions_Buffer, m_nCooperativeContributions_BufferSize );
	SEND( out, CDPTrans::GetInstance(), dpid );
}
#else
BOOL FLSpec::Load_CooperativeContributions()
{
	if( _GetContentState( CT_DONATION ) != CS_VER1 )
		return FALSE;

	const char * pszFileName= "Event/propCooperativeContributions_Event.inc";

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), pszFileName );

	CFileIO kFile;
	//const CString kFileName = pszFileName;
	//kFileName.MakeLower();
	if( kFile.Open( szFullPath, "rb" ) == FALSE 
		|| kFile.GetLength() == 0 ) 
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		return FALSE;
	}

	if( m_pCooperativeContributions_Buffer != NULL || m_nCooperativeContributions_BufferSize != 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ ALREADY LOAD (%s) ]" ), szFullPath );
		return FALSE;
	}

	m_nCooperativeContributions_BufferSize	= kFile.GetLength();
	m_pCooperativeContributions_Buffer		= new BYTE[ m_nCooperativeContributions_BufferSize ];

	kFile.Read( m_pCooperativeContributions_Buffer, m_nCooperativeContributions_BufferSize );
	return TRUE;
}

void FLSpec::TrySend_CooperativeContributions( const DPID dpid )
{
	//OnBaseGameSettingReq ????
	if( _GetContentState( CT_DONATION ) != CS_VER1 )
		return;

	if( m_pCooperativeContributions_Buffer == NULL || m_nCooperativeContributions_BufferSize <= 0 )
		return;

	BEFORESENDDUAL( out, PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_SPEC_NOTI, DPID_UNKNOWN, DPID_UNKNOWN );
	out << m_nCooperativeContributions_BufferSize;
	out.Write( m_pCooperativeContributions_Buffer, m_nCooperativeContributions_BufferSize );
	SEND( out, CDPTrans::GetInstance(), dpid );
}
#endif
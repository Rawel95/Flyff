#include "stdafx.h"
#include "WndZckRecord.h"

#ifdef __ZCK_RECORD

#include "../Neuz/DPClient.h"
#include "WndManager.h"
#include "ResData.h"

extern CDPClient g_DPlay;

namespace
{
    const DWORD ZCK_RECORD_CLIENT_REFRESH = SEC( 15 );
    const int ZCK_RECORD_ROW_HEIGHT = 24;

    const UINT ZCK_RECORD_CATEGORY_CONTROL_IDS[ZCK_RECORD_COUNT] =
    {
        WIDC_ZCK_RECORD_STR,
        WIDC_ZCK_RECORD_DEX,
        WIDC_ZCK_RECORD_STA,
        WIDC_ZCK_RECORD_INT,
        WIDC_ZCK_RECORD_DPS,
        WIDC_ZCK_RECORD_DUNGEON
    };

    const UINT ZCK_RECORD_COLUMN_CONTROL_IDS[] =
    {
        WIDC_ZCK_RECORD_LIST_RANK,
        WIDC_ZCK_RECORD_LIST_NAME,
        WIDC_ZCK_RECORD_LIST_LEVEL,
        WIDC_ZCK_RECORD_LIST_JOB,
        WIDC_ZCK_RECORD_LIST_VALUE
    };
}

CZckRecordClientData::CZckRecordClientData()
    : m_dwVersion( 0 )
    , m_nWeekStart( 0 )
{
}

CZckRecordClientData* CZckRecordClientData::GetInstance()
{
    static CZckRecordClientData s_data;
    return &s_data;
}

void CZckRecordClientData::Deserialize( CAr& ar )
{
    DWORD dwVersion = 0;
    __int64 nWeekStart = 0;
    ar >> dwVersion >> nWeekStart;

    std::vector<ZCK_RECORD_ENTRY> rankings[ZCK_RECORD_COUNT];
    for( int nType = 0; nType < ZCK_RECORD_COUNT; ++nType )
    {
        DWORD dwWireCount = 0;
        ar >> dwWireCount;

        const DWORD dwReserve = ( dwWireCount < static_cast<DWORD>( ZCK_RECORD_TOP_COUNT ) ) ? dwWireCount : static_cast<DWORD>( ZCK_RECORD_TOP_COUNT );
        rankings[nType].reserve( dwReserve );
        for( DWORD i = 0; i < dwWireCount; ++i )
        {
            ZCK_RECORD_ENTRY entry;
            ar >> entry.dwPlayerId;
            ar.Read( entry.szName, sizeof( entry.szName ) );
            entry.szName[MAX_NAME - 1] = '\0';
            ar >> entry.nLevel >> entry.nJob >> entry.nValue;
            ar >> entry.dwDungeonWorldId >> entry.dwDungeonLevel;
            if( i < ZCK_RECORD_TOP_COUNT )
                rankings[nType].push_back( entry );
        }
    }

    m_dwVersion = dwVersion;
    m_nWeekStart = nWeekStart;
    for( int nType = 0; nType < ZCK_RECORD_COUNT; ++nType )
        m_rankings[nType].swap( rankings[nType] );
}

CWndZckRecord::CWndZckRecord()
    : m_type( ZCK_RECORD_STR )
    , m_dwLastRequestTick( 0 )
    , m_dwDisplayedVersion( 0 )
{
    for( int i = 0; i < ZCK_RECORD_COUNT; ++i )
        m_pCategoryButtons[i] = NULL;

    for( int i = 0; i < ZCK_RECORD_COLUMN_COUNT; ++i )
        m_pColumnLists[i] = NULL;
}

CWndZckRecord::~CWndZckRecord()
{
}

BOOL CWndZckRecord::Initialize( CWndBase* pWndParent, DWORD )
{
    return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_ZCK_RECORD, 0, CPoint( 0, 0 ), pWndParent );
}

void CWndZckRecord::OnInitialUpdate()
{
    CWndNeuz::OnInitialUpdate();

    bool bMissingControl = false;

    for( int i = 0; i < ZCK_RECORD_COUNT; ++i )
    {
        m_pCategoryButtons[i] = static_cast<CWndButton*>( GetDlgItem( ZCK_RECORD_CATEGORY_CONTROL_IDS[i] ) );
        if( m_pCategoryButtons[i] == NULL )
            bMissingControl = true;
    }

    for( int i = 0; i < ZCK_RECORD_COLUMN_COUNT; ++i )
    {
        m_pColumnLists[i] = static_cast<CWndListBox*>( GetDlgItem( ZCK_RECORD_COLUMN_CONTROL_IDS[i] ) );
        if( m_pColumnLists[i] == NULL )
        {
            bMissingControl = true;
            continue;
        }

        m_pColumnLists[i]->m_nLineSpace = 0;
        m_pColumnLists[i]->m_nLineSpace = max( 0, ( ZCK_RECORD_ROW_HEIGHT - m_pColumnLists[i]->GetFontHeight() ) / 2 );
        m_pColumnLists[i]->SetOnSelectColor( D3DCOLOR_ARGB( 255, 45, 107, 180 ) );
        m_pColumnLists[i]->SetOnMouseColor( D3DCOLOR_ARGB( 255, 45, 107, 180 ) );
    }

    if( bMissingControl )
        FLERROR_LOG( PROGRAM_NAME, _T( "[ZCK_RECORD] CWndZckRecord::OnInitialUpdate : controle ResData manquant." ) );

    UpdateCategoryButtons();
    UpdateList();
    RequestRefresh( true );
    MoveParentCenter();
}

BOOL CWndZckRecord::Process()
{
    RequestRefresh( false );

    if( CZckRecordClientData::GetInstance()->GetVersion() != m_dwDisplayedVersion )
        UpdateList();

    return CWndBase::Process();
}

void CWndZckRecord::RequestRefresh( bool bForce )
{
    if( g_pPlayer == NULL )
        return;

    const DWORD dwNow = timeGetTime();
    if( bForce == false && m_dwLastRequestTick != 0 && static_cast<DWORD>( dwNow - m_dwLastRequestTick ) < ZCK_RECORD_CLIENT_REFRESH )
        return;

    g_DPlay.SendHdr( PACKETTYPE_ZCK_RECORD_REQUEST );
    m_dwLastRequestTick = dwNow;
}

void CWndZckRecord::UpdateCategoryButtons()
{
    for( int i = 0; i < ZCK_RECORD_COUNT; ++i )
    {
        if( m_pCategoryButtons[i] != NULL )
            m_pCategoryButtons[i]->SetCheck( i == static_cast<int>( m_type ) ? TRUE : FALSE );
    }
}

void CWndZckRecord::UpdateList()
{
    for( int i = 0; i < ZCK_RECORD_COLUMN_COUNT; ++i )
    {
        if( m_pColumnLists[i] == NULL )
            return;
    }

    for( int i = 0; i < ZCK_RECORD_COLUMN_COUNT; ++i )
        m_pColumnLists[i]->ResetContent();

    const std::vector<ZCK_RECORD_ENTRY>& ranking = CZckRecordClientData::GetInstance()->GetRanking( m_type );
    if( ranking.empty() )
    {
        m_pColumnLists[ZCK_RECORD_COLUMN_RANK]->AddString( _T( "-" ) );
        m_pColumnLists[ZCK_RECORD_COLUMN_NAME]->AddString( _T( "Kein Rekord für diese Woche." ) );
        m_pColumnLists[ZCK_RECORD_COLUMN_LEVEL]->AddString( _T( "-" ) );
        m_pColumnLists[ZCK_RECORD_COLUMN_JOB]->AddString( _T( "-" ) );
        m_pColumnLists[ZCK_RECORD_COLUMN_VALUE]->AddString( _T( "-" ) );
        m_dwDisplayedVersion = CZckRecordClientData::GetInstance()->GetVersion();
        return;
    }

    const size_t nCount = min( ranking.size(), static_cast<size_t>( ZCK_RECORD_TOP_COUNT ) );
    for( size_t i = 0; i < nCount; ++i )
    {
        CString strRank;
        strRank.Format( _T( "%u" ), static_cast<unsigned int>( i + 1 ) );
        m_pColumnLists[ZCK_RECORD_COLUMN_RANK]->AddString( strRank, D3DCOLOR_ARGB( 255, 255, 215, 110 ), 0 );

        m_pColumnLists[ZCK_RECORD_COLUMN_NAME]->AddString( ranking[i].szName );

        CString strLevel;
        strLevel.Format( _T( "%d" ), ranking[i].nLevel );
        m_pColumnLists[ZCK_RECORD_COLUMN_LEVEL]->AddString( strLevel );

        LPCTSTR pszJob = _T( "-" );
        if( ranking[i].nJob >= 0 && ranking[i].nJob < MAX_JOB )
            pszJob = prj.m_aJob[ranking[i].nJob].szName;
        m_pColumnLists[ZCK_RECORD_COLUMN_JOB]->AddString( pszJob );

        const CString strValue = FormatValue( ranking[i] );
        m_pColumnLists[ZCK_RECORD_COLUMN_VALUE]->AddString( strValue, D3DCOLOR_ARGB( 255, 46, 112, 169 ), 0 );
    }

    m_dwDisplayedVersion = CZckRecordClientData::GetInstance()->GetVersion();
}

CString CWndZckRecord::FormatValue( const ZCK_RECORD_ENTRY& entry ) const
{
    CString strValue;
    if( m_type == ZCK_RECORD_DUNGEON )
    {
        const DWORD dwMilliseconds = static_cast<DWORD>( entry.nValue );
        const DWORD dwMinutes = dwMilliseconds / 60000;
        const DWORD dwSeconds = ( dwMilliseconds / 1000 ) % 60;
        const DWORD dwMillis = dwMilliseconds % 1000;
        strValue.Format( _T( "%02u:%02u.%03u  [Monde %u / Niv. %u]" ),
            dwMinutes, dwSeconds, dwMillis, entry.dwDungeonWorldId, entry.dwDungeonLevel );
    }
    else if( m_type == ZCK_RECORD_DPS )
    {
        strValue.Format( _T( "%I64d /s" ), entry.nValue );
    }
    else
    {
        strValue.Format( _T( "%I64d" ), entry.nValue );
    }
    return strValue;
}

BOOL CWndZckRecord::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
    return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

BOOL CWndZckRecord::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
    if( message == WNM_CLICKED )
    {
        for( int i = 0; i < ZCK_RECORD_COUNT; ++i )
        {
            if( nID == ZCK_RECORD_CATEGORY_CONTROL_IDS[i] )
            {
                m_type = static_cast<ZCK_RECORD_TYPE>( i );
                UpdateCategoryButtons();
                UpdateList();
                return TRUE;
            }
        }

        if( nID == WIDC_ZCK_RECORD_REFRESH )
        {
            RequestRefresh( true );
            return TRUE;
        }
    }

    return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

#endif // __ZCK_RECORD
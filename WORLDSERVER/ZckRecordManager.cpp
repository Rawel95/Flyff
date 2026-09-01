#include "stdafx.h"
#include "ZckRecordManager.h"

#ifdef __ZCK_RECORD

#include "User.h"
#include <algorithm>
#include <fstream>
#include <limits>

namespace
{
    const char ZCK_RECORD_FILE_MAGIC[8] = { 'Z', 'C', 'K', 'R', '2', '1', '0', '1' };
    const DWORD ZCK_RECORD_FILE_VERSION = 1;
    const DWORD ZCK_RECORD_SAVE_INTERVAL = SEC( 10 );
    const DWORD ZCK_RECORD_DPS_TIMEOUT = SEC( 5 );
    const DWORD ZCK_RECORD_MAX_DISK_PLAYERS = 100000;

    template <typename T>
    bool ZckWriteValue( std::ofstream& file, const T& value )
    {
        file.write( reinterpret_cast<const char*>( &value ), sizeof( value ) );
        return file.good();
    }

    template <typename T>
    bool ZckReadValue( std::ifstream& file, T& value )
    {
        file.read( reinterpret_cast<char*>( &value ), sizeof( value ) );
        return file.good();
    }
}

CZckRecordManager::PLAYER_RECORD::PLAYER_RECORD()
    : dwPlayerId( 0 )
    , nLevel( 0 )
    , nJob( 0 )
    , nBestDps( 0 )
    , dwBestDungeonTime( 0 )
    , dwDungeonWorldId( 0 )
    , dwDungeonLevel( 0 )
{
    memset( szName, 0, sizeof( szName ) );
    memset( nStats, 0, sizeof( nStats ) );
}

CZckRecordManager::DPS_SESSION::DPS_SESSION()
    : objTarget( NULL_ID )
    , nTotalDamage( 0 )
    , dwStartTick( 0 )
    , dwLastHitTick( 0 )
{
}

CZckRecordManager::CZckRecordManager()
    : m_nWeekStart( 0 )
    , m_dwVersion( 1 )
    , m_dwLastSaveTick( 0 )
    , m_bLoaded( false )
    , m_bDirty( false )
{
}

CZckRecordManager::~CZckRecordManager()
{
    if( m_bLoaded && m_bDirty )
        Save();
}

CZckRecordManager* CZckRecordManager::GetInstance()
{
    static CZckRecordManager s_manager;
    return &s_manager;
}

void CZckRecordManager::EnsureLoaded()
{
    if( m_bLoaded )
        return;

    if( Load() == false )
    {
        m_records.clear();
        m_dpsSessions.clear();
        m_nWeekStart = GetCurrentWeekStart();
        m_bLoaded = true;
        m_bDirty = false;
        FLERROR_LOG( PROGRAM_NAME, _T( "[ZCK_RECORD] Chargement impossible. Le manager demarre vide sans modifier le fichier existant." ) );
    }
}

__int64 CZckRecordManager::GetCurrentWeekStart() const
{
    const CTime now = CTime::GetCurrentTime();
    const int nDaysSinceMonday = ( now.GetDayOfWeek() + 5 ) % 7;
    const CTime midnight( now.GetYear(), now.GetMonth(), now.GetDay(), 0, 0, 0 );
    const CTime monday = midnight - CTimeSpan( nDaysSinceMonday, 0, 0, 0 );
    return static_cast<__int64>( monday.GetTime() );
}

std::string CZckRecordManager::GetDataFilePath() const
{
    char szPath[MAX_PATH] = { 0 };
    if( GetModuleFileNameA( NULL, szPath, _countof( szPath ) ) == 0 )
        return ".ZckRecordBook.dat";

    char* pSlash = strrchr( szPath, '\\' );
    if( pSlash != NULL )
        *( pSlash + 1 ) = '\0';
    else
        szPath[0] = '\0';

    return std::string( szPath ) + ".ZckRecordBook.dat";
}

bool CZckRecordManager::CheckWeeklyReset()
{
    const __int64 nCurrentWeekStart = GetCurrentWeekStart();
    if( m_nWeekStart == nCurrentWeekStart )
        return false;

    m_records.clear();
    m_dpsSessions.clear();
    m_nWeekStart = nCurrentWeekStart;
    MarkDirty();
    return true;
}

void CZckRecordManager::MarkDirty()
{
    m_bDirty = true;
    ++m_dwVersion;
    if( m_dwVersion == 0 )
        m_dwVersion = 1;
}

bool CZckRecordManager::UpdateIdentity( PLAYER_RECORD& record, FLWSUser* pUser )
{
    if( IsValidObj( pUser ) == FALSE )
        return false;

    bool bChanged = false;
    if( record.dwPlayerId != pUser->m_idPlayer )
    {
        record.dwPlayerId = pUser->m_idPlayer;
        bChanged = true;
    }

    if( strcmp( record.szName, pUser->GetName() ) != 0 )
    {
        FLStrcpy( record.szName, _countof( record.szName ), pUser->GetName() );
        bChanged = true;
    }

    if( record.nLevel != pUser->GetLevel() )
    {
        record.nLevel = pUser->GetLevel();
        bChanged = true;
    }

    if( record.nJob != pUser->GetJob() )
    {
        record.nJob = pUser->GetJob();
        bChanged = true;
    }

    return bChanged;
}

CZckRecordManager::PLAYER_RECORD& CZckRecordManager::GetOrCreateRecord( FLWSUser* pUser )
{
    return m_records[pUser->m_idPlayer];
}

bool CZckRecordManager::Load()
{
    const std::string strPath = GetDataFilePath();
    const __int64 nCurrentWeekStart = GetCurrentWeekStart();

    std::ifstream file( strPath.c_str(), std::ios::in | std::ios::binary );
    if( file.is_open() == false )
    {
        m_records.clear();
        m_dpsSessions.clear();
        m_nWeekStart = nCurrentWeekStart;
        m_bLoaded = true;
        m_bDirty = false;
        ++m_dwVersion;
        return true;
    }

    char szMagic[8] = { 0 };
    DWORD dwFileVersion = 0;
    __int64 nFileWeekStart = 0;
    DWORD dwCount = 0;

    file.read( szMagic, sizeof( szMagic ) );
    if( file.good() == false || memcmp( szMagic, ZCK_RECORD_FILE_MAGIC, sizeof( szMagic ) ) != 0 )
        return false;
    if( ZckReadValue( file, dwFileVersion ) == false || dwFileVersion != ZCK_RECORD_FILE_VERSION )
        return false;
    if( ZckReadValue( file, nFileWeekStart ) == false )
        return false;
    if( ZckReadValue( file, dwCount ) == false || dwCount > ZCK_RECORD_MAX_DISK_PLAYERS )
        return false;

    RECORD_MAP loadedRecords;
    for( DWORD i = 0; i < dwCount; ++i )
    {
        PLAYER_RECORD record;
        if( ZckReadValue( file, record.dwPlayerId ) == false ) return false;
        file.read( record.szName, sizeof( record.szName ) );
        if( file.good() == false ) return false;
        record.szName[MAX_NAME - 1] = '\0';
        if( ZckReadValue( file, record.nLevel ) == false ) return false;
        if( ZckReadValue( file, record.nJob ) == false ) return false;
        for( int nStat = 0; nStat < 4; ++nStat )
            if( ZckReadValue( file, record.nStats[nStat] ) == false ) return false;
        if( ZckReadValue( file, record.nBestDps ) == false ) return false;
        if( ZckReadValue( file, record.dwBestDungeonTime ) == false ) return false;
        if( ZckReadValue( file, record.dwDungeonWorldId ) == false ) return false;
        if( ZckReadValue( file, record.dwDungeonLevel ) == false ) return false;

        if( record.dwPlayerId != 0 )
            loadedRecords[record.dwPlayerId] = record;
    }

    m_dpsSessions.clear();
    m_bLoaded = true;
    m_bDirty = false;
    ++m_dwVersion;

    if( nFileWeekStart != nCurrentWeekStart )
    {
        m_records.clear();
        m_nWeekStart = nCurrentWeekStart;
        MarkDirty();
        return Save();
    }

    m_records.swap( loadedRecords );
    m_nWeekStart = nFileWeekStart;
    return true;
}

bool CZckRecordManager::Save()
{
    if( m_bLoaded == false )
        return false;

    const std::string strPath = GetDataFilePath();
    const std::string strTempPath = strPath + ".tmp";

    std::ofstream file( strTempPath.c_str(), std::ios::out | std::ios::binary | std::ios::trunc );
    if( file.is_open() == false )
        return false;

    file.write( ZCK_RECORD_FILE_MAGIC, sizeof( ZCK_RECORD_FILE_MAGIC ) );
    if( ZckWriteValue( file, ZCK_RECORD_FILE_VERSION ) == false ) return false;
    if( ZckWriteValue( file, m_nWeekStart ) == false ) return false;

    if( m_records.size() > ZCK_RECORD_MAX_DISK_PLAYERS )
    {
        file.close();
        ::DeleteFile( strTempPath.c_str() );
        return false;
    }

    const DWORD dwCount = static_cast<DWORD>( m_records.size() );
    if( ZckWriteValue( file, dwCount ) == false ) return false;

    for( RECORD_MAP::const_iterator it = m_records.begin(); it != m_records.end(); ++it )
    {
        const PLAYER_RECORD& record = it->second;
        if( ZckWriteValue( file, record.dwPlayerId ) == false ) return false;
        file.write( record.szName, sizeof( record.szName ) );
        if( file.good() == false ) return false;
        if( ZckWriteValue( file, record.nLevel ) == false ) return false;
        if( ZckWriteValue( file, record.nJob ) == false ) return false;
        for( int nStat = 0; nStat < 4; ++nStat )
            if( ZckWriteValue( file, record.nStats[nStat] ) == false ) return false;
        if( ZckWriteValue( file, record.nBestDps ) == false ) return false;
        if( ZckWriteValue( file, record.dwBestDungeonTime ) == false ) return false;
        if( ZckWriteValue( file, record.dwDungeonWorldId ) == false ) return false;
        if( ZckWriteValue( file, record.dwDungeonLevel ) == false ) return false;
    }

    file.flush();
    const bool bWriteSuccess = file.good();
    file.close();
    if( bWriteSuccess == false )
    {
        ::DeleteFile( strTempPath.c_str() );
        return false;
    }

    if( ::MoveFileEx( strTempPath.c_str(), strPath.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH ) == FALSE )
    {
        ::DeleteFile( strTempPath.c_str() );
        return false;
    }

    m_bDirty = false;
    m_dwLastSaveTick = timeGetTime();
    return true;
}

bool CZckRecordManager::Reload()
{
    EnsureLoaded();

    Process();

    if( Save() == false )
    {
        FLERROR_LOG( PROGRAM_NAME, _T( "[ZCK_RECORD] Reload annule : impossible de sauvegarder .ZckRecordBook.dat." ) );
        return false;
    }

    if( Load() == false )
    {
        FLERROR_LOG( PROGRAM_NAME, _T( "[ZCK_RECORD] Reload impossible : .ZckRecordBook.dat est invalide ou illisible." ) );
        return false;
    }

    BroadcastSnapshot();
    return true;
}

void CZckRecordManager::Process()
{
    EnsureLoaded();

    if( CheckWeeklyReset() )
    {
        Save();
        BroadcastSnapshot();
    }

    const DWORD dwNow = timeGetTime();
    for( DPS_MAP::iterator it = m_dpsSessions.begin(); it != m_dpsSessions.end(); )
    {
        DPS_SESSION& session = it->second;
        if( static_cast<DWORD>( dwNow - session.dwLastHitTick ) > ZCK_RECORD_DPS_TIMEOUT )
        {
            it = m_dpsSessions.erase( it );
            continue;
        }

        const DWORD dwElapsed = static_cast<DWORD>( dwNow - session.dwStartTick );
        if( dwElapsed >= SEC( 1 ) && session.nTotalDamage > 0 )
        {
            const __int64 nDps = static_cast<__int64>(
                static_cast<double>( session.nTotalDamage ) * 1000.0 / static_cast<double>( dwElapsed ) );

            RECORD_MAP::iterator itRecord = m_records.find( it->first );
            if( itRecord != m_records.end() && nDps > itRecord->second.nBestDps )
            {
                itRecord->second.nBestDps = nDps;
                MarkDirty();
            }
        }

        ++it;
    }

    if( m_bDirty && ( m_dwLastSaveTick == 0 || static_cast<DWORD>( dwNow - m_dwLastSaveTick ) >= ZCK_RECORD_SAVE_INTERVAL ) )
    {
        if( Save() == false )
            FLERROR_LOG( PROGRAM_NAME, _T( "[ZCK_RECORD] Echec de sauvegarde de .ZckRecordBook.dat" ) );
    }
}

void CZckRecordManager::UpdateStats( FLWSUser* pUser )
{
    if( IsValidObj( pUser ) == FALSE || pUser->m_idPlayer == 0 || pUser->m_idPlayer == NULL_ID )
        return;

    EnsureLoaded();

    PLAYER_RECORD& record = GetOrCreateRecord( pUser );
    bool bChanged = UpdateIdentity( record, pUser );

    const __int64 stats[4] =
    {
        static_cast<__int64>( pUser->GetStr() ),
        static_cast<__int64>( pUser->GetDex() ),
        static_cast<__int64>( pUser->GetSta() ),
        static_cast<__int64>( pUser->GetInt() )
    };

    for( int i = 0; i < 4; ++i )
    {
        if( stats[i] > record.nStats[i] )
        {
            record.nStats[i] = stats[i];
            bChanged = true;
        }
    }

    if( bChanged )
        MarkDirty();
}

void CZckRecordManager::AddDamage( FLWSUser* pAttacker, OBJID objTarget, __int64 nDamage )
{
    if( IsValidObj( pAttacker ) == FALSE || pAttacker->m_idPlayer == 0 || pAttacker->m_idPlayer == NULL_ID || nDamage <= 0 )
        return;

    EnsureLoaded();

    PLAYER_RECORD& record = GetOrCreateRecord( pAttacker );
    if( UpdateIdentity( record, pAttacker ) )
        MarkDirty();

    const DWORD dwNow = timeGetTime();
    DPS_SESSION& session = m_dpsSessions[pAttacker->m_idPlayer];

    const bool bNewSession =
        session.dwStartTick == 0 ||
        session.objTarget != objTarget ||
        static_cast<DWORD>( dwNow - session.dwLastHitTick ) > ZCK_RECORD_DPS_TIMEOUT;

    if( bNewSession )
    {
        session.objTarget = objTarget;
        session.nTotalDamage = nDamage;
        session.dwStartTick = dwNow;
        session.dwLastHitTick = dwNow;
        return;
    }

    if( nDamage > 0 && session.nTotalDamage > ( std::numeric_limits<__int64>::max )() - nDamage )
        session.nTotalDamage = ( std::numeric_limits<__int64>::max )();
    else
        session.nTotalDamage += nDamage;

    session.dwLastHitTick = dwNow;
}

void CZckRecordManager::UpdateDungeonRecord( FLWSUser* pUser, DWORD dwCompleteTime, DWORD dwWorldId, DWORD dwDungeonLevel )
{
    if( IsValidObj( pUser ) == FALSE || pUser->m_idPlayer == 0 || pUser->m_idPlayer == NULL_ID || dwCompleteTime == 0 )
        return;

    EnsureLoaded();

    PLAYER_RECORD& record = GetOrCreateRecord( pUser );
    bool bChanged = UpdateIdentity( record, pUser );
    if( record.dwBestDungeonTime == 0 || dwCompleteTime < record.dwBestDungeonTime )
    {
        record.dwBestDungeonTime = dwCompleteTime;
        record.dwDungeonWorldId = dwWorldId;
        record.dwDungeonLevel = dwDungeonLevel;
        bChanged = true;
    }

    if( bChanged )
        MarkDirty();
}

void CZckRecordManager::BuildTop10( ZCK_RECORD_TYPE type, std::vector<ZCK_RECORD_ENTRY>& out ) const
{
    out.clear();

    for( RECORD_MAP::const_iterator it = m_records.begin(); it != m_records.end(); ++it )
    {
        const PLAYER_RECORD& record = it->second;
        __int64 nValue = 0;

        switch( type )
        {
        case ZCK_RECORD_STR:     nValue = record.nStats[0]; break;
        case ZCK_RECORD_DEX:     nValue = record.nStats[1]; break;
        case ZCK_RECORD_STA:     nValue = record.nStats[2]; break;
        case ZCK_RECORD_INT:     nValue = record.nStats[3]; break;
        case ZCK_RECORD_DPS:     nValue = record.nBestDps; break;
        case ZCK_RECORD_DUNGEON: nValue = static_cast<__int64>( record.dwBestDungeonTime ); break;
        default:                 continue;
        }

        if( nValue <= 0 )
            continue;

        ZCK_RECORD_ENTRY entry;
        entry.dwPlayerId = record.dwPlayerId;
        FLStrcpy( entry.szName, _countof( entry.szName ), record.szName );
        entry.nLevel = record.nLevel;
        entry.nJob = record.nJob;
        entry.nValue = nValue;
        if( type == ZCK_RECORD_DUNGEON )
        {
            entry.dwDungeonWorldId = record.dwDungeonWorldId;
            entry.dwDungeonLevel = record.dwDungeonLevel;
        }
        out.push_back( entry );
    }

    const bool bDungeon = ( type == ZCK_RECORD_DUNGEON );
    std::sort( out.begin(), out.end(), [bDungeon]( const ZCK_RECORD_ENTRY& lhs, const ZCK_RECORD_ENTRY& rhs )
    {
        if( lhs.nValue != rhs.nValue )
            return bDungeon ? lhs.nValue < rhs.nValue : lhs.nValue > rhs.nValue;
        return lhs.dwPlayerId < rhs.dwPlayerId;
    } );

    if( out.size() > ZCK_RECORD_TOP_COUNT )
        out.resize( ZCK_RECORD_TOP_COUNT );
}

void CZckRecordManager::BuildSnapshot( CAr& ar ) const
{
    ar << NULL_ID << SNAPSHOTTYPE_ZCK_RECORD;
    ar << m_dwVersion << m_nWeekStart;

    for( int nType = 0; nType < ZCK_RECORD_COUNT; ++nType )
    {
        std::vector<ZCK_RECORD_ENTRY> ranking;
        BuildTop10( static_cast<ZCK_RECORD_TYPE>( nType ), ranking );

        const DWORD dwCount = static_cast<DWORD>( ranking.size() );
        ar << dwCount;
        for( DWORD i = 0; i < dwCount; ++i )
        {
            const ZCK_RECORD_ENTRY& entry = ranking[i];
            ar << entry.dwPlayerId;
            ar.Write( entry.szName, sizeof( entry.szName ) );
            ar << entry.nLevel << entry.nJob << entry.nValue;
            ar << entry.dwDungeonWorldId << entry.dwDungeonLevel;
        }
    }
}

void CZckRecordManager::SendSnapshot( FLWSUser* pUser )
{
    if( IsValidObj( pUser ) == FALSE )
        return;

    EnsureLoaded();

    CAr ar;
    BuildSnapshot( ar );
    u_long uBufSize = 0;
    LPBYTE lpBuf = ar.GetBuffer( &uBufSize );
    pUser->AddBlock( lpBuf, uBufSize );
}

void CZckRecordManager::BroadcastSnapshot()
{
    EnsureLoaded();

    CAr ar;
    BuildSnapshot( ar );
    u_long uBufSize = 0;
    LPBYTE lpBuf = ar.GetBuffer( &uBufSize );
    g_xWSUserManager->AddBlock( lpBuf, uBufSize );
}

#endif // __ZCK_RECORD
#include "StdAfx.h"
#include "ZckTeleporteur.h"

#ifdef __ZCK_TELEPORTEUR

#ifdef __WORLDSERVER
#include "InstanceDungeonBase.h"
#include "worldmng.h"
#include "../WORLDSERVER/User.h"
#include "../WORLDSERVER/FLItemUsing.h"

extern CWorldMng g_WorldMng;
#endif // __WORLDSERVER

#ifdef __CLIENT
#include "../Neuz/DPClient.h"
#include "ResData.h"
#ifdef __ZCK_VISU_MODELS
#include "../_Interface/WndModelView.h"
#endif // __ZCK_VISU_MODELS

extern CDPClient g_DPlay;
#endif // __CLIENT

namespace
{
#ifdef __WORLDSERVER
    DWORD GetZckTeleporteurStateTextID( int nState )
    {
        switch( nState )
        {
        case 1: return TID_GAME_TRADE_NOTUSE;
        case 2: return TID_GAME_DIE_NOTUSE;
        case 3: return TID_GAME_VENDOR_NOTUSE;
        case 4: return TID_GAME_ATTACK_NOTUSE;
        case 5: return TID_GAME_FLY_NOTUSE;
        case 6: return TID_GAME_ATTACK_NOTUSE;
        default: return 0;
        }
    }

    void AddZckTeleporteurStateError( FLWSUser* pUser, int nState )
    {
        if( IsValidObj( pUser ) == FALSE )
            return;

        const DWORD dwMsgID = GetZckTeleporteurStateTextID( nState );
        if( dwMsgID != 0 )
            pUser->AddDefinedText( TID_GAME_STATE_NOTUSE, "\"%s\"", prj.GetText( dwMsgID ) );
    }
#endif // __WORLDSERVER
}

CZckTeleporteur::CZckTeleporteur()
{
}

CZckTeleporteur::~CZckTeleporteur()
{
    m_mapDestinations.clear();
}

CZckTeleporteur* CZckTeleporteur::GetInstance()
{
    static CZckTeleporteur sTeleporter;
    return &sTeleporter;
}

BOOL CZckTeleporteur::LoadScript( LPCTSTR pszFileName )
{
    if( pszFileName == NULL || pszFileName[0] == 0 )
        return FALSE;

    CScript script;
    if( script.Load( pszFileName ) == FALSE )
    {
        FLERROR_LOG( PROGRAM_NAME, _T( "CZckTeleporteur::LoadScript() - Impossible de charger %s." ), pszFileName );
        return FALSE;
    }

    std::map<int, ZckTeleportDestination> mapLoaded;
    int nKey = 0;

    script.GetToken();
    while( script.tok != FINISHED )
    {
        if( script.Token == _T( "TELEPORT_CASE" ) )
        {
            ZckTeleportDestination destination;
            destination.dwWorldID = static_cast<DWORD>( script.GetNumber() );
            destination.vPos.x = static_cast<float>( script.GetNumber() );
            destination.vPos.y = static_cast<float>( script.GetNumber() );
            destination.vPos.z = static_cast<float>( script.GetNumber() );

            script.GetToken();
            BYTE byType = ZCK_TELEPORT_TYPE_CITY;
            if( script.Token == _T( "TYPE_ZONE" ) )
                byType = ZCK_TELEPORT_TYPE_ZONE;
            else if( script.Token == _T( "TYPE_DUNGEON" ) )
                byType = ZCK_TELEPORT_TYPE_DUNGEON;

            script.GetToken();
            const CString strName = script.Token;
            script.GetToken();
            const CString strTextureName = script.Token;
            script.GetToken();
            const CString strDescription = script.Token;

#ifdef __CLIENT
            destination.byType = byType;
            destination.strName = strName;
            destination.strTextureName = strTextureName;
            destination.strDescription = strDescription;
#else
            UNREFERENCED_PARAMETER( byType );
            UNREFERENCED_PARAMETER( strName );
            UNREFERENCED_PARAMETER( strTextureName );
            UNREFERENCED_PARAMETER( strDescription );
#endif // __CLIENT

            script.GetToken();
            if( script.Token != _T( "{" ) )
            {
                FLERROR_LOG( PROGRAM_NAME, _T( "CZckTeleporteur::LoadScript() - Bloc d'objets manquant pour la destination %d." ), nKey );
                return FALSE;
            }

            DWORD dwItemID = static_cast<DWORD>( script.GetNumber() );
            while( script.tok != FINISHED && script.Token != _T( "}" ) )
            {
#ifdef __CLIENT
                const PT_ITEM_SPEC pItemSpec = g_xSpecManager->GetSpecItem( dwItemID );
                if( pItemSpec == NULL )
                {
                    FLERROR_LOG( PROGRAM_NAME, _T( "CZckTeleporteur::LoadScript() - Item invalide %u pour la destination %d." ), dwItemID, nKey );
                    return FALSE;
                }
                destination.vecItems.push_back( dwItemID );
#endif // __CLIENT
                dwItemID = static_cast<DWORD>( script.GetNumber() );
            }

            if( script.tok == FINISHED )
            {
                FLERROR_LOG( PROGRAM_NAME, _T( "CZckTeleporteur::LoadScript() - Fin de fichier inattendue dans la destination %d." ), nKey );
                return FALSE;
            }

            mapLoaded.insert( std::make_pair( nKey, destination ) );
            ++nKey;
        }

        script.GetToken();
    }

    if( mapLoaded.empty() )
    {
        FLERROR_LOG( PROGRAM_NAME, _T( "CZckTeleporteur::LoadScript() - Aucune destination valide dans %s." ), pszFileName );
        return FALSE;
    }

    m_mapDestinations.swap( mapLoaded );
    return TRUE;
}

#ifdef __WORLDSERVER
void CZckTeleporteur::DoTeleport( int nKey, FLWSUser* pUser )
{
    if( IsValidObj( pUser ) == FALSE )
        return;

    const std::map<int, ZckTeleportDestination>::const_iterator it = m_mapDestinations.find( nKey );
    if( it == m_mapDestinations.end() )
        return;

    CWorld* pUserWorld = pUser->GetWorld();
    if( pUserWorld == NULL )
        return;

    CWorld* pTargetWorld = g_WorldMng.GetWorld( it->second.dwWorldID );
    if( pTargetWorld == NULL )
        return;

    const int nState = pUser->GetSummonState();
    if( nState != 0 )
    {
        AddZckTeleporteurStateError( pUser, nState );
        return;
    }

    if( pUser->m_bAllAction == FALSE )
    {
        pUser->AddDefinedText( TID_GAME_GUILDCOMBAT_NOT_USE );
        return;
    }

    if( pUser->m_pActMover != NULL && pUser->m_pActMover->IsActAttack() != FALSE )
    {
        pUser->AddDefinedText( TID_GAME_GUILDCOMBAT_NOT_USE );
        return;
    }

    if( pUser->m_pActMover == NULL || pUser->m_pActMover->IsState( OBJSTA_STAND ) == FALSE )
    {
        pUser->AddDefinedText( TID_PK_STAND_LIMIT );
        return;
    }

    if( pUser->IsCollecting() == TRUE )
    {
        pUser->AddDefinedText( TID_GAME_GUILDCOMBAT_NOT_USE );
        return;
    }
    if( g_pItemUsing->canUseTeleportation( pUser ) == false )
        return;

    CInstanceDungeonHelper* pInstanceHelper = CInstanceDungeonHelper::GetInstance();
    if( pInstanceHelper->IsInstanceDungeon( it->second.dwWorldID ) )
    {
        if( pInstanceHelper->IsInstanceDungeon( pUserWorld->GetID() ) )
            return;

        const DWORD dwGuildID = 0;
        const DWORD eDungeonLevel = 0;
        pInstanceHelper->EnteranceDungeon( pUser, it->second.dwWorldID, dwGuildID, eDungeonLevel );
        return;
    }

    if( pTargetWorld->GetHeightAttribute( it->second.vPos.x, it->second.vPos.z ) != HATTR_NONE )
    {
        pUser->AddDefinedText( TID_MMI_TELEPORTEUR );
        return;
    }

#ifdef __LAYER_1015
    if( pUser->REPLACE( g_uIdofMulti, it->second.dwWorldID, it->second.vPos, REPLACE_FORCE, nDefaultLayer ) == FALSE )
#else
    if( pUser->REPLACE( g_uIdofMulti, it->second.dwWorldID, it->second.vPos, REPLACE_FORCE ) == FALSE )
#endif // __LAYER_1015
        pUser->AddDefinedText( TID_MMI_TELEPORTEUR );
}
#endif // __WORLDSERVER

#ifdef __CLIENT
CWndZckTeleporteur::CWndZckTeleporteur()
    : m_pWndListLocations( NULL )
    , m_pWndListItems( NULL )
    , m_pWndComboFilter( NULL )
    , m_pLocationName( NULL )
    , m_pWndDescription( NULL )
    , m_pTexture( NULL )
{
}

CWndZckTeleporteur::~CWndZckTeleporteur()
{
    m_vecTeleports.clear();
}

BOOL CWndZckTeleporteur::Initialize( CWndBase* pWndParent, DWORD )
{
    return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_ZCK_TELEPORTEUR, 0, CPoint( 0, 0 ), pWndParent );
}

void CWndZckTeleporteur::OnInitialUpdate()
{
    CWndNeuz::OnInitialUpdate();

    m_pWndListLocations = static_cast<CWndListBox*>( GetDlgItem( WIDC_LISTBOX2 ) );
    m_pWndListItems = static_cast<CWndListBox*>( GetDlgItem( WIDC_LISTBOX1 ) );
    m_pLocationName = static_cast<CWndStatic*>( GetDlgItem( WIDC_STATIC2 ) );
    m_pWndDescription = static_cast<CWndText*>( GetDlgItem( WIDC_TEXT ) );
    m_pWndComboFilter = static_cast<CWndComboBox*>( GetDlgItem( WIDC_COMBOBOX1 ) );

    if( m_pWndListLocations == NULL || m_pWndListItems == NULL || m_pLocationName == NULL || m_pWndDescription == NULL || m_pWndComboFilter == NULL )
    {
        Destroy();
        return;
    }

    m_pWndComboFilter->AddWndStyle( EBS_READONLY );
    m_pWndComboFilter->AddString( _T( "All" ) );
    m_pWndComboFilter->AddString( _T( "City" ) );
    m_pWndComboFilter->AddString( _T( "Zone" ) );
    m_pWndComboFilter->AddString( _T( "Dungeon" ) );
    m_pWndComboFilter->SetCurSel( 0 );

    Refresh();
    MoveParentCenter();
}

void CWndZckTeleporteur::OnDraw( C2DRender* p2DRender )
{
    if( m_pTexture != NULL )
    {
        CWndStatic* pWndStatic = static_cast<CWndStatic*>( GetDlgItem( WIDC_STATIC1 ) );
        if( pWndStatic != NULL )
            m_pTexture->Render( p2DRender, pWndStatic->GetWndRect().TopLeft() );
    }

    CWndNeuz::OnDraw( p2DRender );
}

void CWndZckTeleporteur::OnSize( UINT nType, int cx, int cy )
{
    CWndNeuz::OnSize( nType, cx, cy );
}

BOOL CWndZckTeleporteur::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
    return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

BOOL CWndZckTeleporteur::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
    if( message == WNM_SELCHANGE )
    {
        if( nID == WIDC_COMBOBOX1 )
            Refresh();
        else if( nID == WIDC_LISTBOX2 )
            SelectWorld();
    }
    else if( message == WNM_CLICKED && nID == WIDC_BUTTON1 )
    {
        const FilteredDestination* pDestination = GetCurrentDestination();
        if( pDestination != NULL )
        {
            g_DPlay.SendZckTeleporteurRequest( pDestination->nKey );
            Destroy();
        }
    }
    else if( message == WNM_DBLCLK && nID == WIDC_LISTBOX1 )
    {
#ifdef __ZCK_VISU_MODELS
        if( ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) != 0 && m_pWndListItems != NULL )
        {
            const std::vector<DWORD>* pItems = GetCurrentItems();
            const int nSelected = m_pWndListItems->GetCurSel();
            if( pItems != NULL && nSelected >= 0 && static_cast<size_t>( nSelected ) < pItems->size() )
            {
                const PT_ITEM_SPEC pItemSpec = g_xSpecManager->GetSpecItem( pItems->at( static_cast<size_t>( nSelected ) ) );
                if( pItemSpec != NULL && CWndModelView::CanPreview( pItemSpec ) )
                {
                    CWndModelView* pWndModel = static_cast<CWndModelView*>( g_WndMng.GetApplet( APP_MODEL_VIEW ) );
                    if( pWndModel == NULL )
                        pWndModel = static_cast<CWndModelView*>( g_WndMng.CreateApplet( APP_MODEL_VIEW ) );
                    if( pWndModel != NULL )
                        pWndModel->SetItem( pItemSpec->dwID );
                }
            }
        }
#endif // __ZCK_VISU_MODELS
    }

    return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndZckTeleporteur::Refresh()
{
    if( m_pWndListLocations == NULL || m_pWndComboFilter == NULL )
        return;

    m_vecTeleports.clear();
    m_pWndListLocations->ResetContent();

    const int nFilter = m_pWndComboFilter->GetCurSel();
    const std::map<int, ZckTeleportDestination>& destinations = CZckTeleporteur::GetInstance()->GetDestinations();
    for( std::map<int, ZckTeleportDestination>::const_iterator it = destinations.begin(); it != destinations.end(); ++it )
    {
        if( nFilter > 0 && it->second.byType != static_cast<BYTE>( nFilter ) )
            continue;

        FilteredDestination filtered;
        filtered.nKey = it->first;
        filtered.strName.Format( _T( "[%s] %s" ), static_cast<LPCTSTR>( GetTypeName( it->second.byType ) ), static_cast<LPCTSTR>( it->second.strName ) );
        filtered.strTextureName = it->second.strTextureName;
        filtered.strDescription = it->second.strDescription;
        filtered.vecItems = it->second.vecItems;
        m_vecTeleports.push_back( filtered );
        m_pWndListLocations->AddString( filtered.strName );
    }

    if( m_vecTeleports.empty() == false )
    {
        m_pWndListLocations->SetCurSel( 0 );
        SelectWorld();
    }
    else
    {
        m_pTexture = NULL;
        m_pWndListItems->ResetContent();
        m_pLocationName->SetTitle( _T( "" ) );
        m_pWndDescription->SetString( _T( "" ) );
    }
}

void CWndZckTeleporteur::SelectWorld()
{
    const FilteredDestination* pDestination = GetCurrentDestination();
    if( pDestination == NULL || m_pWndListItems == NULL || m_pLocationName == NULL || m_pWndDescription == NULL )
        return;

    m_pLocationName->SetTitle( pDestination->strName );
    m_pTexture = CWndBase::m_textureMng.AddTexture(
        g_Neuz.m_pd3dDevice,
        MakePath( DIR_THEME, pDestination->strTextureName ),
        0xffff00ff
    );

    m_pWndListItems->ResetContent();
    for( size_t i = 0; i < pDestination->vecItems.size(); ++i )
        m_pWndListItems->AddString( _T( "" ) );

    m_pWndDescription->SetString( pDestination->strDescription );
}

CString CWndZckTeleporteur::GetTypeName( BYTE byType ) const
{
    switch( byType )
    {
    case ZCK_TELEPORT_TYPE_CITY: return _T( "City" );
    case ZCK_TELEPORT_TYPE_ZONE: return _T( "Zone" );
    case ZCK_TELEPORT_TYPE_DUNGEON: return _T( "Dungeon" );
    default: return _T( "" );
    }
}

const CWndZckTeleporteur::FilteredDestination* CWndZckTeleporteur::GetCurrentDestination() const
{
    if( m_pWndListLocations == NULL )
        return NULL;

    const int nSelected = m_pWndListLocations->GetCurSel();
    if( nSelected < 0 || static_cast<size_t>( nSelected ) >= m_vecTeleports.size() )
        return NULL;

    return &m_vecTeleports[static_cast<size_t>( nSelected )];
}

const std::vector<DWORD>* CWndZckTeleporteur::GetCurrentItems() const
{
    const FilteredDestination* pDestination = GetCurrentDestination();
    return pDestination != NULL ? &pDestination->vecItems : NULL;
}
#endif // __CLIENT

#endif // __ZCK_TELEPORTEUR
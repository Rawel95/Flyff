#include "StdAfx.h"

#ifdef __ZCK_WIKI
#include "WndWikiItems.h"
#ifdef __ZCK_VISU_MODELS
#include "WndModelView.h"
#endif // __ZCK_VISU_MODELS
#include "ResData.h"

namespace
{
	bool SortByItemLevelHighest( PT_ITEM_SPEC pFirst, PT_ITEM_SPEC pSecond )
	{
		const int nFirstLevel = ( pFirst->nMinLimitLevel <= 0 ) ? 1 : pFirst->nMinLimitLevel;
		const int nSecondLevel = ( pSecond->nMinLimitLevel <= 0 ) ? 1 : pSecond->nMinLimitLevel;
		return nFirstLevel > nSecondLevel;
	}

	bool SortByItemLevelLowest( PT_ITEM_SPEC pFirst, PT_ITEM_SPEC pSecond )
	{
		const int nFirstLevel = ( pFirst->nMinLimitLevel <= 0 ) ? 1 : pFirst->nMinLimitLevel;
		const int nSecondLevel = ( pSecond->nMinLimitLevel <= 0 ) ? 1 : pSecond->nMinLimitLevel;
		return nFirstLevel < nSecondLevel;
	}
}

CWndWikiItems::CWndWikiItems()
	: m_pWndWikiCre( NULL )
	, m_pNameFilter( NULL )
	, m_pMinLevelFilter( NULL )
	, m_pMaxLevelFilter( NULL )
	, m_pComboSex( NULL )
	, m_pComboJob( NULL )
	, m_pListItems( NULL )
	, m_pWndComboFilter( NULL )
{
}

CWndWikiItems::~CWndWikiItems()
{
	m_vecItems.clear();
	SAFE_DELETE( m_pWndWikiCre );
}

BOOL CWndWikiItems::Initialize( CWndBase* pWndParent, DWORD )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_WIKI_ITEMS, 0, CPoint( 0, 0 ), pWndParent );
}

void CWndWikiItems::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndTreeCtrl* pWndTreeCtrl = static_cast<CWndTreeCtrl*>( GetDlgItem( WIDC_TREE1 ) );
	m_pListItems = static_cast<CWndListBox*>( GetDlgItem( WIDC_LISTBOX1 ) );
	m_pNameFilter = static_cast<CWndEdit*>( GetDlgItem( WIDC_EDIT1 ) );
	m_pMinLevelFilter = static_cast<CWndEdit*>( GetDlgItem( WIDC_EDIT2 ) );
	m_pMaxLevelFilter = static_cast<CWndEdit*>( GetDlgItem( WIDC_EDIT3 ) );
	m_pComboSex = static_cast<CWndComboBox*>( GetDlgItem( WIDC_COMBOBOX1 ) );
	m_pComboJob = static_cast<CWndComboBox*>( GetDlgItem( WIDC_COMBOBOX2 ) );
	m_pWndComboFilter = static_cast<CWndComboBox*>( GetDlgItem( WIDC_COMBOBOX4 ) );

	if( pWndTreeCtrl == NULL || m_pListItems == NULL || m_pNameFilter == NULL ||
		m_pMinLevelFilter == NULL || m_pMaxLevelFilter == NULL || m_pComboSex == NULL ||
		m_pComboJob == NULL || m_pWndComboFilter == NULL )
	{
		Destroy( TRUE );
		return;
	}

	pWndTreeCtrl->LoadTreeScript( MakePath( DIR_CLIENT, _T( "ZckWikiItems.inc" ) ) );

	m_pMinLevelFilter->AddWndStyle( EBS_NUMBER );
	m_pMaxLevelFilter->AddWndStyle( EBS_NUMBER );
	m_pMinLevelFilter->SetString( _T( "1" ) );
	m_pMaxLevelFilter->SetString( _T( "200" ) );

	m_pComboSex->AddString( _T( "~" ) );
	m_pComboSex->AddString( _T( "Homme" ) );
	m_pComboSex->AddString( _T( "Femme" ) );
	m_pComboSex->SetCurSel( 0 );
	m_pComboSex->AddWndStyle( EBS_READONLY );

	int nComboJobIndex = m_pComboJob->AddString( _T( "~" ) );
	m_pComboJob->SetItemData( nComboJobIndex, NULL_ID );
	for( int i = 0; i < MAX_JOB; ++i )
	{
		if( i == 5 || i == 14 || i == 15 )
			continue;

		nComboJobIndex = m_pComboJob->AddString( prj.m_aJob[i].szName );
		m_pComboJob->SetItemData( nComboJobIndex, static_cast<DWORD>( i ) );
	}
	m_pComboJob->SetCurSel( 0 );
	m_pComboJob->AddWndStyle( EBS_READONLY );

	m_pWndComboFilter->AddWndStyle( EBS_READONLY );
	m_pWndComboFilter->AddString( _T( "Trier par niveau d'objet (Plus haut)" ) );
	m_pWndComboFilter->AddString( _T( "Trier par niveau d'objet (Plus bas)" ) );
	m_pWndComboFilter->SetCurSel( 1 );

	ApplyFilters();
	MoveParentCenter();
}

BOOL CWndWikiItems::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( message == WNM_CLICKED )
	{
		if( nID == WIDC_TREE1 )
		{
			LPTREEELEM pTreeElem = reinterpret_cast<LPTREEELEM>( pLResult );
			if( pTreeElem != NULL )
			{
				if( pTreeElem->m_strKeyword == m_strKeyword )
					return FALSE;

				m_strKeyword = pTreeElem->m_strKeyword;
				ApplyFilters();
			}
		}
		else if( nID == WIDC_BUTTON1 )
		{
			CWndTreeCtrl* pWndTreeCtrl = static_cast<CWndTreeCtrl*>( GetDlgItem( WIDC_TREE1 ) );
			if( pWndTreeCtrl != NULL )
				pWndTreeCtrl->SetCurSel( _T( "Alle Items" ) );

			m_strKeyword = _T( "Alle Items" );
			m_pNameFilter->SetString( _T( "" ) );
			m_pMinLevelFilter->SetString( _T( "1" ) );
			m_pMaxLevelFilter->SetString( _T( "200" ) );
			m_pComboSex->SetCurSel( 0 );
			m_pComboJob->SetCurSel( 0 );
			ApplyFilters();
		}
	}
	else if( message == WNM_SELCHANGE )
	{
		if( nID == WIDC_COMBOBOX1 || nID == WIDC_COMBOBOX2 || nID == WIDC_COMBOBOX4 )
			ApplyFilters();
	}
	else if( message == EN_CHANGE )
	{
		if( nID == WIDC_EDIT1 || nID == WIDC_EDIT2 || nID == WIDC_EDIT3 )
			ApplyFilters();
	}
	else if( message == WNM_DBLCLK && nID == WIDC_LISTBOX1 )
	{
		const int nSelected = m_pListItems->GetCurSel();
		if( nSelected >= 0 && nSelected < static_cast<int>( m_vecItems.size() ) )
		{
			PT_ITEM_SPEC pItemSpec = m_vecItems[nSelected];
			if( pItemSpec != NULL )
			{
#ifdef __ZCK_VISU_MODELS
				if( GetAsyncKeyState( VK_SHIFT ) & 0x8000 )
				{
					if( CWndModelView::CanPreview( pItemSpec ) )
					{
						CWndModelView* pWndModel = static_cast<CWndModelView*>( g_WndMng.GetApplet( APP_MODEL_VIEW ) );
						if( pWndModel == NULL )
							pWndModel = static_cast<CWndModelView*>( g_WndMng.CreateApplet( APP_MODEL_VIEW ) );
						if( pWndModel != NULL )
							pWndModel->SetItem( pItemSpec->dwID );
					}
				}
				else
#endif // __ZCK_VISU_MODELS
				if( g_pPlayer != NULL && g_pPlayer->IsAuthHigher( AUTH_ADMINISTRATOR ) )
				{
					SAFE_DELETE( m_pWndWikiCre );
					m_pWndWikiCre = new CWndWikiCreate( pItemSpec->dwID );
					m_pWndWikiCre->Initialize( this );
				}
			}
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndWikiItems::ApplyFilters()
{
	m_vecItems.clear();

	if( g_xSpecManager == NULL || m_pListItems == NULL )
		return;

	CString strFilter = m_pNameFilter != NULL ? m_pNameFilter->GetString() : _T( "" );
	strFilter.MakeLower();

	int nMinLevel = m_pMinLevelFilter != NULL ? _ttoi( m_pMinLevelFilter->GetString() ) : 1;
	int nMaxLevel = m_pMaxLevelFilter != NULL ? _ttoi( m_pMaxLevelFilter->GetString() ) : 200;
	if( nMinLevel <= 0 )
		nMinLevel = 1;

	const int nSexFilter = m_pComboSex != NULL ? m_pComboSex->GetCurSel() : 0;
	DWORD dwJobFilter = NULL_ID;
	if( m_pComboJob != NULL && m_pComboJob->GetCurSel() > 0 )
		dwJobFilter = m_pComboJob->GetItemData( m_pComboJob->GetCurSel() );

	for( SpecItemIdItr it = g_xSpecManager->m_SpecItemIdMap.begin(); it != g_xSpecManager->m_SpecItemIdMap.end(); ++it )
	{
		PT_ITEM_SPEC pItemSpec = &it->second;
		if( pItemSpec == NULL )
			continue;

		if( pItemSpec->dwItemKind2 == IK2_MOB )
			continue;

		if( !m_strKeyword.IsEmpty() && m_strKeyword != _T( "Alle Items" ) )
		{
			if( m_strKeyword == _T( "Waffen" ) )
			{
				if( pItemSpec->dwItemKind1 != IK1_WEAPON && pItemSpec->dwItemKind3 != IK3_SHIELD && pItemSpec->dwItemKind3 != IK3_MAGICBARUNA && pItemSpec->dwItemKind3 != IK3_ZEMBARUNA ) continue;
			}
			else if( m_strKeyword == _T( "Schwert 1-H" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_SWD || pItemSpec->dwHanded != HD_ONE ) continue;
			}
			else if( m_strKeyword == _T( "Schwert 2-H" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_SWD || pItemSpec->dwHanded != HD_TWO ) continue;
			}
			else if( m_strKeyword == _T( "Axt 1-H" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_AXE || pItemSpec->dwHanded != HD_ONE ) continue;
			}
			else if( m_strKeyword == _T( "Axt 2-H" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_AXE || pItemSpec->dwHanded != HD_TWO ) continue;
			}
			else if( m_strKeyword == _T( "Knuckle" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_KNUCKLEHAMMER ) continue;
			}
			else if( m_strKeyword == _T( "Staff" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_STAFF ) continue;
			}
			else if( m_strKeyword == _T( "Bogen" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_BOW
#ifdef __ADDWEAPON_CROSSBOW16
					&& pItemSpec->dwItemKind3 != IK3_CROSSBOW
#endif // __ADDWEAPON_CROSSBOW16
					) continue;
			}
			else if( m_strKeyword == _T( "Yoyo" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_YOYO ) continue;
			}
			else if( m_strKeyword == _T( "Wand" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_WAND ) continue;
			}
			else if( m_strKeyword == _T( "Stick" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_CHEERSTICK ) continue;
			}
			else if( m_strKeyword == _T( "Schild" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_SHIELD && pItemSpec->dwItemKind3 != IK3_MAGICBARUNA && pItemSpec->dwItemKind3 != IK3_ZEMBARUNA ) continue;
			}
			else if( m_strKeyword == _T( "Rüstung" ) )
			{
				if( pItemSpec->dwItemKind1 != IK1_ARMOR || ( pItemSpec->dwItemKind2 != IK2_ARMORETC && pItemSpec->dwItemKind2 != IK2_ARMOR ) ) continue;
			}
			else if( m_strKeyword == _T( "Hat" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_HELMET ) continue;
			}
			else if( m_strKeyword == _T( "Suit" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_SUIT ) continue;
			}
			else if( m_strKeyword == _T( "Gauntlet" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_GAUNTLET ) continue;
			}
			else if( m_strKeyword == _T( "Boots" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_BOOTS ) continue;
			}
			else if( m_strKeyword == _T( "Schmuck" ) )
			{
				if( pItemSpec->dwItemKind2 != IK2_JEWELRY ) continue;
			}
			else if( m_strKeyword == _T( "Ring" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_RING ) continue;
			}
			else if( m_strKeyword == _T( "Ohrring" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_EARRING ) continue;
			}
			else if( m_strKeyword == _T( "Kette" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_NECKLACE ) continue;
			}
			else if( m_strKeyword == _T( "Fashion" ) )
			{
				if( pItemSpec->dwItemKind2 != IK2_CLOTH && pItemSpec->dwItemKind2 != IK2_CLOTHETC ) continue;
			}
			else if( m_strKeyword == _T( "Hat" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_HAT && pItemSpec->dwParts != PARTS_HAT ) continue;
			}
			else if( m_strKeyword == _T( "Suit" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_CLOTH && pItemSpec->dwParts != PARTS_CLOTH ) continue;
			}
			else if( m_strKeyword == _T( "Gauntlet" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_GLOVE && pItemSpec->dwParts != PARTS_GLOVE ) continue;
			}
			else if( m_strKeyword == _T( "Boots" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_SHOES && pItemSpec->dwParts != PARTS_BOOTS ) continue;
			}
			else if( m_strKeyword == _T( "Umhang" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_CLOAK && pItemSpec->dwParts != PARTS_CLOAK ) continue;
			}
			else if( m_strKeyword == _T( "Maske" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_MASK && pItemSpec->dwParts != PARTS_MASK ) continue;
			}
			else if( m_strKeyword == _T( "AuraWing" ) )
			{
#ifdef __ZCK_AILES_SFX
				if( pItemSpec->dwItemKind3 != IK3_AILES_SFX ) continue;
#else
				continue;
#endif // __ZCK_AILES_SFX
			}
			else if( m_strKeyword == _T( "Pets" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_PET && pItemSpec->dwItemKind3 != IK3_EGG ) continue;
			}
			else if( m_strKeyword == _T( "Stat" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_EGG ) continue;
			}
			else if( m_strKeyword == _T( "Sammelpet" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_PET || pItemSpec->dwReferStat1 != NULL_ID ) continue;
			}
			else if( m_strKeyword == _T( "Buffpets" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_PET || pItemSpec->dwReferStat1 != PET_VIS ) continue;
			}
			else if( m_strKeyword == _T( "Möbel" ) )
			{
				if( pItemSpec->dwItemKind1 != IK1_HOUSING ) continue;
			}
			else if( m_strKeyword == _T( "Haus" ) )
			{
				if( pItemSpec->dwItemKind2 != IK2_FURNITURE && pItemSpec->dwItemKind2 != IK2_PAPERING ) continue;
			}
			else if( m_strKeyword == _T( "Gildenhaus" ) )
			{
				if( pItemSpec->dwItemKind2 != IK2_GUILDHOUSE_FURNITURE && pItemSpec->dwItemKind2 != IK2_GUILDHOUSE_PAPERING ) continue;
			}
			else if( m_strKeyword == _T( "Sonstiges" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_SOCKETCARD && pItemSpec->dwItemKind3 != IK3_SOCKETCARD2 &&
					pItemSpec->dwItemKind3 != IK3_ELECARD && pItemSpec->dwItemKind3 != IK3_VIS &&
					pItemSpec->dwItemKind2 != IK2_GEM && pItemSpec->dwItemKind3 != IK3_TICKET ) continue;
			}
			else if( m_strKeyword == _T( "Karten" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_SOCKETCARD && pItemSpec->dwItemKind3 != IK3_SOCKETCARD2 && pItemSpec->dwItemKind3 != IK3_ELECARD ) continue;
			}
			else if( m_strKeyword == _T( "Beads" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_VIS ) continue;
			}
			else if( m_strKeyword == _T( "Questies" ) )
			{
				if( pItemSpec->dwItemKind2 != IK2_GEM ) continue;
			}
			else if( m_strKeyword == _T( "Tickets" ) )
			{
				if( pItemSpec->dwItemKind3 != IK3_TICKET ) continue;
			}
			else if( m_strKeyword == _T( "Fluggeräte" ) )
			{
				if( pItemSpec->dwItemKind1 != IK1_RIDE ) continue;
			}
		}

		if( !strFilter.IsEmpty() )
		{
			CString strName = pItemSpec->szName;
			strName.MakeLower();
			if( strName.Find( strFilter ) < 0 )
				continue;
		}

		const int nItemLevel = ( pItemSpec->nMinLimitLevel <= 0 ) ? 1 : pItemSpec->nMinLimitLevel;
		if( nItemLevel < nMinLevel || nItemLevel > nMaxLevel )
			continue;

		if( nSexFilter == 1 && pItemSpec->dwItemSex != SEX_MALE )
			continue;
		if( nSexFilter == 2 && pItemSpec->dwItemSex != SEX_FEMALE )
			continue;

		if( dwJobFilter != NULL_ID && pItemSpec->dwItemJob != dwJobFilter )
			continue;

		m_vecItems.push_back( pItemSpec );
	}

	SortByItemLevel( m_pWndComboFilter != NULL && m_pWndComboFilter->GetCurSel() == 0 );

	m_pListItems->ResetContent();
	for( size_t i = 0; i < m_vecItems.size(); ++i )
		m_pListItems->AddString( _T( "" ) );
}

void CWndWikiItems::SortByItemLevel( BOOL bHighest )
{
	if( bHighest )
		std::sort( m_vecItems.begin(), m_vecItems.end(), SortByItemLevelHighest );
	else
		std::sort( m_vecItems.begin(), m_vecItems.end(), SortByItemLevelLowest );
}

BOOL CWndWikiItems::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

void CWndWikiItems::OnDestroyChildWnd( CWndBase* pWndChild )
{
	if( pWndChild == m_pWndWikiCre )
		SAFE_DELETE( m_pWndWikiCre );

	CWndNeuz::OnDestroyChildWnd( pWndChild );
}

CWndWikiCreate::CWndWikiCreate( DWORD dwItemID )
	: m_dwItemID( dwItemID )
	, m_pWndEditName( NULL )
	, m_pWndEditCount( NULL )
{
}

CWndWikiCreate::~CWndWikiCreate()
{
}

BOOL CWndWikiCreate::Initialize( CWndBase* pWndParent, DWORD )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_WIKI_ITEMS_CRE, 0, CPoint( 0, 0 ), pWndParent );
}

void CWndWikiCreate::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_pWndEditName = static_cast<CWndEdit*>( GetDlgItem( WIDC_EDIT1 ) );
	m_pWndEditCount = static_cast<CWndEdit*>( GetDlgItem( WIDC_EDIT2 ) );
	if( m_pWndEditName == NULL || m_pWndEditCount == NULL )
	{
		Destroy();
		return;
	}

	PT_ITEM_SPEC pItemSpec = g_xSpecManager != NULL ? g_xSpecManager->GetSpecItem( m_dwItemID ) : NULL;
	CString strTitle;
	strTitle.Format( _T( "Creer objet : %s" ), pItemSpec != NULL ? pItemSpec->szName : _T( "Objet inconnu" ) );
	SetTitle( strTitle );

	m_pWndEditName->SetString( _T( "" ) );
	m_pWndEditCount->AddWndStyle( EBS_NUMBER );
	m_pWndEditCount->SetString( _T( "1" ) );
	MoveParentCenter();
}

BOOL CWndWikiCreate::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( message == WNM_CLICKED )
	{
		if( nID == WIDC_BUTTON1 )
		{
			int nCount = _ttoi( m_pWndEditCount->GetString() );
			if( nCount < 1 )
				nCount = 1;

			CString strName = m_pWndEditName->GetString();
			g_DPlay.SendWikiCreate( m_dwItemID, nCount, strName );
			Destroy();
			return TRUE;
		}
		if( nID == WIDC_BUTTON2 )
		{
			Destroy();
			return TRUE;
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
#endif // __ZCK_WIKI
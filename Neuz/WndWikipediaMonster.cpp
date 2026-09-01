#include "stdafx.h"

#ifdef __WIKIPEDIA_MONSTER
#include "ResData.h"
#include "DPClient.h"
#include "DefineText.h"
#include "AppDefine.h"
#include "WndManager.h"
#include "defineobj.h"
#include "WndWikipediaMonster.h"

extern	CDPClient	g_DPlay;

CWndWikiMob::CWndWikiMob()
{
	m_vecMobs.clear();
	m_nSortType = 0;
	m_bIsGreater = FALSE;
	m_pWikiMobCreate = NULL;
}
CWndWikiMob::~CWndWikiMob()
{
	m_vecMobs.clear();
	m_nSortType = 0;
	m_bIsGreater = FALSE;
	SAFE_DELETE(m_pWikiMobCreate);
}
void CWndWikiMob::OnInitialUpdate()
{
    CWndNeuz::OnInitialUpdate();
    CRect rectRoot = m_pWndRoot->GetLayoutRect();
    CRect rectWindow = GetWindowRect();
    CPoint point( rectRoot.right - rectWindow.Width(), 110 );
    Move( point );
    MoveParentCenter();
	int nIdx = 0;
	m_pListMobs = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
	m_pEditFilter = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	m_pFilter = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	nIdx = m_pFilter->AddString("All");
	m_pFilter->SetItemData(nIdx, 0);
	nIdx = m_pFilter->AddString("Low");
	m_pFilter->SetItemData(nIdx, RANK_LOW);
	nIdx = m_pFilter->AddString("Normal");
	m_pFilter->SetItemData(nIdx, RANK_NORMAL);
	nIdx = m_pFilter->AddString("Captain");
	m_pFilter->SetItemData(nIdx, RANK_CAPTAIN);
	nIdx = m_pFilter->AddString("Mini-Boss");
	m_pFilter->SetItemData(nIdx, RANK_MIDBOSS);
	nIdx = m_pFilter->AddString("Boss");
	m_pFilter->SetItemData(nIdx, RANK_BOSS);
	m_pFilter->SetItemData(nIdx, RANK_SUPER);
	m_pFilter->SetCurSel(0);
	m_pFilter->AddWndStyle( EBS_READONLY );
	UpdateMobs();
}
BOOL CWndWikiMob::Initialize( CWndBase* pWndParent,DWORD dwWndId )
{
	if (!g_pPlayer)
		return FALSE;
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_WIKIPEDIA_MONSTER, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndWikiMob::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( message == WNM_CLICKED )
	{
		Sort();
		switch( nID )
		{
			case WIDC_BUTTON1:
			{
				m_nSortType = 0;
				m_bIsGreater = !m_bIsGreater;
			}
			break;
			case WIDC_BUTTON2:
			{
				m_nSortType = 1;
				m_bIsGreater = !m_bIsGreater;
			}
			break;
			case WIDC_BUTTON3:
			{
				m_nSortType = 2;
				m_bIsGreater = !m_bIsGreater;
			}
			break;
			case WIDC_BUTTON5:
			{
				m_nSortType = 3;
				m_bIsGreater = !m_bIsGreater;
			}
			break;
			case WIDC_BUTTON4:
			{
				m_nSortType = 4;
				m_bIsGreater = !m_bIsGreater;
			}
			break;
		}
	}
	else if( message == EN_CHANGE )
	{
		if( nID == WIDC_EDIT1 )
			UpdateMobs();
	}
	else if( message == WNM_SELCHANGE )
	{
		if( nID == WIDC_COMBOBOX1 )
			UpdateMobs();
	}
	else if( message == WNM_DBLCLK )
	{
		if( nID == WIDC_LISTBOX1 )
		{

		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
void CWndWikiMob::UpdateMobs()
{
	CString strFilter = m_pEditFilter->GetString();
	DWORD dwFilter = m_pFilter->GetSelectedItemData();
	m_vecMobs.clear();
	m_pListMobs->ResetContent();
	for( int i = 0; i < prj.m_nMoverPropSize; i++ )
	{
		MoverProp* pMoverProp = prj.GetMoverProp(i);
		if (pMoverProp)
		{
			if ( pMoverProp->dwAI != AII_MONSTER 
					&& pMoverProp->dwAI != AII_AGGRO_NORMAL
					&& pMoverProp->dwAI != AII_PARTY_AGGRO_SUB
					&& pMoverProp->dwAI != AII_PARTY_AGGRO_LEADER
					&& pMoverProp->dwAI != AII_ARENA_REAPER
					&& pMoverProp->dwAI != AII_CLOCKWORKS
					&& pMoverProp->dwAI != AII_BIGMUSCLE
					&& pMoverProp->dwAI != AII_KRRR
					&& pMoverProp->dwAI != AII_BEAR
					&& pMoverProp->dwAI != AII_METEONYKER			
			)
				continue;
			if ( !strlen(pMoverProp->szName) )
				continue;
			if ( strstr(pMoverProp->szName,"GM") )
				continue;
			if ( strstr(pMoverProp->szName,"?") )
				continue;
			if ( pMoverProp->dwClass == RANK_GUARD )
				continue;
			if ( strFilter.GetLength() )
			{
				if( !strstr(pMoverProp->szName, strFilter) )
					continue;
			}
			if( dwFilter != 0 )
			{
				if( pMoverProp->dwClass != dwFilter )
					continue;
			}
			m_vecMobs.push_back(pMoverProp);
			m_pListMobs->AddString("");
		}
	}
}
bool SortByNameUpper(const MoverProp* pMoverLeft, const MoverProp* pMoverRight)
{
	std::string strLeft = pMoverLeft->szName;
	std::string strRight = pMoverRight->szName;
	return (strLeft < strRight);
}
bool SortByNameLower(const MoverProp* pMoverLeft, const MoverProp* pMoverRight)
{
	std::string strLeft = pMoverLeft->szName;
	std::string strRight = pMoverRight->szName;
	return (strLeft > strRight);
}
bool SortByLevelUpper(const MoverProp* pMoverLeft, const MoverProp* pMoverRight)
{
	return (pMoverLeft->dwLevel < pMoverRight->dwLevel);
}
bool SortByLevelLower(const MoverProp* pMoverLeft, const MoverProp* pMoverRight)
{
	return (pMoverLeft->dwLevel > pMoverRight->dwLevel);
}
bool SortByHpUpper(const MoverProp* pMoverLeft, const MoverProp* pMoverRight)
{
	return (pMoverLeft->dwAddHp < pMoverRight->dwAddHp);
}
bool SortByHpLower(const MoverProp* pMoverLeft, const MoverProp* pMoverRight)
{
	return (pMoverLeft->dwAddHp > pMoverRight->dwAddHp);
}
bool SortByAtkUpper(const MoverProp* pMoverLeft, const MoverProp* pMoverRight)
{
	return (pMoverLeft->dwAtkMax < pMoverRight->dwAtkMax);
}
bool SortByAtkLower(const MoverProp* pMoverLeft, const MoverProp* pMoverRight)
{
	return (pMoverLeft->dwAtkMax > pMoverRight->dwAtkMax);
}
bool SortByElementUpper(const MoverProp* pMoverLeft, const MoverProp* pMoverRight)
{
	return (pMoverLeft->eElementType < pMoverRight->eElementType);
}
bool SortByElementLower(const MoverProp* pMoverLeft, const MoverProp* pMoverRight)
{
	return (pMoverLeft->eElementType > pMoverRight->eElementType);
}
void CWndWikiMob::SortByName()
{
	if( m_bIsGreater )
		std::sort(m_vecMobs.begin(),m_vecMobs.end(), SortByNameUpper);
	else
		std::sort(m_vecMobs.begin(),m_vecMobs.end(), SortByNameLower);
}
void CWndWikiMob::SortByLevel()
{
	if( m_bIsGreater )
		std::sort(m_vecMobs.begin(),m_vecMobs.end(), SortByLevelUpper);
	else
		std::sort(m_vecMobs.begin(),m_vecMobs.end(), SortByLevelLower);
}
void CWndWikiMob::SortByHp()
{
	if( m_bIsGreater )
		std::sort(m_vecMobs.begin(),m_vecMobs.end(), SortByHpUpper);
	else
		std::sort(m_vecMobs.begin(),m_vecMobs.end(), SortByHpLower);
}
void CWndWikiMob::SortByAtk()
{
	if( m_bIsGreater )
		std::sort(m_vecMobs.begin(),m_vecMobs.end(), SortByAtkUpper);
	else
		std::sort(m_vecMobs.begin(),m_vecMobs.end(), SortByAtkLower);
}
void CWndWikiMob::SortByElement()
{
	if( m_bIsGreater )
		std::sort(m_vecMobs.begin(),m_vecMobs.end(), SortByElementUpper);
	else
		std::sort(m_vecMobs.begin(),m_vecMobs.end(), SortByElementLower);
}
void CWndWikiMob::Sort()
{
	m_pListMobs->EnableWindow(FALSE);
	switch( m_nSortType )
	{
		case 0: SortByName(); break;
		case 1: SortByLevel(); break;
		case 2: SortByHp(); break;
		case 3: SortByAtk(); break;
		case 4: SortByElement(); break;
	}
	m_pListMobs->EnableWindow(TRUE);
}
CWndWikiMobCreate::CWndWikiMobCreate()
{
	m_pMoverProp = NULL;
}
BOOL CWndWikiMobCreate::Initialize(CWndBase *pWndParent, DWORD dwStyle)
{
	if (!(g_pPlayer && g_pPlayer->m_dwAuthorization < AUTH_GAMEMASTER))
		return FALSE;
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_WIKIPEDIA_MONSTER_CREATE, WBS_MODAL, CPoint(0, 0), pWndParent);
}
void CWndWikiMobCreate::OnInitialUpdate()
{
    CWndNeuz::OnInitialUpdate();
    CRect rectRoot = m_pWndRoot->GetLayoutRect();
    CRect rectWindow = GetWindowRect();
    CPoint point( rectRoot.right - rectWindow.Width(), 110 );
    Move( point );
    MoveParentCenter();
	m_pStcName = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	m_pEditAmount = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	m_pEditAmount->AddWndStyle(EBS_NUMBER);
	m_pEditAggro = (CWndEdit*)GetDlgItem(WIDC_EDIT2);
	m_pEditAggro->AddWndStyle(EBS_NUMBER);
}
BOOL CWndWikiMobCreate::OnChildNotify(UINT message, UINT nID, LRESULT *pLResult)
{
	CWndWikiMob* pWndWikiMob = (CWndWikiMob*)GetWndBase(APP_WIKIPEDIA_MONSTER);
	if( nID == WIDC_BUTTON1 )
	{
		if( m_pMoverProp )
		{
			int nSize = atoi(m_pEditAmount->GetString());
			int nAggro = atoi(m_pEditAggro->GetString());
			if( (nSize > 0 && nSize <= 99 ) && (nAggro == 0 || nAggro == 1))
			{
				CString str;
				str.Format( "/cn %d %d %d", m_pMoverProp->dwID, nSize, nAggro);
				g_DPlay.SendChat( str );
				Destroy();
				pWndWikiMob->Destroy();
			}
		}
	}
	else if( nID == WIDC_BUTTON2 )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message,nID,pLResult);
}
void CWndWikiMobCreate::SetMoverProp(MoverProp* pMoverPropCreate)
{
	m_pMoverProp = pMoverPropCreate;
	if( m_pMoverProp )
		m_pStcName->SetTitle(m_pMoverProp->szName);
	else
		m_pStcName->SetTitle("");
}
#endif //__WIKIPEDIA_MONSTER
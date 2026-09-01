
#include "StdAfx.h"
#include "WndControl_ListBox.h"
#ifdef __ZCK_WIKI
#include "WndWikiItems.h"
#endif // __ZCK_WIKI
#ifdef __ZCK_TELEPORTEUR
#include "../_Common/ZckTeleporteur.h"
#include "../_Common/FLItemElem.h"
#endif // __ZCK_TELEPORTEUR
#include "../Neuz/CooldownTimer.h"

using namespace WndListBox;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndListBox
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndListBox::CWndListBox()
:	m_nLeftMargin( NULL )
,	m_pFocusItem( NULL )
,	m_nCurSelect( -1 )
,	m_nLineSpace( 0 )
,	m_bStyleState( FALSE )
,	m_bOnItem( FALSE )
{
	PFONTCOLOR_WNDLISTBOX pFontColorWndListBox = g_WndFontColorManager->GetWndListBox();
	
	m_nSelectColor		= pFontColorWndListBox->m_stSelectColor.GetFontColor();
	m_dwOnMouseColor	= pFontColorWndListBox->m_stMouseOverColor.GetFontColor();
	m_dwInvalidColor	= pFontColorWndListBox->m_stInvalidColor.GetFontColor();

	m_strTexture		= DEF_CTRL_TEXT;

	m_byWndType			= WTYPE_LISTBOX;
	m_bTile				= TRUE;
}

CWndListBox::~CWndListBox()
{
	ResetContent();
}

BOOL CWndListBox::Create( DWORD dwListBoxStyle, RECT& rect, CWndBase* pParentWnd, UINT nID )
{
	return CWndBase::Create( dwListBoxStyle | WBS_CHILD, rect, pParentWnd, nID );
}

BOOL CWndListBox::Create( DWORD dwListBoxStyle, RECT& rect, CWndBase* pParentWnd, UINT nID, int nR, int nG, int nB )
{
	SetFontColor( nR, nG, nB );
	return CWndBase::Create( dwListBoxStyle | WBS_CHILD, rect, pParentWnd, nID );
}

void CWndListBox::LoadListBoxScript(LPCTSTR lpFileName) 
{
	CScanner scanner;
 	if(scanner.Load(lpFileName) == FALSE)
		return;
	scanner.GetToken(); // keyword
	do {
		InterpriteScript(scanner,m_listItemArray);
		scanner.GetToken(); // keyword
	} while(scanner.tok != FINISHED);

}
void CWndListBox::InterpriteScript(CScanner& scanner,CPtrArray& ptrArray) 
{
	do {
		LPLISTITEM lpListItem = new LISTITEM;
		lpListItem->m_strWord = scanner.Token;
		ptrArray.Add(lpListItem);
		scanner.GetToken(); 
	} while(*scanner.token != '}' && scanner.tok != FINISHED);
	if(scanner.tok == FINISHED)
		return;
	scanner.GetToken(); 
}

void CWndListBox::OnInitialUpdate()
{
	CRect rect = GetWindowRect();

	m_wndScrollBar.Create( WBS_DOCKING | WBS_VERT, rect, this, 1000 );
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );
}

void CWndListBox::OnDraw(C2DRender* p2DRender)
{
	CWndBase* pWnd = GetParentWnd();
#ifdef __ZCK_TELEPORTEUR
	if( pWnd != NULL && pWnd->GetWndId() == APP_ZCK_TELEPORTEUR && GetWndId() == WIDC_LISTBOX1 )
	{
		CWndZckTeleporteur* pWndTeleporteur = static_cast<CWndZckTeleporteur*>( pWnd );
		const std::vector<DWORD>* pItems = pWndTeleporteur->GetCurrentItems();
		if( pItems == NULL )
			return;

		m_nFontHeight = 40;
		CPoint ptItem( 4, 3 );
		const CRect rectClient = GetClientRect();
		const int nPage = max( 1, rectClient.Height() / m_nFontHeight );

		if( IsWndStyle( WBS_VSCROLL ) )
		{
			m_wndScrollBar.SetVisible( pItems->size() > static_cast<size_t>( nPage ) ? TRUE : FALSE );
			m_wndScrollBar.SetScrollRange( 0, static_cast<int>( pItems->size() ) );
			m_wndScrollBar.SetScrollPage( nPage );
		}

		const size_t nBegin = static_cast<size_t>( m_wndScrollBar.GetScrollPos() );
		const size_t nEnd = min( pItems->size(), nBegin + static_cast<size_t>( nPage + 1 ) );
		for( size_t i = nBegin; i < nEnd; ++i )
		{
			const PT_ITEM_SPEC pItemSpec = g_xSpecManager->GetSpecItem( pItems->at( i ) );
			if( pItemSpec == NULL )
				continue;

			if( static_cast<int>( i ) == m_nCurSelect )
				p2DRender->RenderFillRect( CRect( 0, ptItem.y - 2, rectClient.right, ptItem.y + 36 ), m_nSelectColor );

			CString strIcon = pItemSpec->szIcon;
			if( pItemSpec->dwItemKind3 == IK3_EGG && pItemSpec->dwID != ITEM_INDEX( 21029, II_PET_EGG ) )
				strIcon.Replace( _T( "." ), _T( "_00." ) );

			if( strIcon.IsEmpty() == FALSE )
			{
				CTexture* pIcon = CWndBase::m_textureMng.AddTexture( p2DRender->m_pd3dDevice, MakePath( DIR_ITEM, strIcon ), 0xffff00ff );
				if( pIcon != NULL )
					pIcon->Render( p2DRender, ptItem );
			}

			CRect rectToolTip( ptItem.x, ptItem.y, ptItem.x + 35, ptItem.y + 35 );
			if( rectToolTip.PtInRect( m_ptMouse ) )
			{
				CPoint ptToolTip = m_ptMouse;
				ClientToScreen( &ptToolTip );
				ClientToScreen( &rectToolTip );
				FLItemElem itemElem;
				itemElem.m_dwItemId = pItemSpec->dwID;
				g_WndMng.PutToolTip_Item( static_cast<FLItemBase*>( &itemElem ), ptToolTip, &rectToolTip, APP_ZCK_TELEPORTEUR );
			}

			CString strName = pItemSpec->szName;
			const int nTextWidth = max( 20, rectClient.Width() - 48 );
			const CString strEllipsis = _T( "..." );
			while( strName.IsEmpty() == FALSE && p2DRender->m_pFont->GetTextExtent( strName ).cx > nTextWidth )
				strName = strName.Left( strName.GetLength() - 1 );
			if( strName != pItemSpec->szName )
				strName += strEllipsis;

			p2DRender->TextOut( ptItem.x + 40, ptItem.y + 10, strName, m_dwColor );
			ptItem.y += m_nFontHeight;
		}
		return;
	}
#endif // __ZCK_TELEPORTEUR
#ifdef __ZCK_WIKI
	if( pWnd != NULL && pWnd->GetWndId() == APP_WIKI_ITEMS )
	{
		CWndWikiItems* pWndWiki = static_cast<CWndWikiItems*>( pWnd );
		std::vector<PT_ITEM_SPEC>* pItems = pWndWiki->GetItemList();
		if( pItems == NULL )
			return;

		m_nFontHeight = 40;
		CPoint ptWiki( 10, 3 );
		CRect rectClient = GetClientRect();
		const int nPage = rectClient.Height() / m_nFontHeight;

		if( IsWndStyle( WBS_VSCROLL ) )
		{
			m_wndScrollBar.SetVisible( pItems->size() > static_cast<size_t>( nPage ) ? TRUE : FALSE );
			m_wndScrollBar.SetScrollRange( 0, static_cast<int>( pItems->size() ) );
			m_wndScrollBar.SetScrollPage( nPage );
		}

		const size_t nBegin = static_cast<size_t>( m_wndScrollBar.GetScrollPos() );
		const size_t nEnd = min( pItems->size(), nBegin + static_cast<size_t>( nPage + 1 ) );
		for( size_t i = nBegin; i < nEnd; ++i )
		{
			PT_ITEM_SPEC pItemSpec = pItems->at( i );
			if( pItemSpec == NULL )
				continue;

				const DWORD dwColor = 0xFFFF0000;
				p2DRender->RenderLine( CPoint( -2, ptWiki.y + 37 ), CPoint( rectClient.right - 3, ptWiki.y + 37 ), dwColor );
				if( static_cast<int>( i ) == m_nCurSelect )
				p2DRender->RenderFillRect( CRect( 0, ptWiki.y - 2, rectClient.right, ptWiki.y + 36 ), 0xFFbef5b0 );

			if( _tcslen( pItemSpec->szIcon ) > 0 )
			{
				CString strIcon = pItemSpec->szIcon;
				if( pItemSpec->dwItemKind3 == IK3_EGG && pItemSpec->dwID != ITEM_INDEX( 21029, II_PET_EGG ) )
					strIcon.Replace( _T( "." ), _T( "_00." ) );

				CTexture* pIcon = CWndBase::m_textureMng.AddTexture( p2DRender->m_pd3dDevice, MakePath( DIR_ITEM, strIcon ), 0xffff00ff );
				if( pIcon != NULL )
					pIcon->Render( p2DRender, ptWiki );
			}

			CRect rectToolTip( ptWiki.x, ptWiki.y, ptWiki.x + 35, ptWiki.y + 35 );
			if( rectToolTip.PtInRect( m_ptMouse ) )
			{
				CPoint ptToolTip = m_ptMouse;
				ClientToScreen( &ptToolTip );
				ClientToScreen( &rectToolTip );
				FLItemElem itemElem;
				itemElem.m_dwItemId = pItemSpec->dwID;
				g_WndMng.PutToolTip_Item( &itemElem, ptToolTip, &rectToolTip, APP_WIKI_ITEMS );
			}

			ptWiki.x += 40;
			ptWiki.y += 10;
			const int nNameColumnWidth = 235;
			CString strName = pItemSpec->szName;
			if( pItemSpec->dwItemJob != NULL_ID && p2DRender->m_pFont->GetTextExtent( strName ).cx > nNameColumnWidth )
			{
				const CString strEllipsis = _T( "..." );
				while( !strName.IsEmpty() && p2DRender->m_pFont->GetTextExtent( strName + strEllipsis ).cx > nNameColumnWidth )
					strName = strName.Left( strName.GetLength() - 1 );
				strName += strEllipsis;
			}
			p2DRender->TextOut( ptWiki.x, ptWiki.y, strName, dwColor );

			ptWiki.x += nNameColumnWidth;
			if( pItemSpec->dwItemJob < MAX_JOB )
				p2DRender->TextOut( ptWiki.x, ptWiki.y, prj.m_aJob[pItemSpec->dwItemJob].szName, dwColor );

			const int nLevel = ( pItemSpec->nMinLimitLevel <= 0 ) ? 1 : pItemSpec->nMinLimitLevel;
			CString strLevel;
			strLevel.Format( _T( "%d" ), nLevel );
			ptWiki.x += 129 - strLevel.GetLength() * 4;
			p2DRender->TextOut( ptWiki.x, ptWiki.y, strLevel, dwColor );

			ptWiki.x = 10;
			ptWiki.y += m_nFontHeight - 10;
		}
		return;
	}
#endif // __ZCK_WIKI

#ifdef __WIKIPEDIA_MONSTER
	if (pWnd->GetWndId() == APP_WIKIPEDIA_MONSTER)
	{
		CWndWikiMob* pWndWikiMob = dynamic_cast<CWndWikiMob*>(pWnd);
		if (!pWndWikiMob)
			return; // Ensure pWndWikiMob is not null
		m_nFontHeight = 40;
		CPoint pt(10, 3);
		CTexture* m_pPicTexture = nullptr; // Initialize m_pPicTexture to nullptr
		CString strFile;

		CRect rectClient = GetClientRect();
		int nPage = rectClient.Height() / m_nFontHeight;

		for (size_t i = 0; i < pWndWikiMob->GetMobsList()->size(); ++i)
		{
			MoverProp* pMoverProp = pWndWikiMob->GetMobsList()->at(i);
			if (i < static_cast<size_t>(m_wndScrollBar.GetScrollPos()))
				continue;
			if (i > static_cast<size_t>(m_wndScrollBar.GetScrollPos() + nPage))
				break;

			CString strName;
			CString strLevel;
			CString strHp;
			CString strAtk;
			p2DRender->RenderLine(CPoint(-2, pt.y + 37), CPoint(rectClient.right, pt.y + 37), 0xFF000000);
			pt.y += 8;
			strName.Format("%s", pMoverProp->szName);
			p2DRender->TextOut(pt.x, pt.y, strName, 0xffffffff);
			pt.x += 200;
			strLevel.Format("%d", pMoverProp->dwLevel);
			p2DRender->TextOut(pt.x, pt.y, strLevel, 0xffffffff);
			pt.x += 100;
			strHp.Format("%d", pMoverProp->dwAddHp);
			p2DRender->TextOut(pt.x, pt.y, strHp, 0xffffffff);
			pt.x += 105;
			strAtk.Format("%d~%d", pMoverProp->dwAtkMin, pMoverProp->dwAtkMax);
			p2DRender->TextOut(pt.x, pt.y, strAtk, 0xffffffff);
			switch (pMoverProp->eElementType)
			{
			case SAI79::FIRE:
				strFile = "Feu.tga";
				break;
			case SAI79::WATER:
				strFile = "Eau.tga";
				break;
			case SAI79::ELECTRICITY:
				strFile = "Electrique.tga";
				break;
			case SAI79::WIND:
				strFile = "Vent.tga";
				break;
			case SAI79::EARTH:
				strFile = "Terre.tga";
				break;
			default:
				strFile = "Neutre.tga";
				break;
			}
			pt.x += 150;
			m_pPicTexture = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, strFile), 0xffff00ff);
			if (m_pPicTexture != nullptr)
			{
				if (g_Option.m_nWindowAlpha > 200)
					m_pPicTexture->Render(p2DRender, pt, g_Option.m_nWindowAlpha - 55);
				else
					m_pPicTexture->Render(p2DRender, pt, g_Option.m_nWindowAlpha);
			}
			pt.x = 8;
			pt.y += m_nFontHeight - 10;
		}
		m_wndScrollBar.SetScrollRange(0, static_cast<int>(pWndWikiMob->GetMobsList()->size()));
		m_wndScrollBar.SetScrollPage(nPage);
	}
#endif // __WIKI_MOB


	int nHeight = 0;

#ifdef __COOLDOWN_TIMER
	if (pWnd->GetWndId() == APP_TIMER)
	{
		m_nFontHeight = 20;
		CPoint pt(10, 13);
		DWORD dwNext;
		CTimeSpan tsNext;

		CSize s = p2DRender->m_pFont->GetTextExtent("Zy");
		int nGapY = m_nFontHeight / 2 - s.cy / 2;
		p2DRender->TextOut(pt.x, pt.y + nGapY, "Next GuildWar", 0xffffffff);
		pt.x += 100;
		dwNext = CCoolTimeMgr::Instance()->GetNextGuildWarTime() - g_tmCurrent;
		if ((int)dwNext < 0)
			dwNext = 0;
		tsNext = CTimeSpan(dwNext / 1000);
		p2DRender->TextOut(pt.x, pt.y + nGapY, tsNext.Format("%D:%H:%M:%S"), 0xffffffff);

		DWORD dwWorldId;
		HANDLE hFindDungeon = CCoolTimeMgr::Instance()->FirstDCD(&dwWorldId, &dwNext);
		if (SUCCEEDED(hFindDungeon))
		{
			do {
				dwNext -= g_tmCurrent;
				if ((int)dwNext >= 0)
				{
					pt.x = 10;
					pt.y += m_nFontHeight;

					p2DRender->TextOut(pt.x, pt.y + nGapY, g_WorldMng.GetWorldStruct(dwWorldId)->m_szWorldName, 0xffffffff);;
					pt.x += 200;
					tsNext = CTimeSpan(dwNext / 1000);
					p2DRender->TextOut(pt.x, pt.y + nGapY, tsNext.Format("%D:%H:%M:%S"), 0xffffffff);
				}
			} while (CCoolTimeMgr::Instance()->NextDCD(hFindDungeon, &dwWorldId, &dwNext));

			CCoolTimeMgr::Instance()->FindClose(hFindDungeon);
		}
	}
	else
#endif // __COOLDOWN_TIMER

	if(m_bStyleState)
	{
		nHeight = 5;
	}
	else 
	{
		nHeight = 0;
	}

	m_nFontHeight = GetFontHeight() + m_nLineSpace + nHeight;
	
	{
		CPoint pt(TEXTOUT_OFFSET, TEXTOUT_OFFSET);
		pt.y -= m_nFontHeight * m_wndScrollBar.GetScrollPos();
		PaintListBox(p2DRender,pt,m_listItemArray);

		int nPage = GetClientRect().Height() / m_nFontHeight;
		int nRange = m_listItemArray.GetSize();
		if(	IsWndStyle( WBS_VSCROLL ) )
		{
			int nMinimumHeight = m_wndScrollBar.GetArrowUpButtomTextureSize().cy + m_wndScrollBar.GetArrowDownButtomTextureSize().cy;
			int nListBoxHeight = GetClientRect().Height();
			
			if( nMinimumHeight > nListBoxHeight ||
				nRange < 2 )
			{
				m_wndScrollBar.SetVisible( FALSE );
			}
			else
			{
				m_wndScrollBar.SetVisible( TRUE );
				m_wndScrollBar.SetScrollRange( 0, nRange );
				m_wndScrollBar.SetScrollPage( nPage );
			}
		}
		else
			m_wndScrollBar.SetVisible( FALSE );
	}

}

void  CWndListBox::SetStyleState(BOOL bState,DWORD dwStyle)
{
	m_bStyleState = bState;
	m_dwFontStyle = dwStyle;
}

void CWndListBox::PaintListBox(C2DRender* p2DRender,CPoint& pt,CPtrArray& ptrArray) 
{
	LPLISTITEM pListItem;
	for(int i = 0; i < ptrArray.GetSize(); i++)
	{
		pListItem = (LPLISTITEM)ptrArray.GetAt(i);
		if( pListItem->m_bIsVisible == FALSE )
			continue;

		int nScrollBarWidth = IsWndStyle( WBS_VSCROLL ) ? m_wndScrollBar.GetClientRect().Width() : 0;
		CSize size = p2DRender->m_pFont->GetTextExtent_EditString( pListItem->m_strWord );
		pListItem->m_rect.left = pt.x;
		pListItem->m_rect.top = pt.y;
		pListItem->m_rect.right = pt.x + m_rectWindow.Width() - nScrollBarWidth;
		pListItem->m_rect.bottom = pt.y + m_nFontHeight;
		if( pListItem->m_bIsValid == TRUE )
		{
			if( i == m_nCurSelect )
			{
				pListItem->m_strWord.SetColor( m_nSelectColor );
				if(m_bStyleState)
				{
					pListItem->m_strWord.SetStyle(m_dwFontStyle);
				}
			}
			else
			{
				if(m_bStyleState)
				{
					pListItem->m_strWord.ClearStyle(m_dwFontStyle);
				}

				CPoint point = GetMousePoint();
				if( pListItem->m_rect.PtInRect( point ) == TRUE )
					pListItem->m_strWord.SetColor( m_dwOnMouseColor );
				else
					pListItem->m_strWord.SetColor( m_dwColor );
			}
		}
		else
			pListItem->m_strWord.SetColor( m_dwInvalidColor );
		p2DRender->TextOut_EditString( m_nLeftMargin + pt.x, pt.y, pListItem->m_strWord );
		pt.y += m_nFontHeight;
	}
}
void CWndListBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWndBase* pWnd = GetParentWnd();
	{
		CPoint pt(3,3);
		pt.y -= m_nFontHeight * m_wndScrollBar.GetScrollPos();
		CRect rect;
		LPLISTITEM pListItem;
		for(int i = 0; i < m_listItemArray.GetSize(); i++)
		{
			pListItem = (LPLISTITEM)m_listItemArray.GetAt(i);

			if( pListItem->m_bIsValid == FALSE )
			{
				pt.y += m_nFontHeight;
				continue;
			}

			int nScrollBarWidth = IsWndStyle( WBS_VSCROLL ) ? m_wndScrollBar.GetClientRect().Width() : 0;
			rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - nScrollBarWidth, pt.y + m_nFontHeight );
			if(rect.PtInRect(point))
			{
				if(m_pFocusItem == pListItem)
				{
					// 부모가 차일드 윈도가 아니어야 OnCommand 메시지를 받는다.
					CWndBase* pWnd = m_pParentWnd;
					pWnd->OnChildNotify(WNM_SELCHANGE,m_nIdWnd,(LRESULT*)pListItem);
					return;
				}
			}
			pt.y += m_nFontHeight;
		}
	}
}
void CWndListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	CPoint pt(3,3);
	pt.y -= m_nFontHeight * m_wndScrollBar.GetScrollPos();
	CRect rect;
	LPLISTITEM pListItem;
	for(int i = 0; i < m_listItemArray.GetSize(); i++)
	{
		pListItem = (LPLISTITEM)m_listItemArray.GetAt(i);
		if( pListItem->m_bIsValid == FALSE )
		{
			pt.y += m_nFontHeight;
			continue;
		}

		int nScrollBarWidth = IsWndStyle( WBS_VSCROLL ) ? m_wndScrollBar.GetClientRect().Width() : 0;
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - nScrollBarWidth, pt.y + m_nFontHeight );
		if(rect.PtInRect(point))
		{
			m_nCurSelect = i;
			m_pFocusItem = pListItem;
		}
		pt.y += m_nFontHeight;
	}
}
void CWndListBox::OnRButtonUp(UINT nFlags, CPoint point)
{
	CPoint pt(3,3);
	pt.y -= m_nFontHeight * m_wndScrollBar.GetScrollPos();
	CRect rect;
	LPLISTITEM pListItem;
	for(int i = 0; i < m_listItemArray.GetSize(); i++)
	{
		pListItem = (LPLISTITEM)m_listItemArray.GetAt(i);
		if( pListItem->m_bIsValid == FALSE )
		{
			pt.y += m_nFontHeight;
			continue;
		}

		int nScrollBarWidth = IsWndStyle( WBS_VSCROLL ) ? m_wndScrollBar.GetClientRect().Width() : 0;
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - nScrollBarWidth, pt.y + m_nFontHeight );
		if(rect.PtInRect(point))
		{
//			if(m_pFocusItem == pListItem)
			m_nCurSelect = i;
			m_pFocusItem = pListItem;		//오른쪽 클릭의 경우 ( 선택 + 행동 ), by gmpbigsun
			if( pListItem )
			{
				// 부모가 차일드 윈도가 아니어야 OnCommand 메시지를 받는다.
				CWndBase* pWnd = m_pParentWnd;
				pListItem->m_rect.left = point.x;
				pListItem->m_rect.top = point.y;				
				//while(pWnd->GetStyle() & WBS_CHILD)
				//	pWnd = pWnd->GetParentWnd();
				pWnd->OnChildNotify(WNM_SELCANCEL,m_nIdWnd,(LRESULT*)pListItem);//m_pFocusItem); 
				return;
			}
		}
		pt.y += m_nFontHeight;
	}
}
void CWndListBox::OnRButtonDown(UINT nFlags, CPoint point)
{
}
void CWndListBox::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	CPoint pt(3,3);
	pt.y -= m_nFontHeight * m_wndScrollBar.GetScrollPos();
	CRect rect;
	LPLISTITEM pListItem;
	for(int i = 0; i < m_listItemArray.GetSize(); i++)
	{
		pListItem = (LPLISTITEM)m_listItemArray.GetAt(i);
		int nScrollBarWidth = IsWndStyle( WBS_VSCROLL ) ? m_wndScrollBar.GetClientRect().Width() : 0;
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - nScrollBarWidth, pt.y + m_nFontHeight );
		if(rect.PtInRect(point))
		{
			if(m_pFocusItem == pListItem)
			{
				// 부모가 차일드 윈도가 아니어야 OnCommand 메시지를 받는다.
				CWndBase* pWnd = m_pParentWnd;
				while(pWnd->GetStyle() & WBS_CHILD)
				{
					//직부모에게 알리고 by gmpbigsun(20110906)
					pWnd->OnChildNotify(WNM_DBLCLK,m_nIdWnd,(LRESULT*)m_pFocusItem); 

					pWnd = pWnd->GetParentWnd();
					if( NULL == pWnd )
						break;
				}

				//최종 부모에게 알린다.
				if( pWnd )
					pWnd->OnChildNotify(WNM_DBLCLK,m_nIdWnd,(LRESULT*)m_pFocusItem); 

				return;
			}
		}
		pt.y += m_nFontHeight;
	}
}
void CWndListBox::OnRButtonDblClk( UINT nFlags, CPoint point)
{
}
BOOL CWndListBox::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	if( m_wndScrollBar.GetScrollPage() >= m_wndScrollBar.GetMaxScrollPos() )
		return TRUE;

	if( zDelta < 0 )
	{
		if( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() > m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()+1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() );
	}
	else
	{
		if( m_wndScrollBar.GetMinScrollPos() < m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()-1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMinScrollPos() );
	}
	
	return TRUE;
}

void CWndListBox::OnMouseWndSurface( CPoint point )
{
	if( FALSE == m_bOnItem )
		return;

	CPoint pt(3,3);
	pt.y -= m_nFontHeight * m_wndScrollBar.GetScrollPos();
	CRect rect;
	LPLISTITEM pListItem;
	for(int i = 0; i < m_listItemArray.GetSize(); i++)
	{
		pListItem = (LPLISTITEM)m_listItemArray.GetAt(i);
		if( pListItem->m_bIsValid == FALSE )
		{
			pt.y += m_nFontHeight;
			continue;
		}

		int nScrollBarWidth = IsWndStyle( WBS_VSCROLL ) ? m_wndScrollBar.GetClientRect().Width() : 0;
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - nScrollBarWidth, pt.y + m_nFontHeight );
		if(rect.PtInRect(point))
		{
			m_pParentWnd->OnChildNotify( WNM_ONITEM, m_nIdWnd, (LRESULT*)&i );
			return;
		}

		pt.y += m_nFontHeight;
	}
}

void CWndListBox::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_pParentWnd )
		m_pParentWnd->OnMouseMove( nFlags, point );
}

int CWndListBox::GetCount() const
{
	return m_listItemArray.GetSize();
}
DWORD CWndListBox::GetItemData(int nIndex) const
{
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	return lpListItem->m_dwData;
}

void* CWndListBox::GetItemDataPtr(int nIndex) const
{
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	return (void*)lpListItem->m_dwData;
}

int CWndListBox::SetItemData(int nIndex,DWORD dwItemData)
{
	if(nIndex < 0 || nIndex >= m_listItemArray.GetSize())
		return LB_ERR;
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	lpListItem->m_dwData = dwItemData;
	return 0;
}

int CWndListBox::SetItemDataPtr(int nIndex,void* pData)
{
	if(nIndex < 0 || nIndex >= m_listItemArray.GetSize())
		return LB_ERR;
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	lpListItem->m_dwData = (DWORD) pData;
	return 0;
}

DWORD CWndListBox::GetItemData2( int nIndex ) const
{
	LPLISTITEM lpListItem = ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
	return lpListItem->m_dwData2;
}

void* CWndListBox::GetItemData2Ptr( int nIndex ) const
{
	LPLISTITEM lpListItem = ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
	return ( void* )lpListItem->m_dwData2;
}

BOOL CWndListBox::GetItemValidity( int nIndex )
{
	LPLISTITEM lpListItem = ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
	return lpListItem->m_bIsValid;
}

BOOL CWndListBox::GetItemVisibility( int nIndex )
{
	LPLISTITEM lpListItem = ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
	return lpListItem->m_bIsVisible;
}

int CWndListBox::SetItemData2( int nIndex,DWORD dwItemData )
{
	if( nIndex < 0 || nIndex >= m_listItemArray.GetSize() )
		return LB_ERR;
	LPLISTITEM lpListItem = ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
	lpListItem->m_dwData2 = dwItemData;
	return 0;
}

int CWndListBox::SetItemData2Ptr( int nIndex,void* pData )
{
	if( nIndex < 0 || nIndex >= m_listItemArray.GetSize() )
		return LB_ERR;
	LPLISTITEM lpListItem = ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
	lpListItem->m_dwData2 = ( DWORD )pData;
	return 0;
}

int CWndListBox::SetItemValidity( int nIndex, BOOL bValidity )
{
	if( nIndex < 0 || nIndex >= m_listItemArray.GetSize() )
		return LB_ERR;
	LPLISTITEM lpListItem = ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
	lpListItem->m_bIsValid = bValidity;
	return 0;
}

int CWndListBox::SetItemVisibility( int nIndex, BOOL bIsVisible )
{
	if( nIndex < 0 || nIndex >= m_listItemArray.GetSize() )
		return LB_ERR;
	LPLISTITEM lpListItem = ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
	lpListItem->m_bIsVisible = bIsVisible;
	return 0;
}

const CRect& CWndListBox::GetItemRect( int nIndex ) const
{
	LPLISTITEM lpListItem = ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
	return lpListItem->m_rect;
}

int CWndListBox::GetSel(int nIndex) const
{
		return 1;
}

int CWndListBox::GetText(int nIndex,LPSTR lpszBuffer) const
{
		return 1;
}

void CWndListBox::GetText(int nIndex,CString& rString) const
{
	if(nIndex >= 0 && nIndex < m_listItemArray.GetSize())
	{
		LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
		rString = lpListItem->m_strWord;
	}
}

#ifdef __IMPROVE_MAP_SYSTEM
int CWndListBox::GetTextLen(int nIndex) const
{
	if(nIndex >= 0 && nIndex < m_listItemArray.GetSize())
	{
		LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
		return lpListItem->m_strWord.GetLength();
	}
	return 0;
}
#else // __IMPROVE_MAP_SYSTEM
int CWndListBox::GetTextLen(int nIndex) const
{
		return 1;
}
#endif // __IMPROVE_MAP_SYSTEM


int CWndListBox::GetCurSel() const
{
	return m_nCurSelect;
}

int CWndListBox::SetCurSel(int nSelect)
{
	if( -1 == nSelect )
	{
		m_nCurSelect = nSelect; 
		return 0; //선택안함.
	}

	if(nSelect >= 0 && nSelect < m_listItemArray.GetSize())
	{	
		m_pFocusItem = (LPLISTITEM)m_listItemArray.GetAt(nSelect);
		m_nCurSelect = nSelect;
		return m_nCurSelect;
	}

	FLERROR_LOG( PROGRAM_NAME, _T( "범위 넘어섬 %d" ), nSelect );

	return 0;
}

int CWndListBox::SetSel(int nIndex,BOOL bSelect)
{
		return 1;
}

int CWndListBox::GetSelCount() const
{
		return 1;
}

int CWndListBox::GetSelItems(int nMaxItems,LPINT rgIndex) const
{
		return 1;
}


int CWndListBox::AddString(LPCTSTR lpszItem)
{
	LPLISTITEM lpListItem = new LISTITEM;
	lpListItem->m_strWord = lpszItem;
	lpListItem->m_strWord.Init( m_pFont, &GetClientRect() );
	lpListItem->m_strWord.SetParsingString( lpszItem, m_dwColor, 0x00000000, 0, 0x00000001, TRUE );
	m_listItemArray.Add(lpListItem);
	return m_listItemArray.GetSize()-1;
}

int CWndListBox::AddString(LPCTSTR lpszItem, DWORD color, DWORD style )
{
	LPLISTITEM lpListItem = new LISTITEM;
	lpListItem->m_strWord = lpszItem;
	lpListItem->m_strWord.Init( m_pFont, &GetClientRect() );
	lpListItem->m_strWord.SetParsingString( lpszItem, color, style, 0, 0x00000001, TRUE );
	m_listItemArray.Add(lpListItem);
	return m_listItemArray.GetSize()-1;
}

int CWndListBox::DeleteString(UINT nIndex)
{
	SAFE_DELETE_CONST( (LPLISTITEM)m_listItemArray.GetAt(nIndex) );
	m_listItemArray.RemoveAt(nIndex);
	if(nIndex == m_nCurSelect)
	{
		m_nCurSelect = -1;
		m_pFocusItem = NULL;
	}
	return 1;
}

void CWndListBox::SetString( int nIndex, LPCTSTR lpszItem )
{
	LPLISTITEM lpListItem	= (LPLISTITEM)m_listItemArray.GetAt( nIndex );
	lpListItem->m_strWord	= lpszItem;
}

const CString& CWndListBox::GetString( int nIndex ) const
{
	LPLISTITEM lpListItem	= ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
	return lpListItem->m_strWord;
}

void CWndListBox::SetListStringAlpha( int nIndex, BYTE byAlpha )
{
	LPLISTITEM lpListItem	= ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
	lpListItem->m_strWord.SetAlpha( byAlpha );
}

void CWndListBox::SetKeyString( int nIndex, LPCTSTR lpszItem )
{
	LPLISTITEM lpListItem	= ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
	lpListItem->m_strKey	= lpszItem;
}

const CString& CWndListBox::GetKeyString( int nIndex ) const
{
	LPLISTITEM lpListItem	= ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
	return lpListItem->m_strKey;
}

void CWndListBox::SetOnMouseColor( DWORD dwOnMouseColor )
{
	m_dwOnMouseColor = dwOnMouseColor;
}

DWORD CWndListBox::GetOnMouseColor( void ) const
{
	return m_dwOnMouseColor;
}

void CWndListBox::SetOnSelectColor( DWORD dwOnSelectColor )
{
	m_nSelectColor = dwOnSelectColor;
}
void CWndListBox::SetInvalidColor( DWORD dwInvalidColor )
{
	m_dwInvalidColor = dwInvalidColor;
}

DWORD CWndListBox::GetInvalidColor( void ) const
{
	return m_dwInvalidColor;
}

void CWndListBox::SetLeftMargin( int nLeftMargin )
{
	m_nLeftMargin = nLeftMargin;
}

int CWndListBox::GetLeftMargin( void ) const
{
	return m_nLeftMargin;
}

#ifdef __IMPROVE_MAP_SYSTEM
int CWndListBox::GetItemIndex( const CString& strItem ) const
{
	for( int nIndex = 0; nIndex < m_listItemArray.GetSize(); ++nIndex )
	{
		LPLISTITEM lpListItem = ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
		if( lpListItem == NULL )
		{
			continue;
		}

		if( strcmp( strItem, lpListItem->m_strWord ) == 0 )
		{
			return nIndex;
		}
	}

	return -1;
}

int CWndListBox::GetItemIndex( DWORD dwItem ) const
{
	for( int nIndex = 0; nIndex < m_listItemArray.GetSize(); ++nIndex )
	{
		LPLISTITEM lpListItem = ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
		if( lpListItem == NULL )
		{
			continue;
		}

		if( dwItem == lpListItem->m_dwData )
		{
			return nIndex;
		}
	}

	return -1;
}
#endif // __IMPROVE_MAP_SYSTEM

int CWndListBox::InsertString(int nIndex,LPCTSTR lpszItem)
{
	FLASSERT( 0 && "Not supported yet" );
	return 1;
} 

void CWndListBox::ResetContent()
{
	for(int i = 0; i < m_listItemArray.GetSize(); i++)
		SAFE_DELETE_CONST( (LPLISTITEM)m_listItemArray.GetAt(i) );
	m_nCurSelect = -1;
	m_pFocusItem = NULL;
	m_listItemArray.RemoveAll();
	m_wndScrollBar.SetScrollPos( 0, FALSE );
}

int CWndListBox::FindString( int nStartAfter, LPCTSTR lpszItem ) const
{
	if( m_listItemArray.GetSize() == 0 )
		return -1;
	LPLISTITEM lpListItem;
	int nCmp = 0;
	int nBegin = 0, nEnd = m_listItemArray.GetSize() - 1;
	int nDiv = nBegin + ( ( nEnd - nBegin ) / 2 );
	do
	{
		if( nCmp ==  1 ) nEnd = nDiv;
		if( nCmp == -1 ) nBegin = nDiv;
		nDiv = nBegin + ( ( nEnd - nBegin ) / 2 );
		lpListItem = (LPLISTITEM)m_listItemArray.GetAt( nDiv );
		nCmp = strcmp( lpListItem->m_strWord, lpszItem );
		if( nCmp ==  0 ) return nDiv;
	}
	while( ( nEnd - nBegin ) > 1 );
	int nResult;
	lpListItem = (LPLISTITEM)m_listItemArray.GetAt( nBegin );
	nCmp = strcmp( lpListItem->m_strWord, lpszItem );
	if( nCmp >= 0 ) 
		nResult = nBegin;
	else
	{
		lpListItem = (LPLISTITEM)m_listItemArray.GetAt( nEnd );
		nResult = nEnd;
	}
	if( lpszItem[ 0 ] != lpListItem->m_strWord[ 0 ] )
		nResult = -1;
	return nResult;
}
/*

input d 

0 5 = 2
2 5 = 3
2 3 = 2

0 a
1 b
2 c
3 f
4 f
5 f

0 5 = 2
2 5 = 3
3 5 = 4
3 4 = 3

0 a
1 b
2 c
3 c
4 f
5 f
*/
int QSortListBox( const VOID* arg1, const VOID* arg2 )
{
	//return _stricmp( * ( char** ) arg1, * ( char** ) arg2 );
	LPCTSTR pSrc = (*(CWndListBox::LISTITEM**)arg1)->m_strWord;
    LPCTSTR pDst = (*(CWndListBox::LISTITEM**)arg2)->m_strWord;

	return _stricmp( pSrc, pDst );
}
void CWndListBox::SortListBox() 
{
    qsort( m_listItemArray.GetData(), m_listItemArray.GetSize(), sizeof(LPLISTITEM), QSortListBox ); 
}

BOOL CWndListBox::IsInValidArea( CPoint point )
{
	CPoint pt(3,3);
	pt.y -= m_nFontHeight * m_wndScrollBar.GetScrollPos();
	CRect rect;
	LPLISTITEM pListItem;
	for(int i = 0; i < m_listItemArray.GetSize(); i++)
	{
		pListItem = (LPLISTITEM)m_listItemArray.GetAt(i);
		int nScrollBarWidth = IsWndStyle( WBS_VSCROLL ) ? m_wndScrollBar.GetClientRect().Width() : 0;
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - nScrollBarWidth, pt.y + m_nFontHeight );
		if(rect.PtInRect(point))
		{
			return TRUE;
		}
		pt.y += m_nFontHeight;
	}

	return FALSE;
}


int CWndListBox::FindStringExact(int nIndexStart,LPCTSTR lpszItem) const
{
	return 1;
}

int CWndListBox::SelectString(int nStartAfter,LPCTSTR lpszItem)
{
	m_nCurSelect = FindString( nStartAfter, lpszItem );
	if( m_nCurSelect == -1 )
		return -1;
	int nPos = m_wndScrollBar.GetScrollPos();
	int nRange = m_wndScrollBar.GetScrollRange();
	int nPage = m_wndScrollBar.GetScrollPage();
	if( ( m_nCurSelect < nPos || m_nCurSelect > nPos + nPage ) )
	{
		if( m_nCurSelect < nRange - nPage )
			m_wndScrollBar.SetScrollPos( m_nCurSelect < 0 ? 0 : m_nCurSelect );
		else
			m_wndScrollBar.SetScrollPos( nRange - nPage  );
	}
	return m_nCurSelect;
}
void CWndListBox::OnSize(UINT nType, int cx, int cy)
{
//	m_string.Reset( m_pFont, &GetClientRect() );

	CRect rect = GetWindowRect();
	if( IsWndStyle( WBS_VSCROLL ) ) 
		rect.left = rect.right - VSCROLL_WIDTH;
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );
	m_wndScrollBar.SetWndRect( rect );
	CWndBase::OnSize( nType, cx, cy);
}
void CWndListBox::SetWndRect(CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
//	m_rectClient.DeflateRect( 4, 4 );
	if( IsWndStyle( WBS_VSCROLL ) ) 
		m_rectClient.right -= 15;
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );

	if( bOnSize )
		OnSize(0,m_rectClient.Width(),m_rectClient.Height());
}
void CWndListBox::PaintFrame( C2DRender* p2DRender )
{
	CWndBase::PaintFrame( p2DRender );
}
BOOL CWndListBox::OnEraseBkgnd( C2DRender* p2DRender )
{
	return TRUE;
}
void CWndListBox::OnSetFocus( CWndBase* pOldWnd )
{
}


int CWndListBox::GetScrollWidth()
{
	if( IsWndStyle( WBS_VSCROLL ) )
		return m_wndScrollBar.GetClientRect().Width();

	return 0;
}



#pragma region WndComboListBox

//---------------------------------------------------------------------------------------------
// 윈도우 콤보 박스 리스트 컨트롤 클레스
//---------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------
// 생성자 
//---------------------------------------------------------------------------------------------
CWndComboListBox::CWndComboListBox()
{
	ZeroMemory( m_pTextureBackground, sizeof( CTexture* ) * WndListBox::MAX_BACKGROUND_TEXTRUE_COUNT );
}


//---------------------------------------------------------------------------------------------
// 파괴자 
//---------------------------------------------------------------------------------------------
CWndComboListBox::~CWndComboListBox()
{
}


//---------------------------------------------------------------------------------------------
// 배경 텍스쳐 로딩하기
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndComboListBox::LoadBackgroundTexture()
{
	CString strTexture( _T("") );

	for( UINT nIndex = 0; nIndex < WndListBox::MAX_BACKGROUND_TEXTRUE_COUNT; nIndex++ )
	{
		strTexture.Format( _T("WndEditTile%02d.tga"), nIndex );
		
		m_pTextureBackground[nIndex] = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),strTexture ), WNDCOLOR_DEFAULT_KEY, TRUE );
		FLERROR_ASSERT_LOG_DO( m_pTextureBackground[nIndex], continue, _T("[윈도우 콤보박스] 배경 텍스쳐 생성 실패 - [ %s ]."), strTexture );	
		strTexture = _T("");
	}	
}


#ifdef __IMPROVE_MAP_SYSTEM
//---------------------------------------------------------------------------------------------
// 그리기
// param	: p2DRender
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndComboListBox::PaintFrame( C2DRender* p2DRender )
{
	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		m_pTheme->RenderWndEditFrame( p2DRender, &GetWindowRect() );

	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		DrawBackground( p2DRender );
}
#endif 


//---------------------------------------------------------------------------------------------
// On Kill Focus
// param	: 새로운 윈도우
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndComboListBox::OnKillFocus( CWndBase* pNewWnd )
{
	m_pParentWnd->OnChildNotify( WNM_KILLFOCUS, m_nIdWnd, ( LRESULT* )this );
	CWndBase::OnKillFocus( pNewWnd );
}


//---------------------------------------------------------------------------------------------
// 스킨 텍스쳐 추가
// param	: 새로운 윈도우
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndComboListBox::AdditionalSkinTexture( LPWORD pDest, CSize sizeSurface, D3DFORMAT d3dFormat )
{
	return;
}


//---------------------------------------------------------------------------------------------
// 배경 그리기
// 19차로 인해 배경을 타일 형식으로 그릴때 윈도우 사이즈와 텍스쳐의 크기가 어긋나서
// 계산해서 따로 그려야 될 필요가 있어 이 함수를 사용한다.
// param	: 렌더러
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndComboListBox::DrawBackground( C2DRender* p2DRender )
{
	if( FALSE == m_bVisible )
		return ;

	CTexture* pTexture = m_pTextureBackground[0];
	FLERROR_ASSERT_LOG_RETURN_VOID( pTexture, _T("[윈도우 콤보 리스트 박스] 배경 텍스쳐 포인터가 유효하지 않음.") );	

	CRect rtRect = GetWindowRect();
	
	int nWidthCount( rtRect.Width() / pTexture->m_size.cx );
	int nHeightCount( (rtRect.Height() / pTexture->m_size.cy) + 1);
	int nRemainHeight( pTexture->m_size.cy - ( rtRect.Height() % pTexture->m_size.cy ) );

	CPoint ptLeftTop( rtRect.left, rtRect.top );
	for( int nHeight = 0; nHeight < nHeightCount; nHeight++ )
	{
		for( int nWidth = 0; nWidth < nWidthCount; nWidth++ )
		{
			if( nHeight == 0 )							// 맨 위에 줄 그리기
			{
				if( nWidth == 0 )
					m_pTextureBackground[ 0 ]->Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );
				else if( nWidth == (nWidthCount - 1) )
					m_pTextureBackground[ 2 ]->Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );
				else
					m_pTextureBackground[ 1 ]->Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );
			
			}
			else if( nHeight == (nHeightCount - 1) )	// 마지막 줄 그리기
			{
				if( nWidth == 0 )
					m_pTextureBackground[ 6 ]->Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );
				
				else if( nWidth == (nWidthCount - 1) )
					m_pTextureBackground[ 8 ]->Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );
				else
					m_pTextureBackground[ 7 ]->Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );
			}
			else if( nHeight == (nHeightCount - 2) )	// 마지막 바로 위에 줄 그리기	
			{
				// 윈도우 사이즈가 배경 텍스쳐 사이즈로 나누어서 떨어지지 않을 경우 
				// 맨 밑에줄과 그 바로 위에줄을 위로 올려 배경 텍스쳐의 사이즈를 맞춘다
				// 버그인가 + 4를 해야 사이즈가 맞는다... 젠장 추후 원인 파악해서 수정하겠음.
				if( nWidth == 0 )
				{
					p2DRender->RenderTextureUVEx(	ptLeftTop,
													CPoint( m_pTextureBackground[ 3 ]->m_size.cx, (m_pTextureBackground[ 3 ]->m_size.cy ) + 4 ), 
													m_pTextureBackground[ 3 ],
													0.0f,
													0.0f,
													1.0f,
													1.0f
												);
				}
				else if( nWidth == (nWidthCount - 1) )
				{
					p2DRender->RenderTextureUVEx(	ptLeftTop,
													CPoint( m_pTextureBackground[ 5 ]->m_size.cx, (m_pTextureBackground[ 5 ]->m_size.cy ) + 4 ), 
													m_pTextureBackground[ 5 ],
													0.0f,
													0.0f,
													1.0f,
													1.0f
												);
				}
				else
				{
					p2DRender->RenderTextureUVEx(	ptLeftTop,
													CPoint( m_pTextureBackground[ 4 ]->m_size.cx, (m_pTextureBackground[ 4 ]->m_size.cy ) + 4 ), 
													m_pTextureBackground[ 4 ],
													0.0f,
													0.0f,
													1.0f,
													1.0f
												);
				}
			}
			else
			{
				if( nWidth == 0 )
					m_pTextureBackground[ 3 ]->Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );
				else if( nWidth == (nWidthCount - 1) )
					m_pTextureBackground[ 5 ]->Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );
				else
					m_pTextureBackground[ 4 ]->Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );
			}
			ptLeftTop.x += pTexture->m_size.cx;	
		}
		ptLeftTop.x = rtRect.left;
		ptLeftTop.y += pTexture->m_size.cy;

		if( nHeight == (nHeightCount - 2) )
			ptLeftTop.y = ptLeftTop.y - nRemainHeight;
	}
}

#pragma endregion WndComboListBox

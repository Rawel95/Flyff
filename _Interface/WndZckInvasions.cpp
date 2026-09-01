#include "StdAfx.h"

#ifdef __ZCK_INVASIONS

#include "WndZckInvasions.h"
#include "ResData.h"
#include "../_Common/ZckInvasions.h"
#include "../_Common/playerdata.h"
#include "../_Common/FLItemElem.h"

#include <algorithm>
#include <utility>
#include <vector>

namespace
{
	std::vector<std::pair<u_long, __int64> > BuildZckInvasionRanking( const ZckCurrentInvasion& current )
	{
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
		return ranking;
	}
}

CWndZckInvasions::CWndZckInvasions()
	: m_dwInvasionID( NULL_ID )
	, m_bOnSurface( FALSE )
{
}

CWndZckInvasions::~CWndZckInvasions()
{
}

BOOL CWndZckInvasions::Process()
{
	CRect rtQuickInfoApplet = GetWindowRect();
	static const int REVISION_RECT_LEFT = 4;
	static const int REVISION_RECT_UP = 22;
	rtQuickInfoApplet.left -= REVISION_RECT_LEFT;
	rtQuickInfoApplet.right -= REVISION_RECT_LEFT;
	rtQuickInfoApplet.top -= REVISION_RECT_UP;
	rtQuickInfoApplet.bottom -= REVISION_RECT_UP;
	const CPoint ptMouse = GetMousePoint();
	m_bOnSurface = ( rtQuickInfoApplet.PtInRect( ptMouse ) == TRUE || m_nResizeDir > 0 || m_bPickup == TRUE ) ? TRUE : FALSE;
	m_nAlphaCount = 0;
	return TRUE;
}

void CWndZckInvasions::OnDraw( C2DRender* p2DRender )
{
	if( p2DRender == NULL || g_pPlayer == NULL )
		return;

	CZckInvasions* pManager = CZckInvasions::GetInstance();
	if( pManager == NULL )
		return;

	const ZckInvasionDefinition* pDefinition = pManager->GetDefinition( m_dwInvasionID );
	if( pDefinition == NULL )
		return;

	const std::vector<ZckCurrentInvasion>& currentInvasions = pManager->GetCurrentInvasions();
	const ZckCurrentInvasion* pCurrent = NULL;
	for( std::vector<ZckCurrentInvasion>::const_iterator it = currentInvasions.begin(); it != currentInvasions.end(); ++it )
	{
		if( it->dwInvasionID == m_dwInvasionID )
		{
			pCurrent = &(*it);
			break;
		}
	}
	if( pCurrent == NULL )
		return;

	const std::vector<std::pair<u_long, __int64> > ranking = BuildZckInvasionRanking( *pCurrent );
	CString strBestPlayer( _T( "Aucun" ) );
	if( ranking.empty() == false )
	{
		PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData( ranking.front().first );
		if( pPlayerData != NULL )
			strBestPlayer = pPlayerData->szPlayer;
	}

	int nPlayerRank = 0;
	for( size_t i = 0; i < ranking.size(); ++i )
	{
		if( ranking[i].first == g_pPlayer->m_idPlayer )
		{
			nPlayerRank = static_cast<int>( i ) + 1;
			break;
		}
	}

	const int nPosY = 30;
	const DWORD dwRenderColor = D3DCOLOR_TEMP( 150, 30, 30, 30 );
	const CRect rectBackground( 0, 0, 240, 140 );
	p2DRender->RenderFillRect( rectBackground, dwRenderColor, dwRenderColor, dwRenderColor, dwRenderColor );
	p2DRender->RenderLine( CPoint( 0, nPosY - 5 ), CPoint( 240, nPosY - 5 ), D3DCOLOR_ARGB( 180, 180, 180, 180 ) );

	CString strText;
	CEditString editString;

	const int nWaveCount = static_cast<int>( pDefinition->vWaves.size() );
	int nDisplayWave = pCurrent->nCurrentWave < 0 ? 0 : ( pCurrent->nCurrentWave + 1 );
	if( nDisplayWave > nWaveCount )
		nDisplayWave = nWaveCount;
	strText.Format( _T( "Vague : %d/%d" ), nDisplayWave, nWaveCount );
	editString.SetParsingString( strText );
	editString.SetStyle( ESSTY_BOLD );
	editString.SetColor( 0xFFFFFFFF );
	p2DRender->TextOut_EditString( 120 - ( p2DRender->m_pFont->GetTextExtent( strText ).cx / 2 ), nPosY - 22, editString );

	strText.Format( _T( "Monstres Restants : %d" ), pCurrent->nMonstersAlive );
	editString.SetParsingString( strText );
	editString.SetStyle( 0 );
	editString.SetColor( 0xFFFFFFFF );
	p2DRender->TextOut_EditString( 5, nPosY, editString );

	strText.Format( _T( "Joueur en tete : %s" ), static_cast<LPCTSTR>( strBestPlayer ) );
	editString.SetParsingString( strText );
	editString.SetColor( 0xFFFFFFFF );
	p2DRender->TextOut_EditString( 5, nPosY + 20, editString );

	strText.Format( _T( "Participants : %d" ), static_cast<int>( ranking.size() ) );
	editString.SetParsingString( strText );
	editString.SetColor( 0xFFFFFFFF );
	p2DRender->TextOut_EditString( 5, nPosY + 40, editString );

	if( nPlayerRank > 0 )
		strText.Format( _T( "Votre classement : %d/%d" ), nPlayerRank, static_cast<int>( ranking.size() ) );
	else
		strText.Format( _T( "Votre classement : -/%d" ), static_cast<int>( ranking.size() ) );
	editString.SetParsingString( strText );
	editString.SetColor( 0xFFFFFFFF );
	p2DRender->TextOut_EditString( 5, nPosY + 60, editString );

	strText = _T( "Recompense : " );
	editString.SetParsingString( strText );
	editString.SetColor( 0xFFFFFFFF );
	p2DRender->TextOut_EditString( 5, nPosY + 80, editString );

	const ZckInvasionReward* pReward = NULL;
	if( nPlayerRank > 0 )
	{
		for( std::vector<ZckInvasionReward>::const_iterator itReward = pDefinition->vRewards.begin(); itReward != pDefinition->vRewards.end(); ++itReward )
		{
			if( nPlayerRank >= itReward->nMinRank && nPlayerRank <= itReward->nMaxRank )
			{
				pReward = &(*itReward);
				break;
			}
		}
	}

	const int nLabelSize = p2DRender->m_pFont->GetTextExtent( strText ).cx;
	if( pReward == NULL )
	{
		strText = _T( "-" );
		editString.SetParsingString( strText );
		editString.SetColor( 0xFFFFFFFF );
		p2DRender->TextOut_EditString( 5 + nLabelSize, nPosY + 80, editString );
		return;
	}

	PT_ITEM_SPEC pItemSpec = g_xSpecManager->GetSpecItem( pReward->dwItemID );
	if( pItemSpec == NULL )
		return;

	strText.Format( _T( "%dx " ), pReward->nItemCount );
	editString.SetParsingString( strText );
	editString.SetColor( 0xFFFFFFFF );
	p2DRender->TextOut_EditString( 5 + nLabelSize, nPosY + 80, editString );

	const int nIconX = 5 + nLabelSize + p2DRender->m_pFont->GetTextExtent( strText ).cx - 2;
	CTexture* pItemTexture = CWndBase::m_textureMng.AddTexture(
		g_Neuz.m_pd3dDevice,
		MakePath( DIR_ITEM, pItemSpec->szIcon ),
		0xffff00ff
	);
	if( pItemTexture == NULL )
		return;

	pItemTexture->RenderScal( p2DRender, CPoint( nIconX, nPosY + 76 ), 255, 0.7f, 0.7f );

	CRect rectTooltip( nIconX, nPosY + 76, nIconX + 23, nPosY + 99 );
	const CPoint ptMouse = GetMousePoint();
	if( rectTooltip.PtInRect( ptMouse ) )
	{
		CPoint ptScreen = ptMouse;
		ClientToScreen( &ptScreen );
		ClientToScreen( &rectTooltip );

		FLItemElem itemElem;
		itemElem.m_dwItemId = pItemSpec->dwID;
		g_WndMng.PutToolTip_Item( static_cast<FLItemBase*>( &itemElem ), ptScreen, &rectTooltip, APP_ZCK_INVASIONS );
	}
}

void CWndZckInvasions::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	RestoreDeviceObjects();

	if( g_pPlayer == NULL || m_pWndRoot == NULL )
		return;

	const CRect rectRoot = m_pWndRoot->GetLayoutRect();
	const CRect rectWindow = GetWindowRect();
	Move( CPoint( rectRoot.right - rectWindow.Width(), 160 ) );
}

BOOL CWndZckInvasions::Initialize( CWndBase* pWndParent, DWORD )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_ZCK_INVASIONS, 0, CPoint( 0, 0 ), pWndParent );
}

#endif // __ZCK_INVASIONS
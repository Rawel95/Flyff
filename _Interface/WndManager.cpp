
//---------------------------------------------------------------------------------------------
// «Ï¥ı
//---------------------------------------------------------------------------------------------
#pragma region HeaderInclude

#include "StdAfx.h"

#include "WndMap.h"
#include "WndText.h"
#include "WndBagEx.h"
#include "WndMapEx.h"
#ifdef __ZCK_INVASIONS
#include "WndZckInvasions.h"
#include "../_Common/ZckInvasions.h"
#endif // __ZCK_INVASIONS
#ifdef __ZCK_VISU_MODELS
#include "WndModelView.h"
#endif // __ZCK_VISU_MODELS
#include "WndNotice.h"
#include "WndField2.h"
#include "WndRegVend.h"
#include "WndCommand.h"
#include "WndGuildHouse.h"
#include "WndMessenger.h"
#include "WndDuelConfirm.h"
#include "WndPartyConfirm.h"
#include "WndHelperWebBox.h"
#include "WndSelectVillage.h"
#include "WndFriendConfirm.h"
#include "WndPartyChangeTroup.h"

#include "../Neuz/DPCertified.h"
#include "../Neuz/DPLoginClient.h"
#include "../Neuz/CoupleHelper.h"		// 13¬˜ ƒø«√ Ω√Ω∫≈€

#include <Tax.h>					// ºº±›
#include <Post.h>
#include <Party.h>
#include <YUtil.h>
#include <Guild.h>
#include <ClientMsg.h>
#include <PlayerData.h>		
#include <SecretRoom.h>		
#include <GuildHouse.h>

#ifdef __Y_INTERFACE_VER3
	#include "WorldMap.h"
#endif 

#ifdef __HYPERLINK_ITEM16
	#include "WndTooltip.h"
#endif

#pragma endregion HeaderInclude

CWndMgr											g_WndMng;
CPtrArray										m_wndOrder;
CTexture*										g_pCoolTexArry[128] = { NULL, };
CTexture*										g_pEnchantTexArry[11] = { NULL, };

extern CDPClient								g_DPlay;	 
extern CDPCertified								g_dpCertified;
extern CDPLoginClient							g_dpLoginClient;

static const TCHAR								LOG_POSTION[] = _T( "Postion" );


//---------------------------------------------------------------------------------------------
// ¿¸ø™ «‘ºˆ
//---------------------------------------------------------------------------------------------
#pragma region GlobalFunction

const char* GetATKSpeedString( float fSpeed )
{
	if( fSpeed < 0.035 )
		return prj.GetText(TID_GAME_VERYSLOW);  // "æ∆¡÷ ¥¿∏≤"
	else if( fSpeed < 0.050 )
		return prj.GetText(TID_GAME_SLOW);		//"¥¿∏≤";
	else if( fSpeed < 0.070 )
		return prj.GetText(TID_GAME_NORMALS);	// "∫∏≈Î";
	else if( fSpeed < 0.080 )
		return prj.GetText(TID_GAME_FAST);		//"∫¸∏ß";
	else if( fSpeed < 0.17 )
		return prj.GetText(TID_GAME_VERYFAST);	//"æ∆¡÷ ∫¸∏ß";
	else
		return prj.GetText(TID_GAME_FASTEST);	//"æ∆¡÷ ∫¸∏ß";
}


void RenderEnchant( C2DRender* p2DRender, CPoint pt )
{
	static int i=0;
	
	CPoint ptBack = pt;
	CPoint ptBack_size( 32, 32 );

	i++;

	if( i>=11 )
		i = 0;
	
	p2DRender->RenderTextureEx( ptBack, ptBack_size, g_pEnchantTexArry[i], 255, 1.0f, 1.0f ) ;
}

// ∑π¿Ã¥ı ∏æÁ¿ª ±◊∏∞¥Ÿ.
void RenderRadar( C2DRender* p2DRender, CPoint pt, DWORD dwValue, DWORD dwDivisor )
{
	pt.y += 1;
	pt.x += 15;
	
	CPoint pt_size( 15, 15 );
	const BYTE bAlpha = 200;

	int nDrawIndex = MulDiv( dwValue, 127, dwDivisor );
	
	CPoint ptBack = pt;
	CPoint ptBack_size( 32, 32 );
	ptBack.x -= 15;
	p2DRender->RenderFillRect( CRect( ptBack, ptBack+ptBack_size ), D3DCOLOR_ARGB( 120, 255,255,255 ) );
	
	if( nDrawIndex >= 0 && nDrawIndex < 32 )
	{
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[nDrawIndex], bAlpha, 1.1f, 1.1f );
		
		pt.y+=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[32], bAlpha, 1.1f, 1.1f );
		
		pt.x-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[64], bAlpha, 1.1f, 1.1f );
		
		pt.y-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[96], bAlpha, 1.1f, 1.1f );
	}
	else
	if( nDrawIndex >= 32 && nDrawIndex <= 63 )
	{
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[31], bAlpha, 1.1f, 1.1f );
		
		pt.y+=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[nDrawIndex], bAlpha, 1.1f, 1.1f );
		
		pt.x-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[64], bAlpha, 1.1f, 1.1f );
		
		pt.y-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[96], bAlpha, 1.1f, 1.1f );
	}
	else
	if( nDrawIndex >= 64 && nDrawIndex <= 95 )
	{
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[31], bAlpha, 1.1f, 1.1f );
		
		pt.y+=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[63], bAlpha, 1.1f, 1.1f );
		
		pt.x-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[nDrawIndex], bAlpha, 1.1f, 1.1f );
		
		pt.y-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[96], bAlpha, 1.1f, 1.1f );
	}
	else
	if( nDrawIndex >= 96 && nDrawIndex <= 128 )
	{
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[31], bAlpha, 1.1f, 1.1f );
		
		pt.y+=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[63], bAlpha, 1.1f, 1.1f );
		
		pt.x-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[95], bAlpha, 1.1f, 1.1f );
		
		pt.y-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[nDrawIndex], bAlpha, 1.1f, 1.1f );
	}	
				
}

#pragma endregion GlobalFunction


//---------------------------------------------------------------------------------------------
// ¿©µµøÏ ∏ﬁ¥œ¡Æ ≈¨∑πΩ∫
//---------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------
// ª˝º∫¿⁄
//---------------------------------------------------------------------------------------------
CWndMgr::CWndMgr()
:	m_pWndWorld( NULL )						
,	m_pWndTradeConfirm( NULL )				
,	m_pWndTaskBar( NULL )
,	m_bTitle( FALSE )
,	m_bAutoRun( FALSE )
,	m_bConnect( FALSE )
,	m_clearFlag( FALSE )
,	m_bAllAction( FALSE ) 
,	m_bWaitRequestMail( FALSE )
,	m_bIsOpenLordSkill( false )
,	m_dwSavePlayerTime( 0 )
,	m_strChatBackup( _T("") )
,	m_bShortcutCommand( FALSE )
,	m_nWarningCounter( 0 )
,	m_nWarning2Counter( 0 )
,   m_pWndStatDice( NULL )
,	m_nMaketOpen( 0 )
#ifdef __WIKIPEDIA_MONSTER
,	m_pWndWikiMob ( NULL)
,	m_pWndWikiMobCreate ( NULL)
#endif // __WIKI_MOB
{
	ZeroMemory( m_dwSkillTime, sizeof( DWORD ) * MAX_SKILL );
	ZeroMemory( m_szTimerChat, sizeof( TCHAR ) * 128 );
	ZeroMemory( dwItemColor, sizeof( ToolTipItemTextColor ) * MAX_TC );

	ZeroMemory( g_pCoolTexArry, sizeof( CTexture* ) * 128 );
	ZeroMemory( g_pEnchantTexArry, sizeof( CTexture* ) * 11 );

	m_timerChat.Set( 0 );
	m_timerMessenger.Set( MIN( 5 ) );

	m_nAlphaCount = m_nAlpha;

	InitSetItemTextColor();
	RegisterAllCustomChild();

	m_pWndRoot = this;
}


//---------------------------------------------------------------------------------------------
// ∆ƒ±´¿⁄
//---------------------------------------------------------------------------------------------
CWndMgr::~CWndMgr()
{
	AppletFunc*		pApplet( NULL );
	DWORD			dwIdApplet( NULL );

	POSITION pos = m_mapAppletFunc.GetStartPosition();
	while( pos )
	{
		m_mapAppletFunc.GetNextAssoc( pos, dwIdApplet, (void*&)pApplet );
		SAFE_DELETE( (AppletFunc*) pApplet );
	}

	Free();
	m_vecChildWnds.clear( );
}


//---------------------------------------------------------------------------------------------
// ∏µÁ ƒøΩ∫≈“ ¿⁄Ωƒ ¿©µµøÏ∏¶ µÓ∑œ«—¥Ÿ.
//---------------------------------------------------------------------------------------------
void CWndMgr::RegisterAllCustomChild()
{
	//gmpbigsun(20110722) : WndManager¿« ∏‚πˆ∫Øºˆ¥¬ ¿Ã∑∏∞‘ √ ±‚»≠«ÿæﬂ OnDestroyChildWnd π◊ Freeø°º≠ ¿⁄µø¿∏∑Œ √≥∏ÆµÀ¥œ¥Ÿ.
	
	m_vecChildWnds.reserve( 300 ); 

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		RegisterCustomChild( (CWndBase*&)m_pWndTaskBar );

	RegisterCustomChild( (CWndBase*&)m_pWndMenu );
	RegisterCustomChild( (CWndBase*&)m_pWndActiveDesktop );
#ifdef __ZCK_INVASIONS
	RegisterCustomChild( (CWndBase*&)m_pWndZckInvasions );
#endif // __ZCK_INVASIONS
	RegisterCustomChild( (CWndBase*&)m_pWndShop );
	RegisterCustomChild( (CWndBase*&)m_pWndShop_Cart );
	RegisterCustomChild( (CWndBase*&)m_pWndDialog );
	RegisterCustomChild( (CWndBase*&)m_pWndRevival );
	RegisterCustomChild( (CWndBase*&)m_pReWanted		  );
	RegisterCustomChild( (CWndBase*&)m_pWanted		  );
	RegisterCustomChild( (CWndBase*&)m_pWndResurrectionConfirm );
	RegisterCustomChild( (CWndBase*&)m_pLogOutWaitting );
	RegisterCustomChild( (CWndBase*&)m_pWndCommItemDlg );
	RegisterCustomChild( (CWndBase*&)m_pWndChangeClass1 );
	RegisterCustomChild( (CWndBase*&)m_pWndChangeClass2 );
	RegisterCustomChild( (CWndBase*&)m_pWndTradeGold );
	RegisterCustomChild( (CWndBase*&)m_pWndDropItem );
	RegisterCustomChild( (CWndBase*&)m_pWndQuickList );
	RegisterCustomChild( (CWndBase*&)m_pWndCommand );
	RegisterCustomChild( (CWndBase*&)m_pWndTrade );
	RegisterCustomChild( (CWndBase*&)m_pWndBeautyShop );
	RegisterCustomChild( (CWndBase*&)m_pWndFaceShop );			
	RegisterCustomChild( (CWndBase*&)m_pWndUseCouponConfirm );
	RegisterCustomChild( (CWndBase*&)m_pWndSummonAngel );		
	RegisterCustomChild( (CWndBase*&)m_pWndDropConfirm );
	RegisterCustomChild( (CWndBase*&)m_pWndBank );
	RegisterCustomChild( (CWndBase*&)m_pWndConfirmBank );
	RegisterCustomChild( (CWndBase*&)m_pWndBankPassword );
	RegisterCustomChild( (CWndBase*&)m_pWndConfirmTrade );
	RegisterCustomChild( (CWndBase*&)m_pWndKawiBawiBoGame );
	RegisterCustomChild( (CWndBase*&)m_pWndKawiBawiBoGameConfirm );
	RegisterCustomChild( (CWndBase*&)m_pWndFindWordGame );
	RegisterCustomChild( (CWndBase*&)m_pWndDiceGame );
	RegisterCustomChild( (CWndBase*&)m_pWndPuzzleGame );
	RegisterCustomChild( (CWndBase*&)m_pWndSmeltMixJewel );
#ifdef COSTUME_UPGRADE_MIX
	RegisterCustomChild( (CWndBase*&)m_pWndSmeltItemMix );
#endif
	RegisterCustomChild( (CWndBase*&)m_pWndSmeltJewel );
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
	RegisterCustomChild( (CWndBase*&)m_pWndNewSmeltJewel );
#endif
	RegisterCustomChild( (CWndBase*&)m_pWndExtraction );
	RegisterCustomChild( (CWndBase*&)m_pWndChangeWeapon );
	RegisterCustomChild( (CWndBase*&)m_pWndPetStatus );
	RegisterCustomChild( (CWndBase*&)m_pWndBuffPetStatus );
	RegisterCustomChild( (CWndBase*&)m_pWndConfirmVis );
	RegisterCustomChild( (CWndBase*&)m_pWndFoodConfirm );
	RegisterCustomChild( (CWndBase*&)m_pWndPetMiracle );
	RegisterCustomChild( (CWndBase*&)m_pWndPetFoodMill );
	RegisterCustomChild( (CWndBase*&)m_pWndPetLifeConfirm );
	RegisterCustomChild( (CWndBase*&)m_pWndHeroSkillUp );		
	RegisterCustomChild( (CWndBase*&)m_pWndRemovePiercing );	
	RegisterCustomChild( (CWndBase*&)m_pWndRemoveJewel );		
	RegisterCustomChild( (CWndBase*&)m_pWndPetTransEggs );		
	RegisterCustomChild( (CWndBase*&)m_pWndHeavenTower );		
	RegisterCustomChild( (CWndBase*&)m_pWndRemoveAttribute );	
	RegisterCustomChild( (CWndBase*&)m_pWndPost );
	RegisterCustomChild( (CWndBase*&)m_pWndQueryEquip );
	RegisterCustomChild( (CWndBase*&)m_pWndSelectVillage );
	RegisterCustomChild( (CWndBase*&)m_pWndQuestItemWarning );
	RegisterCustomChild( (CWndBase*&)m_pWndPenaltyPK );
	RegisterCustomChild( (CWndBase*&)m_pWndPartyChangeName );
	RegisterCustomChild( (CWndBase*&)m_pWndPartyConfirm );
	RegisterCustomChild( (CWndBase*&)m_pWndFriendConfirm );
	RegisterCustomChild( (CWndBase*&)m_pWndDuelConfirm );
	RegisterCustomChild( (CWndBase*&)m_pWndMessageNote );
	RegisterCustomChild( (CWndBase*&)m_pWndMessengerNote );
	RegisterCustomChild( (CWndBase*&)m_pWndRandomScrollConfirm );
	RegisterCustomChild( (CWndBase*&)m_pWndStateConfirm		);
	RegisterCustomChild( (CWndBase*&)m_pWndCollecting );
	RegisterCustomChild( (CWndBase*&)m_pWndRankGuild );
	RegisterCustomChild( (CWndBase*&)m_pWndRankInfo );
	RegisterCustomChild( (CWndBase*&)m_pWndRankWar );
	RegisterCustomChild( (CWndBase*&)m_pWndGuildBank );
	RegisterCustomChild( (CWndBase*&)m_pWndGuildBankLog );	
	RegisterCustomChild( (CWndBase*&)m_pWndGuildConfirm );
	RegisterCustomChild( (CWndBase*&)m_pWndGuildWarPeaceConfirm );
	RegisterCustomChild( (CWndBase*&)m_pWndGuildWarRequest );
	RegisterCustomChild( (CWndBase*&)m_pWndGuildWarState );
	RegisterCustomChild( (CWndBase*&)m_pWndGuildCombatRanking );
	RegisterCustomChild( (CWndBase*&)m_pWndFontEdit );
	RegisterCustomChild( (CWndBase*&)m_pWndReSkillWarning );
	RegisterCustomChild( (CWndBase*&)m_pWndGuildCombatBoard      );	
	RegisterCustomChild( (CWndBase*&)m_pWndGuildCombatOffer		);
	RegisterCustomChild( (CWndBase*&)m_pWndGuildCombatSelection	);
	RegisterCustomChild( (CWndBase*&)m_pWndGuildCombatJoinSelection );
	RegisterCustomChild( (CWndBase*&)m_pWndGuildCombatInfoMessageBox2 );
	RegisterCustomChild( (CWndBase*&)m_pWndGuildCombatInfoMessageBox );
	RegisterCustomChild( (CWndBase*&)n_pWndGuildCombatResult );
	RegisterCustomChild( (CWndBase*&)m_pWndGuildCombat1to1Selection );
	RegisterCustomChild( (CWndBase*&)m_pWndGuildCombat1to1Offer );
	RegisterCustomChild( (CWndBase*&)m_pWndChatLog );	
	RegisterCustomChild( (CWndBase*&)m_pWndSecretRoomMsg );
	RegisterCustomChild( (CWndBase*&)m_pWndSecretRoomSelection );
	RegisterCustomChild( (CWndBase*&)m_pWndSecretRoomOffer );
	RegisterCustomChild( (CWndBase*&)m_pWndSecretRoomOfferState );
	RegisterCustomChild( (CWndBase*&)m_pWndSecretRoomChangeTaxRate );
	RegisterCustomChild( (CWndBase*&)m_pWndSecretRoomCheckTaxRate );
	RegisterCustomChild( (CWndBase*&)m_pWndSecretRoomBoard );
	RegisterCustomChild( (CWndBase*&)m_pWndSecretRoomQuick );
	RegisterCustomChild( (CWndBase*&)m_pWndRainbowRaceOffer );
	RegisterCustomChild( (CWndBase*&)m_pWndRainbowRaceInfo );
	RegisterCustomChild( (CWndBase*&)m_pWndRainbowRaceRule );
	RegisterCustomChild( (CWndBase*&)m_pWndRainbowRaceRanking );
	RegisterCustomChild( (CWndBase*&)m_pWndRainbowRacePrize );
	RegisterCustomChild( (CWndBase*&)m_pWndRainbowRaceMiniGameButton );
	RegisterCustomChild( (CWndBase*&)m_pWndRainbowRaceMiniGame );
	RegisterCustomChild( (CWndBase*&)m_pWndRainbowRaceMiniGameEnd );
	RegisterCustomChild( (CWndBase*&)m_pWndRRMiniGameKawiBawiBo );
	RegisterCustomChild( (CWndBase*&)m_pWndRRMiniGameDice );
	RegisterCustomChild( (CWndBase*&)m_pWndRRMiniGameArithmetic );
	RegisterCustomChild( (CWndBase*&)m_pWndRRMiniGameStopWatch );
	RegisterCustomChild( (CWndBase*&)m_pWndRRMiniGameTyping );
	RegisterCustomChild( (CWndBase*&)m_pWndRRMiniGameCard );
	RegisterCustomChild( (CWndBase*&)m_pWndRRMiniGameLadder );
	RegisterCustomChild( (CWndBase*&)m_pWndCoupleMessage );		
	RegisterCustomChild( (CWndBase*&)m_pWndChangeAttribute );	
	RegisterCustomChild( (CWndBase*&)m_pWndLordState );
	RegisterCustomChild( (CWndBase*&)m_pWndLordTender );
	RegisterCustomChild( (CWndBase*&)m_pWndLordVote );
	RegisterCustomChild( (CWndBase*&)m_pWndLordEvent );
	RegisterCustomChild( (CWndBase*&)m_pWndLordSkillConfirm );
	RegisterCustomChild( (CWndBase*&)m_pWndLordInfo );
	RegisterCustomChild( (CWndBase*&)m_pWndLordRPInfo );
	RegisterCustomChild( (CWndBase*&)m_pWndPetAwakCancel );	
	RegisterCustomChild( (CWndBase*&)m_pWndRoomList );
	RegisterCustomChild( (CWndBase*&)m_pWndQuitRoom );
	RegisterCustomChild( (CWndBase*&)m_pWndChangePetName );	
	RegisterCustomChild( (CWndBase*&)m_pWndUpgradeBase );
	RegisterCustomChild( (CWndBase*&)m_pWndPiercing );
	RegisterCustomChild( (CWndBase*&)m_pWndInvenRemoveItem );
	RegisterCustomChild( (CWndBase*&)m_pWndCommerialElem );
	RegisterCustomChild( (CWndBase*&)m_pRemoveElem );
	RegisterCustomChild( (CWndBase*&)m_pQuestItemInfo );
	RegisterCustomChild( (CWndBase*&)m_pWndQuestQuickInfo );
	RegisterCustomChild( (CWndBase*&)m_pWndTextBook );
	RegisterCustomChild( (CWndBase*&)m_pWndTextScroll );
	RegisterCustomChild( (CWndBase*&)m_pWndTextLetter );
	RegisterCustomChild( (CWndBase*&)m_pWndMessageBox );
	RegisterCustomChild( (CWndBase*&)m_pWndMessageBoxUpper );
	RegisterCustomChild( (CWndBase*&)m_pWndChangeName );
	RegisterCustomChild( (CWndBase*&)m_pWndSealChar );
	RegisterCustomChild( (CWndBase*&)m_pWndSealCharSelect );
	RegisterCustomChild( (CWndBase*&)m_pWndSealCharSend );
	RegisterCustomChild( (CWndBase*&)m_pWndSealCharSet );
	RegisterCustomChild( (CWndBase*&)m_pWndAwakening );
	RegisterCustomChild( (CWndBase*&)m_pPetRes );
	RegisterCustomChild( (CWndBase*&)m_pWndBlessingCancel );
	RegisterCustomChild( (CWndBase*&)m_pFunnyCoinConfirm );
	RegisterCustomChild( (CWndBase*&)m_pWndSmeltSafety );
	RegisterCustomChild( (CWndBase*&)m_pWndSmeltSafetyConfirm );
	RegisterCustomChild( (CWndBase*&)m_pWndEquipBindConfirm );
	RegisterCustomChild( (CWndBase*&)m_pWndRestateConfirm );
	RegisterCustomChild( (CWndBase*&)m_pWndQuizEventConfirm );
	RegisterCustomChild( (CWndBase*&)m_pWndQuizEventQuestionOX );
	RegisterCustomChild( (CWndBase*&)m_pWndQuizEventQuestion4C );
	RegisterCustomChild( (CWndBase*&)m_pWndQuizEventButton );
	RegisterCustomChild( (CWndBase*&)m_pWndQuestDetail );
	RegisterCustomChild( (CWndBase*&)m_pWndCampusInvitationConfirm );
	RegisterCustomChild( (CWndBase*&)m_pWndCampusSeveranceConfirm );
	RegisterCustomChild( (CWndBase*&)m_pWndHelpInstant );
	RegisterCustomChild( (CWndBase*&)m_pWndGHMain );
	RegisterCustomChild( (CWndBase*&)m_pWndUpkeep );
	RegisterCustomChild( (CWndBase*&)m_pWndSelectAwakeCase );
	RegisterCustomChild( (CWndBase*&)m_pWndChattingBlockingList );
	RegisterCustomChild( (CWndBase*&)m_pWndGHBid );
	RegisterCustomChild( (CWndBase*&)m_pWndGHNotice );
	RegisterCustomChild( (CWndBase*&)m_pWndCoupon1000722 );
	RegisterCustomChild( (CWndBase*&)m_pWndBarunaNPC );
	RegisterCustomChild( (CWndBase*&)m_pWndColosseumJoin );
	RegisterCustomChild( (CWndBase*&)m_pWndColosseumModeless );
	RegisterCustomChild( (CWndBase*&)m_pWndColosseumModal );
	RegisterCustomChild( (CWndBase*&)m_pWndColosseumCleared );
	RegisterCustomChild( (CWndBase*&)m_pWndColosseumReadyToStart );
	RegisterCustomChild( (CWndBase*&)m_pWndColosseumRetry );
	RegisterCustomChild( (CWndBase*&)m_pWndColosseumRanking );
	RegisterCustomChild( (CWndBase*&)m_pWndDonation );
	RegisterCustomChild( (CWndBase*&)m_pWndDonationRanking );
	RegisterCustomChild( (CWndBase*&)m_pWndEArenaTeam );
	RegisterCustomChild( (CWndBase*&)m_pWndEArenaMessageBox );
	RegisterCustomChild( (CWndBase*&)m_pWndEArenaWinner );
	RegisterCustomChild( (CWndBase*&)m_pWndEArenaReferee );
	RegisterCustomChild( (CWndBase*&)m_pWndEArenaMng );
	RegisterCustomChild( (CWndBase*&)m_pWndScoreTitle );
	RegisterCustomChild( (CWndBase*&)m_pWndProxyMarket );
	RegisterCustomChild( (CWndBase*&)m_pWndComposeTwoWeapon );
	RegisterCustomChild( (CWndBase*&)m_pWndCancel_CTW );
	RegisterCustomChild( (CWndBase*&)m_pWndChangeLook );
	RegisterCustomChild( (CWndBase*&)m_pWndTargetHUD );
	RegisterCustomChild( (CWndBase*&)m_pWndCancellation );
	RegisterCustomChild( (CWndBase*&)m_pCostume_NPC);
	RegisterCustomChild( (CWndBase*&)m_pWndTeleporterMng );
	RegisterCustomChild( (CWndBase*&)m_pWndSelectCh );
	RegisterCustomChild( (CWndBase*&)m_pWndTreasurechest );
	RegisterCustomChild( (CWndBase*&)m_pWndTreasurechest_Result );
	RegisterCustomChild( (CWndBase*&)m_pWndChatNotice );
	RegisterCustomChild( (CWndBase*&)m_pWndRepairItem );
	RegisterCustomChild( (CWndBase*&)m_pWndGuildNickName );
	RegisterCustomChild( (CWndBase*&)m_pWndLvReqDown );
	RegisterCustomChild( (CWndBase*&)m_pWndPartyLeaveConfirm );
	RegisterCustomChild( (CWndBase*&)m_pWndDuelResult );
	RegisterCustomChild( (CWndBase*&)m_pWndAddFriend );
	RegisterCustomChild( (CWndBase*&)m_pWndCloseExistingConnection );
	RegisterCustomChild( (CWndBase*&)m_pWndCharServerMovement_Warning );
	RegisterCustomChild( (CWndBase*&)m_pWndCharServerMovement_Select );
	RegisterCustomChild( (CWndBase*&)m_pWndReawakening );
	RegisterCustomChild( (CWndBase*&)m_pWndPartyUpgradeScroll );
	RegisterCustomChild( (CWndBase*&)m_pWndCommonInputCount );
	RegisterCustomChild( (CWndBase*&)m_pWndCommonItemExchange );
	RegisterCustomChild( (CWndBase*&)m_pWndStatDice );
#ifdef PAT_LOOTOPTION
	RegisterCustomChild( (CWndBase*&)m_pWndPetLootOption );
#endif // PAT_LOOTOPTION
#ifdef CARD_UPGRADE_SYSTEM
	RegisterCustomChild( (CWndBase*&)m_pWndCardUpgrade );
#endif	// CARD_UPGRADE_SYSTEM
}


//---------------------------------------------------------------------------------------------
// æ∆¿Ã≈€ ≈ÿΩ∫∆Æ ªˆªÛ º≥¡§
//---------------------------------------------------------------------------------------------
void CWndMgr::InitSetItemTextColor( )
{
	dwItemColor[FIRST_TC].dwName0				= D3DCOLOR_XRGB(  46, 112, 169 );
	dwItemColor[FIRST_TC].dwName1				= D3DCOLOR_XRGB(   0,  93,   0 );			// ¿Ã∏ß 1¥‹∞Ë
	dwItemColor[FIRST_TC].dwName2				= D3DCOLOR_XRGB( 128,  64,   0 );			// ¿Ã∏ß 2¥‹∞Ë
	dwItemColor[FIRST_TC].dwName3				= D3DCOLOR_XRGB( 127,   0, 255 );			// ¿Ã∏ß 3¥‹∞Ë
	dwItemColor[FIRST_TC].dwName4				= D3DCOLOR_XRGB(   0,   0, 255 );			// ¿Ã∏ß 4¥‹∞Ë
	dwItemColor[FIRST_TC].dwName5				= D3DCOLOR_XRGB(   0,   0, 255 );			// ¿Ã∏ß 5¥‹∞Ë
	dwItemColor[FIRST_TC].dwGeneral				= D3DCOLOR_XRGB(   0,   0,   0 );			// ¿œπ›
	dwItemColor[FIRST_TC].dwGeneralOption		= D3DCOLOR_XRGB(   0,   0,   0 );			// ¿œπ› ø…º«
	dwItemColor[FIRST_TC].dwPlusOption			= D3DCOLOR_XRGB(   0,   0,   0 );			// «√∑ØΩ∫ ø…º«
	dwItemColor[FIRST_TC].dwPiercing			= D3DCOLOR_XRGB( 178,   0, 255 );			// ««æÓΩÃ
	dwItemColor[FIRST_TC].dwResist				= D3DCOLOR_XRGB(   0,   0,   0 );			// º”º∫ ¿œπ›
	dwItemColor[FIRST_TC].dwResistFire			= D3DCOLOR_XRGB(   0,   0,   0 );			// º”º∫ ∫“
	dwItemColor[FIRST_TC].dwResistWater			= D3DCOLOR_XRGB(   0,   0,   0 );			// º”º∫ π∞
	dwItemColor[FIRST_TC].dwResistEarth			= D3DCOLOR_XRGB(   0,   0,   0 );			// º”º∫ ∂•
	dwItemColor[FIRST_TC].dwResistElectricity	= D3DCOLOR_XRGB(   0,   0,   0 );			// º”º∫ ¿¸±‚
	dwItemColor[FIRST_TC].dwResistWind			= D3DCOLOR_XRGB(   0,   0,   0 );			// º”º∫ πŸ∂˜
	dwItemColor[FIRST_TC].dwResistSM			= D3DCOLOR_XRGB(   0, 158,   0 );			// º”º∫ ªÛøÎ»≠ π´±‚
	dwItemColor[FIRST_TC].dwResistSM1			= D3DCOLOR_XRGB( 255,   0,   0 );			// º”º∫ ªÛøÎ»≠ πÊæÓ±∏
	dwItemColor[FIRST_TC].dwTime				= D3DCOLOR_XRGB(   0,   200,   0 );			// Ω√∞£ π◊ »ø∞˙
	dwItemColor[FIRST_TC].dwEffective0			= D3DCOLOR_XRGB(   0,   0,   0 );			// æ∆¿Ã≈€ »ø¥…
	dwItemColor[FIRST_TC].dwEffective1			= D3DCOLOR_XRGB(   0, 255,   0 );			// æ∆¿Ã≈€ »ø¥…1
	dwItemColor[FIRST_TC].dwEffective2			= D3DCOLOR_XRGB( 255,   0,   0 );			// æ∆¿Ã≈€ »ø¥…2
	dwItemColor[FIRST_TC].dwEffective3			= D3DCOLOR_XRGB( 128, 128, 128 );			// æ∆¿Ã≈€ »ø¥…3
	dwItemColor[FIRST_TC].dwRandomOption		= D3DCOLOR_XRGB(   0,   0,   0 );			// ∑£¥˝ ø…º«
	dwItemColor[FIRST_TC].dwEnchantOption		= D3DCOLOR_XRGB(   0,   0,   0 );			// ¿Œ√æ∆Æ ø…º«
	dwItemColor[FIRST_TC].dwSetName				= D3DCOLOR_XRGB(   0,   0,   0 );			// ºº∆Æ ¿Ã∏ß
	dwItemColor[FIRST_TC].dwSetItem0			= D3DCOLOR_XRGB( 178, 178, 178 );			// ºº∆Æ ∏Ò∑œ(∫Ò¬¯øÎΩ√)
	dwItemColor[FIRST_TC].dwSetItem1			= D3DCOLOR_XRGB(   0,   0, 204 );			// ºº∆Æ ∏Ò∑œ(¬¯øÎΩ√)
	dwItemColor[FIRST_TC].dwSetEffect			= D3DCOLOR_XRGB( 255, 157,   0 );			// ºº∆Æ »ø∞˙
	dwItemColor[FIRST_TC].dwGold				= D3DCOLOR_XRGB(   0,   0,   0 );			// ∞°∞›
	dwItemColor[FIRST_TC].dwCommand				= D3DCOLOR_XRGB(   0,   0,   0 );			// º≥∏Ì
	dwItemColor[FIRST_TC].dwNotUse				= D3DCOLOR_XRGB( 255,  66,   0 );			// ªÁøÎ∏¯«‘
	dwItemColor[FIRST_TC].dwAddedOpt1			= D3DCOLOR_XRGB( 127, 255,   0 );			// æÛ≈Õ∏⁄ ø˛∆˘ √ﬂ∞° ø…º« "∞Ê«Ëƒ° ªÛΩ¬"
	dwItemColor[FIRST_TC].dwAddedOpt2			= D3DCOLOR_XRGB(   0, 255, 255 );			// æÛ≈Õ∏⁄ ø˛∆˘ √ﬂ∞° ø…º« "µ•πÃ¡ˆ π›ªÁ"
	dwItemColor[FIRST_TC].dwAddedOpt3			= D3DCOLOR_XRGB( 140,  23,  23 );			// æÛ≈Õ∏⁄ ø˛∆˘ √ﬂ∞° ø…º« "»Ì«˜"
	dwItemColor[FIRST_TC].dwAddedOpt4			= D3DCOLOR_XRGB( 255, 127,   0 );			// æÛ≈Õ∏⁄ ø˛∆˘ √ﬂ∞° ø…º« "ªÁ≥… µ•πÃ¡ˆ ¡ı∞°"
	dwItemColor[FIRST_TC].dwAddedOpt5			= D3DCOLOR_XRGB( 255, 110, 199 );			// æÛ≈Õ∏⁄ ø˛∆˘ √ﬂ∞° ø…º« "PvP µ•πÃ¡ˆ ¡ı∞°"
	dwItemColor[FIRST_TC].dwAddedOpt6			= D3DCOLOR_XRGB( 205, 127,  50 );			// æÛ≈Õ∏⁄ ø˛∆˘ √ﬂ∞° ø…º« "»˚ ¡ı∞°"
	dwItemColor[FIRST_TC].dwAddedOpt7			= D3DCOLOR_XRGB( 255,   0,   0 );			// æÛ≈Õ∏⁄ ø˛∆˘ √ﬂ∞° ø…º« "√º∑¬ ¡ı∞°"
	dwItemColor[FIRST_TC].dwAddedOpt8			= D3DCOLOR_XRGB(  50,  50, 255 );			// æÛ≈Õ∏⁄ ø˛∆˘ √ﬂ∞° ø…º« "πŒ√∏ ¡ı∞°"
	dwItemColor[FIRST_TC].dwAddedOpt9			= D3DCOLOR_XRGB( 147, 112, 219 );			// æÛ≈Õ∏⁄ ø˛∆˘ √ﬂ∞° ø…º« "¡ˆ¥… ¡ı∞°"

//sun: 11, ∞¢º∫, √‡∫π
//sun: 12, π´±‚ ««æÓΩÃ
	dwItemColor[FIRST_TC].dwAwakening			= D3DCOLOR_XRGB( 0, 0, 255 );				// æ∆¿Ã≈€ ∞¢º∫
	dwItemColor[FIRST_TC].dwBlessing			= D3DCOLOR_XRGB( 0, 0, 255 );				// ø©Ω≈¿« √‡∫π

	dwItemColor[FIRST_TC].dwBlessingWarning		= D3DCOLOR_XRGB( 255, 0, 0 );				// ø©Ω≈¿« √‡∫π ∞Ê∞Ì
	dwItemColor[FIRST_TC].dwBarunaRunePiercing	= D3DCOLOR_XRGB( 0, 0, 255 );				// πŸ∑Á≥™ ∑È ««æÓΩÃ
	
	
	memcpy( &dwItemColor[SECOND_TC], &dwItemColor[FIRST_TC], sizeof( ToolTipItemTextColor ) );
	memcpy( &dwItemColor[THIRD_TC], &dwItemColor[FIRST_TC], sizeof( ToolTipItemTextColor ) );
	dwItemColor[SECOND_TC].dwName0				= D3DCOLOR_XRGB(  12, 144, 231 );
	dwItemColor[SECOND_TC].dwResistFire			= D3DCOLOR_XRGB( 230,   0,   0 );			// º”º∫ ∫“
	dwItemColor[SECOND_TC].dwResistWater		= D3DCOLOR_XRGB(   0,   0, 182 );			// º”º∫ π∞
	dwItemColor[SECOND_TC].dwResistEarth		= D3DCOLOR_XRGB( 185, 118,   0 );			// º”º∫ ∂•
	dwItemColor[SECOND_TC].dwResistElectricity	= D3DCOLOR_XRGB(  21,  55, 152 );			// º”º∫ ¿¸±‚
	dwItemColor[SECOND_TC].dwResistWind			= D3DCOLOR_XRGB(   0, 217, 214 );			// º”º∫ πŸ∂˜
	dwItemColor[SECOND_TC].dwTime				= D3DCOLOR_XRGB( 146,   0,  38 );			// Ω√∞£ π◊ »ø∞˙
	dwItemColor[SECOND_TC].dwPlusOption			= D3DCOLOR_XRGB( 199, 155,   0 );			// «√∑ØΩ∫ ø…º«
	dwItemColor[SECOND_TC].dwPiercing			= D3DCOLOR_XRGB(  77,  11, 145 );			// ««æÓΩÃ
	dwItemColor[SECOND_TC].dwRandomOption		= D3DCOLOR_XRGB( 199, 155,   0 );			// ∑£¥˝ ø…º«
	dwItemColor[SECOND_TC].dwSetName			= D3DCOLOR_XRGB(  50, 178,   0 );			// ºº∆Æ ¿Ã∏ß
	dwItemColor[SECOND_TC].dwSetItem0			= D3DCOLOR_XRGB( 178, 178, 178 );			// ºº∆Æ ∏Ò∑œ(∫Ò¬¯øÎΩ√)
	dwItemColor[SECOND_TC].dwSetItem1			= D3DCOLOR_XRGB( 114, 186,  85 );			// ºº∆Æ ∏Ò∑œ(¬¯øÎΩ√)
	dwItemColor[SECOND_TC].dwSetEffect			= D3DCOLOR_XRGB(  50, 178,   0 );			// ºº∆Æ »ø∞˙
	dwItemColor[SECOND_TC].dwCommand			= D3DCOLOR_XRGB( 199, 155,   0 );			// º≥∏Ì
//sun: 11, ∞¢º∫, √‡∫π
//sun: 12, π´±‚ ««æÓΩÃ
	dwItemColor[SECOND_TC].dwAwakening			= D3DCOLOR_XRGB( 0, 0, 255 );				// æ∆¿Ã≈€ ∞¢º∫
	dwItemColor[SECOND_TC].dwBlessing			= D3DCOLOR_XRGB( 0, 0, 255 );				// ø©Ω≈¿« √‡∫π

	dwItemColor[SECOND_TC].dwBlessingWarning	= D3DCOLOR_XRGB( 255, 0, 0 );				// ø©Ω≈¿« √‡∫π ∞Ê∞Ì


  	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	{
		dwItemColor[FIRST_TC].dwName0				= D3DCOLOR_XRGB( 120, 217, 255 );		// ≥Î∏÷
		dwItemColor[FIRST_TC].dwName1				= D3DCOLOR_XRGB( 172, 220, 101 );		// ¿Ø¥œ≈©
		dwItemColor[FIRST_TC].dwName3				= D3DCOLOR_XRGB( 255,  70, 170 );		// æÛ≈Õ∏⁄
		dwItemColor[FIRST_TC].dwName4				= D3DCOLOR_XRGB( 255,  64,  39 );		// πŸ∑Á≥™
		dwItemColor[FIRST_TC].dwName5				= D3DCOLOR_XRGB( 255,  64,  39 );		// πŸ∑Á≥™ πÊæÓ±∏
		dwItemColor[FIRST_TC].dwAwakening			= D3DCOLOR_XRGB( 97,  97,  255 );		// æ∆¿Ã≈€ ∞¢º∫
		dwItemColor[FIRST_TC].dwBlessing			= D3DCOLOR_XRGB( 97,  97,  255 );		// ø©Ω≈¿« √‡∫π
		dwItemColor[FIRST_TC].dwGeneral				= D3DCOLOR_XRGB( 255, 255, 255 );		// ¿œπ›
		dwItemColor[FIRST_TC].dwGeneralOption		= D3DCOLOR_XRGB( 255, 234, 161 );		// ¿œπ› ø…º«
		dwItemColor[FIRST_TC].dwPlusOption			= D3DCOLOR_XRGB( 255, 255, 255 );		// «√∑ØΩ∫ ø…º«
		dwItemColor[FIRST_TC].dwPiercing			= D3DCOLOR_XRGB( 211, 134, 255 );		// ««æÓΩÃ
		dwItemColor[FIRST_TC].dwSetItem1			= D3DCOLOR_XRGB(   1, 171,  25 );		// ºº∆Æ ∏Ò∑œ(¬¯øÎΩ√)
		dwItemColor[FIRST_TC].dwResist				= D3DCOLOR_XRGB( 255, 255, 255 );		// º”º∫ ¿œπ›
		dwItemColor[FIRST_TC].dwResistFire			= D3DCOLOR_XRGB( 255, 255, 255 );		// º”º∫ ∫“
		dwItemColor[FIRST_TC].dwResistWater			= D3DCOLOR_XRGB( 255, 255, 255 );		// º”º∫ π∞
		dwItemColor[FIRST_TC].dwResistEarth			= D3DCOLOR_XRGB( 255, 255, 255 );		// º”º∫ ∂•
		dwItemColor[FIRST_TC].dwResistElectricity	= D3DCOLOR_XRGB( 255, 255, 255 );		// º”º∫ ¿¸±‚
		dwItemColor[FIRST_TC].dwResistWind			= D3DCOLOR_XRGB( 255, 255, 255 );		// º”º∫ πŸ∂˜
		dwItemColor[FIRST_TC].dwEffective0			= D3DCOLOR_XRGB( 255, 255, 255 );		// æ∆¿Ã≈€ »ø¥…
		dwItemColor[FIRST_TC].dwRandomOption		= D3DCOLOR_XRGB( 255, 255, 255 );		// ∑£¥˝ ø…º«
		dwItemColor[FIRST_TC].dwEnchantOption		= D3DCOLOR_XRGB( 255, 255, 255 );		// ¿Œ√æ∆Æ ø…º«
		dwItemColor[FIRST_TC].dwSetName				= D3DCOLOR_XRGB( 255, 255, 255 );		// ºº∆Æ ¿Ã∏ß
		dwItemColor[FIRST_TC].dwGold				= D3DCOLOR_XRGB( 255, 255, 255 );		// ∞°∞›
		dwItemColor[FIRST_TC].dwCommand				= D3DCOLOR_XRGB( 255, 255, 255 );		// º≥∏Ì
		dwItemColor[FIRST_TC].dwNotUse				= D3DCOLOR_XRGB( 255,  0,    0 );		// ªÁøÎ∏¯«‘
		dwItemColor[FIRST_TC].dwBarunaRunePiercing	= D3DCOLOR_XRGB( 255, 64,	 0 );		// πŸ∑Á≥™ ∑È ««æÓΩÃ
	}
}


//---------------------------------------------------------------------------------------------
// «ÿ¡¶
//---------------------------------------------------------------------------------------------
void CWndMgr::Free()
{
  	POSITION		pos( m_mapWndRegInfo.GetStartPosition() );
	DWORD			dwWndId( NULL );
	LPWNDREGINFO	pWndRegInfo;

	while( pos )
	{
		m_mapWndRegInfo.GetNextAssoc( pos, dwWndId, (void*&)pWndRegInfo );
		SAFE_DELETE( pWndRegInfo->lpArchive );
		SAFE_DELETE( pWndRegInfo );
	}
	m_mapWndRegInfo.RemoveAll();


	CString			strTemp( _T("") );
	CWndBase*		pWndBaseTemp( NULL );

	pos = m_mapMessage.GetStartPosition();
	while( pos )
	{
		m_mapMessage.GetNextAssoc( pos, strTemp, (void*&)pWndBaseTemp );
		SAFE_DELETE( pWndBaseTemp );
	}
	m_mapMessage.RemoveAll();


	pos = m_mapInstantMsg.GetStartPosition();
	while( pos )
	{
		m_mapInstantMsg.GetNextAssoc( pos, strTemp, (void*&)pWndBaseTemp );
		SAFE_DELETE( pWndBaseTemp );
	}
	m_mapInstantMsg.RemoveAll();


	pos = m_mapMap.GetStartPosition();
	while( pos )
	{
		m_mapMap.GetNextAssoc( pos, strTemp, (void*&)pWndBaseTemp );
		SAFE_DELETE( pWndBaseTemp );
	}
	m_mapMap.RemoveAll();


#ifdef __HYPERLINK_ITEM16
	CWndTooltip* pWndTooltip = CWndTooltip::_GetThis();

	if( pWndTooltip )
		pWndTooltip->SetVisible( FALSE );		//¡¢º”¡æ∑· -> Login¥‹∞Ë¿œ∞ÊøÏ ∞®√„
#endif

	DeleteAllCustomChild();
	m_pWndWorld = NULL;			//Appet ø°º≠ ªË¡¶µ«π«∑Œ delete«ÿº≠¥¬ æ»µ»¥Ÿ.
}




void CWndMgr::DestroyApplet()
{
	CWndNeuz* pWndBase;
	DWORD dwIdApplet;
	POSITION pos = m_mapWndApplet.GetStartPosition();
	while( pos )
	{
		m_mapWndApplet.GetNextAssoc( pos, dwIdApplet, (void*&)pWndBase );
		// ¿©µµ∞° ¡æ∑·µ… ∂ß WndRect¡§∫∏∏¶ ¿˙¿Â«œ¡ˆ æ ∞‘ «ÿæﬂ«—¥Ÿ.
		// ø©±‚º≠ ∆ƒ±´µ«¥¬ ∞Õ¿∫ ø¿«¬µ«æÓ ¿÷¥¬ ∞Õ¿∏∑Œ ∞£¡÷«ÿæﬂµ«¥¬µ•,
		// ≥ª∫Œø°º≠ ∆ƒ±´µ«∏È ≈©∑Œ¡Ó∑Œ ∆«¥‹«œ±‚ ∂ßπÆø° ¿ÁΩ««‡Ω√ ¿©µµ∞° ø≠∏Æ¡ˆ æ ∞‘ µ»¥Ÿ.
		//pWndBase->SetPutRegInfo( FALSE );
		// ¿Ã¡¶ ø©±‚º≠ ¿”¿«∑Œ ø¿«¬µ«æÓ ¿÷¥Ÿ∞Ì ¿˙¿Â«œ¿⁄.
		//PutRegInfo( dwIdApplet, pWndBase->GetWindowRect( TRUE), TRUE );
		if( pWndBase->IsPutRegInfo() )
			PutRegInfo( (CWndNeuz*)pWndBase, TRUE );//dwIdApplet, pWndBase->GetWindowRect( TRUE), TRUE );
		// ¿Ã¡¶ ¥Ÿ ≥°≥µ¥Ÿ. ∆ƒ±´«œ¿⁄.
		SAFE_DELETE( pWndBase );
	}
	// ∆ƒ±´«“ ∂ß ¿Œ≈Õ∆‰¿ÃΩ∫ ¡§∫∏∞° ¿˙¿Âµ»¥Ÿ. µ˚∂Ûº≠ ∆ƒ±´ ¿Ã»ƒ save«œ±‚.
	if( m_bTitle == FALSE )
		SaveRegInfo( "regInfo.dat" );
	m_mapWndApplet.RemoveAll();
}

BOOL CWndMgr::Initialize(CWndBase* pParentWnd)
{
	CWndBase::m_resMng.Load( "ResData.inc" );
	Create( WBS_NOFRAME | WBS_MANAGER | WBS_NODRAWFRAME, CRect( 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT/* - TASKBAR_HEIGHT*/ ), pParentWnd, 100 );
	return TRUE;
}
void CWndMgr::OnInitialUpdate()
{
	m_timerDobe.Set( SEC( 2 ) );
	m_timerWarning.Set( WARNING_MILLISECOND, TRUE );
	m_timerWarning2.Set( WARNING_2_MILLISECOND, TRUE );
	m_timerShortcutWarning.Set( SHORTCUT_WARNING_MILLISECOND, TRUE );
	m_timerBanning.Set( BANNING_MILLISECOND, TRUE );
	AddAllApplet();

	m_dwSavePlayerTime = 0;

	char filename[MAX_PATH];
	for( int i=0; i<128; i++ )
	{
		FLSPrintf( filename, _countof( filename ), "Icon_CoolTime_%.3d.tga", i );
		g_pCoolTexArry[i] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice,  MakePath( DIR_ICON, filename ), 0xffff00ff );
	}

	for( int i=0; i<11; i++ )
	{
		FLSPrintf( filename, _countof( filename ), "Icon_ImgIncAni_%.2d.tga", i );
		g_pEnchantTexArry[i] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice,  MakePath( DIR_ICON, filename ), 0xffffffff );
	}	
}

void CWndMgr::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{

}

void CWndMgr::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{

}

void CWndMgr::OnDestroyChildWnd( CWndBase* pWndChild )
{
	BOOL bFocus = TRUE;
	if( pWndChild && pWndChild->IsWndStyle( WBS_NOFOCUS ) )
		bFocus = FALSE;

	//_SUN_UPGRADE_WNDMANAGER
	for( vecWndIter iter = m_vecChildWnds.begin(); iter != m_vecChildWnds.end(); ++iter )
	{
		CWndBase** ppWnd = *iter;
		if( pWndChild == *ppWnd )
		{
			SAFE_DELETE( *ppWnd );
			pWndChild = NULL;
			break;
		}
	}

	if( pWndChild )
	{
		CWndNeuz* pWndBase;
#ifndef __IMPROVE_MAP_SYSTEM
		if( pWndChild->GetWndId() == APP_MAP_EX )
		{
			CWndMap* pWndMap = (CWndMap*) pWndChild;
			if( m_mapMap.Lookup( pWndMap->m_szMapFile, (void*&) pWndMap ) == TRUE )
			{
				m_mapMap.RemoveKey( pWndMap->m_szMapFile );
				SAFE_DELETE( pWndMap );
				//return;
			}
		}
		else
#endif // __IMPROVE_MAP_SYSTEM
		if( pWndChild->GetWndId() == APP_PARTY_CONFIRM )
		{
			SAFE_DELETE( pWndChild );
			//return;
		}
		else
		if( m_mapWndApplet.Lookup( pWndChild->GetWndId(), (void*&) pWndBase ) == TRUE )
		{
			m_mapWndApplet.RemoveKey( pWndChild->GetWndId() );
			// æ÷«√∑ø¿∏∑Œ µÓ∑œµ» ¿©µµ∏∏ ∆ƒ±´«“ ∂ß ¿©µµ¿« ¡§∫∏∏¶ ¿˙¿Â«—¥Ÿ. 
			if( pWndBase->IsPutRegInfo() )
				PutRegInfo( pWndBase, FALSE );
			SAFE_DELETE( pWndBase );
			pWndChild = NULL;
			//return;
		}
		else
		if( pWndChild->GetWndId() == APP_MESSAGE )
		{
			CWndMessage* pWndMessage = (CWndMessage*) pWndChild;
			if( m_mapMessage.Lookup( pWndMessage->m_strPlayer, (void*&) pWndBase ) == TRUE )
			{
				if( pWndBase != pWndMessage )
					FLERROR_LOG( PROGRAM_NAME, _T( "APP_MESSAGE_Vampyre" ) );
				m_mapMessage.RemoveKey( pWndMessage->m_strPlayer );
				SAFE_DELETE( pWndBase );
				pWndMessage = NULL;
				//SAFE_DELETE( pWndMessage );
				//return;
			}
		}
		else
		if( pWndChild->GetWndId() == APP_INSTANTMSG )
		{
			CWndInstantMsg* pWndMessage = (CWndInstantMsg*) pWndChild;
			if( m_mapInstantMsg.Lookup( pWndMessage->m_strPlayer, (void*&) pWndBase ) == TRUE )
			{
				if( pWndBase != pWndMessage )
					FLERROR_LOG( PROGRAM_NAME, _T( "APP_INSTANTMSG_Vampyre" ) );
				m_mapInstantMsg.RemoveKey( pWndMessage->m_strPlayer );
				SAFE_DELETE( pWndBase );
				pWndMessage = NULL;
				//SAFE_DELETE( pWndMessage );
				//return;
			}
		}
	}
	if( m_bTitle == FALSE && bFocus )
		SetFocus();

	SAFE_DELETE( m_pQuestItemInfo );
	SAFE_DELETE( m_pWndTextBook );
	SAFE_DELETE( m_pWndTextScroll );
	SAFE_DELETE( m_pWndTextLetter );
	
}	
void CWndMgr::OpenTextBook( CWndBase* pWndParent , FLItemBase* pItemBase )
{
	SAFE_DELETE( m_pWndTextBook );
	m_pWndTextBook = new CWndTextBook;
	m_pWndTextBook->Initialize( pWndParent, pItemBase );
}
void CWndMgr::OpenTextScroll( CWndBase* pWndParent, FLItemBase* pItemBase )
{
	SAFE_DELETE( m_pWndTextScroll );
	m_pWndTextScroll = new CWndTextScroll;
	m_pWndTextScroll->Initialize( pWndParent, pItemBase );
}
void CWndMgr::OpenTextLetter( CWndBase* pWndParent, FLItemBase* pItemBase )
{
	SAFE_DELETE( m_pWndTextLetter );
	m_pWndTextLetter = new CWndTextLetter;
	m_pWndTextLetter->Initialize( pWndParent, pItemBase );
}
void CWndMgr::ChangeTextBook( FLItemBase* pItemBase )
{
	m_pWndTextBook->SetItemBase( pItemBase );
}
void CWndMgr::ChangeTextScroll( FLItemBase* pItemBase )
{
	m_pWndTextScroll->SetItemBase( pItemBase );
}
void CWndMgr::ChangeTextLetter( FLItemBase* pItemBase )
{
	m_pWndTextLetter->SetItemBase( pItemBase );
}


void CWndMgr::ChangeQuestItemInfo(FLItemBase * pItemBase)
{
	m_pQuestItemInfo->SetItemBase(pItemBase);
}


//---------------------------------------------------------------------------------------------
// ∏∂µÂ∏Æ∞•¿« º±π∞ ¿©µµøÏ ª˝º∫
// param	: void
// return	: void
//---------------------------------------------------------------------------------------------
void CWndMgr::OpenMadrigalGift()
{
	if( CS_VER1 == _GetContentState( CT_MADRIGAL_GIFT ) )
		CreateApplet( APP_MADRIGALGIFT );
}


//---------------------------------------------------------------------------------------------
// ƒ˘Ω∫∆Æ ∫¸∏• ¡§∫∏ ¿©µµøÏ ª˝º∫
// param	: void
// return	: void
//---------------------------------------------------------------------------------------------
void CWndMgr::OpenQuestQuickInfo()
{
	if( m_pWndQuestQuickInfo )
		SAFE_DELETE( m_pWndQuestQuickInfo );

	m_pWndQuestQuickInfo = new CWndQuestQuickInfo;
	FLERROR_ASSERT_LOG_RETURN_VOID( m_pWndQuestQuickInfo, _T("[¿©µµøÏ ∏ﬁ¥œ¡Æ] ƒ˘Ω∫∆Æ ∫¸∏• ¡§∫∏ ¿©µµøÏ ª˝º∫ Ω«∆–") );

	m_pWndQuestQuickInfo->Initialize();
	m_pWndQuestQuickInfo->SetVisible( FALSE );
}


//---------------------------------------------------------------------------------------------
// ƒ˘Ω∫∆Æ æ∆¿Ã≈€ ¡§∫∏ ¿©µµøÏ ª˝º∫
// param	: void
// return	: void
//---------------------------------------------------------------------------------------------
void CWndMgr::OpenQuestItemInfo(CWndBase* pWndParent, FLItemBase *pItemBase)
{
	SAFE_DELETE(m_pQuestItemInfo);
	m_pQuestItemInfo = new CWndQuestItemInfo;
	m_pQuestItemInfo->Initialize(pWndParent, pItemBase);
}

//---------------------------------------------------------------------------------------------
// ≈∏¿Ã∆≤ ø≠±‚
// param	: √≥¿Ω¿Œ∞°?
// return	: void
//---------------------------------------------------------------------------------------------
void CWndMgr::OpenTitle( BOOL bFirstTime )
{
	if( m_bTitle == TRUE )
		return;

	// √≥¿Ω Ω««‡«œ¥¬∞‘ æ∆¥œ∂Û∏È ∞‘¿” ¡ﬂ ∑Œ±◊æ∆øÙ«— ∞Õ¿Ã¥Ÿ. ±◊∑∏¥Ÿ∏È «ˆ¿Á ø≠∑¡ ¿÷¥¬ « µÂ∏¶ ªË¡¶«—¥Ÿ.
	if( bFirstTime == FALSE )
	{
		g_Option.Save( "neuz.ini" );
		DestroyApplet();
		Free();

		g_WorldMng.DestroyCurrentWorld();
		g_pPlayer =  NULL;

		CMover::SetActiveObj( NULL );

		g_dpCertified.DeleteDPObject();
		g_dpLoginClient.DeleteDPObject();
		g_DPlay.DeleteDPObject();
		
		if( g_pBipedMesh )
		{
			g_pBipedMesh->DeleteDeviceObjects();
			SAFE_DELETE( g_pBipedMesh );
		}

		g_Party.InitParty();
		g_GuildWarMng.Clear();
		g_GuildMng.Clear();
		g_GuildCombatMng.GuildCombatClear( 99 );
		CPost::GetInstance()->Clear();
		CCoupleHelper::Instance()->Clear();					
		CRainbowRace::GetInstance()->m_dwRemainTime = 0;	
		CPlayerDataCenter::GetInstance()->Clear();			
		GuildHouse->Clear();

		SAFE_DELETE( m_pWndQuestQuickInfo );

		g_QuestTreeInfoManager.DeleteAllTreeInformation();
		prj.m_setBlockedUserID.clear();
	}

	// µ•Ω∫≈©≈æ ªÁ¿Ã¡Ó ºº∆√ 
	g_WndMng.SetWndRect( CRect( 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT ) );
	m_bTitle = TRUE;

	ObjectExecutor( SHORTCUT_APPLET, APP_LOGIN );

	CWndBase::m_nAlpha = 255;
	m_bTitle = TRUE;
}


//---------------------------------------------------------------------------------------------
// « µÂ ø≠±‚
// param	: void
// return	: void
//---------------------------------------------------------------------------------------------
void CWndMgr::OpenField()
{
	m_bAutoRun = FALSE;
	
	memset(g_Neuz.m_aSavedInven, 0, sizeof(g_Neuz.m_aSavedInven));
	memset( g_bSlotSwitchAboutEquipItem, FALSE, sizeof( g_bSlotSwitchAboutEquipItem ) );

	g_WndMng.SetWarningCounter( 0 );
	g_WndMng.SetWarning2Counter( 0 );

	if( m_bTitle == FALSE )
		return;

	StopMusic();

	for( int i = 0; i < MAX_CHARACTER_SLOT; i++ )
	{
		if( g_Neuz.m_apPlayer[i] )
			g_Neuz.m_apPlayer[i]->m_pModel = NULL; 
// Test start
		SAFE_DELETE( g_Neuz.m_apPlayer[i] );
// Test end
	}

	g_DialogMsg.ClearAllMessage();
	SetWndRect( g_Neuz.GetDeviceRect() );
	
	m_aChatString.RemoveAll();
	m_aChatColor.RemoveAll();
	m_aChatStyle.RemoveAll();

	LoadRegInfo( "regInfo.dat" );

	ObjectExecutor( SHORTCUT_APPLET, APP_WORLD );
	ObjectExecutor( SHORTCUT_APPLET, APP_COMMUNICATION_CHAT );
	ObjectExecutor( SHORTCUT_APPLET, APP_NAVIGATOR );
	ObjectExecutor( SHORTCUT_APPLET, APP_STATUS1 );

	OpenMadrigalGift();
	OpenTaskBar();

	// ∞¯¡ˆ∞° enable¿Ã∂Û∏È?
	if( g_Option.m_bNotice )
	{
		switch(_GetContentState(CT_INFO_NOTICE))
		{
		case CS_VER1:
			ObjectExecutor( SHORTCUT_APPLET, APP_INFO_NOTICE );
			break;
		case CS_VER2:
			g_WndMng.m_pWndWorld->GetAdvMgr()->AddAdvButton(APP_NOTICE);
			break;
		}
	}
	else
	// ∞¯¡ˆ∞° ø≠¡ˆ æ ¿Ω¿∏∑Œ µ«æÓ¿÷æÓµµ ≥Ø¬•∞° ∞ªΩ≈µ«æ˙¥Ÿ∏È ∞≠¡¶ ø¿«¬
	{
		CFileStatus fileStatus;
		
		CString strFilter;
		strFilter = GetLangFileName( g_xFlyffConfig->GetMainLanguage(), FILE_NOTICE );
		
		if( CFile::GetStatus( "Client\\"+strFilter, fileStatus ) == TRUE )
		{
			if( g_Option.m_tNoticeTime != fileStatus.m_mtime.GetTime() )
			{
				g_Option.m_tNoticeTime = (time_t)( fileStatus.m_mtime.GetTime() );
				g_Option.m_bNotice = TRUE;
				switch(_GetContentState(CT_INFO_NOTICE))
				{
				case CS_VER1:
					ObjectExecutor( SHORTCUT_APPLET, APP_INFO_NOTICE );
					break;
				case CS_VER2:
					ObjectExecutor( SHORTCUT_APPLET, APP_NOTICE);
					break;
				}
			}
		}
	}

	CWndBase::m_nAlpha = g_Option.m_nWindowAlpha;

	OpenQuestQuickInfo();

	m_bTitle = FALSE;

}

BOOL CWndMgr::OnCommand(UINT nID,DWORD dwMessage)
{
	return TRUE;
}

void CWndMgr::CloseMessageBox()
{
	SAFE_DELETE( m_pWndMessageBox );
	SAFE_DELETE( m_pWndMessageBoxUpper );
}

BOOL CWndMgr::OpenCustomBox( LPCTSTR lpszMessage, CWndMessageBox* pWndMessageBox, CWndBase* pWndParent ) 
{ 
	SAFE_DELETE( m_pWndMessageBox );
	m_pWndMessageBox = pWndMessageBox;
	pWndParent = this;

	if(pWndMessageBox->Initialize( pWndParent ) == FALSE)
	{
		SAFE_DELETE( m_pWndMessageBox );
		return TRUE; 
	}
	return FALSE;
}

BOOL CWndMgr::OpenMessageBox( LPCTSTR lpszMessage, UINT nType, CWndBase* pWndParent ) 
{ 
	SAFE_DELETE( m_pWndMessageBox );

	m_pWndMessageBox = new CWndMessageBox;
	if(pWndParent == NULL)
	{
		pWndParent = this;
	}

	if( m_pWndMessageBox->Initialize( lpszMessage, pWndParent, nType ) == FALSE) 
	{	
		SAFE_DELETE( m_pWndMessageBox );
		return FALSE; 
	}
	return TRUE;
}

BOOL CWndMgr::OpenMessageBoxWithTitle( LPCTSTR lpszMessage, const CString& strTitle, UINT nType, CWndBase* pWndParent )
{
	SAFE_DELETE( m_pWndMessageBox );
	m_pWndMessageBox = new CWndMessageBox;
	pWndParent = this;
	if( m_pWndMessageBox->Initialize( lpszMessage, pWndParent, nType ) == FALSE)
	{
		SAFE_DELETE( m_pWndMessageBox );
		return TRUE;
	}
	m_pWndMessageBox->SetTitle( strTitle );
	return FALSE;
}

BOOL CWndMgr::OpenMessageBoxUpper( LPCTSTR lpszMessage, UINT nType, BOOL bPostLogoutMsg ) 
{ 
	CWndBase* pWndParent = NULL;
	SAFE_DELETE( m_pWndMessageBoxUpper );
	m_pWndMessageBoxUpper = new CWndMessageBoxUpper;
	pWndParent = this;
	if( m_pWndMessageBoxUpper->Initialize( lpszMessage, pWndParent, nType, bPostLogoutMsg ) == FALSE) 
	{
		SAFE_DELETE( m_pWndMessageBoxUpper );
		return TRUE; 
	}
	return FALSE;
}
BOOL CWndMgr::OpenTaskBar() 
{ 
	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		if(m_pWndTaskBar)
			return FALSE;

		m_pWndTaskBar = new CWndTaskBar;	
		if(m_pWndTaskBar->Initialize(this) == FALSE) 
		{
			SAFE_DELETE( m_pWndTaskBar );
			return TRUE; 
		}
	}
	else if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	{
		m_pWndTaskBar = ( CWndTaskBar_HUD* )g_WndMng.CreateApplet( APP_NEWTASKBAR );
		FLASSERT( m_pWndTaskBar );
		g_WndMng.EnableHUD( TRUE );
	}

	
	return TRUE;
}
BOOL CWndMgr::OpenMenu() 
{
	if( m_pWndMenu )
	{
		if( m_pWndMenu->IsVisible() )
			m_pWndMenu->SetVisible( FALSE );
		else
		{
			m_pWndMenu->SetVisible( TRUE );
			m_pWndMenu->SetFocus();
		}
		return FALSE;
	}
	m_pWndMenu = new CWndTaskMenu;
	if( m_pWndMenu->Initialize( this ) == FALSE ) 
	{
		SAFE_DELETE( m_pWndMenu );
		return TRUE; 
	}
	return TRUE;
}
void CWndMgr::OnSize(UINT nType, int cx, int cy)
{
	CWndBase::OnSize(nType,cx,cy);
}

HRESULT CWndMgr::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	return S_OK;
}

HRESULT CWndMgr::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return S_OK;
}

HRESULT CWndMgr::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	return 0;
}

AppletFunc* CWndMgr::GetAppletFunc( DWORD dwIdApplet )
{
	AppletFunc* pApplet( NULL );

	if( m_mapAppletFunc.Lookup( dwIdApplet, (void*&) pApplet ) )
		return pApplet;

	return NULL;
}

DWORD CWndMgr::GetAppletId( TCHAR* lpszAppletName )
{
	AppletFunc*		pApplet( NULL );
	DWORD			dwIdApplet( NULL );
	POSITION		pos = m_mapAppletFunc.GetStartPosition();
	while( pos )
	{
		m_mapAppletFunc.GetNextAssoc( pos, dwIdApplet, (void*&)pApplet );
		if( !_tcscmp( pApplet->m_pAppletName, lpszAppletName ) )
			return pApplet->m_dwIdApplet;
	}

	return 0xffffffff;
}

CWndBase* CWndMgr::GetApplet( DWORD dwIdApplet )
{
	CWndBase* pWndBase( NULL );

	if( m_mapWndApplet.Lookup( dwIdApplet, (void*&)pWndBase ) == FALSE )
		return NULL;

	return pWndBase;
}

void CWndMgr::__HotKeyChange(DWORD dwId, char *pch)
{
	if( g_Option.m_nInterface == 2 )
	{
		switch( dwId )
		{
		case APP_NAVIGATOR:
			*pch = 0;
			break;
		case APP_STATUS1:
			*pch = 0;
			break;
//sun: 8, // __S8_VENDOR_REVISION
		case APP_VENDOR_REVISION:
//		case APP_VENDOREX:
			*pch = 0;
			break;
		case APP_QUEST_EX_LIST:
			*pch = 'L';
			break;
		case APP_MESSENGER_:
			*pch = 'T';
			break;
		}
	}	
}

void CWndMgr::AddAppletFunc( LPVOID pAppletFunc, DWORD dwIdApplet, LPCTSTR lpszAppletName, LPCTSTR pszIconName, LPCTSTR lpszAppletDesc, CHAR cHotkey )
{
	AppletFunc* pApplet = new AppletFunc;
	pApplet->m_pFunc       = (CWndBase* (*)()) pAppletFunc;
	pApplet->m_dwIdApplet  = dwIdApplet ;
	pApplet->m_pAppletName = lpszAppletName;
	pApplet->m_pAppletDesc = lpszAppletDesc;
	pApplet->m_pszIconName = pszIconName; 
#ifdef __Y_INTERFACE_VER3
	__HotKeyChange( dwIdApplet, &cHotkey );
#endif //__Y_INTERFACE_VER3
	pApplet->m_cHotkey = cHotkey; 
	m_mapAppletFunc.SetAt( dwIdApplet, pApplet );
}
CWndBase* CWndMgr::CreateApplet( DWORD dwIdApplet )
{
	CWndNeuz* pWndBase = NULL;
	AppletFunc* pAppletFunc = NULL;
	if( m_mapAppletFunc.Lookup( dwIdApplet, (void*&)pAppletFunc ) == FALSE )
	{
		// Applet¿Ã ¡∏¿Á«œ¡ˆ æ ¿ª ∞ÊøÏ ª˝º∫ 
		pAppletFunc = GetAppletFunc( dwIdApplet );
	}
	if( pAppletFunc )
	{
		CWndWorld* pWndWorld = (CWndWorld*)GetWndBase( APP_WORLD );
//sun: 12, ∆©≈‰∏ÆæÛ ∞≥º±
		CWndGuideSystem* pWndGuide = NULL;
		pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
		if(pWndGuide && pWndGuide->IsVisible()) 
		{
			if(pWndGuide->m_CurrentGuide.m_nVicCondition == OPEN_WINDOW && pWndGuide->m_CurrentGuide.m_nInput == (int)dwIdApplet)
				pWndGuide->m_Condition.nOpenedWindowID = 1;
		}

		pWndBase = (CWndNeuz*)GetWndBase( pAppletFunc->m_dwIdApplet );
		if( pWndBase == NULL )
		{
			pWndBase = (CWndNeuz*)(*pAppletFunc->m_pFunc)();
			if( pWndBase )
			{
				if( pWndBase->Initialize( this, dwIdApplet ) == FALSE )  
				{
					SAFE_DELETE( pWndBase );
					return pWndBase;
				}
				LPWNDREGINFO pWndRegInfo;
				if( m_mapWndRegInfo.Lookup( dwIdApplet, (void*&)pWndRegInfo ) == TRUE )
				{
					if( pWndRegInfo->dwSize )
					{
						// load
						CAr ar( pWndRegInfo->lpArchive, pWndRegInfo->dwSize );
						((CWndNeuz*)pWndBase)->SerializeRegInfo( ar, pWndRegInfo->dwVersion );
					}
					if( pWndBase->IsWndStyle( WBS_THICKFRAME ) )
					{
						if( pWndRegInfo->dwWndSize == WSIZE_WINDOW )
						{
							((CWndNeuz*)pWndBase)->SetSizeWnd();
							pWndBase->SetWndRect( pWndRegInfo->rect );
						}								
						if( pWndRegInfo->dwWndSize == WSIZE_MAX )
							((CWndNeuz*)pWndBase)->SetSizeMax();
					}
					else
					{
						CRect wndRect = pWndBase->GetWindowRect();
						pWndBase->SetWndRect( 
							CRect( 
							pWndRegInfo->rect.left, 
							pWndRegInfo->rect.top, 
							pWndRegInfo->rect.left + wndRect.Width(), 
							pWndRegInfo->rect.top + wndRect.Height() 
							) );
					}							
					//pWndBase->SetWndRect( pWndRegInfo->rect );
				}
				//LPWNDAPPLET lpApplet = m_resMng.GetAt( dwIdApplet );
				//if( lpApplet ) 
				//	pWndBase->SetToolTip( m_resMng.GetAt( dwIdApplet )->strToolTip );
				//pWndBase->SetToolTip( pAppletFunc->m_pAppletDesc );
			
				m_mapWndApplet.SetAt( dwIdApplet, pWndBase );
			}
		}
		else
			pWndBase->SetFocus();
	}
	return pWndBase;
}
void CWndMgr::ObjectExecutor( DWORD dwShortcut, DWORD dwId, DWORD dwType )
{
	SHORTCUT shortcut;
	ZeroMemory( &shortcut, sizeof( shortcut ) );
	shortcut.m_dwShortcut = dwShortcut;
	shortcut.m_dwId = dwId;
	shortcut.m_dwType = dwType;
	ObjectExecutor( &shortcut);
}
void CWndMgr::ObjectExecutor( LPSHORTCUT pShortcut )
{
	pShortcut->m_nExtraFrame = 60;		//..«¡∑π¿”µøæ» »ø∞˙;;

	if( pShortcut->m_dwShortcut == SHORTCUT_APPLET )
	{
		if(g_WndMng.GetWndBase( APP_DIALOG_EX ) ||
		   g_WndMng.GetWndBase( APP_MAP_TELEPORT ))
		   return;

#ifdef __SKILL_UI16
		if( pShortcut->m_dwId == APP_SKILL4 )
#else //sun: 9¬˜ ¿¸Ω¬∞¸∑√ Clienet
		if( pShortcut->m_dwId == APP_SKILL3 )
#endif //__SKILL_UI16
		{
			CWndBase* pWndBase = (CWndBase*)g_WndMng.GetWndBase( APP_QUEITMWARNING );	
			if( pWndBase )
				return;			// Ω∫≈≥ »Æ¿Œ√¢¿Ã∏È Ω∫≈≥√¢¿ª ¥›¿ªºˆ∞° æ¯¿Ω.

#ifdef __SKILL_UI16
			CWndSkill_16* pWndBase1 = (CWndSkill_16*)g_WndMng.GetWndBase( APP_SKILL4 );
#else //sun: 9¬˜ ¿¸Ω¬∞¸∑√ Clienet
			CWndSkillTreeEx* pWndBase1 = (CWndSkillTreeEx*)g_WndMng.GetWndBase( APP_SKILL3 );
#endif //__SKILL_UI16
			if( pWndBase1 && pWndBase1->GetCurrSkillPoint() != g_pPlayer->m_nSkillPoint )
			{
				if( pWndBase == NULL )
				{
					SAFE_DELETE(g_WndMng.m_pWndReSkillWarning);
					g_WndMng.m_pWndReSkillWarning = new CWndReSkillWarning;
					g_WndMng.m_pWndReSkillWarning->Initialize(NULL);
					g_WndMng.m_pWndReSkillWarning->InitItem( TRUE );
				}
				return;
			}
		}
		
		if( pShortcut->m_dwId == APP_QUIT)
		{
			OpenCustomBox( NULL, new CWndQuit );
		}
		else
		if( pShortcut->m_dwId == APP_LOGOUT)
		{
			// ∞¯∞›¡ﬂ¿Ã∏È ∑Œ±◊æ∆øÙ ∫“∞°
			if( g_pPlayer->IsAttackMode() )
			{
				g_WndMng.PutString( prj.GetText(TID_PK_COMBAT_END),NULL,0xffff0000);//, CHATSTY_SYSTEM_CHAT );
				return;
			}
			else
			{
				//_SUN_JAPAN_HANGAME_UI
				if( 0 && g_xFlyffConfig->GetMainLanguage() == LANG_JAP && g_Neuz.m_bThroughPortal == TRUE )
					OpenCustomBox( NULL, new CWndQuit );
				else
					OpenCustomBox( NULL, new CWndLogOut );
			}
		}
		//sun: 8¬˜ ∆ƒ∆º√¢ ∞¸∑√ ¿˚øÎ Neuz
		else if(pShortcut->m_dwId == APP_PARTY)
		{
			CWndBase* pWndParty = (CWndBase*)g_WndMng.GetApplet( APP_PARTY );
			CWndBase* pWndPartyQuick = (CWndBase*)g_WndMng.GetApplet( APP_PARTY_QUICK );
			if(pWndParty == NULL && pWndPartyQuick == NULL)
			{
				CreateApplet( APP_PARTY ); //Party√¢¿Ã ∏µŒ æ¯¿ª ∞ÊøÏ APP_PARTY√¢ ª˝º∫.
				if(g_Party.GetSizeofMember() >= 2) //PartyQuick√¢¿∫ Party Member∞° ¿÷¿ª ∞ÊøÏ∏∏ ª˝º∫.
				{
					CreateApplet( APP_PARTY_QUICK );
				}
			}
			else if(pWndParty != NULL && pWndPartyQuick != NULL) 
			{
				if(pWndParty && pWndPartyQuick)//Party√¢¿Ã ∏µŒ ¿÷¿ª ∞ÊøÏ PARTY√¢ ¿¸∫Œ ∆ƒ±´.
				{
					pWndParty->Destroy();
					pWndPartyQuick->Destroy();
					SetFocus();
				}
			}
			else //Party√¢¿Ã «œ≥™∂Ûµµ ƒ—¡Æ¿÷¥Ÿ∏È ƒ—¡¯ √¢∏∏ ∆ƒ±´.
			{
				if(pWndParty != NULL)
				{
					pWndParty->Destroy();
					SetFocus();	
				}
				else if(pWndPartyQuick != NULL)
				{
					pWndPartyQuick->Destroy();
					SetFocus();	
				}
			}
		}
		else if ( APP_WEBBOX2 == pShortcut->m_dwId )
		{
			CWndHelperWebBox* pBox = ( CWndHelperWebBox* ) g_WndMng.GetApplet( APP_WEBBOX2 );
			if( !pBox )
				pBox = (CWndHelperWebBox*)g_WndMng.CreateApplet( APP_WEBBOX2 );
				
			pBox->SetMode( CWndHelperWebBox::WMODE_HELPER );
		}
		else if ( APP_WEBBOX2 == pShortcut->m_dwId )
		{
			CWndHelperWebBox* pBox = ( CWndHelperWebBox* ) g_WndMng.GetApplet( APP_WEBBOX2 );
			if( !pBox )
				pBox = (CWndHelperWebBox*)g_WndMng.CreateApplet( APP_WEBBOX2 );

			pBox->SetMode( CWndHelperWebBox::WMODE_HELPER );
		}
		else
		{
			CWndBase* pWndBase = (CWndBase*)g_WndMng.GetApplet( pShortcut->m_dwId );
			if( pWndBase )
			{
//sun: 12, ∆©≈‰∏ÆæÛ ∞≥º±
				if(pShortcut->m_dwId == APP_INFOPANG && !pWndBase->IsVisible())
				{
					pWndBase->OnInitialUpdate();
					//pWndBase->Destroy();
					//CreateApplet( pShortcut->m_dwId );
					SetFocus();
				}
				else
				{
					pWndBase->Destroy();
					SetFocus();
				}
			}
			else if( pShortcut->m_dwId == APP_VENDOR_REVISION )		//sun: 8¬˜ ø£¡© º“»Ø Neuz, World, Trans
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_SUMMON_ANGEL );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_SUMMONANGELUSING ), NULL, prj.GetTextColor( TID_GAME_SUMMONANGELUSING ) );
				}
				else
					CreateApplet( pShortcut->m_dwId );
			}
			else if( CreateApplet( pShortcut->m_dwId ) == FALSE )
			{
				FLERROR_LOG( PROGRAM_NAME, _T("[¿©µµøÏ ∏ﬁ¥œ¡Æ] æ÷«√∏¥ ª˝º∫¿ª Ω«∆–«œø¥Ω¿¥œ¥Ÿ. ID - [ %u ]"), pShortcut->m_dwId );
			}
		}
	}
	else if( pShortcut->m_dwShortcut == SHORTCUT_CHAT )
	{
		{
		//	m_timerDobe.Reset();
			CWndWorld* pWndWorld = (CWndWorld*)GetWndBase( APP_WORLD );

			m_bShortcutCommand = TRUE;
			g_WndMng.ParsingChat( pShortcut->m_szString );
		}
	}
	else if( pShortcut->m_dwShortcut == SHORTCUT_EMOTICON )
	{
		TCHAR szCmd[1024] = { 0 };
		FLStrcat( szCmd, _countof( szCmd ), "/" );
		FLStrcat( szCmd, _countof( szCmd ), g_DialogMsg.m_EmiticonCmd[pShortcut->m_dwId].m_szCommand );
		g_WndMng.ParsingChat( szCmd );
	}
	else if( pShortcut->m_dwShortcut == SHORTCUT_ITEM  )
	{
		if( CMover::GetActiveMover()->m_vtInfo.VendorIsVendor() )
			return;

		FLItemBase* pItemBase = g_pPlayer->GetItemId( pShortcut->m_dwId );
		if( IsUsableItem( pItemBase ) )
		{
			CCtrl* pCtrl = (CCtrl*)g_WorldMng.Get()->GetObjFocus();
			DWORD dwObjId = NULL_ID;
			if( pCtrl && pCtrl->GetType() != OT_OBJ )
				dwObjId = pCtrl->GetId();

			BOOL bAble = TRUE;
			if( pItemBase )
			{
				if( pItemBase->GetProp()->dwItemKind1 == IK1_RIDE )	// ªÁøÎ«— æ∆¿Ã≈€¿Ã ≈ª∞Õ¿Œ∞°.
					if( g_pPlayer->m_pActMover->IsFly() == FALSE )		// GroundªÛ≈¬¿Œ∞°?
						if( g_pPlayer->m_pActMover->m_bGround == 0 )		// ∂•ø° πﬂ¿ª æ»∫Ÿ¿Ã∞Ì ¿÷¥Ÿ∏È ∏¯≈∫¥Ÿ.
							bAble = FALSE;	// æ∆¿Ã≈€ªÁøÎ Ω«∆–.
			}

			if( bAble )	// º∫∞¯∆«¡§ ≥µ¿ª∂ß∏∏ ∫∏≥ø.
			{
				FLItemElem* pItemElem = ( FLItemElem* )pItemBase;
		
				if( g_WndMng.GetWndBase( APP_DIALOG_EX ) ||
					g_WndMng.GetWndBase( APP_DIALOG ) ||
					g_WndMng.GetWndBase( APP_DIALOG_EVENT ) ||
					g_WndMng.GetWndBase( APP_INVEN_REMOVE_ITEM )||
					g_WndMng.GetWndBase( APP_MAP_TELEPORT ))
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_GUILDCOMBAT_NOT_USE), NULL, 0xffff0000 );				
				}
				else if( g_WndMng.GetWndBase( APP_SHOP_ )  ||
					g_WndMng.GetWndBase( APP_BANK )  ||
					g_WndMng.GetWndBase( APP_TRADE ) ||
					g_WndMng.GetWndBase( APP_SHOP_CART_ )||
					g_WndMng.GetWndBase( APP_VENDOR_REVISION ))
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_TRADELIMITUSING), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING) );
				}			
				else 
				{
					if( pItemElem != NULL )
					{
						PT_ITEM_SPEC pItemProp = pItemElem->GetProp();
						if( pItemProp != NULL )
						{
							//if( pItemElem->GetProp() && ( pItemElem->GetProp()->dwFlag & IP_FLAG_EQUIP_BIND ) && !pItemElem->IsFlag( FLItemElem::binds ) )
							if( pItemProp->nBindCondition == BIND_CONDITION_USE
								&& pItemProp->dwParts != NULL_ID
								&& pItemElem->IsFlag( FLItemElem::binds ) == FALSE
								&& pItemElem->IsFlag( FLItemElem::reset_bind ) == FALSE )
							{
								SAFE_DELETE( g_WndMng.m_pWndEquipBindConfirm )
									g_WndMng.m_pWndEquipBindConfirm = new CWndEquipBindConfirm( CWndEquipBindConfirm::EQUIP_DOUBLE_CLICK );
								g_WndMng.m_pWndEquipBindConfirm->SetInformation( pItemElem, dwObjId );
								g_WndMng.m_pWndEquipBindConfirm->Initialize( NULL );
							}
							else if( pItemProp->nResetBindCondition == BIND_CONDITION_USE
								&& pItemProp->dwParts != NULL_ID
								&& pItemElem->IsFlag( FLItemElem::binds ) == FALSE
								&& pItemElem->IsFlag( FLItemElem::reset_bind ) == TRUE )
							{
								SAFE_DELETE( g_WndMng.m_pWndEquipBindConfirm )
									g_WndMng.m_pWndEquipBindConfirm = new CWndEquipBindConfirm( CWndEquipBindConfirm::EQUIP_DOUBLE_CLICK );
								g_WndMng.m_pWndEquipBindConfirm->SetInformation( pItemElem, dwObjId );
								g_WndMng.m_pWndEquipBindConfirm->Initialize( NULL );
							}
							else
							{
								// ≈ª¬¯ ∏Ì∑…¿œ ∞ÊøÏ, nPart¥¬ Ω«¡¶ ¿Â¬¯µ«æÓ ¿÷¥¬ ∫Œ∫–∞˙ ¿œƒ°«ÿæﬂ «œπ«∑Œ «¡∑Œ∆€∆ºø°º≠ ≤®≥ª¡ˆ æ ¥¬¥Ÿ.
								int nPart = pItemBase->GetProp()->dwParts;
								BOOL bEquiped	= g_pPlayer->m_Inventory.IsEquip( pItemBase->m_dwObjId );
								if( bEquiped )
									nPart	= pItemBase->m_dwObjIndex;

								//gmpbigsun: ƒ¸ΩΩ∑‘ø°º≠ Ω««‡µ» ∞ÊøÏ ∫ÒΩ∫∞¸∑√ ƒ≥Ω¨¿Œ∞ÊøÏ ¿Á»Æ¿Œ√¢¿∏∑Œ.. 09_1222
								if( !CheckConfirm( pItemBase ) )		//gmpbigsun: ∆–≈∂∫∏≥ª±‚¿¸ »Æ¿ŒµÓ¿« √≥∏Æ¥„¥Á
									g_DPlay.SendDoUseItem( MAKELONG( 0, pItemBase->m_dwObjId ), dwObjId, nPart );
							}
						}					
					}
				}
			}
		}
	}
	else if( pShortcut->m_dwShortcut == SHORTCUT_SKILL )
	{
		if( CMover::GetActiveMover()->m_vtInfo.VendorIsVendor() )
			return;
		if( pShortcut->m_dwType == 2 )
		{
			if( g_Party.m_nSizeofMember >= MIN_PTMEMBER_SIZE && g_Party.GetKind() == PARTY_KIND_EXTEND )
			{
				SkillProp* pSkillProp =  prj.GetPartySkill( pShortcut->m_dwId );
				if( pSkillProp )
				{
					if( g_Party.IsLeader(g_pPlayer->m_idPlayer) )
					{
						if( int(g_Party.GetLevel() - pSkillProp->dwReqDisLV) >= 0 )
						{
							if( (g_Party.GetPoint() - pSkillProp->dwReqPartyPoint) >= 0 )
							{
								// Ω∫≈≥ªÁøÎ
								SkillProp* pSkillProp = prj.GetPartySkill( pShortcut->m_dwId );
//sun: 12, ±ÿ¥‹¿Ø∑·æ∆¿Ã≈€
								DWORD	dwSkillTimeTmp = 0;
								CWndWorld *pWndWorld = (CWndWorld *)g_WndMng.m_pWndWorld;
								for( BuffItr i = pWndWorld->m_buffs.m_listBuffs.begin(); i != pWndWorld->m_buffs.m_listBuffs.end(); ++i )
								{
									IBuff* ptr	= *i;
									if( ptr == NULL )
									{
										continue;
									}
									if( ptr->GetId() == pShortcut->m_dwId )	
										dwSkillTimeTmp = ptr->GetTotal();
								}
								if( dwSkillTimeTmp == 0 
									|| g_pPlayer->HasBuff( BUFF_ITEM, ITEM_INDEX( 26539, II_SYS_SYS_SCR_PARTYSKILLUP01 ) )
									|| g_pPlayer->HasBuff( BUFF_ITEM, ITEM_INDEX( 26540, II_SYS_SYS_SCR_PARTYSKILLUP02 ) )
									|| g_pPlayer->HasBuff( BUFF_ITEM, ITEM_INDEX( 20297, II_SYS_SYS_SCR_PARTYSKILLUP01_01 ) ) )
								{
									g_DPlay.SendPartySkillUse( pShortcut->m_dwId );
									m_dwSkillTime[pShortcut->m_dwId] = g_tmCurrent;
								}
								else
								{
									/*
									#define ST_CALL                            1
									#define ST_BLITZ                           2
									#define ST_RETREAT                         3
									#define ST_SCOUT                           4
									#define ST_SPHERECIRCLE                    5
									#define ST_LINKATTACK                      6
									#define ST_FORTUNECIRCLE                   7
									#define ST_STRETCHING                      8
									#define ST_GIFTBOX                         9
									*/
									if( pShortcut->m_dwId != SKILL_INDEX( 1, ST_CALL )    &&
										pShortcut->m_dwId != SKILL_INDEX( 2, ST_BLITZ )   &&
										pShortcut->m_dwId != SKILL_INDEX( 3, ST_RETREAT ) )
									{
										CString str;
										str.Format( prj.GetText( TID_GAME_NOT_TIME ), pSkillProp->szName );
										g_WndMng.PutString( (LPCTSTR)str, NULL, prj.GetTextColor( TID_GAME_NOT_TIME ) );
									}
								}
							}
							else g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0049) ) );
						}
						else // ∑æ¿Ã ∏¿⁄∂Û Ω∫≈≥¿ª ªÁøÎ«“ºˆ æ¯¿Ω
							g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0019) ) );
					}
					else // ¥‹¿Â¿Ã æ∆¥œ¥Ÿ
						g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0047) ) );
				}
			}
		}
		else
		{	// ºÙƒ∆¿Ã Ω∫≈≥¿œ ∞ÊøÏ
			UseSkillShortCut( pShortcut->m_dwType, pShortcut->m_dwId );
		}
	}
//sun: 12, ±∫¡÷
	else if( pShortcut->m_dwShortcut == SHORTCUT_LORDSKILL )
	{
		// ±∫¡÷Ω∫≈≥¿ª Ω««‡«—¥Ÿ
		CCtrl* pCtrl = (CCtrl*)g_WorldMng.Get()->GetObjFocus();
		DWORD dwObjId = NULL_ID;
		CCLord* pLord									= CCLord::Instance();
		CLordSkillComponentExecutable* pComponent		= pLord->GetSkills()->GetSkill(pShortcut->m_dwId);

		switch(pShortcut->m_dwId)
		{
			case SKILL_INDEX( 5, LI_SUMMON ):
			case SKILL_INDEX(  6, LI_TELEPORT ):
				if(pComponent->GetTick() == 0)
				{
					SAFE_DELETE(m_pWndLordSkillConfirm);
					m_pWndLordSkillConfirm = new CWndLordSkillConfirm(pShortcut->m_dwId);
					m_pWndLordSkillConfirm->Initialize();
				}
				else
				{
					PutString(prj.GetText(TID_GAME_STILLNOTUSE), NULL, prj.GetTextColor(TID_GAME_STILLNOTUSE));
				}
				break;

			default:
				if(!pCtrl)
				{
					g_DPlay.SendLordSkillUse(pShortcut->m_dwId, g_pPlayer->GetName());
				}
				else if(pCtrl && pCtrl->GetType() == OT_MOVER)
				{
					if(((CMover*)pCtrl)->IsPlayer())
						g_DPlay.SendLordSkillUse(pShortcut->m_dwId, ((CMover*)pCtrl)->GetName());
				}
				break;
		};
		
	}
	else if( pShortcut->m_dwShortcut == SHORTCUT_MOTION )
	{
		{
			CWndBase* pWndBase	= GetWndVendorBase();
			if( pWndBase ) // ªÛ¡°√¢¿Ã ø≠∏∞ ªÛ≈¬ø°º≠¥¬ ∏º« Ω««‡ ±›¡ˆ
				return;
		}
		MotionProp* pMotionProp = prj.GetMotionProp( pShortcut->m_dwId );
		if( pMotionProp->szName[ 0 ] )
		{
			FLStrcpy( m_szTimerChat, _countof( m_szTimerChat ), pMotionProp->szName );
			m_timerChat.Set( 1000 );
		}
		bool fBehavior	= false;
		if( pShortcut->m_dwId == MOT_BASE_PICKUP )
			g_pPlayer->DoPickupItemAround();
		else		
		if( pShortcut->m_dwId == MOT_BASE_BATTLE )
		{
			if( g_pPlayer->m_pActMover->IsStateFlag( OBJSTAF_COMBAT ) ) {
				if( g_pPlayer->SendActMsg( OBJMSG_MODE_PEACE ) == 1 ) {
					fBehavior	= true;
				}
			}
			else {
				if( g_pPlayer->SendActMsg( OBJMSG_MODE_COMBAT ) == 1 ) {
					fBehavior	= true;
				}
			}
		}
		else
		// æ…±‚/º≠±‚ ¿¸»Ø
		if( pShortcut->m_dwId == MOT_BASE_SIT )
		{
			if( g_pPlayer->m_pActMover->IsStateFlag( OBJSTAF_SIT ) ) {	// æ…æ∆¿÷¥¬ ªÛ≈¬¿œ∂ß
				if( FALSE == g_pPlayer->m_pActMover->IsActJump() &&
					FALSE == g_pPlayer->m_pActMover->IsActAttack() && 
					FALSE == g_pPlayer->m_pActMover->IsActDamage() &&
					FALSE == g_pPlayer->m_pActMover->IsAction() &&
					!( g_pPlayer->m_dwReqFlag & REQ_USESKILL ) )
				{
					g_DPlay.SendMotion( OBJMSG_STANDUP );
				}
			}
			else {
				if( FALSE == g_pPlayer->m_pActMover->IsActJump() &&
					FALSE == g_pPlayer->m_pActMover->IsActAttack() && 
					FALSE == g_pPlayer->m_pActMover->IsActDamage() &&
					FALSE == g_pPlayer->m_pActMover->IsAction() &&
					!( g_pPlayer->m_dwReqFlag & REQ_USESKILL ) )
				{
					g_DPlay.SendMotion( OBJMSG_SITDOWN );
				}
			}
		} 
		else
		if( pShortcut->m_dwId == MOT_BASE_COLLECT )
		{
//			CObj *pFocus = g_pPlayer->GetWorld()->GetObjFocus();	// «ˆ¿Á ¿‚»˘ ≈∏∞Ÿ.
//			g_pPlayer->CMD_SetCollect( pFocus );
		}
		else
		if( pShortcut->m_dwId == MOT_BASE_ESCAPE )
		{
			g_DPlay.SendDoEscape();
		}
		else
		if( pShortcut->m_dwId == MOT_BASE_CHEER )
		{
			CObj *pFocus = g_pPlayer->GetWorld()->GetObjFocus();	// «ˆ¿Á ¿‚»˘ ≈∏∞Ÿ.

			if( pFocus && pFocus->GetType() == OT_MOVER && ((CMover*)pFocus)->IsPlayer() )
			{
				g_DPlay.SendCheering( ((CMover*)pFocus)->GetId() );
				fBehavior	= false;
			}
			else
			{
				g_WndMng.PutString( prj.GetText( TID_CHEER_NO2 ), NULL, prj.GetTextColor( TID_CHEER_NO2 ) );
			}
		}
//sun: 8,     // 8¬˜ ≈∏∞Ÿ≈¨∏Ø¿⁄µø∞¯∞› ¿Ã∏∆ºƒ‹
		else
		if( pShortcut->m_dwId == MOT_BASE_AUTOATTACK )
		{
			CObj *pFocus = g_pPlayer->GetWorld()->GetObjFocus();	// «ˆ¿Á ¿‚»˘ ≈∏∞Ÿ.

			if(g_pPlayer->IsAttackAble(pFocus) != NULL)
			{
				if( pFocus && pFocus->GetType() == OT_MOVER )//&& ((CMover*)pFocus)->IsNPC() )«√∑π¿ÃæÓø°∞‘µµ æµºˆ¿÷¿Ω pkøÎ
				{
					PT_ITEM_SPEC pItemProp = g_pPlayer->GetActiveHandItemProp();
					if( pItemProp && pItemProp->dwItemKind3 == IK3_WAND ) //Wand¿œ ∞ÊøÏ AutoAttack¿ª «œ¡ˆ æ ¿Ω.
					{
						CCtrl* pFocusObj = (CCtrl*)pFocus;
						g_pPlayer->CMD_SetMagicAttack( pFocusObj->GetId(), 0 );
					}
					else
						((CWndWorld *)g_WndMng.m_pWndWorld)->m_bAutoAttack = TRUE;
				}
				else
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_NOTTARGET ), NULL, prj.GetTextColor( TID_GAME_NOTTARGET ) );
				}
			}
		}

//sun: 13, ƒø«√ Ω√Ω∫≈€
		else if( pShortcut->m_dwId == MOT_BASE_COUPLE )
		{
			CObj *pFocus = g_pPlayer->GetWorld()->GetObjFocus();	// «ˆ¿Á ¿‚»˘ ≈∏∞Ÿ.

			if( pFocus && pFocus->GetType() == OT_MOVER && ((CMover*)pFocus)->IsPlayer() )
			{
				// «¡∑Ø∆˜¡Ó ∫∏≥æ ∏ﬁºº¡ˆ √¢ √‚∑¬
				if(g_WndMng.m_pWndCoupleMessage)
					SAFE_DELETE(g_WndMng.m_pWndCoupleMessage);

				g_WndMng.m_pWndCoupleMessage = new CWndCoupleMessage;
				if(g_WndMng.m_pWndCoupleMessage)
				{
					CString strText;
					strText.Format(prj.GetText(TID_GAME_PROPOSETO), ((CMover*)pFocus)->GetName());
					g_WndMng.m_pWndCoupleMessage->SetMessageMod(strText, CWndCoupleMessage::CM_SENDPROPOSE, pFocus);
					g_WndMng.m_pWndCoupleMessage->Initialize(NULL);
				}
			}
			else
			{
				g_WndMng.PutString( prj.GetText( TID_GAME_NOTCOUPLETARGET ), NULL, prj.GetTextColor( TID_GAME_NOTCOUPLETARGET ) );
			}
		}
		else
		{
			// ±‚≈∏ ¿Ã∏º«.
			MotionProp *pProp = prj.GetMotionProp( pShortcut->m_dwId );
			if( pProp )
			{
				int nMotion = pProp->dwMotion;
				int nPlayMode = 0;
				switch( pProp->dwPlay )
				{
				case 0:	nPlayMode = ANILOOP_1PLAY;	break;
				case 1:	nPlayMode = ANILOOP_CONT;	break;
				case 2:	nPlayMode = ANILOOP_LOOP;	break;
				default: ANILOOP_1PLAY;
				}

				if( g_pPlayer->SendActMsg( OBJMSG_MOTION, nMotion, nPlayMode ) == 1 )
				{
					switch(nMotion)
					{
						case MTI_POINTWAYNORTH:
							g_pPlayer->SetAngle(180.0f);
							break;
						case MTI_POINTWAYSOUTH:
							g_pPlayer->SetAngle(0.0f);
							break;
						case MTI_POINTWAYEAST:
							g_pPlayer->SetAngle(90.0f);
							break;
						case MTI_POINTWAYWEST:
							g_pPlayer->SetAngle(270.0f);
							break;
					}
					fBehavior	= true;
				}
			}
		}
				
		if( fBehavior ) 
		{
			g_DPlay.SendEndSkillQueue();
			g_pPlayer->ClearDest();
			g_WorldMng()->SetObjFocus( NULL );
			g_DPlay.SendPlayerBehavior();
		}			
	}
	else
	if( pShortcut->m_dwShortcut == SHORTCUT_OBJECT )
	{
		CMover* CMover = prj.GetMover( pShortcut->m_dwId );
		if( CMover )
		{
			CWndWorld* pWndWorld = (CWndWorld*) GetWndBase( APP_WORLD );
			//if( g_WorldMng()->GetObjFocus() != CMover )
			//	pWndWorld->m_bNewSelect = TRUE;
			g_WndMng.OpenMessage( pShortcut->m_szString );

			//pWndMessage->AddMessage( lpString );

			//g_WorldMng()->SetObjFocus( CMover );
			

			/*
			if( pObj->GetType() == OT_MOVER )
			{
				CMover* pMover = (CMover*) pObj;
				// ¿⁄Ω≈¿Ã∂Û∏È ªÛ≈¬√¢  
				if( pMover == g_Neuz.m_pPlayer )
				{
					CreateApplet( APP_EQUIPMENT );
				}
				elseaaaaaaaaaaaaaaa
				// NPC∂Û∏È ¥Î»≠ »§¿∫ ¿¸≈ı 
				if( pMover->IsNPC() )
				{
					g_World.SetObjFocus( pMover );
					CMover* pMover = (CMover*) pObj;
					g_WndMng.CreateApplet( APP_DIALOG ); 					
					
					//CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet( APP_WORLD );
					//if( pWndWorld )
					//{
					//	if( pWndWorld->m_bBattle )
					//	{
						//}

				}
				else
				// «√∑π¿ÃæÓ∂Û∏È ¬ ¡ˆ ¿¸º€ 
				{

				}
			}
			*/
		}
	}
	else
	if( pShortcut->m_dwShortcut == SHORTCUT_SKILLFUN )
	{
		g_WndMng.m_pWndWorld->UseSkill();
	}
}

BOOL CWndMgr::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	return TRUE;
	//CRect rect = GetClientRect();
	//rect.DeflateRect( 5, 5);
	for( int i = 0; i < m_awndShortCut.GetSize(); i++)
	{
		if( ((CWndBase*)m_awndShortCut.GetAt( i ))->GetWndId() == pShortcut->m_dwIndex )
		{

			((CWndBase*)m_awndShortCut.GetAt( i ))->Move( CPoint( point.x - 12, point.y - 12 ) );
			return TRUE;
		}
	}
	CWndButton* pWndButton = new CWndButton;
	pWndButton->Create( pShortcut->m_szString,WBS_MENUITEM,CRect( point.x - 10, point.y - 10, point.x + 10, point.y + 10),this, pShortcut->m_dwIndex);
	pWndButton->m_shortcut.m_dwShortcut = pShortcut->m_dwShortcut;
	pWndButton->AddWndStyle( WBS_MOVE );
	m_awndShortCut.Add( pWndButton );
	pWndButton->m_strToolTip = pWndButton->GetTitle();
	return TRUE;
}
BOOL CWndMgr::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	if( message == WNM_CLICKED )
	{
	//	CWndButton* pWndButton = (CWndButton*)pLResult;
	//	ObjectExecutor( pWndButton->m_n,  nID );
	}
	return TRUE;
}

void CWndMgr::OnDraw( C2DRender* p2DRender )
{
}


/*
BOOL CWndMgr::WndMessageBoxToTitle(CString strMessage) 
{ 
	if(m_pWndMessageBox) delete m_pWndMessageBox;
	m_pWndMessageBox = new CWndToTitleBox;
	if(m_pWndMessageBox->Initialize(strMessage,this) == FALSE) 
	{
		delete m_pWndMessageBox;
		m_pWndMessageBox = NULL;
		return TRUE; 
	}
	return FALSE;
}


*/
BOOL CWndMgr::OnEraseBkgnd( C2DRender* p2DRender )
{
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet( APP_WORLD );
	static DWORD dwTimeGameGradeScreen = g_tmCurrent + SEC( 3 );
	if( g_xFlyffConfig->GetMainLanguage() == LANG_KOR && pWndWorld == NULL && g_tmCurrent <= dwTimeGameGradeScreen )
	{
		CWndLogin* pWndLogin = ( CWndLogin* )g_WndMng.GetWndBase( APP_LOGIN );
		if( pWndLogin )
		{
			pWndLogin->SetVisible( FALSE );
			pWndLogin->EnableWindow( FALSE );
		}

		m_pTheme->RenderGameGradeScreen( &g_Neuz.m_2DRender );
	}
	else if( pWndWorld == NULL && m_bTitle )
	{
		CWndLogin* pWndLogin = ( CWndLogin* )g_WndMng.GetWndBase( APP_LOGIN );
		if( pWndLogin )
		{
			pWndLogin->SetVisible( TRUE );
			pWndLogin->EnableWindow( TRUE );
		}

		if( m_pWndActiveDesktop )
		{
			m_pWndActiveDesktop->SetVisible( TRUE );
		}
		else
		{
			m_pTheme->RenderDesktop( &g_Neuz.m_2DRender );
		}
	}
	else
	if( pWndWorld )
	{
		//DWORD dwColor = CWorld::GetDiffuseColor();
		//m_pApp->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, dwColor /*D3DCOLOR_ARGB( 255, 255, 255, 255 )*/, 1.0f, 0 ) ;

		//if( pWndWorld->m_nWinSize != WSIZE_MAX )
		m_pApp->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, D3DCOLOR_ARGB( 255, 255, 0, 0 ), 1.0f, 0 ) ;
		// ø˘µÂ∞° »≠∏È¿ª øœ¿¸»˜ ∞°∏Æ∞Ì ¿÷¿ª ∂ß 
		if( m_pWndActiveDesktop )
			m_pWndActiveDesktop->SetVisible( FALSE );
	}
	else
		m_pApp->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, D3DCOLOR_ARGB( 255, 0, 0, 0 ), 1.0f, 0 ) ;
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	return TRUE;
}



//---------------------------------------------------------------------------------------------
// ESC∏¶ ¥≠∑Øµµ ¥›»˜¡ˆ æ ¿ª UI ∏Ò∑œ¿ª ¿Ã∞˜ø° µÓ∑œ«—¥Ÿ. ( ∆ƒ±´«œ¡ˆ æ ¿ª UI ¿Œ∞°? )
// param	: ¿©µµøÏ æ∆¿Ãµ
// return	: øπ / æ∆¥œø‰
//---------------------------------------------------------------------------------------------
bool CWndMgr::IsDontDestroy( DWORD dwWndId )
{
	if( dwWndId == APP_MINIGAME_KAWIBAWIBO ||
		dwWndId == APP_MINIGAME_KAWIBAWIBO_WIN ||
		dwWndId == APP_MINIGAME_DICE ||
		dwWndId == APP_COLLECTING ||
		dwWndId == APP_RR_MINIGAME_KAWIBAWIBO ||
		dwWndId == APP_RR_MINIGAME_DICE ||
		dwWndId == APP_RR_MINIGAME_ARITHMETIC ||
		dwWndId == APP_RR_MINIGAME_STOPWATCH ||
		dwWndId == APP_RR_MINIGAME_TYPING ||
		dwWndId == APP_RR_MINIGAME_CARD ||
		dwWndId == APP_RR_MINIGAME_LADDER ||
		dwWndId == APP_NEWTASKBAR04 ||
		dwWndId == APP_NEWTASKBAR03 ||
		dwWndId == APP_NEWTASKBAR02 ||
		dwWndId == APP_NEWTASKBAR ||
		dwWndId == APP_TARGETINFORNATION ||
		dwWndId == APP_MADRIGALGIFT ||
		dwWndId == APP_RANDOMOPTIONRETRY ||
		dwWndId == APP_STAT_DICE ||
		dwWndId == APP_COMMITEM_DIALOG
	  )
	{
		return true;
	}
	
	return false;
}


//---------------------------------------------------------------------------------------------
// √≥∏Æ
// param	: void 
// return	: º∫∞¯ / Ω«∆–
//---------------------------------------------------------------------------------------------
BOOL CWndMgr::Process()
{
	static DWORD dwTargetTime = g_tmCurrent + 5000;
	if( m_bWaitRequestMail && dwTargetTime < g_tmCurrent )		//¢¨®≠AIAI AO¢•U°∆i ©ˆ®≠°∆i, ¢¨®≠AIA¢¥®¨¢¨¢¨| ©ˆ®≠Ao ¢¨©™CNA¢¥ 5AE°∆¢Æ Ea¢¨°◊.
	{
		g_DPlay.SendQueryMailBox();
		dwTargetTime = g_tmCurrent + 5000;
	}

#ifdef __ZCK_INVASIONS
	CZckInvasions* pInvasionManager = CZckInvasions::GetInstance();
	pInvasionManager->ProcessClientSync();

	DWORD dwLocalInvasionID = NULL_ID;
	if( pInvasionManager->GetLocalInvasionID( dwLocalInvasionID ) )
	{
		if( m_pWndZckInvasions == NULL )
		{
			m_pWndZckInvasions = new CWndZckInvasions;
			if( m_pWndZckInvasions->Initialize( this, 0 ) == FALSE )
			{
				SAFE_DELETE( m_pWndZckInvasions );
			}
		}
		if( m_pWndZckInvasions != NULL )
			m_pWndZckInvasions->SetInvasionID( dwLocalInvasionID );
	}
	else
	{
		SAFE_DELETE( m_pWndZckInvasions );
	}
#endif // __ZCK_INVASIONS

#ifdef __BS_CONSOLE
		_update_console( );
#endif

	CWndRegVend*	pWndRegVend( (CWndRegVend*)g_WndMng.GetWndBase(APP_VENDOREX_SELL) );
	CWndRegVend*	pWndVendorBuy( (CWndRegVend*)g_WndMng.GetWndBase(APP_VENDOREX_BUY) );
	
	if( pWndRegVend ||
		pWndVendorBuy )
		return CWndBase::Process();
	
	const PFLSHORTCUTKEY_CONTROL	pShortcutKey_Control( g_xKeyManager->GetShortcutKey_Control() );
	const PFLSHORTCUTKEY_SYSTEM		pShortcutKey_System( g_xKeyManager->GetShortcutKey_System() );

	if( g_pPlayer )
	{
		if( g_bKeyTable[ pShortcutKey_System->KeyEscape ] && m_pLogOutWaitting == NULL )
		{
			// ®¨IE°∆ ¢¨A¢•®¨ 
			g_bKeyTable[ pShortcutKey_System->KeyEscape ] = 0;

			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
			CWorldMap* pWorldMap = CWorldMap::GetInstance();
			
			if( pWndWorld && pWndWorld->IsViewMap() && pWorldMap )
			{
				pWndWorld->SetViewMap(FALSE);
				pWorldMap->DeleteWorldMap();
				return CWndBase::Process();
			}
		
			if( g_pPlayer->IsDie() )
			{
				if( m_pWndRevival == NULL )
				{
					//gmpbigsun(20110725) : Ei ®¨IE°∆A°ÀA°Ì ®˙©£A|©¯®£ ¢“c¢Øo®˘°æ ®˙E•ÏC¢•A °ÌoE©˜AI ©ˆ©¨°ÌyC©¨¢•U. 
					//°ÌoE©˜®°C¢•U CO®˘o°∆¢Æ A©¨°∆¢Æ•Ï®°•Ïu.
					if( TRUE == IsMakingRevivalWnd( ) )
					{
						m_pWndRevival = new CWndRevival;
						m_pWndRevival->Initialize( this, 0 );
					}
				}
			}
			else if( m_pWndMenu == NULL || m_pWndMenu->IsVisible() == FALSE )
			{
				CWorld* pWorld( g_WorldMng() );
				if( NULL == pWorld )
					return FALSE;

				CObj* pObj( pWorld->GetObjFocus() );

//sun: 11, A¢¨°∆U C°Õ®ˆA ©ˆ©°©ˆy ®¨?°∆©°
				CWndWorld* pWndWorld( g_WndMng.m_pWndWorld );

				if(pWndWorld && pWndWorld->m_pNextTargetObj)
				{
					pWndWorld->m_pNextTargetObj = NULL;

					CMover* pMover( (CMover*)pObj );

					if( pMover->IsPeaceful() == FALSE )
						pWndWorld->m_pRenderTargetObj = pObj;
				}
				else if( pObj )
				{
					{
//sun: 10A°¿ Au®ˆA®ˆA®ˆ®¨AU	Neuz, World, Trans
						DWORD	dwTmpTick = GetTickCount();
						int	nTmpSkillID = g_pPlayer->m_pActMover->GetCastingSKillID();
						if( g_pPlayer->m_pActMover->GetCastingEndTick() > dwTmpTick && ( nTmpSkillID == SKILL_INDEX( 238, SI_KNT_HERO_DRAWING ) || nTmpSkillID == SKILL_INDEX( 244, SI_RIG_HERO_RETURN ) ) )
						{
							g_pPlayer->m_pActMover->SetCastingEndTick(0);
							g_pPlayer->m_pActMover->ClearState();				// °ÌoAA A°˛¢¨¢Á®˙iCI°∆i ¢•U®ˆA ¢¨AAa.
						}

						pWorld->SetObjFocus( NULL );
//sun: 11, A¢¨°∆U C°Õ®ˆA ©ˆ©°©ˆy ®¨?°∆©°
						if(pWndWorld)
							pWndWorld->m_pRenderTargetObj = NULL;

						pWorld->m_idObjFocusOld = NULL_ID;
						g_pPlayer->ClearDest();
//sun: 11, A¢¨°∆U C°Õ®ˆA ©ˆ©°©ˆy ®¨?°∆©°
						if(pWndWorld)
							pWndWorld->m_bAutoAttack = FALSE;

						if( g_pPlayer->m_dwFlag & MVRF_TRACKING )	// AI©ˆI ®ˆCCaA©¨AI¢¨e C®™A|.
						{
							g_pPlayer->m_dwFlag &= (~MVRF_TRACKING);		// A©¨Au¢¨©£•ÏaC®™A|.
							g_pPlayer->m_idTracking = NULL_ID;
						}
					}
				}
//sun: 13, Au®ˆ®¨®°¢Á NPCA°◊A¢Æ ®˙E¢¨©˜
				else if(pWndWorld->m_bSetQuestNPCDest)
					pWndWorld->m_bSetQuestNPCDest = FALSE;
				else
				{
//sun: 12, ¢¨o®ˆ®¨AI °Ìy®˘®¨ Neuz, World
					if( g_Neuz.m_pCreateMonItem )
					{
						g_Neuz.m_pCreateMonItem = NULL;
					}
					else
					{
						if( CWndBase::m_GlobalShortcut.IsEmpty() )
						{
							CWndBase* pWndBase = g_WndMng.GetWndBase();
							
							if( pWndBase )
							{
								LPWNDAPPLET lpWndApplet = pWndBase->GetWndApplet();
								if(pWndBase == &g_WndMng.m_pWndWorld->m_wndMenuMover || pWndBase->GetWndId() == 10)
									g_WndMng.m_pWndWorld->SetFocus();
								else if(lpWndApplet != NULL)
								{
									if( false == IsDontDestroy( lpWndApplet->dwWndId ) )
										pWndBase->Destroy();
								}
							}
							else
							{
								g_WndMng.OpenMenu();
								CRect rect = g_Neuz.GetDeviceRect();
								if( g_WndMng.m_pWndMenu )
									g_WndMng.m_pWndMenu->Move( CPoint( 0, rect.bottom - 48 - g_WndMng.m_pWndMenu->GetWndRect().Height() ) ); //48->TaskBar Height
							}
						}
					}
				}
			}
//sun: 12, ®¨n©ˆ®¢AC ©ˆ©°
			if(m_wndRemove.GetSize() > 0)
			{
				CWndBase* pWndBase = (CWndBase*)m_wndRemove.GetAt(0);

				if(pWndBase && pWndBase->GetWndApplet()->dwWndId == APP_SECRETROOM_GUILDMEMBER)
					pWndBase->Destroy(TRUE);
			}
		}
		if( m_szTimerChat[ 0 ] && m_timerChat.IsTimeOut() )
		{
			g_DPlay.SendChat( m_szTimerChat );
			m_szTimerChat[ 0 ] = 0;
			m_timerChat.Set( 0 );
		}

		if( m_bConnect && m_timerMessenger.IsTimeOut() )
			m_timerMessenger.Set( MIN( 5 ) );
//sun: 12, ®¨n©ˆ®¢AC ©ˆ©°
		if( g_pPlayer && CSecretRoomMng::GetInstance()->m_nState >= SRCONT_WAR )
		{
			if( CSecretRoomMng::GetInstance()->IsInTheSecretRoom( g_pPlayer ) )
			{
				if(m_pWndSecretRoomQuick == NULL)
				{
					//Create GuildMemberMng Window
					int nGuildIndex = CSecretRoomMng::GetInstance()->m_pSRCont->GetTenderGuild(g_pPlayer->m_idGuild);
					if(nGuildIndex != NULL_ID)
					{
						int nGuildMemberCount = CSecretRoomMng::GetInstance()->m_pSRCont->m_vecSecretRoomTender[nGuildIndex].vecLineUpMember.size();
						SAFE_DELETE(m_pWndSecretRoomQuick);
						m_pWndSecretRoomQuick = new CWndSecretRoomQuick;
						m_pWndSecretRoomQuick->SetGuildMemCount(nGuildMemberCount);

						for(int k=0; k<nGuildMemberCount; k++)
							m_pWndSecretRoomQuick->SetGuildMember(CSecretRoomMng::GetInstance()->m_pSRCont->m_vecSecretRoomTender[nGuildIndex].vecLineUpMember[k]);

						m_pWndSecretRoomQuick->Initialize(NULL);
					}
				}
			}
		}
		
		if( !CSecretRoomMng::GetInstance()->IsInTheSecretRoom( g_pPlayer ) )
		{
			if(m_pWndSecretRoomQuick)
				SAFE_DELETE(m_pWndSecretRoomQuick);
		}
	}

	if( m_pLogOutWaitting ) // A®˙°§aA©™CaA©¨AI¢¨e A°ø®ˆ®¨A®œ©ˆU ®ˆ®ˆ°§OA®˘AIAo ®¨O°∆¢Æ¢•E
		return CWndBase::Process();

	if( m_pWndTaskBar )
	{
#ifdef __Y_INTERFACE_VER3
		if( g_Option.m_nInterface == 2 )
		{
			if( g_bKeyTable[ pShortcutKey_System->KeyF1 ] )
			{
				g_bKeyTable[ pShortcutKey_System->KeyF1 ] = 0;
				m_pWndTaskBar->SetItemSlot( 0 );
			}
			if( g_bKeyTable[ pShortcutKey_System->KeyF2 ] )
			{
				g_bKeyTable[ pShortcutKey_System->KeyF2 ] = 0;
				m_pWndTaskBar->SetItemSlot( 1 );
			}
			if( g_bKeyTable[ pShortcutKey_System->KeyF3 ] )
			{
				g_bKeyTable[ pShortcutKey_System->KeyF3 ] = 0;
				m_pWndTaskBar->SetItemSlot( 2 );
			}
			if( g_bKeyTable[ pShortcutKey_System->KeyF4 ] )
			{
				g_bKeyTable[ pShortcutKey_System->KeyF4 ] = 0;
				m_pWndTaskBar->SetItemSlot( 3 );
			}
			if( g_bKeyTable[ pShortcutKey_System->KeyF1 ] )
			{
				g_bKeyTable[ pShortcutKey_System->KeyF1 ] = 0;
				ObjectExecutor( SHORTCUT_APPLET, APP_HELPER_HELP );
			}
			if( g_bKeyTable[ pShortcutKey_System->Key0 ] )
			{
				g_bKeyTable[ pShortcutKey_System->Key0 ] = 0;
				ScreenCapture();
			}
			if( g_bKeyTable[ pShortcutKey_System->KeyF9 ] )
			{
				g_bKeyTable[ pShortcutKey_System->KeyF9 ] = 0;
				FLINFO_LOG( LOG_POSTION, _T( "Pos x = %f, z = %f" ), g_pPlayer->GetPos().x, g_pPlayer->GetPos().z );
				PutString( prj.GetText(TID_ADMIN_COORDINATESAVE), NULL, prj.GetTextColor(TID_ADMIN_COORDINATESAVE) );
			}
		}
		else
#else //__Y_INTERFACE_VER3
		{
			if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
			{
				BOOL bShift = TRUE;
				if( g_bKeyTable[ pShortcutKey_System->KeyBackTick ] )  //'`'
				{
					m_pWndTaskBar->TurnPage( );
					g_bKeyTable[ pShortcutKey_System->KeyBackTick ] = 0;
				}

				bShift = GetAsyncKeyState( VK_SHIFT ) & 0x8000;
				if( bShift )
				{
					if( g_bKeyTable[ pShortcutKey_System->Key1 ] )
					{
						g_bKeyTable[ pShortcutKey_System->Key1 ] = 0;
						m_pWndTaskBar->SetItemSlot( 0 );
					}
					if( g_bKeyTable[ pShortcutKey_System->Key2 ] )
					{
						g_bKeyTable[ pShortcutKey_System->Key2 ] = 0;
						m_pWndTaskBar->SetItemSlot( 1 );
					}
					if( g_bKeyTable[ pShortcutKey_System->Key3 ] )
					{
						g_bKeyTable[ pShortcutKey_System->Key3 ] = 0;
						m_pWndTaskBar->SetItemSlot( 2 );
					}
					if( g_bKeyTable[ pShortcutKey_System->KeyBackTick ] )  //'`'
					{
						m_pWndTaskBar->TurnPage( FALSE );
					}
				}

				if( g_bKeyTable[ pShortcutKey_System->KeyScreenCapture ] )
				{
					g_bKeyTable[ pShortcutKey_System->KeyScreenCapture ] = 0;
					ScreenCapture();
				}
			}
			else if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
			{
				if( g_bKeyTable[ pShortcutKey_System->Key1 ] )
				{
					g_bKeyTable[ pShortcutKey_System->Key1 ] = 0;
					m_pWndTaskBar->SetItemSlot( 0 );
				}
				if( g_bKeyTable[ pShortcutKey_System->Key2 ] )
				{
					g_bKeyTable[ pShortcutKey_System->Key2 ] = 0;
					m_pWndTaskBar->SetItemSlot( 1 );
				}
				if( g_bKeyTable[ pShortcutKey_System->Key3 ] )
				{
					g_bKeyTable[ pShortcutKey_System->Key3 ] = 0;
					m_pWndTaskBar->SetItemSlot( 2 );
				}
				if( g_bKeyTable[ pShortcutKey_System->Key4 ] )
				{
					g_bKeyTable[ pShortcutKey_System->Key4 ] = 0;
					m_pWndTaskBar->SetItemSlot( 3 );
				}
	//sun: 11, AA®ˆ®¨A®œ©ˆU E¢ÁAa
				if( g_bKeyTable[ pShortcutKey_System->Key5 ] )
				{
					g_bKeyTable[ pShortcutKey_System->Key5 ] = 0;
					m_pWndTaskBar->SetItemSlot( 4 );
				}
				if( g_bKeyTable[ pShortcutKey_System->Key6 ] )
				{
					g_bKeyTable[ pShortcutKey_System->Key6 ] = 0;
					m_pWndTaskBar->SetItemSlot( 5 );
				}
				if( g_bKeyTable[ pShortcutKey_System->Key7 ] )
				{
					g_bKeyTable[ pShortcutKey_System->Key7 ] = 0;
					m_pWndTaskBar->SetItemSlot( 6 );
				}
				if( g_bKeyTable[ pShortcutKey_System->Key8 ] )
				{
					g_bKeyTable[ pShortcutKey_System->Key8 ] = 0;
					m_pWndTaskBar->SetItemSlot( 7 );
				}

				if( g_bKeyTable[ pShortcutKey_System->Key1 ] )
				{
					g_bKeyTable[ pShortcutKey_System->Key1 ] = 0;
					ObjectExecutor( SHORTCUT_APPLET, APP_HELPER_HELP );
				}
				
				if( g_bKeyTable[ pShortcutKey_System->Key0 ] )
				{
					g_bKeyTable[ pShortcutKey_System->Key0 ] = 0;
					ScreenCapture();
				}
				if( g_bKeyTable[ pShortcutKey_System->Key9 ] )
				{
					g_bKeyTable[ pShortcutKey_System->Key9 ] = 0;
					FLINFO_LOG( LOG_POSTION, _T( "Pos x = %f, z = %f" ), g_pPlayer->GetPos().x, g_pPlayer->GetPos().z );
					PutString( prj.GetText(TID_ADMIN_COORDINATESAVE), NULL, prj.GetTextColor(TID_ADMIN_COORDINATESAVE) );
				}
			}
		}
#endif //__Y_INTERFACE_VER3

		if( GetWndBase( APP_WEBBOX ) )
		{
			if( !CWebBox::GetInstance()->Get_Start() )
				CWebBox::GetInstance()->Set_Start();
		}
	}

	//sun: 8A°¿ ®°A®°®˘A°À °∆u°§A Au¢Øe Neuz
	CWndBase* pWndPartyQuick = (CWndBase*)g_WndMng.GetApplet( APP_PARTY_QUICK );
	if(pWndPartyQuick)
	{
		if(g_Party.GetSizeofMember() < 2)
		{
			pWndPartyQuick->Destroy();
			SetFocus();
		}
	}		
	return CWndBase::Process();
}
void CWndMgr::SetPlayer( CMover* pMover )
{
	CWndBase* pWndBase;

	if( pMover != NULL )
	{
		if( pWndBase = GetApplet( APP_GAUGE ) )
		{
			pWndBase->SetTitle( pMover->GetName() );
			//( (CWndStatus*)pWndBase )->m_pModel	= prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, pMover->m_dwType, pMover->m_dwIndex, TRUE );
			//prj.m_modelMng.LoadMotion( ( (CWndStatus*)pWndBase )->m_pModel, pMover->m_dwType, pMover->m_dwIndex, 0 );
		}


#ifdef __SKILL_UI16
		if( pWndBase = GetApplet( APP_SKILL4 ) )
			( (CWndSkill_16*)pWndBase )->InitItem( pMover->GetJob(), pMover->m_aJobSkill );
#else //sun: 9¬˜ ¿¸Ω¬∞¸∑√ Clienet
		if( pWndBase = GetApplet( APP_SKILL3 ) )
			( (CWndSkillTreeEx*)pWndBase )->InitItem( pMover->GetJob(), pMover->m_aJobSkill );
#endif //__SKILL_UI16
		
		switch(_GetContentState(CT_NEW_INVENTORY))
		{
		case CS_VER1:
			{
				if( pWndBase = GetApplet( APP_INVENTORY ) )
				{
					( (CWndInventory*)pWndBase )->m_dwEnchantWaitTime = 0xffffffff;	// ¡¶∑√«œ¥¬µµ¡ﬂ replace«œ¥¬ ∞ÊøÏ neuz∞° ≈©∑°Ω¨ µ«¥¬ «ˆªÛ ºˆ¡§
					( (CWndInventory*)pWndBase )->m_wndItemCtrl.InitItem( &pMover->m_Inventory, APP_INVENTORY );
				}
			}
			break;
		case CS_VER2:
			{
				if( pWndBase = GetApplet( APP_INVENTORY ) )
				{
					for( int i = 0; i < MAX_VENDOR_NEW_INVENTAB; i++ )
					{
						( (CWndInventory*)pWndBase )->m_dwEnchantWaitTime = 0xffffffff;	// ¡¶∑√«œ¥¬µµ¡ﬂ replace«œ¥¬ ∞ÊøÏ neuz∞° ≈©∑°Ω¨ µ«¥¬ «ˆªÛ ºˆ¡§
						( (CWndInventory*)pWndBase )->m_wndItemCtrl_[i].InitItem( &pMover->m_Inventory, APP_INVENTORY );
					}			
				}

			}
			break;
		default:
			FLASSERT(0);
			break;
		}
		//sun: 11, ¡÷∏”¥œ
		if( pWndBase = GetApplet( APP_BAG_EX ) )
			( (CWndBagEx*)pWndBase )->InitItem();

		g_Neuz.m_camera.SetPos( pMover->GetPos() );		//

		if( CMover::GetActiveMover() == NULL )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "ActiveMover æ¯¿Ω" ) );
		}
		if( CMover::GetActiveMover()->m_pActMover == NULL )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "ActionMover æ¯¿Ω" ) );
		}

		/*
		m_bLock = TRUE;
		CMover* pMoverTarget = (CMover*)g_WorldMng.Get()->GetObjFocus() ;
		if( !(pMoverTarget && CMover::GetActiveMover()->m_pActMover->IsFly()) ) {
			FLOAT fAngle = CMover::GetActiveMover()->GetAngle(); 
			m_fRotx=-fAngle;
		}
		*/
//		FLERROR_LOG( PROGRAM_NAME, _T( "WorldName-%s, Targer-%s, ActMover-%s, ActionMover-%s" )  )

		if( pMover->m_pActMover->IsFly() )
			g_Neuz.m_camera.Unlock();
		else
			g_Neuz.m_camera.Lock();

		// ø˘µÂ∞° ∞°µø«œ±‚ ¿¸ø° «¡∑Œ¡ßº«∞˙ ∫‰ ∏≈∆Æ∏ØΩ∫∏¶ ∏∏µÈæÓ¡÷æÓæﬂ 
		// πÆ¡¶∞° æ¯¥Ÿ. CWndWorld¥¬ ∏≈Ω√¡ˆ √≥∏Æ±‚¿Ã±‚ ∂ßπÆø° πÃ√≥ «‡∑ƒµÈ¿Ã
		// ºº∆√µ«±‚µµ ¿¸ø° øÏº±µ» ∏≈Ω√¡ˆø°º≠ «‡∑ƒ¿ª ªÁøÎ«“ øÏ∑¡∞° ¿÷¥Ÿ.(PickObject µ˚¿ßµÈ)
		//CWndWorld* pWndWorld = (CWndWorld*)GetWndBase( APP_WORLD );
		//pWndWorld->Projection( D3DDEVICE );
		g_Neuz.m_camera.Reset();
		g_Neuz.m_camera.Process( D3DDEVICE );
		g_Neuz.m_camera.Transform( D3DDEVICE, g_WorldMng.Get() );

		//gmpbigsun: g_pPlayer∞° ¿Ã¡¶ ∏∑ ºº∆√µ , ±ÊµÂ«œøÏΩ∫ 
		// ≈¨∂Û LandScape¥¬ Ω«¡¶∑Œ ReadWorld∞° Ω««‡µ…∂ß ∞™¿Ã √§øˆ¡¯¥Ÿ. ( º≠πˆ∑Œ ∆–≈∂ºˆΩ≈Ω√ æ˜µ•¿Ã∆Æ µ  ), ±◊¿¸ø° ∫∏¿Ã¥¬∞ÕµÈø° ¥Î«ÿ πÆ¡¶∞° πﬂª˝«œπ«∑Œ
		// g_pPlayer∞° ª˝º∫µ…∂ß «—π¯ æ˜µ•¿Ã∆Æ .
		CWorld* pWorld = g_WorldMng.Get();
		if( pWorld && g_pPlayer )
		{
			if( pMover != g_pPlayer )
				FLERROR_LOG( PROGRAM_NAME, _T( "argu is not g_pPlayer" ) );

			pWorld->ReadWorld( g_pPlayer->GetPos() );
			GuildHouse->ApplyEFTexture( ); //! 5≥‚π≠¿∫ ∂ß∏¶ π˛±‚¿⁄ 
		}

	}
	else 
	{
#ifdef __SKILL_UI16
		if( pWndBase = GetApplet( APP_SKILL4 ) )
#else //sun: 9¬˜ ¿¸Ω¬∞¸∑√ Clienet
		if( pWndBase = GetApplet( APP_SKILL3 ) )
#endif //__SKILL_UI16
			( (CWndSkillTreeEx*)pWndBase )->InitItem( 0, NULL );

		if( pWndBase = GetApplet( APP_GAUGE ) )
			SAFE_DELETE( ( (CWndStatus*)pWndBase )->m_pModel );

		
		switch(_GetContentState(CT_NEW_INVENTORY))
		{
		case CS_VER1:
			{
				if( pWndBase = GetApplet( APP_INVENTORY ) )
					( (CWndInventory*)pWndBase )->m_wndItemCtrl.InitItem( NULL, APP_INVENTORY );
			}
			break;
		case CS_VER2:
			{
				if( pWndBase = GetApplet( APP_INVENTORY ) )
				{
					for( int i = 0; i < MAX_VENDOR_NEW_INVENTAB; i++ )
					{
						( (CWndInventory*)pWndBase )->m_wndItemCtrl_[i].InitItem( NULL, APP_INVENTORY );
					}
				}
			}
			break;
		default:
			FLASSERT(0);
			break;
		}
	}
}
void CWndMgr::WordChange( CString& rString )
{
//sun: 10, __FILTER_0705
	CString strResult;

	CScanner s;
	s.SetProg( (LPVOID)(LPCSTR)rString );

	s.GetToken();
	BOOL bFirst	= ( *s.m_pProg == ' ' );

	while( s.tok != FINISHED )
	{
		CString str		= s.Token;
//		str.MakeLower();

		CScanner s1;
		s1.SetProg( (LPVOID)(LPCSTR)str );
		s1.GetToken();
		BOOL bSecond	= ( *s1.m_pProg == ' ' );
		BOOL bTerminate	= ( *s1.m_pProg == '\0' );

		while( s1.tok != FINISHED )
		{
			CString str1	= s1.Token;
			str1.MakeLower();

			std::map<std::string, std::string>::iterator it	= prj.m_mapAlphaFilter.find( (LPSTR)(LPCSTR)str1 );
			if( it != prj.m_mapAlphaFilter.end() )
			{
//				if( g_Option.m_nSlangWord == 1 ) 
				{
					TCHAR szWord[256]	= { 0,};
					memset( szWord, '*', lstrlen( str1 ) );
					strResult	+= szWord;
				}
//				else
//				{
//					strResult	+= it->second.c_str();
//				}
			}
			else
			{
				strResult	+= s1.Token;
			}
			if( ( bTerminate && bFirst ) ||
				( !bTerminate && bSecond ) )
				strResult	+= ' ';
//			strResult	+= ' ';
			s1.GetToken();
			bSecond	= ( *s1.m_pProg == ' ' );
			bTerminate	= ( *s1.m_pProg == '\0' );
		}
		s.GetToken();
		bFirst	= ( *s.m_pProg == ' ' );
	}

	for( std::map<std::string, std::string>::iterator it = prj.m_mapNonalphaFilter.begin(); it != prj.m_mapNonalphaFilter.end(); ++it )
	{
		CString str		= it->first.c_str();
		if( g_Option.m_nSlangWord == 1 ) 
		{
			TCHAR	szWord[256]	= { 0,};
			memset( szWord, '*', str.GetLength() );
			strResult.Replace( str, szWord );
		}
		else
		{
			strResult.Replace( str, it->second.c_str() );
		}
	}
	rString		= strResult;
}
// √™ø°º≠ ∂«¥¬ ∏≈≈©∑Œ∑Œ ¿‘∑¬µ» ≈ÿΩ∫∆Æ∞° ¿Ã «‘ºˆ∏¶ øÎµµø° ∏¬∞‘ ∫–∑˘µ»¥Ÿ.
void CWndMgr::ParsingChat( CString string )
{
//sun: 12, ∆©≈‰∏ÆæÛ ∞≥º±
	CWndGuideSystem* pWndGuide = NULL;
	pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
	if(pWndGuide && pWndGuide->IsVisible()) pWndGuide->m_Condition.strInput.Format("%s", string); 

	string.TrimLeft( ' ' );
	if( string.IsEmpty() == TRUE )
		return;

	// πÆ¿Â±Ê¿Ã∞° ≥ π´ ±Ê∏È 64πŸ¿Ã∆Æ∑Œ ¡Ÿ¿”.
	SetStrNull( string, 120 );

	int nWarning = string.Find( "$", 0 );

	//gmpbigsun( 20110321 ), #12958 : '$'∞° πﬂ∞ﬂµ» ∞ÊøÏ ¥Ÿ¿Ω πÆ¿⁄∞° i¿Ã∞≈≥™ ni∞° æ∆¥œ∂Û∏È ¿«µµ¿˚¿Œ æÓ∫‰¬°¿∏∑Œ ∆«¥‹«œ∞Ì ∞…∑Ø≥Ω¥Ÿ.
	while( nWarning != -1 )
	{
		char ch = string.GetAt( nWarning + 1 );
		if( ch == 'n' )
		{
			char nextCh = string.GetAt( nWarning + 2 );
			if( nextCh == 'i' || nextCh == 'a' )
			{
				//ok
			}
			else
			{
				//∫“∑Æ
				string.Empty();
				return;
			}
		}
		else if( ch == 'i' || ch == 'a' )
		{
			//ok
		}
		else
		{
			//∫“∑Æ
			string.Empty();
			return;
		}

		nWarning = string.Find( "$", nWarning+1 );
	}

	int nStart = string.Find( "$i", 0 );
	int nEnd = string.Find( "$ni", 0 );

	int nStartLink = string.Find( "$a", 0 );
	int nEndLink = string.Find( "$na", 0 );

	/// æ∆¿Ã≈€ ∏µ≈©¥¬ æ»∞…∑Ø¡ˆ∞‘≤˚ «—¥Ÿ.
	if( -1 != nStartLink || -1 != nEndLink ) //√£æ“¥Ÿ.
	{
		CString strTemp = string;
		strTemp.Replace( string.Mid( nStartLink, nEndLink + 3 ), "_EMPTY_" );

		int nStart2 = strTemp.Find( "$i", 0 );
		int nEnd2 = strTemp.Find( "$ni", 0 );

		if( -1 != nStart2 || -1 != nEnd2 ) //æ∆¿Ã≈€ ∏µ≈©∏ª∞Ìµµ ªˆªÛ ∞¸∑√ ±€¿⁄∏¶ ΩË¿∏∏È ªÁøÎ±›¡ˆ.
		{
			CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
			if( !pWndChat )
				return;
			
			string.Empty();
			pWndChat->m_kTempHLData_Send.reset();
			return;
		}

		// ≈Î∞˙
		nStart = 0;
		nEnd = 0;
	}

	if( -1 != nStart || -1 != nEnd ) //√£æ“¥Ÿ.
	{
		CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
		if( !pWndChat )
			return;

		// æ‡º”πÆ¿⁄ Ω√¿€ »§¿∫ ≥°¿ª ¬˛æ“¿∏≥™, µ—¥Ÿ æ¯¿ª∞ÊøÏ ≥™ª€≥—¿∏∑Œ ∞£¡÷
		if( -1 == nStart || -1 == nEnd )
		{
			string.Empty();
			pWndChat->m_kTempHLData_Send.reset();
			return;
		}
	
		//gmpbigsun( 20111213 ) ¡¶∞≈ 
		if( nStart > 0 )
		{
			int nloop = nStart - 1;
			while( nloop >= 0 )
			{
				char ch = string.GetAt( nloop );
				if( ch != ' ' && IsAlphaNum( ch ) == FALSE )		//πŸ∑Œæ’πÆ¿⁄∞° ∆ØºˆπÆ¿⁄∏È ≥™ª€≥¿∏∑Œ ∞£¡÷
				{
					string.Empty();
					pWndChat->m_kTempHLData_Send.reset();
					return;
				}

				--nloop;
			}
		}

		if( nEnd > 0 )
		{
			int nloop = nEnd - 1;
			while( nloop >= 0 )
			{
				char ch = string.GetAt( nloop );
				if( ch != ' ' && IsAlphaNum( ch ) == FALSE )		//πŸ∑Œæ’πÆ¿⁄∞° ∆ØºˆπÆ¿⁄∏È ≥™ª€≥¿∏∑Œ ∞£¡÷
				{
					string.Empty();
					pWndChat->m_kTempHLData_Send.reset();
					return;
				}

				--nloop;
			}
		}
		
		if( nEnd - nStart > 100 )	//≥ π´±Ë
		{
			string.Empty();
			pWndChat->m_kTempHLData_Send.reset();
			return;
		}

	}

	if( string.Find("#l") != -1 )
	{
		int mCut = 0;
		mCut = string.Find('/');

		if( mCut != -1 )
		{
			string = string.Right( string.GetLength() - mCut );
		}
	}

	FormalizeChatString( string );

	if( string.GetLength() > 0 )
	{
		if( string[0] == '/' )
		{
			ParsingCommand( string.LockBuffer(), g_pPlayer );
			string.UnlockBuffer();
		}
		else 
		{
			if( m_timerBanning.IsTimeOut() == FALSE )
			{
				CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
				if( pWndChat )
				{
					int nOriginalSecond = static_cast< int >( BANNING_MILLISECOND - static_cast< int >( m_timerBanning.GetLeftTime() ) ) / 1000;
					int nMinute = static_cast< int >( nOriginalSecond / 60 );
					int nSecond = static_cast< int >( nOriginalSecond % 60 );
					CString strMessage = _T( "" );
					// «ˆ¿Á √§∆√ ±›¡ˆ ∆‰≥Œ∆º∏¶ πﬁ∞Ì ¿÷Ω¿¥œ¥Ÿ. (≥≤¿∫ Ω√∞£: %d∫– %d√ )
					strMessage.Format( prj.GetText( TID_GAME_ERROR_CHATTING_3 ), nMinute, nSecond );
					pWndChat->PutString( strMessage, 0xffff0000 );
					return;
				}
			}
			else
			{
				if( string != m_strChatBackup || m_timerDobe.IsTimeOut() || g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
				{
					m_timerDobe.Reset();
					m_strChatBackup = string;
					WordChange( string );

					// mirchang string hex code check
					if( string[0] == '/' )
					{
						ParsingCommand( string.LockBuffer(), g_pPlayer );
						string.UnlockBuffer();
					}
					else	// mirchang string hex code check
					{
						SetStrNull( string, 120 );
						if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
							g_DPlay.SendChat( ( LPCSTR )string );
						else
						{
							if( m_bShortcutCommand == TRUE )
							{
								if( m_timerShortcutWarning.IsTimeOut() == FALSE )
								{
									++m_nWarningCounter;
									CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
									if( pWndChat )
									{
										if( m_nWarningCounter >= BANNING_POINT )
										{
											// ∞˙µµ«— √§∆√¿∏∑Œ ¿Œ«œø© %d∫– µøæ» √§∆√ ±›¡ˆ ∆‰≥Œ∆º∏¶ πﬁ¿∏ºÃΩ¿¥œ¥Ÿ.
											CString strChattingError1 = _T( "" );
											strChattingError1.Format( prj.GetText( TID_GAME_ERROR_CHATTING_2 ), BANNING_MILLISECOND / 1000 / 60 );
											pWndChat->PutString( strChattingError1, prj.GetTextColor( TID_GAME_ERROR_CHATTING_2 ) );
											m_nWarningCounter = 0;
											m_timerBanning.Reset();
										}
										else
										{
											// ø¨º” √§∆√¿∏∑Œ ¿Œ«œø© ∏ﬁΩ√¡ˆ∞° √‚∑¬µ«¡ˆ æ æ“Ω¿¥œ¥Ÿ.
											pWndChat->PutString( prj.GetText( TID_GAME_ERROR_CHATTING_1 ), 0xffff0000 );
										}
									}
								}
								else
									g_DPlay.SendChat( ( LPCSTR )string );
								m_timerShortcutWarning.Reset();
							}
							else
							{
								if( m_timerWarning.IsTimeOut() == FALSE )
								{
									++m_nWarningCounter;
									CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
									if( pWndChat )
									{
										if( m_nWarningCounter >= BANNING_POINT )
										{
											// ∞˙µµ«— √§∆√¿∏∑Œ ¿Œ«œø© %d∫– µøæ» √§∆√ ±›¡ˆ ∆‰≥Œ∆º∏¶ πﬁ¿∏ºÃΩ¿¥œ¥Ÿ.
											CString strChattingError1 = _T( "" );
											strChattingError1.Format( prj.GetText( TID_GAME_ERROR_CHATTING_2 ), BANNING_MILLISECOND / 1000 / 60 );
											pWndChat->PutString( strChattingError1, prj.GetTextColor( TID_GAME_ERROR_CHATTING_2 ) );
											m_nWarningCounter = 0;
											m_timerBanning.Reset();
										}
										else
										{
											// ø¨º” √§∆√¿∏∑Œ ¿Œ«œø© ∏ﬁΩ√¡ˆ∞° √‚∑¬µ«¡ˆ æ æ“Ω¿¥œ¥Ÿ.
											pWndChat->PutString( prj.GetText( TID_GAME_ERROR_CHATTING_1 ), 0xffff0000 );
										}
									}
								}
								else
								{
									if( m_timerWarning2.IsTimeOut() == FALSE )
									{
										++m_nWarning2Counter;
										CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
										if( pWndChat )
										{
											if( m_nWarning2Counter >= BANNING_2_POINT )
											{
												// ∞˙µµ«— √§∆√¿∏∑Œ ¿Œ«œø© %d∫– µøæ» √§∆√ ±›¡ˆ ∆‰≥Œ∆º∏¶ πﬁ¿∏ºÃΩ¿¥œ¥Ÿ.
												CString strChattingError1 = _T( "" );
												strChattingError1.Format( prj.GetText( TID_GAME_ERROR_CHATTING_2 ), BANNING_MILLISECOND / 1000 / 60 );
												pWndChat->PutString( strChattingError1, prj.GetTextColor( TID_GAME_ERROR_CHATTING_2 ) );
												m_nWarning2Counter = 0;
												m_timerBanning.Reset();
											}
											else
												g_DPlay.SendChat( ( LPCSTR )string );
										}
									}
									else
										g_DPlay.SendChat( ( LPCSTR )string );
								}
								m_timerWarning.Reset();
								m_timerWarning2.Reset();
							}
						}
					}
				}
				else
				{
					CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
					if( pWndChat )
					{
						// ø¨º”¿∏∑Œ ∞∞¿∫ ≥ªøÎ¿ª ¿‘∑¬«“ ºˆ æ¯Ω¿¥œ¥Ÿ.
						pWndChat->PutString( prj.GetText( TID_GAME_CHATSAMETEXT ), prj.GetTextColor( TID_GAME_CHATSAMETEXT ) );
					}
				}
			}

			m_bShortcutCommand = FALSE;
		}
	}
}
void CWndMgr::PutDefinedString( DWORD dwText, ... )
{
	TCHAR szBuffer[ 512 ];
	TCHAR szFormat[ 512 ];
	FLStrcpy( szFormat, _countof( szFormat ), prj.GetText( dwText ) );
	va_list args;
	va_start( args, szFormat );
	FLVSPrintf( szBuffer, _countof( szBuffer ), szFormat, args );
	va_end( args );

	DWORD dwColor	= prj.GetTextColor( dwText );
	g_WndMng.PutString( (LPCTSTR)szBuffer, NULL, dwColor );
}
/*
// Msg
m_bMsgSys
m_bMsgHelp
m_bMsg
// Chat
m_bChatGeneral
m_bChatSay
m_bChatParty
m_bChatGuild
*/
//DWORD g_Option.m_dwChatFilter = CHATSTY_GENERAL | CHATSTY_HELP | 
//	CHATSTY_SYSTEM | CHATSTY_GAME | CHATSTY_CLIENTMSG;
void CWndMgr::PutString( LPCTSTR lpszString, CObj* pObj, DWORD dwColor, DWORD dwChatStyle, DWORD dwPStyle )
{
	CMover* pMover = (CMover*)pObj;
	CString str = lpszString;
	DWORD dwStyle = 0;
//	BOOL bChat = FALSE;

	dwPStyle = PS_USE_MACRO;
	
	// Text Message Style 
	#define TMS_CLIENT   0x00000001
	#define TMS_DIALOG   0x00000002
	#define TMS_CAPTION  0x00000004
	#define TMS_CHAT     0x00000008

	int nChannel = 0;
	switch( CWndChat::m_nChatChannel )
	{
		case CHATSTY_GENERAL: nChannel = 0; break;
		case CHATSTY_WHISPER: nChannel = 1; break;
		case CHATSTY_SHOUT  : nChannel = 2; break;
		case CHATSTY_PARTY  : nChannel = 3; break;
		case CHATSTY_GUILD  : nChannel = 4; break;
		case CHATSTY_ALL	: nChannel = 5; break;
	}
	BOOL  bChatLog = TRUE;
	DWORD dwChatFilter = g_Option.m_dwChatFilter[ nChannel ];
	if( ( dwChatFilter & CHATSTY_GENERAL       ) == dwChatStyle ) 
		{ dwStyle |= TMS_CHAT; bChatLog = FALSE; } 
	if( ( dwChatFilter & CHATSTY_WHISPER       ) == dwChatStyle ) 
		{ dwStyle |= TMS_CHAT; bChatLog = FALSE; } 
	if( ( dwChatFilter & CHATSTY_SHOUT         ) == dwChatStyle ) 
		{ dwStyle |= TMS_CHAT; bChatLog = FALSE; } 
	if( ( dwChatFilter & CHATSTY_PARTY         ) == dwChatStyle ) 
		{ dwStyle |= TMS_CHAT; bChatLog = FALSE; } 
	if( ( dwChatFilter & CHATSTY_GUILD         ) == dwChatStyle ) 
		{ dwStyle |= TMS_CHAT; bChatLog = FALSE; } 
	if( ( dwChatFilter & CHATSTY_ALL			) == dwChatStyle )
	{dwStyle |= TMS_CHAT; bChatLog = FALSE;	}

	DWORD dwChatFilterEtc = g_Option.m_dwChatFilterEtc;
	if( ( dwChatFilterEtc & CHATSTY_SYSTEM_CLIENT ) && ( dwChatStyle == CHATSTY_SYSTEM ) ) 
	{ dwStyle |= TMS_CLIENT; dwChatStyle = CHATSTY_SYSTEM_CHAT; }
	if( ( dwChatFilterEtc & CHATSTY_SYSTEM_CHAT   ) && ( dwChatStyle == CHATSTY_SYSTEM ) ) 
	{ dwStyle |= TMS_CHAT  ; dwChatStyle = CHATSTY_SYSTEM_CHAT; }
	if( ( dwChatFilterEtc & CHATSTY_HELP_CLIENT   ) && ( dwChatStyle == CHATSTY_HELP   ) ) 
	{ dwStyle |= TMS_CLIENT; dwChatStyle = CHATSTY_HELP_CHAT  ; }   
	if( ( dwChatFilterEtc & CHATSTY_HELP_CHAT     ) && ( dwChatStyle == CHATSTY_HELP   ) ) 
	{ dwStyle |= TMS_CHAT  ; dwChatStyle = CHATSTY_HELP_CHAT  ; }  
	if( ( dwChatFilterEtc & CHATSTY_GAME_CLIENT   ) && ( dwChatStyle == CHATSTY_GAME   ) ) 
	{ dwStyle |= TMS_CLIENT; dwChatStyle = CHATSTY_GAME_CHAT  ; }  
	if( ( dwChatFilterEtc & CHATSTY_GAME_CHAT     ) && ( dwChatStyle == CHATSTY_GAME   ) ) 
	{ dwStyle |= TMS_CHAT  ; dwChatStyle = CHATSTY_GAME_CHAT  ; } 
	BOOL bRecord = TRUE; // π´¡∂∞« ±‚∑œ. ≥™¡ﬂø° ¿Ã ∫Øºˆ æ¯æ÷¿⁄.

	CWndChat* pWndChat = (CWndChat*)g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
	CEditString* pChatString = &m_ChatString;

	//
	//  øπø‹ ªÛ»≤ √≥∏Æ(∞°¥…ªÛ»≤ø°º≠ ¡§«ÿ¡¯∞Õ¿Ã ø©±‚º≠ π´»ø∞° µ… ºˆµµ ¿÷¥Ÿ.)
	//
	if( pObj == NULL ) // ø¿∫Í¡ß∆Æ∞° ≥Œ¿Ã∏È ∏ª«≥º± ±›¡ˆ 
		dwStyle &= ~TMS_DIALOG; 
	else
	if( pObj->GetType() == OT_MOVER )
	{
		if( dwChatStyle & CHATSTY_GENERAL )
			dwStyle |= TMS_DIALOG;
		if( ( (CMover*)pObj )->IsPlayer() == FALSE ) // «√∑°¿ÃæÓ∞° æ∆¥œ∂Û∏È √™ ±›¡ˆ 
		{
			dwStyle &= ~TMS_CHAT;
			bRecord = FALSE;
		}
		if( ( (CMover*)pObj )->m_vtInfo.IsVendorOpen() ) // ∞≥¿Œ ªÛ¡°¿Ã∏È ∏ª«≥º± ±›¡ˆ 
			dwStyle &= ~TMS_DIALOG;

		// chipi_080311 - øÓøµ¿⁄¿Œ ∞ÊøÏ ∏ª«≥º± ±›¡ˆ
		if( ((CMover*)pObj)->m_dwAuthorization >= AUTH_GAMEMASTER && ((CMover*)pObj)->IsMode( TRANSPARENT_MODE ) )
			dwStyle &= ~TMS_DIALOG;

		if( ( (CMover*)pObj )->GetWorld() && ( (CMover*)pObj )->GetWorld()->GetID() == WI_WORLD_QUIZ && ( ( (CMover*)pObj )->m_dwMode & QUIZ_RENDER_SKIP_MODE ) )
			dwStyle &= ~TMS_DIALOG;
	}		
	/*******************************************
	    √‚∑¬ 
	*/
	// ≈¨∂Û¿Ãæ∆Æ ∏≈Ω√¡ˆ 
	if( dwStyle & TMS_CLIENT )
		g_ClientMsg.AddMessage( lpszString, dwColor );

	CString strChat	= lpszString;
	strChat.Replace( "\n", " " );
	strChat.Replace( '\n', ' ' );

	// √§∆√ ∏≈Ω√¡ˆ, bRecord∞° TRUE¿Ã∏È √§∆√ √¢ø° √‚∑¬«“ ∏≈Ω√¡ˆ∂Û¥¬ ¿«πÃ (π›µÂΩ√ «ˆ¿Á √§≥Œø° √‚∑¬µ«¥¬ ∞Õ¿∫ æ∆¥‘)
	if( bRecord )
	{
		// ±€ ≥ªøÎ¿Ã 1000¡Ÿ¿ª ≥—æÓ∞°∏È 500∂Û¿Œ¿ª ¿ﬂ∂Û≥Ω¥Ÿ.
		if( m_aChatString.GetSize() > 500 )
		{
			m_aChatString.RemoveAt( 0, 250 );
			m_aChatColor.RemoveAt( 0, 250 );
			m_aChatStyle.RemoveAt( 0, 250 );
		}
		
		m_aChatString.Add( strChat );
//		m_aChatString.Add( lpszString );
		m_aChatColor.Add( dwColor );
		m_aChatStyle.Add( dwChatStyle );
	}
	if( dwStyle & TMS_CHAT )
	{
		if( pWndChat )
			pWndChat->PutString( strChat, dwColor, dwPStyle );
		//else
		//{
		//	m_ChatString.AddString( lpszString, dwColor );
		//	m_ChatString.AddString( "\n", dwColor );// += '\n';
		//}
	}		
	// ƒ∏º« ∏≈Ω√¡ˆ 
	if( dwStyle & TMS_CAPTION )
	{
		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
		if( g_xFlyffConfig->GetMainLanguage() != LANG_JAP )
			g_Caption1.AddCaption( lpszString, pWndWorld->m_pFontAPITitle );
		else
			g_Caption1.AddCaption( lpszString, NULL );		
	}

	// ¥Ÿ¿ÃæÛ∑Œ±◊ ∏≈Ω√¡ˆ 
	if( dwStyle & TMS_DIALOG )
	{
		if( pObj->GetType() == OT_MOVER )// && ((CMover*)pObj)->m_nQuestEmoticonIndex == -1 )
		{
			const TCHAR* lpszChat	= _tcschr( lpszString, _T( ':' ) );
			lpszChat += sizeof(TCHAR) * 2;	// Skip ": "
			g_DialogMsg.AddMessage( pObj, lpszChat, 0xffffffff, 0, dwPStyle );
		}
	}

	//sun: Ω√Ω∫≈€ ∏ﬁºº¡ˆ ¿©µµøÏ»≠
	if( m_pWndChatLog )
	{
		if( bChatLog && !(dwStyle&TMS_DIALOG) && dwChatStyle != CHATSTY_SHOUT ) //ø‹ƒ°±‚, NPC∏ª«≥º± ≥ªøÎ System Chat√¢ø° µÈæÓø¿¡ˆ æ ∞‘ ∏∑¿⁄.
			m_pWndChatLog->PutString( lpszString, dwColor, dwPStyle );
	}
}

void CWndMgr::PutToolTip_Skill( DWORD dwSkill, DWORD dwLevel, CPoint point, CRect* pRect, BOOL bActive )
{
	SkillProp* pSkillProp = prj.GetSkillProp( dwSkill );
	if( pSkillProp == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Ω∫≈≥(%d)¿« «¡∑Œ∆€∆º∞° æ˜¥Ÿ." ), dwSkill );
		return;	// property not found
	}

	DWORD dwSkillLevel	= dwLevel;

	AddSkillProp* pAddSkillProp		= prj.GetAddSkillProp( pSkillProp->dwSubDefine, dwSkillLevel );
	if( pAddSkillProp == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "addΩ∫≈≥(%d)¿« «¡∑Œ∆€∆º∞° æ˜¥Ÿ." ), dwSkill );
		return;	// property not found
	}

	DWORD dwaf = D3DCOLOR_XRGB( 199, 155,   0 );
	CString str;
	CString strstr;
	CString string;
	if( bActive )
		string.Format( "#b#cff2fbe6d%s   %s %d#nb#nc", pSkillProp->szName, prj.GetText(TID_SKILL_BUFFICONLEVEL01), dwSkillLevel );
	else
		string.Format( "#b#cff2fbe6d%s#nb#nc", pSkillProp->szName );
	char statebuf[30] = {0,};
	if( pAddSkillProp->nReqMp != 0xffffffff )	
	{
		FLSPrintf( statebuf, _countof( statebuf ), "\nMP : %d", g_pPlayer->GetReqMp( pAddSkillProp->nReqMp ) );
		string += statebuf;
	}
	if( pAddSkillProp->nReqFp != 0xffffffff )	
	{
		FLSPrintf( statebuf, _countof( statebuf ), "\nFP : %d", g_pPlayer->GetReqFp( pAddSkillProp->nReqFp ) );
		string += statebuf;
	}
	
	
//	strstr.Format( prj.GetText(TID_GAME_BASESKILLLEVEL), pSkillProp->dwReqDisLV );
//	if( g_pPlayer->GetLevel() < pSkillProp->dwReqDisLV )
//		str.Format( "\n#cffff0000%s#nc", strstr );
//	else
//		str.Format( "\n%s", strstr );
//	string += str;

	if( pSkillProp->dwReqSkill1 != NULL_ID )
	{
		LPSKILL pskill = g_pPlayer->GetSkill(pSkillProp->dwReqSkill1);

		if( pskill )
		{
			strstr.Format( prj.GetText(TID_GAME_SKILLDIS), pskill->GetProp()->szName, pSkillProp->dwReqSkillLevel1 );
			if( pskill->dwLevel < pSkillProp->dwReqSkillLevel1 )
				str.Format( "\n#cffff0000%s#nc", strstr );
			else
				str.Format( "\n%s#nc", strstr );
			string += str;
		}
	}
	if( pSkillProp->dwReqSkill2 != NULL_ID )
	{
		LPSKILL pskill = g_pPlayer->GetSkill(pSkillProp->dwReqSkill2);
		
		if( pskill )
		{
			strstr.Format( prj.GetText(TID_GAME_SKILLDIS), pskill->GetProp()->szName, pSkillProp->dwReqSkillLevel2 );
			if( pskill->dwLevel < pSkillProp->dwReqSkillLevel2 )
				str.Format( "\n#cffff0000%s#nc", strstr );
			else
				str.Format( "\n%s#nc", strstr );
			string += str;
		}
	}

	strstr.Format( prj.GetText(TID_GAME_BASESKILLLEVEL), pSkillProp->dwReqDisLV );

//sun: 10¬˜ ¿¸Ω¬Ω√Ω∫≈€	Neuz, World, Trans
	if( g_pPlayer->GetLevel() < (int)( pSkillProp->dwReqDisLV ) && dwLevel <= 0 && !g_pPlayer->IsMaster() && !g_pPlayer->IsHero() )
		str.Format( "\n#cffff0000%s#nc", strstr );
	else
		str.Format( "\n%s", strstr );
	string += str;

	if( strcmp( pSkillProp->szCommand, "=" ) )
	{
		string += "\n";
		string += pSkillProp->szCommand;
	}
	
	int nSkillIdx = g_pPlayer->GetSkillIdx( dwSkill );

	if( nSkillIdx != -1 )
	{
		DWORD dwDelay = g_pPlayer->GetReuseDelay( nSkillIdx );
		if( dwDelay > 0 )
		{
			CTimeSpan ct( (long)(dwDelay / 1000.0f) );		// ≥≤¿∫Ω√∞£¿ª √ ¥‹¿ß∑Œ ∫Ø»Ø«ÿº≠ ≥—∞‹¡‹
			str.Format( prj.GetText(TID_TOOLTIP_COOLTIME), ct.GetMinutes(), ct.GetSeconds() );		// ≥≤¿∫Ω√∞£¿ª ∫–/√  «¸≈¬∑Œ √‚∑¬.
			string += "\n";
			string += str;
		}
	}
	
	CEditString strEdit;
	strEdit.SetParsingString( string );
	
	g_toolTip.PutToolTip( dwSkill, strEdit, *pRect, point, 0 );
}
void CWndMgr::PutToolTip_Troupe( DWORD dwSkill, CPoint point, CRect* pRect )
{
	SkillProp* pSkillProp = prj.GetPartySkill( dwSkill );
	
	if( pSkillProp == NULL )
		return;
	
	CString string;
	string.Format( "#b#cff2fbe6d%s#nb#nc", pSkillProp->szName );

	//string = pSkillProp->szName;
	char statebuf[30] = {0,};
	if( pSkillProp->dwReqMp_ != 0xffffffff )	
	{
		FLSPrintf( statebuf, _countof( statebuf ), "\nMP : %d", pSkillProp->dwReqMp_ );
		string += statebuf;
	}
	if( pSkillProp->dwReqFp_ != 0xffffffff )	
	{
		FLSPrintf( statebuf, _countof( statebuf ), "\nFP : %d", pSkillProp->dwReqFp_ );
		string += statebuf;
	}
	if( strcmp( pSkillProp->szCommand, "=" ) )
	{
		CString str;
		string += "\n";
		string += pSkillProp->szCommand;
	}
	CEditString strEdit;
	strEdit.SetParsingString( string );
	
	g_toolTip.PutToolTip( dwSkill, strEdit, *pRect, point, 0 );
}
void CWndMgr::PutToolTip_Item( DWORD dwType, DWORD dwId, CPoint point, CRect* pRect, int flag )
{
	FLItemBase* pItemBase = g_pPlayer->GetItemId( dwId );
	
	if( pItemBase )
		PutToolTip_Item( pItemBase, point, pRect, flag );
}

struct DST_STRING
{
	int nDstParam;		// ex) DST_STR
	int nNameIndex;
};

static DST_STRING g_DstString[] = 
{
	DST_STR                     , TID_TOOLTIP_STR, 
	DST_DEX                     , TID_TOOLTIP_DEX,
	DST_INT                     , TID_TOOLTIP_INT,
	DST_STA                     , TID_TOOLTIP_STA,
	DST_SPEED                   , TID_TOOLTIP_SPEED,
	DST_ABILITY_MIN             , TID_TOOLTIP_ABILITYMIN,
	DST_ABILITY_MAX             , TID_TOOLTIP_ABILITYMAX,
	DST_ATTACKSPEED             , TID_TOOLTIP_ATKSPEED,
	DST_ADJDEF                  , TID_TOOLTIP_DEFENCE,
	DST_RESIST_MAGIC            , TID_TOOLTIP_DEFMAGIC, 
	DST_RESIST_ELECTRICITY      , TID_TOOLTIP_DEFELECTRICITY,
	DST_RESIST_ALL              , TID_TOOLTIP_DEFALLELEMETAL,
	DST_RESIST_FIRE             , TID_TOOLTIP_DEFFIFE,
	DST_RESIST_WIND             , TID_TOOLTIP_DEFWIND,
	DST_RESIST_WATER            , TID_TOOLTIP_DEFWATER, 
	DST_RESIST_EARTH            , TID_TOOLTIP_DEFEARTH,
	DST_HP_MAX                  , TID_TOOLTIP_MAXHP,
	DST_MP_MAX                  , TID_TOOLTIP_MAXMP,
	DST_FP_MAX                  , TID_TOOLTIP_MAXFP,
	DST_HP                      , TID_TOOLTIP_HP,
	DST_MP                      , TID_TOOLTIP_MP,
	DST_FP                      , TID_TOOLTIP_FP,
	DST_HP_RECOVERY             , TID_TOOLTIP_HPRECOVERY,
	DST_MP_RECOVERY             , TID_TOOLTIP_MPRECOVERY,
	DST_FP_RECOVERY             , TID_TOOLTIP_FPRECOVERY,
	DST_HP_RECOVERY_RATE        , TID_TOOLTIP_HPRECOVERYRATE,
	DST_MP_RECOVERY_RATE        , TID_TOOLTIP_MPRECOVERYRATE,
	DST_FP_RECOVERY_RATE        , TID_TOOLTIP_FPRECOVERYRATE,
	DST_ALL_RECOVERY			, TID_TOOLTIP_ALL_RECOVERY,		
	DST_ALL_RECOVERY_RATE		, TID_TOOLTIP_ALL_RECOVERY_RATE,
	DST_KILL_HP					, TID_TOOLTIP_KILL_HP,		
	DST_KILL_MP					, TID_TOOLTIP_KILL_MP,					
	DST_KILL_FP					, TID_TOOLTIP_KILL_FP,					
	DST_KILL_ALL				, TID_TOOLTIP_KILL_ALL,				
	DST_KILL_HP_RATE			, TID_TOOLTIP_KILL_HP_RATE,
	DST_KILL_MP_RATE			, TID_TOOLTIP_KILL_MP_RATE,			
	DST_KILL_FP_RATE			, TID_TOOLTIP_KILL_FP_RATE,			
	DST_KILL_ALL_RATE			, TID_TOOLTIP_KILL_ALL_RATE,			
	DST_ALL_DEC_RATE			, TID_TOOLTIP_ALL_DEC_RATE,
	DST_ADJ_HITRATE             , TID_TOOLTIP_HITRATE,
	DST_CHR_DMG                 , TID_TOOLTIP_CHRDMG,
	DST_CHRSTATE                , TID_TOOLTIP_CHRSTATE,
	DST_PARRY                   , TID_TOOLTIP_PARRY,
	DST_ATKPOWER_RATE           , TID_TOOLTIP_ATKPOWER,
	DST_JUMPING                 , TID_TOOLTIP_JUMPING,
	DST_BLOCK_MELEE				, TID_GAME_TOOLTIP_BLOCK,
	DST_BLOCK_RANGE				, TID_GAME_TOOLTIP_BLOCKRANGE,
	DST_STAT_ALLUP				, TID_GAME_TOOLTIPALLSTAT,
	DST_HP_MAX_RATE				, TID_TOOLTIP_DST_HP_MAX_RATE,
	DST_ADDMAGIC				, TID_GAME_TOOLTIPCONDITIONRATE,
	DST_ADJDEF_RATE				, TID_TOOLTIP_DST_ADJDEF_RATE,
	DST_HP_MAX_RATE				, TID_TOOLTIP_DST_HP_MAX_RATE,
	DST_MP_MAX_RATE				, TID_TOOLTIP_DST_MP_MAX_RATE, 
	DST_FP_MAX_RATE				, TID_TOOLTIP_DST_FP_RATE,
	DST_CHR_CHANCECRITICAL		, TID_TOOLTIP_DST_CRITICAL_RATE,
	DST_CHR_WEAEATKCHANGE		, TID_TOOLTIP_DST_CHR_WEAEATKCHANGE,
	DST_MASTRY_EARTH			, TID_TOOLTIP_DST_MASTRY_EARTH,
	DST_MASTRY_FIRE				, TID_TOOLTIP_DST_MASTRY_FIRE,
	DST_MASTRY_WATER			, TID_TOOLTIP_DST_MASTRY_WATER,
	DST_MASTRY_ELECTRICITY		, TID_TOOLTIP_DST_MASTRY_ELECTRICITY,
	DST_MASTRY_WIND				, TID_TOOLTIP_DST_MASTRY_WIND,
	DST_REFLECT_DAMAGE			, TID_TOOLTIP_DST_REFLECT_DAMAGE,
	DST_MP_DEC_RATE				, TID_TOOLTIP_DST_MP_DEC_RATE,
	DST_FP_DEC_RATE				, TID_TOOLTIP_DST_FP_DEC_RATE,
	DST_SPELL_RATE				, TID_TOOLTIP_DST_SPELL_RATE,
	DST_CAST_CRITICAL_RATE		, TID_TOOLTIP_DST_CAST_CRITICAL_RATE,
	DST_CRITICAL_BONUS			, TID_TOOLTIP_DST_CRITICAL_BONUS,
	DST_YOY_DMG,				  TID_TOOLTIP_DST_YOY_DMG,
	DST_BOW_DMG,				  TID_TOOLTIP_DST_BOW_DMG,
	DST_KNUCKLE_DMG,			  TID_TOOLTIP_DST_KNUCKLE_DMG,
	DST_SWD_DMG,				  TID_TOOLTIP_DST_SWD_DMG,
	DST_AXE_DMG,				  TID_TOOLTIP_DST_AXE_DMG,

	DST_ATTACKSPEED_RATE,		  TID_TOOLTIP_ATTACKSPEED_RATE,
	DST_CHR_STEALHP,			  TID_TOOLTIP_DST_DMG_GET,
//sun: 8, // __GUILDCOMBAT_85
	DST_PVP_DMG_RATE,				TID_TOOLTIP_DST_DMG_GET,			//gmpbigsun:jsææ∞° πŸ≤„¡÷∏È ∞Ìƒ•∞Õ

//sun: 9¬˜ ¿¸Ω¬∞¸∑√ Clienet
	DST_EXPERIENCE,					TID_TOOLTIP_DST_EXPERIENCE,
	DST_MELEE_STEALHP,				TID_TOOLTIP_DST_MELEE_STEALHP,
	DST_MONSTER_DMG,				TID_TOOLTIP_DST_MONSTER_DMG,
	DST_PVP_DMG,					TID_TOOLTIP_DST_PVP_DMG,

	DST_HEAL,	TID_TOOLTIP_DST_HEAL,	//sun: 9, 9-10¬˜ ∆Í

	DST_ATKPOWER,	TID_TOOLTIP_ATKPOWER_VALUE,
//sun: 10¬˜ ¿¸Ω¬Ω√Ω∫≈€	Neuz, World, Trans
	DST_ONEHANDMASTER_DMG,			TID_TOOLTIP_DST_ONEHANDMASTER_DMG,
	DST_TWOHANDMASTER_DMG,			TID_TOOLTIP_DST_TWOHANDMASTER_DMG,
	DST_YOYOMASTER_DMG,				TID_TOOLTIP_DST_YOYOMASTER_DMG,
	DST_BOWMASTER_DMG,				TID_TOOLTIP_DST_BOWMASTER_DMG,
	DST_KNUCKLEMASTER_DMG,			TID_TOOLTIP_DST_KNUCKLEMASTER_DMG,
	DST_HAWKEYE_RATE,				TID_TOOLTIP_DST_HAWKEYE,
	DST_RESIST_MAGIC_RATE,			TID_TOOLTIP_DEFMAGIC_RATE, 

	DST_GIFTBOX,					TID_TOOLTIP_DST_GIFTBOX,

	DST_HPDMG_UP,					TID_TOOLTIP_DST_HPDMG_UP,
	DST_DEFHITRATE_DOWN,			TID_TOOLTIP_DST_DEFHITRATE_DOWN,
	DST_RESTPOINT_RATE,				TID_TOOLTIP_RESTPOINT_RATE,
	DST_CHR_RANGE,					TID_GAME_TOOLTIP_ATTACKRANGE4,
	DST_STOP_MOVEMENT,				TID_GAME_TOOLTIP_MOVEMENT1,
	DST_IMMUNITY,					TID_GAME_TOOLTIP_IMMUNITY1,
	DST_IGNORE_DMG_PVP,				TID_MMI_ABSOLUTEBERRIER01,
	
	DST_TAKE_PVP_DMG_PHYSICAL_RATE, 		TID_TOOLTIP_TAKE_PVP_DMG_PHYSICAL_RATE,
	DST_TAKE_PVP_DMG_MAGIC_RATE,			TID_TOOLTIP_TAKE_PVP_DMG_MAGIC_RATE,
	DST_TAKE_PVE_DMG_PHYSICAL_RATE, 		TID_TOOLTIP_TAKE_PVE_DMG_PHYSICAL_RATE,
	DST_TAKE_PVE_DMG_MAGIC_RATE,			TID_TOOLTIP_TAKE_PVE_DMG_MAGIC_RATE,
	DST_DROP_ITEM_ALLGRADE_RATE,			TID_TOOLTIP_DROP_ITEM_ALLGRADE_RATE,
	DST_DROP_ITEM_ONEMORE_CHANCE,			TID_TOOLTIP_DROP_ITEM_ONEMORE_CHANCE,
	DST_GIVE_PVE_DMG_ELEMENT_FIRE_RATE,		TID_TOOLTIP_GIVE_PVE_DMG_ELEMENT_FIRE_RATE,
	DST_GIVE_PVE_DMG_ELEMENT_WATER_RATE,	TID_TOOLTIP_GIVE_PVE_DMG_ELEMENT_WATER_RATE,
	DST_GIVE_PVE_DMG_ELEMENT_ELECT_RATE,	TID_TOOLTIP_GIVE_PVE_DMG_ELEMENT_ELECT_RATE,
	DST_GIVE_PVE_DMG_ELEMENT_WIND_RATE,		TID_TOOLTIP_GIVE_PVE_DMG_ELEMENT_WIND_RATE,
	DST_GIVE_PVE_DMG_ELEMENT_EARTH_RATE,	TID_TOOLTIP_GIVE_PVE_DMG_ELEMENT_EARTH_RATE,
	DST_ADJ_ITEM_EQUIP_LEVEL,				TID_TOOLTIP_ADJ_ITEM_EQUIP_LEVEL,
	DST_SKILL_LEVELUP_ALL,					TID_TOOLTIP_SKILL_LEVELUP_ALL,
	DST_GIVE_DMG_RATE_ENEMY_STUN,			TID_TOOLTIP_GIVE_DMG_RATE_ENEMY_STUN,
	DST_GIVE_DMG_RATE_ENEMY_DARK,			TID_TOOLTIP_GIVE_DMG_RATE_ENEMY_DARK,
	DST_GIVE_DMG_RATE_ENEMY_POISON,			TID_TOOLTIP_GIVE_DMG_RATE_ENEMY_POISON,
	DST_GIVE_DMG_RATE_ENEMY_SLOW,			TID_TOOLTIP_GIVE_DMG_RATE_ENEMY_SLOW,
	DST_GIVE_DMG_RATE_ENEMY_BLEEDING,		TID_TOOLTIP_GIVE_DMG_RATE_ENEMY_BLEEDING,
	DST_GIVE_DMG_RATE_ENEMY_SILENT,			TID_TOOLTIP_GIVE_DMG_RATE_ENEMY_SLILENT,
	DST_GIVE_DMG_RATE_ENEMY_LOOT,			TID_TOOLTIP_GIVE_DMG_RATE_ENEMY_LOOT,
	DST_GIVE_DMG_RATE_ENEMY_SETSTONE,		TID_TOOLTIP_GIVE_DMG_RATE_ENEMY_SETSTONE,
	DST_GIVE_DMG_RATE_ENEMY_SLEEPING,		TID_TOOLTIP_GIVE_DMG_RATE_ENEMY_SLEEPING,
	
	0,							  0									// ∏∂¡ˆ∏∑ ø‰º“
};

BOOL IsDst_Rate( int nDstParam )
{
	static int nDstRate[] = {
			0, 
			0, 
			DST_ADJ_HITRATE,
			0, 
			DST_ATKPOWER_RATE,
			DST_ADJDEF_RATE,
			DST_DEFHITRATE_DOWN,
			DST_HP_MAX_RATE,
			DST_MP_MAX_RATE,		
			DST_FP_MAX_RATE,		
			DST_HP_RECOVERY_RATE,
			DST_MP_RECOVERY_RATE,
			DST_FP_RECOVERY_RATE,
			DST_CHR_CHANCECRITICAL,
			DST_MASTRY_EARTH,
			DST_MASTRY_FIRE,
			DST_MASTRY_WATER,
			DST_MASTRY_ELECTRICITY,
			DST_MASTRY_WIND,
			DST_ATTACKSPEED,
			DST_MP_DEC_RATE,
			DST_FP_DEC_RATE,
			DST_SPELL_RATE,
			DST_CAST_CRITICAL_RATE,
			DST_CRITICAL_BONUS,
			DST_PARRY,

			DST_ALL_RECOVERY_RATE,
			DST_KILL_HP_RATE,	
			DST_KILL_MP_RATE,	
			DST_KILL_FP_RATE,
			DST_KILL_ALL_RATE,
			DST_ALL_DEC_RATE,
			DST_BLOCK_MELEE,
			DST_BLOCK_RANGE,		
			DST_ATTACKSPEED_RATE,
			DST_CHR_STEALHP,
//sun: 10¬˜ ¿¸Ω¬Ω√Ω∫≈€	Neuz, World, Trans
			DST_EXPERIENCE,
			DST_HAWKEYE_RATE,
			DST_RESIST_MAGIC_RATE,

			DST_SPEED,
			DST_REFLECT_DAMAGE
			, DST_RESTPOINT_RATE
			, DST_MONSTER_DMG
			, DST_PVP_DMG
			, DST_TAKE_PVP_DMG_PHYSICAL_RATE
			, DST_TAKE_PVP_DMG_MAGIC_RATE
			, DST_TAKE_PVE_DMG_PHYSICAL_RATE
			, DST_TAKE_PVE_DMG_MAGIC_RATE			
			, DST_DROP_ITEM_ALLGRADE_RATE			
			, DST_DROP_ITEM_ONEMORE_CHANCE		
			, DST_GIVE_PVE_DMG_ELEMENT_FIRE_RATE	
			, DST_GIVE_PVE_DMG_ELEMENT_WATER_RATE
			, DST_GIVE_PVE_DMG_ELEMENT_ELECT_RATE
			, DST_GIVE_PVE_DMG_ELEMENT_WIND_RATE	
			, DST_GIVE_PVE_DMG_ELEMENT_EARTH_RATE
			, DST_GIVE_DMG_RATE_ENEMY_STUN	
			, DST_GIVE_DMG_RATE_ENEMY_DARK	
			, DST_GIVE_DMG_RATE_ENEMY_POISON	
			, DST_GIVE_DMG_RATE_ENEMY_SLOW	
			, DST_GIVE_DMG_RATE_ENEMY_BLEEDING
			, DST_GIVE_DMG_RATE_ENEMY_SILENT	
			, DST_GIVE_DMG_RATE_ENEMY_LOOT	
			, DST_GIVE_DMG_RATE_ENEMY_SETSTONE
			, DST_GIVE_DMG_RATE_ENEMY_SLEEPING

	};
	
	int n = sizeof( nDstRate ) / sizeof( nDstRate[0] );
	
	for( int i=0; i<n; ++i )
	{
		if( nDstRate[i] == nDstParam )
			return TRUE;
	}

	return FALSE;
}

// dst ∆ƒ∂Û∏ﬁ≈Õ¿Ã∏ß¿ª Ω∫∆Æ∏µ¿∏∑Œ ∏Æ≈œ.
// ex) nDstparam = DST_STR -> "»˚"
char *FindDstString( int nDstParam )
{
	for( int i = 0; ; ++i )
	{
		if( g_DstString[i].nDstParam == 0 )
			break;

		if( g_DstString[i].nDstParam == nDstParam )	// ∞∞¿∫∞… √£æ“¿∏∏È ±◊ ¿Ã∏ß¿ª ∏Æ≈œ.
			return (char*)prj.GetText(g_DstString[i].nNameIndex);
	}
	return "";
}

// æ∆¿Ã≈€ ≈¯∏≥ √‚∑¬«“∞Õ
// ∑£¥˝ ø…º« ¿Ã∏ß, æ∆¿Ã≈€ ¿Ã∏ß( ±ÊµÂ ∏¡≈‰∏È ±ÊµÂ ¿Ã∏ß ), (ºº∆Æ)
// øπ ) ∞ı¿« ƒ⁄∆∞ Ω¥∆Æ(ºº∆Æ)
DWORD CWndMgr::PutItemName( FLItemElem* pItemElem, CEditString* pEdit )
{
	if( NULL == pItemElem )
		return NULL;

	if( NULL == pEdit )
		return NULL;

	CString		str( _T("") );
	CString		strTemp( _T("") );

#ifdef __HYPERLINK_ITEM16
	DWORD dwColorbuf( GetItemTitleTextColor( pItemElem ) );
#else // __HYPERLINK_ITEM16
	DWORD dwColorbuf( dwItemColor[g_Option.m_nToolTipText].dwName0 );

//sun: 9AÇA°§ AÄA®˘A®ˆAÇA°∆A®˘A°§AÉ Clienet
	if( pItemElem->GetProp()->dwReferStat1 == WEAPON_GENERAL )
		dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwName0;
	else if( pItemElem->GetProp()->dwReferStat1 == WEAPON_UNIQUE )
		dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwName1;
	else if( pItemElem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE )
		dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwName3;
	else if( pItemElem->GetProp()->dwReferStat1 == ARMOR_SET )
		dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwName1;	
	else if(	pItemElem->GetProp()->dwReferStat1 == BARUNA_D ||
				pItemElem->GetProp()->dwReferStat1 == BARUNA_C ||
				pItemElem->GetProp()->dwReferStat1 == BARUNA_B ||
				pItemElem->GetProp()->dwReferStat1 == BARUNA_A ||
				pItemElem->GetProp()->dwReferStat1 == BARUNA_S 	
				)
	{
		dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwName4;	
	}
	else
	{
		__noop;
	}

#endif // __HYPERLINK_ITEM16

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		PT_ITEM_SPEC pItemProp = pItemElem->GetProp();
		if( pItemProp != NULL )
		{
			if(pItemProp->nBindCondition == BIND_CONDITION_NOTBIND)
			{
				if( pItemElem->IsFlag( FLItemElem::binds ) == TRUE )				// A°æAçA®˘Aì A®˘A©¯AÅA¢¥AÄAå A•ÏAáAÄAñA°∆AÖA©¯A®£..		
				{
					if( pItemElem->m_dwItemId != ITEM_INDEX( 2082, II_GEN_MAT_ORICHALCUM01_1 )
						&& pItemElem->m_dwItemId != ITEM_INDEX( 2083, II_GEN_MAT_MOONSTONE_1 ) )
					{
						strTemp.Format( "%s ", prj.GetText( TID_GAME_TOOLTIP_ITEM_BINDS ) );
					}
					else
					{
						strTemp.Format( "" );
					}
				}
				else if( pItemElem->IsFlag( FLItemElem::reset_bind ) == TRUE )// A°æAçA®˘Aì AáAòAÅA|A¢¨A| AáAë A°∆A|A¢ØA°˛..
				{
					strTemp.Format( "" );//
				}
			}	
		}
	}

	// A°§A°ÃA¢•A®ˆ A¢ØAâA®˘Aá
	if( pItemElem->GetProp()->dwParts != (DWORD)-1 )
	{
		PRANDOMOPTITEM pRandomOptItem( CRandomOptItemGen::GetInstance()->GetRandomOptItem( static_cast<int>( pItemElem->GetRandomOptionOriginID() ) ) );
		if( pRandomOptItem )
			strTemp.Format( "%s ", pRandomOptItem->pszString );
	}

	BOOL bGuildCombatCloak = FALSE;

	if( pItemElem->m_dwItemId == ITEM_INDEX( 4703, II_ARM_S_CLO_CLO_DRAGON1 ) || pItemElem->m_dwItemId == ITEM_INDEX( 4704, II_ARM_S_CLO_CLO_DRAGON2 ) ||
		pItemElem->m_dwItemId == ITEM_INDEX( 4705, II_ARM_S_CLO_CLO_DRAGON3 ) || pItemElem->m_dwItemId == ITEM_INDEX( 4706, II_ARM_S_CLO_CLO_DRAGON4 ) )
		bGuildCombatCloak  = TRUE;

	if( pItemElem->GetProp()->dwItemKind3 == IK3_CLOAK && pItemElem->m_idGuild )	// A°æA|A•ÏA°ÕA©ˆA¢¨AàA°ÃA°∆A¢Æ A©ˆAöAáA¢•AÄAñAÄA¢¨A¢¨A®œ.
	{
		CGuild *pGuild = g_GuildMng.GetGuild( pItemElem->m_idGuild );
		if( pGuild && bGuildCombatCloak == FALSE )
			str.Format( prj.GetText( TID_GUILD_CLOAK ), pGuild->m_szGuild );
		else
			str.Format( "%s", pItemElem->GetProp()->szName );
	}
//sun: 9AÇA°§ AÄA®˘A®ˆAÇA°∆A®˘A°§AÉ Clienet
	else
	{
		str	= pItemElem->GetName();
	}

#ifdef __ZCK_ARME_RARE
	const int nZckRarity = static_cast<int>( pItemElem->GetZckArmeRareLevel() );
	const ZckArmeRareProp* pZckRarity = pItemElem->IsZckArmeRareEligible() ? prj.GetZckArmeRare( nZckRarity ) : NULL;
	if( pZckRarity && nZckRarity > ZCK_ARME_RARE_MIN_LEVEL )
	{
		CString strRarityPrefix;
		strRarityPrefix.Format( _T( "[%s] " ), pZckRarity->m_szName );
		strTemp = strRarityPrefix + strTemp;
		dwColorbuf = pZckRarity->m_dwColor;
	}
#endif // __ZCK_ARME_RARE

	CString stredit( _T("") );
	strTemp += str;

#ifdef __ENCHANT_BARUNA16
	if( pItemElem->GetProp()->IsBaruna() )
		stredit.Format("             %s", strTemp);
	else
#endif //__ENCHANT_BARUNA16
	if( pItemElem->GetProp()->IsUltimate() ) //Ultimate A®˙AÜAÄAåAÑAúAÄAå A•ÏA®œA®˙A¢ÁA°∆A¢ÆA°æA°À AÄA°◊AáAë A¢ØA®œA©ˆA®œ
		stredit.Format("             %s", strTemp);
	else
		stredit.Format("%s", strTemp);

	pEdit->AddString( stredit, dwColorbuf, ESSTY_BOLD );
	
	return dwColorbuf;
}

void CWndMgr::PutItemAbilityPiercing( FLItemElem* pItemElem, CEditString* pEdit, DWORD dwColorBuf, int nFlag /* = 0*/ )
{
	// A®˙AÜAÄAåAÖAõ A°æA©¯AàA? AÉA°ÀA¢ØA¢ÆA®˘A°© A¢ØAÇ A®˙AÜAÄAåAÖAõ AÄAè A°∆A|A¢ØA°˛ AÄAèA©ˆAùAÅA|A°§AÉ A•ÏAà A®˙AÜAÄAåAÖAõAÄA°Ì AÉA°ÀA°§AÇ AÄA°◊AáAò
	// AÅA¢¥A®¨A¢¨A¢¨A| A®˙A©˜A®˙A¢ÁA¢ØAçA®˘A°© A®˙AÜAÄAåAÖAõ A•ÏAöA¢ØA¢Æ AáAäA¢ØA¢¥ AÅA|A°§AÉ A®˘A¢“AÑA¢ÆA¢¨A| AÉA°ÀA°§AÇAáAò AÅAòA¢•Aô. ( EX. A¢ØA°˛A•ÏA°◊A®˘AíA•ÏA°Õ +9 ~ +11 )
	// AáAèA•ÏA°ÕAÑAöA•ÏA©ˆAÄAåA¢“A©¯ AÅAãA®˘AõAáAïA¢•AèA¢•Aô.
	if( APP_DIALOG_EVENT == nFlag )
	{
		CWndCommonItemExchange* pWndCommonItemExchange( (CWndCommonItemExchange* )g_WndMng.GetWndBase( APP_DIALOG_EVENT ) );
		if( pWndCommonItemExchange )
		{
			WndCommonItemExchange::ICONDRAWINFO stCurrentOnMouseItemDrawInfo = pWndCommonItemExchange->GetCurrentOnMouseItemDrawInfo();
					
			if( NULL != stCurrentOnMouseItemDrawInfo.m_dwMinGeneralEnchant &&
				NULL != stCurrentOnMouseItemDrawInfo.m_dwMaxGeneralEnchant )
			{
				CString strRange( _T("") );
			
				strRange.Format( _T(" %+d ~ %+d"), stCurrentOnMouseItemDrawInfo.m_dwMinGeneralEnchant, stCurrentOnMouseItemDrawInfo.m_dwMaxGeneralEnchant );
				pEdit->AddString( strRange, dwColorBuf, ESSTY_BOLD );
				
				return ;
			}
		}
	}

	CString strTemp;
	DWORD dwAbilityColor = dwColorBuf;
#ifdef __ZCK_ARME_RARE
	const int nZckRarity = static_cast<int>( pItemElem->GetZckArmeRareLevel() );
	const ZckArmeRareProp* pZckRarity = pItemElem->IsZckArmeRareEligible() ? prj.GetZckArmeRare( nZckRarity ) : NULL;
	if( pZckRarity && nZckRarity > ZCK_ARME_RARE_MIN_LEVEL )
		dwAbilityColor = pZckRarity->m_dwColor;
#endif // __ZCK_ARME_RARE

	if( pItemElem->GetAbilityOption() )
	{
		strTemp.Format( " %+d", pItemElem->GetAbilityOption() );
		pEdit->AddString( strTemp, dwAbilityColor, ESSTY_BOLD );
	}
	
	int nCount = 0;
	for( size_t j = 0; j < pItemElem->GetGeneralPiercingSize(); j++ )
	{
		if( pItemElem->GetGeneralPiercingItemID( j ) != 0 )
			nCount++;
	}
//sun, 11 AÑAâA¢¨A?AÖAç A®¨AÄAÄAé A°∆AÖA°§A¢Æ A°æA°ÀA¢•Aâ world,database,neuz
	if( pItemElem->GetGeneralPiercingSize() > 0 && pItemElem->m_dwItemId != ITEM_INDEX( 26476,II_SYS_SYS_SCR_SEALCHARACTER ) )
//sun, 11 AÑAâA¢¨A?AÖAç A®¨AÄAÄAé A°∆AÖA°§A¢Æ A°æA°ÀA¢•Aâ world,database,neuz( < 11 )
//	if( pItemElem->GetGeneralPiercingSize() > 0 )
	{
		strTemp.Format( "    (%d/%d)", nCount, pItemElem->GetGeneralPiercingSize() );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwPiercing );
	}
}

void CWndMgr::PutItemResist( FLItemElem* pItemElem, CEditString* pEdit )
{
	CString str;
	CString strTemp;
	DWORD dwResistColor = dwItemColor[g_Option.m_nToolTipText].dwResist;
	str = "";
	switch( pItemElem->m_byItemResist )
	{
	case SAI79::FIRE:
		{
			str = prj.GetText(TID_UPGRADE_FIRE);
			dwResistColor = dwItemColor[g_Option.m_nToolTipText].dwResistFire;
		}					
		break;
	case SAI79::WATER:
		{
			str = prj.GetText(TID_UPGRADE_WATER);
			dwResistColor = dwItemColor[g_Option.m_nToolTipText].dwResistWater;
		}					
		break;
	case SAI79::EARTH:
		{
			str = prj.GetText(TID_UPGRADE_EARTH);
			dwResistColor = dwItemColor[g_Option.m_nToolTipText].dwResistEarth;
		}
		break;
	case SAI79::ELECTRICITY:
		{
			str = prj.GetText(TID_UPGRADE_ELECTRICITY);
			dwResistColor = dwItemColor[g_Option.m_nToolTipText].dwResistElectricity;
		}					
		break;
	case SAI79::WIND:
		{
			str = prj.GetText(TID_UPGRADE_WIND);
			dwResistColor = dwItemColor[g_Option.m_nToolTipText].dwResistWind;
		}					
		break;
	}
	
	if( pItemElem->GetProp()->dwItemKind1 == IK1_WEAPON  || pItemElem->GetProp()->dwItemKind1 == IK1_ARMOR )
	{
		if( !str.IsEmpty() )
		{
			BOOL bBold = FALSE;
			strTemp.Format( "\n%s%+d", str, pItemElem->m_nResistAbilityOption );
			if( pItemElem->m_nResistSMItemId != 0 )
				bBold = TRUE;

			if( bBold )
				pEdit->AddString( strTemp, dwResistColor, ESSTY_BOLD );
			else
				pEdit->AddString( strTemp, dwResistColor );
		}
	}
	else
	{
		strTemp.Format( "\n%s %s%d", str, prj.GetText(TID_SKILL_BUFFICONLEVEL01), pItemElem->GetProp()->dwItemLV );
		pEdit->AddString( strTemp, dwResistColor );
	}
}

void CWndMgr::PutItemSpeed( FLItemElem* pItemElem, CEditString* pEdit )
{
	CString strTemp;
	if( pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_DIRECT || pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_MAGIC )
	{	// π´±‚
		if( pItemElem->GetProp()->fAttackSpeed != 0xffffffff ) // ∞¯∞›º”µµ
		{
			pEdit->AddString( "\n" );
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ATTACKSPEED), GetATKSpeedString( pItemElem->GetProp()->fAttackSpeed ) );
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral ); 
		}
	}
}

//---------------------------------------------------------------------------------------------
// πˆ∫Ì ≈∏¿” ( √ﬂ∞° »ﬁΩƒ ∞Ê«Ëƒ° ) ≈¯∆¡ √‚∑¬
// param	: MMI ¡§¿« ¿Œµ¶Ω∫
// return	: øπ / æ∆¥œø‰
//---------------------------------------------------------------------------------------------
void CWndMgr::PutBubbleTime( CEditString* pEdit )
{
	FLERROR_ASSERT_LOG_RETURN_VOID( pEdit, _T("[¿©µµøÏ ∏ﬁ¥œ¡Æ] ∆Ì¡˝ πÆ¿⁄ø≠ ∆˜¿Œ≈Õ∞° ¿Ø»ø«œ¡ˆ æ Ω¿¥œ¥Ÿ.") );
	FLERROR_ASSERT_LOG_RETURN_VOID( GuildHouse, _T("[¿©µµøÏ ∏ﬁ¥œ¡Æ] ±ÊµÂ «œøÏΩ∫ ∆˜¿Œ≈Õ∞° ¿Ø»ø«œ¡ˆ æ Ω¿¥œ¥Ÿ.") );

	CString strBubblePointPercent( _T("") );
	strBubblePointPercent.Format( prj.GetText( TID_MMI_BUBBLEPOINTTOOLTIP ), GuildHouse->m_nExtraExp );

	pEdit->AddString( strBubblePointPercent ); 
}



//sun: 11, ∞¢º∫, √‡∫π
void CWndMgr::PutAwakeningBlessing( FLItemElem* pItemElem, CEditString* pEdit )
{
	if( pItemElem == NULL )
	{
		return;
	}

	PT_ITEM_SPEC pItemProp = pItemElem->GetProp();
	if( pItemProp == NULL || pItemProp->IsUpgradeAble( IUTYPE_RANDOM_OPTION_EXTENSION ) == FALSE )
	{
		return;
	}

// 	int nKind	= g_xRandomOptionProperty->GetRandomOptionKind( pItemElem );
// 	if( nKind < 0 )		// æ∆¿Ã≈€ ∞¢º∫, ø©Ω≈¿« √‡∫π ¥ÎªÛ¿Ã æ∆¥œ∏È,
// 		return;

	bool bIsSetRandomOptionExtension = pItemElem->IsSetRandomOptionExtension();
//	int nSize	= g_xRandomOptionProperty->GetRandomOptionSize( pItemElem->GetRandomOptItemId() );

	//∞¢º∫ ∫∏»£ √Îº“µ» æ∆¿Ã≈€¿Œ∞°.. ±◊∑∏¥Ÿ∏È ¡Ÿ±◊æÓ¡¯ ø…º«¿ª ∫∏ø©¡‡æﬂ «—¥Ÿ.
	bool bSafe = false;
	if( pItemElem->IsSetSafeGuardRandomOptionExtensionFlag() == true )
	{
		bSafe = true;
	}
//	bool bSafe = g_xRandomOptionProperty->IsCheckedSafeFlag( pItemElem->GetRandomOptItemId() );
// 	if( bSafe )
// 		nSize = g_xRandomOptionProperty->GetViewRandomOptionSize( pItemElem->GetRandomOptItemId() );

	CString	str;

	BOOL bBlessing	= FALSE;
	// title
	if( pItemProp->IsWeaponParts() == TRUE || pItemProp->IsArmorParts() == TRUE )
//	if( nKind == CRandomOptionProperty::eAwakening )
	{
//sun: 12, π´±‚ ««æÓΩÃ

//gmpbigsun( 20101109 ) : πŸ∑Á≥™ π´±‚¥¬ "∞¢º∫∞°¥…" πÆ±∏ ¡¶∞≈, #11159
//BEGIN ---------------------------------------------------------------------------------------------------
		BOOL bCheckExtra = TRUE;

#ifdef __ENCHANT_BARUNA16
		T_ITEM_SPEC* pSpec = pItemElem->GetProp();
		bCheckExtra = pSpec->IsBaruna( );
#endif
		if(pSpec->dwRandomOptionMode == 1 )// ∞¢º∫ ∞°¥…«“∂ß∏∏,,Text¬ÔæÓ¡‡æﬂµ«¥¬µ•,,,§ª
		{
			if( bIsSetRandomOptionExtension == false)
//END---------------------------------------------------------------------------------------------------
			{
				str.Format( "\n\"%s\"", prj.GetText( TID_GAME_AWAKENING ) );	// "∞¢º∫«“ ºˆ ¿÷¥¬ æ∆¿Ã≈€"
				pEdit->AddString( str, dwItemColor[g_Option.m_nToolTipText].dwAwakening );
			}
		}
	}
	else if( pItemProp->IsCostumeParts() == TRUE )
//	else if( nKind == CRandomOptionProperty::eBlessing )
	{
		if( bIsSetRandomOptionExtension == true )
		{
			str.Format( "\n%s", prj.GetText( TID_GAME_BLESSING_CAPTION ) );	// √‡∫ππﬁ¿∫ ø…º«
			pEdit->AddString( str, dwItemColor[g_Option.m_nToolTipText].dwBlessing );
			bBlessing	= TRUE;
		}
	}
//sun: 12, ∆Í ∞¢º∫
	else if( pItemProp->IsSystemPet() == TRUE || pItemProp->IsEatPet() == TRUE )
//	else if( nKind == CRandomOptionProperty::eSystemPet || nKind == CRandomOptionProperty::eEatPet )
	{	// Ω√Ω∫≈€ ∆Í∞˙ ∏‘∆Í¿« ≈¯∆¡ø° ∞¢º∫∞˙ ∞¸∑√µ» ≥ªøÎ¿ª √ﬂ∞°«—¥Ÿ
//sun: 12, π´±‚ ««æÓΩÃ
		if( bIsSetRandomOptionExtension == false )
		{
			str.Format( "\n\"%s\"", prj.GetText( TID_GAME_AWAKENNIG_PET_00 ) );	// "∞¢º∫«“ ºˆ ¿÷¥¬ æ∆¿Ã≈€"
			pEdit->AddString( str, dwItemColor[g_Option.m_nToolTipText].dwAwakening );
		}
	}

	// option
	if( bIsSetRandomOptionExtension == true )
	{
		for( size_t Nth = 0; Nth < pItemElem->GetRandomOptionExtensionSize(); ++Nth )
		{
			WORD wDstID = pItemElem->GetRandomOptionExtensionDstID( Nth );
			short shAdjValue = pItemElem->GetRandomOptionExtensionAdjValue( Nth );
			
			if( IsDst_Rate( wDstID ) == TRUE )
			{
				//sun: 12, π´±‚ ««æÓΩÃ
				if( wDstID == DST_ATTACKSPEED )
				{
					str.Format( "\n%s %c%d%% ", FindDstString( wDstID ), ( shAdjValue > 0? '+': '-' ), ::abs( shAdjValue / 2 / 10 ) );
				}
				else
				{
					str.Format( "\n%s %c%d%%", FindDstString( wDstID ), ( shAdjValue > 0? '+': '-' ), ::abs( shAdjValue ) );
				}
			}
			else
			{
				//sun: 12, π´±‚ ««æÓΩÃ
				str.Format( "\n%s %c%d", FindDstString( wDstID ), ( shAdjValue > 0? '+': '-' ), ::abs( shAdjValue ) );
			}

			if( pItemProp->IsWeaponParts() == TRUE || pItemProp->IsArmorParts() == TRUE )
			{
				DWORD dwStyle = 0;
				dwStyle = ( bSafe ? ESSTY_STRIKETHROUGH : 0 );		//¡Ÿ ±◊æÓπˆ∏±±Ó?
				pEdit->AddString( str, dwItemColor[g_Option.m_nToolTipText].dwAwakening, dwStyle );
			}
			else
			{
				pEdit->AddString( str, dwItemColor[g_Option.m_nToolTipText].dwBlessing );
			}
		}

		if( bBlessing )
		{
			str.Format( "\n%s", prj.GetText( TID_GAME_BLESSING_WARNING ) );
			pEdit->AddString( str, dwItemColor[g_Option.m_nToolTipText].dwBlessingWarning );
		}
	}
// 	for( int i = 0; i < nSize; i++ )
// 	{
// 		int nDst, nAdj;
// 		if( !g_xRandomOptionProperty->GetParam( pItemElem->GetRandomOptItemId(), i, &nDst, &nAdj ) )
// 			continue;
// 		if( IsDst_Rate( nDst ) )
// 		{
// //sun: 12, π´±‚ ««æÓΩÃ
// 			if( nDst == DST_ATTACKSPEED )
// 				str.Format( "\n%s %c%d%% ", FindDstString( nDst ), ( nAdj > 0? '+': '-' ), ::abs( nAdj / 2 / 10 ) );
// 			else
// 				str.Format( "\n%s %c%d%%", FindDstString( nDst ), ( nAdj > 0? '+': '-' ), ::abs( nAdj ) );
// 		}
// 		else
// 		{
// //sun: 12, π´±‚ ««æÓΩÃ
// 			str.Format( "\n%s %c%d", FindDstString( nDst ), ( nAdj > 0? '+': '-' ), ::abs( nAdj ) );
// 		}
// 
// 		if( nKind == CRandomOptionProperty::eAwakening )
// 		{
// 			DWORD dwStyle = 0;
// 			dwStyle = ( bSafe ? ESSTY_STRIKETHROUGH : 0 );		//¡Ÿ ±◊æÓπˆ∏±±Ó?
// 			pEdit->AddString( str, dwItemColor[g_Option.m_nToolTipText].dwAwakening, dwStyle );
// 		}
// 		else
// 			pEdit->AddString( str, dwItemColor[g_Option.m_nToolTipText].dwBlessing );
// 	}
// 	if( bBlessing )
// 	{
// 		str.Format( "\n%s", prj.GetText( TID_GAME_BLESSING_WARNING ) );
// 		pEdit->AddString( str, dwItemColor[g_Option.m_nToolTipText].dwBlessingWarning );
// 	}
}


void CWndMgr::PutRandomOpt( FLItemElem* pItemElem, CEditString* pEdit, int nType )
{
	if( pItemElem->GetProp()->dwParts == (DWORD)-1 )
		return;
	CString strTemp;

	WORD wID = 0;
	switch( nType )
	{
	case 0 : wID = pItemElem->GetRandomOptionOriginID( );			break;
	case 1 : wID = pItemElem->GetCombinedRandomOptionOriginID( );	break;
	default : FLASSERT( 0 );	break;
	}

	PRANDOMOPTITEM pRandomOptItem	= CRandomOptItemGen::GetInstance()->GetRandomOptItem( static_cast<int>( wID ) );
	if( pRandomOptItem ) // 2. ∑£¥˝ ø…º«¿« ≥ªøÎ¿ª √‚∑¬«—¥Ÿ.
	{
		for( int i = 0; i < pRandomOptItem->ia.nSize; i++ )
		{
			int nDst = (int)pRandomOptItem->ia.anDstParam[i];
			int nAdj = (int)pRandomOptItem->ia.anAdjParam[i];
			
			if( IsDst_Rate(nDst) )
			{
				if( nDst == DST_ATTACKSPEED )
					strTemp.Format( "\n%s%+d%%", FindDstString(nDst), nAdj / 2 / 10 );
				else
					strTemp.Format( "\n%s%+d%%", FindDstString(nDst), nAdj );					
			}
			else
			{
				strTemp.Format( "\n%s+%d", FindDstString(nDst), nAdj );								
			}
	
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwRandomOption );
		}
	}
}
void CWndMgr::PutPiercingOpt( FLItemElem* pItemElem, CEditString* pEdit, int nType )
{
	CString strTemp;
	PIERCINGAVAIL pa = { 0 };
	//mem_set( &pa, 0, sizeof(pa) );

	switch( nType )
	{
	case 0 : pItemElem->GetPiercingAvail( &pa );	break;
	case 1 : pItemElem->GetCombinedPiercingAvail( &pa ); break;
	}
	
	for( int i = 0; i < pa.nSize; i++ )
	{
		int nDst = (int)pa.anDstParam[i];
		int nAdj = (int)pa.anAdjParam[i];
		
		if( IsDst_Rate(nDst) )
		{
			if( nDst == DST_ATTACKSPEED )
				strTemp.Format( "\n%s%+d%%", FindDstString( nDst ), nAdj / 2 / 10 );
			else
				strTemp.Format( "\n%s%+d%%", FindDstString( nDst ), nAdj );
		}
		else
		{
			strTemp.Format( "\n%s+%d", FindDstString( nDst ), nAdj );
		}
		
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwPiercing );
	}
}

#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
void CWndMgr::PutGemOpt( FLItemElem* pItemElem, CEditString* pEdit )
{
	CString strTemp;

	int nAblity[5] = {0,};
	
	for( int i = 0; i < pItemElem->GetUltimatePiercingSize(); i++ )
	{
		int nDst = (int)pItemElem->GetUltimatePiercingItemID( i );
		int nAdj = (int)pItemElem->GetGemAvail(nDst);

		for( int j = 0; j < 5; j++ )
		{
			if( nDst == nAblity[j] )
			{
				nAdj = 0;
				break;
			}
		}

		nAblity[i] = nDst;

		if ( nAdj == 0 )
			continue;
		
		if( IsDst_Rate(nDst) )
		{
			if( nDst == DST_ATTACKSPEED )
				strTemp.Format( "\n%s%+d%%", FindDstString( nDst ), nAdj / 2 / 10 );
			else
				strTemp.Format( "\n%s%+d%%", FindDstString( nDst ), nAdj );
		}
		else
		{
			strTemp.Format( "\n%s+%d", FindDstString( nDst ), nAdj );
		}
		
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwAddedOpt7 );
	}
}
#endif

void CWndMgr::PutEnchantOpt( CMover* pMover, FLItemElem* pItemElem, CEditString* pEdit, int flag )
{
	CString str;
	CString strTemp;
	BOOL bPSetItem = FALSE;
	int nAbilityOption	= pMover->GetSetItemClient();
	if( pMover->IsActiveMover() )
	{
		if( flag == APP_INVENTORY && pMover->m_Inventory.IsEquip(pItemElem->m_dwObjId) && pMover->IsSetItemPart( pItemElem->GetProp()->dwParts )
			&& nAbilityOption > 0 )
			bPSetItem = TRUE;
	}
	else
	{
		if( flag == APP_QUERYEQUIP && pMover->IsSetItemPart( pItemElem->GetProp()->dwParts ) && nAbilityOption > 0 )
			bPSetItem = TRUE;
	}
	if( bPSetItem ) // √‚∑¬
	{	
		PSETITEMAVAIL psa = prj.GetSetItemAvail( nAbilityOption );
		if( psa ) // DST_MAX_HITRATE
		{					
			if( psa->nHitRate > 0 )
			{
				strTemp.Format( "\n%s+%d%%", FindDstString( (int)DST_ADJ_HITRATE ), (int)psa->nHitRate );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwEnchantOption );
			}
			if( psa->nBlock > 0 )
			{
				str = prj.GetText( TID_GAME_TOOLTIPBLOCKRATE );
				strTemp.Format( "\n%s+%d%%", str, (int)psa->nBlock );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwEnchantOption );
			}
			if( psa->nMaxHitPointRate > 0 )
			{
				strTemp.Format( "\n%s+%d%%", FindDstString( (int)DST_HP_MAX_RATE ), (int)psa->nMaxHitPointRate );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwEnchantOption );
			}
			if( psa->nAddMagic > 0 )
			{
				strTemp.Format( "\n%s+%d", FindDstString( (int)DST_ADDMAGIC ), (int)psa->nAddMagic );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwEnchantOption );
			}
			if( psa->nAdded > 0 )
			{
				strTemp.Format( "\n%s+%d", FindDstString( (int)DST_STAT_ALLUP ), (int)psa->nAdded );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwEnchantOption );
			}
		}
	}
}

void CWndMgr::PutSetItemOpt( CMover* pMover, FLItemElem* pItemElem, CEditString* pEdit )
{
	CString strTemp;
	// 2. ºº∆Æ æ∆¿Ã≈€¿« ±∏º∫ ø‰º“ ¿Œ∞°?
	CSetItem* pSetItem	= CSetItemFinder::GetInstance()->GetSetItemByItemId( pItemElem->m_dwItemId );
	if( pSetItem )	{
		// «ÿ¥Á ºº∆Æ æ∆¿Ã≈€¿« ±∏º∫ ø‰º“∏¶ ∏µŒ √‚∑¬ «œµ« ¿Â¬¯ µ» ∞Õ∞˙ æ»µ» ∞Õ¿ª ªˆ¿∏∑Œ ±∏∫∞«œø© √‚∑¬«—¥Ÿ.
		int pbEquipedCount[MAX_HUMAN_PARTS] = { 0 };
		int nTotal, nEquiped;
		pMover->GetEquipedSetItem( pSetItem, pbEquipedCount, &nTotal, &nEquiped );
		// 3. ºº∆Ææ∆¿Ã≈€ ≈∏¿Ã∆≤ √‚∑¬
		strTemp.Format( "\n\n%s (%d/%d)", pSetItem->GetString(), nEquiped, nTotal );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwSetName );
		
		for( int i = 0; i < pSetItem->m_nElemSize; i++ )	{
			PT_ITEM_SPEC pItemProp	= g_xSpecManager->GetSpecItem( pSetItem->m_adwItemId[i] );
			if( pItemProp )	{
				strTemp.Format( "\n   %s", pItemProp->szName  );
				for( int j = 0; j < pSetItem->m_anEquipCount[i]; ++j )	{
					if( pbEquipedCount[i] > j )
						pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwSetItem1 );
					else
						pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwSetItem0 );
				}
			}
		}
		// 4. √ﬂ∞° ¥…∑¬ƒ° √‚∑¬
		ITEMAVAIL itemAvail;// = { 0 };
		pSetItem->GetItemAvail( &itemAvail, nEquiped, TRUE );
		for( int i = 0; i < itemAvail.nSize; i++ )	{
			int nDst = (int)itemAvail.anDstParam[i];
			int nAdj = (int)itemAvail.anAdjParam[i];
			
			if( IsDst_Rate(nDst) )	{
				if( nDst == DST_ATTACKSPEED )
					strTemp.Format( "\n%s: %s% +d%%", prj.GetText(TID_TOOLTIP_SET), FindDstString( nDst ), nAdj / 2 / 10 );	
				else
					strTemp.Format( "\n%s: %s% +d%%", prj.GetText(TID_TOOLTIP_SET), FindDstString( nDst ), nAdj );	
			}
			else	{
				strTemp.Format( "\n%s: %s +%d", prj.GetText(TID_TOOLTIP_SET), FindDstString( nDst ), nAdj );
			}
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwSetEffect );
		}
	}
}

#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
void CWndMgr::PutItemMinMax( CMover* pMover, FLItemElem* pItemElem, CEditString* pEdit, bool bCostume)
#else
void CWndMgr::PutItemMinMax( CMover* pMover, FLItemElem* pItemElem, CEditString* pEdit )
#endif
{
	pEdit->AddString( "\n" );
	CString strTemp;
	if( pItemElem->GetProp()->dwAbilityMin != 0xffffffff && pItemElem->GetProp()->dwAbilityMax != 0xffffffff 
		&& pItemElem->GetProp()->dwEndurance != 0xffffffff )
	{	
		DWORD dwColorMinMax = dwItemColor[g_Option.m_nToolTipText].dwEffective3;	// AàA¢¨A°ÌA¢“
		float f = pMover->GetItemMultiplier( pItemElem );
		
		int nMin	= (int)( pMover->GetItemAbilityMin( pItemElem->GetProp()->dwID ) * f );
		int nMax	= (int)( pMover->GetItemAbilityMax( pItemElem->GetProp()->dwID ) * f );
		if( 1.0f <= f )
			dwColorMinMax = dwItemColor[g_Option.m_nToolTipText].dwEffective0;		// A°∆AãAÅA¢¥
		else if( 0.8f <= f)
			dwColorMinMax = dwItemColor[g_Option.m_nToolTipText].dwEffective1;		// A©¯A°ÏA¢“A°Ì
		else if( 0.6f <= f )
			dwColorMinMax = dwItemColor[g_Option.m_nToolTipText].dwEffective2;		// AÄA°ÌA°ÌA¢“
#ifdef __ZCK_ARME_RARE
		const int nZckRarity = static_cast<int>( pItemElem->GetZckArmeRareLevel() );
		const ZckArmeRareProp* pZckRarity = pItemElem->IsZckArmeRareEligible() ? prj.GetZckArmeRare( nZckRarity ) : NULL;
		if( pZckRarity && nZckRarity > ZCK_ARME_RARE_MIN_LEVEL )
		{
			nMin = static_cast<int>( static_cast<float>( nMin ) * pZckRarity->m_fAttackRate );
			nMax = static_cast<int>( static_cast<float>( nMax ) * pZckRarity->m_fAttackRate );
			dwColorMinMax = pZckRarity->m_dwColor;
		}
#endif // __ZCK_ARME_RARE
		
		if( pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_DIRECT || pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_MAGIC )
		{	// A°∆A¢¨A°∆AùA°§AÇ
			int nOpt = 0;
			if( pItemElem->GetAbilityOption() > 0 )
			{
				int nAdd = (int)pow( (float)( pItemElem->GetAbilityOption() ), 1.5f );

				nMin += nAdd;
				nMax += nAdd;
			}
			if( nOpt )
			{
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ATTACKRANGE1) );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
				strTemp.Format( " (%d ~ %d)+%d", nMin, nMax, nOpt );
			}
			else
			{
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ATTACKRANGE2) );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
				strTemp.Format( " %d ~ %d", nMin, nMax );
			}
		}
		else
		{	// A©ˆA|A®˙A¢ÁA°§AÇ
			int nOpt = 0;			
			if( 
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
				!bCostume && 
#endif
				pItemElem->GetAbilityOption() > 0 )
			{
				int nAdd = (int)pow( (float)( pItemElem->GetAbilityOption() ), 1.5f );

				nMin += nAdd;
				nMax += nAdd;
			}
			if( nOpt )
			{	
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_DEFENSE_A) );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
				strTemp.Format( " (%d ~ %d)+%d", nMin, nMax, nOpt );
			}
			else
			{
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_DEFENSE_B) );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
				strTemp.Format( " %d ~ %d", nMin, nMax );
			}
		}
		pEdit->AddString( strTemp, dwColorMinMax );
	}
}

void CWndMgr::PutMedicine( FLItemElem* pItemElem, DWORD dwParam, LONG nParamVal, CEditString* pEdit )
{
	CString strTemp;
	if( dwParam != 0xffffffff )
	{	//  ƒ°∑·∑Æ
		if( nParamVal != 0xffffffff )
		{
			if( DST_MP == dwParam )
			{ // MP ƒ°∑·∑Æ
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_RECOVMP), nParamVal );
				pEdit->AddString( "\n" );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
			}
			else if( DST_HP == dwParam )
			{ // HP ƒ°∑·∑Æ
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_RECOVHP), nParamVal );
				pEdit->AddString( "\n" );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
			}
			else if( DST_FP == dwParam )
			{ // FP ƒ°∑·∑Æ
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_RECOVFP), nParamVal );
				pEdit->AddString( "\n" );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
			}
		}

		if( pItemElem->GetProp()->dwAbilityMin != 0xffffffff )
		{	// √÷¥Î»∏∫π∑Æ
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_MAXRECOVER), pItemElem->GetProp()->dwAbilityMin );
			pEdit->AddString( "\n" );
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
		}
	}
}

void CWndMgr::PutBaseItemOpt( FLItemElem* pItemElem, CEditString* pEdit )
{
	CString str;
	CString strTemp;
#ifdef __ZCK_ARME_RARE
	const int nZckRarity = static_cast<int>( pItemElem->GetZckArmeRareLevel() );
	const ZckArmeRareProp* pZckRarity = pItemElem->IsZckArmeRareEligible() ? prj.GetZckArmeRare( nZckRarity ) : NULL;
	const DWORD dwZckOptionColor = ( pZckRarity && nZckRarity > ZCK_ARME_RARE_MIN_LEVEL ) ? pZckRarity->m_dwColor : dwItemColor[g_Option.m_nToolTipText].dwGeneralOption;
#endif // __ZCK_ARME_RARE

	for( int i = 0; i < MAX_ITEM_PARAM; i ++ )	//sun: 10, __PROP_0827
	{
		if( pItemElem->GetProp()->dwDestParam[i] != 0xffffffff )		
		{
			int nAdj = static_cast<int>( pItemElem->GetProp()->nAdjParamVal[i] );
#ifdef __ZCK_ARME_RARE
			if( pZckRarity && nZckRarity > ZCK_ARME_RARE_MIN_LEVEL )
				nAdj = static_cast<int>( static_cast<float>( nAdj ) * pZckRarity->m_fStatsRate );
#endif // __ZCK_ARME_RARE

			if( pItemElem->GetProp()->dwDestParam[i] == DST_STAT_ALLUP )
			{
				str.Format( "\n%s%+d", FindDstString( DST_STR ), nAdj );
				strTemp = str;
				str.Format( "\n%s%+d", FindDstString( DST_DEX ), nAdj );
				strTemp += str;
				str.Format( "\n%s%+d", FindDstString( DST_INT ), nAdj );
				strTemp += str;
				str.Format( "\n%s%+d", FindDstString( DST_STA ), nAdj );
				strTemp += str;
			}
			else
			{
				int nDst	= (int)pItemElem->GetProp()->dwDestParam[i];
				if( IsDst_Rate( nDst ) )
				{
					if( nDst == DST_ATTACKSPEED )
						strTemp.Format( "\n%s%+d%%", FindDstString( nDst ), nAdj / 2 / 10 );
					else
						strTemp.Format( "\n%s%+d%%", FindDstString( nDst ), nAdj );
				}
				else
				{
					strTemp.Format( "\n%s%+d", FindDstString( (int)pItemElem->GetProp()->dwDestParam[i] ), nAdj );
				}
			}
#ifdef __ZCK_ARME_RARE
			pEdit->AddString( strTemp, dwZckOptionColor );
#else
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneralOption );
#endif // __ZCK_ARME_RARE
		}
	}
//sun: 11, AÉA¢¥AÅA®ˆ A®ˆAÉA®ˆA®¨AÖAõ
	CAccessoryProperty* pProperty	= CAccessoryProperty::GetInstance();
	if( pItemElem && pItemElem->IsAccessory() )		// A®˙AóA®˘A®˘A®˘A°©A¢¨A¢Á
	{
		const std::vector<SINGLE_DST>* pDst	= pProperty->GetDst( pItemElem->m_dwItemId, pItemElem->GetAbilityOption() );
		if( pDst != NULL && pDst->empty() == false )
		{
			const size_t nSize	= pDst->size();
			for( size_t i = 0; i < nSize; i++ )
			{
				int nDst	= (*pDst).at( i ).nDst;
				int nAdj	= (*pDst).at( i ).nAdj;
				if( nDst == DST_STAT_ALLUP )
				{
					str.Format( "\n%s%+d", FindDstString( DST_STR ), nAdj );
					strTemp = str;
					str.Format( "\n%s%+d", FindDstString( DST_DEX ), nAdj );
					strTemp += str;
					str.Format( "\n%s%+d", FindDstString( DST_INT ), nAdj );
					strTemp += str;
					str.Format( "\n%s%+d", FindDstString( DST_STA ), nAdj );
					strTemp += str;
				}
				else
				{
					if( IsDst_Rate( nDst ) )
					{
						if( nDst == DST_ATTACKSPEED )
							strTemp.Format( "\n%s%+d%%", FindDstString( nDst ), nAdj / 2 / 10 );
						else
							strTemp.Format( "\n%s%+d%%", FindDstString( nDst ), nAdj );
					}
					else
					{
						strTemp.Format( "\n%s%+d", FindDstString( nDst ), nAdj );
					}
				}
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneralOption );
			}
		}
	}
}

//sun: 9¬˜ ¿¸Ω¬∞¸∑√ Client
void CWndMgr::PutAddedOpt( FLItemElem* pItemElem, CEditString* pEdit )
{
	CString str;
	CString strTemp;
	int i;
	int nCount = 0;
	
	int nAddedOptDST[9];
	int nAddedOpt[5];
	int nAddedValue[5];

	for(i=0; i<5; i++)
	{
		nAddedOpt[i] = -1;
		nAddedValue[i] = -1;
	}

	//DST ¡§¿«µ«¥¬¥Î∑Œ æ∆∑° ºˆ¡§.
	nAddedOptDST[0] = DST_HP_MAX;
	nAddedOptDST[1] = DST_ATKPOWER;
	nAddedOptDST[2] = DST_ADJDEF;
	nAddedOptDST[3] = DST_MELEE_STEALHP;
	nAddedOptDST[4] = DST_PVP_DMG;
	nAddedOptDST[5] = DST_STR;
	nAddedOptDST[6] = DST_STA;
	nAddedOptDST[7] = DST_DEX;
	nAddedOptDST[8] = DST_INT;

	std::map<int, int> mapDst = prj.m_UltimateWeapon.GetDestParamUltimate( pItemElem );

	for( std::map<int, int>::iterator it=mapDst.begin(); it!=mapDst.end(); it++ )
	{
		int a = it->first;
		for(i=0; i<9; i++)
		{
			if( nAddedOptDST[i] == it->first )
			{
				nAddedOpt[nCount] = i;
				nAddedValue[nCount] = it->second;
				nCount++;
			}
		}
	}

	for(i=0; i<nCount; i++)
	{
		//√ﬂ∞° ø…º«¿ª ≈¯∆¡ø° √ﬂ∞°.
		if(nAddedOpt[i] >= 0)
		{
			if(FindDstString( nAddedOptDST[nAddedOpt[i]] ))
			{
				if(nAddedOpt[i] == 3 || nAddedOpt[i] == 4)
					str.Format( "\n%s+%d%%", FindDstString( nAddedOptDST[nAddedOpt[i]] ), nAddedValue[i] );
				else
					str.Format( "\n%s+%d", FindDstString( nAddedOptDST[nAddedOpt[i]] ), nAddedValue[i] );
			}

			strTemp = str;
//sun: 12, π´±‚ ««æÓΩÃ
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwAddedOpt7 );
		}			
	}			
}

void CWndMgr::PutPetInfo( FLItemElem* pItemElem, CEditString* pEdit )
{
	if(pItemElem == NULL || pEdit == NULL)
		return;
	
	CString strTemp;
	pEdit->Empty();
	//Name
//	strTemp.Format( "%s", pItemElem->GetProp()->szName );
	strTemp	= pItemElem->GetName();
	pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwName0, ESSTY_BOLD );

	PutPetKind( pItemElem, pEdit );

//	int nLife = pItemElem->m_pPet->GetLife();
//	if(nLife <= 0)
	if( pItemElem->IsFlag( FLItemElem::expired ) )
	{
		strTemp.Format(" %s", prj.GetText(TID_GAME_PETINFO_DEAD));
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwResistSM1, ESSTY_BOLD );
	}

	//Level
	int nLevel = PL_EGG;
	if( pItemElem->m_pPet )
	{
		nLevel	= pItemElem->m_pPet->GetLevel();

		DWORD dwLevelText;
		switch(nLevel)
		{
			case PL_EGG:
				dwLevelText = TID_GAME_PETGRADE_E;
				break;
			case PL_D:
				dwLevelText = TID_GAME_PETGRADE_D;
				break;
			case PL_C:
				dwLevelText = TID_GAME_PETGRADE_C;
				break;
			case PL_B:
				dwLevelText = TID_GAME_PETGRADE_B;
				break;
			case PL_A:
				dwLevelText = TID_GAME_PETGRADE_A;
				break;
			case PL_S:
				dwLevelText = TID_GAME_PETGRADE_S;
				break;
		}
		strTemp.Format( "%s : %s", prj.GetText(TID_GAME_CHARACTER_02), prj.GetText(dwLevelText) );
		pEdit->AddString( "\n" );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwResistSM );

		//Ability value
		if(nLevel != PL_EGG)
		{
			DWORD dwDstParam;
			int nParam;
			DWORD dwTooltip;
			pItemElem->m_pPet->GetAvailDestParam(dwDstParam, nParam);
			
			switch(dwDstParam) 
			{
				case DST_STR:
					dwTooltip = TID_TOOLTIP_STR;
					break;
				case DST_DEX:
					dwTooltip = TID_TOOLTIP_DEX;
					break;
				case DST_INT:
					dwTooltip = TID_TOOLTIP_INT;
					break;
				case DST_STA:
					dwTooltip = TID_TOOLTIP_STA;
					break;
				case DST_ATKPOWER:
					dwTooltip	= TID_TOOLTIP_ATKPOWER_VALUE;
					break;
				case DST_ADJDEF:
					dwTooltip = TID_TOOLTIP_DEFENCE;
					break;
				case DST_HP_MAX:
					dwTooltip = TID_TOOLTIP_DST_HP_MAX;
					break;
			}
			strTemp.Format( "%s : %s +%d", prj.GetText(TID_GAME_ABILITY), prj.GetText(dwTooltip), nParam );
			pEdit->AddString( "\n" );
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwResistSM1 );
		}
		//Level History
		int nLevel = pItemElem->m_pPet->GetLevel();
		if(nLevel > PL_EGG)
		{
			pEdit->AddString( "\n" );
			pEdit->AddString( "(", D3DCOLOR_XRGB(0, 200, 255) );
			for(int i=PL_D; i<=nLevel; i++)
			{
				BYTE bLevel = pItemElem->m_pPet->GetAvailLevel(i);
				strTemp.Format(prj.GetText(TID_GAME_PET_TOOLTIP_LEVEL), bLevel);
				pEdit->AddString( strTemp, D3DCOLOR_XRGB(0, 127, 255) );
				if(i != nLevel)
					pEdit->AddString( "/", D3DCOLOR_XRGB(0, 200, 255) );
			}
			pEdit->AddString( ")", D3DCOLOR_XRGB(0, 200, 255) );

			//Pet Experience
			EXPINTEGER	nExpResult = pItemElem->m_pPet->GetExp() * (EXPINTEGER)10000 / pItemElem->m_pPet->GetMaxExp();
			float fExp = (float)nExpResult / 100.0f;

			if( fExp >= 99.99f )
				strTemp.Format(prj.GetText(TID_GAME_PET_TOOLTIP_EXP_MAX));
			else
				strTemp.Format(prj.GetText(TID_GAME_PET_TOOLTIP_EXP), fExp);
			pEdit->AddString( "\n" );
			pEdit->AddString( strTemp, D3DCOLOR_XRGB(120, 120, 220) );

			//Pet Energy
			int nMaxEnergy = pItemElem->m_pPet->GetMaxEnergy();
			int nEnergy = pItemElem->m_pPet->GetEnergy();
			int nLife = pItemElem->m_pPet->GetLife();
			pEdit->AddString( "\n" );
			strTemp.Format(prj.GetText(TID_GAME_PET_TOOLTIP_LIFE), nLife);
			pEdit->AddString( strTemp, D3DCOLOR_XRGB(255, 100, 100) );
			pEdit->AddString( "\n" );
			strTemp.Format(prj.GetText(TID_GAME_PET_TOOLTIP_HP), nEnergy, nMaxEnergy);
			pEdit->AddString( strTemp, D3DCOLOR_XRGB(255, 10, 10) );
		}
		else
		{
			//Pet Experience
			EXPINTEGER	nExpResult = pItemElem->m_pPet->GetExp() * (EXPINTEGER)10000 / pItemElem->m_pPet->GetMaxExp();
			float fExp = (float)nExpResult / 100.0f;

			if( fExp >= 99.99f )
				strTemp.Format(prj.GetText(TID_GAME_PET_TOOLTIP_EXP_MAX));
			else
				strTemp.Format(prj.GetText(TID_GAME_PET_TOOLTIP_EXP), fExp);
			pEdit->AddString( "\n" );
			pEdit->AddString( strTemp, D3DCOLOR_XRGB(120, 120, 220) );
		}

		//Description
		strTemp.Format( "%s", pItemElem->GetProp()->szCommand );
		pEdit->AddString( "\n" );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwPiercing );
	}
}

void CWndMgr::PutPetFeedPocket( FLItemElem* pItemElem, CEditString* pEdit )
{
	CString strTemp, str;
	pEdit->Empty();

	if(pItemElem->m_dwKeepTime > 0) //¿Ø∑·æ∆¿Ã≈€¿Ã ªÁøÎµ» ªÛ≈¬¿Œ∞°?
	{
		//Name
		strTemp.Format( "%s", pItemElem->GetProp()->szName );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwName0, ESSTY_BOLD );

		if(g_pPlayer->HasBuff( BUFF_ITEM, ITEM_INDEX( 21035, II_SYS_SYS_SCR_PET_FEED_POCKET ) )) //»∞º∫»≠
		{
			strTemp.Format( " %s", prj.GetText(TID_GAME_POCKETUSING) );
			pEdit->AddString( strTemp, D3DCOLOR_XRGB( 255, 0, 0 ) );

			//ªÁøÎ ¡¶«— Ω√«—
			time_t t = pItemElem->m_dwKeepTime - time_null();
			if( pItemElem->m_dwKeepTime && !pItemElem->IsFlag( FLItemElem::expired ) )
			{
				if( t > 0 )
				{
					CTimeSpan time( t );
					if( time.GetDays() )
						str.Format( prj.GetText(TID_PK_LIMIT_DAY), static_cast<int>(time.GetDays()+1) );
					else if( time.GetHours() )
						str.Format( prj.GetText(TID_PK_LIMIT_HOUR), time.GetHours() );
//sun: 11, ±‚∞£¡¶ æ∆¿Ã≈€ ¡ˆº”Ω√∞£ √ ¥‹¿ß «•±‚
					else if( time.GetMinutes() )
						str.Format( prj.GetText(TID_PK_LIMIT_MINUTE), time.GetMinutes() );
					else
						str.Format( prj.GetText(TID_PK_LIMIT_SECOND), time.GetSeconds() );
				}

				strTemp = str + prj.GetText(TID_TOOLTIP_PERIOD);	
				pEdit->AddString( "\n" );
				pEdit->AddString( strTemp, D3DCOLOR_XRGB( 255, 20, 20 ) );
			}
			
			//ªÁ∑· ∞≥ºˆ
			pEdit->AddString( "\n" );
			strTemp.Format( "%s %d", prj.GetText( TID_GAME_PET_FEED_COUNT ), g_pPlayer->GetItemNumForClient( ITEM_INDEX( 21037, II_SYS_SYS_FEED_01 ) ) );
			pEdit->AddString( strTemp, D3DCOLOR_XRGB( 50, 50, 205 ) ); 
			
			//¡ˆº” ∞°¥… Ω√«—
/*
			CTimeSpan time( g_pPlayer->GetItemNum( ITEM_INDEX( 21037, II_SYS_SYS_FEED_01 ) ) * 2 );
			pEdit->AddString( "\n" );
			strTemp.Format( "%s : ", prj.GetText( TID_TOOLTIP_ITEMTIME ) );
			pEdit->AddString( strTemp, D3DCOLOR_XRGB( 107, 35, 142 ) );

			if(time.GetDays())
			{
				strTemp.Format( prj.GetText( TID_GAME_LIMIT_DAY ), time.GetDays() );	
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwTime );
				pEdit->AddString( " " );
			}
			if(time.GetHours())
			{
				strTemp.Format( prj.GetText( TID_GAME_LIMIT_HOUR ), time.GetHours() );	
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwTime );
				pEdit->AddString( " " );
			}
			if(time.GetMinutes())
			{
				strTemp.Format( prj.GetText( TID_GAME_LIMIT_MINUTE ), time.GetMinutes() );	
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwTime );
				pEdit->AddString( " " );
			}
			if(time.GetSeconds())
			{
				strTemp.Format( prj.GetText( TID_GAME_LIMIT_SECOND ), time.GetSeconds() );	
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwTime );
			}
*/
		}
		else //∫Ò»∞º∫»≠
		{
			strTemp.Format( " %s", prj.GetText(TID_GAME_POCKET_NOTUSING) );
			pEdit->AddString( strTemp, D3DCOLOR_XRGB( 255, 0, 0 ) );

			//ªÁøÎ ¡¶«— Ω√«—
			time_t t = pItemElem->m_dwKeepTime - time_null();
			if( pItemElem->m_dwKeepTime && !pItemElem->IsFlag( FLItemElem::expired ) )
			{
				if( t > 0 )
				{
					CTimeSpan time( t );
					if( time.GetDays() )
						str.Format( prj.GetText(TID_PK_LIMIT_DAY), static_cast<int>(time.GetDays()+1) );
					else if( time.GetHours() )
						str.Format( prj.GetText(TID_PK_LIMIT_HOUR), time.GetHours() );
//sun: 11, ±‚∞£¡¶ æ∆¿Ã≈€ ¡ˆº”Ω√∞£ √ ¥‹¿ß «•±‚
					else if( time.GetMinutes() )
						str.Format( prj.GetText(TID_PK_LIMIT_MINUTE), time.GetMinutes() );
					else
						str.Format( prj.GetText(TID_PK_LIMIT_SECOND), time.GetSeconds() );
				}

				strTemp = str + prj.GetText(TID_TOOLTIP_PERIOD);	
				pEdit->AddString( "\n" );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwTime );
			}		
		}
		
		//Description
		strTemp.Format( "%s", prj.GetText(TID_GAME_PET_FEEDPOCKET) );
		pEdit->AddString( "\n" );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwPiercing );
	}
	else
	{
		//Name
		strTemp.Format( "%s", pItemElem->GetProp()->szName );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwName0, ESSTY_BOLD );
		//Description
		strTemp.Format( "%s", prj.GetText(TID_GAME_PET_FEEDPOCKET_USE) );
		pEdit->AddString( "\n" );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwPiercing );				
	}
}

//sun, 11 ƒ…∏Ø≈Õ ∫¿¿Œ ∞≈∑° ±‚¥… world,database,neuz
void CWndMgr::PutSealChar( FLItemElem* pItemElem, CEditString* pEdit )
{
	CString str,strTemp,strTemp2,strTemp3;

	str.Format( "\n%s\n%s", prj.GetText(TID_TOOLTIP_CHARNAME), pItemElem->m_szItemText );
	strTemp = str;

	int	nJob = (int)pItemElem->m_nRepair;
	int nLevel =  (int)pItemElem->m_nRepairNumber;

	if( MAX_PROFESSIONAL <= nJob && nJob < MAX_MASTER )
		strTemp2.Format( "%d%s", nLevel, prj.GetText( TID_GAME_TOOLTIP_MARK_MASTER ) );
	else if( MAX_MASTER <= nJob )
		strTemp2.Format( "%d%s", nLevel, prj.GetText( TID_GAME_TOOLTIP_MARK_HERO ) );
	else 
		strTemp2.Format( "%d", nLevel );

	str.Format( "\n%s%s", prj.GetText(TID_TOOLTIP_CHARLEVEL), strTemp2 );
	strTemp += str;
	
	strTemp3.Format("%s",prj.m_aJob[nJob].szName);

	str.Format( "\n%s%s", prj.GetText(TID_TOOLTIP_CHARJOB), strTemp3 );
	strTemp += str;
	str.Format( "\n%s", prj.GetText(TID_TOOLTIP_CHARSTAT) );
	strTemp += str;

	
	str.Format( "\n%s%d", prj.GetText(TID_TOOLTIP_CHARSTA), (int)pItemElem->GetGeneralPiercingItemID( 0 ) );
	strTemp += str;
	str.Format( "\n%s%d", prj.GetText(TID_TOOLTIP_CHARSTR), (int)pItemElem->GetGeneralPiercingItemID( 1 ) );
	strTemp += str;
	str.Format( "\n%s%d", prj.GetText(TID_TOOLTIP_CHARDEX), (int)pItemElem->GetGeneralPiercingItemID( 2 ) );
	strTemp += str;
	str.Format( "\n%s%d", prj.GetText(TID_TOOLTIP_CHARINT), (int)pItemElem->GetGeneralPiercingItemID( 3 ) );
	strTemp += str;
	str.Format( "\n%s%d", prj.GetText(TID_TOOLTIP_CHARPOINT), (int)pItemElem->m_nResistAbilityOption );
	strTemp += str;

	pEdit->AddString( strTemp, D3DCOLOR_XRGB( 255, 20, 20 ) );
}


void CWndMgr::PutHitPoint( FLItemElem* pItemElem, CEditString* pEdit )
{
	FLERROR_ASSERT_LOG_RETURN_VOID( pItemElem, _T("[¿©µµøÏ ∏ﬁ¥œ¡Æ] æ∆¿Ã≈€ º”º∫ ∆˜¿Œ≈Õ∞° ¿Ø»ø«œ¡ˆ æ Ω¿¥œ¥Ÿ.") );

	CString strHitPoint( _T("") );
	strHitPoint.Append( _T("\n") );
	
	CString strTemp( _T("") );
	strTemp.Format( prj.GetText(TID_MMI_ELLDINPOTIONTOOLTIP_TEXT01), pItemElem->m_nHitPoint );
	
	strHitPoint.Append( strTemp );
	
	pEdit->AddString( strHitPoint );
}

void CWndMgr::PutBaseResist( FLItemElem* pItemElem, CEditString* pEdit )
{
	CString strTemp;
	if( pItemElem->GetProp()->nItemResistElecricity )
	{
		strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ELEC_RES), pItemElem->GetProp()->nItemResistElecricity );
		pEdit->AddString( "\n" );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwResistElectricity );
	}
	if( pItemElem->GetProp()->nItemResistFire )
	{
		strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_FIRE_RES), pItemElem->GetProp()->nItemResistFire );
		pEdit->AddString( "\n" );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwResistFire );
	}
	if( pItemElem->GetProp()->nItemResistWater )
	{
		strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_WATER_RES), pItemElem->GetProp()->nItemResistWater );
		pEdit->AddString( "\n" );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwResistWater );
	}
	if( pItemElem->GetProp()->nItemResistWind )
	{
		strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_WIND_RES), pItemElem->GetProp()->nItemResistWind );
		pEdit->AddString( "\n" );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwResistWind );
	}
	if( pItemElem->GetProp()->nItemResistEarth )
	{
		strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_EARTH_RES), pItemElem->GetProp()->nItemResistEarth );
		pEdit->AddString( "\n" );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwResistEarth );
	}
}

void CWndMgr::PutItemGold( CMover* pMover, FLItemElem* pItemElem, CEditString* pEdit, int flag )
{
	CString str;
	CString strTemp;
	if( flag == APP_SHOP_ ) // ∞°∞›¿∫ ≥™¥¬ æ»≥™ø¿∞‘ «œ∞Ì ªÛ¡° ¿Œ≈Õ∆‰¿ÃΩ∫ø°º≠¥¬ ≥™ø¿∞‘«‘ 42¿∫ ªÛ¡°¿« Invantory ∞≥ºˆ
	{	// ªÛ¡°
		if( pItemElem->GetCost() != 0xffffffff ) 
		{	// ∞°∞›
//sun: 11, ƒ®¿∏∑Œ ªÛ¡° ¿ÃøÎ «œ±‚ (±ÊµÂ ¥Î¿¸ ∞¸∑√)
			CWndShop* pWndShop	= (CWndShop*)GetWndBase( APP_SHOP_ );
			LPCHARACTER lpCharacter = pWndShop->m_pMover->GetCharacter();
			int nCost = 0;
			int nBeforeTax = 0;
			if(lpCharacter)
			{
				if(lpCharacter->m_nVenderType == VENDOR_TYPE_PENYA)
				{
					nCost = pItemElem->GetCost();
					nCost = (int)( prj.m_fShopBuyRate * nCost );
				//sun: 12, ºº±›
					nBeforeTax = nCost;
					if( CTax::GetInstance()->IsApplyPurchaseTaxRate( pWndShop->m_pMover, pItemElem ) )
						nCost += ( static_cast<int>( nCost * CTax::GetInstance()->GetPurchaseTaxRate( pWndShop->m_pMover) ) );
				}
				else if(lpCharacter->m_nVenderType == VENDOR_TYPE_CHIP)
					nCost = pItemElem->GetChipCost();
			}

			nCost = (int)(nCost * prj.m_fShopCost );
//sun: ºˆ«• ±‚¥… ∆‰∏∞ ∞¸∑√
			if( pItemElem->m_dwItemId == ITEM_INDEX( 26456, II_SYS_SYS_SCR_PERIN ) )
				nCost = PERIN_VALUE;
			
			if( nCost <= 0 )
			{
				nCost = 1;
				nBeforeTax = 1;	//sun: 12, ºº±›
			}
			
			char szbuffer[1024];
			FLIntToString( nCost, szbuffer, _countof( szbuffer ), 10 );
			str = GetNumberFormatEx(szbuffer);
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_COST2), str );
//sun: 11, ƒ®¿∏∑Œ ªÛ¡° ¿ÃøÎ «œ±‚ (±ÊµÂ ¥Î¿¸ ∞¸∑√)
			if(lpCharacter && lpCharacter->m_nVenderType == VENDOR_TYPE_CHIP)
			{
				CString temp;
				temp.Format(" %s", prj.GetText(TID_GAME_REDCHIP));
				strTemp += temp;
			}

			pEdit->AddString( "\n" );
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGold );
//sun: 12, ºº±›
			if( CTax::GetInstance()->IsApplyPurchaseTaxRate( pWndShop->m_pMover, pItemElem )
				&& lpCharacter->m_nVenderType == VENDOR_TYPE_PENYA
				)
			{
				FLIntToString( nBeforeTax, szbuffer, _countof( szbuffer ), 10 );
				str = GetNumberFormatEx(szbuffer);
				strTemp.Format( prj.GetText( TID_GAME_TAX_RATE ), str );
				pEdit->AddString( "\n" );
				pEdit->AddString( strTemp, prj.GetTextColor( TID_GAME_TAX_RATE ) );
			}
		}
	}
	else if( flag == APP_SHOP_CART_ ) // ∞°∞›¿∫ ≥™¥¬ æ»≥™ø¿∞‘ «œ∞Ì ªÛ¡° ¿Œ≈Õ∆‰¿ÃΩ∫ø°º≠¥¬ ≥™ø¿∞‘«‘ 42¿∫ ªÛ¡°¿« Invantory ∞≥ºˆ
	{
		// _JIN_SHOP_CART
		// ªÛ¡°
		if( pItemElem->GetCost() != 0xffffffff ) 
		{	// ∞°∞›
			//sun: 11, ƒ®¿∏∑Œ ªÛ¡° ¿ÃøÎ «œ±‚ (±ÊµÂ ¥Î¿¸ ∞¸∑√)
			CFLWndShop_Cart* pWndShop_Cart = (CFLWndShop_Cart*)GetWndBase( APP_SHOP_CART_ );
			LPCHARACTER lpCharacter = pWndShop_Cart->m_pMover->GetCharacter();
			int nCost = 0;
			int nBeforeTax = 0;
			if(lpCharacter)
			{
				if(lpCharacter->m_nVenderType == 0)
				{
					nCost = pItemElem->GetCost();
					nCost = (int)( prj.m_fShopBuyRate * nCost );
					//sun: 12, ºº±›
					nBeforeTax = nCost;
					if( CTax::GetInstance()->IsApplyPurchaseTaxRate( pWndShop_Cart->m_pMover, pItemElem ) )
						nCost += ( static_cast<int>(nCost * CTax::GetInstance()->GetPurchaseTaxRate( pWndShop_Cart->m_pMover )) );
				}
				else if(lpCharacter->m_nVenderType == 1)
					nCost = pItemElem->GetChipCost();
			}

			nCost = (int)(nCost * prj.m_fShopCost );
			//sun: ºˆ«• ±‚¥… ∆‰∏∞ ∞¸∑√
			if( pItemElem->m_dwItemId == ITEM_INDEX( 26456, II_SYS_SYS_SCR_PERIN ) )
				nCost = PERIN_VALUE;

			if( nCost <= 0 )
			{
				nCost = 1;
				nBeforeTax = 1;	//sun: 12, ºº±›
			}

			char szbuffer[1024];
			FLIntToString( nCost, szbuffer, _countof( szbuffer ), 10 );
			str = GetNumberFormatEx(szbuffer);
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_COST2), str );
			//sun: 11, ƒ®¿∏∑Œ ªÛ¡° ¿ÃøÎ «œ±‚ (±ÊµÂ ¥Î¿¸ ∞¸∑√)
			if(lpCharacter && lpCharacter->m_nVenderType == 1)
			{
				CString temp;
				temp.Format(" %s", prj.GetText(TID_GAME_REDCHIP));
				strTemp += temp;
			}

			pEdit->AddString( "\n" );
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGold );
			//sun: 12, ºº±›
			if( CTax::GetInstance()->IsApplyPurchaseTaxRate( pWndShop_Cart->m_pMover, pItemElem )
				&& lpCharacter->m_nVenderType == 0
				)
			{
				FLIntToString( nBeforeTax, szbuffer, _countof( szbuffer ), 10 );
				str = GetNumberFormatEx(szbuffer);
				strTemp.Format( prj.GetText( TID_GAME_TAX_RATE ), str );
				pEdit->AddString( "\n" );
				pEdit->AddString( strTemp, prj.GetTextColor( TID_GAME_TAX_RATE ) );
			}
		}
	}
	else if( flag == APP_INVENTORY )	// ¿Œ∫•≈‰∏Æ / π≈© ≈¯∆¡?
	{
							
		// _JIN_SHOP_CART
		switch( _GetContentState(CT_SHOP_CART_17) )
		{
		case CS_VER1:
			{
				CWndShop* pWndShop	= (CWndShop*)GetWndBase( APP_SHOP_ );
				if( pWndShop )	// ªÛ¡°¿ª ø≠æ˘¿ª∂ß ∆ƒ¥¬∞°∞›µµ ≥™øÕæﬂ«‘
				{
					if( pItemElem->GetCost() != 0xffffffff )
					{	//	∆ƒ¥¬∞°∞›
						char szbuffer[1024];
						DWORD dwCostTem = 0;
						dwCostTem = pItemElem->GetCost() / 4;
						dwCostTem = (DWORD)( prj.m_fShopSellRate * dwCostTem );
						//sun: 12, ºº±›
						DWORD dwBeforeTax = dwCostTem;
						if( CTax::GetInstance()->IsApplySalesTaxRate( g_pPlayer, pItemElem ) )
							dwCostTem -= ( static_cast<DWORD>(dwCostTem * CTax::GetInstance()->GetSalesTaxRate( g_pPlayer )) );

						if( dwCostTem < 1 )
						{				
							dwCostTem = 1;
							dwBeforeTax = 1;		//sun: 12, ºº±›
						}

						FLIntToString( dwCostTem, szbuffer, _countof( szbuffer ), 10 );
						str = GetNumberFormatEx(szbuffer);
						strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_SHELLCOST2), str );
						pEdit->AddString( "\n" );
						pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGold );
						//sun: 12, ºº±›
						if( CTax::GetInstance()->IsApplySalesTaxRate( g_pPlayer, pItemElem ) )
						{
							FLIntToString( dwBeforeTax, szbuffer, _countof( szbuffer ), 10 );
							str = GetNumberFormatEx(szbuffer);
							strTemp.Format( prj.GetText( TID_GAME_TAX_RATE ), str );
							pEdit->AddString( "\n" );
							pEdit->AddString( strTemp, prj.GetTextColor( TID_GAME_TAX_RATE ) );	
						}
					}
				}
			}
			break;

		case CS_VER2:
			{
				CFLWndShop_Cart* pWndShop_Cart	= (CFLWndShop_Cart*)GetWndBase( APP_SHOP_CART_);
				if( pWndShop_Cart )	// ªÛ¡°¿ª ø≠æ˘¿ª∂ß ∆ƒ¥¬∞°∞›µµ ≥™øÕæﬂ«‘
				{
					if( pItemElem->GetCost() != 0xffffffff )
					{	//	∆ƒ¥¬∞°∞›
						char szbuffer[1024];
						DWORD dwCostTem = 0;
						dwCostTem = pItemElem->GetCost() / 4;
						dwCostTem = (DWORD)( prj.m_fShopSellRate * dwCostTem );
						//sun: 12, ºº±›
						DWORD dwBeforeTax = dwCostTem;
						if( CTax::GetInstance()->IsApplySalesTaxRate( g_pPlayer, pItemElem ) )
							dwCostTem -= ( static_cast<DWORD>(dwCostTem * CTax::GetInstance()->GetSalesTaxRate( g_pPlayer )) );

						if( dwCostTem < 1 )
						{				
							dwCostTem = 1;
							dwBeforeTax = 1;		//sun: 12, ºº±›
						}

						FLIntToString( dwCostTem, szbuffer, _countof( szbuffer ), 10 );
						str = GetNumberFormatEx(szbuffer);
						strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_SHELLCOST2), str );
						pEdit->AddString( "\n" );
						pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGold );
						//sun: 12, ºº±›
						if( CTax::GetInstance()->IsApplySalesTaxRate( g_pPlayer, pItemElem ) )
						{
							FLIntToString( dwBeforeTax, szbuffer, _countof( szbuffer ), 10 );
							str = GetNumberFormatEx(szbuffer);
							strTemp.Format( prj.GetText( TID_GAME_TAX_RATE ), str );
							pEdit->AddString( "\n" );
							pEdit->AddString( strTemp, prj.GetTextColor( TID_GAME_TAX_RATE ) );	
						}
					}
				}
			}
			break;

		default:
			FLASSERT( 0 );
			break;
		}
	}
	else if( flag == APP_VENDOR )
	{
		if( pItemElem->m_nCost != 0xffffffff )
		{	// ∆ƒ¥¬∞°∞›
			char szbuffer[1024];
			DWORD dwCostTem = 0;
			dwCostTem = pItemElem->m_nCost;
			FLIntToString( dwCostTem, szbuffer, _countof( szbuffer ), 10 );
			str = GetNumberFormatEx(szbuffer);
			
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_SHELLCOST2), str );
			pEdit->AddString( "\n" );
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGold );
		}
	}
}

void CWndMgr::PutCoolTime( CMover* pMover, FLItemElem* pItemElem, CEditString* pEdit )
{
	CString strTemp;
	DWORD dwGroup = pMover->m_cooltimeMgr.GetGroup( pItemElem->GetProp() );
	if( dwGroup )
	{
		DWORD dwCur = g_tmCurrent;
		DWORD dwEnd = pMover->m_cooltimeMgr.GetTime( dwGroup );		// ¿Ã∫•∆Æ ¡æ∑· Ω√∞¢ 
		if( dwEnd != 0 && dwEnd > dwCur )
		{
			DWORD dwBase = pMover->m_cooltimeMgr.GetBase( dwGroup );	// ¿Ã∫•∆Æ Ω√¿€ Ω√∞¢ 
			
			CTimeSpan ct( (dwEnd - dwCur + 500) / 1000 );		// ≥≤¿∫Ω√∞£¿ª √ ¥‹¿ß∑Œ ∫Ø»Ø«ÿº≠ ≥—∞‹¡‹, +500 π›ø√∏≤ 
			strTemp.Format( prj.GetText(TID_TOOLTIP_COOLTIME), ct.GetMinutes(), ct.GetSeconds() );		// ≥≤¿∫Ω√∞£¿ª ∫–/√  «¸≈¬∑Œ √‚∑¬.
			pEdit->AddString( "\n" );
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );	
		}
	}
}

void CWndMgr::PutKeepTime( FLItemElem* pItemElem, CEditString* pEdit )
{
	CString str, strTemp;

	const PT_ITEM_SPEC pItemSpec	= pItemElem->GetProp();
	if( pItemSpec == NULL ) {
		return;
	}

	if( pItemSpec->dwCircleTime != 0xffffffff )
	{
		pEdit->AddString( "\n" );
		if( pItemSpec->dwCircleTime == 1 )
		{
			pEdit->AddString( prj.GetText( TID_GAME_COND_USE ), dwItemColor[g_Option.m_nToolTipText].dwTime ); // ªÁ∏¡Ω√ »ø∞˙ ¿˚øÎ
		}
		else
		{
			CTimeSpan ct( pItemSpec->dwCircleTime );
			strTemp.Format( "%s : ", prj.GetText( TID_TOOLTIP_ITEMTIME ) );	// ¡ˆº”Ω√∞£ : 
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwTime ); 
			strTemp.Format( prj.GetText( TID_TOOLTIP_DATE ), static_cast<int>(ct.GetDays()), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );	// ¡ˆº”Ω√∞£ : 
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwTime ); 
		}
	}

	time_t t	= pItemElem->m_dwKeepTime - time_null();
	DWORD dwKeepTimeForTip = pItemElem->m_dwKeepTime;

	//∫ÒΩ∫ ≥≤¿∫ Ω√∞£ «•Ω√( ¿Œ∫•ø° ¿÷¥¬,, æ∆¡˜ ªÁøÎ¿¸¿Ãπ«∑Œ º≠πˆµ•¿Ã≈Õ∞° æ¯¥Ÿ. Propø°º≠ ∞°¡Æ¥Ÿ æ¥¥Ÿ ( ¥‹¿ß¥¬ ∫– ) 2009_11_10 
	if( pItemSpec->IsVis( ) ) {
		/************************************************************************/
		/* dwSkillTime ¿Ã 999999999 ¿Ã∏È øµ±∏. Ω√∞£ √‚∑¬ «œ¡ˆ æ ¥¬¥Ÿ.           */
		if( pItemSpec->dwSkillTime == 999999999 ) {
			dwKeepTimeForTip	= 0;
		}
		/************************************************************************/
		else if( t <= 0  )		// ¿Â¬¯µ» ∫ÒΩ∫∏∏ ¿Ø¡ˆΩ√∞£¿ª πﬁ∞‘ µ«∞Ì, ¿Œ∫•ø° ¿ßƒ°«— ∫ÒΩ∫¥¬ Propø°º≠ ∞°¡Æ¥Ÿ Ω·æﬂ «‘.
		{
			//	pItemElem->m_dwKeepTime = pItemElem->GetProp()->dwAbilityMin;
			//	t = (time_t)( pItemElem->m_dwKeepTime * 60.0f );		//∫–¥‹¿ß ∑Œ πŸ≤Ò 

			//gmpbigsun(20110920) : #19403
			//ItemElem¿« ∆˜¿Œ≈Õø° ¥Î«ÿ ¡˜¡¢ ∞™¿ª ¥Î¿‘«œ∏È æ»µ»¥Ÿ( Ω∫≈√∞ÀªÁø°º≠ Ω«∆–∂‰ )
			dwKeepTimeForTip = pItemSpec->dwAbilityMin;
			t = (time_t)( dwKeepTimeForTip * 60.0f );		//∫–¥‹¿ß ∑Œ πŸ≤Ò 
		}
	}

	if( dwKeepTimeForTip && !pItemElem->IsFlag( FLItemElem::expired ) )
	{
		if( t > 0 )
		{
			CTimeSpan time( t );
			if( g_xSpecManager->IsChargeZoneTicket( pItemElem->m_dwItemId ) != false )	// ø÷ ∆ºƒœ∏∏?
			{
				str.Format( prj.GetText( TID_TOOLTIP_DATE ), static_cast<int>(time.GetDays()), time.GetHours(), time.GetMinutes(), time.GetSeconds() );
			}
			else
			{
				if( pItemSpec->IsVis( ) )		// gmpbigsun : ¿œ, Ω√∞£, ∫–, √  ¿« ≈ÿΩ∫∆Æ∏¶ ¿–æÓº≠ æÀ∏¬∞‘ ¡∂«’«ÿ¡ÿ¥Ÿ.
				{
					CString strDays, strHours, strMinutes, strSeconds;
					if( time.GetDays() )
					{
						strDays.Format( prj.GetText(TID_PK_LIMIT_DAY), static_cast<int>(time.GetDays()) );
						strHours.Format( prj.GetText( TID_PK_LIMIT_HOUR ), time.GetHours() );
						strMinutes.Format( prj.GetText(TID_PK_LIMIT_MINUTE), time.GetMinutes() );

						str = strDays + strHours + strMinutes;
					}
					else if( time.GetHours() )
					{
						strHours.Format( prj.GetText(TID_PK_LIMIT_HOUR), time.GetHours() );
						strMinutes.Format( prj.GetText(TID_PK_LIMIT_MINUTE), time.GetMinutes() );

						str = strHours + strMinutes;
					}else if( time.GetMinutes() )
					{
						strMinutes.Format( prj.GetText( TID_PK_LIMIT_MINUTE ), time.GetMinutes() );
						strSeconds.Format( prj.GetText( TID_PK_LIMIT_SECOND ), time.GetSeconds() );

						str = strMinutes + strSeconds;
					}
					else
					{
						str.Format( prj.GetText( TID_PK_LIMIT_SECOND ), time.GetSeconds() );
					}
				}
				else if( time.GetDays() )
					str.Format( prj.GetText(TID_PK_LIMIT_DAY), static_cast<int>(time.GetDays()+1) );
				else if( time.GetHours() )
					str.Format( prj.GetText(TID_PK_LIMIT_HOUR), time.GetHours() );
//sun: 11, ±‚∞£¡¶ æ∆¿Ã≈€ ¡ˆº”Ω√∞£ √ ¥‹¿ß «•±‚
				else if( time.GetMinutes() )
					str.Format( prj.GetText(TID_PK_LIMIT_MINUTE), time.GetMinutes() );
				else
					str.Format( prj.GetText(TID_PK_LIMIT_SECOND), time.GetSeconds() );
			}
		}

		strTemp = str + prj.GetText(TID_TOOLTIP_PERIOD);	
		pEdit->AddString( "\n" );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwTime );
	}
}

void CWndMgr::PutEndurance( FLItemElem* pItemElem, CEditString* pEdit, int flag )
{
}

void CWndMgr::PutSex( CMover* pMover, FLItemElem* pItemElem, CEditString* pEdit )
{
	if( pItemElem->GetProp()->dwItemSex != 0xffffffff )
	{	// º∫∫∞
		CString strTemp;
		if( pItemElem->GetProp()->dwItemSex == 0 )	// ≥≤
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_SEXMALE) );
		else	// ø©
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_SEXFEMALE) );
		pEdit->AddString( "\n" );
		if( pItemElem->GetProp()->dwItemSex == pMover->GetSex() )
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral ); 
		else
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwNotUse ); 
	}
}

void CWndMgr::PutJob( CMover* pMover, FLItemElem* pItemElem, CEditString* pEdit )
{
	if( pItemElem->GetProp()->dwItemJob != 0xffffffff )
	{	// ¡˜æ˜
		CString strTemp;
		switch( pItemElem->GetProp()->dwItemJob )
		{
		case JOB_VAGRANT:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_REGVANG) );
			break;
		case JOB_MERCENARY:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_REGMERSER) );
			break;
		case JOB_ACROBAT:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ACRO) );
			break;
		case JOB_ASSIST:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ASSIST) );
			break;
		case JOB_MAGICIAN:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_MAG) );
			break;
		case JOB_PUPPETEER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_PUPPET) );
			break;

		case JOB_KNIGHT:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_KNIGHT) );
			break;
		case JOB_BLADE: 
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_BLADE) );
			break;
		case JOB_JESTER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_JASTER) );
			break;
		case JOB_RANGER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_RANGER) );
			break;
		case JOB_RINGMASTER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_RINGMAS) );
			break;
		case JOB_BILLPOSTER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_BILLPOS) );
			break;
		case JOB_PSYCHIKEEPER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_PSYCHIKEEPER) );
			break;
		case JOB_ELEMENTOR:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ELEMENTOR) );
			break;
		case JOB_GATEKEEPER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_GATE) );
			break;
		case JOB_DOPPLER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_DOPPLER) );
			break;
		case JOB_KNIGHT_MASTER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_KNIGHT_MASTER) );
			break;
		case JOB_BLADE_MASTER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_BLADE_MASTER) );
			break;
		case JOB_JESTER_MASTER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_JESTER_MASTER) );
			break;
		case JOB_RANGER_MASTER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_RANGER_MASTER) );
			break;
		case JOB_RINGMASTER_MASTER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_RINGMASTER_MASTER) );
			break;
		case JOB_BILLPOSTER_MASTER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_BILLPOSTER_MASTER) );
			break;
		case JOB_PSYCHIKEEPER_MASTER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_PSYCHIKEEPER_MASTER) );
			break;
		case JOB_ELEMENTOR_MASTER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ELEMENTOR_MASTER) );
			break;

		case JOB_KNIGHT_HERO:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_KNIGHT_HERO) );
			break;
		case JOB_BLADE_HERO:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_BLADE_HERO) );
			break;
		case JOB_JESTER_HERO:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_JESTER_HERO) );
			break;
		case JOB_RANGER_HERO:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_RANGER_HERO) );
			break;
		case JOB_RINGMASTER_HERO:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_RINGMASTER_HERO) );
			break;
		case JOB_BILLPOSTER_HERO:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_BILLPOSTER_HERO) );
			break;
		case JOB_PSYCHIKEEPER_HERO:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_PSYCHIKEEPER_HERO) );
			break;
		case JOB_ELEMENTOR_HERO:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ELEMENTOR_HERO) );
			break;
		case JOB_LORDTEMPLER_HERO:
			strTemp.Format( prj.GetText( TID_GAME_TOOLTIP_LORDTEMPLER_HERO ) );
			break;
		case JOB_STORMBLADE_HERO:
			strTemp.Format( prj.GetText( TID_GAME_TOOLTIP_STORMBLADE_HERO ) );
			break;
		case JOB_WINDLURKER_HERO:
			strTemp.Format( prj.GetText( TID_GAME_TOOLTIP_WINDLURKER_HERO ) );
			break;
		case JOB_CRACKSHOOTER_HERO:
			strTemp.Format( prj.GetText( TID_GAME_TOOLTIP_CRACKSHOOTER_HERO ) );
			break;
		case JOB_FORCEMASTER_HERO:
			strTemp.Format( prj.GetText( TID_GAME_TOOLTIP_FORCEMASTER_HERO ) );
			break;
		case JOB_FLORIST_HERO:
			strTemp.Format( prj.GetText( TID_GAME_TOOLTIP_FLORIST_HERO ) );
			break;
		case JOB_MENTALIST_HERO:
			strTemp.Format( prj.GetText( TID_GAME_TOOLTIP_MENTALIST_HERO ) );
			break;
		case JOB_ELEMENTORLORD_HERO:
			strTemp.Format( prj.GetText( TID_GAME_TOOLTIP_ELEMENTORLORD_HERO ) );
			break;
		} // switch
		pEdit->AddString( "\n" );
		if( pMover->IsInteriorityJob( pItemElem->GetProp()->dwItemJob ) )
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
		else 
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwNotUse );
	}
}

void CWndMgr::PutLevel( CMover* pMover, FLItemElem* pItemElem, CEditString* pEdit )
{
	DWORD dwParts = pItemElem->GetProp()->dwParts;
	CString strTemp;
	//flyingjin :  ¿Â¬¯∑˘øÕ ¿œπ›æ∆¿Ã≈€¿« « ø‰∑π∫ß «•Ω√π˝ ∫Ø∞Ê 
	// ¿Â¬¯∑˘ :  √÷º“∑π∫ß∏∏ «•Ω√
	// ±◊ø‹ : √÷º“ ~ √÷¥Î ∑π∫ß «•Ω√ 

	int nMaxLevel = pItemElem->GetProp()->nMaxLimitLevel;
	int nMinLevel = pItemElem->GetProp()->nMinLimitLevel;

	if(!(nMinLevel == 1 && nMaxLevel == MAX_CHARACTER_LEVEL))
	{
		if(dwParts == NULL_ID)
		{                     
			if( nMinLevel > 1 || nMaxLevel < MAX_CHARACTER_LEVEL )//√÷∞Ì∑π∫ß∫∏¥Ÿ ¿€¿ª∂ß∏‡ √÷º“ ~ √÷¥Î ∑π∫ß«•Ω√ 
			{
				strTemp.Format( prj.GetText(TID_MMI_TOOLTIP_LEVEL), pItemElem->GetProp()->nMinLimitLevel , pItemElem->GetProp()->nMaxLimitLevel);
				pEdit->AddString( "\n" );
			} 
		}
		else
		{
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_REQLEVEL), pItemElem->GetProp()->nMinLimitLevel );
			pEdit->AddString( "\n" );
		}
	}
	if( pItemElem->CanUseLevel( pMover ) == false )
	{
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwNotUse );
	}
	else
	{
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
	}

	if( pItemElem->GetLevelDown() > 0 )
	{
		strTemp.Format( "(-%d)", pItemElem->GetLevelDown() );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwNotUse );
	}	
	
}

void CWndMgr::PutChangedLook( FLItemElem* pItemElem, CEditString* pEdit )
{
	//ø‹«¸∫Ø∞Ê ¡§∫∏
	DWORD dwIndex = pItemElem->GetLooksChangeItemID( );
	if( 0 != dwIndex )
	{	
		PT_ITEM_SPEC pSec = g_xSpecManager->GetSpecItem( dwIndex );
		if( NULL == pSec )
			return;

		CString strBuffer;
		strBuffer.Format( prj.GetText( TID_MMI_LOOKCHANGE10 ), pSec->szName );
		pEdit->AddString( "\n" );
		pEdit->AddString( strBuffer, 0xffff0000 );
	}
}

void CWndMgr::PutWeapon( FLItemElem* pItemElem, CEditString* pEdit )
{
	if( pItemElem->GetProp()->dwItemKind3 != IK3_SHIELD )
	{
		if( pItemElem->GetProp()->dwHanded != 0xffffffff )
		{ // «—º’π´±‚øÎ / æÁº’π´±‚øÎ
			CString strTemp;
			if( HD_ONE == pItemElem->GetProp()->dwHanded )
			{
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ONEHANDWEAPON) );
			}
			else if( HD_TWO == pItemElem->GetProp()->dwHanded )
			{
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_TWOHANDWEAPON) );
			}
			pEdit->AddString( "\n" );
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral ); 
		}
	}
}

void CWndMgr::PutCommand( FLItemElem* pItemElem, CEditString* pEdit )
{
	CString strTemp;
#ifdef	__ITEM_COUNT_TOOLTIP  
	if (pItemElem->GetProp()->dwPackMax > 1)
	{
		CWndShop * pWndShop = (CWndShop *)g_WndMng.GetWndBase(APP_SHOPEX);
		if (!pWndShop)
		{
			short nItemNum = pItemElem->m_nItemNum;
			strTemp.Format("\nCount: %d", nItemNum);
			pEdit->AddString(strTemp, 0xff00ff00);
		}
	}
#endif //__ITEM_COUNT_TOOLTIP	
	if( strlen( pItemElem->GetProp()->szCommand ) )
	{
		pEdit->AddString( "\n" );

//sun: 8, // __S_COUPLERING01
		if( ( pItemElem->m_dwItemId == ITEM_INDEX( 3600, II_GEN_WARP_COUPLERING ) || pItemElem->m_dwItemId == ITEM_INDEX( 18186, II_GEN_WARP_WEDDING_BAND ) || pItemElem->m_dwItemId == ITEM_INDEX( 3601,II_GEN_WARP_COUPLERING01 ) )
			&& pItemElem->GetCoupleId() > 0 )
		{
			u_long idPlayer	= static_cast<u_long>( pItemElem->GetCoupleId() );

			//sun: 11, ƒ≥∏Ø≈Õ ¡§∫∏ ≈Î«’
			const char* pszPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( idPlayer );

			CString strDesc;
			strDesc.Format( prj.GetText( TID_ITEM_COUPLERING_DESC ), pszPlayer? pszPlayer: "" );
			strTemp.Format( prj.GetText( TID_ITEM_INFO ), strDesc );	// º≥∏Ì :
		}
		else
		{
			strTemp.Format( prj.GetText( TID_ITEM_INFO ), pItemElem->GetProp()->szCommand );	// º≥∏Ì :
		}
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwCommand );
	}
}



//---------------------------------------------------------------------------------------------
// æ∆¿Ã≈€ ≈¯∆¡¿ª √‚∑¬«—¥Ÿ.
// param	: æ∆¿Ã≈€, ¿ßƒ°, øµø™, «√∑π±◊
// return	: void
//---------------------------------------------------------------------------------------------
void CWndMgr::PutToolTip_Item( FLItemBase* pItemBase, CPoint point, CRect* pRect, int flag )
{
	if( NULL == pRect )
		return ;

	if( NULL == pItemBase )
		return ;

	PT_ITEM_SPEC pItemProp( pItemBase->GetProp() );
	if( NULL == pItemProp )
		return;

	CEditString strItem( _T("") );

	MakeToolTipText( pItemBase, strItem, flag );

#if defined( __ZCK_TELEPORTEUR ) && defined( __ZCK_VISU_MODELS )
	if( flag == APP_ZCK_TELEPORTEUR && CWndModelView::CanPreview( pItemProp ) )
		strItem.AddString( _T( "\nSHIFT + Double Click to view model" ), 0xFF000000 );
#endif // __ZCK_TELEPORTEUR && __ZCK_VISU_MODELS

	g_toolTip.PutToolTip( pItemBase->m_dwItemId, strItem, *pRect, point, flag );

	if( pItemProp->IsUltimate() 
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
		|| pItemProp->IsCostumeEnhanceParts()
#endif
#ifdef BARUNA_UPGRADE_ENHANCEMENT_GEM
		|| pItemProp->IsBarunaWeapon()
#endif
		)
		g_toolTip.SetUltimateToolTip( pItemBase );

#ifdef __ENCHANT_BARUNA16
	if( pItemProp->IsBaruna() )
		g_toolTip.SetVal_m_nAdded_( 2 );
#endif //__ENCHANT_BARUNA16

#ifdef BARUNA_ULTIMATE_UPDATE
	if( pItemProp->IsBarunaUltimate() )
		g_toolTip.SetVal_m_nAdded_( 3 );
#endif
}

void CWndMgr::PutToolTipItemAndSubToolTip( FLItemBase* pItemBase, CPoint point, CRect* pRect, int nFlag )
{
	// ¥Ÿ¡ﬂ ≈¯∆¡ Ω√Ω∫≈€¿Ã ¿˚øÎµ«∏Èº≠ ≈¯∆¡ √‚∑¬ ¿¸ø° ≈¯∆¡ Ω∫¿ßƒ°∏¶ πÃ∏Æ ºº∆√«ÿº≠ ¿ßƒ°∏¶ ¡∂¡§
	g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_MAIN );
	PutToolTipParts( pItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_MAIN );
	PT_ITEM_SPEC pItemProp = pItemBase->GetProp();
	switch( pItemProp->dwItemKind2 )
	{
	case IK2_WEAPON_DIRECT:
	case IK2_WEAPON_MAGIC:
	case IK2_ARMOR:
	case IK2_ARMORETC:
	case IK2_CLOTH:
	case IK2_CLOTHETC:
	case IK2_JEWELRY:
	case IK2_RIDING:
	case IK2_CLOTHWIG:
		{
			FLItemBase* pEquipItemBase = g_pPlayer->GetEquipItem( pItemProp->dwParts );
			if( pEquipItemBase )
			{
				g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB1 );
				PutToolTipParts( pEquipItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB1 );
			}
			if( pItemProp->dwParts == PARTS_RWEAPON )
			{
				FLItemBase* pRWeaponItemBase = g_pPlayer->GetEquipItem( PARTS_RWEAPON );
				FLItemBase* pLWeaponItemBase = g_pPlayer->GetEquipItem( PARTS_LWEAPON );
				if( pRWeaponItemBase && pLWeaponItemBase )
				{
					g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB2 );
					PutToolTipParts( pLWeaponItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB2 );
				}
				else if( pRWeaponItemBase )
				{
					g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB1 );
					PutToolTipParts( pRWeaponItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB1 );
				}
				else if( pLWeaponItemBase )
				{
					g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB1 );
					PutToolTipParts( pLWeaponItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB1 );
				}
			}
			else if( pItemProp->dwParts == PARTS_RING1 )
			{
				FLItemBase* pRing1ItemBase = g_pPlayer->GetEquipItem( PARTS_RING1 );
				FLItemBase* pRing2ItemBase = g_pPlayer->GetEquipItem( PARTS_RING2 );
				if( pRing1ItemBase && pRing2ItemBase )
				{
					g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB2 );
					PutToolTipParts( pRing2ItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB2 );
				}
				else if( pRing1ItemBase )
				{
					g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB1 );
					PutToolTipParts( pRing1ItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB1 );
				}
				else if( pRing2ItemBase )
				{
					g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB1 );
					PutToolTipParts( pRing2ItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB1 );
				}
			}
			else if( pItemProp->dwParts == PARTS_EARRING1 )
			{
				FLItemBase* pEarring1ItemBase = g_pPlayer->GetEquipItem( PARTS_EARRING1 );
				FLItemBase* pEarring2ItemBase = g_pPlayer->GetEquipItem( PARTS_EARRING2 );
				if( pEarring1ItemBase && pEarring2ItemBase )
				{
					g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB2 );
					PutToolTipParts( pEarring2ItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB2 );
				}
				else if( pEarring1ItemBase )
				{
					g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB1 );
					PutToolTipParts( pEarring1ItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB1 );
				}
				else if( pEarring2ItemBase )
				{
					g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB1 );
					PutToolTipParts( pEarring2ItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB1 );
				}
			}
			break;
		}
	}
}

void CWndMgr::PutToolTipParts( FLItemBase* pPartsItemBase, CPoint point, CRect* pRect, const int nFlag, const int nSubToolTipFlag )
{
	if( pPartsItemBase == NULL )
		return;

	PT_ITEM_SPEC pPartsItemProp = pPartsItemBase->GetProp();
	CEditString strEquipItem = _T( "" );
	if( nSubToolTipFlag != 0 )
		PutEquipItemText( &strEquipItem );
	MakeToolTipText( pPartsItemBase, strEquipItem, nFlag );
	switch( nSubToolTipFlag )
	{
	case 0:
		{
			g_toolTip.PutToolTipEx( pPartsItemBase->m_dwItemId, strEquipItem, *pRect, point, 0, nSubToolTipFlag );
			if( pPartsItemProp->IsUltimate() 
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
		|| pPartsItemProp->IsCostumeEnhanceParts()
#endif
#ifdef BARUNA_UPGRADE_ENHANCEMENT_GEM
		|| pPartsItemProp->IsBarunaWeapon()
#endif
				)
				g_toolTip.SetUltimateToolTip( pPartsItemBase );

#ifdef __ENCHANT_BARUNA16
			if( pPartsItemProp->IsBaruna() )
				g_toolTip.SetVal_m_nAdded_( 2 );
#endif //__ENCHANT_BARUNA16

#ifdef BARUNA_ULTIMATE_UPDATE
			if( pPartsItemProp->IsBarunaUltimate() )
				g_toolTip.SetVal_m_nAdded_( 3 );
#endif

			break;
		}
	case 1:
		{
			g_toolTipSub1.PutToolTipEx( pPartsItemBase->m_dwItemId, strEquipItem, *pRect, point, 0, nSubToolTipFlag );
			if( pPartsItemProp->IsUltimate() 
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
		|| pPartsItemProp->IsCostumeEnhanceParts()
#endif
#ifdef BARUNA_UPGRADE_ENHANCEMENT_GEM
		|| pPartsItemProp->IsBarunaWeapon()
#endif
				)
				g_toolTipSub1.SetUltimateToolTip( pPartsItemBase );

#ifdef __ENCHANT_BARUNA16
			if( pPartsItemProp->IsBaruna() )
				g_toolTipSub1.SetVal_m_nAdded_( 2 );
#endif //__ENCHANT_BARUNA16

#ifdef BARUNA_ULTIMATE_UPDATE
			if( pPartsItemProp->IsBarunaUltimate() )
				g_toolTipSub1.SetVal_m_nAdded_( 3 );
#endif
			break;
		}
	case 2:
		{
			g_toolTipSub2.PutToolTipEx( pPartsItemBase->m_dwItemId, strEquipItem, *pRect, point, 0, nSubToolTipFlag );
			if( pPartsItemProp->IsUltimate() 
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
		|| pPartsItemProp->IsCostumeEnhanceParts()
#endif
#ifdef BARUNA_UPGRADE_ENHANCEMENT_GEM
		|| pPartsItemProp->IsBarunaWeapon()
#endif
				)
				g_toolTipSub2.SetUltimateToolTip( pPartsItemBase );

#ifdef __ENCHANT_BARUNA16
			if( pPartsItemProp->IsBaruna() )
				g_toolTipSub2.SetVal_m_nAdded_( 2 );
#endif //__ENCHANT_BARUNA16

#ifdef BARUNA_ULTIMATE_UPDATE
			if( pPartsItemProp->IsBarunaUltimate() )
				g_toolTipSub2.SetVal_m_nAdded_( 3 );
#endif
			break;
		}
	}
}

void CWndMgr::PutItemCharge( FLItemElem* pItemElem, CEditString* pEdit )
{
	//_JIN_ITEM_CHARGE
	PT_ITEM_SPEC pItemProp = pItemElem->GetProp();

	// Title 
	// color = black
	// prop  = red

	//=====================================
	// ¡¶∑√
	//=====================================
	switch(pItemProp->dwGeneralEnchantMode)
	{
	case 1:
		break;
	case 0:
		pEdit->AddString( "\n" );
		pEdit->AddString( prj.GetText( TID_MMI_SMELTING03_TEXT ),dwItemColor[g_Option.m_nToolTipText].dwGeneral );
		pEdit->AddString( " : " );
		pEdit->AddString(  prj.GetText( TID_MMI_SMELTING02_TEXT ),dwItemColor[g_Option.m_nToolTipText].dwEffective2 );
		break;
	case NULL_ID:
		break;
	}

	//=====================================
	// º”º∫ ¡¶∑√
	//=====================================
	switch(pItemProp->dwAttributeEnchantMode)
	{
	case 1:
		break;
	case 0:
		pEdit->AddString( "\n" );
		pEdit->AddString( prj.GetText( TID_MMI_SMELTING04_TEXT ),dwItemColor[g_Option.m_nToolTipText].dwGeneral );
		pEdit->AddString( " : " );
		pEdit->AddString( prj.GetText( TID_MMI_SMELTING02_TEXT ),dwItemColor[g_Option.m_nToolTipText].dwEffective2 );
		break;
	case NULL_ID:
		break;
	}

	//=====================================
	// ∫∏ºÆ ¡¶∑√
	//=====================================
	switch(pItemProp->dwGemEnchantMode)
	{
	case 1:
		break;
	case 0:
		pEdit->AddString( "\n" );
		pEdit->AddString( prj.GetText( TID_MMI_SMELTING05_TEXT ),dwItemColor[g_Option.m_nToolTipText].dwGeneral );
		pEdit->AddString( " : " );
		pEdit->AddString( prj.GetText( TID_MMI_SMELTING02_TEXT ),dwItemColor[g_Option.m_nToolTipText].dwEffective2 );
		break;
	case NULL_ID:
		break;
	}
	

	//=====================================
	// ««æÓΩÃ(πŸ∑Á≥™ ««æÓΩÃ ∆˜«‘)
	//=====================================
	switch(pItemProp->dwPiercingMode)
	{
	case 1:
		break;
	case 0:
		pEdit->AddString( "\n" );
		pEdit->AddString( prj.GetText( TID_MMI_SMELTING06_TEXT ),dwItemColor[g_Option.m_nToolTipText].dwGeneral );
		pEdit->AddString( " : " );
		pEdit->AddString( prj.GetText( TID_MMI_SMELTING02_TEXT ),dwItemColor[g_Option.m_nToolTipText].dwEffective2 );
		break;
	case NULL_ID:
		break;
	}

	//=====================================
	// ∞¢º∫
	//=====================================
	switch(pItemProp->dwRandomOptionMode)
	{
	case 1:
		break;
	case 0:
		pEdit->AddString( "\n" );
		pEdit->AddString( prj.GetText( TID_MMI_SMELTING07_TEXT ),dwItemColor[g_Option.m_nToolTipText].dwGeneral );
		pEdit->AddString( " : " );
		pEdit->AddString( prj.GetText( TID_MMI_SMELTING02_TEXT ),dwItemColor[g_Option.m_nToolTipText].dwEffective2 );
		break;
	case NULL_ID:
		break;
	}

	return;
}

void CWndMgr::PutBarunaPearcing( FLItemElem* pItemElem, CEditString* pEdit )
{
	DWORD dwIndex = 0;
	
	for( size_t nSize = 0; nSize < pItemElem->GetGeneralPiercingSize(); nSize++ )
	{
		if( pItemElem->GetGeneralPiercingItemID( nSize ) != 0 )
		{
			dwIndex = pItemElem->GetGeneralPiercingItemID( nSize );
		}
		else
		{
			dwIndex = 0;		
		}
	}

	if(dwIndex == 0) return;

	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem(dwIndex);
	
	if(pItemProp == NULL)return;
	DWORD ddwID = pItemProp->dwHitActiveSkillId;
 	SkillProp* pSkillProp = prj.m_aPropSkill.GetAt( ddwID );
	CString szInfo;
	if(pSkillProp)
	{
		szInfo.Format( "\n%s", pSkillProp->szCommand );
		pEdit->AddString( szInfo ,dwItemColor[g_Option.m_nToolTipText].dwBarunaRunePiercing );	
	}
}

void CWndMgr::PutTHCombinedOption( FLItemElem* pItemElem, CEditString* pEdit )
{
	PT_ITEM_SPEC pItemProp = pItemElem->GetProp();
	if( NULL == pItemProp )
		return;

	CString strTemp;

	LPCTSTR szTitle = prj.GetText( TID_MMI_TWOHANDEDWEAPONMERGE_RESULTTEXT01 );
	strTemp.Format( "\n %s", szTitle );
	pEdit->AddString( strTemp.GetString() , 0xffff0000 );		//"[æÁº’ π´±‚ «’º∫ ø…º«]"

	//1. ∞¯∞›∑¬
	int nAddDmg = (int)pItemElem->GetCombinedAddDamage();
	if( nAddDmg > 0 )
	{
		pEdit->AddString( "\n" );
		strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ATTACKRANGE2) );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
		strTemp.Format( " %d", nAddDmg );
		pEdit->AddString( strTemp.GetString(), 0xffff0000 );
	}

	//2. ««æÓΩÃ
	PutPiercingOpt( pItemElem, pEdit, 1 );

	//3. ∑£¥˝ø…º«
	PutRandomOpt( pItemElem, pEdit, 1 );

	//4. ∞¢º∫
	for( size_t Nth = 0; Nth < pItemElem->GetCombinedRandomOptionExtensionSize(); ++Nth )
	{
		WORD wDstID = pItemElem->GetCombinedRandomOptionExtensionDstID( Nth );
		short shAdjValue = pItemElem->GetCombinedRandomOptionExtensionAdjValue( Nth );
		
		if( IsDst_Rate( wDstID ) == TRUE )
		{
			if( wDstID == DST_ATTACKSPEED )
			{
				strTemp.Format( "\n%s %c%d%% ", FindDstString( wDstID ), ( shAdjValue > 0 ? '+': '-' ), ::abs( shAdjValue / 2 / 10 ) );
			}
			else
			{
				strTemp.Format( "\n%s %c%d%%", FindDstString( wDstID ), ( shAdjValue > 0 ? '+': '-' ), ::abs( shAdjValue ) );
			}
		}
		else
		{
			strTemp.Format( "\n%s %c%d", FindDstString( wDstID ), ( shAdjValue > 0 ? '+': '-' ), ::abs( shAdjValue ) );
		}

		if( pItemProp->IsWeaponParts() == TRUE || pItemProp->IsArmorParts() == TRUE )
		{
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwAwakening );
		}
		else
		{
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwBlessing );
		}
	}
}


//---------------------------------------------------------------------------------------------
// ≈¯∆¡ø° ±Õº” º”º∫ √‚∑¬«œ±‚
// param	: æ∆¿Ã≈€, πÆ¿⁄ø≠
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndMgr::PutBind( FLItemElem* pItemElem, CEditString* pEdit )
{
	FLERROR_ASSERT_LOG_RETURN_VOID( pEdit, _T("[¿©µµøÏ ∏ﬁ¥œ¡Æ] ∆Ì¡˝ πÆ¿⁄ø≠ ∆˜¿Œ≈Õ∞° ¿Ø»ø«œ¡ˆ æ Ω¿¥œ¥Ÿ.") );

	PT_ITEM_SPEC pItemProp = pItemElem->GetProp();
	FLERROR_ASSERT_LOG_RETURN_VOID( pItemProp, _T("[¿©µµøÏ ∏ﬁ¥œ¡Æ] æ∆¿Ã≈€ º”º∫ ∆˜¿Œ≈Õ∞° ¿Ø»ø«œ¡ˆ æ Ω¿¥œ¥Ÿ.") );

	if( pItemElem->IsBindState() )
	{
		pEdit->AddString( _T("\n") );
		pEdit->AddString( GETTEXT( TID_TOOLTIP_EQUIPBIND_AFTER ), GETTEXTCOLOR( TID_TOOLTIP_EQUIPBIND_AFTER ) );
	}
	else
	{
		if( pItemElem->IsUseBindState() )
		{
			pEdit->AddString( _T("\n") );
			pEdit->AddString( GETTEXT( TID_TOOLTIP_EQUIPBIND_BEFORE ), GETTEXTCOLOR( TID_TOOLTIP_EQUIPBIND_BEFORE ) );
		}
	}
}

//---------------------------------------------------------------------------------------------
// ≈¯∆¡ πÆ¿⁄ø≠¿ª ª˝º∫«—¥Ÿ.
// param	: æ∆¿Ã≈€, πÆ¿⁄ø≠, «√∑π±◊
// return	: void
//---------------------------------------------------------------------------------------------
void CWndMgr::MakeToolTipText( FLItemBase* pItemBase, CEditString& strEdit, int flag )
{
	if( pItemBase == NULL )
		return;

	PT_ITEM_SPEC pItemProp( pItemBase->GetProp() );
	FLERROR_ASSERT_LOG_RETURN_VOID( pItemProp, _T("[¿©µµøÏ ∏ﬁ¥œ¡Æ] æ∆¿Ã≈€ º”º∫¿ª √£¿ª ºˆ æ¯Ω¿¥œ¥Ÿ. ID - [%u]"), pItemBase->m_dwItemId );

	if( NULL == g_pPlayer )
		return;
	
	CMover* pMover( g_pPlayer );

	if( flag == APP_QUERYEQUIP )
	{
		CWndQueryEquip * pWndQueryEquip = (CWndQueryEquip *)g_WndMng.GetWndBase( APP_QUERYEQUIP );
		if( pWndQueryEquip )
			pMover = pWndQueryEquip->GetMover();
	}

	if( NULL == pMover )
		return;

	FLItemElem*		pItemElem( (FLItemElem*)pItemBase );

	CString			str( _T( "" ) );
	CString			strTemp( _T( "" ) );
	CString			strEnter( _T('\n') );

	DWORD			dwColorBuf( PutItemName( pItemElem, &strEdit ) );

	PutItemAbilityPiercing( pItemElem, &strEdit, dwColorBuf, flag );
	PutPetKind( pItemElem, &strEdit );		//gmpbigsun : æ∆¿Ã≈€ ∏Ì ¥Ÿ¿Ω¡Ÿø° ∆Í ¡æ∑˘ ( ∏ÆæÓ, «»æ˜, πˆ«¡ ) ª¿‘ 
	PutBind( pItemElem, &strEdit );			// ±Õº” ø©∫Œ √‚∑¬ 
	PutWeapon( pItemElem, &strEdit );
	PutSex( pMover, pItemElem, &strEdit );
	
	switch( pItemProp->dwItemKind2 )
	{
		case IK2_WEAPON_DIRECT:
		case IK2_WEAPON_MAGIC:
		case IK2_ARMORETC:
		case IK2_CLOTHETC:
		case IK2_ARMOR:
		case IK2_CLOTH:
		case IK2_BLINKWING:
		{
			
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
			PutItemMinMax( pMover, pItemElem, &strEdit, pItemProp->IsCostumeParts() );
#else
			PutItemMinMax( pMover, pItemElem, &strEdit );
#endif
			PutItemSpeed( pItemElem, &strEdit );
			if( pItemProp->dwItemKind3 == IK3_ELECARD )
				PutItemResist( pItemElem, &strEdit );
			else if( pItemElem->m_nResistAbilityOption && ( pItemProp->dwItemKind1 == IK1_WEAPON  || pItemProp->dwItemKind1 == IK1_ARMOR ) )
				PutItemResist( pItemElem, &strEdit );

			PutBaseResist( pItemElem, &strEdit );	// º”º∫ ¿˙«◊∑¬
			
			PutBaseItemOpt( pItemElem, &strEdit );

			PutRandomOpt( pItemElem, &strEdit );			
			PutEnchantOpt( pMover, pItemElem, &strEdit, flag );
			break;
		}
	case IK2_REFRESHER:
	case IK2_FOOD:
	case IK2_POTION:
		{
			// TODO : property∏¶ ¥¯¿˙¡÷∞Ì ≥ª∫Œø°º≠ √≥∏Æ¥¬?
			PutMedicine( pItemElem, pItemElem->GetProp()->dwDestParam[0],  pItemProp->nAdjParamVal[0], &strEdit );
			PutMedicine( pItemElem, pItemElem->GetProp()->dwDestParam[1],  pItemProp->nAdjParamVal[1], &strEdit );
			break;
		}
	case IK2_JEWELRY:
		{
			PutBaseItemOpt( pItemElem, &strEdit );
			break;
		}
//sun, 11, ƒ…∏Ø≈Õ ∫¿¿Œ ∞≈∑° ±‚¥… world,database,neuz
	case IK2_SYSTEM:
		{
			if( pItemProp->dwItemKind3 == IK3_VIS )
			{
				PutNeededVis( pItemElem, &strEdit );
				PutBaseItemOpt( pItemElem, &strEdit );
			}
			
			if( pItemElem->m_dwItemId == ITEM_INDEX( 26476,II_SYS_SYS_SCR_SEALCHARACTER ) )
			{
				PutSealChar( pItemElem, &strEdit );
			}
			break;
		}

	case IK2_GMTEXT:
		{
			if( pItemProp->dwItemKind3 == IK3_TEXT_DISGUISE )
			{
				PutDestParam( pItemProp->dwDestParam[0], pItemProp->dwDestParam[1],
					pItemProp->nAdjParamVal[0], pItemProp->nAdjParamVal[1], strEdit );

				//sun: ∆÷, ∫ØΩ≈ æ∆¿Ã≈€ ≈‰±€πˆ«¡ ¿˚øÎ... Neuz, World
				if( pItemProp->dwActiveSkill != NULL_ID )
				{
					SkillProp *pSkillProp;
					AddSkillProp* pAddSkillProp;
					pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, pItemProp->dwActiveSkill, 1, "CWndMgr::PutToolTip_Item" );
					
					if( pAddSkillProp )
					{
						PutDestParam( pAddSkillProp->dwDestParam[0], pAddSkillProp->dwDestParam[1],
							pAddSkillProp->dwAdjParamVal[0], pAddSkillProp->dwAdjParamVal[1], strEdit );
					}
				}
				
			}
			break;
		}
	case IK2_ELLDINPOTION:	// ø§µÚ¿« «◊æ∆∏Æ
		{
			if( pItemProp->dwItemKind3 == IK3_SAVEPOTION )
				PutHitPoint( pItemElem, &strEdit );

			break;
		}
	default:
		{
			break;
		}
	}	// switch( pItemProp->dwItemKind2 )

	//sun: ∆÷, ∫ØΩ≈ æ∆¿Ã≈€ ≈‰±€πˆ«¡ ¿˚øÎ... Neuz, World
	if( pItemElem->IsEatPet() )
	{
		if( pItemProp->dwActiveSkill != NULL_ID )
		{
			SkillProp *pSkillProp;
			AddSkillProp* pAddSkillProp;
			pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, pItemProp->dwActiveSkill, 1, "CWndMgr::PutToolTip_Item" );
			
			if( pAddSkillProp )
			{
				PutDestParam( pAddSkillProp->dwDestParam[0], pAddSkillProp->dwDestParam[1],
					pAddSkillProp->dwAdjParamVal[0], pAddSkillProp->dwAdjParamVal[1], strEdit );
			}
		}
	}

	if( pItemElem->IsVisPet( ) )
	{
		PutVisPetInfo( pItemElem, &strEdit );
	}

	PutCoolTime( pMover, pItemElem, &strEdit );			// ƒ≈∏¿”
	PutEndurance( pItemElem, &strEdit, flag );			// ≥ª±∏∑¬
	PutKeepTime( pItemElem, &strEdit );					// ªÁøÎ«“ºˆ ¿÷¥¬ Ω√∞£
	PutJob( pMover, pItemElem, &strEdit );
	PutLevel( pMover, pItemElem, &strEdit );	
	PutChangedLook( pItemElem, &strEdit );				//ø‹«¸∫Ø∞Ê
	PutCommand( pItemElem, &strEdit );					// øÎµµ 
	PutItemGold( pMover, pItemElem, &strEdit, flag );	// ∞°∞›
	PutSetItemOpt( pMover, pItemElem, &strEdit );
//sun: 9¬˜ ¿¸Ω¬∞¸∑√ Clienet
	if( pItemProp->dwItemKind3 == IK3_EGG && pItemElem->m_pPet )//&& pItemElem->m_pPet->GetLevel() != PL_EGG )
		PutPetInfo( pItemElem, &strEdit );
	if( pItemProp->dwID == ITEM_INDEX( 21035, II_SYS_SYS_SCR_PET_FEED_POCKET ) ) //∏‘¿Ã ¡÷∏”¥œ ≈¯∆¡
		PutPetFeedPocket( pItemElem, &strEdit );

	PutPiercingOpt( pItemElem, &strEdit );				//sun: 12, π´±‚ ««æÓΩÃ



//sun: 12, π´±‚ ««æÓΩÃ
	if( (pItemProp->dwItemKind2 == IK2_WEAPON_DIRECT || pItemProp->dwItemKind2 == IK2_WEAPON_MAGIC) && 
		(pItemElem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE 
#ifdef BARUNA_UPGRADE_ENHANCEMENT_GEM
		|| pItemElem->GetProp()->IsBarunaWeapon()
#endif
		))
		PutAddedOpt( pItemElem, &strEdit );
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
	else
		PutGemOpt( pItemElem, &strEdit );
#endif	
	//_JIN_ITEM_CHARGE
	switch(_GetContentState(CT_ITEM_CHARGE))	
	{
	case CS_VER1:
		PutItemCharge(pItemElem, &strEdit);
		break;
	default:
		break;
	}

	if(pItemProp->IsBaruna())
	{
		PutBarunaPearcing(pItemElem, &strEdit);
	}

	PutAwakeningBlessing( pItemElem, &strEdit );		//sun: 11, ∞¢º∫, √‡∫π
	// ¿ßƒ° ∫Ø∞Ê 16.01.27

	//æÁº’π´±‚ ø…º«
	if( pItemElem->IsSetCombinedOption( ) )
	{
		PutTHCombinedOption(pItemElem, &strEdit );
	}
#ifdef __ZCK_PING_FPS_AND_TOOLTIP
	PutZckItemTechnicalInfo( pItemBase, &strEdit, flag );
#endif // __ZCK_PING_FPS_AND_TOOLTIP
}

void CWndMgr::PutToolTip_Character( int SelectCharacter, CPoint point, CRect* pRect )
{
	CString string;
	char statebuf[30] = {0,};
	// ∫Ì∑œ ªÛ≈¬( ∫Ì∑œªÛ≈¬∏È ª°∞£ªˆ )
	if( g_Neuz.m_nCharacterBlock[SelectCharacter] )
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
			string.Format( "#b#cff0000ff%s#nc#nb", g_Neuz.m_apPlayer[SelectCharacter]->GetName() );
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
			string.Format( "#b#cff27c4cc%s#nc#nb", g_Neuz.m_apPlayer[SelectCharacter]->GetName() );
	}
	else
		string.Format( "#b#cffff0000%s\nConnet Cut#nc#nb", g_Neuz.m_apPlayer[SelectCharacter]->GetName() );
	// º∫∫∞
	if(	g_Neuz.m_apPlayer[SelectCharacter]->GetSex() )
		FLSPrintf( statebuf, _countof( statebuf ), prj.GetText(TID_GAME_TOOLTIP_SEXFEMALE) );
	else
		FLSPrintf( statebuf, _countof( statebuf ), prj.GetText(TID_GAME_TOOLTIP_SEXMALE) );
	string += '\n'; string += statebuf;

	// ∑π∫ß
//sun: 10¬˜ ¿¸Ω¬Ω√Ω∫≈€	Neuz, World, Trans
	if(g_Neuz.m_apPlayer[SelectCharacter]->IsMaster())
	{
		FLSPrintf( statebuf, _countof( statebuf ), prj.GetText(TID_GAME_TOOL_EX_LEVEL), g_Neuz.m_apPlayer[SelectCharacter]->m_nLevel );
		string += '\n'; string += statebuf;
		string += prj.GetText( TID_GAME_TOOLTIP_MARK_MASTER );		
	}
	else if(g_Neuz.m_apPlayer[SelectCharacter]->IsHero())
	{
		FLSPrintf( statebuf, _countof( statebuf ), prj.GetText(TID_GAME_TOOL_EX_LEVEL), g_Neuz.m_apPlayer[SelectCharacter]->m_nLevel );
		string += '\n'; string += statebuf;
		string += prj.GetText( TID_GAME_TOOLTIP_MARK_HERO );
	}
	else if( g_Neuz.m_apPlayer[SelectCharacter]->IsLegendClass( ) )
	{
		FLSPrintf( statebuf, _countof( statebuf ), prj.GetText(TID_GAME_TOOL_EX_LEVEL), g_Neuz.m_apPlayer[SelectCharacter]->m_nLevel );
		string += '\n'; string += statebuf;
		string += prj.GetText( TID_GAME_TOOLTIP_MARK_HERO );
	}
	else
	{
		FLSPrintf( statebuf, _countof( statebuf ), prj.GetText(TID_GAME_TOOL_EX_LEVEL), g_Neuz.m_apPlayer[SelectCharacter]->m_nLevel);
		string += '\n'; string += statebuf;
	}

	// Str
	FLSPrintf( statebuf, _countof( statebuf ), prj.GetText(TID_GAME_TOOL_EX_STR), g_Neuz.m_apPlayer[SelectCharacter]->m_Stat.GetStr() );
	string += '\n'; string += statebuf;
	
	// Sta 
	FLSPrintf( statebuf, _countof( statebuf ), prj.GetText(TID_GAME_TOOL_EX_STA), g_Neuz.m_apPlayer[SelectCharacter]->m_Stat.GetSta() );
	string += '\n'; string += statebuf;
	
	// Dex
	FLSPrintf( statebuf, _countof( statebuf ), prj.GetText(TID_GAME_TOOL_EX_DEX), g_Neuz.m_apPlayer[SelectCharacter]->m_Stat.GetDex() );
	string += '\n'; string += statebuf;
	
	// Int
	FLSPrintf( statebuf, _countof( statebuf ), prj.GetText(TID_GAME_TOOL_EX_INT), g_Neuz.m_apPlayer[SelectCharacter]->m_Stat.GetInt() );
	string += '\n'; string += statebuf;
				
	// ¡˜æ˜
	FLSPrintf( statebuf, _countof( statebuf ), "%s : %s", prj.GetText(TID_APP_CHARACTER_JOB), prj.m_aJob[ g_Neuz.m_apPlayer[SelectCharacter]->m_nJob ].szName );
	string += '\n'; string += statebuf;

	CEditString strEdit;
	strEdit.SetParsingString( string );
	g_toolTip.PutToolTip( SelectCharacter, strEdit, *pRect, point, 3 );
}

// ºÙƒ∆¿∏∑Œ Ω∫≈≥ Ω««‡«œ±‚.
void CWndMgr::UseSkillShortCut( DWORD dwType, DWORD dwSkillIdx )
{
	LPSKILL lpSkill = g_pPlayer->GetSkill( dwType, dwSkillIdx );
	if( lpSkill )
	{
		CCtrl* pCtrl = (CCtrl*)g_WorldMng.Get()->GetObjFocus();
		DWORD dwObjId = NULL_ID;
		if( pCtrl && pCtrl->GetType() != OT_OBJ )
			dwObjId = pCtrl->GetId();

		{
			// Ω∫≈≥πŸ ªÁøÎ¡ﬂ¿œ∂ß¥¬ «÷≈∞∑Œ Ω∫≈≥ªÁøÎ æ»µ . / ≈∏∞Ÿ¿Ã π´πˆø©æﬂ∏∏«‘.
			if( ((CWndTaskBar *)g_WndMng.m_pWndTaskBar)->m_nExecute == 0 && (pCtrl == NULL || (pCtrl && pCtrl->GetType() == OT_MOVER)) )	
			{
//				if( ((CWndTaskBar *)g_WndMng.m_pWndTaskBar)->m_nExecute )	// ¥‹√‡≈∞∑Œ Ω∫≈≥¿ª Ω««‡«ﬂ¿ª∂ß æ◊º«ΩΩ∑‘¿∏∑Œ Ω««‡ªÛ≈¬ø¥¥Ÿ∏È ±◊∞Õ¿ª √Îº“Ω√≈¥.
//					((CWndTaskBar *)g_WndMng.m_pWndTaskBar)->OnCancelSkill();
				int nResult = 0;
				
				if( g_pPlayer->IsBullet( lpSkill->GetProp() ) == FALSE )
				{
					return;
				}

				nResult = g_pPlayer->CMD_SetUseSkill( dwObjId, dwSkillIdx );

				if( nResult == 0 )
				{
					g_WndMng.m_pWndWorld->SetNextSkill( NEXTSKILL_NONE );
				}

				if( nResult )		// Ω««‡«“ ∏Ì∑…¿ª º¬∆√. ¿Ãµø + Ω∫≈≥ªÁøÎ¿Ã «’√ƒ¡¯ ∏Ì∑….
				{
					g_WndMng.m_pWndWorld->SetNextSkill( NEXTSKILL_NONE );
				} else
				// ∞¯∞›¡ﬂ¿Ã∂Û Ω∫≈≥ ªÁøÎ¿Ã æ√«˚¿∏∏È
				if( g_pPlayer->m_pActMover->IsActAttack() )
				{
					// ¥Ÿ¿Ω ≈œ∂ß ¿Ã Ω∫≈≥¿ª øπæ‡Ω√ƒ—µ“.
					g_WndMng.m_pWndWorld->SetNextSkill( dwSkillIdx );
				}				
			}
				
		}
	}
}

BOOL CWndMgr::ScreenCapture()
{
	static long	bmpcnt = 0;
	char	filename[ _MAX_PATH ] = "";
	FILE	*fp;

//sun: 11, JPG∆˜∏À¿∏∑Œ ƒ∏√ƒ
	if( g_xFlyffConfig->GetMainLanguage() == LANG_KOR )
	{
		{
			CreateDirectory( "Capture", NULL );
			while(1)
			{
				FLSPrintf( filename, _countof( filename ), "Capture\\flyff%05d.bmp", bmpcnt );
				if( (fp = FLStdFileOpen(filename,"r")) == NULL ) break; else fclose(fp);
				bmpcnt ++;
			}
		}
		FLSPrintf( filename, _countof( filename ), "Capture\\flyff%05d.bmp", bmpcnt );

		SaveBitmap( filename );
		CString string;
		string.Format( prj.GetText(TID_ADMIN_SCREENSHOTSAVE), bmpcnt++ );
		PutString( string, NULL, prj.GetTextColor(TID_ADMIN_SCREENSHOTSAVE) );
	}
	else
	{
		{
			CreateDirectory( "Capture", NULL );
			while(1)
			{
				FLSPrintf( filename, _countof( filename ), "Capture\\flyff%05d.jpg", bmpcnt );
				if( (fp = FLStdFileOpen(filename,"r")) == NULL ) break; else fclose(fp);
				bmpcnt ++;
			}
		}
		FLSPrintf( filename, _countof( filename ), "Capture\\flyff%05d.jpg", bmpcnt );
		SaveJPG( filename );
		CString string;
		string.Format( prj.GetText(TID_ADMIN_CAPTUREJPG), bmpcnt++ );
		PutString( string, NULL, prj.GetTextColor(TID_ADMIN_CAPTUREJPG) );
	}
				
	return TRUE;
}

//sun: 11, JPG∆˜∏À¿∏∑Œ ƒ∏√ƒ
BOOL CWndMgr::SaveJPG( LPCTSTR lpszName )
{
	IDirect3DSurface9* pDestSurface;

	CTime ctTime = CTime::GetCurrentTime();
	char lpString[260]	= { 0, };
	FLSPrintf( lpString, _countof( lpString ), prj.GetText( TID_GAME_SCREENSHOT_TIME ), ctTime.GetYear(), ctTime.GetMonth(), ctTime.GetDay(), ctTime.GetHour(), ctTime.GetMinute(), ctTime.GetSecond() );
	CRect rect = g_Neuz.GetDeviceRect();
	CSize size = m_p2DRender->m_pFont->GetTextExtent( lpString );
	g_Neuz.m_2DRender.TextOut(rect.right - size.cx - 20, rect.bottom - 70, lpString);

	m_pApp->m_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pDestSurface );
	BOOL bResult = D3DXSaveSurfaceToFile(lpszName, D3DXIFF_JPG, pDestSurface, NULL, NULL );
	pDestSurface->Release();

	return bResult;
}

BOOL CWndMgr::SaveBitmap( LPCTSTR lpszName )
{
	LPBYTE  lpData = new BYTE[ g_Option.m_nResWidth * g_Option.m_nResHeight * 4 ];
	LPDWORD lpdwData = (LPDWORD) lpData;
	memset( lpData, 255, g_Option.m_nResWidth * g_Option.m_nResHeight * 4 );
	
	IDirect3DSurface9* pDestSurface;

	CTime ctTime = CTime::GetCurrentTime();
	char lpString[260]	= { 0, };
	FLSPrintf( lpString, _countof( lpString ), prj.GetText( TID_GAME_SCREENSHOT_TIME ), ctTime.GetYear(), ctTime.GetMonth(), ctTime.GetDay(), ctTime.GetHour(), ctTime.GetMinute(), ctTime.GetSecond() );
	CRect rect = g_Neuz.GetDeviceRect();
	CSize size = m_p2DRender->m_pFont->GetTextExtent( lpString );
	g_Neuz.m_2DRender.TextOut(rect.right - size.cx - 20, rect.bottom - 70, lpString);

	m_pApp->m_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pDestSurface );

	D3DLOCKED_RECT lockedRect;
	CRect rectIn ( 0, 0, m_pApp->m_d3dsdBackBuffer.Width, m_pApp->m_d3dsdBackBuffer.Height );//cb.bih.biWidth, abs(cb.bih.biHeight) );
	HRESULT hr = pDestSurface->LockRect( &lockedRect, NULL, 0 ) ;
	if( hr == D3D_OK )// Lock
	{
		LPBYTE pSrcByte = (LPBYTE)lockedRect.pBits; 
		LPWORD pSrcWord = (LPWORD)lockedRect.pBits; // 16bit
		//LPDWORD pSrcDWord = (LPDWORD)lockedRect.pBits; // 32bit

		DWORD dwOffset = 0;
		DWORD dwBit = 0;
		switch( g_Neuz.m_d3dpp.BackBufferFormat )
		{
			case D3DFMT_R8G8B8: dwBit = 24; break; // 20 24-bit RGB pixel format with 8 bits per channel. 
			case D3DFMT_A8R8G8B8: dwBit = 32; break; //  21 32-bit ARGB pixel format with alpha, using 8 bits per channel. 
			case D3DFMT_X8R8G8B8: dwBit = 32; break; //  22 32-bit RGB pixel format, where 8 bits are reserved for each color. 
			case D3DFMT_R5G6B5: dwBit = 16; break; //  23 16-bit RGB pixel format with 5 bits for red, 6 bits for green, and 5 bits for blue. 
			case D3DFMT_X1R5G5B5: dwBit = 16; break; //  24 16-bit pixel format where 5 bits are reserved for each color. 
			case D3DFMT_A1R5G5B5: dwBit = 16; break; //  25 16-bit pixel format where 5 bits are reserved for each color and 1 bit is reserved for alpha. 
			case D3DFMT_A4R4G4B4: dwBit = 16; break; //  26 16-bit ARGB pixel format with 4 bits for each channel. 
			case D3DFMT_R3G3B2: dwBit = 8; break; //  27 8-bit RGB texture format using 3 bits for red, 3 bits for green, and 2 bits for blue. 
			case D3DFMT_A8: dwBit = 8; break; //  28 8-bit alpha only. 
			case D3DFMT_A8R3G3B2: dwBit = 16; break; //  29 16-bit ARGB texture format using 8 bits for alpha, 3 bits each for red and green, and 2 bits for blue. 
			case D3DFMT_X4R4G4B4: dwBit = 16; break; //  30 16-bit RGB pixel format using 4 bits for each color. 
			case D3DFMT_A2B10G10R10: dwBit = 32; break; //  31 32-bit pixel format using 10 bits for each color and 2 bits for alpha. 
			case D3DFMT_A8B8G8R8: dwBit = 32; break; //  32 32-bit ARGB pixel format with alpha, using 8 bits per channel. 
			case D3DFMT_X8B8G8R8: dwBit = 32; break; //  33 32-bit RGB pixel format, where 8 bits are reserved for each color. 
			case D3DFMT_G16R16: dwBit = 32; break; //  34 32-bit pixel format using 16 bits each for green and red. 
			case D3DFMT_A2R10G10B10: dwBit = 32; break; //  35 32-bit pixel format using 10 bits each for red, green, and blue, and 2 bits for alpha. 
			case D3DFMT_A16B16G16R16: dwBit = 64; break; //  36 64-bit pixel format using 16 bits for each component. 
			case D3DFMT_A8P8: dwBit = 8; break; //  40 8-bit color indexed with 8 bits of alpha. 
			case D3DFMT_P8: dwBit = 8; break; //  41 8-bit color indexed. 
			case D3DFMT_L8: dwBit = 8; break; //  50 8-bit luminance only. 
			case D3DFMT_L16: dwBit = 16; break; //  81 16-bit luminance only. 
			case D3DFMT_A8L8: dwBit = 16; break; //  51 16-bit using 8 bits each for alpha and luminance. 
			case D3DFMT_A4L4: dwBit = 8; break; //  8-bit using 4 bits each for alpha and luminance.
		}	
		if( dwBit == 16 || dwBit == 32 )
		{
			int nPitch = lockedRect.Pitch;
			if( dwBit == 32 )
			{
				for(int y = 0; y < g_Option.m_nResHeight; y++ )
				{
					for(int x = 0; x < g_Option.m_nResWidth; x++, dwOffset += 3 )
					{
						// 32 bit color code
						BYTE byData3 = pSrcByte[ y * nPitch + ( x * 4 ) + 0 ];
						BYTE byData2 = pSrcByte[ y * nPitch + ( x * 4 ) + 1 ];
						BYTE byData1 = pSrcByte[ y * nPitch + ( x * 4 ) + 2 ];
						lpData[ dwOffset + 0 ] = byData3;
						lpData[ dwOffset + 1 ] = byData2;
						lpData[ dwOffset + 2 ] = byData1;
					}
				}
			}
			else
			{
				nPitch /= 2;
				for(int y = 0; y < g_Option.m_nResHeight; y++ )
				{
					for(int x = 0; x < g_Option.m_nResWidth; x++, dwOffset += 3 )
					{
						WORD wData = pSrcWord[ y * nPitch + x ];
							
						BYTE byData1 = ( wData & 0xf800 ) >> 8;
						BYTE byData2 = ( wData & 0x07e0 ) >> 3;
						BYTE byData3 = ( wData & 0x001f ) << 3 ;
						
						lpData[ dwOffset + 0 ] = byData3;
						lpData[ dwOffset + 1 ] = byData2;
						lpData[ dwOffset + 2 ] = byData1;
					}
				}
			}
		}
		pDestSurface->UnlockRect();
	}
	
	pDestSurface->Release();
	BOOL bResult = SaveBMP( lpszName, lpData,CSize( g_Option.m_nResWidth, g_Option.m_nResHeight ) );
	SAFE_DELETE_ARRAY( lpData );
	return bResult;
}
CWndMap* CWndMgr::OpenMap( LPCTSTR lpszMapFileName )
{
	CWndMap* pWndMap = NULL;
	if( m_mapMap.Lookup( lpszMapFileName, (void*&)pWndMap ) == FALSE )
	{
		pWndMap = new CWndMap;
		FLStrcpy( pWndMap->m_szMapFile, _countof( pWndMap->m_szMapFile ), lpszMapFileName );
		pWndMap->Initialize();
		m_mapMap.SetAt( lpszMapFileName, pWndMap );
	}
	return pWndMap;
}

CCollectingWnd* CWndMgr::OpenCollecting()
{

	if(!m_pWndCollecting)
	{
		m_pWndCollecting = new CCollectingWnd;
		m_pWndCollecting->Initialize();
	}

	return m_pWndCollecting;
}


BOOL CWndMgr::CloseCollecting()
{

	if(m_pWndCollecting)
	{
		m_pWndCollecting->Destroy();
		SAFE_DELETE(m_pWndCollecting);
		m_pWndCollecting = NULL;
		return TRUE;
	}

	return FALSE;
}

void CWndMgr::UpdateContentsByBuff( WORD wType, WORD wID )
{
	//πˆ«¡∑Œ ¿Œ«ÿ ƒ¡≈Ÿ√˜∞° æ˜µ•¿Ã∆Æ µ«æÓæﬂ «œ¥¬∞ÊøÏ √≥∏Æ
	PT_ITEM_SPEC pItemProp = NULL; 
	if( wType == BUFF_ITEM )
		pItemProp = g_xSpecManager->GetSpecItem( wID );

	if( pItemProp )
	{
		if( pItemProp->dwItemKind3 == IK3_MAPOPEN_DUNGEON )
		{
			CWndMapEx* pWndMap = ( CWndMapEx* )GetApplet( APP_MAP_EX );
			if( pWndMap )
				pWndMap->OnBlindBuff();
		}
	}
}


CWndMessage* CWndMgr::GetMessage( LPCTSTR lpszFrom )
{
	CString string;
	CWndMessage* pWndMessage = NULL;
	m_mapMessage.Lookup( lpszFrom, (void*&)pWndMessage );
	return pWndMessage;
}
CWndMessage* CWndMgr::OpenMessage( LPCTSTR lpszFrom )
{
	CString string;
	CWndMessage* pWndMessage = NULL;
	if( m_mapMessage.Lookup( lpszFrom, (void*&)pWndMessage ) == FALSE )
	{
		pWndMessage = new CWndMessage;
		pWndMessage->Initialize();
		m_mapMessage.SetAt( lpszFrom, pWndMessage );
		string = pWndMessage->GetTitle();
		string += " - ";
		string += lpszFrom;
		pWndMessage->SetTitle( string );
		pWndMessage->m_strPlayer = lpszFrom;
	}
	else
	{
		pWndMessage->InitSize();
	}
	return pWndMessage;
}

BOOL CWndMgr::UpdateMessage( LPCTSTR pszOld, LPCTSTR pszNew )
{
	CWndMessage* pWndMessage	= NULL;
	if( m_mapMessage.Lookup( pszOld, (void*&)pWndMessage ) )
	{
		m_mapMessage.RemoveKey( pWndMessage->m_strPlayer );
		m_mapMessage.SetAt( pszNew, pWndMessage );
		CString string	= pWndMessage->GetTitle();
		string.Replace( pszOld, pszNew );
		pWndMessage->SetTitle( string );
		pWndMessage->m_strPlayer	= pszNew;
		return TRUE;
	}
	return FALSE;
}

CWndInstantMsg* CWndMgr::GetInstantMsg( LPCTSTR lpszFrom )
{
	CString string;
	CWndInstantMsg* pWndMessage = NULL;
	m_mapInstantMsg.Lookup( lpszFrom, (void*&)pWndMessage );
	return pWndMessage;
}
CWndInstantMsg* CWndMgr::OpenInstantMsg( LPCTSTR lpszFrom )
{
	CString string;
	CWndInstantMsg* pWndMessage = NULL;
	if( m_mapInstantMsg.Lookup( lpszFrom, (void*&)pWndMessage ) == FALSE )
	{
		CWndBase* pWndFocus = GetFocusWnd();
		pWndMessage = new CWndInstantMsg;
		pWndMessage->Initialize();
		pWndFocus->SetFocus();
		pWndMessage->m_strPlayer = lpszFrom;
		int nNumber = m_mapInstantMsg.GetCount();
		CRect rcMsg = pWndMessage->GetWindowRect();
		CRect rcWnd = GetLayoutRect();

		int nNumHeight = rcWnd.Height() / rcMsg.Height();
		int nNumWidth = rcWnd.Width() / rcMsg.Width();

		int nCount = ( nNumber / nNumHeight ) + 1; // +1æ»«ÿ¡÷∏È »≠∏È¿ª π˛æÓ≥≠ ∞˜ø° ø≠∏≤ 
		int nCount2 = ( nNumber % nNumHeight ) + 1;

		CPoint pt( rcWnd.right - ( rcMsg.Width() * nCount ) , rcWnd.bottom - ( rcMsg.Height() * nCount2 ) );
 		pWndMessage->Move( pt );
		m_mapInstantMsg.SetAt( lpszFrom, pWndMessage );
		
	}
	return pWndMessage;
}
// ∑π¡ˆΩ∫∆Æ ¿Œ∆˜∏¶ æÚ¥¬¥Ÿ.
LPWNDREGINFO CWndMgr::GetRegInfo( DWORD dwWndId  )
{
	LPWNDREGINFO pWndRegInfo = NULL;
	m_mapWndRegInfo.Lookup( dwWndId, (void*&)pWndRegInfo );
	return pWndRegInfo;
}		
// ƒ√∑±∆Æ ¿©µµ∞° ¥›»˙ ∂ß »£√‚ µ  
BOOL CWndMgr::PutRegInfo( CWndNeuz* pWndNeuz, BOOL bOpen )
{
	LPWNDREGINFO pWndRegInfo = NULL;
	BOOL bLookup = FALSE;
	if( m_mapWndRegInfo.Lookup( pWndNeuz->GetWndId(), (void*&)pWndRegInfo ) == TRUE )
	{
		SAFE_DELETE( pWndRegInfo->lpArchive );
		pWndRegInfo->dwSize = 0;
		bLookup = TRUE;
	}
	else
		pWndRegInfo = new WNDREGINFO;
	pWndRegInfo->dwWndId = pWndNeuz->GetWndId();
	pWndRegInfo->rect = pWndNeuz->GetWindowRect( TRUE );
	pWndRegInfo->bOpen = bOpen;
	pWndRegInfo->dwWndSize = pWndNeuz->m_nWinSize;
	pWndRegInfo->dwVersion = 0;
	CAr ar;
	u_long nSize;
	LPBYTE lpData;
	// Write
	pWndNeuz->SerializeRegInfo( ar, pWndRegInfo->dwVersion );
	lpData = ar.GetBuffer( &nSize );
	if( nSize )
	{
		pWndRegInfo->lpArchive = new BYTE[ nSize ];
		pWndRegInfo->dwSize = nSize;
		memcpy( pWndRegInfo->lpArchive, lpData, nSize );
	}
	else
	{
		pWndRegInfo->lpArchive = NULL;
		pWndRegInfo->dwSize = 0;
	}
	if( bLookup == FALSE )
		m_mapWndRegInfo.SetAt( pWndNeuz->GetWndId(), pWndRegInfo );
	return TRUE;
}
// ∑ŒµÂ«— ∑π¡ˆΩ∫∆Æ∏Æ¿Œ∆˜∏¶ ºº∆Æ«“∂ß 
BOOL CWndMgr::PutRegInfo( LPWNDREGINFO lpRegInfo )
{
	LPWNDREGINFO pWndRegInfo = NULL;
	if( m_mapWndRegInfo.Lookup( lpRegInfo->dwWndId, (void*&)pWndRegInfo ) == TRUE )
	{
		// ∞ªΩ≈ 
		SAFE_DELETE( pWndRegInfo->lpArchive );
		memcpy( pWndRegInfo, lpRegInfo, sizeof( WNDREGINFO ) );
		if( lpRegInfo->dwSize )
		{
			pWndRegInfo->lpArchive = new BYTE[ lpRegInfo->dwSize ];
			memcpy( pWndRegInfo->lpArchive, lpRegInfo->lpArchive, lpRegInfo->dwSize );
		}
		else
			pWndRegInfo->lpArchive = NULL;
		return FALSE;
	}
	pWndRegInfo = new WNDREGINFO;
	memcpy( pWndRegInfo, lpRegInfo, sizeof( WNDREGINFO ) );
	if( lpRegInfo->dwSize )
	{
		pWndRegInfo->lpArchive = new BYTE[ lpRegInfo->dwSize ];
		memcpy( pWndRegInfo->lpArchive, lpRegInfo->lpArchive, lpRegInfo->dwSize );
	}
	else
		pWndRegInfo->lpArchive = NULL;
	m_mapWndRegInfo.SetAt( lpRegInfo->dwWndId, pWndRegInfo );
	return TRUE;
}
BOOL CWndMgr::SaveRegInfo( LPCTSTR lpszFileName )
{
	CFileIO file;
	if( file.Open( lpszFileName, "wb" ) == FALSE )
		return FALSE;
	POSITION pos = m_mapWndRegInfo.GetStartPosition();
	LPWNDREGINFO pWndRegInfo = NULL;
	//CRect* pRect;
	DWORD dwWndId;
	file.PutDW( REG_VERSION );
	file.PutDW( m_mapWndRegInfo.GetCount() );
	while( pos )
	{
		m_mapWndRegInfo.GetNextAssoc( pos, dwWndId, (void*&)pWndRegInfo );
		file.Write( &pWndRegInfo->dwWndId, sizeof( pWndRegInfo->dwWndId ) );
		file.Write( &pWndRegInfo->rect, sizeof( pWndRegInfo->rect ) );
		file.Write( &pWndRegInfo->dwWndSize, sizeof( pWndRegInfo->dwWndSize ) );
		file.Write( &pWndRegInfo->bOpen, sizeof( pWndRegInfo->bOpen ) );
		file.Write( &pWndRegInfo->dwVersion, sizeof( pWndRegInfo->dwVersion ) );
		file.Write( &pWndRegInfo->dwSize, sizeof( pWndRegInfo->dwSize ) );
		if( pWndRegInfo->dwSize )
			file.Write( pWndRegInfo->lpArchive, pWndRegInfo->dwSize );
	}
	file.Close();
	// resolution ∆ƒ¿œ ∏∏µÈ±‚. «ˆ¿Á ªÁ¿Ã¡Ó∑Œ ¿˙¿Â
	if( file.Open( "resolution", "wb" ) == FALSE )
		return FALSE;
	file.PutDW( g_Option.m_nResWidth );
	file.PutDW( g_Option.m_nResHeight );
	file.Close();
	return TRUE;
}
BOOL CWndMgr::LoadRegInfo( LPCTSTR lpszFileName )
{
	CFileIO file;
	BOOL bResult = FALSE;
	if( file.Open( "resolution", "rb" ) )
	{
		int nWidth = file.GetDW();
		int nHeight = file.GetDW();
		file.Close();
		// «ÿªÛµµ∞° ∞∞¿∏∏È ∫Ø∞Ê¿Ã æ¯¥¬ ªÛ≈¬¥Ÿ. ∑π¡ˆΩ∫∆Æ ∆ƒ¿œ ¿–±‚ 
		if( g_Option.m_nResWidth == nWidth && g_Option.m_nResHeight == nHeight )
		{
			bResult = TRUE;
			// ªÁ¿Ã¡Ó ¿˙¿Â ∆ƒ¿œ ∑ŒµÂ 
			if( file.Open( lpszFileName, "rb" ) == FALSE )
				return FALSE;
			DWORD dwRegVersion = file.GetDW();
			if( dwRegVersion != REG_VERSION )
				return FALSE;
			WNDREGINFO wndRegInfo;
			//CWndBase* pWndBase; 
			int nNum = file.GetDW();
			for( int i = 0; i < nNum; i++ )
			{
				file.Read( &wndRegInfo.dwWndId, sizeof( wndRegInfo.dwWndId ) );
				file.Read( &wndRegInfo.rect, sizeof( wndRegInfo.rect ) );
				file.Read( &wndRegInfo.dwWndSize, sizeof( wndRegInfo.dwWndSize ) );
				file.Read( &wndRegInfo.bOpen, sizeof( wndRegInfo.bOpen ) );
				file.Read( &wndRegInfo.dwVersion, sizeof( wndRegInfo.dwVersion ) );
				file.Read( &wndRegInfo.dwSize, sizeof( wndRegInfo.dwSize ) );
				if( wndRegInfo.dwSize )
				{
					wndRegInfo.lpArchive = new BYTE[ wndRegInfo.dwSize ];
					file.Read( wndRegInfo.lpArchive, wndRegInfo.dwSize );
				}
				else
					wndRegInfo.lpArchive = NULL;
//sun: 13, ƒ≥∏Ø≈Õ¡§∫∏√¢ ∏Æ¥∫æÛ
				if(wndRegInfo.rect.left < 0 )
					wndRegInfo.rect.left = 0;
				if(wndRegInfo.rect.top < 0)
					wndRegInfo.rect.top = 0;

				/*if( wndRegInfo.bOpen )
				{
					pWndBase = CreateApplet( wndRegInfo.dwWndId );
					if( pWndBase )
					{
						if( wndRegInfo.dwSize )
						{
							// load
							CAr ar( wndRegInfo.lpArchive, wndRegInfo.dwSize );
							((CWndNeuz*)pWndBase)->SerializeRegInfo( ar, wndRegInfo.dwVersion );
						}
						if( pWndBase->IsWndStyle( WBS_THICKFRAME ) )
						{
							if( wndRegInfo.dwWndSize == WSIZE_WINDOW )
							{
								((CWndNeuz*)pWndBase)->SetSizeWnd();
								pWndBase->SetWndRect( wndRegInfo.rect );
							}								
							if( wndRegInfo.dwWndSize == WSIZE_MAX )
								((CWndNeuz*)pWndBase)->SetSizeMax();
						}
						else
						{
							CRect wndRect = pWndBase->GetWindowRect();	
							pWndBase->SetWndRect( 
								CRect( 
								wndRegInfo.rect.left, 
								wndRegInfo.rect.top, 
								wndRegInfo.rect.left + wndRect.Width(), 
								wndRegInfo.rect.top + wndRect.Height() 
								) );
						}							
					}
				}*/
				PutRegInfo( &wndRegInfo );//.dwWndId, wndRegInfo.rect, wndRegInfo.bOpen );
				SAFE_DELETE( wndRegInfo.lpArchive );
				//PutRegInfo( wndRegInfo.dwWndId, wndRegInfo.rect, wndRegInfo.bOpen );
			}
			file.Close();
		}
	}
	return bResult;
}

void CWndMgr::SetMessengerAutoState()
{
	if( m_bConnect && m_RTMessenger.GetState() == FRS_AUTOABSENT )
	{
		g_DPlay.SendSetState( FRS_ONLINE );	
		m_timerMessenger.Set( MIN( 5 ) );
	}
}

void CWndMgr::PutDestParam( DWORD dwDst1, DWORD dwDst2, DWORD dwAdj1, DWORD dwAdj2, CEditString &str )
{
	PFONTCOLOR_WNDMGR pFontColorWndMgr = g_WndFontColorManager->GetWndMgr();

	CString strTemp( _T("") );
	CString strSignAdj( _T("") );
//sun: 10¬˜ ¿¸Ω¬Ω√Ω∫≈€	Neuz, World, Trans
	if( dwDst1 != NULL_ID && dwDst1 != 0 && dwDst1 != DST_CHRSTATE )
	{
		int nAdj = dwAdj1;

		if(nAdj < 0)
			strSignAdj = "";
		else
			strSignAdj = "+";
		
		if( IsDst_Rate(dwDst1) )
		{
			if( dwDst1 == DST_ATTACKSPEED )
				strTemp.Format( "\n%s%s%d%%", FindDstString(dwDst1), strSignAdj, static_cast< int >( dwAdj1 ) / 2 / 10 );
			else
				strTemp.Format( "\n%s%s%d%%", FindDstString(dwDst1), strSignAdj, dwAdj1 );					
		}
		else
		{
			strTemp.Format( "\n%s%s%d", FindDstString(dwDst1), strSignAdj, dwAdj1 );								
		}

		str.AddString(strTemp, pFontColorWndMgr->m_stBuffSkillToolTipAbility.GetFontColor() );
	}

//sun: 10¬˜ ¿¸Ω¬Ω√Ω∫≈€	Neuz, World, Trans
	if( dwDst2 != NULL_ID && dwDst2 != 0 && dwDst2 != DST_CHRSTATE )
	{
		int nAdj = dwAdj2;

		if(nAdj < 0)
			strSignAdj = "";
		else
			strSignAdj = "+";

		if( IsDst_Rate(dwDst2) )
		{
			if( dwDst2 == DST_ATTACKSPEED )
				strTemp.Format( "\n%s%s%d%%", FindDstString(dwDst2), strSignAdj, static_cast< int >( dwAdj2 ) / 2 / 10 );
			else
				strTemp.Format( "\n%s%s%d%%", FindDstString(dwDst2), strSignAdj, dwAdj2 );					
		}
		else
		{
			strTemp.Format( "\n%s%s%d", FindDstString(dwDst2), strSignAdj, dwAdj2 );								
		}

		str.AddString(strTemp, pFontColorWndMgr->m_stBuffSkillToolTipAbility.GetFontColor() );
	}	
}

BOOL IsEquipedVis( FLItemElem* pPetItem, DWORD visIndex );
BOOL IsEquipedRequireVis( FLItemElem* pPetItem, DWORD visIndex, BOOL bSelfCheck );


//---------------------------------------------------------------------------------------------
// ≈¯∆¡ √‚∑¬ ( « ø‰ ∫ÒΩ∫ )
// param	: æ∆¿Ã≈€, ≈¯∆¡ πÆ¿⁄ø≠
// return	: void
//---------------------------------------------------------------------------------------------
void CWndMgr::PutNeededVis( FLItemElem* pItemElem, CEditString* pEdit )
{
	CString strToolTip( _T("") );

	PT_ITEM_SPEC pItemPropVis( pItemElem->GetProp() );
	FLERROR_ASSERT_LOG_RETURN_VOID( pItemPropVis, _T("[¿©µµøÏ ∏ﬁ¥œ¡Æ] ∫ÒΩ∫ æ∆¿Ã≈€¿« º”º∫¿ª æÚæÓ ø√ ºˆ æ¯Ω¿¥œ¥Ÿ.") );

	PT_VIS_CHECK_LIST pVisCheckList( g_xSpecManager->GetVisCheckList( pItemPropVis->dwID ) );
	
	if( NULL == pVisCheckList )
		return ;

	BYTE	byVisState( FAILED_BOTH_NEEDVIS );

	FLItemElem* pPetItem( g_pPlayer->GetVisPetItem() );
	if( NULL == pPetItem ) // πˆ«¡∆Í¿Ã º“»Øµ«æÓ ¿÷¡ˆ æ ¿∫ ªÛ≈¬
		byVisState = FAILED_BOTH_NEEDVIS;
	else
		byVisState = g_pPlayer->IsSatisfyNeedVis( pPetItem, pItemPropVis );

	for( UINT nCount = 0; nCount < MAX_VIS_CONDITION_COUNT; nCount++ )
	{
		if( NULL_ID == pVisCheckList->dwGroupID[nCount] )
			continue;

		DWORD dwTextColor( m_dwColor );

		if( FAILED_BOTH_NEEDVIS == byVisState )
			dwTextColor = 0xFFFF0000;

		if( FAILED_1ST_NEEDVIS == byVisState )
		{
			if( 0 == nCount )
				dwTextColor = 0xFFFF0000;
		}
	
		if( FAILED_2ND_NEEDVIS == byVisState )
		{
			if( 1 == nCount )
				dwTextColor = 0xFFFF0000;
		}

		PT_VIS_GROUP_LIST pVisGroupList( g_xSpecManager->GetVisGroupInfo( pVisCheckList->dwGroupID[nCount] ) );
		FLERROR_ASSERT_LOG_DO( pVisGroupList, continue, _T("[¿©µµøÏ ∏ﬁ¥œ¡Æ] ∫ÒΩ∫ æ∆¿Ã≈€¿« ±◊∑Ï ∏ÆΩ∫∆Æ∏¶ æÚæÓ ø√ ºˆ æ¯Ω¿¥œ¥Ÿ.") );

		strToolTip.Format( _T("\n%s: %s"), GETTEXT( TID_GAME_BUFFPET_REQUIRE ), prj.GetText( pVisGroupList->dwGroupName ) ); 

		pEdit->AddString( strToolTip, dwTextColor );
	}
}

BOOL IsEquipedRequireVis( FLItemElem* pItemElem, DWORD index, BOOL bSelfCheck );
void CWndMgr::PutVisPetInfo( FLItemElem* pItemElem, CEditString* pEdit )
{
	if( !pItemElem )
		return;

	//∆Í¿« ¡§∫∏ (∫ÒΩ∫µÈ¿« √—«’ )
	std::map< int, int > cTotalOpt;
	std::map< int, int >::iterator iter;
	CString strTemp, strTemp2;
	
	static const int MAX_PROP = 3;

	int availableSlot = pItemElem->GetGeneralPiercingSize( );
	
	// ¿Â¬¯µ» ∏µÁ ∫ÒΩ∫¿« ¥…∑¬ƒ° ¥ı«œ±‚ 
	for( int ia = 0; ia < availableSlot; ++ia )
	{
		DWORD index = pItemElem->GetGeneralPiercingItemID( ia );
		PT_ITEM_SPEC pProp = g_xSpecManager->GetSpecItem( index );
		if( !pProp )
			continue;

		if( pProp->dwSkillTime != 999999999 && time_null() >= pItemElem->GetVisPetPiercingItemKeepTime( ia ) )
			continue;

		BOOL bOK = g_pPlayer->IsSatisfyNeedVis( pItemElem, pProp );		

		if( SUCCSESS_NEEDVIS == bOK )		//OK »∞º∫¡ﬂ¿”.
		{
			for( int iaa = 0; iaa < MAX_PROP; ++iaa )
			{
				int nDst	= (int)pProp->dwDestParam[iaa];
				if( NULL_ID == nDst )
					continue;

				int nVal	= (int)pProp->nAdjParamVal[iaa];
				iter = cTotalOpt.find( nDst );
				if( iter == cTotalOpt.end() )
					cTotalOpt.insert( std::map< int, int >::value_type( nDst, nVal ) );
				else cTotalOpt[ nDst ] += nVal;
			}
		}
	}

	//¿¸∫Œ ¥ı«ÿ¡¯ ø…º«ø° ¥Î«ÿº≠ √‚∑¬ 
	for( iter = cTotalOpt.begin(); iter != cTotalOpt.end(); ++iter )
	{
		int nDst = iter->first;
		int nVal = iter->second;
		
		if( IsDst_Rate( nDst ) )
		{
			if( nDst == DST_ATTACKSPEED )
				strTemp.Format( "\n%s%+d%%", FindDstString( nDst ), nVal / 2 / 10 );
			else
				strTemp.Format( "\n%s%+d%%", FindDstString( nDst ), nVal );
		}
		else
		{
			strTemp.Format( "\n%s%+d", FindDstString( nDst ), nVal );
		}

		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwPiercing );
	}

	//¿Â¬¯µ» ∫ÒΩ∫ ¿Ã∏ß + ≥≤¿∫ Ω√∞£ √‚∑¬ 
	for( int ib = 0; ib < MAX_VIS; ++ib )
	{
		DWORD index = pItemElem->GetGeneralPiercingItemID( ib );
		if( 0 != index && NULL_ID != index )
		{
			PT_ITEM_SPEC pProp = g_xSpecManager->GetSpecItem( index );
			if( !pProp )
				continue;

			/************************************************************************/
			/* dwSkillTime ¿Ã 999999999 ¿Ã∏È øµ±∏. Ω√∞£ √‚∑¬ «œ¡ˆ æ ¥¬¥Ÿ.           */
			/************************************************************************/
			if( pProp->dwSkillTime == 999999999 ) {
				continue;
			}

			time_t dwKeepTime = pItemElem->GetVisPetPiercingItemKeepTime( ib );

			time_t t = dwKeepTime - time_null();
			CTimeSpan ct( t );

			//strTemp2.Format( prj.GetText( TID_TOOLTIP_DATE ), static_cast<int>(ct.GetDays()), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );	// ¡ˆº”Ω√∞£ : 
			
		//	strTemp2.Format( prj.GetText( TID_TOOLTIP_DATE ), static_cast<int>(ct.GetDays()), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );	// ¡ˆº”Ω√∞£ : 
			LONG nDay	= (LONG)( ct.GetDays() );
			LONG nHour	= ct.GetHours();
			LONG nMin	= ct.GetMinutes();
			LONG nSec	= ct.GetSeconds();

			CString strDays, strHours, strMinutes, strSeconds;
			if( nDay > 0 )
			{
				strDays.Format( prj.GetText(TID_PK_LIMIT_DAY), static_cast<int>(nDay) );
				strHours.Format( prj.GetText( TID_PK_LIMIT_HOUR ), nHour );
				strMinutes.Format( prj.GetText(TID_PK_LIMIT_MINUTE), nMin );

				strTemp2 = strDays + strHours + strMinutes;
			}
			else if( nHour > 0 )
			{
				strHours.Format( prj.GetText(TID_PK_LIMIT_HOUR), nHour );
				strMinutes.Format( prj.GetText(TID_PK_LIMIT_MINUTE), nMin );

				strTemp2 = strHours + strMinutes;
			}else if( nMin > 0 )
			{
				strMinutes.Format( prj.GetText( TID_PK_LIMIT_MINUTE ), nMin );
				strSeconds.Format( prj.GetText( TID_PK_LIMIT_SECOND ), nSec );

				strTemp2 = strMinutes + strSeconds;
			}
			else if( nSec >= 0 )
			{
				strTemp2.Format( prj.GetText( TID_PK_LIMIT_SECOND ), nSec );
			}
			else 
			{
				strTemp2.Format( GETTEXT( TID_GAME_TOOLTIP_TIMEOUT ) ); //"Ω√∞£ ∏∏∑·" 
			}

			strTemp = pProp->szName + CString(" (") + strTemp2 + CString(")");

			pEdit->AddString( "\n" );

			if( t > 0 )
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwTime );
			else 
				pEdit->AddString( strTemp, 0xffff0000 );
	
		}
	}
}

void CWndMgr::PutPetKind( FLItemElem* pItemElem, CEditString* pEdit )
{
	// PET ¡æ∑˘ ¿‘∑¬ 
	CString strTemp;
	
	if( pItemElem->IsEatPet() )
	{
		if( pItemElem->IsVisPet( ) )
		{
			strTemp.Format( "\n%s", GETTEXT(TID_TOOLTIP_PET_BUFF) );		//πˆ«¡∆Í 
		//	pEdit->AddString( strTemp, GETTEXTCOLOR(TID_TOOLTIP_PET_BUFF) );
		}
		else 
		{
			strTemp.Format( "\n%s", GETTEXT(TID_TOOLTIP_PET_PICKUP) );	// «»æ˜∆Í
		//	pEdit->AddString( strTemp, GETTEXTCOLOR(TID_TOOLTIP_PET_PICKUP) );
		}
	}

	if( pItemElem->IsEgg( ) )
	{
		strTemp.Format( "\n%s", GETTEXT(TID_TOOLTIP_PET_REAR) );	//∏ÆæÓ∆Í 
		//pEdit->AddString( strTemp, GETTEXTCOLOR(TID_TOOLTIP_PET_REAR) );
	}

	pEdit->AddString( strTemp );
	
#ifdef __SHOW_ITEM_ID
	if( g_pPlayer->IsAuthHigher( AUTH_OPERATOR ) )
    {
		PT_ITEM_SPEC pItemProp = pItemElem->GetProp();
        CString nID;
        nID.Format( "\nItem ID: %d", pItemProp->dwID );
        pEdit->AddString( nID, D3DCOLOR_XRGB( 255, 102, 0 ) );
    }      
#endif //__SHOW_ITEM_ID	

}
#ifdef __ZCK_PING_FPS_AND_TOOLTIP
void CWndMgr::PutZckItemTechnicalInfo( FLItemBase* pItemBase, CEditString* pEdit, int flag )
{
	if( pItemBase == NULL || pEdit == NULL || g_pPlayer == NULL )
		return;

	if( g_pPlayer->IsAuthHigher( AUTH_ADMINISTRATOR ) == FALSE )
		return;

	PT_ITEM_SPEC pItemProp = pItemBase->GetProp();
	if( pItemProp == NULL )
		return;

	const DWORD dwTechnicalColor = D3DCOLOR_XRGB( 255, 102, 0 );
	CString strTechnical;

	strTechnical.Format(
		_T( "\nItem ID : %lu" ),
		static_cast<unsigned long>( pItemProp->dwID )
	);
	pEdit->AddString( strTechnical, dwTechnicalColor );

#ifdef __ZCK_WIKI
	if( flag == APP_WIKI_ITEMS )
	{
		strTechnical.Format(
			_T( "\nItem Kind 1 : %lu" ),
			static_cast<unsigned long>( pItemProp->dwItemKind1 )
		);
		pEdit->AddString( strTechnical, dwTechnicalColor );

		strTechnical.Format(
			_T( "\nItem Kind 2 : %lu" ),
			static_cast<unsigned long>( pItemProp->dwItemKind2 )
		);
		pEdit->AddString( strTechnical, dwTechnicalColor );

		strTechnical.Format(
			_T( "\nItem Kind 3 : %lu" ),
			static_cast<unsigned long>( pItemProp->dwItemKind3 )
		);
		pEdit->AddString( strTechnical, dwTechnicalColor );

		strTechnical.Format(
			_T( "\nItem Level : %lu" ),
			static_cast<unsigned long>( pItemProp->dwItemLV )
		);
		pEdit->AddString( strTechnical, dwTechnicalColor );

		if( pItemProp->dwItemJob != NULL_ID )
		{
			strTechnical.Format(
				_T( "\nItem Job : %lu" ),
				static_cast<unsigned long>( pItemProp->dwItemJob )
			);
			pEdit->AddString( strTechnical, dwTechnicalColor );
		}

		if( pItemProp->dwParts != NULL_ID )
		{
			strTechnical.Format(
				_T( "\nParts : %lu" ),
				static_cast<unsigned long>( pItemProp->dwParts )
			);
			pEdit->AddString( strTechnical, dwTechnicalColor );
		}

		strTechnical.Format(
			_T( "\nItem Grade : %lu" ),
			static_cast<unsigned long>( pItemProp->dwItemGrade )
		);
		pEdit->AddString( strTechnical, dwTechnicalColor );

		strTechnical.Format(
			_T( "\nSubDefine : %lu" ),
			static_cast<unsigned long>( pItemProp->dwSubDefine )
		);
		pEdit->AddString( strTechnical, dwTechnicalColor );

		return;
	}
#endif // __ZCK_WIKI

	if( pItemBase->m_dwObjId != NULL_ID )
	{
		strTechnical.Format(
			_T( "\nObj ID : %lu" ),
			static_cast<unsigned long>( pItemBase->m_dwObjId )
		);
		pEdit->AddString( strTechnical, dwTechnicalColor );
	}

	if( pItemBase->m_dwObjIndex != NULL_ID )
	{
		strTechnical.Format(
			_T( "\nObj Index : %lu" ),
			static_cast<unsigned long>( pItemBase->m_dwObjIndex )
		);
		pEdit->AddString( strTechnical, dwTechnicalColor );
	}

	const SERIALNUMBER iSerialNumber = pItemBase->GetSerialNumber();
	if( iSerialNumber != 0 && iSerialNumber != static_cast<SERIALNUMBER>( NULL_ID ) )
	{
		strTechnical.Format(
			_T( "\nSerial : %lu" ),
			static_cast<unsigned long>( iSerialNumber )
		);
		pEdit->AddString( strTechnical, dwTechnicalColor );
	}
}
#endif // __ZCK_PING_FPS_AND_TOOLTIP
BOOL CWndMgr::CheckConfirm( FLItemBase* pItem )
{
	PT_ITEM_SPEC pItemProp = pItem->GetProp();
	if( !pItemProp )
		return FALSE;

	if( g_pPlayer->HasActivatedVisPet( ) )
	{	
		// ¥Ÿ¿Ω¿∫ πˆ«¡∆Í¿Ã »∞º∫»≠ µ» ªÛ≈¬ø°º≠∏∏ ∫“∑¡æﬂ «—¥Ÿ.
		if( pItemProp->IsVis( ) )
		{
			// gmpbigsun : ∫ÒΩ∫∏¶ ¿Â¬¯«œ±‚¿¸ »Æ¿Œ√¢ 
			if( !g_WndMng.m_pWndBuffPetStatus )
			{
				g_WndMng.m_pWndBuffPetStatus = new CWndBuffPetStatus;
				g_WndMng.m_pWndBuffPetStatus->Initialize();	
			}
				
			g_WndMng.m_pWndBuffPetStatus->DoModal_ConfirmQuestion( pItem->m_dwObjId, g_pPlayer->GetId(), pItemProp->dwID );
			return TRUE;
		}

		if( pItemProp->IsVisKey( ) )
		{
			if( !g_WndMng.m_pWndBuffPetStatus )
			{
				g_WndMng.m_pWndBuffPetStatus = new CWndBuffPetStatus;
				g_WndMng.m_pWndBuffPetStatus->Initialize();	
			}
				
			g_WndMng.m_pWndBuffPetStatus->DoModal_ConfirmQuestion( pItem->m_dwObjId, g_pPlayer->GetId(), pItemProp->dwID, 0, CWndConfirmVis::CVS_EQUIP_VISKEY );
			return TRUE;
		}
	}

	if( pItemProp->IsPickupToBuff( ) )
	{
		// πˆ«¡∆Í¿∏∑Œ ∫Ø»ØΩ√≈∞¥¬ æ∆¿Ã≈€ √≥∏Æ 
		// gmpbigsun : «»æ˜∆Í¿∫ ≈¨∂Ûø°º≠ ¡§∫∏∏¶ ∞°¡ˆ∞Ì ¿÷¡ˆ æ ¥Ÿ. ∞Ì∑Œ ªÁøÎΩ√ »Æ¿Œ√¢∏∏ ∂Áøˆ¡÷∞Ì ≥™∏”¡¯ º≠πˆø°º≠..
		SAFE_DELETE( g_WndMng.m_pWndConfirmVis );

		g_WndMng.m_pWndConfirmVis = new CWndConfirmVis;
		CWndConfirmVis* pWnd = g_WndMng.m_pWndConfirmVis;

		pWnd->m_dwItemId = pItem->m_dwObjId;
		pWnd->m_objid = g_pPlayer->GetId();
		pWnd->m_dwItemIndex = pItemProp->dwID;
		
		pWnd->m_eSection = CWndConfirmVis::CVS_PICKUP_TO_BUFF;		
		pWnd->Initialize( &g_WndMng );
	
		return TRUE;
	}

	//gmpbigsun : 
	//1.∞¢º∫∫∏»£ »Æ¿Œ√¢ ¿∏∑Œ ≥—±Ë 
	//2.»Æ¿Œ√¢ø°º≠ OKΩ√ ∆–≈∂Ω˜¡‹

	if( ITEM_INDEX( 26961, II_SYS_SYS_SCR_AWAKESAFE ) == pItem->m_dwItemId )
	{
		//∞¢º∫∫∏»£ µŒ∑Á∏∂∏Æ
		SAFE_DELETE( g_WndMng.m_pWndConfirmVis );

		g_WndMng.m_pWndConfirmVis = new CWndConfirmVis;
		CWndConfirmVis* pWnd = g_WndMng.m_pWndConfirmVis;

		pWnd->m_dwItemId = pItem->m_dwObjId;
		pWnd->m_objid = g_pPlayer->GetId();
		pWnd->m_dwItemIndex = pItemProp->dwID;
		
		pWnd->m_eSection = CWndConfirmVis::ETC_PROTECT_AWAKE;		
		pWnd->Initialize( &g_WndMng );

		return TRUE;
	}

	//¿ÃΩ«∑Á∆Æ¿« »Ω∫“ : ¥¯¡Ø¡ˆø™ ¡ˆµµø¿«¬
	if( IK3_MAPOPEN_DUNGEON == pItemProp->dwItemKind3 )
	{
		SAFE_DELETE( g_WndMng.m_pWndConfirmVis );

		g_WndMng.m_pWndConfirmVis = new CWndConfirmVis;
		CWndConfirmVis* pWnd = g_WndMng.m_pWndConfirmVis;

		pWnd->m_dwItemId = pItem->m_dwObjId;
		pWnd->m_objid = g_pPlayer->GetId();
		pWnd->m_dwItemIndex = pItemProp->dwID;
		// %s æ∆¿Ã≈€¿ª ªÁøÎ«“ ∞ÊøÏ «ÿ¥Á ¡ˆµµ∞° ∆Àæ˜µ«∏Á æ∆¿Ã≈€¿∫ º“∏Í µÀ¥œ¥Ÿ
		pWnd->m_strTitleString.Format(GETTEXT( TID_MMI_CASHMAP_MAPITEMUSE01 ), pItemProp->szName );	
		
		pWnd->m_eSection = CWndConfirmVis::ETC_CASHITEM;
		pWnd->Initialize( &g_WndMng );

		return TRUE;
	}

	if( pItem->m_dwItemId == ITEM_INDEX( 24391, II_CHR_SYS_SCR_STAT_DICE_EXCEPT_VAGRANT ) && g_pPlayer->GetJob() <= JOB_VAGRANT ) {
		CString string;
		string.Format( prj.GetText( TID_GAME_DO_NOT_USE_VAGRANT ), pItemProp->szName );
		g_WndMng.PutString( (LPCTSTR)string, NULL, prj.GetTextColor( TID_GAME_DO_NOT_USE_VAGRANT ) );
		return TRUE;
	}

	return FALSE;
}

void CWndMgr::PutEquipItemText( CEditString* pEdit )
{
	CString strEdit = _T( "" );
	strEdit.Format( "[ %s ]\n", prj.GetText( TID_GAME_EQUIPED_ITEM ) );

	PFONTCOLOR_WNDTOOLTIP pFontColorWndToolTip = g_WndFontColorManager->GetWndToolTip();
	pEdit->AddString( strEdit, pFontColorWndToolTip->m_stPutEquipItemText.GetFontColor(), ESSTY_BOLD );
}

#ifdef __HYPERLINK_ITEM16
DWORD CWndMgr::GetItemTitleTextColor( FLItemElem* pItemElem )
{
	DWORD dwColor = 0;
	switch( pItemElem->GetProp()->dwReferStat1 )
	{
	case WEAPON_GENERAL:
		{
			dwColor = dwItemColor[ g_Option.m_nToolTipText ].dwName0;
			break;
		}
	case WEAPON_UNIQUE:
		{
			dwColor = dwItemColor[ g_Option.m_nToolTipText ].dwName1;
			break;
		}
	case WEAPON_ULTIMATE:
		{
			dwColor = dwItemColor[ g_Option.m_nToolTipText ].dwName3;
			break;
		}
	case ARMOR_SET:
		{
			dwColor = dwItemColor[ g_Option.m_nToolTipText ].dwName1;
			break;
		}
	case BARUNA_S:
	case BARUNA_A:
	case BARUNA_B:
	case BARUNA_C:
	case BARUNA_D:
		{
			dwColor = dwItemColor[ g_Option.m_nToolTipText ].dwName4;
			break;
		}
	default:
		{
			dwColor = dwItemColor[ g_Option.m_nToolTipText ].dwName0;
		}
	}
	return dwColor;
}
#endif // __HYPERLINK_ITEM16

//sun: 8¬˜, 'T'≈∞ ªÁøÎ, ªÛ¥ÎπÊ ∑π∫ß æ»∫∏¿Ã±‚, »≠∏È ∏µÁ √¢ On/Off Neuz
void CWndMgr::ClearAllWnd()
{
	CWndNeuz* pWndBase;
	DWORD dwIdApplet;
	if(!m_clearFlag)
	{
		POSITION pos = m_mapWndApplet.GetStartPosition();
		while( pos )
		{
			m_mapWndApplet.GetNextAssoc( pos, dwIdApplet, (void*&)pWndBase );
			if( pWndBase && pWndBase->GetWndId() != APP_WORLD)
			{
				m_tempWndId.push_back(pWndBase->GetWndId());
				pWndBase->SetVisible(FALSE);
			}
		}

		if( g_WndMng.m_pWndWorld )
		{
			CAdvMgr* pAdvMgr( g_WndMng.m_pWndWorld->GetAdvMgr() );
			if( pAdvMgr )
				pAdvMgr->SetVisible( FALSE );
		}
		
		CWndChat* pWndChat = (CWndChat*)g_WndMng.GetApplet( APP_COMMUNICATION_CHAT );
		if(pWndChat != NULL && pWndChat->m_bChatLog)
			m_pWndChatLog->SetVisible(FALSE);

//sun: 9¬˜ ¿¸Ω¬∞¸∑√ Clienet
		CWndPetStatus* pWndPetStatus = (CWndPetStatus*)g_WndMng.GetWndBase( APP_PET_STATUS );
		if(pWndPetStatus != NULL)
			pWndPetStatus->SetVisible(FALSE);

		CWndBuffPetStatus* pWndBuffPetStatus = (CWndBuffPetStatus*)g_WndMng.GetWndBase( APP_BUFFPET_STATUS );
		if(pWndBuffPetStatus != NULL)
			pWndBuffPetStatus->SetVisible(FALSE);

		CWndGHMainMenu* pWndGHMain = ( CWndGHMainMenu* )g_WndMng.GetWndBase( APP_GH_MAIN );
		if( pWndGHMain )
			pWndGHMain->SetVisible( FALSE );

		CWndQuestQuickInfo* pWndQuestQuickInfo = ( CWndQuestQuickInfo* )g_WndMng.GetWndBase( APP_QUEST_QUICK_INFO );
		if( pWndQuestQuickInfo )
			pWndQuestQuickInfo->SetVisible( FALSE );

//sun: 12, ∫Òπ–¿« πÊ
		CWndSecretRoomQuick* pWndSecretRoomQuick = (CWndSecretRoomQuick*)g_WndMng.GetWndBase( APP_SECRETROOM_QUICK );
		if(pWndSecretRoomQuick != NULL)
			pWndSecretRoomQuick->SetVisible(FALSE);

		//gmpbigsun( 20110526 ) : ¿Ã¡¶¥¬ ≈¬Ω∫≈©πŸµµ æ¯æŸºˆ¿÷¥Ÿ.
		CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
		if( pTaskBar )
		{
			pTaskBar->SetVisible( FALSE );
			 EnableHUD( FALSE );
		}

		SetFocus();
	}
	m_clearFlag = TRUE;
}

void CWndMgr::RestoreWnd()
{
	CWndNeuz* pWndBase;
	DWORD dwIdApplet;
	BOOL stopFlag = TRUE;
	int wndId;

	if(m_clearFlag)
	{
		std::list<int>::iterator it = m_tempWndId.begin();
		while(it !=  m_tempWndId.end())
		{
			POSITION pos = m_mapWndApplet.GetStartPosition();
			while( pos && stopFlag)
			{
				m_mapWndApplet.GetNextAssoc( pos, dwIdApplet, (void*&)pWndBase );
				wndId = *it;
				if(wndId == pWndBase->GetWndId())
				{
					pWndBase->SetVisible(TRUE);
					stopFlag = FALSE;
				}
			}
			it++;
			stopFlag = TRUE;
		}
		m_tempWndId.clear();
		CWndChat* pWndChat = (CWndChat*)g_WndMng.GetApplet( APP_COMMUNICATION_CHAT );
		if(pWndChat != NULL && pWndChat->m_bChatLog)
			m_pWndChatLog->SetVisible(TRUE);
	
		if( g_WndMng.m_pWndWorld )
		{
			CAdvMgr* pAdvMgr( g_WndMng.m_pWndWorld->GetAdvMgr() );
			if( pAdvMgr )
				pAdvMgr->SetVisible( TRUE );
		}

//sun: 9¬˜ ¿¸Ω¬∞¸∑√ Clienet
		CWndPetStatus* pWndPetStatus = (CWndPetStatus*)g_WndMng.GetWndBase( APP_PET_STATUS );
		if(pWndPetStatus != NULL)
			pWndPetStatus->SetVisible(TRUE);

		CWndBuffPetStatus* pWndBuffPetStatus = (CWndBuffPetStatus*)g_WndMng.GetWndBase( APP_BUFFPET_STATUS );
		if(pWndBuffPetStatus != NULL)
			pWndBuffPetStatus->SetVisible(TRUE);

//sun: 12, ∫Òπ–¿« πÊ
		CWndSecretRoomQuick* pWndSecretRoomQuick = (CWndSecretRoomQuick*)g_WndMng.GetWndBase( APP_SECRETROOM_QUICK );
		if(pWndSecretRoomQuick != NULL)
			pWndSecretRoomQuick->SetVisible(TRUE);

		CWndGHMainMenu* pWndGHMain = (CWndGHMainMenu*)g_WndMng.GetWndBase( APP_GH_MAIN );
		if(pWndGHMain != NULL)
			pWndGHMain->SetVisible(TRUE);

		CWndQuestQuickInfo* pWndQuestQuickInfo = ( CWndQuestQuickInfo* )g_WndMng.GetWndBase( APP_QUEST_QUICK_INFO );
		if( pWndQuestQuickInfo )
			pWndQuestQuickInfo->SetVisible( TRUE );
		
		CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
		if( pTaskBar )
		{
			pTaskBar->SetVisible( TRUE );
			EnableHUD( TRUE );
		}
	}
	m_clearFlag = FALSE;
}


CWndBase* CWndMgr::GetWndVendorBase( void )
{
	return GetWndBase( APP_VENDOR_REVISION );
}

//sun: 12, ««æÓΩÃ ¡¶∞≈ √¢ ∞≥º± π◊ æÛ≈Õ∏⁄ ∫∏ºÆ ¡¶∞≈ √¢ √ﬂ∞°
DWORD CWndMgr::GetDSTStringId(int nDstParam)
{
	for( int i=0; ; ++i )
	{
		if( g_DstString[i].nDstParam == 0 )
			break;

		if( g_DstString[i].nDstParam == nDstParam )
			return g_DstString[i].nNameIndex;
	}

	return 0;
}

BOOL CWndMgr::IsDstRate(int nDstParam)
{
	return IsDst_Rate(nDstParam);
}

void CWndMgr::FormalizeChatString( CString & str )
{	// À¨: 2008-07-18
	// "¿ª ¿ÃøÎ«œø© ≈¨∂Û¿Ãæ∆Æ ∏Ì∑…æÓ ∆ƒΩÃ¿ª π´Ω√«œ¥¬ «‡¿ß∏¶ ∏∑±‚ ¿ß«— ∞Õ
	str.TrimLeft();
	str.TrimRight();

	while( str.GetLength() > 0 && str.GetAt( 0 ) == '\"' )
	{
		str.Delete( 0 );

		if(str.GetLength() > 1)
		{
			if( str.GetAt( str.GetLength() - 1 ) == '\"' )
				str.Delete( str.GetLength() - 1 );
			str.TrimLeft();
			str.TrimRight();
		}
	} 
}
void CWndMgr::CloseFuncApp()
{
#ifdef __SKILL_UI16
	CWndSkill_16* pSkillTree = (CWndSkill_16*)g_WndMng.GetWndBase( APP_SKILL4 );
#else //sun: 9¬˜ ¿¸Ω¬∞¸∑√ Clienet
	CWndSkillTreeEx* pSkillTree = (CWndSkillTreeEx*)g_WndMng.GetWndBase( APP_SKILL3 );
#endif //__SKILL_UI16
    if(pSkillTree)
	{
		pSkillTree->Destroy();
	}
	SAFE_DELETE(m_pWndCoupleMessage);
	CWndInventory* pWndInven = (CWndInventory*)g_WndMng.GetWndBase( APP_INVENTORY );
	if(pWndInven)
	{
		pWndInven->Destroy();
	}
	CWndParty* pWndParty = (CWndParty*)g_WndMng.GetWndBase( APP_PARTY );
	if(pWndParty)
	{
		pWndParty->Destroy();
	}

	CWndMapEx* pWndMap = (CWndMapEx*)g_WndMng.GetWndBase( APP_MAP_EX );
	if(pWndMap)
	{
		pWndMap->Destroy();
	}

	CWndHousing* pWndHousing = (CWndHousing*)g_WndMng.GetWndBase( APP_HOUSING );
	if(pWndHousing)
	{
		pWndHousing->Destroy();
	}
	CWndGuildHousing* pWndGH = (CWndGuildHousing*)g_WndMng.GetWndBase( APP_GH_FURNITURE_STORAGE );
	if(pWndGH)
	{
		pWndGH->Destroy();
	}

	CWndCharacter* pWndCHAR = (CWndCharacter*)g_WndMng.GetWndBase( APP_CHARACTER3 );
	if(pWndCHAR)
	{
		pWndCHAR->Destroy();
	}

	CWndTrade *pWndTrade = (CWndTrade *)g_WndMng.GetWndBase( APP_TRADE );
	if(pWndTrade)
	{
		pWndTrade->Destroy();
	}
	CWndQuest* pWndQuest = (CWndQuest*)g_WndMng.GetWndBase( APP_QUEST_EX_LIST );
	if(pWndQuest)
	{
		pWndQuest->Destroy();
	}

	CWndGuild* pWndGuild = (CWndGuild*)g_WndMng.GetWndBase( APP_GUILD );
	if( pWndGuild )
	{
		pWndGuild->Destroy();
	}
	
	CWndGuildVote* pWndGuildVote = (CWndGuildVote*)g_WndMng.GetWndBase( APP_GUILD_VOTE );
	if( pWndGuildVote )
	{
		pWndGuildVote->Destroy();
	}

	CWndBagEx* pWndBag	= (CWndBagEx*)g_WndMng.GetWndBase( APP_BAG_EX );
	if( pWndBag )
	{
		pWndBag->Destroy();
	}
	CWndCommItem* pWndCommItem	= (CWndCommItem*)g_WndMng.GetWndBase( APP_COMM_ITEM );
	if( pWndCommItem )
	{
		pWndCommItem->Destroy();
	}
	CWndMessenger* pWndMessenger = (CWndMessenger*)GetWndBase( APP_MESSENGER_ );
	if( pWndMessenger )
	{
		pWndMessenger->Destroy();
	}
	CWndCoupleManager* pWndCoupleManager = (CWndCoupleManager*)GetWndBase( APP_COUPLE_MAIN );
	if( pWndCoupleManager )
	{
		pWndCoupleManager->Destroy();
	}
}
void CWndMgr::CloseBoundWindow(void) // æ∆¿Ã≈€¿Ã ∞…∑¡ ¿÷∞≈≥™, æ∆¿Ã≈€¿ª ¡∂¿€«“ ∞°¥…º∫¿Ã ¿÷¥¬ ∏µÁ √¢¿ª ∞≠¡¶∑Œ ¥›¥¬¥Ÿ.
{
	SAFE_DELETE( m_pWndPiercing );				// ««æÓΩÃ
	SAFE_DELETE( m_pWndChangeWeapon );			// ¿Ø¥œ≈© ø˛∆˘, æÛ≈Õ∏⁄ ø˛∆˘ ∫Ø»Ø √¢
	SAFE_DELETE( m_pWndRemoveAttribute );		// º”º∫¡¶∑√ ¡¶∞≈
	SAFE_DELETE( m_pWndRemovePiercing );		// ««æÓΩÃ ø…º« ¡¶∞≈
	SAFE_DELETE( m_pWndRemoveJewel );			// æÛ≈Õ∏⁄ ø˛∆˘ ∫∏ºÆ ¡¶∞≈
	SAFE_DELETE( m_pWndSmeltMixJewel );			// ø¿∏ÆƒÆƒÒ, πÆΩ∫≈Ê «’º∫√¢
	SAFE_DELETE( m_pWndSmeltJewel );			// ∫∏ºÆ «’º∫ √¢
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
	SAFE_DELETE( m_pWndNewSmeltJewel );			// ∫∏ºÆ «’º∫ √¢(æ∆¿Ãƒ‹)
#endif
	SAFE_DELETE( m_pWndExtraction );			// ∫∏ºÆ √ﬂ√‚√¢
	SAFE_DELETE( m_pWndLvReqDown );				// ¬¯øÎ∑π∫ß «œ∂Ù «ÿ¡ˆ√¢
	SAFE_DELETE( m_pWndBlessingCancel );		// ø©Ω≈¿« √‡∫π √Îº“
	SAFE_DELETE( m_pWndBank );					// ¿∫«‡
	SAFE_DELETE( m_pWndGuildBank );				// ±ÊµÂ√¢∞Ì
	SAFE_DELETE( m_pWndPost );					// øÏ∆Ì
	SAFE_DELETE( m_pWndUpgradeBase );			// ¡¶∑√
#ifdef CARD_UPGRADE_SYSTEM
	SAFE_DELETE( m_pWndCardUpgrade );					// ƒ´µÂ æ˜±◊∑π¿ÃµÂ
#endif	// CARD_UPGRADE_SYSTEM
#ifdef COSTUME_UPGRADE_MIX
	SAFE_DELETE( m_pWndSmeltItemMix );						// æ∆¿Ã≈€«’º∫
#endif

	/*CWndElldinsJar* pWndElldinsJar = (CWndElldinsJar*)g_WndMng.GetWndBase( APP_ELLDINPOTION );
	if(pWndElldinsJar)
	{
		pWndElldinsJar->Destroy();
	}*/

	switch( _GetContentState(CT_SHOP_CART_17) )
	{
	case CS_VER1:
		SAFE_DELETE( m_pWndShop );
		break;

	case CS_VER2:
		SAFE_DELETE( m_pWndShop_Cart );
		break;

	default:
		FLASSERT( 0 );
		break;
	}

	SAFE_DELETE( m_pWndSmeltSafety );			// æ»¿¸ ¡¶∑√
	SAFE_DELETE( m_pWndSmeltSafetyConfirm );	// ¡¶∑√ »Æ¿Œ
	SAFE_DELETE( m_pWndEquipBindConfirm );		// ±Õº” »Æ¿Œ

	//_JIN_ITEM_CANCELLATION
	switch(_GetContentState(CT_CANCELLATION))
	{
	case CS_VER1:
		SAFE_DELETE( m_pWndCancellation );
		break;
	default:
		//FLASSERT( 0 );
		break;
	}

	switch(_GetContentState(CT_COSTUME_MIX))
	{
	case CS_VER1:
		SAFE_DELETE( m_pCostume_NPC );
		break;
	default:
		break;
	}


	// lenahyang
	// ø˘µÂ ¿Ãµø Ω√ ¥‹√‡ ΩΩ∑‘¿ª ∏∂øÏΩ∫ø° µÈ∞Ì ¿÷¿ª ∞ÊøÏ ≈©∑πΩ¨ πﬂª˝
	// ¥‹√‡ ΩΩ∑‘ √ ±‚»≠
	m_GlobalShortcut.Empty();
}

//-----------------------------------------------------------------------------
void CWndMgr::SetWarningCounter( int nWarningCounter )
{
	m_nWarningCounter = nWarningCounter;
}
//-----------------------------------------------------------------------------
void CWndMgr::SetWarning2Counter( int nWarning2Counter )
{
	m_nWarning2Counter = nWarning2Counter;
}
//-----------------------------------------------------------------------------
int CWndMgr::GetWarningCounter( void ) const
{
	return m_nWarningCounter;
}
//-----------------------------------------------------------------------------
int CWndMgr::GetWarning2Counter( void ) const
{
	return m_nWarning2Counter;
}
//-----------------------------------------------------------------------------
CTimer& CWndMgr::GetWarningTimer( void )
{
	return m_timerWarning;
}
//-----------------------------------------------------------------------------
CTimer& CWndMgr::GetWarning2Timer( void )
{
	return m_timerWarning2;
}
//-----------------------------------------------------------------------------
CTimer& CWndMgr::GetShortcutWarningTimer( void )
{
	return m_timerShortcutWarning;
}
//-----------------------------------------------------------------------------
CTimer& CWndMgr::GetBanningTimer( void )
{
	return m_timerBanning;
}
//-----------------------------------------------------------------------------
BOOL CWndMgr::IsShortcutCommand( void ) const
{
	return m_bShortcutCommand;
}
//-----------------------------------------------------------------------------

void CWndMgr::RegisterCustomChild( CWndBase*& pWndChild )
{
	//¿ÃπÃ º±æµ«æÓ¿÷¥¬ ∏‚πˆ∫Øºˆ∏¶ ª˝º∫Ω√∞£ø° √ ±‚»≠π◊ µÓ∑œ¿ª ¥„¥Á 
	//¡÷¿«!! CWndWorld√≥∑≥ µÓ∑œ¿∫ µ«æÓ¿÷¿∏µ≈, ¥Ÿ∏•∞˜ø°º≠ ª˝º∫ ªË¡¶∏¶ «œ¥¬∞ÊøÏ µÓ∑œ¿Ã µ«æÓº≠¥¬ æ∆¥œµ»¥Ÿ.
	//øπø‹∑Œ ¥Ÿ∏•∞˜ø°º≠ ªË¡¶∏¶ «“∂ß ∏‚πˆ∫Øºˆ √ ±‚»≠∏¶ √•¿”¡¯¥Ÿ¥¬ ∞°¡§«œø° µÓ∑œ∞°¥…

	vecWndIter iter = std::find( m_vecChildWnds.begin(), m_vecChildWnds.end(), &pWndChild );
	if( iter != m_vecChildWnds.end() )
	{
		//¿ÃπÃ µÓ∑œµ» ChildWnd, ∞™∏∏ πŸ≤€¥Ÿ
		CWndBase** ppWnd = *iter;
		ppWnd = &pWndChild;
	}
	else
	{
		pWndChild = NULL;		//√ ±‚»≠
		m_vecChildWnds.push_back( &pWndChild );
	}
}

void CWndMgr::DeleteAllCustomChild( )
{
	int nCount = 0;

	for( vecWndIter iter = m_vecChildWnds.begin(); iter != m_vecChildWnds.end(); ++iter )
	{
		CWndBase** ppWnd = *iter;
		SAFE_DELETE( (*ppWnd) );

		++nCount;
	}
}

void CWndMgr::PreDelete( CWndBase* pBase )
{
	//AutoFree∞° ¿÷æÓº≠ -_-; ø°»ø
	//AutoFree¿Ã¿¸ø° «ÿæﬂ«“ ¿œ¿ª «—¥Ÿ.
	for( vecWndIter iter = m_vecChildWnds.begin(); iter != m_vecChildWnds.end(); ++iter )
	{
		CWndBase** ppWnd = *iter;
		if( *ppWnd == pBase )
		{
			*ppWnd = NULL;
			return;
		}
	}
}

BOOL CWndMgr::IsMakingRevivalWnd( )
{
	BOOL bOK = TRUE;

	//case1 : ¿Ã∫•∆Æ æ∆∑π≥™ø°º≠¥¬ ±›¡ˆ
	if( g_pPlayer )
	{
		CWorld* pWorld = g_pPlayer->GetWorld( );
		if( pWorld )
		{
			bOK = pWorld->IsEventArena( );
			bOK = !bOK;
		}
	}

	return bOK;
}

BOOL CWndMgr::EnableHUD( BOOL bEnable )
{
	if( bEnable )
	{
		if( NULL == m_pWndTargetHUD )
		{
			m_pWndTargetHUD = new CWndTargetInfo_HUD;
			m_pWndTargetHUD->Initialize( this );
		}
	
		if( m_pWndTargetHUD )
			m_pWndTargetHUD->SetVisible( TRUE );

		if( m_pWndWorld )
			m_pWndWorld->ShowMainMenu( TRUE );

		if( m_pWndTaskBar )
			m_pWndTaskBar->AllVisible( TRUE );
	}
	else
	{
		if( m_pWndTargetHUD )
			m_pWndTargetHUD->SetVisible( FALSE );

		if( m_pWndWorld )
			m_pWndWorld->ShowMainMenu( FALSE );

		if( m_pWndTaskBar )
			m_pWndTaskBar->AllVisible( FALSE );
	}

	return TRUE;
}

BOOL CWndMgr::IsConsignmentMarket_Register( )
{
	if( NULL == m_pWndProxyMarket )
		return FALSE;

	return m_pWndProxyMarket->IsShowWndId( APP_TRADE_REGISTER );
}

BOOL CWndMgr::DoModalConsignmentMarket_Register( DWORD dwItemId )
{
	if( NULL == m_pWndProxyMarket )
		return FALSE;

	m_pWndProxyMarket->DoModalRegister( dwItemId );

	return TRUE;
}

//global function
int _GetHeadQuest( const int nQuest )
{
	if( _GetContentState( CT_RENDER_QUESTEMOTICON ) == CS_VER1 )
		return -1;

	QuestProp* pProp = prj.m_aPropQuest.GetAt( nQuest );
	int nMaxLoop = 0;

	while( pProp )	// ∆Æ∏Æ±∏¡∂¿Ã±‚∂ßπÆø° root∏¶ √£æ∆æﬂ ∫–∑˘∏¶ æÀºˆ¿÷¥Ÿ.
	{
		switch( pProp->m_nHeadQuest )
		{
		case QUEST_KIND_SCENARIO:	
		case QUEST_KIND_EVENT:		
		case QUEST_KIND_NORMAL:		
		case QUEST_KIND_REQUEST:	return pProp->m_nHeadQuest;	
									
		default: 
			if( pProp->m_nHeadQuest <= 0 )	//data error
				return -1;

			pProp = prj.m_aPropQuest.GetAt( pProp->m_nHeadQuest );
			break;
		}

		if( ++nMaxLoop > 20 )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "FUCK! Cannot find headquest!!!! questindex:%d" ), pProp->m_wId );
			pProp = NULL;
		}
	}

	return -1;
}

int _CalcQuestEmotionOffset( const int nHeadQuest, const int nOffset )
{
	//ƒ˘Ω∫∆Æ ¿Ã∏∆ºƒ‹ texture set¿« offset∞™¿Ã¥Ÿ.

	int nRst = 0;
	switch( nHeadQuest )
	{
	case QUEST_KIND_SCENARIO:	nRst += nOffset; 
	case QUEST_KIND_EVENT:		nRst += nOffset; 
	case QUEST_KIND_NORMAL:		//ok
	case QUEST_KIND_REQUEST:	//ok
								break;
	default: 
		break;
	}

	return nRst;
}

void CWndMgr::AlighWindow( CRect rcOld, CRect rcNew )
{
	for(int i = 0; i < m_wndOrder.GetSize(); i++ )
	{
		CWndBase* pWndBase = (CWndBase*) m_wndOrder.GetAt( i );
		CRect rcWnd = pWndBase->GetWindowRect( TRUE );
		CPoint point = rcWnd.TopLeft();
		if( pWndBase != this && pWndBase != m_pWndTaskBar )
		{
			if( pWndBase->m_nWinSize == WSIZE_MAX )
			{
				pWndBase->SetWndRect( rcNew );
			}
			else
			{
				if( rcWnd.left   < rcNew.left   ) point.x = rcNew.left;
				if( rcWnd.top    < rcNew.top    ) point.y = rcNew.top;
				if( rcWnd.right  > rcNew.right  ) point.x = rcNew.right - rcWnd.Width();
				if( rcWnd.bottom > rcNew.bottom ) point.y = rcNew.bottom - rcWnd.Height();

				if( rcWnd.left   == rcOld.left   ) point.x = rcNew.left;
				if( rcWnd.top    == rcOld.top    ) point.y = rcNew.top;
				if( rcWnd.right  == rcOld.right  ) point.x = rcNew.right - rcWnd.Width();
				if( rcWnd.bottom == rcOld.bottom ) point.y = rcNew.bottom - rcWnd.Height();

				pWndBase->Move( point );
			}
		}
	}
}


//---------------------------------------------------------------------------------------------
// ∞¯øÎ ∞≥ºˆ ¿‘∑¬ √¢ ª˝º∫«œ±‚
// param	: void
// return	: º∫∞¯ / Ω«∆–
//---------------------------------------------------------------------------------------------
CWndCommonInputCount* CWndMgr::GetWndCommonInputCount( CWndBase* pParentWnd )
{
	SAFE_DELETE( m_pWndCommonInputCount );

	m_pWndCommonInputCount = new CWndCommonInputCount;
	FLERROR_ASSERT_LOG_RETURN( m_pWndCommonInputCount, NULL , _T("[¿©µµøÏ ∏ﬁ¥œ¡Æ] ∞¯øÎ ∞≥ºˆ ¿‘∑¬ √¢ ª˝º∫¿ª Ω«∆–«œø¥Ω¿¥œ¥Ÿ.") );

	m_pWndCommonInputCount->Initialize( pParentWnd );

	return m_pWndCommonInputCount;
}

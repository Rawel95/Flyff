#include "stdafx.h"
#include "defineText.h"
#include "resdata.h"
#include "Project.h"
#include "ProjectCmn.h"

#include "Party.h"
extern	CPartyMng	g_PartyMng;

#ifdef __WORLDSERVER
#include "DialogMsg.h"
#include "../WorldServer/user.h"
#include "spevent.h"
#include "../WorldServer/FLCC_Reward.h"
#include "../WorldServer/slord.h"
#include "PCBang.h"
#include "GuildHouse.h"
#include "../WorldServer/CampusHelper.h"
#include "../WorldServer/FLCC_Reward.h"
#include "../worldserver/FLMadrigalGift.h"
#include "../_Common/SeasonEffect.h"
#endif	// __WORLDSERVER

#ifdef __CLIENT			
#include "Dialogmsg.h"
#include "defineSound.h"
#endif



LPSKILL CMover::GetSkill( int /*nType*/, int nIdx )
{
	// nType == 0¿Ã∏È ¡˜æ˜ Ω∫≈≥
	// nType == 1¿Ã∏È ∂Û¿Ãº±Ω∫ Ω∫≈≥ 
	if( nIdx >= MAX_SKILL_JOB )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s nId = %d π¸¿ß∏¶ π˛æÓ≥≤" ), m_szName, nIdx );
		nIdx = 0;
	}
	return &m_aJobSkill[ nIdx ];
}
LPSKILL CMover::GetSkill( DWORD dwSkill )
{
	for( int i = 0; i < MAX_SKILL_JOB; i++ )	
	{
		if( m_aJobSkill[i].dwSkill == dwSkill )
			return &m_aJobSkill[i];
	}
	return NULL;
}

int		CMover::GetSkillIdx( DWORD dwSkill )
{
	for( int i = 0; i < MAX_SKILL_JOB; i++ )	
	{
		if( m_aJobSkill[i].dwSkill == dwSkill )
			return i;
	}
	return -1;
}


BOOL CMover::CheckSkill( DWORD dwSkill )
{
	SkillProp* pSkillProp = prj.GetSkillProp( dwSkill );
	
	if( pSkillProp == NULL )
		return FALSE;
//sun: 10¬˜ ¿¸Ω¬Ω√Ω∫≈€	Neuz, World, Trans

	if( IsMaster() == FALSE && IsHero() == FALSE && IsLegendHero() == FALSE && GetLevel() < (int)( pSkillProp->dwReqDisLV ) )
	{
		return FALSE;
	}

	if( pSkillProp->dwReqSkill1 != NULL_ID )
	{
		LPSKILL pSkillBuf = GetSkill( pSkillProp->dwReqSkill1 );
		if( pSkillBuf )
		{
			if( pSkillBuf->dwLevel < pSkillProp->dwReqSkillLevel1 )
			{
				return FALSE;
			}
		}
	}
	
	if( pSkillProp->dwReqSkill2 != NULL_ID )
	{
		LPSKILL pSkillBuf = GetSkill( pSkillProp->dwReqSkill2 );
		if( pSkillBuf )
		{
			if( pSkillBuf->dwLevel < pSkillProp->dwReqSkillLevel2 )
			{
				return FALSE;
			}				
		}
	}	
	
	return TRUE;
}

bool	CMover::isLearnSkill( const int job_skill_array_index ) const
{
	if( job_skill_array_index < 0 || job_skill_array_index >= MAX_SKILL_JOB ) {
		return false;
	}

	const SkillProp* pSkillProp	= prj.GetSkillProp( m_aJobSkill[job_skill_array_index].dwSkill );
	if( pSkillProp == NULL ) {
		return false;
	}

	return m_aJobSkill[job_skill_array_index].dwLevel > 0;
}


// ƒ…∏Ø≈Õ∞° ∑π∫ßæ˜Ω√ Ω∫≈≥¡ﬂø° ƒ…∏Ø≈Õ∑π∫ß∂´Ω√ »∞º∫»≠ æ»µ»∞ÕµÈ »∞º∫ø©∫Œ √º≈©«œø© ¡§∫∏√‚∑¬
#ifdef __CLIENT
void CMover::PutLvUpSkillName_1( DWORD dwLevel )
{
	for( int i = 0; i < MAX_SKILL_JOB; i++ )	
	{
		if( m_aJobSkill[i].dwSkill == NULL_ID ) 
			continue;
		
		SkillProp* pSkillProp = prj.GetSkillProp( m_aJobSkill[i].dwSkill );
		
		if( pSkillProp && dwLevel == pSkillProp->dwReqDisLV )
		{
			if( pSkillProp->dwReqSkill1 != NULL_ID )
			{
				LPSKILL pSkill = GetSkill(pSkillProp->dwReqSkill1);

				if( pSkillProp->dwReqSkillLevel1 > pSkill->dwLevel )
					continue;
			}
			if( pSkillProp->dwReqSkill2 != NULL_ID )
			{
				LPSKILL pSkill = GetSkill(pSkillProp->dwReqSkill2);
				
				if( pSkillProp->dwReqSkillLevel2 > pSkill->dwLevel )
					continue;
			}

			CString str;
			str.Format( prj.GetText( TID_GAME_REAPSKILL ), pSkillProp->szName );
			g_WndMng.PutString( (LPCTSTR)str, NULL, prj.GetTextColor( TID_GAME_REAPSKILL ) );
		}
	}
}		
#else
void CMover::PutLvUpSkillName_1( DWORD /*dwLevel*/ )
{
}
#endif //__CLIENT

BOOL IsSkillParent( CMover* pMover, DWORD dwSkill, DWORD dwChildSkill )
{
	LPSKILL pSkill = pMover->GetSkill( dwSkill );
	if( pSkill == NULL )
		return FALSE;

	SkillProp* pSkillProp = prj.GetSkillProp( dwSkill );
	if( pSkillProp == NULL )
		return FALSE;

	if( pSkillProp->dwReqSkill1 == dwChildSkill || pSkillProp->dwReqSkill2 == dwChildSkill )
		return TRUE;

	return FALSE;
}

BOOL IsActive( CMover* pMover, DWORD dwSkill )
{
	LPSKILL pSkill = pMover->GetSkill( dwSkill );
	if( pSkill == NULL )
		return FALSE;

	SkillProp* pSkillProp = prj.GetSkillProp( dwSkill );
	if( pSkillProp == NULL )
		return FALSE;

	if( pSkillProp->dwReqSkill1 != NULL_ID )
	{
		LPSKILL pSkill1;
		SkillProp* pSkillProp1;

		pSkill1		= pMover->GetSkill( pSkillProp->dwReqSkill1 );
		if( pSkill1 == NULL )
			return FALSE;

		pSkillProp1 = prj.GetSkillProp( pSkill1->dwSkill );
		if( pSkillProp1 == NULL )
			return FALSE;

//sun: 10¬˜ ¿¸Ω¬Ω√Ω∫≈€	Neuz, World, Trans

		if( (int)( pSkillProp1->dwReqDisLV ) > pMover->GetLevel() && pMover->IsMaster() == FALSE && pMover->IsHero() == FALSE && pMover->IsLegendHero() == FALSE )
		{
			return FALSE;
		}

		if( pSkillProp->dwReqSkillLevel1 != pSkill1->dwLevel )
			return FALSE;
	}

	if( pSkillProp->dwReqSkill2 != NULL_ID )
	{
		LPSKILL pSkill1;
		SkillProp* pSkillProp1;
		
		pSkill1		= pMover->GetSkill( pSkillProp->dwReqSkill2 );
		if( pSkill1 == NULL )
			return FALSE;
	
		pSkillProp1 = prj.GetSkillProp( pSkill1->dwSkill );
		if( pSkillProp1 == NULL )
			return FALSE;

//sun: 10¬˜ ¿¸Ω¬Ω√Ω∫≈€	Neuz, World, Trans
		if( (int)( pSkillProp1->dwReqDisLV ) > pMover->GetLevel() && pMover->IsMaster() == FALSE && pMover->IsHero() == FALSE && pMover->IsLegendHero() == FALSE )
		{
			return FALSE;
		}

		if( pSkillProp->dwReqSkillLevel2 != pSkill1->dwLevel )
			return FALSE;		
	}

//sun: 10¬˜ ¿¸Ω¬Ω√Ω∫≈€	Neuz, World, Trans
	if( (int)( pSkillProp->dwReqDisLV ) > pMover->GetLevel() && pMover->IsMaster() == FALSE && pMover->IsHero() == FALSE && pMover->IsLegendHero() == FALSE )
	{
		return FALSE;
	}

	return TRUE;
}


// Ω∫≈≥ ∑π∫ßæ˜Ω√ ƒ…∏Ø≈Õ ∑π∫ß∂´Ω√ æ»≥™ø‘¥¯ Ω∫≈≥µÈ ∞Àªˆ«œø© ¡§∫∏ √‚∑¬
#ifdef __CLIENT
void CMover::PutLvUpSkillName_2( DWORD dwSkill )
{
	LPSKILL pSkill = GetSkill( dwSkill );
	SkillProp* pSkillProp = prj.GetSkillProp( dwSkill );
	
	//«ˆ¿Á »∞º∫»≠ µ«æÓ¿÷¥¬≥ƒ?
	//≥™∏¶ ∞Æ∞Ì¿÷¥¬ Ω∫≈≥¿ª ∞ÀªÁ«œø© ¡∂∞«ø° ∏¬¿∏∏È ±◊Ω∫≈≥¿ª »∞º∫»≠ «—¥Ÿ.
	for( int i = 0; i < MAX_SKILL_JOB; i++ )	
	{
		if( m_aJobSkill[i].dwSkill == NULL_ID ) 
			continue;

		if( IsSkillParent( this, m_aJobSkill[i].dwSkill, dwSkill ) )
		{
			if( IsActive( this, m_aJobSkill[i].dwSkill ) )
			{
				SkillProp* pSkillProp1;
				pSkillProp1 = prj.GetSkillProp( m_aJobSkill[i].dwSkill );

				CString str;
				str.Format( prj.GetText( TID_GAME_REAPSKILL ), pSkillProp1->szName );
				g_WndMng.PutString( (LPCTSTR)str, NULL, prj.GetTextColor( TID_GAME_REAPSKILL ) );	
			}
		}
	}
}
#else
void CMover::PutLvUpSkillName_2( DWORD /*dwSkill*/ )
{

}
#endif // CLIENT

#ifdef __WORLDSERVER
/// ∞≈∑°, ¡◊¿Ω, ∞≥¿ŒªÛ¡°¡ﬂ, ¿¸≈ı¡ﬂ FALSE
int CMover::GetSummonState()
{
	// ∞≈∑°
	if( m_vtInfo.GetOther() != NULL )
		return 1;
	  // √¢∞Ì
	if( m_bBank )
		return 1;
	  // ±ÊµÂ√¢∞Ì
	if( m_bGuildBank )
		return 1;

	// ¡◊¿Ω
	if( IsDie() )
		return 2;

	// ∞≥¿ŒªÛ¡°¡ﬂ
	if( m_vtInfo.VendorIsVendor() || m_vtInfo.IsVendorOpen() )
		return 3;

	// ¿¸≈ı¡ﬂ
	if( IsAttackMode() )
		return 4;
	
	// ∫Ò«‡¡ﬂ
	if( ( IsFly() ) )
		return 5;


//sun: 8, // 8¬˜ µ‡æÛ 061226 ma
	if( m_nDuel )
		return 6;

	return 0;
}
#endif //


#ifdef __CLIENT
LPCTSTR CMover::GetJobString()
{
	if( m_nJob != -1 )
		return prj.m_aJob[ m_nJob ].szName;
	return _T( "" );
}
#endif	// __CLIENT

BOOL  CMover::IsJobType( DWORD dwJobType ) 
{ 
	if( IsExpert() )
	{
		if( dwJobType == JTYPE_EXPERT ) 
			return TRUE;
		return FALSE;
	}
	return JTYPE_BASE == dwJobType;
}
BOOL CMover::IsBaseJob()
{
	return prj.m_aJob[ m_nJob ].dwJobType == JTYPE_BASE;
}
BOOL CMover::IsExpert()
{
	return prj.m_aJob[ m_nJob ].dwJobType == JTYPE_EXPERT;
}
BOOL CMover::IsPro()
{
	return prj.m_aJob[ m_nJob ].dwJobType == JTYPE_PRO;
}

//sun: 10¬˜ ¿¸Ω¬Ω√Ω∫≈€	Neuz, World, Trans
BYTE	CMover::GetLegendChar()
{
	if(IsMaster())
		return LEGEND_CLASS_MASTER;
	else if(IsHero())
		return	LEGEND_CLASS_HERO;
	else if( IsLegendHero() == TRUE )
	{
		return LEGEND_CLASS_LEGEND_HERO;
	}
	else
		return LEGEND_CLASS_NORMAL;
}

BOOL	CMover::IsMaster()
{
	return prj.m_aJob[ m_nJob ].dwJobType == 5;//temp JTYPE_MASTER;
}

BOOL	CMover::IsHero()
{
	return prj.m_aJob[ m_nJob ].dwJobType == 6;//temp JTYPE_HERO;
}

BOOL	CMover::IsLegendClass()
{
	DWORD dwJobType = prj.m_aJob[ m_nJob ].dwJobType;
	if( dwJobType != NULL_ID && dwJobType >= JTYPE_HERO )
	{
		return TRUE;
	}

	return FALSE;
}


BOOL CMover::IsLegendHero()
{
	return prj.m_aJob[ m_nJob ].dwJobType == JTYPE_LEGEND_HERO;
}


BOOL CMover::IsInteriorityJob( int nJob )
{
	if( nJob == JOB_VAGRANT || nJob == m_nJob )
		return TRUE;

	if( IsPro() && JOB_VAGRANT < nJob && nJob < MAX_EXPERT )
	{
		if( nJob * 2 + 4 == m_nJob || nJob * 2 + 5 == m_nJob )
		{
			return TRUE;
		}
	}
//sun: 10¬˜ ¿¸Ω¬Ω√Ω∫≈€	Neuz, World, Trans
	if( IsMaster()  )	// ∑π∫ß¿Ã æ»µ«∏È ∏¯¬¸.				
	{
		if( nJob < MAX_EXPERT )
		{
			if( nJob * 2 + 14 == m_nJob || nJob * 2 + 15 == m_nJob )
				return TRUE;
		}
		else if( nJob < MAX_PROFESSIONAL )
		{
			if( nJob + 10 == m_nJob )
				return TRUE;
		}
	}
	if( IsHero()  )	// ∑π∫ß¿Ã æ»µ«∏È ∏¯¬¸.				
	{
		if( nJob < MAX_EXPERT )
		{
			if( nJob * 2 + 22 == m_nJob || nJob * 2 + 23 == m_nJob )
				return TRUE;
		}
		else if( nJob < MAX_PROFESSIONAL )
		{
			if( nJob + 18 == m_nJob )
				return TRUE;
		}
		else if( nJob < MAX_MASTER )	// Hero¥¬ ∏∂Ω∫≈Õ π´±‚∏¶ ¿Â¬¯«“ ºˆ ¿÷¥Ÿ.
		{
			if( nJob + 8 == m_nJob )
				return TRUE;
		}
	}

	if( IsLegendHero() == TRUE )	// ∑π∫ß¿Ã æ»µ«∏È ∏¯¬¸.				
	{
		if( nJob < MAX_EXPERT )
		{
			if( nJob * 2 + 30 == m_nJob || nJob * 2 + 31 == m_nJob )
			{
				return TRUE;
			}
		}
		else if( nJob < MAX_PROFESSIONAL )
		{
			if( nJob + 26 == m_nJob )
			{
				return TRUE;
			}
		}
		else if( nJob < MAX_MASTER )	// NewHero¥¬ ∏∂Ω∫≈Õ π´±‚∏¶ ¿Â¬¯«“ ºˆ ¿÷¥Ÿ.
		{
			if( nJob + 16 == m_nJob )
			{
				return TRUE;
			}
		}
		else if( nJob < MAX_HERO )	// NewHero¥¬ »˜æÓ∑Œ π´±‚∏¶ ¿Â¬¯«“ ºˆ ¿÷¥Ÿ.
		{
			if( nJob + 8 == m_nJob )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

// Ω∫≈≥ºˆ 3(±‚∫ª)14(¿ÕΩ∫∆€∆Æ)7(«¡∑Œ∆‰º≈≥Œ)
// ¡˜æ˜ 10, ¿ÕΩ∫∆€∆Æ 40. «¡∑Œ 30
BOOL  CMover::SetExpert( int nJob )
{
	if( m_nJob == nJob )
		return FALSE;
	m_nJob = nJob;

	int nNum = 3; // 3¿∫ ±‚∫ª Ω∫≈≥¿« ∞πºˆ¥Ÿ. 3 ¿ÃªÛ∫Œ≈Õ ¿ÕΩ∫∆€∆Æ Ω∫≈≥¿Ã √ﬂ∞°µ»¥Ÿ¥¬ ¿«πÃ 
	LPSKILL lpSkill;
	SkillProp** apSkillProp = prj.m_aJobSkill[ m_nJob ];
	int nJobNum = prj.m_aJobSkillNum[ m_nJob ];
	for( int i = 0; i < nJobNum; i++ )
	{
		SkillProp* pSkillProp = apSkillProp[ i ];
		lpSkill = &m_aJobSkill[ nNum++ ];
		lpSkill->dwSkill = pSkillProp->dwID;
	}
	return TRUE;
}

int   CMover::GetJob()
{
	return m_nJob; 
}

int   CMover::GetExpPercent()
{
	int nExpPercent = 0;

	nExpPercent = (int)( GetExp1() * 10000 / GetMaxExp1() );

	return nExpPercent; 
}


int   CMover::GetRemainGP()
{
	return m_Stat.GetRemainStatPoint();
}

// ∏Ì¡ﬂ∑¸ «œ∂Ù ªÛ≈¬¥¬ ¿Ã≈¢¿Ã∫•∆ÆøÕ ªÛ∞¸æ¯¿Ã ø√ºˆ ¿÷±‚∂ßπÆø° GetParm()¿ª ø©±‚º≠ √÷¡æ ∞ËªÍ«ÿæﬂ«—¥Ÿ.
int	CMover::GetAdjHitRate( void )
{
	return GetParam( DST_ADJ_HITRATE, m_nAdjHitRate );
}

// »∏««¿≤ 
int CMover::GetParrying()
{
	if( IsPlayer() )
		return	( (int)( (GetDex() * 0.5f) + GetParam( DST_PARRY, m_nAdjParry ) ) );
	else
		return GetProp()->dwER;
}


void CMover::UpdateParam()
{
	if( IsPlayer() ) 
	{
		SumEquipDefenseAbility( &m_nDefenseMin, &m_nDefenseMax ); 
		SumEquipAdjValue( &m_nAdjHitRate, &m_nAdjParry );
	}
	else
	{
		m_nDefenseMin = m_nDefenseMax = GetNaturalArmor();
		m_nAdjHitRate = 0;
		m_nAdjParry = 0;
	}
}
// æ∆¿Ã≈€¿« º∫¥…¿ª æÚ¥¬¥Ÿ. 
int CMover::GetItemAbility( int nItem )
{
	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( nItem );
	int nAbilityMin = GetParam( DST_ABILITY_MIN, pItemProp->dwAbilityMin );
	int nAbilityMax = GetParam( DST_ABILITY_MAX, pItemProp->dwAbilityMax );
	if( nAbilityMax == nAbilityMin )
		return nAbilityMin;
	return nAbilityMin + ( xRand() % (nAbilityMax - nAbilityMin ) );
}
// æ∆¿Ã≈€¿« √÷º“ º∫¥…¿ª æÚ¥¬¥Ÿ.
int CMover::GetItemAbilityMin( int nItem )
{
	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( nItem );
	int nAbilityMin = GetParam( DST_ABILITY_MIN, pItemProp->dwAbilityMin );
	return nAbilityMin;
}
// æ∆¿Ã≈€¿« √÷¥Î º∫¥…¿ª æÚ¥¬¥Ÿ.
int CMover::GetItemAbilityMax( int nItem )
{
	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( nItem );
	int nAbilityMax = GetParam( DST_ABILITY_MAX, pItemProp->dwAbilityMax );
	return nAbilityMax;
}

// »˜∆Æ∆˜¿Œ∆Æ ¡ı∞®
void CMover::IncHitPoint(int nVal) 
{ 
	SetPointParam( DST_HP, m_nHitPoint + nVal );
}
// ∏∂≥™∆˜¿Œ∆Æ ¡ı∞®
void CMover::IncManaPoint(int nVal) 
{ 
#ifdef __WORLDSERVER 			
	if( IsSMMode( SM_MAINTAIN_MP ) && nVal < 0 )
		return;
#endif // __WORLDSERVER
	SetPointParam( DST_MP, m_nManaPoint + nVal );
}

void CMover::IncFatiguePoint(int nVal) 
{ 
#ifdef __WORLDSERVER
	if( IsSMMode( SM_MAINTAIN_FP ) && nVal < 0 )
		return;
#endif // __WORLDSERVER
	SetPointParam( DST_FP, m_nFatiguePoint + nVal );
}

// µ‡æÛ»ƒ¿« ∞·∞˙ √≥∏Æ.
// this∞° Ω¬¿⁄∞° µ»¥Ÿ.
// pLose¥¬ ∆–πË¿⁄.

#ifdef __WORLDSERVER

void CMover::ChangeFame( CMover *pLose )
{
	int nVal = 0;
	float fVal = 0;
	int nGap = 0;

	nGap = pLose->GetLevel() - GetLevel();		// ∆–¿⁄øÕ Ω¬¿⁄¿« ∑π∫ß¬˜¿Ã

	if( nGap >= 80 )	    { nVal = 100; fVal = 15.0f; }	// ∆–¿⁄∞° ∑π∫ß¿Ã »Œ ≥Ùæ“¥Ÿ.
	else if( nGap >= 30 )	{ nVal = 50;  fVal = 10.0f; }
	else if( nGap >= 20 )	{ nVal = 30;  fVal = 8.0f;  }
	else if( nGap >= 10 )	{ nVal = 10;  fVal = 6.0f;  }
	else if( nGap <= -80 )	{ nVal = 1;   fVal = 1.0f;  }	// ∆–¿⁄∞° ∑π∫ß¿Ã »Œ ≥∑æ“¥Ÿ.
	else if( nGap <= -30 )	{ nVal = 2;   fVal = 2.0f;  }
	else if( nGap <= -20 )	{ nVal = 3;   fVal = 3.0f;  }
	else if( nGap <= -10 )	{ nVal = 4;   fVal = 4.0f;  }
	else                    { nVal = 5;   fVal = 5.0f;  }	// 9 ~ -9


	if( nVal )		// Ω¬¿⁄ø°∞‘ ∏Ìº∫ƒ° ¡‹
	{	
		if( this->IsFly() && pLose->IsFly() )	// ∞¯¡ﬂ µ‡æÛ¿« ∞ÊøÏø°¥¬ ∆–πËΩ√ ªÛΩ««œ¥¬ ∏Ìº∫ ∆˜¿Œ∆Æ¥¬ µø¿œ«œ¡ˆ∏∏, Ω¬∏Æ Ω√ »πµÊ«œ¥¬ ∏Ìº∫∆˜¿Œ∆Æ¥¬ ¡ˆªÛ ¿¸≈ıø° 2πË∑Œ «—¥Ÿ
			nVal *= 2;
			
		m_nFame += nVal;	
		g_xWSUserManager->AddSetFame( this, m_nFame );
		((FLWSUser*)this)->AddDefinedText( TID_GAME_GETFAMEPOINT, "%d", nVal );	// xxx ∏Ìº∫ ∆˜¿Œ∆Æ∏¶ æÚæ˙¥Ÿ
//sun: 13, ¥ﬁ¿Œ
		((FLWSUser*)this)->CheckHonorStat();
		((FLWSUser*)this)->AddHonorListAck();
		g_xWSUserManager->AddHonorTitleChange( this, m_nHonor);
	}
	
	if( fVal )	// ∆–¿⁄¿« ∏Ìº∫ƒ°(∆€ºæ∆Æ)∏¶ ±¿Ω.
	{
		int		nDecVal;
		fVal = fVal / 100.0f;	// ∫Ò¿≤∑Œ »ØªÍ.
		nDecVal = (int)(pLose->m_nFame * fVal);
		if( nDecVal <= 0 )	nDecVal = 1;
		pLose->m_nFame -= nDecVal;
		if( pLose->m_nFame < 0 )
			pLose->m_nFame = 0;

		g_xWSUserManager->AddSetFame( pLose, pLose->m_nFame );
		((FLWSUser*)pLose)->AddDefinedText( TID_GAME_DECFAMEPOINT, "%d", nDecVal );	// xxx ∏Ìº∫ ∆˜¿Œ∆Æ∞° ∞®º“.
		g_dpDBClient.SendLogPkPvp( this, pLose, nVal, 'D' );
//sun: 13, ¥ﬁ¿Œ
		((FLWSUser*)pLose)->CheckHonorStat();
		((FLWSUser*)pLose)->AddHonorListAck();
		g_xWSUserManager->AddHonorTitleChange( pLose, pLose->m_nHonor);
	}
}
#endif // __WORLDSERVER

int	CMover::DuelResult( CMover *pLose )
{
#ifdef __WORLDSERVER
	if( pLose->m_nDuel == 1 )
	{
		if( pLose->GetId() != m_idDuelOther )
			return 0;
	}

	// Ω¬¿⁄¥¬ ∑π∫ß¬˜ø°µ˚∂Û ∞Ê«Ëƒ° ªÛΩ¬
	// ∆–¿⁄¥¬ ∑π∫ß¬˜ø°µ˚∂Û ∞Ê«Ëƒ°% «œ∂Ù
	if( pLose->m_nDuel == 1 )	// ∞≥¿Œ PvP
	{
		ChangeFame( pLose );
		
		pLose->m_nDead = PROCESS_COUNT * 2;		// ¡æ∑· »ƒ 2√ ∞£¿∫ π´¿˚
		pLose->RemoveAttackerBuffs( pLose->m_idDuelOther );
		m_nDead = PROCESS_COUNT * 2;		// ¡æ∑· »ƒ 2√ ∞£¿∫ π´¿˚
		RemoveAttackerBuffs( m_idDuelOther );
	}
	else
	// ±ÿ¥‹ µ‡æÛ.
	if( pLose->m_nDuel == 2 )		// ±ÿ¥‹ µ‡æÛ¡ﬂ¿œ∂© ±ÿ¥‹¿Â≥¢∏Æ¿« ∑π∫ß¿ª ∫Ò±≥.
	{
		int nVal = 0;
		float fVal = 0;
		int nGap = 0;

		CParty *pLoseParty = g_PartyMng.GetParty( pLose->m_idparty );
		if( pLoseParty == NULL || pLoseParty->IsLeader( pLose->m_idPlayer ) == FALSE )		// ∆ƒ∆º∞° æ¯∞≈≥™ ¡◊¿∫≥—¿Ã ¥Î¿Â¿Ã æ∆¥œ∏È ∞¡ ∏Æ≈œ.
			return 0;
		
		CParty *pParty = g_PartyMng.GetParty( m_idparty );		// ¿Ã±‰√¯ ∆ƒ∆º.
		if( pParty == NULL )
			return 0;

		CMover* pLeader	= pParty->GetLeader();
		if( IsValidObj( pLeader ) )
			nGap	= pLose->GetLevel() - pLeader->GetLevel();
		else
			nGap	= pLose->GetLevel() - GetLevel();
		
		if( nGap >= 80 )	{ nVal = 100; fVal = 15.0f; }		// ∆–¿⁄∞° ∑π∫ß¿Ã »Œ ≥Ùæ“¥Ÿ.
		else if( nGap >= 50 )	{ nVal = 50; fVal = 10.0f; }
		else if( nGap >= 20 )	{ nVal = 30; fVal = 8.0f; }
		else if( nGap >= 10 )	{ nVal = 10; fVal = 6.0f; }
		else if( nGap <= -80 )	{ nVal = 1; fVal = 1.0f; }		// ∆–¿⁄∞° ∑π∫ß¿Ã »Œ ≥∑æ“¥Ÿ.
		else if( nGap <= -50 )	{ nVal = 2; fVal = 2.0f; }
		else if( nGap <= -20 )	{ nVal = 3; fVal = 3.0f; }
		else if( nGap <= -10 )	{ nVal = 4; fVal = 4.0f; }
		else { nVal = 5; fVal = 5.0f; }	// 9 ~ -9

		g_DPCoreClient.SendSetPartyDuel( pParty->m_uPartyId, pLoseParty->m_uPartyId, FALSE );

		pLose->SetDecreaseExperienceRevival( false );
		pLoseParty->DoDuelResult( pParty, 0, nVal, fVal );		// ∆ƒ∆ºø¯µÈø°∞‘ ∆–πËªÁΩ«¿ª æÀ∏Æ∞Ì µ‡æÛ ¡ﬂ¡ˆΩ√≈¥.
		pParty->DoDuelResult( pLoseParty, 1, nVal, fVal );		// ∆ƒ∆ºø¯µÈø°∞‘ Ω¬∏Æ∏¶ æÀ∏≤.
	}
#endif	// __WORLDSERVER

	return 1;
}

// »˜∆Æ∆˜¿Œ∆Æ∏¶ º¬«œ±‚
void CMover::SetHitPoint(int nVal) 
{
	m_nHitPoint = nVal;
	if( m_nHitPoint < 0 )
		m_nHitPoint = 0;
	if( m_nHitPoint > GetMaxHitPoint() )
		m_nHitPoint = GetMaxHitPoint(); 
}
// ∏∂≥™ ∆˜¿Œ∆Æ∏¶ º¬«œ±‚ 
void CMover::SetManaPoint(int nVal) 
{
	m_nManaPoint = nVal;
	if( m_nManaPoint < 0 )
		m_nManaPoint = 0;
	if( m_nManaPoint > GetMaxManaPoint() )
		m_nManaPoint = GetMaxManaPoint(); 
}
// ««¡ˆƒÆ ∆˜¿Œ∆Æ∏¶ º¬«œ±‚ 
void CMover::SetFatiguePoint(int nVal) 
{
	m_nFatiguePoint = nVal;
	if( m_nFatiguePoint < 0 )
		m_nFatiguePoint = 0;
	if( m_nFatiguePoint > GetMaxFatiguePoint() )
		m_nFatiguePoint = GetMaxFatiguePoint(); 
}

int CMover::GetPointParam(int nDestParameter )
{
	switch( nDestParameter )
	{
		case DST_HP:	return( GetHitPoint() );
		case DST_MP:	return( GetManaPoint() );
		case DST_FP:	return( GetFatiguePoint() );
		case DST_GOLD:	return( GetGold() );
	}
	return( 0 );
}

void CMover::SetPointParam( int nDstParameter, int nValue, BOOL bTrans )	// bTrans∞° TRUE¿Ã∏È ∞≠¡¶¿¸º€
{
#ifdef __WORLDSERVER
	int nMax = nValue;
#endif // __WORLDSERVER
	if( nValue < 0 )	nValue	= 0;
	BOOL	bTransfer	= bTrans;

	switch( nDstParameter )
	{
		case DST_HP:
#ifdef __WORLDSERVER
			if( nValue > GetMaxHitPoint() || nMax == 999999999 )	nValue	= GetMaxHitPoint();
#else // __WORLDSERVER
			if( nValue > GetMaxHitPoint() )	nValue	= GetMaxHitPoint();
#endif // __WORLDSERVER
			if( m_nHitPoint != nValue )
			{
				bTransfer	= TRUE;
				m_nHitPoint		= nValue;
			}
			break;
		case DST_MP:
#ifdef __WORLDSERVER
			if( nValue > GetMaxManaPoint() || nMax == 999999999 )	nValue	= GetMaxManaPoint();
#else // __WORLDSERVER
			if( nValue > GetMaxManaPoint() )	nValue	= GetMaxManaPoint();
#endif // __WORLDSERVER
			if( m_nManaPoint != nValue )
			{
				bTransfer	= TRUE;
				m_nManaPoint	= nValue;
			}
			break;
		case DST_FP:
#ifdef __WORLDSERVER
			if( nValue > GetMaxFatiguePoint() || nMax == 999999999 )	nValue	= GetMaxFatiguePoint();
#else // __WORLDSERVER
			if( nValue > GetMaxFatiguePoint() )		nValue	= GetMaxFatiguePoint();
#endif // __WORLDSERVER
			if( m_nFatiguePoint != nValue )
			{
				bTransfer	= TRUE;
				m_nFatiguePoint		= nValue;
			}
			break;
		case DST_GOLD:
			//m_dwGold	= (DWORD)nValue;
//#ifdef __PERIN_BUY_BUG	// chipi_091219 √ﬂ∞°
//#ifdef __WORLDSERVER
#if defined( __PERIN_BUY_BUG ) && defined( __WORLDSERVER )
			FLERROR_LOG( PROGRAM_NAME, _T( "SetPointParam( DST_GOLD ) - [User:%s(%07d)], [Value:%d]" ), static_cast<FLWSUser*>(this)->GetName(), static_cast<FLWSUser*>(this)->m_idPlayer, nValue );
			return;
//#endif // __WORLDSERVE
//#endif // __PERIN_BUY_BUG
#else

	#ifdef __CLIENT
			if( IsActiveMover() )
				PLAYSND(SND_INF_TRADE);
	#endif //__CLIENT
			bTransfer	= TRUE;
			SetGold( nValue );
			break;
#endif //defined( __PERIN_BUY_BUG ) && defined( __WORLDSERVER )
	}
#ifdef __WORLDSERVER
	if( bTransfer )
		g_xWSUserManager->AddSetPointParam( this, nDstParameter, nValue );
#endif	// __WORLDSERVER
}

BOOL CMover::AddExperience( EXPINTEGER nExp, BOOL bFirstCall, BOOL bMultiPly, BOOL bMonster )
{
	//	mulcom	BEGIN100315	∫£∆Æ≥≤ Ω√∞£ ¡¶«—
	if( g_xFlyffConfig->GetMainLanguage() == LANG_VTN )
	{
		if( IsPlayer() && m_nAccountPlayTime != -1 )
		{
			if( m_nAccountPlayTime < 0 || m_nAccountPlayTime > MIN( 300 ) )
			{
				nExp	= 0;
			}
			else if( m_nAccountPlayTime >= MIN( 180 ) && m_nAccountPlayTime <= MIN( 300 ) )
			{
				nExp	= (EXPINTEGER)( nExp * 0.5f );
			}
		}
	}
	//	mulcom	END100315	∫£∆Æ≥≤ Ω√∞£ ¡¶«—

	if( nExp <= 0 )		// nExp∞° ∏∂¿Ã≥ Ω∫¿Ã∞≈≥™ 0¿Ã∏È √≥∏Æ«“« ø‰ æ¯¿Ω.
		return FALSE;

	if( m_nHitPoint <= 0 )
		return FALSE;

#ifdef __INTERNALSERVER
	FLTRACE_LOG( PROGRAM_NAME, _T( "EXP = %I64d" ), nExp );
#endif	// __INTERNALSERVER

	if( IsMaster() || IsHero() )
		nExp /= 2;
	if( bFirstCall && bMultiPly )
	{
#ifdef __WORLDSERVER
		EXPINTEGER nAddExp = static_cast<FLWSUser*>( this )->GetAddExpAfterApplyRestPoint( nExp );
		nExp	= (EXPINTEGER)( nExp * GetExpFactor() );
		nExp += nAddExp;
#endif // __WORLDSERVER
	}

	//sun: 8¬˜ ø£¡© º“»Ø Neuz, World, Trans
	if( bFirstCall && HasBuffByIk3( IK3_ANGEL_BUFF ) )
	{
		int nAngel = 100;
		IBuff* pBuff	= m_buffs.GetBuffByIk3( IK3_ANGEL_BUFF );
		WORD wId	= ( pBuff? pBuff->GetId(): 0 );
		if( wId > 0 )
		{
			PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( wId );
			if( pItemProp )
				nAngel = (int)( (float)pItemProp->nAdjParamVal[0] );	// TODO : »ø∞˙ ¿˚øÎ ºˆƒ°∑Œ ª©¿⁄
		}
		if( nAngel <= 0 || 100 < nAngel  )
			nAngel = 100;
		// À¨: ¿¸¡˜ ¿¸ ø£¡© πˆ«¡ »∞º∫»≠ »ƒ ¿¸¡˜«œ¥¬ ∞ÊøÏ, « ø‰ ºˆƒ°∞° ≥ π´ ≈©¥Ÿ.
		EXPINTEGER nMaxAngelExp = prj.m_aExpCharacter[m_nAngelLevel].nExp1 / 100 * nAngel;
		if( m_nAngelExp < nMaxAngelExp )
		{
			//////////////////////////////////////////////////////////////////////////
// 			if( HasBuffByIk3( IK3_PKPENALTYSCROLL_C ) == TRUE )
// 			{
// 				m_nAngelExp += ( nExp / 2 );
// 			}
// 			else
// 			{
// 				nExp /= 2;
// 				m_nAngelExp += nExp;
// 			}
 			nExp /= 2;
 			m_nAngelExp += nExp;
			//////////////////////////////////////////////////////////////////////////
#ifdef __WORLDSERVER
			// ø£¡© ∞¸∑√ ∑Œ±◊
			int nAngelExpPercent = (int)( m_nAngelExp * 100 / nMaxAngelExp );
			int nNextAngelExpLog = (int)(((FLWSUser*)this)->m_nAngelExpLog / 20 + 1) * 20;
			
			if( nAngelExpPercent >= nNextAngelExpLog )
			{
				((FLWSUser*)this)->m_nAngelExpLog = nAngelExpPercent;
				PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( wId );
				
				if( pItemProp )
				{
					LogItemInfo aLogItem;
					//aLogItem.Action		= "&";
					//aLogItem.SendName	= ((FLWSUser*)this)->GetName();
					//aLogItem.RecvName	= "ANGEL_EXP_LOG";
					FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "&" );
					FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), ((FLWSUser*)this)->GetName() );
					FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "ANGEL_EXP_LOG" );

					aLogItem.WorldId	= ((FLWSUser*)this)->GetWorld()->GetID();
					aLogItem.Gold		= aLogItem.Gold2 = ((FLWSUser*)this)->GetGold();
					//aLogItem.ItemName	= pItemProp->szName;
					FLSPrintf( aLogItem.kLogItem.szItemName, _countof( aLogItem.kLogItem.szItemName ), "%d", pItemProp->dwID );
					aLogItem.Gold_1		= (DWORD)( m_nAngelExp );
					g_DPSrvr.OnLogItem( aLogItem );
				}
			}
			
			BOOL bAngelComplete = FALSE;
			if( m_nAngelExp > nMaxAngelExp )
			{
				m_nAngelExp = nMaxAngelExp;
				bAngelComplete = TRUE;
			}
			((FLWSUser*)this)->AddAngelInfo( bAngelComplete );
#endif // __WORLDSERVER
		}
	}

	if( IsBaseJob() == TRUE )
	{
		if( m_nLevel >= MAX_JOB_LEVEL )
		{
			m_nLevel	= MAX_JOB_LEVEL;
			m_nExp1		= 0;
			return FALSE;
		}
	}
	else if( IsExpert() == TRUE )
	{
		if( m_nLevel >= MAX_JOB_LEVEL + MAX_EXP_LEVEL )
		{
			m_nLevel	= MAX_JOB_LEVEL + MAX_EXP_LEVEL;
			m_nExp1		= 0;
			return FALSE;
		}
	}
	else if( IsPro() == TRUE )
	{
		if( m_nLevel > MAX_GENERAL_LEVEL )
		{
			m_nLevel	= MAX_GENERAL_LEVEL;
			m_nExp1		= ( prj.m_aExpCharacter[m_nLevel + 1].nExp1 - 1 );
			return FALSE;
		}		
	}
//sun: 10¬˜ ¿¸Ω¬Ω√Ω∫≈€	Neuz, World, Trans
	else if( IsMaster() == TRUE )
	{
		if( m_nLevel > MAX_GENERAL_LEVEL )
		{
			m_nLevel	= MAX_GENERAL_LEVEL;
			m_nExp1		= ( prj.m_aExpCharacter[m_nLevel + 1].nExp1 - 1 );
			return FALSE;
		}
	}
	else if( IsHero() == TRUE )
	{
		if( m_nLevel >= MAX_LEGEND_LEVEL )
		{
			m_nLevel	= MAX_LEGEND_LEVEL;
			m_nExp1		= 0;
			return FALSE;
		}
	}
	else if( IsLegendHero() == TRUE )
	{
//#if __VER >= 17
		if( m_nLevel >= MAX_3RD_LEGEND_LEVEL )
		{
			m_nLevel	= MAX_3RD_LEGEND_LEVEL;
			m_nExp1		= 0;
			return FALSE;
		}
// #else // __VER >= 17
// 		if( m_nLevel > MAX_3RD_LEGEND_LEVEL )
// 		{
// 			m_nLevel	= MAX_3RD_LEGEND_LEVEL;
// 			m_nExp1		= ( prj.m_aExpCharacter[m_nLevel + 1].nExp1 - 1 );
// 			return FALSE;
// 		}
// #endif // __VER >= 17
	}
	
//	int nLevelbuf = m_nLevel;
	int nNextLevel = m_nLevel + 1;

	m_nExp1 += nExp;									// pxpøÕ ∞¸∞Ëæ¯¿Ã exp∞° ¡ı∞°. ∏∏Ω¶¿Ã!

// #ifdef __WORLDSERVER
// 	const bool bChaoPlayer	= ( IsChaotic() == TRUE && static_cast<FLWSUser*>( this )->HasBuffByIk3( IK3_PKPENALTYSCROLL_C ) == FALSE ) ?
// 							true : false;
// #endif // __WORLDSERVER
	
	if( bFirstCall )
	{
//sun: 8, // __S8_PK
#ifdef __WORLDSERVER
		if( bMonster && IsChaotic() )
//		if( bMonster && bChaoPlayer )
		{
			m_dwPKExp	= (DWORD)( m_dwPKExp + nExp );
			DWORD dwPropensity = GetPKPropensity(); // undorflow
			int nLevelPKExp = prj.GetLevelExp( GetLevel() ); // Level∫∞ ∞®º“ ∞Ê«Ëƒ°
			if( nLevelPKExp != 0 )
			{
				int nSubExp = m_dwPKExp / nLevelPKExp;
				if( nSubExp )
				{
					SetPKPropensity( GetPKPropensity() - ( m_dwPKExp / nLevelPKExp ) );
					if( dwPropensity <= GetPKPropensity() )
						SetPKPropensity( 0 );
					m_dwPKExp %= nLevelPKExp;
					
					g_xWSUserManager->AddPKPropensity( this );
					g_dpDBClient.SendLogPkPvp( this, NULL, 0, 'P' );
				}
			}
		}
#endif // __WORLDSERVER

	}

	if( m_nExp1 >= prj.m_aExpCharacter[nNextLevel].nExp1 )	// ∑π∫ßæ˜
	{
//sun: 10¬˜ ¿¸Ω¬Ω√Ω∫≈€	Neuz, World, Trans
		if( IsLegendHero() == TRUE && ( nNextLevel > MAX_3RD_LEGEND_LEVEL ) )
		{
			m_nLevel = MAX_3RD_LEGEND_LEVEL;
#if __VER >= 17
			m_nExp1  = 0;
#else // __VER >= 17
			m_nExp1  = ( prj.m_aExpCharacter[nNextLevel].nExp1 - 1 );
#endif // __VER >= 17
			return FALSE;
		}
		else if( IsHero() && ( nNextLevel > MAX_LEGEND_LEVEL ) )
		{
			m_nLevel = MAX_LEGEND_LEVEL;
			m_nExp1  = 0;
			return FALSE;
		}
		else if( IsLegendHero() == FALSE && IsHero() == FALSE && nNextLevel > MAX_GENERAL_LEVEL )
		{
			m_nLevel = MAX_GENERAL_LEVEL;
			m_nExp1  = ( prj.m_aExpCharacter[nNextLevel].nExp1 - 1 );
			return FALSE;
		}

		EXPINTEGER nExptmp;
//		BOOL f	= FALSE;

		{
			m_Stat.SetRemainStatPoint( m_Stat.GetRemainStatPoint() + prj.m_aExpCharacter[ nNextLevel ].dwLPPoint );

//sun: 10¬˜ ¿¸Ω¬Ω√Ω∫≈€	Neuz, World, Trans
			if( IsMaster() == TRUE || IsHero() == TRUE || IsLegendHero() == TRUE )
			{
				m_Stat.SetRemainStatPoint( m_Stat.GetRemainStatPoint() + 1 );
			}

			nExptmp		= m_nExp1 - prj.m_aExpCharacter[nNextLevel].nExp1;
//sun: 8,
			m_nExp1		= 0;
//!			m_nExp1		= m_nDeathExp		= 0;

			m_nLevel	= nNextLevel;

//			if( m_nLevel > m_nDeathLevel )
//				f	= TRUE;

			BOOL bLevelUp = TRUE;
			if( IsBaseJob() && m_nLevel > MAX_JOB_LEVEL ) 
			{
				m_nLevel = MAX_JOB_LEVEL;
				bLevelUp = FALSE;
			}
			else if( IsExpert() && m_nLevel > MAX_JOB_LEVEL + MAX_EXP_LEVEL )
			{
				m_nLevel = MAX_JOB_LEVEL + MAX_EXP_LEVEL;
				bLevelUp = FALSE;
			}
			else if( IsPro() && m_nLevel > MAX_GENERAL_LEVEL )
			{
				m_nLevel = MAX_GENERAL_LEVEL;
				bLevelUp = FALSE;

				m_nExp1  = ( prj.m_aExpCharacter[nNextLevel].nExp1 - 1 );
				nExptmp  = 0;
				return FALSE;
			}
//sun: 10¬˜ ¿¸Ω¬Ω√Ω∫≈€	Neuz, World, Trans
			else if( IsMaster() && m_nLevel > MAX_GENERAL_LEVEL )
			{
				m_nLevel = MAX_GENERAL_LEVEL;
				bLevelUp = FALSE;
				m_nExp1  = ( prj.m_aExpCharacter[nNextLevel].nExp1 - 1 );
				nExptmp  = 0;
				return FALSE;
			}
			else if( IsHero() && (m_nLevel > MAX_LEGEND_LEVEL ) )
			{
				m_nLevel = MAX_LEGEND_LEVEL;
				bLevelUp = FALSE;
				m_nExp1  = ( prj.m_aExpCharacter[nNextLevel].nExp1 - 1 );
				nExptmp  = 0;
				return FALSE;
			}
			else if( IsLegendHero() == TRUE && ( m_nLevel > MAX_3RD_LEGEND_LEVEL ) )
			{
				m_nLevel = MAX_3RD_LEGEND_LEVEL;
				bLevelUp = FALSE;
#if __VER >= 17
				m_nExp1  = 0;
#else // __VER >= 17
				m_nExp1  = ( prj.m_aExpCharacter[nNextLevel].nExp1 - 1 );
#endif // __VER >= 17
				nExptmp  = 0;
				return FALSE;
			}

			if( bLevelUp )
			{
				m_nHitPoint = GetMaxHitPoint();
				m_nManaPoint = GetMaxManaPoint();
				m_nFatiguePoint = GetMaxFatiguePoint();
				SetJobLevel( m_nLevel, m_nJob );
//sun: 8,
				if( m_nDeathLevel >= m_nLevel )
				{
					m_Stat.SetRemainStatPoint( m_Stat.GetRemainStatPoint() - prj.m_aExpCharacter[ nNextLevel ].dwLPPoint );
//sun: 10¬˜ ¿¸Ω¬Ω√Ω∫≈€	Neuz, World, Trans

					if( IsMaster() == TRUE || IsHero() == TRUE || IsLegendHero() == TRUE )
					{
						m_Stat.SetRemainStatPoint( m_Stat.GetRemainStatPoint() - 1 );
					}
				}


#ifdef __WORLDSERVER
				if( m_nDeathLevel < m_nLevel )
				{
					int nGetPoint = 0;

					if( IsLegendHero() == TRUE )
					{
						if( GetJob() == JOB_LORDTEMPLER_HERO || GetJob() == JOB_FLORIST_HERO )
						{
							nGetPoint = SKILLPOINT_LEGENDHERO_EXCEPTION;
						}
						else
						{
							nGetPoint = SKILLPOINT_LEGENDHERO_BASE;
						}
					}
					else
					{
						nGetPoint = ( ( GetLevel() - 1 ) / SKILLPOINT_DIVIDE ) + SKILLPOINT_BASE;
					}

//sun: 10¬˜ ¿¸Ω¬Ω√Ω∫≈€	Neuz, World, Trans
					if( IsMaster() || IsHero() )
						SetMasterSkillPointUp();
					else
					{
						AddSkillPoint( nGetPoint );
						g_dpDBClient.SendLogSkillPoint( LOG_SKILLPOINT_GET_HUNT, nGetPoint, this, NULL );
					}
//sun: 10, __S_RECOMMEND_EVE
				    if( g_eLocal.GetState( EVE_RECOMMEND ) && IsPlayer() )
					{
						g_dpDBClient.SendRecommend( (FLWSUser*)this );
					}					

					((FLWSUser*)this)->m_nExpLog = 0;
#ifdef __EVENTLUA_GIFT
					prj.m_EventLua.SetLevelUpGift( (FLWSUser*)this, m_nLevel );
#endif // __EVENTLUA_GIFT
					CCampusHelper::GetInstance()->SetLevelUpReward( (FLWSUser*)this );
				}

				g_pMadrigalGift->OnLevel( static_cast<FLWSUser*>( this ) );
#endif // __WORLDSERVER
			}
			else
			{
				m_Stat.SetRemainStatPoint( m_Stat.GetRemainStatPoint() - prj.m_aExpCharacter[ nNextLevel ].dwLPPoint );

//sun: 10¬˜ ¿¸Ω¬Ω√Ω∫≈€	Neuz, World, Trans
				if( IsMaster() == TRUE || IsHero() == TRUE || IsLegendHero() == TRUE )
				{
					m_Stat.SetRemainStatPoint( m_Stat.GetRemainStatPoint() - 1 );
				}

				nExptmp	= m_nExp1	= 0;
			}

			// ∑π∫ß 20¿Ã µ«¥¬º¯∞£ ∫Ò«‡∑π∫ß¿∫ 1¿Ãµ»¥Ÿ.
			if( m_nLevel == 20 )
				SetFlightLv( 1 );
			
#ifdef __CLIENT
			if( m_pActMover && ( m_pActMover->IsState( OBJSTA_STAND ) || m_pActMover->IsState( OBJSTA_STAND2 )) )
				SetMotion( MTI_LEVELUP, ANILOOP_1PLAY, MOP_FIXED );
			CreateSfx(g_Neuz.m_pd3dDevice, XI_INDEX( 42, XI_GEN_LEVEL_UP01 ),GetPos(),GetId());
			PlayMusic( BGM_IN_LEVELUP );
#endif	// __CLIENT
		}

		if( nExptmp > 0 )
			AddExperience( nExptmp, FALSE, bMultiPly );
		return TRUE;
	}

	return FALSE;
}

// ∞Ê«Ëƒ°∏¶ nExp∏∏≈≠ ±¥¬¥Ÿ.  ∑æ¥ŸøÓµµ µ»¥Ÿ.
// bExp2Clear : pxp∏¶ 0¿∏∑Œ «“∞«¡ˆ ∏ª∞«¡ˆ.
// bLvDown : ∑π∫ß¥ŸøÓ¿ª «“∞«¡ˆ ∏ª∞«¡ˆ.
// BOOL CMover::DecExperience( EXPINTEGER nExp, BOOL /*bExp2Clear*/, BOOL bLvDown )
// {
// 	if( nExp < 0 )	// ∞™¿∫ «◊ªÛ æÁºˆ∑Œ∏∏ ¡‡æﬂ «—¥Ÿ.
// 		return FALSE;
// 	
// 	int nPrevLevel	= m_nLevel - 1;
// 	if( nPrevLevel < 1 )	// ∑π∫ß 1¿Ã«œ∑Œ¥¬ ¥ı¿ÃªÛ ≥ª∑¡∞°¡ˆ æ ¿Ω.
// 		return FALSE;
// 
// 	m_nExp1		-= nExp;	// nExp∏∏≈≠ ∞Ê«Ëƒ°∏¶ ±¿Ω. Exp2¥¬ 0%∑Œ.
// 
// 	if( m_nExp1 < 0 )
// 	{
// 		if( bLvDown )	// ∑æ¥ŸøÓ¿Ã µ«¥¬ ªÛ»≤.
// 		{
// //sun: 8,  
// 			--m_nLevel;
// 			m_nExp1 = (EXPINTEGER)(prj.m_aExpCharacter[m_nLevel+1].nExp1 + m_nExp1 );
// 
// 			return TRUE;	// ∑æ¥ŸøÓ µ∆¿∏∏È TRUE∏Æ≈œ
// 		}
// 		else
// 		{
// 			m_nExp1		= 0;
// #ifdef __WORLDSERVER
// 			g_dpDBClient.SendLogLevelUp( this, 7 );
// #endif // __WORLDSERVER
// 		}
// 	}
// 	return FALSE;
// }

// «ˆ¿Á ∑π∫ß¿« √÷¥Î∞Ê«Ëƒ°¿« %∑Œ ∞Ê«Ëƒ° «œ∂Ù
// ex) fPercent = 0.2f;		// 20%
// BOOL CMover::DecExperiencePercent( float fPercent, BOOL bExp2Clear, BOOL bLvDown )
// {
// 	EXPINTEGER	nExpOld		= m_nExp1;		// ±¿Ã±‚¿¸ exp
// 	EXPINTEGER	nMaxExp		= prj.m_aExpCharacter[m_nLevel+1].nExp1;	// ±¿Ã±‚¿¸ ∞Ê«Ëƒ°ƒ° ∏∆Ω∫∑Æ.
// 	int	nOldLv	= m_nLevel;		// ±¿Ã±‚¿¸ ∑π∫ß.
// 
// 	if( fPercent == 0.0f )	
// 		return FALSE;	// 0%∏¶ ±¥¬ ªÛ»≤¿Ã∏È ∞ËªÍ«“ « ø‰ æ¯¥Ÿ.
// 
// 	EXPINTEGER	nDecExp = (EXPINTEGER)(prj.m_aExpCharacter[m_nLevel+1].nExp1 * fPercent );	// «ˆ¿Á∑π∫ß¿« √÷¥Î∞Ê«Ëƒ° * ∆€ºæ∆Æ
// 	bLvDown		= DecExperience( nDecExp, bExp2Clear, bLvDown );
// 
// 	if( bLvDown )	// Ω«¡¶∑Œ ∑π∫ß¿Ã ¥ŸøÓµ«æ˙¿ª∂ß.
// 	{
// 		if( m_nLevel - nOldLv < -1 )	// ∑π∫ß¿Ã µŒ¥‹∞Ë ¿ÃªÛ ¥ŸøÓµ«¥¬ ¿œ¿∫ æ¯æÓæﬂ «—¥Ÿ.
// 			FLERROR_LOG( PROGRAM_NAME, _T( "%s ∑π∫ß¥ŸøÓ ¿ÃªÛ. %d -> %d  %I64d(%f)" )
// 			, m_szName, nOldLv, m_nLevel, nDecExp, fPercent );
// 
// 		EXPINTEGER	nDecExp2	= ( prj.m_aExpCharacter[m_nLevel+1].nExp1 - m_nExp1 ) + nExpOld;	// Ω«¡¶ ±¿Œ ∆˜¿Œ∆Æ.
// 		if( nDecExp2 > nDecExp )	// Ω«¡¶ ±¿ŒæÁ¿Ã ø¯∑° ±¿∏∑¡∞Ì «ﬂ¥¯ æÁ∫∏¥Ÿ ∏π¿∏∏È ∞Ê∞Ì.
// 			FLERROR_LOG( PROGRAM_NAME, _T( "%s(Lv%d) ∞Ê«Ëƒ°∞° ¿ÃªÛ«œ∞‘ ±¿”. %I64d %I64d(%f) %I64d %I64d" )
// 			, m_szName, GetLevel(), nMaxExp, nDecExp, fPercent, nExpOld, m_nExp1 );
// 	}
// 	else
// 	{
// 		EXPFLOAT fOldPercent = static_cast<EXPFLOAT>( nExpOld ) / static_cast<EXPFLOAT>( nMaxExp );		// ±¿Ã±‚¿¸ ∆€ºæ∆Æ∑Æ.
// 		EXPFLOAT fCurPercent = static_cast<EXPFLOAT>( m_nExp1 ) / static_cast<EXPFLOAT>( nMaxExp );		// ±Ô¿∫»ƒ ∆€ºæ∆Æ∑Æ.
// 
// 		if( fOldPercent - fCurPercent > 0.060f )
// 			FLERROR_LOG( PROGRAM_NAME, _T( "%s(Lv%d) ∞Ê«Ëƒ°∞° ¿ÃªÛ«œ∞‘ ±¿”. %I64d %I64d(%f) %I64d(%f) %I64d(%f)" )
// 			, m_szName, GetLevel(), nMaxExp, nDecExp, fPercent, nExpOld, fOldPercent, m_nExp1, fCurPercent );
// 	}
// 
// 	return bLvDown;
// }

// ∫Ò«‡ ∞Ê«Ëƒ° »πµÊ
BOOL CMover::AddFxp( int nFxp )
{
	if( nFxp <= 0 )		// nExp∞° ∏∂¿Ã≥ Ω∫¿Ã∞≈≥™ 0¿Ã∏È √≥∏Æ«“« ø‰ æ¯¿Ω.
		return FALSE;

	if( m_nHitPoint <= 0 )		// ¡◊∞Ì≥™º± ∞Ê«Ëƒ° »πµÊ ∏¯«‘.
		return FALSE;

	if( GetFlightLv() == 0 )
		return FALSE;

	int nNextLevel = GetFlightLv() + 1;

	m_nFxp += nFxp;		// ¿œ¥‹ ∞Ê«Ëƒ°∏¶ ¥ı«œ∞Ì.
	if( (DWORD)( m_nFxp ) >= prj.m_aFxpCharacter[nNextLevel].dwFxp ) // ∑π∫ßæ˜¿Ã µ«æ˙¥¬¡ˆ »Æ¿Œ.
	{
		int		FxpBuf;
		FxpBuf = m_nFxp - prj.m_aFxpCharacter[ nNextLevel ].dwFxp;		// ∑π∫ßæ˜«œ∞Ìµµ æÛ∏∂≥™ ¥ı ø¿πˆµ∆¥¬¡ˆ?
		m_nFxp = 0;
		SetFlightLv( nNextLevel );	// ∑π∫ß ¡ı∞°.
		
#ifdef __CLIENT
		// ∑π∫ßæ˜Ω√ ¿Ã∆Â∆Æ
		CreateSfx(g_Neuz.m_pd3dDevice,XI_INDEX( 42, XI_GEN_LEVEL_UP01 ),GetPos(),GetId());
		PlayMusic( BGM_IN_LEVELUP );
#endif	// __CLIENT
	
		// ø¿πŸµ» ∞Ê«Ëƒ°¥¬ «—π¯¥ı µπ∑¡º≠ ¥ı«ÿ¡‹.
		if( FxpBuf > 0)
		{
			AddFxp( FxpBuf );
		}

		return TRUE;
	}
	return FALSE;
}

BOOL CMover::AddChangeJob( int nJob )
{
	BOOL	bResult = FALSE;
	LPSKILL lpSkill;

	if( nJob == JOB_VAGRANT )
	{
		m_nJob = nJob;

		SkillProp** apSkillProp = prj.m_aJobSkill[ nJob ];
		int nJobNum = prj.m_aJobSkillNum[ nJob ];
		for( int i = 0; i < nJobNum; i++ )
		{
			SkillProp* pSkillProp = apSkillProp[ i ];
			lpSkill = &m_aJobSkill[ i ];
			lpSkill->dwSkill = pSkillProp->dwID;
		}
	}

	if( MAX_JOBBASE <= nJob && nJob < MAX_EXPERT ) // 1¬˜ ¿¸¡˜ ±Ó¡ˆ~~ ^^;;;;
	{
		m_nJob = nJob;
		
		SkillProp** apSkillProp = prj.m_aJobSkill[ m_nJob ];
		int nJobNum = prj.m_aJobSkillNum[ m_nJob ];
		for( int i = 0; i < nJobNum; i++ )
		{
			SkillProp* pSkillProp = apSkillProp[ i ];
			lpSkill = &m_aJobSkill[ i + MAX_JOB_SKILL ];
			lpSkill->dwSkill = pSkillProp->dwID;
		}
		bResult = TRUE;
	}
	if( MAX_EXPERT <= nJob && nJob < MAX_PROFESSIONAL ) // 2¬˜ ¿¸¡˜ ±Ó¡ˆ~~ ^^;;;;
	{
		m_nJob = nJob;
		
		SkillProp** apSkillProp = prj.m_aJobSkill[ m_nJob ];
		int nJobNum = prj.m_aJobSkillNum[ m_nJob ];
		for( int i = 0; i < nJobNum; i++ )
		{
			SkillProp* pSkillProp = apSkillProp[ i ];
			lpSkill = &m_aJobSkill[ i + MAX_JOB_SKILL + MAX_EXPERT_SKILL ];
			lpSkill->dwSkill = pSkillProp->dwID;
		}
		bResult = TRUE;
	}
//sun: 10¬˜ ¿¸Ω¬Ω√Ω∫≈€	Neuz, World, Trans
	if( MAX_PROFESSIONAL <= nJob && nJob < MAX_MASTER ) // ¿¸Ω¬ ±Ó¡ˆ~~ ^^;;;;
	{
		m_nJob = nJob;
		
		SkillProp** apSkillProp = prj.m_aJobSkill[ m_nJob ];
		int nJobNum = prj.m_aJobSkillNum[ m_nJob ];
		for( int i = 0; i < nJobNum; i++ )
		{
			SkillProp* pSkillProp = apSkillProp[ i ];
			lpSkill = &m_aJobSkill[ i + MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL];
			lpSkill->dwSkill = pSkillProp->dwID;
			lpSkill->dwLevel = 1;	//masterΩ∫≈≥¿∫ 1∫Œ≈Õ ¡ÿ¥Ÿ.
		}
		bResult = TRUE;
	}
	
	if( MAX_MASTER <= nJob && nJob < MAX_HERO ) // øµøı ±Ó¡ˆ~~ ^^;;;;
	{
		m_nJob = nJob;
		
		SkillProp** apSkillProp = prj.m_aJobSkill[ m_nJob ];
		int nJobNum = prj.m_aJobSkillNum[ m_nJob ];
		for( int i = 0; i < nJobNum; i++ )
		{
			SkillProp* pSkillProp = apSkillProp[ i ];
			lpSkill = &m_aJobSkill[ i + MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL + MAX_MASTER_SKILL ];
			lpSkill->dwSkill = pSkillProp->dwID;
		}
		bResult = TRUE;
	}
	else if( MAX_HERO <= nJob && nJob < MAX_LEGEND_HERO )
	{
		m_nJob = nJob;

		SkillProp** apSkillProp = prj.m_aJobSkill[ m_nJob ];
		int nJobNum = prj.m_aJobSkillNum[ m_nJob ];
		for( int i = 0; i < nJobNum; i++ )
		{
			SkillProp* pSkillProp = apSkillProp[ i ];
			lpSkill = &m_aJobSkill[ i + MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL + MAX_MASTER_SKILL + MAX_HERO_SKILL ];
			lpSkill->dwSkill = pSkillProp->dwID;
		}
		bResult = TRUE;
	}


#ifdef __WORLDSERVER
//sun: 13, ¥ﬁ¿Œ
	((FLWSUser*)this)->CheckHonorStat();
	((FLWSUser*)this)->AddHonorListAck();
	g_xWSUserManager->AddHonorTitleChange( this, m_nHonor);

	if( bResult == TRUE )
	{
		g_pMadrigalGift->OnLevel( static_cast<FLWSUser*>( this ) );
	}

#endif // __WORLDSERVER
	return bResult;
}

BOOL CMover::SetFxp( int nFxp, int nFlightLv )
{
	m_nFxp = nFxp;

	// ∑π∫ß¿Ã ø√∂˙¿∏∏È.
	if( nFlightLv > GetFlightLv() )
	{
		SetFlightLv( nFlightLv );
#ifdef __CLIENT
		CreateSfx(g_Neuz.m_pd3dDevice,XI_INDEX( 42, XI_GEN_LEVEL_UP01 ),GetPos(),GetId());
		PlayMusic( BGM_IN_LEVELUP );
		g_WndMng.PutString( prj.GetText( TID_GAME_FLYLVLUP ), NULL, prj.GetTextColor( TID_GAME_FLYLVLUP ) );
#endif // CLIENT
		return TRUE;
	}

	return FALSE;
}

BOOL CMover::SetExperience( EXPINTEGER nExp1, int nLevel )
{
	m_nExp1		= nExp1;

	if( IsInvalidObj(this) )
		return 0;

	if( nLevel > m_nLevel )
	{
#ifdef __CLIENT
 		// 15∑æ µ«∏È ¥ı¿ÃªÛ √ ∫∏¿⁄∞° æ∆¥œπ«∑Œ ¿⁄µø¿∏∑Œ √ ∫∏¿⁄µµøÚ∏ª ≤Ù¿⁄.
		if( nLevel == 15 )		// 1¬˜¿¸¡˜∑π∫ß 15ø° ¥Î«— define ¿÷¿∏∏È ±◊∞…∑Œ πŸ≤„¡÷. -xuzhu-
			g_Option.m_nInstantHelp = 0;
 		
		PutLvUpSkillName_1(nLevel);

		if( m_pActMover && ( m_pActMover->IsState( OBJSTA_STAND ) || m_pActMover->IsState( OBJSTA_STAND2 ) ) )
			SetMotion( MTI_LEVELUP, ANILOOP_1PLAY, MOP_FIXED );
		CreateSfx(g_Neuz.m_pd3dDevice,XI_INDEX( 42, XI_GEN_LEVEL_UP01 ),GetPos(),GetId());
		PlayMusic( BGM_IN_LEVELUP );
		if( m_nDeathLevel < nLevel )
			g_WndMng.PutString( prj.GetText( TID_GAME_LEVELUP ), NULL, prj.GetTextColor( TID_GAME_LEVELUP ) );
		
		if( g_WndMng.m_pWndWorld )
		{
			if( g_xFlyffConfig->GetMainLanguage() != LANG_JAP )
				g_Caption1.AddCaption(  prj.GetText( TID_GAME_LEVELUP_CAPTION ), g_WndMng.m_pWndWorld->m_pFontAPICaption );// CWndBase::m_Theme.m_pFontCaption );
			else
				g_Caption1.AddCaption(  prj.GetText( TID_GAME_LEVELUP_CAPTION ), NULL );// CWndBase::m_Theme.m_pFontCaption );
		}
		
		// 2, 8, 12
		static	int nPatLv[4]	= { 2, 8, 12, 0	};
		for( int i = 0; i < 3; i++ ) {
			if( m_nLevel < nPatLv[i] && nLevel >= nPatLv[i] ) {
				g_WndMng.PutString( prj.GetText( TID_GAME_PATTERNUP ), NULL, prj.GetTextColor( TID_GAME_PATTERNUP ) );
				break;
			}
			else if( m_nLevel >= nPatLv[2] )
				break;
		}

		if( TRUE == IsJobType( JTYPE_BASE ) )
		{
			if( nLevel == 15 )
				g_WndMng.PutString( prj.GetText( TID_EVE_LEVEL15 ), NULL, prj.GetTextColor( TID_EVE_LEVEL15 ) );

			static	int nNum	= prj.m_aJobSkillNum[JOB_VAGRANT];
		}
		if( m_nLevel < 20 && nLevel >= 20 )
		{
			g_WndMng.PutString( prj.GetText( TID_GAME_FLYLVLUP ), NULL, prj.GetTextColor( TID_GAME_FLYLVLUP ) );
			SetFlightLv( 1 );
		}
		else if( nLevel < 20 )
			SetFlightLv( 0 );
#endif	// __CLIENT
		m_nLevel	= nLevel;

		SetJobLevel( m_nLevel, m_nJob );

		m_nHitPoint = GetMaxHitPoint();
		m_nManaPoint = GetMaxManaPoint();
		m_nFatiguePoint = GetMaxFatiguePoint();

#ifdef __CLIENT
		if( IsPlayer() )
		{
			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
			
			if( pWndWorld ) 
			{
//sun: 13, ƒ≥∏Ø≈Õ¡§∫∏√¢ ∏Æ¥∫æÛ
				pWndWorld->GetAdvMgr()->AddAdvButton(APP_CHARACTER3);

				switch(GetLevel())
				{
			//sun: 12, ∆©≈‰∏ÆæÛ ∞≥º±
				case 15:
					pWndWorld->m_pWndGuideSystem->GuideStart(GUIDE_CHANGEJOB);
					break;						
				case 20:
					pWndWorld->m_pWndGuideSystem->GuideStart(GUIDE_FLIGHT_METHOD);
					break;
				case 40:
					pWndWorld->m_pWndGuideSystem->GuideStart(GUIDE_APP_GUILD);
					break;
				}
			}
		}
#endif //__CLIENT		

		return TRUE;
	}
	else if( nLevel < m_nLevel )	// ∑π∫ß ¥ŸøÓ¿Ã µ«æ˙¿ª∂ß.
	{
		m_nLevel	= nLevel;
		if( nLevel < 20 )
			SetFlightLv( 0 );

#ifdef __CLIENT
		if( IsPlayer() )
		{
			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
			
			if( pWndWorld ) 
			{
				switch(GetLevel())
				{
				case 2:
//sun: 13, ƒ≥∏Ø≈Õ¡§∫∏√¢ ∏Æ¥∫æÛ
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(APP_CHARACTER3);
					break;
				case 3:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_EVENT_BERSERKERMODE);
					break;
				case 4:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(APP_SKILL_BEFOREJOB);
					break;
				case 8:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_ESSENSE);
					break;
				case 15:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_CHANGEJOB);
					break;					
				case 16:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_FLIGHT);
					break;
				case 20:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_FLIGHT_METHOD);
					break;
				case 40:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_APP_GUILD);
					break;
				}
			}
		}
#endif	// __CLIENT
	}


	return FALSE;
}


void CMover::SetEquipDstParam()
{
	if( IsPlayer() ) 
	{
		FLItemElem* pItemElem;
		for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
		{
			pItemElem = GetEquipItem( i ); //m_Inventory.GetAtId( m_adwEquipment[ i ] );
			if( pItemElem )
				SetDestParam( pItemElem->m_dwItemId, FALSE );	//don't send
		}
	}
}


/////////////////////////////////////////////////////////////////////////////// aaaaaaaaa
int CMover::SumEquipDefenseAbility( LONG* pnMin, LONG* pnMax )
{
	if( IsPlayer() )
	{
		int	nMin = 0, nMax = 0;
		FLItemElem* pItemElem;
		PT_ITEM_SPEC pItemProp;

		for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
		{
			pItemElem = GetEquipItem( i ); //m_Inventory.GetAtId( m_adwEquipment[ i ] );
			if( pItemElem )
			{
				pItemProp = pItemElem->GetProp();

				if( pItemProp == NULL )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "Parts Not Property = %d" ), i );
				}

				if( pItemProp )
				{
					if( pItemProp->dwItemKind2 == IK2_ARMOR || pItemProp->dwItemKind2 == IK2_ARMORETC )
					{
						int nOption = pItemElem->GetAbilityOption();
						int nOptionVal = 0;
						if( nOption > 0 ) 
							nOptionVal = (int)( pow( (float)( nOption ), 1.5f ) );
							
						float f		= GetItemMultiplier( pItemElem );
						int nItemMin = static_cast<int>( pItemProp->dwAbilityMin * f );
						int nItemMax = static_cast<int>( pItemProp->dwAbilityMax * f );

#ifdef __ZCK_ARME_RARE
						const int nRarity = static_cast<int>( pItemElem->GetZckArmeRareLevel() );
						const ZckArmeRareProp* pRarity = pItemElem->IsZckArmeRareEligible() ? prj.GetZckArmeRare( nRarity ) : NULL;
						if( pRarity && nRarity > ZCK_ARME_RARE_MIN_LEVEL )
						{
							nItemMin = static_cast<int>( static_cast<float>( nItemMin ) * pRarity->m_fAttackRate );
							nItemMax = static_cast<int>( static_cast<float>( nItemMax ) * pRarity->m_fAttackRate );
						}
#endif // __ZCK_ARME_RARE

						nMin	+= nItemMin + nOptionVal;
						nMax	+= nItemMax + nOptionVal;
					}
#ifdef ADD_CHARACTER_INFO_DISPLAY
					else if( pItemProp->IsCostumeParts() )
					{
						int nOption = pItemElem->GetAbilityOption();
						float f		= GetItemMultiplier( pItemElem );

						nMin	+= (int)( pItemProp->dwAbilityMin * f );
						nMax	+= (int)( pItemProp->dwAbilityMax * f );

					}
#endif
				}
			}
		}
		// A¢¨A°∆A•ÏA°◊ AáAïA°ÌA®£AÄAå A©¯A¢ÆA©¯A•ÏA¢•AôA¢¨A®œ A°∆A°À A¢ØA¢¥A®˘AíA¢ØA¢Æ A®˘A¢“AÅA¢¥AÑA¢ÆA¢¨A| AÄA°ÌA¢ØA°ÏAáAëA¢•Aô.
		nMin = GetParam( DST_ABILITY_MIN, nMin );
		nMax = GetParam( DST_ABILITY_MAX, nMax );
		if( pnMin ) *pnMin = nMin;
		if( pnMax ) *pnMax = nMax;
		return ( ( nMin + nMax ) / 2 ); // A¢¨A¢ÁAÖAè A°∆A®£AÄA®¨ AÜA©˜A°æAï A°∆A®£AÄAåA¢•Aô. AÜAáAÅA¢¥A®ˆAÉA¢ØA¢ÆA¢•AÇ A°ÌA°◊A¢ØA°ÏAáAèAÅA¢“ A®˙AäA¢•AÇA¢•Aô. AÉA°ÀA°§AÇAÄAåA©¯A®£ A°æA°ÀAÖA¢¨ A¢ØA°ÏA•ÏA•ÏA°§Aé A°ÌA°◊A¢ØA°ÏAáAëA¢•Aô.
	}
	else
	{
	}
	
	return 0;
}

void CMover::SumEquipAdjValue( int* pnAdjHitRate, int* pnAdjParray )
{
	*pnAdjHitRate = 0;
	*pnAdjParray = 0;

	FLItemElem* pItemElem;
	for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
	{
		pItemElem = GetEquipItem( i );
		if( pItemElem )  
		{
			if( pItemElem->GetProp() )
			{
				if( (int)pItemElem->GetProp()->nAdjHitRate > 0 )
					*pnAdjHitRate += pItemElem->GetProp()->nAdjHitRate;
				if( (int)pItemElem->GetProp()->dwParry > 0 )
					*pnAdjParray += pItemElem->GetProp()->dwParry;
			}
		}
	}
}

// æ»∫∏¿Ã¥¬ ªÛ≈¬
BOOL CMover::SetDarkCover( BOOL bApply, DWORD tmMaxTime )
{
	if( bApply )
	{
		if( GetAdjParam( DST_IMMUNITY ) & CHS_INVISIBILITY )	// ≈ı∏Ì»≠ø° ∏Èø™¿Ã∂Û....±◊∑±∞Õµµ ¿÷≥™?
			return FALSE;	
		SetDestParam( DST_CHRSTATE, CHS_INVISIBILITY, NULL_CHGPARAM );
		m_wDarkCover =	(short)( (tmMaxTime / 1000.0f) * PROCESS_COUNT );
	} else
	{
		if( GetAdjParam( DST_CHRSTATE) & CHS_INVISIBILITY )
		{
			ResetDestParam( DST_CHRSTATE, CHS_INVISIBILITY, TRUE );		// æœ»ÊªÛ≈¬«ÿ¡¶
		}
		m_wDarkCover = 0;
	}

	return TRUE;
}

// Ω∫≈œªÛ≈¬∑Œ ∏∏µÈ±‚
BOOL CMover::SetStun( BOOL bApply, DWORD tmMaxTime )
{
	if( bApply )
	{
		if( GetAdjParam( DST_IMMUNITY ) & CHS_STUN ) 
			return FALSE;		// µ∂ø° ∏Èø™ªÛ≈¬¥Ÿ.

		SetDestParam( DST_CHRSTATE, CHS_STUN, NULL_CHGPARAM, FALSE );
		SendActMsg( OBJMSG_STUN );		// Ω∫≈œ ªÛ≈¬ Ω√¿€.
		m_wStunCnt = (short)( (tmMaxTime / 1000.0f) * PROCESS_COUNT );		// æÛ∏∂µøæ»¿Ã≥™ Ω∫≈œªÛ≈¬∞° µ«¥¿≥ƒ.
	}
	else
	{
		if( GetAdjParam( DST_CHRSTATE ) & CHS_STUN )	// ¿Ã∞≈ ∞ÀªÁæ»«œ∏È ∏≈π¯ ≈¨∂Û∑Œ ResetDestParam¿ª ≥Ø∏∞¥Ÿ.
			ResetDestParam( DST_CHRSTATE, CHS_STUN, FALSE);	// Ω∫≈œ ªÛ≈¬ «ÿ¡¶.
		m_wStunCnt = 0;
		m_dwFlag &= (~MVRF_NOACTION);				// æ◊º« ±›¡ˆ ªÛ≈¬∑Œ ¿¸»Ø.
	}

#ifdef __WORLDSERVER
	g_xWSUserManager->AddSetStun( this, tmMaxTime );
#endif // Worldserver

	return TRUE;
}

// this∏¶ µ∂ø° ∞…∏Æ∞‘ «—¥Ÿ.
// idAttacker : ∞¯∞›¿⁄(ƒ¡∆Æ∑—¿œºˆµµ ¿÷Ωø)
// tmMaxTime : 1/1000√  ¥‹¿ß.
// tmUnit : ∏Ó√  ∏∂¥Ÿ µ∂µ•πÃ¡ˆ∞° ±¿œ≤®≥ƒ.
// nDamage : µ•πÃ¡ˆ∑Æ
BOOL CMover::SetPoison( BOOL bApply, OBJID idAttacker, DWORD tmMaxTime, DWORD tmUnit, short wDamage )
{
	if( bApply )
	{
		if( GetAdjParam( DST_IMMUNITY ) & CHS_POISON ) 
			return FALSE;		// µ∂ø° ∏Èø™ªÛ≈¬¥Ÿ.
		SetDestParam( DST_CHRSTATE, CHS_POISON, NULL_CHGPARAM );
		if( tmMaxTime == -1 )
			FLERROR_LOG( PROGRAM_NAME, _T( "%s µ∂»ø∞˙¿« ¡ˆº”Ω√∞£¿Ã ¡ˆ¡§µ«¡ˆ æ ¿Ω" ), GetName() );
		if( tmUnit == -1 )
			FLERROR_LOG( PROGRAM_NAME, _T( "%s µ∂»ø∞˙¿« tick Ω√∞£¿Ã ¡ˆ¡§µ«¡ˆ æ ¿Ω" ), GetName() );
		if( wDamage == -1 )
			FLERROR_LOG( PROGRAM_NAME, _T( "%s µ∂»ø∞˙¿« ∆Ω¥Á µ•πÃ¡ˆ∞° ¡ˆ¡§µ«¡ˆ æ ¿Ω" ), GetName() );
		
		m_wPoisonCnt = (short)( (tmMaxTime / 1000.0f) * PROCESS_COUNT );			// 1/1000
		m_wPoisonDamage = wDamage;
		m_tmPoisonUnit = tmUnit;
		m_idPoisonAttacker = idAttacker;
	} else
	{
		if( GetAdjParam( DST_CHRSTATE) & CHS_POISON )
			ResetDestParam( DST_CHRSTATE, CHS_POISON, TRUE );	// µ∂ªÛ≈¬ «ÿ¡¶
		m_wPoisonCnt = 0;
		m_wPoisonDamage = 0;
		m_tmPoisonUnit = 0;
	}

	return TRUE;
}

// this∏¶ æœ»ÊªÛ≈¬ø° ∫¸¡ˆ∞‘ «—¥Ÿ
// tmMaxTime : æœ»Ê ¡ˆº”Ω√∞£(1/1000)
// nAdjHitRate : ¡ˆº”Ω√∞£µøæ» ºˆ¡§µ… ∏Ì¡ﬂ∑¸
// bApply : ¿˚øÎ/«ÿ¡¶
BOOL CMover::SetDark( BOOL bApply, DWORD tmMaxTime, int nAdjHitRate )
{
	if( bApply )
	{
		if( GetAdjParam( DST_IMMUNITY ) & CHS_DARK ) 
			return FALSE;		// æœ»Êø° ∏Èø™ªÛ≈¬¥Ÿ.
		SetDestParam( DST_CHRSTATE, CHS_DARK, NULL_CHGPARAM );
		SetDestParam( DST_ADJ_HITRATE, nAdjHitRate, NULL_CHGPARAM );
		m_wDarkCnt = (short)( (tmMaxTime / 1000.0f) * PROCESS_COUNT );
		if( nAdjHitRate == -1 )
			FLERROR_LOG( PROGRAM_NAME, _T( "%s æÓµ“»ø∞˙¿« ∏Ì¡ﬂ∑¸ ºˆ¡§ƒ°∞° ¡ˆ¡§µ«¡ˆ æ ¿Ω" ), GetName() );
		m_wDarkVal = static_cast< short >( nAdjHitRate );
	} else
	{
		if( GetAdjParam( DST_CHRSTATE) & CHS_DARK )
		{
			ResetDestParam( DST_CHRSTATE, CHS_DARK, TRUE );		// æœ»ÊªÛ≈¬«ÿ¡¶
			ResetDestParam( DST_ADJ_HITRATE, m_wDarkVal, TRUE );	// æœ»Êµøæ» ºˆ¡§µ∆¥¯ ∏Ì¡ﬂ∑¸ «ÿ¡¶
//			if( m_wDarkCnt >= 0 )
//				FLERROR_LOG( PROGRAM_NAME, _T( "SetDark : %d" ), m_wDarkVal );
		}
		m_wDarkCnt = 0;
		m_wDarkVal = 0;
	}
	return TRUE;
}

// this∏¶ √‚«˜¿ª ¿œ¿∏≈≤¥Ÿ. µ∂∞˙ ∫ÒΩ¡.
// tmMaxTime : 1/1000√  ¥‹¿ß.
// tmUnit : ∏Ó√  ∏∂¥Ÿ µ•πÃ¡ˆ∞° ±¿œ≤®≥ƒ.
// nDamage : µ•πÃ¡ˆ∑Æ
BOOL	CMover::SetBleeding( BOOL bApply, OBJID idAttacker, DWORD tmMaxTime, DWORD tmUnit, short wDamage )
{
	if( bApply )
	{
		if( GetAdjParam( DST_IMMUNITY ) & CHS_BLEEDING ) return FALSE;		// √‚«˜ø° ∏Èø™ªÛ≈¬¥Ÿ.
		
		SetDestParam( DST_CHRSTATE, CHS_BLEEDING, NULL_CHGPARAM );
		if( tmMaxTime == -1 )
			FLERROR_LOG( PROGRAM_NAME, _T( "%s √‚«˜»ø∞˙¿« ¡ˆº”Ω√∞£¿Ã ¡ˆ¡§µ«¡ˆ æ ¿Ω" ), GetName() );
		if( tmUnit == -1 )
			FLERROR_LOG( PROGRAM_NAME, _T( "%s √‚«˜»ø∞˙¿« tick Ω√∞£¿Ã ¡ˆ¡§µ«¡ˆ æ ¿Ω" ), GetName() );
		if( wDamage == -1 )
			FLERROR_LOG( PROGRAM_NAME, _T( "%s √‚«˜»ø∞˙¿« ∆Ω¥Á µ•πÃ¡ˆ∞° ¡ˆ¡§µ«¡ˆ æ ¿Ω" ), GetName() );
		
		m_wBleedingCnt = (short)( (tmMaxTime / 1000.0f) * PROCESS_COUNT );			// 1/1000
		m_wBleedingDamage = wDamage;
		m_tmBleedingUnit = tmUnit;
		m_idBleedingAttacker = idAttacker;
	} else
	{
		if( GetAdjParam( DST_CHRSTATE) & CHS_BLEEDING )
			ResetDestParam( DST_CHRSTATE, CHS_BLEEDING, TRUE );	// µ∂ªÛ≈¬ «ÿ¡¶
		m_wBleedingCnt = 0;
		m_wBleedingDamage = 0;
		m_tmBleedingUnit = 0;
	}
	return TRUE;
}

// dwState∏¶ «ÿ¡¶«‘
// CHS_ALL¿ª «œ∏È ∏µÁ µπˆ«¡.
void CMover::RemoveDebuff( DWORD dwState )
{
	// Ω∫≈≥∑Œ ∞…∏∞∞« ¿Ã∞…∑Œ «ÿ¡¶«ÿæﬂ«œ∞Ì
	RemoveChrStateBuffs( dwState );
	// Ω∫≈≥¿Ã æ∆¥— æ∆¿Ã≈€∞∞¿∫∞…∑Œ ªÛ≈¬∏∏ ∞…∏∞∞« ¿Ã∞…∑Œ «ÆæÓæﬂ «—¥Ÿ.
	if( dwState & CHS_DARK )
		SetDark( FALSE );
	if( dwState & CHS_POISON )
		SetPoison( FALSE );
	if( dwState & CHS_STUN )
		SetStun( FALSE );
	if( dwState & CHS_SLEEPING )
	{
		SetSleeping( false );
	}
//sun: 10, 
	if( dwState & CHS_BLEEDING )
		SetBleeding( FALSE );
	
	// TODO : ªÁøÎ«œ¡ˆ æ ¿Ω
// 	if( dwState & CHS_DEBUFFALL )
// 	{
// 		RemoveDstParamBuffs( DST_SPEED );
// 		RemoveDstParamBuffs( DST_CHR_CHANCEPOISON );
// 		RemoveDstParamBuffs( DST_CHR_CHANCESTUN );
// 		RemoveDstParamBuffs( DST_CHR_CHANCEBLEEDING );
// 	}

	// ¿Ã∞≈ æ»ø°º≠ Reset¿ª »£√‚«—¥Ÿ.
	SetDestParam( DST_CURECHR, dwState, NULL_CHGPARAM );
}

// thisø°∞‘ ∞…∏∞ πˆ«¡∏¶ 1∞≥ æ¯æÿ¥Ÿ.
void CMover::RemoveBuffOne( DWORD /*dwSkill*/ )
{
	m_buffs.RemoveBuffs( RBF_ONCE | RBF_GOODSKILL, 0 );
}	

void CMover::RemoveBuffAll( void )
{
	m_buffs.RemoveBuffs( RBF_GOODSKILL, 0 );
}




///////////////////////////////////////////////////////////////////////////////
// 
// µ∂, æœ»ÊµÓ √ﬂ∞°¥…∑¬ƒ°¿« ºº∆√¿∫ ¿Ã∞…∑Œ «œ¿⁄.
void CMover::SetDestParam( int nIdx, SkillProp *pProp, BOOL bSend )
{
	// TODO : range check nIdx
	SetDestParam( pProp->dwDestParam[nIdx], pProp->nAdjParamVal[nIdx], pProp->dwChgParamVal[nIdx], bSend );
}

void CMover::ResetDestParam( int nIdx, SkillProp *pProp, BOOL bSend )
{
	// TODO : range check nIdx
	ResetDestParam( pProp->dwDestParam[nIdx], pProp->nAdjParamVal[nIdx], bSend );
}

// ∏Ò«• ∆ƒ∂ÛπÃ≈Õ∏¶ º¬«—¥Ÿ.
void CMover::SetDestParam( int nItemIdx, BOOL bSend )
{
	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( nItemIdx );

	if( pItemProp == NULL )
	{
		return;
	}

	for( size_t Nth = 0; Nth < MAX_ITEM_PARAM; ++Nth )
	{
		if( pItemProp->dwDestParam[ Nth ] != NULL_ID )
		{
			SetDestParam( pItemProp->dwDestParam[ Nth ], pItemProp->nAdjParamVal[ Nth ], pItemProp->dwChgParamVal[ Nth ], bSend );
		}
	}

// 	if( pItemProp->dwDestParam1 != -1 )
// 		SetDestParam( pItemProp->dwDestParam1, pItemProp->nAdjParamVal1, pItemProp->dwChgParamVal1, bSend );
// 	if( pItemProp->dwDestParam2 != -1 )
// 		SetDestParam( pItemProp->dwDestParam2, pItemProp->nAdjParamVal2, pItemProp->dwChgParamVal2, bSend );
// //sun: 10, __PROP_0827
// 	if( pItemProp->dwDestParam3 != -1 )
// 		SetDestParam( pItemProp->dwDestParam3, pItemProp->nAdjParamVal3, pItemProp->dwChgParamVal3, bSend );
}

//--------------------------------------------------------------------------
//
// ∏Ò«• ∆ƒ∂Û∏ﬁ≈∏∏¶ º¬«—¥Ÿ.
void CMover::SetDestParam( int nDstParameter, int nAdjParameterValue, int nChgParameterValue, BOOL bSend )
{
	BOOL fSuccess	= FALSE;

//sun: 10¬˜ ¿¸Ω¬Ω√Ω∫≈€	Neuz, World, Trans
	if( (nDstParameter < 0 ) && ( nDstParameter != DST_ADJDEF_RATE && nDstParameter != DST_ADJDEF ) )
		return;
	
	// HP, MPµÓ¿∫ SetPointParam¿∏∑Œ √≥∏Æ«—¥Ÿ. ±Ÿµ SetDestParam «œ≥™∑Œ ≈Î¿œΩ√ƒ◊¿∏∏È ≥¥¡ˆ æ ¿ª±Ó?
	switch( nDstParameter )
	{
#ifdef __WORLDSERVER
	case DST_HP:
		{
			if( nAdjParameterValue == -1 )
			{
				SetPointParam( nDstParameter, GetMaxHitPoint() );
			}
			else
			{
				SetPointParam( nDstParameter, GetHitPoint() + nAdjParameterValue );
			}

			if( GetAdjParam( DST_IGNORE_DMG_PVP ) > 0 )
			{
				ResetDestParam( DST_IGNORE_DMG_PVP, 0, TRUE );
				RemoveDstParamBuffs( DST_IGNORE_DMG_PVP );
			}
		}
		return;
	case DST_MP:
		{
			if( nAdjParameterValue == -1 )
				SetPointParam( nDstParameter, GetMaxManaPoint() );
			else
				SetPointParam( nDstParameter, GetManaPoint() + nAdjParameterValue );
			
		}
		return;
	case DST_FP:
		{
			if( nAdjParameterValue == -1 )
				SetPointParam( nDstParameter, GetMaxFatiguePoint() );
			else
				SetPointParam( nDstParameter, GetFatiguePoint() + nAdjParameterValue );
		}
		return;
#else // __WORLDSERVER
	case DST_HP:	SetPointParam( nDstParameter, GetHitPoint() + nAdjParameterValue );	return;
	case DST_MP:	SetPointParam( nDstParameter, GetManaPoint() + nAdjParameterValue );	return;
	case DST_FP:	SetPointParam( nDstParameter, GetFatiguePoint() + nAdjParameterValue );	return;
#endif // __WORLDSERVER
	case DST_GOLD:	
#ifdef __PERIN_BUY_BUG	// chipi_091219 √ﬂ∞°
		FLERROR_LOG( PROGRAM_NAME, _T( "SetDestParam( DST_GOLD ) - [User:%s(%07d)], [Value:%d]" ), static_cast<FLWSUser*>(this)->GetName(), static_cast<FLWSUser*>(this)->m_idPlayer, nAdjParameterValue );
		return;
#else
		if( AddGold( nAdjParameterValue ) == FALSE )	
			SetGold( 0 );		// underflow¿Ã∏È 0∑Œ ∏∏µÁ¥Ÿ. ¿Ã¿¸ƒ⁄µÂøÕ »£»Øµ«∞‘ «—¥Ÿ. π∫∞° ¬Ú¬Ú 
		return;
#endif // __PERIN_BUY_BUG

	case DST_RESIST_ALL:
		SetDestParam( DST_RESIST_ELECTRICITY,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_RESIST_FIRE,			nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_RESIST_WIND,			nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_RESIST_WATER,			nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_RESIST_EARTH,			nAdjParameterValue, nChgParameterValue, bSend );
		return;
	case DST_STAT_ALLUP:
		SetDestParam( DST_STR,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_DEX,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_INT,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_STA,	nAdjParameterValue, nChgParameterValue, bSend );
		return;
	case DST_HPDMG_UP:	// DST_HP_MAX + DST_CHR_DMG 
		SetDestParam( DST_HP_MAX, nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_CHR_DMG, nAdjParameterValue, nChgParameterValue, bSend );
		return;
	case DST_DEFHITRATE_DOWN: // DST_ADJDEF + DST_ADJ_HITRATE
		SetDestParam( DST_ADJDEF, nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_ADJ_HITRATE, nAdjParameterValue, nChgParameterValue, bSend );
		return;
	case DST_LOCOMOTION:
		SetDestParam( DST_SPEED,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_JUMPING,	( nAdjParameterValue * 3 ), nChgParameterValue, bSend );
		return;

	case DST_HP_RECOVERY_RATE:
		{
			int nMax	= GetMaxOriginHitPoint();		// πˆ«¡¿« øµ«‚¿ª πﬁ¡ˆ æ ¿∫ ø¿∏Æ¡ˆ≥Ø æÁ
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_HP_RECOVERY, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_MP_RECOVERY_RATE:
		{
			int nMax	= GetMaxOriginManaPoint();		// πˆ«¡¿« øµ«‚¿ª πﬁ¡ˆ æ ¿∫ ø¿∏Æ¡ˆ≥Ø æÁ
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_MP_RECOVERY, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_FP_RECOVERY_RATE:
		{
			int nMax	= GetMaxOriginFatiguePoint();		// πˆ«¡¿« øµ«‚¿ª πﬁ¡ˆ æ ¿∫ ø¿∏Æ¡ˆ≥Ø æÁ
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_FP_RECOVERY, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_ALL_RECOVERY:
		{
			// HP
			SetDestParam( DST_HP_RECOVERY, nAdjParameterValue, NULL_CHGPARAM, bSend );
			// MP
			SetDestParam( DST_MP_RECOVERY, nAdjParameterValue, NULL_CHGPARAM, bSend );
			// FP
			SetDestParam( DST_FP_RECOVERY, nAdjParameterValue, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_ALL_RECOVERY_RATE:
		{
			// HP
			int nMax	= GetMaxOriginHitPoint();		// πˆ«¡¿« øµ«‚¿ª πﬁ¡ˆ æ ¿∫ ø¿∏Æ¡ˆ≥Ø æÁ
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_HP_RECOVERY, nRecv, NULL_CHGPARAM, bSend );

			// MP
			nMax	= GetMaxOriginManaPoint();		// πˆ«¡¿« øµ«‚¿ª πﬁ¡ˆ æ ¿∫ ø¿∏Æ¡ˆ≥Ø æÁ
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_MP_RECOVERY, nRecv, NULL_CHGPARAM, bSend );

			// FP
			nMax	= GetMaxOriginFatiguePoint();		// πˆ«¡¿« øµ«‚¿ª πﬁ¡ˆ æ ¿∫ ø¿∏Æ¡ˆ≥Ø æÁ
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_FP_RECOVERY, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_KILL_ALL:	
		{
			// HP
			SetDestParam( DST_KILL_HP, nAdjParameterValue, NULL_CHGPARAM, bSend );
			// MP
			SetDestParam( DST_KILL_MP, nAdjParameterValue, NULL_CHGPARAM, bSend );
			// FP
			SetDestParam( DST_KILL_FP, nAdjParameterValue, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_KILL_HP_RATE:
		{
			int nMax	= GetMaxOriginHitPoint();		// πˆ«¡¿« øµ«‚¿ª πﬁ¡ˆ æ ¿∫ ø¿∏Æ¡ˆ≥Ø æÁ
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_KILL_HP, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_KILL_MP_RATE:
		{
			int nMax	= GetMaxOriginManaPoint();		// πˆ«¡¿« øµ«‚¿ª πﬁ¡ˆ æ ¿∫ ø¿∏Æ¡ˆ≥Ø æÁ
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_KILL_MP, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_KILL_FP_RATE:
		{
			int nMax	= GetMaxOriginFatiguePoint();		// πˆ«¡¿« øµ«‚¿ª πﬁ¡ˆ æ ¿∫ ø¿∏Æ¡ˆ≥Ø æÁ
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_KILL_FP, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_KILL_ALL_RATE:
		{
			// HP
			int nMax	= GetMaxOriginHitPoint();		// πˆ«¡¿« øµ«‚¿ª πﬁ¡ˆ æ ¿∫ ø¿∏Æ¡ˆ≥Ø æÁ
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_KILL_HP, nRecv, NULL_CHGPARAM, bSend );
			
			// MP
			nMax	= GetMaxOriginManaPoint();		// πˆ«¡¿« øµ«‚¿ª πﬁ¡ˆ æ ¿∫ ø¿∏Æ¡ˆ≥Ø æÁ
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_KILL_MP, nRecv, NULL_CHGPARAM, bSend );
			
			// FP
			nMax	= GetMaxOriginFatiguePoint();		// πˆ«¡¿« øµ«‚¿ª πﬁ¡ˆ æ ¿∫ ø¿∏Æ¡ˆ≥Ø æÁ
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_KILL_FP, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_ALL_DEC_RATE:
		{
			SetDestParam( DST_MP_DEC_RATE, nAdjParameterValue, NULL_CHGPARAM, bSend );
			SetDestParam( DST_FP_DEC_RATE, nAdjParameterValue, NULL_CHGPARAM, bSend );
		}
		return;
	} // switch

	// ¿Ã ∞ÀªÁ∏¶ ø÷ ¿ßø°¥Ÿ æ»≥÷∞Ì ø©±‚¥Ÿ ≥÷æ˙≥ƒ∏È
	// DST_GOLD, PXP, RESIST_ALL∞∞¿∫∞ÊøÏ¥¬ MAX_ADJPARAMARY∫∏¥Ÿ ¥ı ≈´∞™¿ª ªÁøÎ«—¥Ÿ. 
	// ±◊∑°º≠ ±◊∑±∞ÕµÈ¿∫ ¿ßø°º≠ √≥∏Æ«œ∞Ì πŸ∑Œ ∏Æ≈œ«œ∞Ì
	// ¿œπ›¿˚¿Œ∞ÕµÈ∏∏ ¿Ã¬ ¿∏∑Œ ø¬¥Ÿ.
	if( nDstParameter >= MAX_ADJPARAMARY)
		return;
	
	if( nAdjParameterValue != 0 )	// 0¿Ã µÈæÓø¬∞Õµµ ¿ﬂ∏¯µ»∞≈¥œ±Ó ∞Ê∞Ì∏¶ ≥ªæﬂ «œ¡ˆ æ ¿ª±Ó ΩÕ¥Ÿ.
	{
		fSuccess	= TRUE;

		switch( nDstParameter )
		{
		case DST_CHRSTATE:		// ªÛ≈¬∫Ø∞Ê¿« ∞ÊøÏ¥¬
		case DST_IMMUNITY:
			if( nAdjParameterValue != NULL_ID )
			{
				if( nAdjParameterValue == 0xffffffff )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "SetDestParam : Adj == -1, %s" ), GetName() );
					return;
				}
				m_adjParamAry[nDstParameter] |= nAdjParameterValue;		// ∫Ò∆Æ∑Œ ø¬ø¿«¡ µ»¥Ÿ.
			} else
				fSuccess = FALSE;
			break;
		case DST_CURECHR:		// ªÛ≈¬«ÿ¡¶
			ResetDestParam( DST_CHRSTATE, nAdjParameterValue, bSend );
			return;
		case DST_REFLECT_DAMAGE:
			m_adjParamAry[nDstParameter] += nAdjParameterValue;		// µ•πÃ¡ˆ¿« ∏Ó%∏¶ µπ∑¡¡÷¥¬∞°?
			m_chgParamAry[nDstParameter] = nChgParameterValue;		// ∏Ó%¿« »Æ∑¸∑Œ πﬂµøµ«¥¬∞°?
			break;
		case DST_CHR_CHANCEBLEEDING:
		case DST_CHR_CHANCESTEALHP:
		case DST_CHR_CHANCEPOISON:
		case DST_CHR_CHANCEDARK:
		case DST_CHR_CHANCESTUN:
		case DST_AUTOHP:
			m_adjParamAry[nDstParameter] += nAdjParameterValue;		// ¬˘Ω∫ »Æ∑¸.	// %πÃ∏∏¿∏∑Œ »∏∫πµ«∞‘ «“≤®≥ƒ.
			m_chgParamAry[nDstParameter] = nChgParameterValue;		// ¬˘Ω∫∞° ∞…∑»¿ª∂ß ¡ˆº”Ω√∞£		// %HP∞° »∏∫πµ…≤®≥ƒ.
			break;
//sun: 9, 9-10¬˜ ∆Í
		case DST_HEAL:
			m_nHealCnt	= (short)( PROCESS_COUNT * 2.0f );		// ƒ≈∏¿” 2√ 
			m_adjParamAry[nDstParameter]	= nAdjParameterValue;	
			break;
		default:
			m_adjParamAry[nDstParameter] += nAdjParameterValue;		// ¥©¿˚
			break;
		}
	}

	else if( nChgParameterValue != 0x7FFFFFFF ) 
	{
		fSuccess	= TRUE;

		// Î≈
		if( nDstParameter == DST_SPEED )
		{
			if( m_chgParamAry[nDstParameter] == 0x7FFFFFFF )
				m_chgParamAry[nDstParameter]	= 0;
			m_chgParamAry[nDstParameter]++;
		}
		else
		{
			m_chgParamAry[nDstParameter] = nChgParameterValue;	// ∞Ì¡§µ» ∞™.
		}
	}
#ifdef __WORLDSERVER
	if( TRUE == ( fSuccess & bSend ) ) {
		g_xWSUserManager->AddSetDestParam( this, nDstParameter, nAdjParameterValue, nChgParameterValue );
	}
#endif	// __WORLDSERVER
}

//
// ∏Ò«• ∆ƒ∂Û∏ﬁ≈∏∏¶ ∏Æº¬«—¥Ÿ.
//
void CMover::ResetDestParam( int nDstParameter, int nAdjParameterValue, BOOL fSend )
{
	switch( nDstParameter )
	{
	case DST_RESIST_ALL:
		ResetDestParam( DST_RESIST_ELECTRICITY,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_RESIST_FIRE,		nAdjParameterValue, TRUE );
		ResetDestParam( DST_RESIST_WIND,		nAdjParameterValue, TRUE );
		ResetDestParam( DST_RESIST_WATER,		nAdjParameterValue, TRUE );
		ResetDestParam( DST_RESIST_EARTH,		nAdjParameterValue, TRUE );
		return;
	case DST_STAT_ALLUP:
		ResetDestParam( DST_STR,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_DEX,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_INT,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_STA,	nAdjParameterValue, TRUE );
		return;
	case DST_HPDMG_UP:	// DST_HP_MAX + DST_CHR_DMG 
		ResetDestParam( DST_HP_MAX, nAdjParameterValue, TRUE );
		ResetDestParam( DST_CHR_DMG, nAdjParameterValue, TRUE );
		return;
	case DST_DEFHITRATE_DOWN: // DST_ADJDEF + DST_ADJ_HITRATE
		ResetDestParam( DST_ADJDEF, nAdjParameterValue, TRUE );
		ResetDestParam( DST_ADJ_HITRATE, nAdjParameterValue, TRUE );
		return;
	case DST_LOCOMOTION:
		ResetDestParam( DST_SPEED,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_JUMPING,	( nAdjParameterValue * 3 ), TRUE );
		return;

	case DST_HP_RECOVERY_RATE:
		{
			int nMax	= GetMaxOriginHitPoint();		// πˆ«¡¿« øµ«‚¿ª πﬁ¡ˆ æ ¿∫ ø¿∏Æ¡ˆ≥Ø æÁ
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_HP_RECOVERY, nRecv, TRUE );
		}
		return;
	case DST_MP_RECOVERY_RATE:
		{
			int nMax	= GetMaxOriginManaPoint();		// πˆ«¡¿« øµ«‚¿ª πﬁ¡ˆ æ ¿∫ ø¿∏Æ¡ˆ≥Ø æÁ
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_MP_RECOVERY, nRecv, TRUE );
		}
		return;
	case DST_FP_RECOVERY_RATE:
		{
			int nMax	= GetMaxOriginFatiguePoint();		// πˆ«¡¿« øµ«‚¿ª πﬁ¡ˆ æ ¿∫ ø¿∏Æ¡ˆ≥Ø æÁ
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_FP_RECOVERY, nRecv, TRUE );
		}
		return;
	case DST_ALL_RECOVERY:
		{
			// HP
			ResetDestParam( DST_HP_RECOVERY, nAdjParameterValue, TRUE );
			// MP
			ResetDestParam( DST_MP_RECOVERY, nAdjParameterValue, TRUE );
			// FP
			ResetDestParam( DST_FP_RECOVERY, nAdjParameterValue, TRUE );
		}
		return;
	case DST_ALL_RECOVERY_RATE:
		{
			// HP
			int nMax	= GetMaxOriginHitPoint();		// πˆ«¡¿« øµ«‚¿ª πﬁ¡ˆ æ ¿∫ ø¿∏Æ¡ˆ≥Ø æÁ
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_HP_RECOVERY, nRecv, TRUE );
			
			// MP
			nMax	= GetMaxOriginManaPoint();		// πˆ«¡¿« øµ«‚¿ª πﬁ¡ˆ æ ¿∫ ø¿∏Æ¡ˆ≥Ø æÁ
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_MP_RECOVERY, nRecv, TRUE );
			
			// FP
			nMax	= GetMaxOriginFatiguePoint();		// πˆ«¡¿« øµ«‚¿ª πﬁ¡ˆ æ ¿∫ ø¿∏Æ¡ˆ≥Ø æÁ
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_FP_RECOVERY, nRecv, TRUE );
		}
		return;
	case DST_KILL_ALL:	
		{
			// HP
			ResetDestParam( DST_KILL_HP, nAdjParameterValue, TRUE );
			// MP
			SetDestParam( DST_KILL_MP, nAdjParameterValue, TRUE );
			// FP
			SetDestParam( DST_KILL_FP, nAdjParameterValue, TRUE );
		}
		return;
	case DST_KILL_HP_RATE:
		{
			int nMax	= GetMaxOriginHitPoint();		// πˆ«¡¿« øµ«‚¿ª πﬁ¡ˆ æ ¿∫ ø¿∏Æ¡ˆ≥Ø æÁ
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_KILL_HP, nRecv, TRUE );
		}
		return;
	case DST_KILL_MP_RATE:
		{
			int nMax	= GetMaxOriginManaPoint();		// πˆ«¡¿« øµ«‚¿ª πﬁ¡ˆ æ ¿∫ ø¿∏Æ¡ˆ≥Ø æÁ
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_KILL_MP, nRecv, TRUE );
		}
		return;
	case DST_KILL_FP_RATE:
		{
			int nMax	= GetMaxOriginFatiguePoint();		// πˆ«¡¿« øµ«‚¿ª πﬁ¡ˆ æ ¿∫ ø¿∏Æ¡ˆ≥Ø æÁ
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_KILL_FP, nRecv, TRUE );
		}
		return;
	case DST_KILL_ALL_RATE:
		{
			// HP
			int nMax	= GetMaxOriginHitPoint();		// πˆ«¡¿« øµ«‚¿ª πﬁ¡ˆ æ ¿∫ ø¿∏Æ¡ˆ≥Ø æÁ
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_KILL_HP, nRecv, TRUE );
			// MP
			nMax	= GetMaxOriginManaPoint();		// πˆ«¡¿« øµ«‚¿ª πﬁ¡ˆ æ ¿∫ ø¿∏Æ¡ˆ≥Ø æÁ
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_KILL_MP, nRecv, TRUE );
			// FP
			nMax	= GetMaxOriginFatiguePoint();		// πˆ«¡¿« øµ«‚¿ª πﬁ¡ˆ æ ¿∫ ø¿∏Æ¡ˆ≥Ø æÁ
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_KILL_FP, nRecv, TRUE );
		}
		return;
	case DST_ALL_DEC_RATE:
		{
			SetDestParam( DST_MP_DEC_RATE, nAdjParameterValue, TRUE );
			SetDestParam( DST_FP_DEC_RATE, nAdjParameterValue, TRUE );
		}
	}


	// ¿Ã ∞ÀªÁ∏¶ ø÷ ¿ßø°¥Ÿ æ»≥÷∞Ì ø©±‚¥Ÿ ≥÷æ˙≥ƒ∏È
	// DST_GOLD, PXP, RESIST_ALL∞∞¿∫∞ÊøÏ¥¬ MAX_ADJPARAMARY∫∏¥Ÿ ¥ı ≈´∞™¿ª ªÁøÎ«—¥Ÿ. 
	// ±◊∑°º≠ ±◊∑±∞ÕµÈ¿∫ ¿ßø°º≠ √≥∏Æ«œ∞Ì πŸ∑Œ ∏Æ≈œ«œ∞Ì
	// ¿œπ›¿˚¿Œ∞ÕµÈ∏∏ ¿Ã¬ ¿∏∑Œ ø¬¥Ÿ.
	
	if( nDstParameter >= MAX_ADJPARAMARY || nDstParameter < 0 )
	{
//		FLERROR_LOG( PROGRAM_NAME, _T( "ResetDestParam %s : ¿ﬂ∏¯µ» ∞™ nDstParam : %d" ), m_szName, nDstParameter );
		return;
	}
	if( nAdjParameterValue != 0 ) 
	{
		if( nDstParameter == DST_CHRSTATE )
		{
			m_adjParamAry[nDstParameter] &= (~nAdjParameterValue);		// «ÿ¥Á∫Ò∆Æ ø¿«¡.
		}
		else
		{
			if( m_adjParamAry[nDstParameter] != 0 )
			{
				m_adjParamAry[nDstParameter] += (-nAdjParameterValue);
			}
		}
	}

	// Î≈
	if( nAdjParameterValue == 0 )	//
	{
		if( nDstParameter == DST_SPEED && m_chgParamAry[nDstParameter] != 0x7FFFFFFF && m_chgParamAry[nDstParameter] > 0 )
		{
			if( --m_chgParamAry[nDstParameter] == 0 )
				m_chgParamAry[nDstParameter]		= 0x7FFFFFFF;
		}
		else
		{
			m_chgParamAry[nDstParameter]		= 0x7FFFFFFF;
		}
	}
#ifdef __WORLDSERVER
	if( fSend == TRUE ) 
	{
		if( nDstParameter == DST_SPEED )
			g_xWSUserManager->AddResetDestParamSync( this, nDstParameter, nAdjParameterValue, (int)m_adjParamAry[nDstParameter] );
		else
			g_xWSUserManager->AddResetDestParam( this, nDstParameter, nAdjParameterValue );
	}
#endif	// __WORLDSERVER
}

#ifdef __CLIENT
void CMover::ResetDestParamSync( int nDstParameter,int nAdjParameterValue, int nParameterValue, BOOL /*fSend*/ )
{
	if( nDstParameter >= MAX_ADJPARAMARY || nDstParameter < 0 )
	{
//		FLERROR_LOG( PROGRAM_NAME, _T( "ResetDestParam %s : ¿ﬂ∏¯µ» ∞™ nDstParam : %d" ), m_szName, nDstParameter );
		return;
	}

	switch( nDstParameter )
	{
		case DST_SPEED:
			{
				m_adjParamAry[nDstParameter] = nParameterValue;
				if( nAdjParameterValue == 0 && m_chgParamAry[nDstParameter] != 0x7FFFFFFF && m_chgParamAry[nDstParameter] > 0 )
				{
					if( --m_chgParamAry[nDstParameter] == 0 )
						m_chgParamAry[nDstParameter]		= 0x7FFFFFFF;
				}
			}
			break;
	}
}
#else
void CMover::ResetDestParamSync( int /*nDstParameter*/,int /*nAdjParameterValue*/, int /*nParameterValue*/, BOOL /*fSend*/ )
{

}

#endif	// __CLIENT

// ºˆ¡§ ∆ƒ∂ÛπÃ≈Õ∏¶ æÚ¥¬¥Ÿ.
int CMover::GetAdjParam( int nDestParameter )
{
	if( nDestParameter < MAX_ADJPARAMARY )
		return m_adjParamAry[ nDestParameter ];
	return 0;
}
// ±≥√º ∆ƒ∂ÛπÃ≈Õ∏¶ æÚ¥¬¥Ÿ.
int CMover::GetChgParam( int nDestParameter )
{
	if( nDestParameter < MAX_ADJPARAMARY )
		return m_chgParamAry[ nDestParameter ];
	return 0x7FFFFFFF;
}

// ∆ƒ∂ÛπÃ≈Õ∏¶ æÚ¥¬¥Ÿ.
int CMover::GetParam( int nDest, int nParam )
{
	int nChgParam = GetChgParam( nDest );
	if( nChgParam != 0x7FFFFFFF ) 
		return nChgParam;

	int nAdjParam = GetAdjParam( nDest );
	if( nAdjParam )	
		return nParam + nAdjParam;
	else
		return nParam;
}

/////////////////////////////////////////////////////////////////////////////
// ø©±‚∫Œ≈Õ ∆ƒ∂Û∏ﬁ≈∏ æÚ±‚ «‘ºˆµÈ 
// GetParam¿ª ªÁøÎ«œø© ºˆ¡§µ» ∞™¿ª ≥ª¡ÿ¥Ÿ.
//
// »˜∆Æ ∆˜¿Œ∆Æ æÚ±‚ 
int CMover::GetHitPoint()
{
//	return GetParam( DST_HP, m_nHitPoint );
	return min( GetParam( DST_HP, m_nHitPoint ), GetMaxHitPoint() );
}
// ∏∂≥™ ∆˜¿Œ∆Æ æÚ±‚ 
int CMover::GetManaPoint()
{
//	return GetParam( DST_MP, m_nManaPoint );
	return min( GetParam( DST_MP, m_nManaPoint ), GetMaxManaPoint() );
}
// ««¡ˆƒÆ ∆˜¿Œ∆Æ æÚ±‚
int CMover::GetFatiguePoint()
{
//	return GetParam( DST_FP, m_nFatiguePoint );
	return min( GetParam( DST_FP, m_nFatiguePoint ), GetMaxFatiguePoint() );
}

// √÷¥Î »˜∆Æ ∆˜¿Œ∆Æ æÚ±‚ 
int CMover::GetMaxHitPoint()
{
	if( IsInvalidObj(this) )	
		return 0;

	float factor = 1.0f;
	__int64 nResult = GetParam( DST_HP_MAX, GetMaxOriginHitPoint( FALSE ) );
	int nFactor = GetParam( DST_HP_MAX_RATE, 0 );

	factor += (float)nFactor/(float)100;
	int nCF = (int)(factor * 10000);
	nResult	= (nResult * nCF) / 10000;

	if( nResult < 1 )
		nResult		= 1;

	return (int)(nResult); 
}

// √÷¥Î ∏∂≥™ ∆˜¿Œ∆Æ æÚ±‚ 
int CMover::GetMaxManaPoint()
{
	float factor = 1.0f;
	int nResult = GetParam( DST_MP_MAX, GetMaxOriginManaPoint( FALSE ) );
	int nFactor = GetParam( DST_MP_MAX_RATE, 0 );
	
	factor += (float)nFactor/(float)100;
	nResult	= (int)( nResult * factor );

	if( nResult < 1 )
		nResult		= 1;

	return nResult; 
}

// √÷¥Î ««∑Œ ∆˜¿Œ∆Æ æÚ±‚
int CMover::GetMaxFatiguePoint()
{
	float factor = 1.0f;
	int nResult = GetParam( DST_FP_MAX, GetMaxOriginFatiguePoint( FALSE ) );
	int nFactor = GetParam( DST_FP_MAX_RATE, 0 );
	
	factor += (float)nFactor/(float)100;
	nResult	= (int)( nResult * factor );

	if( nResult < 1 )
		nResult		= 1;

	return nResult; 
}

int CMover::GetMaxPoint(int nDest)
{
	switch( nDest )
	{
		case DST_HP:
			return GetMaxHitPoint();
		case DST_MP:
			return GetMaxManaPoint();
		case DST_FP:
			return GetMaxFatiguePoint();
	}
	
	return 0;
}

int CMover::GetMaxOriginHitPoint( BOOL bOriginal )
{
	if( IsInvalidObj(this) )	
		return 0;

	if( IsPlayer() )
	{
		int nSta;
		if( bOriginal )
			nSta = m_Stat.GetSta();
		else
			nSta = GetSta();

		JobProp* pProperty = prj.GetJobProp( GetJob() ); 

		float a = (pProperty->fFactorMaxHP*m_nLevel)/2.0f;
		float b = a * ((m_nLevel+1.0f)/4.0f) * (1.0f + nSta/50.0f) + (nSta*10.0f) ;
		float maxHP = b + 80.f; 
		return (int)maxHP;
	}

	MoverProp *pMoverProp = GetProp();
#ifdef __S1108_BACK_END_SYSTEM
	//gmpbigsun( 20110106 ) : 
	float fRstRate = prj.m_fMonsterHitpointRate * pMoverProp->m_fHitPoint_Rate;
	int nRate = 0;
	if( fRstRate > 0.0f )
	{
		nRate = (int)(fRstRate * 10000);
	}

	__int64 n64Val = ((__int64)pMoverProp->dwAddHp * nRate) / 10000;
	return (int)n64Val;

	//		return int( pMoverProp->dwAddHp * prj.m_fMonsterHitpointRate * pMoverProp->m_fHitPoint_Rate ); 
#else // __S1108_BACK_END_SYSTEM
	return pMoverProp->dwAddHp; 
#endif // __S1108_BACK_END_SYSTEM
}

int CMover::GetMaxOriginManaPoint( BOOL bOriginal )
{
	int nInt = 0;
	if( bOriginal )
		nInt = m_Stat.GetInt();
	else
		nInt = GetInt();

	if( IsPlayer() )
	{
		// INT((((BaseLv*2) + (INT*8))*Job∞Ëºˆ) + 22)+(INT*Job∞Ëºˆ)) +  ®Á 
		// ®Á : æ∆¿Ã≈€ø°¿««— √ﬂ∞° ªÛΩ¬ (%¥‹¿ß) TDDO
		JobProp* pProperty = prj.GetJobProp( GetJob() ); 
		float factor = pProperty->fFactorMaxMP;

// 		int nMaxMP = (int)( ((((m_nLevel*2.0f) + ( nInt*8.0f))*factor) + 22.0f)+( nInt*factor) );

		float fMaxMP = ( ( ( ( (float)m_nLevel * 2.0f ) + ( (float)nInt * 8.0f ) ) * factor ) + ( (float)nInt * factor ) ) + 22.0f;

		return (int)fMaxMP;
	}
	return 	( ( m_nLevel * 2 ) + ( nInt * 8 ) + 22 );
}

int CMover::GetMaxOriginFatiguePoint( BOOL bOriginal )
{
	int nSta, nStr, nDex;
	if( bOriginal )
	{
		nSta = m_Stat.GetSta();
		nStr = m_Stat.GetStr();
		nDex = m_Stat.GetDex();
	}
	else
	{
		nSta = GetSta();
		nStr = GetStr();
		nDex = GetDex();
	}

	if( IsPlayer() )
	{
		// (((BaseLv*2) + (STA*6))*Job∞Ëºˆ) +(STA*Job∞Ëºˆ) + ®Á + ®Ë
		JobProp* pProperty = prj.GetJobProp( GetJob() ); 
		float factor = pProperty->fFactorMaxFP;

		int nMaxFP = (int)( (((m_nLevel*2.0f) + (nSta*6.0f))*factor) +(nSta*factor) );
		return nMaxFP;
	}
	
	return ( ( m_nLevel * 2 ) + ( nStr * 7 ) + ( nSta *2 ) + ( nDex * 4 ) );
}

int CMover::GetHitPointPercent( int nPercent )
{
	int nMax = GetMaxHitPoint();

	if( nMax == 0 ) 
		return 0;

	int nCurrentHitPoint( GetHitPoint() );

	if( nCurrentHitPoint > nMax )
		nCurrentHitPoint = nMax;

	return MulDiv( nCurrentHitPoint, nPercent, nMax );
}

int CMover::GetManaPointPercent( int nPercent )
{
	int nMax = GetMaxManaPoint();

	if( nMax == 0 )
		return 0;

	int nCurrentManaPoint( GetManaPoint() );

	if( nCurrentManaPoint > nMax )
		nCurrentManaPoint = nMax;
	
	return MulDiv( nCurrentManaPoint, nPercent, nMax );
}

int CMover::GetFatiguePointPercent( int nPercent )
{
	int nMax = GetMaxFatiguePoint();

	if( nMax == 0 ) 
		return 0;

	int nCurrentFatiguePoint( GetFatiguePoint() );

	if( nCurrentFatiguePoint > nMax )
		nCurrentFatiguePoint = nMax;


	return MulDiv( nCurrentFatiguePoint,  nPercent, nMax );
}

// HP »∏∫π ∞™ æÚ±‚
/*
	HP Recovery : »∏∫π∑Æ	PC/NPC : int(((«ˆ¿Á∑π∫ß + STA)/3.4)+2) + AddHpRecovery
	MP Recovery : »∏∫π∑Æ	PC/NPC : int(((«ˆ¿Á∑π∫ß + INT)/4)+2) + Addecovery
	FP Recovery : »∏∫π∑Æ	PC/NPC : int(((«ˆ¿Á∑π∫ß + STA + DEX)/4.5)+2) + AddHpRecovery

	HP Recovery : »∏∫πº”µµ	PC/NPC : »ﬁΩƒ(¡§¡ˆ)Ω√ 5√ ø° «—π¯æø »∏∫π«œ∏Á »ﬁΩƒ«œ¡ˆ æ ¥¬ ªÛ»≤ø°º≠¥¬ 12√ ø° «—π¯æø »∏∫π«‘ - AddHpRecoveryTime
	MP Recovery : »∏∫πº”µµ	PC/NPC : »ﬁΩƒ(¡§¡ˆ)Ω√ 5√ ø° «—π¯æø »∏∫π«œ∏Á »ﬁΩƒ«œ¡ˆ æ ¥¬ ªÛ»≤ø°º≠¥¬ 12√ ø° «—π¯æø »∏∫π«‘ - AddMpRecoveryTime
	FP Recovery : »∏∫πº”µµ	PC/NPC : »ﬁΩƒ(¡§¡ˆ)Ω√ 5√ ø° «—π¯æø »∏∫π«œ∏Á »ﬁΩƒ«œ¡ˆ æ ¥¬ ªÛ»≤ø°º≠¥¬ 12√ ø° «—π¯æø »∏∫π«‘ - AddFpRecoveryTime

  */
int CMover::GetHPRecovery()
{
	float fFactor = 1.0f;
	if( IsPlayer() )
	{
		JobProp* pProperty = prj.GetJobProp( GetJob() ); 
		fFactor = pProperty->fFactorHPRecovery;
	}

	//(∑π∫ß/3)+ (MaxHP/ (500* ∑π∫ß))+(STA*Job∞Ëºˆ)
	int nValue = (int)( (GetLevel() / 3.0f) + (GetMaxHitPoint() / (500.f * GetLevel())) + (GetSta() * fFactor) );

	nValue	= (int)( nValue - ( nValue * 0.1f ) ); // »∏∫π∑Æ 10% «œ«‚		// 9¬˜ √ﬂ∞° (≈∏∞›, ««∞› 10√  »ƒ ¿⁄µø »∏∫π)

	return GetParam( DST_HP_RECOVERY, nValue );
}
// MP »∏∫π ∞™ æÚ±‚
int CMover::GetMPRecovery()
{
	float fFactor = 1.0f;
	if( IsPlayer() )
	{
		JobProp* pProperty = prj.GetJobProp( GetJob() ); 
		fFactor = pProperty->fFactorMPRecovery;
	}

	// ((∑π∫ß*1.5)+(MaxMP/(500*∑π∫ß))+(INT*Job∞Ëºˆ))*0.2
	int nValue = (int)( ((GetLevel() * 1.5f) + (GetMaxManaPoint() / (500.f * GetLevel())) + (GetInt() * fFactor)) * 0.2f );

	nValue	= (int)( nValue - ( nValue * 0.1f ) ); // »∏∫π∑Æ 10% «œ«‚		// 9¬˜ √ﬂ∞° (≈∏∞›, ««∞› 10√  »ƒ ¿⁄µø »∏∫π)

	return GetParam( DST_MP_RECOVERY, nValue );
}
// FP »∏∫π ∞™ æÚ±‚ 
int CMover::GetFPRecovery()
{
	float fFactor = 1.0f;
	if( IsPlayer() )
	{
		JobProp* pProperty = prj.GetJobProp( GetJob() ); 
		fFactor = pProperty->fFactorFPRecovery;
	}
	// ((∑π∫ß*2)+(MaxFP/(500*∑π∫ß))+(STA*Job∞Ëºˆ))*0.2
	int nValue = (int)( ((GetLevel() * 2.0f) + (GetMaxFatiguePoint() / (500.f * GetLevel())) + (GetSta() * fFactor)) * 0.2f );

	nValue	= (int)( nValue - ( nValue * 0.1f ) ); // »∏∫π∑Æ 10% «œ«‚	// 9¬˜ √ﬂ∞° (≈∏∞›, ««∞› 10√  »ƒ ¿⁄µø »∏∫π)

	return GetParam( DST_FP_RECOVERY, nValue );
}

#ifdef __CLIENT
// ∏Ìº∫ ¿Ã∏ß¿ª Ω∫∆Æ∏µ¿∏∑Œ µπ∑¡¡ÿ¥Ÿ.
LPCTSTR CMover::GetFameName( void )
{
	switch( GetJob() )
	{
	// æ∆≈©∑ŒπÓ
	case JOB_ACROBAT:
	case JOB_JESTER:
	case JOB_RANGER:
	case JOB_JESTER_MASTER:
	case JOB_RANGER_MASTER:
	case JOB_JESTER_HERO:
	case JOB_RANGER_HERO:
		if( m_nFame >= 100000000 )	return prj.GetText( TID_GAME_ACR_FAME10 );
		else if( m_nFame >= 5000000 )	return prj.GetText( TID_GAME_ACR_FAME09 );
		else if( m_nFame >= 1000000 )	return prj.GetText( TID_GAME_ACR_FAME08 );
		else if( m_nFame >= 50000 )	return prj.GetText( TID_GAME_ACR_FAME07 );
		else if( m_nFame >= 20000 )	return prj.GetText( TID_GAME_ACR_FAME06 );
		else if( m_nFame >= 10000 )	return prj.GetText( TID_GAME_ACR_FAME05 );
		else if( m_nFame >= 4000 )	return prj.GetText( TID_GAME_ACR_FAME04 );
		else if( m_nFame >= 1000 )	return prj.GetText( TID_GAME_ACR_FAME03 );
		else if( m_nFame >= 100 )	return prj.GetText( TID_GAME_ACR_FAME02 );
		else if( m_nFame >= 10 )	return prj.GetText( TID_GAME_ACR_FAME01 );
		break;

	// ∏”º≠≥ ∏Æ¿œ∂ß
	case JOB_MERCENARY:	
	case JOB_KNIGHT:	
	case JOB_BLADE:
	case JOB_KNIGHT_MASTER:
	case JOB_BLADE_MASTER:
	case JOB_KNIGHT_HERO:	
	case JOB_BLADE_HERO:
		if( m_nFame >= 100000000 )	return prj.GetText( TID_GAME_MER_FAME10 );
		else if( m_nFame >= 5000000 )	return prj.GetText( TID_GAME_MER_FAME09 );
		else if( m_nFame >= 1000000 )	return prj.GetText( TID_GAME_MER_FAME08 );
		else if( m_nFame >= 50000 )	return prj.GetText( TID_GAME_MER_FAME07 );
		else if( m_nFame >= 20000 )	return prj.GetText( TID_GAME_MER_FAME06 );
		else if( m_nFame >= 10000 )	return prj.GetText( TID_GAME_MER_FAME05 );
		else if( m_nFame >= 4000 )	return prj.GetText( TID_GAME_MER_FAME04 );
		else if( m_nFame >= 1000 )	return prj.GetText( TID_GAME_MER_FAME03 );
		else if( m_nFame >= 100 )	return prj.GetText( TID_GAME_MER_FAME02 );
		else if( m_nFame >= 10 )	return prj.GetText( TID_GAME_MER_FAME01 );
		break;
	// ∏≈¡ˆº«
	case JOB_MAGICIAN:
	case JOB_PSYCHIKEEPER:	
	case JOB_ELEMENTOR:
	case JOB_PSYCHIKEEPER_MASTER:	
	case JOB_ELEMENTOR_MASTER:
	case JOB_PSYCHIKEEPER_HERO:	
	case JOB_ELEMENTOR_HERO:

		if( m_nFame >= 100000000 )	return prj.GetText( TID_GAME_MAG_FAME10 );
		else if( m_nFame >= 5000000 )	return prj.GetText( TID_GAME_MAG_FAME09 );
		else if( m_nFame >= 1000000 )	return prj.GetText( TID_GAME_MAG_FAME08 );
		else if( m_nFame >= 50000 )	return prj.GetText( TID_GAME_MAG_FAME07 );
		else if( m_nFame >= 20000 )	return prj.GetText( TID_GAME_MAG_FAME06 );
		else if( m_nFame >= 10000 )	return prj.GetText( TID_GAME_MAG_FAME05 );
		else if( m_nFame >= 4000 )	return prj.GetText( TID_GAME_MAG_FAME04 );
		else if( m_nFame >= 1000 )	return prj.GetText( TID_GAME_MAG_FAME03 );
		else if( m_nFame >= 100 )	return prj.GetText( TID_GAME_MAG_FAME02 );
		else if( m_nFame >= 10 )	return prj.GetText( TID_GAME_MAG_FAME01 );
		break;

	// æÓΩ√Ω∫∆Æ
	case JOB_ASSIST:
	case JOB_BILLPOSTER:	
	case JOB_RINGMASTER:
	case JOB_BILLPOSTER_MASTER:	
	case JOB_RINGMASTER_MASTER:
	case JOB_BILLPOSTER_HERO:	
	case JOB_RINGMASTER_HERO:
		if( m_nFame >= 100000000 )	return prj.GetText( TID_GAME_ASS_FAME10 );
		else if( m_nFame >= 5000000 )	return prj.GetText( TID_GAME_ASS_FAME09 );
		else if( m_nFame >= 1000000 )	return prj.GetText( TID_GAME_ASS_FAME08 );
		else if( m_nFame >= 50000 )	return prj.GetText( TID_GAME_ASS_FAME07 );
		else if( m_nFame >= 20000 )	return prj.GetText( TID_GAME_ASS_FAME06 );
		else if( m_nFame >= 10000 )	return prj.GetText( TID_GAME_ASS_FAME05 );
		else if( m_nFame >= 4000 )	return prj.GetText( TID_GAME_ASS_FAME04 );
		else if( m_nFame >= 1000 )	return prj.GetText( TID_GAME_ASS_FAME03 );
		else if( m_nFame >= 100 )	return prj.GetText( TID_GAME_ASS_FAME02 );
		else if( m_nFame >= 10 )	return prj.GetText( TID_GAME_ASS_FAME01 );		

		break;
	}
	return "";
}

#endif


// ∏∂π˝ ¿˙«‚∑¬ æÚ±‚ 
int CMover::GetResistMagic()
{
	return GetParam( DST_RESIST_MAGIC, 0 );
}
int CMover::GetResistSpell( int nDestParam )
{
	MoverProp *pProp = GetProp();
	if( pProp == NULL )
		FLERROR_LOG( PROGRAM_NAME, _T( "%d «¡∑Œ∆€∆º ¿–±‚ Ω«∆–" ), GetName() );

	int		nResist = 0;

	switch( nDestParam )
	{
	case DST_RESIST_ELECTRICITY:	nResist = pProp->nResistElecricity;		break;
	case DST_RESIST_FIRE:			nResist = pProp->nResistFire;		break;
	case DST_RESIST_WATER:			nResist = pProp->nResistWater;		break;
	case DST_RESIST_EARTH:			nResist = pProp->nResistEarth;		break;
	case DST_RESIST_WIND:			nResist = pProp->nResistWind;		break;
	default:
		FLERROR_LOG( PROGRAM_NAME, _T( "%s ∆ƒ∂Û∏ﬁ≈Õ ¿ﬂ∏¯µ  %d" ), GetName(), nDestParam );
		break;
	}
	return GetParam( nDestParam, nResist );
}

int CMover::GetNaturalArmor()
{
	return GetProp()->dwNaturalArmor;//GetParam(DST_NATURAL_ARMOR,GetProp()->m_dwNaturalArmor);
}

int CMover::GetStr()
{
	int nResult = m_Stat.GetStr() + GetParam( DST_STR, 0 );

	if( nResult < 1 )
		nResult	= 1;

	return nResult;
}
int CMover::GetDex()
{
	int nResult = m_Stat.GetDex() + GetParam( DST_DEX, 0 );

	if( nResult < 1 )
		nResult	= 1;

	return nResult;
}
int CMover::GetInt()
{
	int nResult = m_Stat.GetInt() + GetParam( DST_INT, 0 );

	if( nResult < 1 )
		nResult	= 1;

	return nResult;
}
int CMover::GetSta()
{
	int nResult = m_Stat.GetSta() + GetParam( DST_STA, 0 );

	if( nResult < 1 )
		nResult	= 1;

	return nResult;
}

bool CMover::HasZeroSpeedStateParam_hardcoding()
{
	const int nStateParam	= GetAdjParam( DST_CHRSTATE );
	return ( nStateParam & ( CHS_STUN | CHS_LOOT | CHS_SETSTONE | CHS_SLEEPING ) ) != 0;
}

FLOAT CMover::GetSpeed(FLOAT fSrcSpeed)
{
//sun: 9, 9-10¬˜ ∆Í
#ifdef __CLIENT
	if( m_dwAIInterface == AII_EGG && m_pAIInterface )
	{
		CAIEgg* pAI	= (CAIEgg*)m_pAIInterface;
		CMover* pOwner	= prj.GetMover( pAI->GetOwnerId() );
		if( IsValidObj( pOwner ) )
			return pOwner->GetSpeed( pOwner->m_pActMover->m_fSpeed );
	}
#endif	// __CLIENT

	if( HasZeroSpeedStateParam_hardcoding() == true )
		return 0;

	if( IsPlayer() == FALSE )
	{
		fSrcSpeed	*= GetSpeedFactor();		//sun: 9,	//__AI_0509
	}

	int nChgValue	= GetChgParam( DST_SPEED );
//	if( nChgValue != 0x7FFFFFFF )
//		return fSrcSpeed * (nChgValue / 100.0f);		// ex) fSrcSpeed * 0.5f

	// Î≈
	if( nChgValue != 0x7FFFFFFF )
		return 0;

	int nAdjValue	= GetAdjParam( DST_SPEED );
	if( nAdjValue != 0 )
	{
		fSrcSpeed = fSrcSpeed + ( fSrcSpeed * (nAdjValue / 100.0f) );
		if( fSrcSpeed < 0.0F )
			fSrcSpeed	= 0.0F;

		return fSrcSpeed;
	}
	if( fSrcSpeed < 0.0F )
		fSrcSpeed	= 0.0F;

	return fSrcSpeed;
}

BOOL CMover::IsCompleteQuest( int nQuestId )
{
	for( size_t i = 0; i < m_nCompleteQuestSize; i++ )
	{
		if( m_aCompleteQuest[ i ] == nQuestId )
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CMover::MakeCompleteQuest( int nQuestId, LPQUEST lpQuest )
{
	for( size_t i = 0; i < m_nCompleteQuestSize; i++ )
	{
		if( m_aCompleteQuest[ i ] == nQuestId )
		{
			ZeroMemory( lpQuest, sizeof( QUEST ) );
			lpQuest->m_wId = static_cast< WORD >( nQuestId );
			lpQuest->m_nState = QS_END;
			return TRUE;
		}
	}
	return FALSE;
}
void CMover::RemoveAllQuest()
{
	m_nQuestSize = 0;
	m_nCompleteQuestSize = 0;
	m_nCheckedQuestSize = 0;
}
void CMover::RemoveCompleteQuest()
{
	m_nCompleteQuestSize = 0;
}


LPQUEST CMover::FindQuest( int nQuestId )
{
	for( size_t i = 0; i < m_nQuestSize; i++ )
	{
		if( m_aQuest[ i ].m_wId == nQuestId )
		{
			return &m_aQuest[ i ];
		}
	}

	return NULL;
}
BOOL CMover::RemoveQuest( int nQuestId )
{
//	QuestProp* pProp = prj.m_aPropQuest.GetAt( nQuestId );

	for( size_t i = 0; i < m_nQuestSize; i++ )
	{
		if( m_aQuest[ i ].m_wId == nQuestId )
		{
			for( ; i < m_nQuestSize - 1; i++ )
			{
				m_aQuest[ i ] = m_aQuest[ i + 1 ];
			}
			m_nQuestSize--;
			break;
		}
	}
	for( size_t i = 0; i < m_nCompleteQuestSize; i++ )
	{
		if( m_aCompleteQuest[ i ] == nQuestId )
		{
			for( ; i < m_nCompleteQuestSize - 1; i++ )
			{
				m_aCompleteQuest[ i ] = m_aCompleteQuest[ i + 1 ];
			}
			m_nCompleteQuestSize--;
			break;
		}
	}
	for( size_t i = 0; i < m_nCheckedQuestSize; ++i )
	{
		if( m_aCheckedQuest[ i ] == nQuestId )
		{
			for( ; i < m_nCheckedQuestSize -1; ++i )
				m_aCheckedQuest[ i ] = m_aCheckedQuest[ i + 1 ];
			m_aCheckedQuest[ --m_nCheckedQuestSize ] = 0;
			break;
		}
	}

#ifdef __CLIENT
	PlayMusic( BGM_EV_END );
#endif
	return TRUE;
}

BOOL CMover::SetQuest( int nQuestId, int nState, LPQUEST lpReturnQuest )
{
	QUEST newQuest;
	ZeroMemory( &newQuest, sizeof( QUEST ) );
	newQuest.m_wId = static_cast< WORD >( nQuestId );
	newQuest.m_nState = static_cast< BYTE >( nState );

	LPQUEST lpQuest = FindQuest( nQuestId );
	// ¿ÃπÃ øœ∑·µ» ƒ…Ω∫∆Æ∂Û∏È ºº∆√ Ω«∆– 
	if( lpQuest == NULL && IsCompleteQuest( newQuest.m_wId ) )
		return FALSE;
	if( lpReturnQuest )
		memcpy( lpReturnQuest, &newQuest, sizeof( QUEST ) );
	return __SetQuest( lpQuest, &newQuest );
}
BOOL CMover::SetQuest( LPQUEST lpNewQuest )
{
	LPQUEST lpQuest = FindQuest( lpNewQuest->m_wId );
	// ¿ÃπÃ øœ∑·µ» ƒ…Ω∫∆Æ∂Û∏È ºº∆√ Ω«∆– 
	if( lpQuest == NULL && IsCompleteQuest( lpNewQuest->m_wId ) )
		return FALSE;
	return __SetQuest( lpQuest, lpNewQuest );
}
BOOL CMover::__SetQuest( LPQUEST lpQuest, LPQUEST lpNewQuest )
{
	// ƒ˘Ω∫∆Æ∏¶ πﬂ∞ﬂ ∏¯«ﬂ¿∏∏È ªı∑Œ √ﬂ∞°«—¥Ÿ.
	if( lpQuest == NULL )
	{
		if( m_nQuestSize >= MAX_DOING_QUEST )
		{
			return FALSE;
		}
#ifdef __CLIENT
		PlayMusic( BGM_EV_START );
#endif
		if( m_nCompleteQuestSize < MAX_COMPLETE_QUEST ) 
			lpQuest = &m_aQuest[ m_nQuestSize++ ];
		else
			return FALSE;

		QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( lpNewQuest->m_wId );
		if( pQuestProp && pQuestProp->m_nEndCondLimitTime  )
			lpNewQuest->m_wTime = static_cast< WORD >( pQuestProp->m_nEndCondLimitTime );
#ifdef __WORLDSERVER
		// ≈∏¿Ã∏” √ ±‚»≠ 
		m_timerQuestLimitTime.Reset();
#endif
	}
	else
	// ƒ˘Ω∫∆Æ øœ∑·¥Ÿ. ±◊∑∏¥Ÿ∏È ƒ≥Ω¨ø°º≠ µ⁄¡Æº≠ ¡¶∞≈«œ¿⁄ 
	if( lpNewQuest->m_nState == QS_END )
	{
		// ¥ÎΩ≈, øœ∑· ±ÀΩ∫∆Æ πËø≠ø° ≥÷æÓ ∫∏∞¸«—¥Ÿ.
		if( m_nCompleteQuestSize < MAX_COMPLETE_QUEST ) 
		{
			for( size_t i = 0; i < m_nQuestSize; i++ )
			{
				if( m_aQuest[ i ].m_wId == lpNewQuest->m_wId )
				{
					for( ; i < m_nQuestSize - 1; i++ )
					{
						m_aQuest[ i ] = m_aQuest[ i + 1 ];
					}
					m_nQuestSize--;
					break;
				}
			}

			for( size_t i = 0; i < m_nCheckedQuestSize; ++i )
			{
				if( m_aCheckedQuest[ i ] == lpNewQuest->m_wId )
				{
					for( ; i < m_nCheckedQuestSize - 1; ++i )
						m_aCheckedQuest[ i ] = m_aCheckedQuest[ i + 1 ];
					m_aCheckedQuest[--m_nCheckedQuestSize] = 0;
					break;
				}
			}

			QuestProp* pProp = prj.m_aPropQuest.GetAt( lpNewQuest->m_wId );
			// ƒ˘Ω∫∆Æ ≈∏¿‘¿Ã ¿«∑⁄º“∂Û∏È øœ∑· πËø≠ø° µÈæÓ∞°¡ˆ æ ¥¬¥Ÿ. 
			//if( pProp->m_nQuestType != QT_REQUEST && pProp->m_bRepeat == FALSE )
			if( pProp && pProp->m_bRepeat == FALSE )
				m_aCompleteQuest[ m_nCompleteQuestSize++ ] = lpNewQuest->m_wId; 
			lpQuest = NULL;
#if defined( __CLIENT )
			g_QuestTreeInfoManager.DeleteTreeInformation( lpNewQuest->m_wId );
			D3DXVECTOR3& rDestinationArrow = g_WndMng.m_pWndWorld->m_vDestinationArrow;
			rDestinationArrow = D3DXVECTOR3( -1.0F, 0.0F, -1.0F );
#endif // defined( __CLIENT )
		}
		else
			return FALSE;
	}
	if( lpQuest )
		memcpy( lpQuest, lpNewQuest, sizeof( QUEST ) );

#ifdef __CLIENT
	if( lpNewQuest->m_nState == QS_END )
		PlayMusic( BGM_EV_END );
#endif
	return TRUE;
}

LPQUEST CMover::GetQuest( int nQuestId )
{
	return FindQuest( nQuestId );
}

int CMover::GetItemNum( DWORD dwItemId )
{
	int nNum	= 0;
	DWORD dwSize	= m_Inventory.GetMax();
	FLItemElem* pItemElem;
	for( DWORD i = 0 ; i < dwSize; i++ )
	{
		pItemElem	= m_Inventory.GetAtId( i );
		if( pItemElem )
		{
			//raiders.06.05.02 (prevent abuse quest ending condition : trade & quest )
			if( IsUsableItem( pItemElem ) == FALSE )
				return 0;
			if( pItemElem->m_dwItemId == dwItemId )
				nNum	+= pItemElem->m_nItemNum;
		}
	}
	return nNum;
}

#ifdef __CLIENT
int CMover::GetItemNumForClient( DWORD dwItemId )
{
	int nNum	= 0;
	DWORD dwSize	= m_Inventory.GetMax();
	FLItemElem* pItemElem;
	for( DWORD i = 0 ; i < dwSize; i++ )
	{
		pItemElem	= m_Inventory.GetAtId( i );
		if( pItemElem )
		{
			if( pItemElem->m_dwItemId == dwItemId )
				nNum	+= pItemElem->m_nItemNum;
		}
	}
	return nNum;
}
#endif //__CLIENT
#ifdef __WORLDSERVER
// Ω∫≈©∏≥∆Æø°º≠ ªÁøÎ«œ¥¬ «‘ºˆ
// ƒ˘Ω∫∆Æø°º≠ æ∆¿Ã≈€¿ª ¡¶∞≈«“ ∂ß ªÁøÎ«—¥Ÿ. ƒ˘Ω∫∆Æ æ∆¿Ã≈€µÈ¿∫ ∞≈∑°≥™ ∞≥¿ŒªÛ¡°ø° ø√∏± ºˆ æ¯±‚ ∂ßπÆø°
// IsUsableItem∞ÀªÁ∏¶ «œ¡ˆ æ ¥¬¥Ÿ.
int CMover::RemoveItemA( DWORD dwItemId, int nNum )
{
	if( nNum == -1 )
	{
		return RemoveAllItem( dwItemId );
	}

	DWORD	dwSize	= m_Inventory.GetMax();
	FLItemElem* pItemElem;
	int nRemnant	= nNum;

	for( DWORD i = 0 ; i < dwSize && nRemnant > 0; i++ )
	{
		pItemElem	= m_Inventory.GetAtId( i );
		if( pItemElem && pItemElem->m_dwItemId == dwItemId )
		{
			if( nRemnant > pItemElem->m_nItemNum )
			{
				nRemnant	-= pItemElem->m_nItemNum;
				UpdateItem( i, UI_NUM, 0 );	// remove
			}
			else
			{
				UpdateItem( i, UI_NUM, pItemElem->m_nItemNum - nRemnant );
				nRemnant	= 0;
			}
		}
	}
	return ( nNum - nRemnant );
}
#endif	// __WORLDSERVER

// Ω∫≈©∏≥∆Æø°º≠ ªÁøÎ«œ¥¬ «‘ºˆ 
int CMover::RemoveAllItem( DWORD dwItemId )
{
	int nNum = 0;
	DWORD	dwSize	= m_Inventory.GetMax();

	for( DWORD i = 0 ; i < dwSize; ++i )
	{
		FLItemElem* pItemElem = m_Inventory.GetAtId( i );
		if( pItemElem && pItemElem->m_dwItemId == dwItemId )
		{
			nNum += pItemElem->m_nItemNum;
			UpdateItem( i, UI_NUM, 0 );		// remove
		}
	}
	return nNum;
}

void CMover::OnTradeRemoveUser()
{
	if( IsPlayer() )
	{
		CMover* pOther = m_vtInfo.GetOther();
		if( IsValidObj( pOther ) && pOther->IsPlayer() )
		{	
			if( pOther->m_vtInfo.GetOther() == this )
			{
				pOther->m_vtInfo.TradeClear();
			}
		}
		m_vtInfo.TradeClear();
	}
}

#ifdef __WORLDSERVER

void CMover::Abrade( CMover* /*pAttacker*/, int /*nParts*/ )
{
}

DWORD CMover::GetRandomPartsAbraded( void )
{
//	Ω«µÂ 50%
	int nRand	= xRand();
	if( nRand % 2 )
		return PARTS_SHIELD;
	nRand	= nRand % 100;
	if( nRand < 20 )	// 20%
		return PARTS_CAP;
	else if( nRand < 60 )	// 40%
		return PARTS_UPPER_BODY;
	else if( nRand < 85 )	// 25%
		return PARTS_HAND;
	else	// 15%
		return PARTS_FOOT;
}
#endif	// __WORLDSERVER



float CMover::GetItemEnduranceInfluence( int nEndurance )
{
	if( nEndurance == 0 )
		return 0.0f;
/*	else if( nEndurance <= 50 )
		return 0.6f;
	else if( nEndurance <= 75 )
		return 0.8f;
*/	return 1.0f;
}

int CMover::GetItemEnduranceWeight( int /*nEndurance*/ )
{
	int nMinusMultyPly = 5;
/*	if( nEndurance <= 50 )
		return (int)3 * nMinusMultyPly;
	else if( nEndurance <= 75 )
		return (int)2 * nMinusMultyPly;
*/	return 1 * nMinusMultyPly;
}

// //sun: 11, ∞¢º∫, √‡∫π
// void CMover::UpdateItemEx( unsigned char id, char cParam, __int64 iValue )
// {
// 	FLItemElem* pItemElem	= (FLItemElem*)GetItemId( id );
// 
// 	if( pItemElem )
// 	{
// 		switch( cParam )
// 		{
// 			case UI_RANDOMOPTITEMID:
// 				{
// 					pItemElem->SetRandomOptItemId( iValue );
// 					break;
// 				}
// 			default:
// 				break;
// 		}
// #ifdef __CLIENT
// 		CWndInventory* pWnd		= (CWndInventory*)g_WndMng.GetWndBase( APP_INVENTORY );
// 		if( pWnd )
// 			pWnd->UpdateTooltip();
// #endif	// __CLIENT
// 	}
// #ifdef __WORLDSERVER
// 	if( IsPlayer() )
// 		( (FLWSUser*)this )->AddUpdateItemEx( id, cParam, iValue );
// #endif
// }

void CMover::UpdateItem( DWORD dwId, CHAR cParam, DWORD dwValue, DWORD dwTime )
{
	FLItemBase* pItemBase = GetItemId( dwId );
	if( pItemBase == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ pItemBase is NULL ]" ) );
		return;
	}

	const T_ITEM_SPEC* pItemSpec	= pItemBase->GetProp();
	if( pItemSpec == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ pItemSpec is NULL ]" ) );
		return;
	}

	if( pItemBase )
	{
#ifdef __CLIENT
		//gmpbigsun( 20100706 ) : m_bControlForbidden AÄAå A°ÌA©¯AÅA¢ÆA¢ØA¢ÆA®˘A°© A®˙AÜAÄAåAÖAõAÄA°Ì AÜAàA°∆A|A¢ØA°˛ AÜAêAÖA¢“A©ˆAûA°æA°ÀAÄA®˘A¢ØA¢Æ AÅA¢“AÄAõAáAèA¢¨A®œA®˘A°© AÖA®œA°§A¢ÆA®ˆA°˛A•ÏAáA¢•AÇ AáA¢“A°ÌA©¯ A®˘A¢“AÅA¢¥A¢“A°◊A©ˆA¢ÁA¢ØA¢Æ A•ÏA®œA®˙A¢ÁA°∆A°ÃA•ÏA°© AáAèA¢•Aô.
        //AÜAêAÖA¢“AÄA°Ì A©ˆAûA®˙AÜA®˘A°© A®˙AÜAÄAåAÖAõAÄAå A®˙A°§A•ÏA°ÕAÄAåAÜA¢Á A•ÏAáA¢•AÇA°∆A|A¢ØA°˛ LockAÄA°Ì A¢ØA®œA°æA°ÀA®˘A°© AáA¢ÁA®˙A¢ÁAÅAñAÄAö.
		if( pItemBase->m_bControlForbidden )
			pItemBase->m_bControlForbidden = FALSE;
		//gmpbigsun end
#endif //__CLIENT

		switch( cParam )
		{
			case UI_COOLTIME:
			case UI_NUM:
				{
					if( (int)dwValue <= 0 ) 
					{
						RemoveItemId( dwId );
					}
					else
					{
						/////////////////////////////////////////////////////////////////////////////
						//	BEGIN100708
						if( dwValue > pItemSpec->dwPackMax )
						{
							FLERROR_LOG( PROGRAM_NAME, _T( "UI_NUM Error. m_idPlayer [%d], Serial[%d], dwId [%d], OBJID[%d], ItemID[%d]" ),
								m_idPlayer, pItemBase->GetSerialNumber(), dwId, pItemBase->m_dwObjId, pItemBase->m_dwItemId );

							RemoveItemId( dwId );
						}
						else
						{
							( (FLItemElem*)pItemBase )->m_nItemNum	= (int)( dwValue );
						}
						//	END100708
						/////////////////////////////////////////////////////////////////////////////
					}
					break;
				}
			case UI_HP:
				{
					( (FLItemElem*)pItemBase )->m_nHitPoint	= dwValue;
					UpdateParam();
					break;
				}
			case UI_REPAIR_NUM:
				{
					( (FLItemElem*)pItemBase )->m_nRepair	= dwValue;
					break;
				}
			case UI_RN:
				{
					if( ( (FLItemElem*)pItemBase )->m_nRepairNumber >= 100 )
						return;
					if( dwValue > 100 )
						dwValue		= 100;
					( (FLItemElem*)pItemBase )->m_nRepairNumber	= (BYTE)( dwValue );
					UpdateParam();
				}
				break;
				
			case UI_AO: // A®˙AÜAÄAåAÖAõ + A¢ØAÉA¢¨A¢ÁA°æA°À...
				{
					DWORD dwMax = 20;		//sun:13, AÅA|A°§AÉ AàA¢ÁAÄA°Õ(A®˘AìA®˘A®¨, AÄAèA©ˆAù)

//sun: 11, AÉA¢¥AÅA®ˆ A®ˆAÉA®ˆA®¨AÖAõ
					if( ( (FLItemElem*)pItemBase )->IsAccessory() == TRUE || pItemSpec->IsAccessory() == TRUE )
						dwMax	= 20;

//sun: 11, A°∆A°ÀA®˘A®¨, AÉA?A®¨A©ˆ
					if( ( (FLItemElem*)pItemBase )->IsCollector( TRUE ) )
						dwMax	= 5;

					if( ( (FLItemElem*)pItemBase )->GetAbilityOption() > (int)( dwMax ) )
						return;
					if( dwValue > dwMax )
						dwValue		= dwMax;

					( (FLItemElem*)pItemBase )->SetAbilityOption( dwValue );
					UpdateParam();
				}
				break;
			case UI_RAO: // A®˙AÜAÄAåAÖAõ A®˘AìA®˘A®¨ + A¢ØAÉA¢¨A¢ÁA°æA°À...
				{
					if( ( (FLItemElem*)pItemBase )->m_nResistAbilityOption > 20 )		//sun:13, AÅA|A°§AÉ AàA¢ÁAÄA°Õ(A®˘AìA®˘A®¨, AÄAèA©ˆAù)
						return;
//sun:13, AÅA|A°§AÉ AàA¢ÁAÄA°Õ(A®˘AìA®˘A®¨, AÄAèA©ˆAù)
					if( dwValue > 20 )
						dwValue		= 20;

					( (FLItemElem*)pItemBase )->m_nResistAbilityOption	= dwValue;

					if( ( (FLItemElem*)pItemBase )->m_nResistAbilityOption <= 0 )
					{
						( (FLItemElem*)pItemBase )->m_byItemResist	= SAI79::NO_PROP;
					}
					UpdateParam();
				}
				break;
			case UI_IR:  // A®˙AÜAÄAåAÖAõA¢ØA¢Æ A©ˆA°ÏA®ˆA®˘A®˘AìA®˘A®¨AÄAéAÅA¢“ A©¯AñA°æA°À
				{
					( (FLItemElem*)pItemBase )->m_byItemResist	= (BYTE)( dwValue );
					UpdateParam();
				}
				break;

			case UI_PIERCING_SIZE:
				if( static_cast<FLItemElem*>( pItemBase )->IsEatPet() ) {
					return;
				}
				( (FLItemElem*)pItemBase )->SetGeneralPiercingSize( dwValue );
				break;
			case UI_PIERCING:
				{
					if( static_cast<FLItemElem*>( pItemBase )->IsEatPet() ) {
						return;
					}
					WORD wIndex	= LOWORD( dwValue ), wItemId	= HIWORD( dwValue );
					( (FLItemElem*)pItemBase )->SetGeneralPiercingItemID( wIndex, wItemId );
				}
				break;

//sun: 12, A©ˆA°ÏA°æA°À AáAáA®˙A¢ÁA®ˆAå
			case UI_ULTIMATE_PIERCING_SIZE:
				( (FLItemElem*)pItemBase )->SetUltimatePiercingSize( dwValue );
				break;
			case UI_ULTIMATE_PIERCING:
				{
					WORD wIndex	= LOWORD( dwValue ), wItemId	= HIWORD( dwValue );
					( (FLItemElem*)pItemBase )->SetUltimatePiercingItemID( wIndex, wItemId );
				}
				break;

			case UI_PETVIS_SIZE:	
				{
					static_cast<FLItemElem*>(pItemBase)->SetGeneralPiercingSize( dwValue );
					static_cast<FLItemElem*>(pItemBase)->SetVisPetPiercingSize( dwValue );

#ifdef __CLIENT	//sun : A®ˆAáAÉA®˘ A®¨A°æA®ˆA®¨ AÄA°ÕAÇA¢¨AÄAå( A®˘A°©A©ˆA¢“A°§AéA®¨AéAÖAç AÜAêAÖA¢“AÄA°Ì A©ˆAûA®˙AÜA®˘A°© AÉA©¯A¢¨A¢ÁA•ÏAâA¢“A°◊ )AÄAåA°§A°◊A®˙A¢ÁAÅA®¨A¢“A°◊ AàA¢ØA°∆A®¨AÄA®ˆ AÉA°ÀA°§AÇ 
					PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
#endif // __CLIENT
				}
				break;
			case UI_PETVIS_ITEM:
				{
					WORD wIndex	= LOWORD( dwValue ), wItemId	= HIWORD( dwValue );
					static_cast<FLItemElem*>(pItemBase)->SetGeneralPiercingItemID( wIndex, wItemId );
					static_cast<FLItemElem*>(pItemBase)->SetVisPetPiercingItemKeepTime( wIndex, ( dwTime * 60 ) + time_null() ); // A®¨AêA¢•AúAÄA°◊ A®ˆAÉA°∆A°ÃAÄA°Ì AÉAäA¢•AúAÄA°◊A°§Aé A®¨A?A°∆A|

#ifdef __CLIENT
					PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
#endif // __CLIENT
				}
				break;

			case UI_PETVIS_ITEMSWAP:
				{
					WORD wPos1	= LOWORD( dwValue ), wPos2	= HIWORD( dwValue );
					static_cast<FLItemElem*>(pItemBase)->SwapVisItem( wPos1, wPos2 );

#ifdef __CLIENT
					PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
#endif // __CLIENT
				}
				break;

			case UI_TRANSFORM_VISPET:
				{
					static_cast<FLItemElem*>(pItemBase)->m_bTranformVisPet = static_cast<BOOL>( dwValue );
					static_cast<FLItemElem*>(pItemBase)->m_bCharged = TRUE;
				}
				break;

			case UI_RANDOMOPTITEMID:
				{
					static_cast<FLItemElem*>( pItemBase )->SetRandomOptionOriginID( static_cast<WORD>( dwValue ) );
					break;
				}
			case UI_KEEPTIME:	// offset
				{
					CTime tm	= CTime::GetCurrentTime() + CTimeSpan( 0, 0, dwValue, 0 );
					static_cast<FLItemElem*>( pItemBase )->m_dwKeepTime	= static_cast<DWORD>( tm.GetTime() );
					break;
				}

			case UI_FLAG:
				{
#ifdef __CLIENT
					DWORD dwObjIndex	= static_cast<DWORD>( LOWORD( dwValue ) );
					BYTE byFlag		= static_cast<BYTE>( HIWORD( dwValue ) );
					int nParts	= dwObjIndex;
					if( nParts >= 0 && nParts < MAX_HUMAN_PARTS )
						m_aEquipInfo[nParts].byFlag	= byFlag;
					static_cast<FLItemElem*>( pItemBase )->m_byFlag		= byFlag;
					UpdateParts( FALSE );
#endif	// __CLIENT
					UpdateParam();
					break;
				}

			case UI_EQUIP_LEVEL:
				{
					if( static_cast<int>( dwValue ) > 0 )
					{
						static_cast<FLItemElem*>( pItemBase )->SetLevelDown( dwValue );
					}
					else
					{
						static_cast<FLItemElem*>( pItemBase )->InitializeLevelDown();
					}
				}
				break;

			case UI_LOOKS_CHANGE:
				{
					dwValue > 0 ? static_cast<FLItemElem*>( pItemBase )->SetLooksChangeItemID( dwValue )
								: static_cast<FLItemElem*>( pItemBase )->InitializeLooksChangeItemID();
				}
				break;

			default:
				break;
		}
//sun: 9AÇA°§ AÄA®˘A®ˆAÇA°∆A®˘A°§AÉ Clienet
#if defined(__ZCK_SFX_AMELIORER)
		if( cParam == UI_AO || cParam == UI_RAO || cParam == UI_IR )
		{
			FLItemElem* pItemElem = static_cast<FLItemElem*>( pItemBase );
			if( RefreshZckSfxEquipInfo( pItemElem ) )
			{
#ifdef __CLIENT
				if( m_pModel )
					UpdateParts( FALSE );

				CWndInventory* pWndInventory = static_cast<CWndInventory*>( g_WndMng.GetWndBase( APP_INVENTORY ) );
				if( pWndInventory )
					pWndInventory->UpdateParts();
#endif // __CLIENT
#ifdef __WORLDSERVER
				if( IsPlayer() )
				{
					for( int nPart = 0; nPart < MAX_HUMAN_PARTS; ++nPart )
					{
						if( m_Inventory.GetEquip( nPart ) == pItemElem )
							g_xWSUserManager->AddZckSfxEquipInfo( this, nPart, m_aEquipInfo[nPart] );
					}
				}
#endif // __WORLDSERVER
			}
		}
#endif // __ZCK_SFX_AMELIORER

#ifdef __CLIENT
		CWndInventory* pWnd	= (CWndInventory*)g_WndMng.GetWndBase( APP_INVENTORY );
		if( pWnd )
			pWnd->OnMouseWndSurface( pWnd->GetMousePoint() );
		//	pWnd->UpdateTooltip();

#endif	// __CLIENT
	}
	else
	{
		switch( cParam )
		{
			case UI_FLAG:
				{
#ifdef __CLIENT
					DWORD dwObjIndex	= (DWORD)(short)LOWORD( dwValue );
					BYTE byFlag		= (BYTE)HIWORD( dwValue );
					int nParts	= dwObjIndex;
					if( nParts >= 0 && nParts < MAX_HUMAN_PARTS )
					{
						m_aEquipInfo[nParts].byFlag	= byFlag;
						UpdateParts( TRUE );
					}
#endif	// __CLIENT
					break;
				}
			default:
				break;
		}
	}
#ifdef __WORLDSERVER
	if( IsPlayer() )
	{
		static_cast<FLWSUser*>( this )->AddUpdateItem( 0, dwId, cParam, dwValue, dwTime );
		if( pItemBase && cParam == UI_TRANSFORM_VISPET && HasActivatedEatPet() )
		{
			InactivateEatPet();
			ActivateEatPet( static_cast<FLItemElem*>( pItemBase ) );
		}
	}
#endif // __WORLDSERVER
}

float CMover::GetItemDropRateFactor( CMover* /*pAttacker*/ )
{
	float fFactor	= 1.0F;
#ifdef __WORLDSERVER
	fFactor		*= prj.m_fItemDropRate;
#ifdef __S1108_BACK_END_SYSTEM
	fFactor		*= GetProp()->m_fItemDrop_Rate;
#endif // __S1108_BACK_END_SYSTEM

	fFactor		*= CEventGeneric::GetInstance()->GetItemDropRateFactor();

	//fFactor	*= prj.m_EventLua.GetDropFactor() + nsCooperativeContributions::GLOBAL_REWARD_ALARM_DROP_RATE().GetRewardValue();	//sun: 9, ¿Ã∫•∆Æ (∑Áæ∆ Ω∫≈©∏≥∆Æ ¿˚øÎ)
	const float fMulRate = ( prj.m_EventLua.GetItemDropRate() - 1.0f ) + ( ( nsCooperativeContributions::GLOBAL_REWARD_ALARM_DROP_RATE().GetRewardValue() / ( float )REWARD_PRECISION ) - 1.0f ) + 1.0f;
	fFactor			*= fMulRate;

#endif	// __WORLDSERVER
	return fFactor;
}

float CMover::GetPieceItemDropRateFactor( CMover* pAttacker )
{
	float fFactor	= 1.0F;

#ifdef __WORLDSERVER
	fFactor		*= prj.m_EventLua.GetPieceItemDropRate();		//sun: 9, ¿Ã∫•∆Æ (∑Áæ∆ Ω∫≈©∏≥∆Æ ¿˚øÎ)

//sun: 12, ±∫¡÷
	ILordEvent* pEvent		= CSLord::Instance()->GetEvent();
	fFactor		*= pEvent->GetIFactor();
	if( pAttacker->HasBuff( BUFF_ITEM, ITEM_INDEX( 20019, II_SYS_SYS_LS_CHEERING ) ) )
		fFactor		*=	1.1F;
	if( pAttacker->HasBuff( BUFF_ITEM, ITEM_INDEX( 20036, II_SYS_SYS_VOTE_THANKS ) ) )
		fFactor		*= 1.05F;

//sun: 13, ƒø«√ ∫∏ªÛ
	if( pAttacker->HasBuff( BUFF_ITEM, ITEM_INDEX( 26740, II_COUPLE_BUFF_MIRACLE_01 ) ) )
		fFactor	*= 1.05F;

	if( pAttacker->IsPlayer() )
		fFactor *= CPCBang::GetInstance()->GetPieceItemDropFactor( static_cast<FLWSUser*>( pAttacker ) );
#endif // __WORLDSERVER
	return fFactor;
}

#ifdef __WORLDSERVER
float CMover::GetExpFactor( void )
{
	float fFactor	= 1.0f;
	if( IsMode( EVENT_OLDBOY_MODE ) )
		fFactor	*= 1.1f;

	BOOL bGlvCheer	= FALSE;

	int nValue = g_xApplyItemEffect->GetAdjValueDuplicationBuff( (FLWSUser*)this, IK3_EXP_RATE );

	if( nValue > 0 )
	{
		fFactor *= ( (float)( (float)nValue / 100.0f ) + 1.0f );
	}

	if( HasBuff( BUFF_ITEM, ITEM_INDEX( 10467, II_SYS_SYS_SCR_GLVCHEER ) ) )
	{
		bGlvCheer	= TRUE;
		fFactor	*= 1.10f;
	}

	if( !bGlvCheer && HasBuff( BUFF_ITEM, ITEM_INDEX( 10445, II_CHEERUP ) ) )
	{
#ifdef __EVENTLUA_CHEEREXP
		if( prj.m_EventLua.GetCheerExpFactor() > 1.05f )
			fFactor *= prj.m_EventLua.GetCheerExpFactor();
		else
#endif // __EVENTLUA_CHEEREXP
			fFactor	*= 1.05f;
	}

#ifdef __WORLDSERVER

	fFactor	*= CEventGeneric::GetInstance()->GetExpFactor();
	//fFactor	*= prj.m_EventLua.GetExpFactor() + nsCooperativeContributions::GLOBAL_REWARD_ALARM_EXP_FACTOR().GetRewardValue();	//sun: 9, ¿Ã∫•∆Æ (∑Áæ∆ Ω∫≈©∏≥∆Æ ¿˚øÎ)
	//const float fMulRate = ( prj.m_EventLua.GetExpFactor() - 1.0f ) + ( nsCooperativeContributions::GLOBAL_REWARD_ALARM_EXP_FACTOR().GetRewardValue() - 1.0f ) + 1.0f;
	const float fMulRate	= ( prj.m_EventLua.GetExpFactor() - 1.0f ) + ( ( nsCooperativeContributions::GLOBAL_REWARD_ALARM_EXP_FACTOR().GetRewardValue() / ( float )REWARD_PRECISION ) - 1.0f ) + 1.0f;
	fFactor			*= fMulRate;

	fFactor	*= prj.m_fMonsterExpRate;

//sun: 9,10¬˜ ¡¶∑√ __ULTIMATE
	int nDstExp = GetParam( DST_EXPERIENCE, 0 );
	if( 0 < nDstExp )
		fFactor *= ( 1.0f + ( nDstExp / 100.0f ) );

//sun: 12, ±∫¡÷
	ILordEvent* pEvent		= CSLord::Instance()->GetEvent();
	fFactor		*= pEvent->GetEFactor();
	if( HasBuff( BUFF_ITEM, ITEM_INDEX( 20019, II_SYS_SYS_LS_CHEERING ) ) )
		fFactor		*=	1.1F;
	if( HasBuff( BUFF_ITEM, ITEM_INDEX( 20036, II_SYS_SYS_VOTE_THANKS ) ) )
		fFactor		*=	1.05F;

	if( g_pSeasonEffect->getEffect() == true && g_pSeasonEffect->getSeason() != SEASON_NONE )
	{
		fFactor *= prj.m_EventLua.GetWeatherEventExpFactor();
	}

#endif	// __WORLDSERVER

	return fFactor;
}
#endif // __WORLDSERVER


bool	CMover::SetSleeping( bool bApply, DWORD dwDuration )
{
	if( bApply == true )
	{
		MoverProp* pMoverProp = GetProp();

		if( pMoverProp != NULL )
		{
			if( GetAdjParam( DST_IMMUNITY ) & CHS_SLEEPING ) 
			{
				return false;
			}

			SetDestParam( DST_CHRSTATE, CHS_SLEEPING, NULL_CHGPARAM, FALSE );
			SendActMsg( OBJMSG_SLEEPING );
			m_dwSleepingDuration = (DWORD)( ( dwDuration / 1000.0f ) * PROCESS_COUNT );
		}
	}
	else
	{
		if( GetAdjParam( DST_CHRSTATE ) & CHS_SLEEPING )
		{
			ResetDestParam( DST_CHRSTATE, CHS_SLEEPING, FALSE );
		}
		m_dwSleepingDuration = 0;
		m_dwFlag &= (~MVRF_NOACTION);
	}

#ifdef __WORLDSERVER
	g_xWSUserManager->AddSetSleeping( this, dwDuration );
#endif // Worldserver

	return true;
}

bool	CMover::IsNoActionState()
{
	if( GetAdjParam( DST_CHRSTATE ) & CHS_SLEEPING )
	{
		return true;
	}

	if( GetAdjParam( DST_CHRSTATE ) & CHS_STUN )
	{
		return true;
	}

	return false;
}

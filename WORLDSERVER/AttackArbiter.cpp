#include "stdafx.h"
#include <ActionMover.h>
#include "User.h"
#include "AttackArbiter.h"
#include <guild.h>
extern	CGuildMng			g_GuildMng;

#include <party.h>
extern	CPartyMng	g_PartyMng;


#ifdef __EVENT_MONSTER
#include <EventMonster.h>
#endif // __EVENT_MONSTER

#include <CreateMonster.h>

#include "FLCC_Reward.h"

#include "../_Common/FLSkillSystem.h"
#include "FLEventArenaGlobal.h"
#ifdef __ZCK_INVASIONS
#include "../_Common/ZckInvasions.h"
#endif // __ZCK_INVASIONS
#ifdef __ZCK_RECORD
#include "ZckRecordManager.h"
#endif // __ZCK_RECORD
#include "../_AIInterface/FLFSM.h"

//////////////////////////////////////////////////////////////////////
// ATTACK_INFO
//////////////////////////////////////////////////////////////////////

ATK_TYPE ATTACK_INFO::GetAtkType() const
{
	ATK_TYPE type = ATK_GENERIC;

	if( dwAtkFlags & AF_MELEESKILL )
	{
		type = ATK_MELEESKILL;
	}
	else if( dwAtkFlags & AF_MAGICSKILL )
	{
		type = ATK_MAGICSKILL;
	}	
	else if( dwAtkFlags & AF_MAGIC )		// ¿œπ› øœµÂ ∞¯∞›
	{
		type = ATK_MAGIC;
	}
	else if( dwAtkFlags & AF_FORCE )		// π›ªÁ µ•πÃ¡ˆ∞∞¿∫ ∞ÊøÏ 
	{
		type = ATK_FORCE;
	}

	return type;
}

// πÊæÓ∑¬ π´Ω√¿« ∞ÊøÏ¿Œ∞°?
BOOL ATTACK_INFO::CanIgnoreDEF() const		
{
	int nSkill = GetSkill();
	switch ( nSkill )
	{
	case SKILL_INDEX( 159, SI_BIL_PST_ASALRAALAIKUM ) :
		return TRUE;
	case SKILL_INDEX( 212, SI_JST_YOYO_HITOFPENYA ) :
		return TRUE;
	}

	if( dwAtkFlags & AF_FORCE )		// πÃ∏Æ∞ËªÍ µ» µ•πÃ¡ˆ¿« ∞ÊøÏ, πÊæÓ∑¬ π´Ω√ (øπ: π›ªÁ, ªÁ¿Ã≈± ø˘)  
		return TRUE;
	return FALSE;
}

int ATTACK_INFO::GetChargeLevel() const 
{
	if( IsSkillAttack( dwAtkFlags ) )
		return 0;
	
	return ( nParam & 0xFF);			// øœµÂ√Ê¿¸∑Æ
}

int ATTACK_INFO::GetSkillLevel() const 
{
	FLASSERT( IsSkillAttack( dwAtkFlags ) );
	return ( nParam & 0xFF);	
}

int ATTACK_INFO::GetAttackCount() const 
{
	return ( nParam >> 8) & 0xFF;		// ¿œπ› 0, ¡ˆº”µ•πÃ¡ˆ 1¿ÃªÛ 
}

int ATTACK_INFO::GetSkill() const
{
	if( IsSkillAttack( dwAtkFlags ) )
		return ( nParam >> 16) & 0xFFFF;	// Ω∫≈≥¿Ãæ˙¥Ÿ∏È Ω∫≈≥ID
	else
		return 0;
}

BOOL ATTACK_INFO::IsRangeAttack() const
{
	return ( dwAtkFlags & AF_RANGE );
}

//////////////////////////////////////////////////////////////////////
// CAttackArbiter
//////////////////////////////////////////////////////////////////////

CAttackArbiter::CAttackArbiter( DWORD dwMsg, CMover* pAttacker, CMover* pDefender, DWORD dwAtkFlags, int nParam, BOOL bTarget, int nReflect )
		: m_dwMsg( dwMsg ), 
		  m_pAttacker( pAttacker ), 
		  m_pDefender( pDefender ), 
		  m_dwAtkFlags( dwAtkFlags ), 
		  m_nParam( nParam ),
		  m_bTarget( bTarget ),
		  m_nReflect( nReflect )		  
{
	m_nReflectDmg = 0;
}

CAttackArbiter::~CAttackArbiter()
{
}

int CAttackArbiter::OnDamageMsgW()
{
	if( !CheckValidDamageMsg() )
	{
		return 0;
	}

	ATTACK_INFO info;
	info.dwAtkFlags = m_dwAtkFlags;
	info.pAttacker  = m_pAttacker;
	info.pDefender  = m_pDefender;
	info.nParam     = m_nParam;
	info.nParts     = PARTS_RWEAPON;

	int nDamage = 0;
	BYTE cbHandFlag	= GetHandFlag();
	for( BYTE cbFlag = 0x01; cbFlag <= 0x02; ++cbFlag )	
	{
		if( cbHandFlag & cbFlag )
		{
			info.nParts = (cbFlag & 0x01) ? PARTS_RWEAPON : PARTS_LWEAPON;
#ifdef __EVENTLUA_SPAWN
			const BOOL bAIOnekillMode	= m_pAttacker->m_pFSM != NULL && ( m_pAttacker->m_pFSM->GetOptionalFlags().bOneKillMode == TRUE );

			if( m_pDefender->IsNPC() && (prj.m_EventLua.IsEventSpawnMonster( m_pDefender->GetIndex() ) && ( !m_pDefender->IsRank( RANK_SUPER ) && !m_pDefender->IsRank( RANK_MIDBOSS ) && !m_pDefender->IsRank( RANK_MIDBOSS )))
				&& !m_pAttacker->IsMode( ONEKILL_MODE )
				&& bAIOnekillMode == FALSE )
			{
				nDamage = 1;
				continue;
			}
#endif // __EVENTLUA_SPAWN
			int n = CalcDamage( &info );					// °Õi¢ÆI®œoIAo°À°˛| ¢Æ®˙°À°˛CN°À°ÕU. (m_dwAtkFlags°Õi°Õi °ß°˛?¢Æ®°®œ¢Æ°ÕiE °ßuo AO°À°ÕU) 
			if( n > 0 )
			{
				if( m_pAttacker->IsPlayer() == TRUE && m_pDefender->IsPlayer() == TRUE )
				{
					if( m_pDefender->GetAdjParam( DST_IGNORE_DMG_PVP ) == 0 )
					{
						ProcessAbnormal( n, &info );			// °ßo°ß°˛AI, °ßuIE®œ¢Æ, °Õi°ÀO, ®œoY¢Æic ¢ÆioAAA®œ©™°À°˛°Àc
						StealHP( n, info.GetAtkType() );		// EiC¢ÆA
					}
				}
				else
				{
					ProcessAbnormal( n, &info );			// °ßo°ß°˛AI, °ßuIE®œ¢Æ, °Õi°ÀO, ®œoY¢Æic ¢ÆioAAA®œ©™°À°˛°Àc
					StealHP( n, info.GetAtkType() );		// EiC¢ÆA
				}

				nDamage += n;
			}
		}
	}

	if( m_pAttacker->IsPlayer() && m_pDefender->IsPlayer() )
	{
		nDamage -= (nDamage * m_pDefender->GetParam( DST_PVP_DMG_RATE, 0 ) / 100);
		if( !m_pAttacker->IsGuildCombatTarget( m_pDefender ) )
		{
			int nDamageDecRate = m_pAttacker->GetPKValue() * 5;
			if( nDamageDecRate > 99 )
				nDamageDecRate = 99;

			nDamage -= (int)( nDamage * ( (float)nDamageDecRate/(float)100) );
		}
	}

	DWORD dwFakeDamage = (DWORD)nDamage;
	if( m_pAttacker->IsPlayer() == TRUE && m_pDefender->IsPlayer() == TRUE )
	{
		int nMaxGuardPVPDamage = m_pDefender->GetAdjParam( DST_IGNORE_DMG_PVP );
		if( nMaxGuardPVPDamage > 0 )
		{
			IBuff* pBuff	= m_pDefender->m_buffs.GetBuffByDestParam( DST_IGNORE_DMG_PVP );
			if( pBuff != NULL )
			{
				int nOnPVPDamage = pBuff->GetAdjValue();
				nOnPVPDamage += nDamage;
				if( nOnPVPDamage < 0 )
				{
					m_pDefender->ResetDestParam( DST_IGNORE_DMG_PVP, 0, TRUE );
					m_pDefender->RemoveDstParamBuffs( DST_IGNORE_DMG_PVP );
				}
				else if( nOnPVPDamage >= nMaxGuardPVPDamage )
				{
					nDamage = nOnPVPDamage - nMaxGuardPVPDamage;
					m_pDefender->ResetDestParam( DST_IGNORE_DMG_PVP, 0, TRUE );
					m_pDefender->RemoveDstParamBuffs( DST_IGNORE_DMG_PVP );
				}
				else
				{
					pBuff->SetAdjValue( nOnPVPDamage );
					nDamage = 0;
				}
			}
		}
	}


	nDamage = max( nDamage, 1 );
	int nHP = MinusHP( &nDamage );							// HP ¢Æ®°¢Æ°◊°ßuO °ßoAA°À°Õ 

	if( CMonsterSkill::GetInstance()->MonsterTransform( m_pDefender, nHP ) )
		return 0;

	if( m_pDefender->m_pActMover->IsSit() )					// °ßuE¢Æ®˙aC°ß®£A| & AI°Õi°À?A¢Æi °À°˛°ß®£Aa 
		m_pDefender->m_pActMover->ResetState( OBJSTA_MOVE_ALL );

	// AI°Õi°À?A®œ°˛AI°ÀO°ßI °ßu°ßu°À?iAo °ßuE°À°ÕA°À°ÕU.
	if( m_pDefender->m_pActMover->IsMove() == FALSE )
	{
		m_pDefender->m_pActMover->SendActMsg( OBJMSG_STAND );
	}

	m_pDefender->OnAttacked( m_pAttacker, nDamage, m_bTarget, m_nReflect );		// ¢Æ®°®œ®£¢Æ®°YAU CC¢Æ®°YAU AuAa, ¢Æ®°®œ¢ÆCeA°À¢Á Ao¢Æ®˙°ß°©A®œ°¿ °ß°˛?¢Æ®°®œ¢Æ	
#ifdef __ZCK_WORLD_BOSS
	m_pDefender->AddDamageByAttacker( m_pAttacker, static_cast<__int64>( nDamage ) );
#endif // __ZCK_WORLD_BOSS
#ifdef __ZCK_RECORD
	if( m_pAttacker->IsPlayer() && nDamage > 0 )
		CZckRecordManager::GetInstance()->AddDamage( static_cast<FLWSUser*>( m_pAttacker ), m_pDefender->GetId(), static_cast<__int64>( nDamage ) );
#endif // __ZCK_RECORD
#ifdef __ZCK_INVASIONS
	CZckInvasions::GetInstance()->AddUserDamage( m_pAttacker, m_pDefender, nDamage );
#endif // __ZCK_INVASIONS
	if( nHP > 0 )
	{
		OnDamaged( nDamage, dwFakeDamage );

		// MinusHP()°À?°À¢Á°ßu¢Æ®œ ¢Æ®°°À¢ÁA°Àc°À?E - °Õi¢ÆI®œoIAo AIEA E°À°˛°ß°˛®œo°ßoAAN°ßu®œ°˛ CN°À°ÕU.
		int nActionHPRate = m_pDefender->GetAdjParam( DST_AUTOHP );							// ®œo®œ°˛°Õi°À? HPA®œ°¿
		if( nActionHPRate > 0 )
		{
			float fHPPercent = (float)nHP / m_pDefender->GetMaxHitPoint();					// CoAc HPAC °ß¢ÆU°ßu°ßu°ß¢Æ°Àc A®œ°¿
			if( fHPPercent <= (nActionHPRate / 100.0f) )									// CoAc HP¢Æ®°°À¢Á ®œo®œ°˛°Õi°À?HPAICI¢Æ®°°À¢Á °ÕiC°À°˛e
			{
				float fAddHP = m_pDefender->GetChgParam( DST_AUTOHP ) / 100.0f;				// E°À°˛°ß°˛®œo°ßoAA®œ©™ HPA®œ°¿
				int nAddHP = (int)( m_pDefender->GetMaxHitPoint() * fAddHP );				// MaxHPAC AIA°À¢•°ß°˛nA®œ°¿A¢Æi AeCO.
				nHP += nAddHP;
				m_pDefender->SetPointParam( DST_HP, nHP, TRUE );									// HPAo¢Æ®°°À¢Á.
				g_xWSUserManager->AddCreateSfxObj( m_pDefender, XI_INDEX( 62, XI_GEN_CURE01 ) );					// E°À°˛°ß°˛®œo E°À?¢Æ®°u ®œo®œ°˛¢Æiy.
				m_pDefender->RemoveBuff( BUFF_SKILL, SKILL_INDEX( 48, SI_ASS_HEAL_PREVENTION ) );	// C°ß®£A|.
			}
		}

		if( m_nReflectDmg )
		{
			m_pAttacker->m_pActMover->SendDamage( AF_FORCE, m_pDefender->GetId(), m_nReflectDmg, TRUE, 2 );
			m_nReflectDmg = 0;
		}
	}
	else	
		OnDied();											// ¢Æ®°®œ¢ÆCeA°À¢Á, °ßu°ß¢ÆAIAU °Õia°ÀO®œ®£ 
	

	m_pDefender->EndPVP(nHP);

	return nDamage;
}

// µ•πÃ¡ˆ∏¶ ¿‘±‚ ¿ß«— ¡∂∞«¿ª √Ê¡∑«œ¥¬∞°? TRUE - √Ê¡∑ 
BOOL CAttackArbiter::CheckValidDamageMsg()
{
#ifdef __EVENT_MONSTER
	// ¿Ã∫•∆Æ ∏ÛΩ∫≈Õ¿Ã∞Ì ∑π∫ß¿Ã 15≥—∞‘ ¬˜¿Ã≥™∏È MISS
	if( m_pAttacker->IsPlayer() && !m_pDefender->IsPlayer() )
	{
		MoverProp* lpMoverProp = m_pDefender->GetProp();
		if( lpMoverProp && CEventMonster::GetInstance()->SetEventMonster( lpMoverProp->dwID ) )
		{
			if( m_pAttacker->GetLevel() - m_pDefender->GetLevel() >= CEventMonster::GetInstance()->GetLevelGap() )
				m_dwAtkFlags = AF_MISS;
		}
	}
#endif // __EVENT_MONSTER

	if( m_pAttacker->IsPlayer() && !CCreateMonster::GetInstance()->IsAttackAble( static_cast<FLWSUser*>(m_pAttacker), m_pDefender, TRUE ) )
	{
		m_dwAtkFlags = AF_MISS;
		m_pDefender->m_idAttacker = NULL_ID;
		m_pDefender->m_idTargeter = NULL_ID;
	}

	if( m_dwAtkFlags & AF_MISS ) 
	{
		m_pDefender->PostAIMsg( AIMSG_DAMAGE, m_pAttacker->GetId(), 0 );
        g_xWSUserManager->AddDamage( m_pDefender, GETID( m_pAttacker ), 0, m_dwAtkFlags );
		return FALSE;
	}

	// ∏¬¥¬Ω√¡°ø°µµ Ω∫∆ø√º≈©∏¶ «ÿæﬂ«—¥Ÿ.
	// µøΩ√ø° Ω∫≈≥¿ª ΩË¿ª∂ß πﬂµø«œ¥¬Ω√¡°ø° Ω∫≈≥¿ª ªÁøÎ«œ±‚∂´ø° µ—¥Ÿ Ω∫∆øªÛ≈¬∞° æ∆¥—√§∑Œ µÈæÓø¿±‚ ∂ßπÆ¿Ã¥Ÿ.
	// πﬂµø«œ¥¬ Ω√¡°ø°µµ ø™Ω√ Ω∫∆ø√º≈©∏¶ «ÿæﬂ«œ¥¬∞Õ¿∫ ≥ª∏˜¿ª µ˝≥¿Ã ∞¯∞›«œ¥¬ ∏º«¿Ã æ∆øπ ≥™ø¿¡ˆ æ ±‚ ¿ß«ÿº≠ πÃ∏Æ √º≈© «ÿæﬂ«œ±‚∂ßπÆ.
	if( !g_eLocal.GetState( EVE_STEAL ) )
	{
		if( m_pAttacker->IsSteal( m_pDefender ) )	// m_pAttacker∞° Ω∫∆ø«œ∑¡ «œ¥¬∞°?
			return FALSE;					
	}

	return TRUE;
}

int CAttackArbiter::CalcATK( ATTACK_INFO* pInfo )
{
	int nATK = 0;
	int nCount = pInfo->GetAttackCount();		// AÄAèA©ˆAù 0, AÅA¢“A®˘AìA•ÏA°ÕA©ˆAåAÅA¢“ 1AÄAåA°ÌA©¯ 

	switch( pInfo->GetAtkType() )
	{
	case ATK_FORCE:								// A©ˆAåA¢¨A¢ÁA°∆A°ßA°ÌA®£A•ÏAà A•ÏA°ÕA©ˆAåAÅA¢“ (A©ˆAùA°ÌA°◊, A°ÌA°◊AÄAåAÖA°æ A¢ØA©ˆ)
		nATK = m_nParam;						
		nCount = 0;		
		break;
	case ATK_MELEESKILL:						// A©ˆAêA¢¨A¢ÁA®ˆA®¨AÖA©¯ 
		nATK = m_pAttacker->GetMeleeSkillPower( pInfo );
		break;
	case ATK_MAGICSKILL:						// A¢¨AÖAÅA°§ A®ˆA®¨AÖA©¯ 
		nATK = m_pAttacker->GetMagicSkillPower( pInfo );
		break;
	case ATK_MAGIC:								// A¢ØAèA•ÏA°Õ A°∆A¢¨A°∆Aù 
		nATK = m_pAttacker->GetMagicHitPower( pInfo->GetChargeLevel() );
		break;
	case ATK_GENERIC:							// AÄAèA©ˆAù A°∆A¢¨A°∆Aù 
		nATK = m_pAttacker->GetHitPower( pInfo );
		break;
	}

	nATK	= (int)( nATK * m_pAttacker->GetATKMultiplier( m_pDefender, pInfo->dwAtkFlags ) );	// A®˘A¢¨A®˘A¢“A°∆A¢¨A°∆AùA°§AÇAÄA°Ì AÅA•ÏAÜA¢¨AáAëA¢•Aô. 	
	if( nCount > 0 )															// AÅA¢“A®˘AìA•ÏA°ÕA©ˆAåAÅA¢“A¢•AÇ A°∆A°ßA°ÌA®£A•ÏAà ATK 10%
		nATK	= (int)( nATK * 0.1f );

	nATK	= static_cast<int>( static_cast<float>( nATK ) * GetAdjustDamageFactor( *pInfo ) );

	nATK	+= m_pAttacker->GetParam( DST_ATKPOWER, 0 );

	if( _GetContentState( CT_COMPOSE_TWOWEAPON19 ) == CS_VER1 )
	{
		FLItemElem* pItemElem	= m_pAttacker->GetWeaponItem( pInfo->nParts );
		if( pItemElem != NULL )
		{
			nATK	+= pItemElem->GetCombinedAddDamage();
		}
	}

	if( m_pAttacker->IsPlayer() )
	{
		nATK += prj.m_EventLua.GetAttackPower() + nsCooperativeContributions::GLOBAL_REWARD_ALARM_ATTACK_POWER().GetRewardValue();
	}

#ifdef __ZCK_ARME_RARE
	if( pInfo->GetAtkType() != ATK_FORCE &&
		( pInfo->nParts == PARTS_RWEAPON || pInfo->nParts == PARTS_LWEAPON ) )
	{
		FLItemElem* pZckWeapon = m_pAttacker->GetWeaponItem( pInfo->nParts );
		if( pZckWeapon && pZckWeapon->IsZckArmeRareEligible() )
		{
			const int nRarity = static_cast<int>( pZckWeapon->GetZckArmeRareLevel() );
			const ZckArmeRareProp* pRarity = prj.GetZckArmeRare( nRarity );
			if( pRarity && nRarity > ZCK_ARME_RARE_MIN_LEVEL )
				nATK = static_cast<int>( static_cast<float>( nATK ) * pRarity->m_fAttackRate );
		}
	}
#endif // __ZCK_ARME_RARE

	if( nATK < 0 )
		nATK	= 0;

	return nATK;
}


// µ•πÃ¡ˆ∏¶ ±∏«—¥Ÿ. (m_dwAtkFlagsµµ ∫Ø∞Êµ… ºˆ ¿÷¥Ÿ)
int CAttackArbiter::CalcDamage( ATTACK_INFO* pInfo )
{
	if( m_pAttacker->m_dwMode & ONEKILL_MODE )	// ø¯≈≥ ∏µÂ
		return m_pDefender->GetHitPoint();	

	if( m_pAttacker->m_pFSM != NULL && m_pAttacker->m_pFSM->GetOptionalFlags().bOneKillMode == TRUE )
		return m_pDefender->GetHitPoint();	

	int nDamage = PostCalcDamage( CalcATK( pInfo ), pInfo );
	m_dwAtkFlags = pInfo->dwAtkFlags;

	return nDamage;
}

int CAttackArbiter::OnAfterDamage( ATTACK_INFO* pInfo, int nDamage )
{
	CMover* pAttacker = pInfo->pAttacker;
	CMover* pDefender = pInfo->pDefender;

	if( pAttacker->IsPlayer() && nDamage > 0 )
	{
		if( pDefender->GetAdjParam( DST_CHRSTATE ) & CHS_DOUBLE )	// ¥ı∫Ìµ•πÃ¡ˆ ªÛ≈¬ ∞…∑»¥Ÿ
			pDefender->RemoveChrStateBuffs( CHS_DOUBLE );
	}

	if( pDefender->GetAdjParam( DST_CHRSTATE) & CHS_ATK_COUNTERATTACK  ) // ∞¯∞›¥Á«ﬂ¿ª∂ß : µ•πÃ¡ˆ æ¯¿Ω
	{
		if( pDefender->IsRangeObj( pAttacker->GetPos(), 3.0f ) )
		{
			pDefender->RemoveChrStateBuffs( CHS_ATK_COUNTERATTACK );

			int nLevel = 1;
			if( pDefender->IsPlayer() )
			{
				LPSKILL pSkill = pDefender->GetSkill(SKILL_INDEX( 204, SI_ACR_YOYO_COUNTER ));
				if( pSkill )
				{
#ifdef __SKILL0517
					nLevel	= pDefender->GetSkillLevel( pSkill );
#else	// __SKILL0517
					nLevel = pSkill->dwLevel;
#endif	// __SKILL0517
				}
			}
			g_cSkillSystem->DoUseSkill( pDefender, SKILL_INDEX( 223, SI_GEN_ATK_COUNTER ), nLevel, pAttacker->GetId(), SUT_NORMAL, FALSE, 0 );

			nDamage = 0;
			pInfo->dwAtkFlags = AF_MISS;
		}
	}
	else if( pDefender->GetAdjParam( DST_CHRSTATE) & CHS_DMG_COUNTERATTACK  ) // ∞¯∞›¥Á«ﬂ¿ª∂ß : µ•πÃ¡ˆ ¿÷¿Ω
	{
		if( pDefender->IsRangeObj( pAttacker->GetPos(), 3.0f ) )
		{
			pDefender->RemoveChrStateBuffs( CHS_DMG_COUNTERATTACK );
			g_cSkillSystem->DoUseSkill( pDefender, SKILL_INDEX( 223, SI_GEN_ATK_COUNTER ), pInfo->GetSkillLevel(), pAttacker->GetId(), SUT_NORMAL, FALSE, 0 );
		}
	}

	if( pInfo->GetSkill() == SKILL_INDEX( 212, SI_JST_YOYO_HITOFPENYA ) && nDamage > 0 )
	{
		nDamage = min( pAttacker->GetGold(), nDamage );
		pAttacker->AddGold( -nDamage );
	}

	if( pDefender->IsReturnToBegin() )
	{
		nDamage = 0;
		pInfo->dwAtkFlags = AF_MISS;
	}

	return nDamage;
}

enum POSTCALC_TYPE
{
	POSTCALC_DPC,				// Defense, Parry, Critical ¿˚øÎ
	POSTCALC_MAGICSKILL,		// º”º∫∞¯∞› ∆˜«‘ ∏≈¡˜ Ω∫≈≥  
	POSTCALC_GENERIC,			// ±Ÿ¡¢∞¯∞›  
};

// ∞¯∞›∞ËªÍ ≈∏¿‘
POSTCALC_TYPE GetPostCalcType( DWORD dwAtkFlags )
{
	if ( dwAtkFlags & AF_MAGICSKILL )		// ∏≈¡˜ Ω∫≈≥
		return POSTCALC_MAGICSKILL;

	if ( dwAtkFlags & AF_GENERIC )			// ¿œπ› ∞¯∞›
		return POSTCALC_GENERIC;
	
	return POSTCALC_DPC;
}

int CAttackArbiter::PostCalcDamage( int nATK, ATTACK_INFO* pInfo )
{
	if ( pInfo->dwAtkFlags & AF_FORCE )		// 
		return nATK;

	CMover* pAttacker = pInfo->pAttacker;
	CMover* pDefender = pInfo->pDefender;

	// øπø‹√≥∏Æ: ∏≈¡˜Ω∫≈≥¿Ã æ∆¥œ∏È, ∏ÛΩ∫≈Õ∞° «√∑π¿ÃæÓ∏¶ ∞¯∞›«“ ∂ß ∑π∫ß¬˜ø° µ˚∏• ∞¯∞›∑¬UP
	if( ((pInfo->dwAtkFlags & AF_MAGICSKILL) == 0) && pAttacker->IsNPC() && pDefender->IsPlayer() )	
	{
		int nDelta = pAttacker->GetLevel() - pDefender->GetLevel();
		if( nDelta > 0 )
		{
			float fpower = 1.0f + ( 0.05f * nDelta );
			nATK	= (int)( nATK * fpower );
		}
	}

	int nDamage = nATK;
	switch( GetPostCalcType( pInfo->dwAtkFlags ) )
	{
	case POSTCALC_DPC:
		nDamage = pDefender->ApplyDPC( nATK, pInfo );	// Defense, Parry, Critical ¿˚øÎ
		break;
	case POSTCALC_MAGICSKILL:
		nDamage = pAttacker->PostCalcMagicSkill( nATK, pInfo );
		break;
	case POSTCALC_GENERIC:
		nDamage = pAttacker->PostCalcGeneric( nATK, pInfo );
		break;
	}

	if( nDamage <= 0 )
		return 0;

	nDamage += pAttacker->CalcLinkAttackDamage( nDamage );						// ∏µ≈©æÓ≈√ Ω∫≈≥¿ª ¿˚øÎ«—¥Ÿ.
	if( pInfo->GetSkill() == SKILL_INDEX( 159, SI_BIL_PST_ASALRAALAIKUM ) )
		nDamage += PostAsalraalaikum();

	nDamage	= (int)( nDamage * pAttacker->GetDamageMultiplier( pInfo ) );		// µ•πÃ¡ˆ¡ı∆¯ 
	nDamage = OnAfterDamage( pInfo, nDamage );
	return nDamage;
}	

int CAttackArbiter::PostAsalraalaikum()
{
	int nAddDmg = 0;

	LPSKILL pSkill = m_pAttacker->GetSkill( SKILL_INDEX( 159, SI_BIL_PST_ASALRAALAIKUM ) );
	DWORD dwSkillLevel = 
#ifdef __SKILL0517
		m_pAttacker->GetSkillLevel( pSkill );
#else	// __SKILL0517
		pSkill ? pSkill->dwLevel : 0;
#endif	// __SKILL0517

	if( m_pAttacker->IsNPC() )
		dwSkillLevel = CMonsterSkill::GetInstance()->GetMonsterSkillLevel( m_pAttacker, SKILL_INDEX( 159, SI_BIL_PST_ASALRAALAIKUM ) );

	switch( dwSkillLevel )
	{
		case 1:		nAddDmg = 20;	break;
		case 2:		nAddDmg = 30;	break;
		case 3:		nAddDmg = 40;	break;
		case 4:		nAddDmg = 50;	break;
		case 5:		nAddDmg = 60;	break;
		case 6:		nAddDmg = 70;	break;
		case 7:		nAddDmg = 80;	break;
		case 8:		nAddDmg = 90;	break;
		case 9:		nAddDmg = 100;	break;
		case 10:	nAddDmg = 150;	break;
	}

	int nMP = m_pAttacker->GetManaPoint();						// ¿œ¥‹ πﬁæ∆≥ı∞Ì
	if( m_pAttacker->IsPlayer() && m_pAttacker->IsSMMode( SM_MAINTAIN_MP ) == FALSE )		// MP ¿Ø∑·æ∆¿Ã≈€ ∏‘æ˙¿ª∂ß¥¬ MP¥‚∏È æ»µ»¥Ÿ.
		m_pAttacker->SetPointParam( DST_MP, 0 );				// 0¿∏∑Œ ∏∏µÎ.
	return ( ( ( m_pAttacker->GetStr() / 10 ) * dwSkillLevel ) * ( 5 + nMP / 10 ) + nAddDmg );
}

//
//
void CAttackArbiter::ChanceSkill( int nDestParam, CCtrl *pAttackCtrl, CCtrl *pDefendCtrl )
{
#ifdef _DEBUG
	if( nDestParam == DST_CHR_CHANCESTEALHP )
	{
		int a = 0;
		a;
	}
#endif
	if( m_pAttacker->GetType() != OT_MOVER )	return;
	if( m_pDefender->GetType() != OT_MOVER )	return;
	
	CMover *pAttacker = (CMover *)pAttackCtrl;
	CMover *pDefender = (CMover *)pDefendCtrl;

	int nRate = pAttacker->GetAdjParam( nDestParam );
	if( nRate > 0 )	// ∞¯∞›¿⁄∞° Ω∫≈≥πﬂµøªÛ≈¬
	{
		if( (int)( xRandom(100) ) <= nRate )
		{
			DWORD dwAddSkill = pAttacker->GetChgParam( nDestParam );		// ≈∏∞›Ω√ πﬂµøµ… æ÷µÂΩ∫≈≥
			
			AddSkillProp *pAddSkillProp;
			pAddSkillProp = prj.GetAddSkillProp( dwAddSkill );
			if( pAddSkillProp )
				g_cSkillSystem->DoActiveSkill( pAttacker, pAddSkillProp->dwSkillPropID, pAddSkillProp->dwSkillLvl, pDefender );		// dwActiveSkill Lv1¿Ã πﬂµøµ .
			//				m_pDefender->SetPoison( TRUE, m_pAttacker->GetId(), m_pAttacker->GetChgParam( DST_CHR_CHANCEPOISON ), m_pAttacker->m_tmPoisonUnit, m_pAttacker->m_wPoisonDamage );
		}
		
	}
}


// ªÛ≈¬¿ÃªÛ(π›ªÁ, Ω∫≈œ, æœ»Ê, µ∂)√≥∏Æ
void CAttackArbiter::ProcessAbnormal( int nDamage, ATTACK_INFO* pInfo )
{
	int	nSkill = pInfo->GetSkill();

	//πÊæÓ¿⁄∞° π›ªÁªÛ≈¬¿Ã∏È?  ∞¯∞›¿⁄µµ µ•πÃ¡ˆ∏¶ ¿‘¥¬¥Ÿ.
	int nReflectDmgRate = m_pDefender->GetAdjParam( DST_REFLECT_DAMAGE );
	if( nReflectDmgRate > 0 )
	{
		int nReflectRate = m_pDefender->GetChgParam( DST_REFLECT_DAMAGE );	

		BOOL bAble = TRUE;
		if( nReflectRate > 0 )			// π›ªÁΩ√≈≥ »Æ∑¸¿Ã ¿÷¿ª∂© »Æ∑¸∞ÀªÁ «‘
		{
			if( (int)( xRandom(100) ) > nReflectRate )	// »Æ∑¸ Ω«∆–≥ƒ?
				bAble = FALSE;
		}

		ATK_TYPE atkType = pInfo->GetAtkType();
		switch( atkType )
		{
		case ATK_MELEESKILL:
		case ATK_MAGICSKILL:
			if( m_pDefender->HasBuff( BUFF_SKILL, SKILL_INDEX( 165, SI_PSY_NLG_CRUCIOSPELL ) ) )		// πÊæÓ¿⁄∞° ≈©∑ÁΩ√ø¿ Ω∫∆Á¿œ ∞ÊøÏ
				bAble = FALSE;
			break;
		case ATK_FORCE:															// πÃ∏Æ∞ËªÍµ» µ•πÃ¡ˆ
			bAble = FALSE;
			break;
		}

		if( bAble )
		{
			int nDmg = (int)( nDamage * (float)(nReflectDmgRate / 100.0f) );
			if( m_pAttacker->IsPlayer() && m_pDefender->IsPlayer() )
				nDmg = (int)( nDmg * 0.1f );

			m_nReflectDmg = nDmg;
		}
	}

	// chipi_080325 - AF_MAGICSKILL ¿Œ∞ÊøÏ ∆Ø¡§ Ω∫≈≥ø° «—«ÿ ªÛ≈¬¿ÃªÛ √≥∏Æ
	if( (m_dwAtkFlags & AF_MAGICSKILL) && m_pAttacker->IsPlayer() )
	{
		switch( nSkill )
		{
			case SKILL_INDEX( 158, SI_BIL_PST_BGVURTIALBOLD ) :
			case SKILL_INDEX( 159 ,SI_BIL_PST_ASALRAALAIKUM ) :
				ChanceSkill( DST_CHR_CHANCESTUN,	m_pAttacker, m_pDefender );
				break;
		}
	}
		
	if( m_dwAtkFlags & (AF_GENERIC | AF_MELEESKILL) )		// ±Ÿ¡¢∆Ú≈∏/±Ÿ¡¢Ω∫≈≥ø° ∏¬æ“¥Ÿ.
	{
		if( m_pAttacker->IsPlayer() )
		{
			ChanceSkill( DST_CHR_CHANCESTUN,	m_pAttacker, m_pDefender );
			ChanceSkill( DST_CHR_CHANCEPOISON,	m_pAttacker, m_pDefender );
			ChanceSkill( DST_CHR_CHANCEDARK,	m_pAttacker, m_pDefender );
			ChanceSkill( DST_CHR_CHANCEBLEEDING, m_pAttacker, m_pDefender );
			ChanceSkill( DST_CHR_CHANCESTEALHP, m_pAttacker, m_pDefender );
		}
		
		// Active Skill(∏∂π˝ πﬂµø)
		{
			PT_ITEM_SPEC pAttackerHandItemProp = m_pAttacker->GetActiveHandItemProp();
			if( pAttackerHandItemProp )
			{
				DWORD dwActiveSkill	= pAttackerHandItemProp->dwActiveSkill;
				DWORD dwActiveSkillLevel = pAttackerHandItemProp->dwActiveSkillLv;
				if( dwActiveSkillLevel == NULL_ID )
					dwActiveSkillLevel = 1;
				if(  dwActiveSkill != NULL_ID )	// ∞¯∞›¿⁄¿« π´±‚ø° πﬂµø∏∂π˝¿Ã ¿÷¥Ÿ.
				{
					if( pAttackerHandItemProp->dwActiveSkillRate == NULL_ID ||		// = ∏È 100%
						(DWORD)( random(100) ) < pAttackerHandItemProp->dwActiveSkillRate )	// πﬂµø»Æ∑¸ø° ∞…∑»¥¬∞°.
					{
						g_cSkillSystem->DoActiveSkill( m_pAttacker, dwActiveSkill, dwActiveSkillLevel, m_pDefender, true );		// dwActiveSkill Lv1¿Ã πﬂµøµ . true∏¶ ¡ÿ∞«. ø©±‚º≠ »Æ∑¸∞ËªÍ «ﬁ¿∏π«∑Œ ≥ª∫Œø°º≠¥¬ «œ¡ˆ∏∂∂Û¥¬∂Ê.
						FLTRACE_LOG( PROGRAM_NAME, _T( "%s¿« ActiveSkill πﬂµø, " ), m_pAttacker->GetName() );
					}
				}
			}
		}
	} // AF_GENERIC

	if( m_dwAtkFlags & ( AF_MELEESKILL | AF_MAGICSKILL ) )	// Ω∫≈≥∞¯∞›¿Ã Ω√µµµ…∂ß
	{
		SkillProp* pSkillProp;
		AddSkillProp *pAddSkillProp;
		m_pAttacker->GetSkillProp( &pSkillProp, &pAddSkillProp, nSkill, pInfo->GetSkillLevel(), "ProcessAbnormal" );
		DWORD dwActiveSkill = pAddSkillProp->dwActiveSkill;
		if( dwActiveSkill != NULL_ID )		// √ﬂ∞° πﬂµø Ω∫≈≥¿Ã ¿÷¥Ÿ.
		{
			DWORD dwActiveSkillRate	= pAddSkillProp->dwActiveSkillRate;
			if( m_pAttacker->IsPlayer() && m_pDefender->IsPlayer() )
				dwActiveSkillRate	= pAddSkillProp->dwActiveSkillRatePVP;

			// πﬂµø»Æ∑¸ø° ∞…∑»¥¬∞°.
			if( dwActiveSkillRate == NULL_ID || xRandom(100) < dwActiveSkillRate )
				g_cSkillSystem->DoActiveSkill( m_pAttacker, dwActiveSkill, pAddSkillProp->dwSkillLvl, m_pDefender );	// πﬂµø
		}
	}


	const DWORD dwProb = xRandom( 10000 );

	//////////////////////////////////////////////////////////////////////////
	// ¬¯øÎ «— ¿Â∫Òø° πﬂµø µ«¥¬ Ω∫≈≥¿Ã ¿÷¥¬¡ˆ √º≈©
	g_xApplyItemEffect->DoApplyHitActiveSkill( m_pAttacker, m_pDefender, dwProb );
	g_xApplyItemEffect->DoApplyDamageActiveSkill( m_pDefender, m_pAttacker, dwProb );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// ¿˚øÎ¡ﬂ¿Œ πˆ«¡ø° πﬂµø µ«¥¬ Ω∫≈≥¿Ã ¿÷¥¬¡ˆ √º≈©
	m_pAttacker->DoApplyHitActiveSkill( m_pDefender, dwProb );
	m_pDefender->DoApplyDamageActiveSkill( m_pAttacker, dwProb );
	//////////////////////////////////////////////////////////////////////////
}

// HP ∞®º“ Ω√≈¥ 
int CAttackArbiter::MinusHP( int *pnDamage )
{
	if( m_pAttacker->IsPlayer() == TRUE && m_pDefender->IsPlayer() == TRUE && m_pDefender->GetAdjParam( DST_IGNORE_DMG_PVP ) > 0 )
	{
		*pnDamage = 0;
		return m_pDefender->GetHitPoint();
	}

	int nHP = m_pDefender->GetHitPoint() - *pnDamage;	// HP∏¶ ±¥¬¥Ÿ.
	if( nHP <= 0 )
	{
		nHP				= 0;

		// π›π´¿˚ √º≈©
		if( ( m_pAttacker->m_pFSM == NULL )		
			|| ( ( m_pAttacker->m_pFSM != NULL && m_pAttacker->m_pFSM->GetOptionalFlags().bIgnoreMatchless == FALSE ) ) 
			)
		{
			if( m_pDefender->m_dwMode & MATCHLESS2_MODE )	// HP∞° 0πÃ∏∏¿∏∑Œ ∂≥æÓ¡≥¥¬µ• π›π´¿˚ ªÛ≈¬≥ƒ
				nHP = 1;									// hp∏¶ 1∑Œ ∫π±∏«ÿ¡‹
		}

	//	else
	//		nHP = 0;									// π›π´¿˚ æ∆¥œ∏È ∞¡ ¡◊¥¬∞‘ ∏¬¥Ÿ.
		*pnDamage = m_pDefender->GetHitPoint() - nHP;
	}
	/*
	else	
	{
		int nActionHPRate = m_pDefender->GetAdjParam( DST_AUTOHP );							// πﬂµø HP¿≤
		if( nActionHPRate > 0 )
		{
			float fHPPercent = (float)nHP / m_pDefender->GetMaxHitPoint();					// «ˆ¿Á HP¿« ∆€ºæ∆Æ ¿≤
			if( fHPPercent <= (nActionHPRate / 100.0f) )									// «ˆ¿Á HP∞° πﬂµøHP¿Ã«œ∞° µ«∏È
			{
				float fAddHP = m_pDefender->GetChgParam( DST_AUTOHP ) / 100.0f;				// »∏∫πΩ√≈≥ HP¿≤
				int nAddHP = m_pDefender->GetMaxHitPoint() * fAddHP;						// MaxHP¿« ¿œ¡§∫Ò¿≤¿ª √Î«‘.
				nHP += nAddHP;
				m_pDefender->SetPointParam( DST_HP, nHP );									// HP¡ı∞°.
				g_xWSUserManager->AddCreateSfxObj( m_pDefender, XI_INDEX( 62, XI_GEN_CURE01 ) );					// »∏∫π »ø∞˙ πﬂª˝.
				m_pDefender->RemoveBuff( BUFF_SKILL, SI_ASS_HEAL_PREVENTION );	// «ÿ¡¶.				
			}
		}
	}
	*/

	return nHP;
}

// »Ì«˜ 
void CAttackArbiter::StealHP( int nDamage, ATK_TYPE type )
{
	PT_ITEM_SPEC pHandItem = m_pAttacker->GetActiveHandItemProp();
	if( pHandItem )
	{
		// TODO : ??????????????????????????
		if( pHandItem->dwDestParam[0] == DST_CHR_STEALHP )					// µÈ∞Ì ¿÷¥¯ π´±‚ø° »Ì«˜ ø…º«¿Ã ¿÷¿∏∏È
		{
			int nStealHP = (int)( nDamage * (pHandItem->nAdjParamVal[0] / 100.0f) );	// Ω«¡¶∞°∞›«— µ•πÃ¡ˆ¿« %∏¶ √Î«‘.
			m_pAttacker->SetDestParam( DST_HP, nStealHP, NULL_CHGPARAM );				// ∞¯∞›¿⁄ø°∞‘ »Ì«˜HP∏¶ ¡‹.
		}
	}

	if( type == ATK_GENERIC && m_pAttacker->IsPlayer() )
	{
		int nDstHP = m_pAttacker->GetParam( DST_MELEE_STEALHP, 0 );
		int nStealHP = (int)( nDamage * (nDstHP / 100.0f) );
		if( 0 < nDstHP )
			m_pAttacker->SetDestParam( DST_HP, nStealHP, NULL_CHGPARAM );				// ∞¯∞›¿⁄ø°∞‘ »Ì«˜HP∏¶ ¡‹.
	}
}




// ∞¯∞›¥Á«œ∞Ì µ•πÃ¡ˆ∏¶ ¿‘¿ª ∞ÊøÏ √≥∏Æ 
void CAttackArbiter::OnDamaged( int nDamage, DWORD dwFakeDamage )
{
	m_pDefender->m_nAtkCnt = 1;
	m_pDefender->SetDamagedMotion( m_pAttacker, m_dwAtkFlags );

	g_xWSUserManager->AddDamage( m_pDefender, GETID( m_pAttacker ), nDamage, m_dwAtkFlags, dwFakeDamage );
}

// ∞¯∞›¥Á«œ∞Ì ¡◊¥¬ ∞ÊøÏ √≥∏Æ 
void CAttackArbiter::OnDied()
{
	if( m_pDefender == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, "[ m_pDefender is NULL ]" );
		return;
	}

	if( m_pDefender->IsDie() )
	{
		return;
	}

	if( m_pDefender && m_pAttacker && m_pDefender->m_nDuel != 0 && m_pAttacker->m_nDuel != 0 && 
		m_pDefender->m_idDuelOther == m_pAttacker->GetId() )
	{
		OnDiedPVP();
		return;
	}

	m_pDefender->SubPVP( m_pAttacker, m_nReflect );					// m_pDefender∞° ¡◊∞Ì≥≠ »ƒ ΩΩ∑Œ≈Õ∞¸∑√ √≥∏Æ.

	m_pAttacker->AddKillRecovery();

	OnDiedSchoolEvent();

	if( m_pDefender->IsNPC() && m_pDefender->IsDie() )	 
	{
		m_pDefender->Delete();							// ¿œ¥‹ ∂´ªß... ¿Ã∑±∞ÊøÏ¥¬ ∞¡ æ¯æ÷¿⁄.
		return;
	}

	if( m_pDefender->m_dwIndex == OBJECT_INDEX( 164, MI_CLOCKWORK1 ) || m_pDefender->m_dwIndex == OBJECT_INDEX( 165, MI_BIGMUSCLE ) || m_pDefender->m_dwIndex == OBJECT_INDEX( 166, MI_KRRR ) )
		m_pDefender->SubAroundExp( m_pAttacker, 50.0f );		// π›∞Ê50πÃ≈Õ ¿Ã≥ªø°¥¬ ¥Ÿ ∞Ê«Ëƒ° ≥™¥≤∞°¡¸.
	else
		m_pAttacker->SubExperience( m_pDefender );		// m_pDefender∏¶ ¡◊¿Ã∞Ì ≥≠»ƒ¿« m_pAttacker ∞Ê«Ëƒ° √≥∏Æ.
	
	switch( _GetContentState( CT_DROP_ITEM_REFORM  ) )
	{
	case CS_VER1:
		{
			m_pDefender->OnDie( *m_pAttacker );
		}
		break;
	default:
		m_pDefender->DropItemByDied( m_pAttacker );				// ∏ÛΩ∫≈Õø¥¥Ÿ∏È æ∆¿Ã≈€ µÂ∂¯.
		break;
	}

	BOOL bArena		= m_pDefender->GetWorld() && m_pDefender->GetWorld()->IsArena();
	bArena = g_pEventArenaGlobal->IsArenaChannel() ? TRUE : bArena;

	if( m_pDefender->IsPlayer() && m_pDefender->IsChaotic() && !bArena )
	{
		DWORD dwPropensity = m_pDefender->GetPKPropensity(); // undorflow
		m_pDefender->SetPKPropensity( m_pDefender->GetPKPropensity() - xRandom( prj.m_PKSetting.nDiePropensityMin, prj.m_PKSetting.nDiePropensityMax + 1 ));
		if( dwPropensity <= m_pDefender->GetPKPropensity() )
			m_pDefender->SetPKPropensity( 0 );
		g_xWSUserManager->AddPKPropensity( m_pDefender );
		g_dpDBClient.SendLogPkPvp( m_pDefender, m_pAttacker, 0, 'P' );
	}	

	m_pAttacker->m_nAtkCnt = 0;							// ≈∏∞Ÿ¿ª ¡◊ø¥¿∏∏È ∞¯∞›¿⁄¿« æÓ≈√ƒ´øÓ∆Æ ≈¨∏ÆæÓ
	m_pDefender->DoDie( m_pAttacker, m_dwMsg );			// m_pDefenderæﬂ ¡◊æÓ∂Û. 

	m_pAttacker->ClearDestObj();						// ∏Ò«•∏¶ ¡◊ø¥¿∏∏È ¿Ãµø∏Ò«•µµ ≈¨∏ÆæÓ.
}

// PVP∞¯∞›¥Á«œ∞Ì ¡◊¥¬ ∞ÊøÏ √≥∏Æ 
void CAttackArbiter::OnDiedPVP()
{
	if( m_pDefender->IsNPC() && m_pDefender->IsDie() )	 
	{
		m_pDefender->Delete();							// ¿œ¥‹ ∂´ªß... ¿Ã∑±∞ÊøÏ¥¬ ∞¡ æ¯æ÷¿⁄.
		return;
	}

	m_pDefender->SubDuel( m_pAttacker );//µ‡æÛ∞·∞˙ ∫∏≥ª±‚
	
	m_pDefender->SetPosChanged( TRUE );
	m_pAttacker->SetPosChanged( TRUE );

	m_pAttacker->ClearDuel();
	( (FLWSUser*)m_pAttacker )->AddSetDuel( m_pAttacker );
	m_pDefender->ClearDuel();
	( (FLWSUser*)m_pDefender )->AddSetDuel( m_pDefender );

	m_pDefender->AddKillRecovery();
	m_pAttacker->AddKillRecovery();

	m_pAttacker->m_nAtkCnt = 0;							//  ∞¯∞›¿⁄¿« æÓ≈√ƒ´øÓ∆Æ ≈¨∏ÆæÓ
	m_pDefender->m_nAtkCnt = 0;							//  πÊæÓ¿⁄¿« æÓ≈√ƒ´øÓ∆Æ ≈¨∏ÆæÓ

	m_pDefender->DoPVPEnd( m_pAttacker, false ,m_dwMsg );			//

	( (FLWSUser*)m_pAttacker )->AddDuelStart( m_pDefender->m_idPlayer , 3 );	// º≠∑Œ ªÛ¥ÎπÊø° ¥Î«— æ∆¿Ãµ∏∏ ∫∏≥ª¡÷∏È µ»¥Ÿ. 3:¿Ã±Ë, 4 : ¡¸
	( (FLWSUser*)m_pDefender )->AddDuelStart( m_pAttacker->m_idPlayer , 4 );


	m_pAttacker->ClearDestObj();						// ¿Ãµø∏Ò«•µµ ≈¨∏ÆæÓ.
	m_pDefender->ClearDestObj();						// ¿Ãµø∏Ò«•µµ ≈¨∏ÆæÓ.
}


// Ω∫ƒ¿Ã∫•∆Æ √≥∏Æ 
void CAttackArbiter::OnDiedSchoolEvent()
{
	if( g_eLocal.GetState( EVE_SCHOOL_BATTLE ) == SBS_START2 )	// '/sbstart2'ø°º≠∏∏ ««∞›
	{
		if( m_pDefender->m_dwAuthorization < AUTH_GAMEMASTER )
		{
			if( !m_pDefender->IsMode( OBSERVE_MODE ) )
			{
				CGuild* pGuild	= m_pDefender->GetGuild();
				if( pGuild )
					pGuild->m_nDead++;	
			}
			m_pDefender->SetMode( OBSERVE_MODE );
			g_xWSUserManager->AddModifyMode( (FLWSUser*)m_pDefender );
		}

		static EVENTITEM eventitem[3] =
		{
			ITEM_INDEX( 2805, II_GEN_FOO_INS_HOTDOG ),		12,
			ITEM_INDEX( 2539, II_GEN_REF_REF_TENTH ),		7,
			ITEM_INDEX( 2552, II_GEN_POT_DRI_VITAL900 ),	7,
		};

		for( int i = 0; i < 3; i++ )
		{
			FLItemElem* pItemElem		= new FLItemElem;
			pItemElem->m_dwItemId		= eventitem[i].dwIndex;
			pItemElem->m_nItemNum		= (int)( eventitem[i].dwPrabability );
			pItemElem->m_nHitPoint		= ( pItemElem->GetProp()->dwEndurance == -1 ) ? 0 : pItemElem->GetProp()->dwEndurance;//pItemElem->GetProp()->dwEndurance;
			pItemElem->SetSerialNumber();

			CItem* pItem		= new CItem;
			pItem->m_pItemBase	= pItemElem;
			pItem->SetIndex( D3DDEVICE, pItemElem->m_dwItemId );
			pItem->SetPos( m_pDefender->GetPos() );
			pItem->SetAngle( (float)( xRandom( 360 ) ) );
			if( m_pDefender->GetWorld()->ADDOBJ( pItem, TRUE, m_pDefender->GetLayer() ) == FALSE )
			{
				SAFE_DELETE( pItem );
			}
		}
	}
}

BYTE CAttackArbiter::GetHandFlag()
{
	BYTE cbHandFlag = 0x01;				// µ∆˙∆Æ¥¬ ø¿∏•º’¿Ã¥Ÿ .

	if( m_dwAtkFlags & AF_RANGE )		// »∞¿« ∞ÊøÏ π´¡∂∞« ø¿∏•º’ 
		return cbHandFlag;

	if( m_dwAtkFlags & AF_GENERIC )		// ∆Ú≈∏¿œ∂©
	{
		cbHandFlag = static_cast< BYTE >( m_nParam & 0x03 );	// «œ¿ß 2∫Ò∆Æ∏¶ ª©ø¿∏È ø¿∏•º’¿∏∑Œ ƒ£∞«¡ˆ øﬁº’¿∏∑Œƒ£∞«¡ˆ ≥™ø¬¥Ÿ.
		if( cbHandFlag == 0 )	
			cbHandFlag = 0x01;			// ∞™¿Ã æ¯¿∏∏È µ∆˙∆Æ¥¬ ø¿∏•º’
	}

	return cbHandFlag;
}

float		CAttackArbiter::GetAdjustDamageFactor( ATTACK_INFO & kAttackInfo ) const
{
	float fFactor = 1.0;

	fFactor += GetFactor_AttackType( kAttackInfo );

	fFactor	+= GetFactor_AbnormalState( kAttackInfo );

	fFactor += GetFactor_MonsterAttribute( kAttackInfo );

	return fFactor > 0.0f ? fFactor : 0.0f;
}

float		CAttackArbiter::GetFactor_AttackType( ATTACK_INFO & kAttackInfo ) const
{
	float fFactor = 0.0f;

	if( kAttackInfo.pAttacker == NULL || kAttackInfo.pDefender == NULL )
	{
		return fFactor;
	}

	if( kAttackInfo.pDefender->IsPlayer() == FALSE )
	{
		return fFactor;
	}

	int nParamID	= DST_NONE;

	if( kAttackInfo.GetAtkType() == ATK_MAGICSKILL )
	{
		nParamID	= m_pAttacker->IsPlayer() == TRUE ? DST_TAKE_PVP_DMG_MAGIC_RATE : DST_TAKE_PVE_DMG_MAGIC_RATE;
	}
	else
	{
		nParamID	= m_pAttacker->IsPlayer() == TRUE ? DST_TAKE_PVP_DMG_PHYSICAL_RATE : DST_TAKE_PVE_DMG_PHYSICAL_RATE;
	}

	const int nParam	= kAttackInfo.pDefender->GetParam( nParamID, 0 );

	fFactor				+= static_cast<float>( nParam ) / 100.f;

	return fFactor;
}

float		CAttackArbiter::GetFactor_AbnormalState( ATTACK_INFO & kAttackInfo ) const
{
	float fFactor	= 0.0f;

	if( kAttackInfo.pAttacker == NULL || kAttackInfo.pDefender == NULL )
	{
		return fFactor;
	}

	if( kAttackInfo.pDefender->IsAbnormalState( CHS_STUN ) == true )
	{
		const int nParam	 = kAttackInfo.pAttacker->GetParam( DST_GIVE_DMG_RATE_ENEMY_STUN );
		fFactor				+= static_cast<float>( nParam ) / 100.0f;
	}

	if( kAttackInfo.pDefender->IsAbnormalState( CHS_DARK ) == true )
	{
		const int nParam	= kAttackInfo.pAttacker->GetParam( DST_GIVE_DMG_RATE_ENEMY_DARK );
		fFactor				+= static_cast<float>( nParam ) / 100.0f;
	}

	if( kAttackInfo.pDefender->IsAbnormalState( CHS_POISON ) == true )
	{
		const int nParam	= kAttackInfo.pAttacker->GetParam( DST_GIVE_DMG_RATE_ENEMY_POISON );
		fFactor				+= static_cast<float>( nParam ) / 100.0f;
	}

	if( kAttackInfo.pDefender->IsAbnormalState( CHS_SLOW ) == true )
	{
		const int nParam	= kAttackInfo.pAttacker->GetParam( DST_GIVE_DMG_RATE_ENEMY_SLOW );
		fFactor				+= static_cast<float>( nParam ) / 100.0f;
	}

	if( kAttackInfo.pDefender->IsAbnormalState( CHS_BLEEDING ) == true )
	{
		const int nParam	= kAttackInfo.pAttacker->GetParam( DST_GIVE_DMG_RATE_ENEMY_BLEEDING );
		fFactor				+= static_cast<float>( nParam ) / 100.0f;
	}

	if( kAttackInfo.pDefender->IsAbnormalState( CHS_SILENT ) == true )
	{
		const int nParam	= kAttackInfo.pAttacker->GetParam( DST_GIVE_DMG_RATE_ENEMY_SILENT );
		fFactor				+= static_cast<float>( nParam ) / 100.0f;
	}

	if( kAttackInfo.pDefender->IsAbnormalState( CHS_LOOT ) == true )
	{
		const int nParam	 = kAttackInfo.pAttacker->GetParam( DST_GIVE_DMG_RATE_ENEMY_LOOT );
		fFactor				+= static_cast<float>( nParam ) / 100.0f;
	}

	if( kAttackInfo.pDefender->IsAbnormalState( CHS_SETSTONE ) == true )
	{
		const int nParam	= kAttackInfo.pAttacker->GetParam( DST_GIVE_DMG_RATE_ENEMY_SETSTONE );
		fFactor				+= static_cast<float>( nParam ) / 100.0f;
	}

	if( kAttackInfo.pDefender->IsAbnormalState( CHS_SLEEPING ) == true )
	{
		const int nParam	= kAttackInfo.pAttacker->GetParam( DST_GIVE_DMG_RATE_ENEMY_SLEEPING );
		fFactor				+= static_cast<float>( nParam ) / 100.0f;
	}

	return fFactor;
}

float		CAttackArbiter::GetFactor_MonsterAttribute( ATTACK_INFO & kAttackInfo ) const
{
	float fFactor	= 0.0f;

	if( kAttackInfo.pAttacker == NULL || kAttackInfo.pDefender == NULL )
	{
		return fFactor;
	}

	if( kAttackInfo.pAttacker->IsPlayer() == FALSE || kAttackInfo.pDefender->IsPlayer() == TRUE )
	{
		return fFactor;
	}

	MoverProp* pMoverProp	= prj.GetMoverProp( kAttackInfo.pDefender->m_dwIndex );
	if( pMoverProp == NULL )
	{
		return fFactor;
	}

	int nParamID	= DST_NONE;

	switch( pMoverProp->eElementType )
	{
	case SAI79::FIRE:
		nParamID	= DST_GIVE_PVE_DMG_ELEMENT_FIRE_RATE;
		break;

	case SAI79::WATER:
		nParamID	= DST_GIVE_PVE_DMG_ELEMENT_WATER_RATE;
		break;

	case SAI79::ELECTRICITY:
		nParamID	= DST_GIVE_PVE_DMG_ELEMENT_ELECT_RATE;
		break;

	case SAI79::WIND:
		nParamID	= DST_GIVE_PVE_DMG_ELEMENT_WIND_RATE;
		break;

	case SAI79::EARTH:
		nParamID	= DST_GIVE_PVE_DMG_ELEMENT_EARTH_RATE;
		break;
	}

	const int nParam	= kAttackInfo.pAttacker->GetParam( nParamID, 0 );

	fFactor				+= static_cast<float>( nParam ) / 100.0f;

	return fFactor;
}

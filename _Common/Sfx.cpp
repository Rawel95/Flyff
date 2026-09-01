#include "stdafx.h"
#include "Sfx.h"
#include "light.h"
#include "defineSound.h"
#include "..\_Common\ParticleMng.h"

#include "../Neuz/dpclient.h"
extern	CDPClient	g_DPlay;


#include "party.h"
extern	CParty g_Party;


MEMPOOLER_IMPLEMENT( CSfxGenNormalDmg01, 128 );
MEMPOOLER_IMPLEMENT( CSfxGenMoveMark, 4 );
MEMPOOLER_IMPLEMENT( CSfxGenWaterCircle, 64 );
MEMPOOLER_IMPLEMENT( CSfxGenRainCircle, 64 );


CSfxGenMoveMark* g_pMoveMark=NULL;





//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
static int g_nEffect = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ¿œπ› »ø∞˙µÈ 

// ¿œπ› ∞¯∞›¿∏∑Œ µ•πÃ¡ˆ ¿‘¿ª ∂ß »ø∞˙ 
CSfxGenNormalDmg01::CSfxGenNormalDmg01()
{
}
CSfxGenNormalDmg01::~CSfxGenNormalDmg01()
{
}
void CSfxGenNormalDmg01::Process()
{
	m_nFrame++; // «— «¡∑π¿” ¡¯«‡Ω√≈∞∞Ì
	if(m_pSfxObj->Process()) // SFX¿« æ÷¥œ∏ﬁ¿Ãº«¿ª √≥∏Æ«ÿº≠ SFX¿« ¿Áª˝¿Ã ≥°≥µ¿∏∏È
	{
		Delete(); // ¿⁄Ω≈¿ª ¡ˆøÓ¥Ÿ
	}
	// ¿Ã∞Õ¿Ã Process¿« ±‚∫ª. ¥‹º¯ ¿Áª˝ SFX¥¬ ¥Î∫Œ∫– ¿Ã «¸≈¬∏¶ √Î«—¥Ÿ.
}
void CSfxGenNormalDmg01::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos = GetPos();		// 2006/6/20 xuzhu
//	m_pSfxObj->m_vPos = GetPos()+D3DXVECTOR3(.0f,1.0f,.0f);
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}
// ¿œπ› ∞¯∞›¿∏∑Œ µ•πÃ¡ˆ ¿‘¿ª ∂ß »ø∞˙ 
CSfxGenSuperDmg01::CSfxGenSuperDmg01()
{
}
CSfxGenSuperDmg01::~CSfxGenSuperDmg01()
{
}
void CSfxGenSuperDmg01::Process()
{
	m_nFrame++;
	if(m_pSfxObj->Process())
	{
		Delete();
	}
	// ø™Ω√ ¥‹º¯ ¿Áª˝ Process¿Ã¥Ÿ.
}
void CSfxGenSuperDmg01::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos = GetPos()+D3DXVECTOR3(.0f,1.0f,.0f);
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// Ω∫≈≥∑Œ µ•πÃ¡ˆ ¿‘¿ª ∂ß »ø∞˙ 
CSfxGenSkillDmg01::CSfxGenSkillDmg01()
{
}
CSfxGenSkillDmg01::~CSfxGenSkillDmg01()
{
}
void CSfxGenSkillDmg01::Process()
{
	m_nFrame++;
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxGenSkillDmg01::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos = GetPos()+D3DXVECTOR3(.0f,1.0f,.0f);
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// ∏ÛΩ∫≈Õ∞° µ•πÃ¡ˆ ¿‘¿ª ∂ß »ø∞˙ 
CSfxGenMonsterDmg01::CSfxGenMonsterDmg01()
{
}
CSfxGenMonsterDmg01::~CSfxGenMonsterDmg01()
{
}
void CSfxGenMonsterDmg01::Process()
{
	m_nFrame++; // «—«¡∑π¿” ¡¯«‡Ω√≈∞∞Ì
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc ); // ∫ŸæÓ¥Ÿ¥“ ≥ ID∑Œ Mover¿« ∆˜¿Œ≈Õ∏¶ ∞Æ∞Ìø¬¥Ÿ.
	if(pObjSrc) SetPos(pObjSrc->GetPos()); // ±◊≥¿Ã ¿÷¿∏∏È ±◊≥¿« ¿ßƒ°∑Œ ¿ÃµøΩ√≈∞∞Ì
	if(m_pSfxObj->Process()) // SFX¿« æ÷¥œ∏ﬁ¿Ãº«¿ª √≥∏Æ«ÿº≠ SFX¿« ¿Áª˝¿Ã ≥°≥µ¿∏∏È
	{
		Delete(); // ¿⁄Ω≈¿ª ¡ˆøÓ¥Ÿ.
	}
	// ¿Ã∞Õ¿Ã ∆Ø¡§ Moverø° ∫ŸæÓ¥Ÿ¥œ¥¬ SFX¿« Process ±‚∫ª«¸¿Ã¥Ÿ.
}
void CSfxGenMonsterDmg01::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos = GetPos()+D3DXVECTOR3(.0f,1.0f,.0f);
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// ∫Œ»∞ »ø∞˙ 
CSfxGenRestoration01::CSfxGenRestoration01()
{
}
CSfxGenRestoration01::~CSfxGenRestoration01()
{
}
void CSfxGenRestoration01::Process()
{
	m_nFrame++;
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if(pObjSrc) SetPos(pObjSrc->GetPos());
	if(m_pSfxObj->Process())
	{
		Delete();
	}
	// ø™Ω√ m_idSrc∏¶ µ˚∂Û¥Ÿ¥œ¥¬ Process¿Ã¥Ÿ.
}
void CSfxGenRestoration01::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate.y=GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// ƒ°∑· »ø∞˙ 
CSfxGenCure::CSfxGenCure()
{
}
CSfxGenCure::~CSfxGenCure()
{
}
void CSfxGenCure::Process()
{
	m_nFrame++;
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if(pObjSrc) SetPos(pObjSrc->GetPos());
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxGenCure::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate.y=GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// »ø∑¬ ¡ı¥Î »ø∞˙ 
CSfxGenIncrease01::CSfxGenIncrease01()
{
}
CSfxGenIncrease01::~CSfxGenIncrease01()
{

}
void CSfxGenIncrease01::Process()
{
	m_nFrame++;
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if(pObjSrc) SetPos(pObjSrc->GetPos());
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxGenIncrease01::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate.y=GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// ∑π∫ß æ˜ »ø∞˙ 
CSfxGenLevelUp::CSfxGenLevelUp()
{
}
CSfxGenLevelUp::~CSfxGenLevelUp()
{
}
void CSfxGenLevelUp::Process()
{
	m_nFrame++;
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if(pObjSrc) {
		SetPos(pObjSrc->GetPos()+D3DXVECTOR3(.0f,.1f,.0f));
		SetAngle(-pObjSrc->GetAngle()); // µ˚∂Û¥Ÿ¥œ¥¬ Processø° ∞¢µµ±Ó¡ˆ ¿œƒ°Ω√≈∞¥¬ ∫Œ∫–¿Ã √ﬂ∞°µ«æ˙¥Ÿ.
	}
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxGenLevelUp::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate=D3DXVECTOR3(.0f,GetAngle(),.0f);
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// ±ÿ¥‹Ω∫≈≥ ¿Ã∆Â∆Æ : Ω∫∆Æ∑πƒ™ 01
CSfxTroStretching01::CSfxTroStretching01()
{
}
CSfxTroStretching01::~CSfxTroStretching01()
{
}
void CSfxTroStretching01::Process()
{
	m_nFrame++;
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if(pObjSrc) {
		SetPos(pObjSrc->GetPos()+D3DXVECTOR3(.0f,.1f,.0f));
		SetAngle(-pObjSrc->GetAngle()); // µ˚∂Û¥Ÿ¥œ¥¬ Processø° ∞¢µµ±Ó¡ˆ ¿œƒ°Ω√≈∞¥¬ ∫Œ∫–¿Ã √ﬂ∞°µ«æ˙¥Ÿ.
	}
	if(m_pSfxObj->Process())
	{
		Delete();
		
		if( pObjSrc )
		{
			D3DXVECTOR3 v = pObjSrc->GetPos();
			
			SkillProp* pSkillProp = prj.GetPartySkill( SKILL_INDEX( 7, ST_STRETCHING ) );
			
			CSfx *pSfx = CreateSfx( g_Neuz.m_pd3dDevice, pSkillProp->dwSfxObj2, v, pObjSrc->GetId(), D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), NULL_ID, -1 );	// ∏”∏Æ¿ßø° sfxª˝º∫.
		}
		
	}
}

void CSfxTroStretching01::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate=D3DXVECTOR3(.0f,GetAngle(),.0f);
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// ±ÿ¥‹Ω∫≈≥ ¿Ã∆Â∆Æ : Ω∫∆Æ∑πƒ™ 02
CSfxTroStretching02::CSfxTroStretching02()
{
}
CSfxTroStretching02::~CSfxTroStretching02()
{
}
void CSfxTroStretching02::Process()
{
	m_nFrame++;
	if( m_nSec == 0 )
	{
		if( m_pSfxObj->Process() )
			Delete();
	}
	else
	{
		if( m_pSfxObj->Process() )
			m_pSfxObj->m_nCurFrame = 0;
		if( m_nSec != -1 )
		{
			if( m_nFrame > ( m_nSec * 60 ) )
				Delete();
		}
	}

	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );

	if(pObjSrc) 
	{
		if( !pObjSrc->m_pActMover->IsSit() || g_Party.m_dwModeTime[PARTY_STRETCHING_MODE] == 0 )
		{
			Delete();
		}
		else
		{
			SetPos(pObjSrc->GetPos()+D3DXVECTOR3(.0f,.1f,.0f));
			SetAngle(-pObjSrc->GetAngle()); // µ˚∂Û¥Ÿ¥œ¥¬ Processø° ∞¢µµ±Ó¡ˆ ¿œƒ°Ω√≈∞¥¬ ∫Œ∫–¿Ã √ﬂ∞°µ«æ˙¥Ÿ.
		}
	}
	else
		Delete();
}

void CSfxTroStretching02::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate=D3DXVECTOR3(.0f,GetAngle(),.0f);
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// ±ÿ¥‹Ω∫≈≥ ¿Ã∆Â∆Æ : ∫Ì∏¥√˜~
CSfxTroBlitz::CSfxTroBlitz()
{
}
CSfxTroBlitz::~CSfxTroBlitz()
{
}
void CSfxTroBlitz::Process()
{
	m_nFrame++;
	if( m_nSec == 0 )
	{
		if( m_pSfxObj->Process() )
			Delete();
	}
	else
	{
		if( m_pSfxObj->Process() )
			m_pSfxObj->m_nCurFrame = 0;
		if( m_nSec != -1 )
		{
			if( m_nFrame > ( m_nSec * 60 ) )
				Delete();
		}
	}
}

void CSfxTroBlitz::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );

	if( IsInvalidObj(pObjDest) )	
		return;

	m_pSfxObj->m_vPos = pObjDest->GetPos();
	m_pSfxObj->m_vPos.y += ((pObjDest->m_pModel->GetMaxHeight()*pObjDest->GetScale().y) - (pObjDest->GetScale().y*0.7f));
	
	m_pSfxObj->m_vRotate=D3DXVECTOR3(.0f,GetAngle(),.0f);
	m_pSfxObj->m_vScale = pObjDest->GetScale();
	
	m_pSfxObj->Render( pd3dDevice );
}

// ∑Œ±◊¿Œ »ø∞˙ 
CSfxGenLogin::CSfxGenLogin()
{
}
CSfxGenLogin::~CSfxGenLogin()
{
}
void CSfxGenLogin::Process()
{
	m_nFrame++;
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if(pObjSrc) SetPos(pObjSrc->GetPos());
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxGenLogin::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate.y=GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// øˆ«¡ »ø∞˙ 
CSfxGenWarp::CSfxGenWarp()
{
}
CSfxGenWarp::~CSfxGenWarp()
{
}
void CSfxGenWarp::Process()
{
	m_nFrame++;
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if(pObjSrc) SetPos(pObjSrc->GetPos());
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxGenWarp::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate.y=GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// «√∑π¿ÃæÓ ¡◊¿ª ∂ß »ø∞˙ 
CSfxGenPcDie::CSfxGenPcDie()
{
}
CSfxGenPcDie::~CSfxGenPcDie()
{
}
void CSfxGenPcDie::Process()
{
	m_nFrame++;
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if(pObjSrc) SetPos(pObjSrc->GetPos());
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxGenPcDie::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate.y=GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// ∏ÛΩ∫≈Õ Ω∫∆˘ «“ ∂ß »ø∞˙ 
CSfxGenMonsterSpawn::CSfxGenMonsterSpawn()
{
}
CSfxGenMonsterSpawn::~CSfxGenMonsterSpawn()
{
}
void CSfxGenMonsterSpawn::Process()
{
	m_nFrame++;
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if(pObjSrc) SetPos(pObjSrc->GetPos());
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxGenMonsterSpawn::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate.y=GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}


CSfxGenMoveMark::CSfxGenMoveMark()
{
	nRemove = FALSE;
}

CSfxGenMoveMark::~CSfxGenMoveMark()
{
}
void CSfxGenMoveMark::Process()
{
	m_nFrame++;
	

	if	(g_pMoveMark!=this)
	{	m_pSfxObj->m_nCurFrame=180;
	}	


	if(m_pSfxObj->Process())
	{
			
 		//g_pMoveMark=NULL;
		Delete();
		
	}
}
void CSfxGenMoveMark::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vPos.y += 0.05f;
	m_pSfxObj->m_vScale = GetScale();

	m_pSfxObj->Render2( pd3dDevice );
}

// ¿œπ› ∞¯∞›¿∏∑Œ µ•πÃ¡ˆ ¿‘¿ª ∂ß »ø∞˙ 
CSfxGenWaterCircle::CSfxGenWaterCircle()
{
}
CSfxGenWaterCircle::~CSfxGenWaterCircle()
{
}
void CSfxGenWaterCircle::Process()
{
	m_nFrame++;
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxGenWaterCircle::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

CSfxGenRainCircle::CSfxGenRainCircle()
{
}
CSfxGenRainCircle::~CSfxGenRainCircle()
{
}
void CSfxGenRainCircle::Process()
{
	m_nFrame++;
	if(m_pSfxObj->Process())
	{
		Delete();
	}

	m_pSfxObj->m_vPos = GetPos();
	m_pSfxObj->m_vScale    = GetScale();
}
void CSfxGenRainCircle::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->Render2( pd3dDevice );
}


// ¿œπ› ∞¯∞›¿∏∑Œ µ•πÃ¡ˆ ¿‘¿ª ∂ß »ø∞˙ 
CSfxGenWaterCrown::CSfxGenWaterCrown()
{
}
CSfxGenWaterCrown::~CSfxGenWaterCrown()
{
}
void CSfxGenWaterCrown::Process()
{
	m_nFrame++;
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxGenWaterCrown::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// æ∆¿Ã≈€ »ø∞˙µÈ 

// ∞¯¡ﬂ¿¸ø°º≠ πﬂªÁ«œ¥¬ øœµÂ¿« πﬂªÁ√º.
CSfxItemWandAtkAir::CSfxItemWandAtkAir()
{
	m_SfxObj2.SetSfx( "sfx_HitWand01" ); // ∏¬¥¬ º¯∞£¿« SFX∏¶ µ˚∑Œ ∑Œµ˘«—¥Ÿ.
	m_bHit = FALSE; // æ∆¡˜ æ»∏¬æ“¥Ÿ.
}
CSfxItemWandAtkAir::~CSfxItemWandAtkAir()
{
}

int CSfxItemWandAtkAir::SetSfx( LPDIRECT3DDEVICE9 pd3dDevice, int nIndex, 
								 D3DXVECTOR3& vPosSrc, OBJID idSrc, D3DXVECTOR3& vPosDest, OBJID idDest, int nSec )
{
	CSfx::SetSfx( pd3dDevice, nIndex, vPosSrc, idSrc, vPosDest, idDest, nSec );		// ±‚∫ª√ ±‚»≠ ∏’¿˙«œ∞Ì.

	CMover* pObjSrc = prj.GetMover( m_idSrc );		// πﬂªÁ√¯.
	if( IsInvalidObj(pObjSrc) )		return 0;		// Ω«∆–.
	CMover* pObjDest = prj.GetMover( m_idDest );	// ≈∏∞Ÿ√¯¿« ∆˜¿Œ≈Õ.
	if( IsInvalidObj(pObjDest) )	return 0;		// Ω«∆–.
	
	// !!!!!!!!!!!! SFX∏¶ SetPos«“∂ß ∫π¿‚«— √≥∏Æ «œ¡ˆ æ µµ∑œ πŸ≤Ÿ¿⁄.
	SetPos( pObjSrc->GetPos() + D3DXVECTOR3( 0, 1.0f, 0 ) );		// πﬂªÁ√¯¿« ¡¬«•ø°º≠ æ‡∞£ ¿ßø°º≠ πﬂªÁµ .
	m_pSfxObj->m_vPos = GetPos();
	m_vPosDest = pObjDest->GetPos();		// ≈∏∞Ÿ√¯ ∆˜¡ˆº«.

	return 1;
}

//
//
//
void CSfxItemWandAtkAir::ShootSfx( float fAngXZ, float fAngY, float fSpeed )
{
	AngleToVector( &m_vDelta, fAngXZ, fAngY, fSpeed );		// √ ±‚ πﬂªÁ»˚.	- øœµÂ∞° «‚«œ∞Ì ¿÷¥¬ πÊ«‚¿∏∑Œ ΩÍ∞‘ πÊ√‚.
}

	
void CSfxItemWandAtkAir::Process()
{
	FLOAT	fRadius = 0.6f;		// ≈∏∞Ÿ¿« π›∞Ê.
	CMover* pObjDest = prj.GetMover( m_idDest );		// ≈∏∞Ÿ√¯¿« ∆˜¿Œ≈Õ.
	if( IsValidObj(pObjDest) )		// ∏∏æ‡ πﬂªÁ√º∞° ≥Øæ∆∞°¥¬ ¡ﬂø° ≈∏∞Ÿ¿Ã æ¯æÓ¡≥¥Ÿ∏È ∏∂¡ˆ∏∑ ¿÷æ˙¥¯ ¡¬«•∑Œ ≥Øæ∆∞°º≠ ∆ƒ±´µ»¥Ÿ.
	{
		m_vPosDest = pObjDest->GetPos();
		m_vPosDest.y += 1.0f;
		if( pObjDest->IsNPC() )		// ∏˜¿«∞ÊøÏ ≈∏∞Ÿ¿« ≈©±‚∏¶ ¡ª¥ı ≈©∞‘ «œ¿⁄.
			fRadius = 3.0f;
	}
	
	if( m_bHit == FALSE )	// æ∆¡˜ æ»∏¬¿∫ ªÛ≈¬¿Ã∏È
	{ 
		m_pSfxObj->Process(); // SFX æ÷¥œ∏ﬁ¿Ãº«¿ª √≥∏Æ«œ∞Ì
		if( m_pSfxObj->m_nCurFrame == 20 ) // «ˆ¿Á 20«¡∑π¿”±Ó¡ˆ ¡¯«‡«ﬂ¿∏∏È
		{
			m_pSfxObj->m_nCurFrame = 0; // ∏Ò«•¡°ø° ∏¬¿ª∂ß±Ó¡ˆ ∑Á«¡Ω√≈∞±‚ ¿ß«ÿ ¥ŸΩ√ 0«¡∑π¿”¿∏∑Œ µπ∑¡≥ı¥¬¥Ÿ.
		}

		// «ˆ¿Á ¿ßƒ°ø°º≠ ∏Ò«•πÊ«‚¬ ¿∏∑Œ¿« ∫§≈Õ ±∏«‘.
		D3DXVECTOR3 vDist = m_vPosDest - GetPos();
		D3DXVECTOR3 vDistXZ = vDist;
		vDistXZ.y = 0;		// XZ∆Ú∏È¿« º¯ºˆ ±Ê¿Ã∏¶ ±∏«œ±‚ ¿ß«ÿ.

		FLOAT fAngXZ = D3DXToDegree( atan2( vDist.x, -vDist.z ) );		// ∏Ò«•πÊ«‚¬ ¿∏∑Œ¿« XZ∞¢µµ ±∏«‘.
		FLOAT fDistXZ = D3DXVec3Length( &vDistXZ );				// ∏Ò«•πÊ«‚¬ ¿∏∑Œ¿« XZ∆Ú∏Èø°º≠¿« ±Ê¿Ã.
		FLOAT fAngH  = D3DXToDegree( atan2( fDistXZ, vDist.y ) );		// ∏Ò«•πÊ«‚¬ ¿∏∑Œ¿« ≥Ù¿Ã ∞¢µµ.

		fAngH -= 90.0f;
		fAngH = -fAngH;
		D3DXVECTOR3	vAcc;
		AngleToVector( &vAcc, fAngXZ, fAngH, 0.007f );		// Ω«Ω√∞£¿∏∑Œ ∏Ò«•¬ ¿∏∑Œ «‚«œ¥¬ »˚.
		
		FLOAT fSpeedSq = D3DXVec3LengthSq( &m_vDelta );		// µπˆ±ÎøÎ.  «ˆ¿Á πﬂªÁ√º¿« º”µµ∏¶ ªÍ√‚.
		if( fSpeedSq < (0.45f * 0.45f) )
		{
			m_vDelta += vAcc;		// ±‚¡∏ ∞¸º∫ø° ªı∑ŒøÓ »˚¿ª ¥ı«‘.
		}
		m_vDelta *= 0.985f;		// ∏∂¬˚¿ª ¡‡º≠ ¿œ¡§¿ÃªÛ º”µµ∞° ª°∂Û¡ˆ¡ˆ æ µµ∑œ «œ¿⁄.
		//m_vDelta *= 0.965f;		// ∏∂¬˚¿ª ¡‡º≠ ¿œ¡§¿ÃªÛ º”µµ∞° ª°∂Û¡ˆ¡ˆ æ µµ∑œ «œ¿⁄.
		D3DXVECTOR3 vPos = GetPos();
		vPos += m_vDelta;
		SetPos( vPos );		// «ˆ¿Á ¿ßƒ° ∞ªΩ≈.

		m_pSfxObj->m_vRotate.y = m_SfxObj2.m_vRotate.y = fAngXZ;	// sfx model¿« ∞¢µµµµ ∫Ø»≠ Ω√ƒ—¡‹.
		
		if( D3DXVec3LengthSq( &vDist ) < (fRadius * fRadius) )  // ∏Ò«•¡°¿« ∞≈∏ÆøÕ xx cm¿Ã«œ¿Œ ∞ÊøÏø°¥¬...
		{			
			m_bHit = TRUE; // ∏¬¿∫ ∞…∑Œ ∆«¥‹«—¥Ÿ.
			m_SfxObj2.m_vPos = GetPos();		// «ˆ¿Á ¿ßƒ°ø°º≠ ∆¯πﬂ.
			DamageToTarget(); // ø©±‚º≠ ∏¬¥¬º¯∞£√≥∏Æ
		}

		if( IsValidObj(pObjDest) )
		{
			if( pObjDest == CMover::GetActiveMover() )	// ≥Ø ≈∏∞Ÿ¿∏∑Œ ¿‚∞Ì ≥Øæ∆ø¿¥¬ ≥¿∫ ª°∞£ªˆ
				g_ParticleMng.CreateParticle( 0, GetPos(), D3DXVECTOR3(0,0,0), pObjDest->GetPos().y );
			else
				g_ParticleMng.CreateParticle( 1, GetPos(), D3DXVECTOR3(0,0,0), pObjDest->GetPos().y );
			
		}
		
	}
	else 
	{
		if( m_SfxObj2.Process() )		// ∏∂¡ˆ∏∑¿∏∑Œ ∏¬¥¬ º¯∞£ √‚∑¬µ«¥¬ SFX¿« ¿Áª˝¿Ã ≥°≥™∏È
		{ 
			Delete(); // ¿⁄Ω≈¿ª ¡ˆøÓ¥Ÿ.
		}
	}
	m_pSfxObj->m_vPos = GetPos();
	
	m_nFrame++;
	if( m_nFrame >= 60 * 8 )	
	{
		m_bHit = TRUE;
		m_SfxObj2.m_vPos = GetPos();		// «ˆ¿Á ¿ßƒ°ø°º≠ ∆¯πﬂ.
		if( g_pPlayer->IsActiveMover() )
			g_DPlay.SendSfxClear( m_idSfxHit );		// ¿Ã∞Õ¿Ã ªË¡¶µ«æ˙¥Ÿ¥¬∞… º≠πˆø° æÀ∏≤.
	}
}

void CSfxItemWandAtkAir::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	if( m_bHit == FALSE ) 
	{
		m_pSfxObj->Render2( pd3dDevice );
	}
	else 
	{
		if( !g_Option.m_bSFXRenderOff )
			m_SfxObj2.Render( pd3dDevice );
	}
}



CSfxMagicMiAtk1::CSfxMagicMiAtk1()
{
	m_SfxObj2.SetSfx( "sfx_HitWand01" ); // ∏¬¥¬ º¯∞£¿« SFX∏¶ µ˚∑Œ ∑Œµ˘«—¥Ÿ.
	m_bHit = FALSE; // æ∆¡˜ æ»∏¬æ“¥Ÿ.
	m_fCenter = 0;
	m_fRadiusXZ = 0.2f;		// µ∆˙∆Æ 20ºæƒ°.
}
CSfxMagicMiAtk1::~CSfxMagicMiAtk1()
{
}


void CSfxMagicMiAtk1::Process()
{
	// ¿Ã∞Õ¿∫ ∏Ò«•¡°±Ó¡ˆ ≥Ø∂Û∞°º≠ ∏¬¥¬ SFX¿« ¿œπ›¿˚¿Œ Process¿Ã¥Ÿ.
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );
	if( pObjDest ) 
	{
		if( pObjDest->IsDie() )
		{
			Delete();
			return;
		}

		// πﬂªÁ ∏Ò«•¡ˆ¡°¿∫ ªÛ¥Î¿« ¡ﬂæ”.
		float fHeight = fabs(pObjDest->m_pModel->m_vMax.y) / 2.0f;	// æ∆¿ÃπŸ∆Æ∞∞¿∫≥—¿∫ ∞¯¡ﬂø° ∂∞¿÷±‚∂´ø° º¯ºˆ ≥Ù¿Ã¿Œ Max∞™∏∏ √Î«ﬂ¥Ÿ.
		fHeight *= pObjDest->GetScale().x;
		m_fCenter = fHeight;	
		
		m_vPosDest = pObjDest->GetPos();		// ≈∏∞Ÿ¿« ¿ßƒ° ∞ªΩ≈ - ≈∏∞Ÿ¿Ã ∞Ëº” øÚ¡˜¿Ãπ«∑Œ...
		m_vPosDest.y += m_fCenter;
		if( m_nFrame == 0 )
		{
			// ∏« √π«¡∑π¿”ø°º≠ ∞ËªÍ.
			FLOAT fLenX = fabs(pObjDest->m_pModel->m_vMax.x - pObjDest->m_pModel->m_vMin.x);
			FLOAT fLenZ = fabs(pObjDest->m_pModel->m_vMax.z - pObjDest->m_pModel->m_vMin.z);
			m_fRadiusXZ = ((fLenX + fLenZ) / 2.0f);		// X≈©±‚ Z≈©±‚ ∆Ú±’¿ª ≥ø
			m_fRadiusXZ *= pObjDest->GetScale().x;
			m_fRadiusXZ /= 3.0f;	// ¡§»Æ»˜ π›¡ˆ∏ß¿ª æ≤¡¸∂Û∞Ì π›¡ˆ∏ß(/2)∫∏¥Ÿ ¡∂±› ≈©∞‘ æ≤¿⁄ 3¿ª ≥™¥Æ.
		}
	} else
	{
		Delete();
		return;
	}

		
	if( m_bHit == FALSE )		// æ∆¡˜ æ»∏¬¿∫ ªÛ≈¬¿Ã∏È
	{ 
		m_pSfxObj->Process(); // SFX æ÷¥œ∏ﬁ¿Ãº«¿ª √≥∏Æ«œ∞Ì
		if( m_pSfxObj->m_nCurFrame == 20 ) // «ˆ¿Á 20«¡∑π¿”±Ó¡ˆ ¡¯«‡«ﬂ¿∏∏È
		{
			m_pSfxObj->m_nCurFrame=0; // ∏Ò«•¡°ø° ∏¬¿ª∂ß±Ó¡ˆ ∑Á«¡Ω√≈∞±‚ ¿ß«ÿ ¥ŸΩ√ 0«¡∑π¿”¿∏∑Œ µπ∑¡≥ı¥¬¥Ÿ.
		}

		D3DXVECTOR3 vPos = GetPos();
		D3DXVECTOR3 vDelta = m_vPosDest - vPos;		// «ˆ¿Á ¿ßƒ°ø°º≠ ≈∏∞ŸπÊ«‚¿∏∑Œ¿« ∫§≈Õ.
		D3DXVec3Normalize( &vDelta, &vDelta );		// ¿Ø¥÷∫§≈Õ»≠ - 1πÃ≈Õ»≠.
		vPos += (vDelta * 0.4f);					// 20ºæƒ°∑Œ ¡Ÿø©º≠ «ˆ¿Á¿ßƒ°ø° ¥ı«‘.
		
		SetPos( vPos );				// «ˆ¿Á ¿ßƒ° ∞ªΩ≈.
		m_pSfxObj->m_vPos = vPos;	// SFXModel¿« ¿ßƒ°µµ ∞ªΩ≈.

		D3DXVECTOR3 vDir      = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
		D3DXVECTOR3 vDestNor  = m_vPosDest - vPos;
		D3DXVECTOR3 vAxis;
		D3DXQUATERNION   qRot;
		
		FLOAT       fTheta;
		D3DXVec3Normalize( &vDestNor, &vDestNor );
		D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
		fTheta = D3DXVec3Dot( &vDir, &vDestNor );
		D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );

		D3DXVECTOR3 vYPW;

		QuaternionRotationToYPW( qRot, vYPW );
		m_pSfxObj->m_vRotate.x = m_SfxObj2.m_vRotate.x = D3DXToDegree(vYPW.x);
		m_pSfxObj->m_vRotate.y = m_SfxObj2.m_vRotate.y = D3DXToDegree(vYPW.y);
		m_pSfxObj->m_vRotate.z = m_SfxObj2.m_vRotate.z = D3DXToDegree(vYPW.z);

		vDelta = GetPos() - m_vPosDest;
		if( D3DXVec3LengthSq( &vDelta ) < m_fRadiusXZ * m_fRadiusXZ )		// ∏Ò«•¡°¿« ∞≈∏ÆøÕ 20cm¿Ã«œ¿Œ ∞ÊøÏø°¥¬...
		{ 
			m_SfxObj2.m_vPos = GetPos();//m_vPosDest;		// ≈∏∞Ÿ¿« ¿ßƒ°ø° ∆¯πﬂ ¿Ã∆Â∆Æ ¡¬«•.
			m_SfxObj2.m_vPos.y+=1.0f;
			m_bHit = TRUE; // ∏¬¿∫ ∞…∑Œ ∆«¥‹«—¥Ÿ.
		}
	}
	else 
	{
		if( m_SfxObj2.m_nCurFrame == 0 )		// m_SfxObj2¥¬ ∏¬¥¬ º¯∞£ √‚∑¬µ«¥¬ SFX¿Ã¥Ÿ.
		{ 
			DamageToTarget(); // ø©±‚º≠ ∏¬¥¬º¯∞£√≥∏Æ
		}
		if( m_SfxObj2.Process() )		// ∏∂¡ˆ∏∑¿∏∑Œ ∏¬¥¬ º¯∞£ √‚∑¬µ«¥¬ SFX¿« ¿Áª˝¿Ã ≥°≥™∏È
		{ 
			Delete(); // ¿⁄Ω≈¿ª ¡ˆøÓ¥Ÿ.
		}
	}
	m_nFrame++;
	if( m_nFrame >= SEC1 * 10 )		// ≥ π´ø¿∑° ?æ∆∞°¡ˆ æ ∞‘ 10√ ¡ˆ≥™∏È º“∏Í
		Delete();
}
void CSfxMagicMiAtk1::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	if( m_bHit == FALSE ) 
	{
		m_pSfxObj->Render2( pd3dDevice );
	}
	else 
	{
		if( !g_Option.m_bSFXRenderOff )
			m_SfxObj2.Render( pd3dDevice );
	}
}



CSfxItemWandAtk1::CSfxItemWandAtk1()
{
	m_SfxObj2.SetSfx( "sfx_HitWand01" ); // ∏¬¥¬ º¯∞£¿« SFX∏¶ µ˚∑Œ ∑Œµ˘«—¥Ÿ.
	m_bHit = FALSE; // æ∆¡˜ æ»∏¬æ“¥Ÿ.
	m_fCenter = 0;
	m_fRadiusXZ = 0.2f;		// µ∆˙∆Æ 20ºæƒ°.
}
CSfxItemWandAtk1::~CSfxItemWandAtk1()
{
}


void CSfxItemWandAtk1::Process()
{
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );
	if( pObjDest ) 
	{
		if( pObjDest->IsDie() )
		{
			Delete();
			return;
		}

		// πﬂªÁ ∏Ò«•¡ˆ¡°¿∫ ªÛ¥Î¿« ¡ﬂæ”.
		float fHeight = fabs(pObjDest->m_pModel->m_vMax.y) / 2.0f;	// æ∆¿ÃπŸ∆Æ∞∞¿∫≥—¿∫ ∞¯¡ﬂø° ∂∞¿÷±‚∂´ø° º¯ºˆ ≥Ù¿Ã¿Œ Max∞™∏∏ √Î«ﬂ¥Ÿ.
		fHeight *= pObjDest->GetScale().x;
		m_fCenter = fHeight;	
		
		m_vPosDest = pObjDest->GetPos();		// ≈∏∞Ÿ¿« ¿ßƒ° ∞ªΩ≈ - ≈∏∞Ÿ¿Ã ∞Ëº” øÚ¡˜¿Ãπ«∑Œ...
		m_vPosDest.y += m_fCenter;
		if( m_nFrame == 0 )
		{
			// ∏« √π«¡∑π¿”ø°º≠ ∞ËªÍ.
			FLOAT fLenX = fabs(pObjDest->m_pModel->m_vMax.x - pObjDest->m_pModel->m_vMin.x);
			FLOAT fLenZ = fabs(pObjDest->m_pModel->m_vMax.z - pObjDest->m_pModel->m_vMin.z);
			m_fRadiusXZ = ((fLenX + fLenZ) / 2.0f);		// X≈©±‚ Z≈©±‚ ∆Ú±’¿ª ≥ø
			m_fRadiusXZ *= pObjDest->GetScale().x;
			m_fRadiusXZ /= 3.0f;	// ¡§»Æ»˜ π›¡ˆ∏ß¿ª æ≤¡¸∂Û∞Ì π›¡ˆ∏ß(/2)∫∏¥Ÿ ¡∂±› ≈©∞‘ æ≤¿⁄ 3¿ª ≥™¥Æ.
		}
	} else
	{
		Delete();
		return;
	}

		
	if( m_bHit == FALSE )		// æ∆¡˜ æ»∏¬¿∫ ªÛ≈¬¿Ã∏È
	{ 
		m_pSfxObj->Process(); // SFX æ÷¥œ∏ﬁ¿Ãº«¿ª √≥∏Æ«œ∞Ì
		if( m_pSfxObj->m_nCurFrame == 20 ) // «ˆ¿Á 20«¡∑π¿”±Ó¡ˆ ¡¯«‡«ﬂ¿∏∏È
		{
			m_pSfxObj->m_nCurFrame=0; // ∏Ò«•¡°ø° ∏¬¿ª∂ß±Ó¡ˆ ∑Á«¡Ω√≈∞±‚ ¿ß«ÿ ¥ŸΩ√ 0«¡∑π¿”¿∏∑Œ µπ∑¡≥ı¥¬¥Ÿ.
		}

		D3DXVECTOR3 vPos = GetPos();
		D3DXVECTOR3 vDelta = m_vPosDest - vPos;		// «ˆ¿Á ¿ßƒ°ø°º≠ ≈∏∞ŸπÊ«‚¿∏∑Œ¿« ∫§≈Õ.
		D3DXVec3Normalize( &vDelta, &vDelta );		// ¿Ø¥÷∫§≈Õ»≠ - 1πÃ≈Õ»≠.
		vPos += (vDelta * 0.2f);					// 20ºæƒ°∑Œ ¡Ÿø©º≠ «ˆ¿Á¿ßƒ°ø° ¥ı«‘.
		
		SetPos( vPos );				// «ˆ¿Á ¿ßƒ° ∞ªΩ≈.
		m_pSfxObj->m_vPos = vPos;	// SFXModel¿« ¿ßƒ°µµ ∞ªΩ≈.

		D3DXVECTOR3 vDir      = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
		D3DXVECTOR3 vDestNor  = m_vPosDest - vPos;
		D3DXVECTOR3 vAxis;
		D3DXQUATERNION   qRot;
		
		FLOAT       fTheta;
		D3DXVec3Normalize( &vDestNor, &vDestNor );
		D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
		fTheta = D3DXVec3Dot( &vDir, &vDestNor );
		D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );

		D3DXVECTOR3 vYPW;

		QuaternionRotationToYPW( qRot, vYPW );
		m_pSfxObj->m_vRotate.x = m_SfxObj2.m_vRotate.x = D3DXToDegree(vYPW.x);
		m_pSfxObj->m_vRotate.y = m_SfxObj2.m_vRotate.y = D3DXToDegree(vYPW.y);
		m_pSfxObj->m_vRotate.z = m_SfxObj2.m_vRotate.z = D3DXToDegree(vYPW.z);

		vDelta = GetPos() - m_vPosDest;
		if( D3DXVec3LengthSq( &vDelta ) < m_fRadiusXZ * m_fRadiusXZ )		// ∏Ò«•¡°¿« ∞≈∏ÆøÕ 20cm¿Ã«œ¿Œ ∞ÊøÏø°¥¬...
		{ 
			m_SfxObj2.m_vPos = GetPos();//m_vPosDest;		// ≈∏∞Ÿ¿« ¿ßƒ°ø° ∆¯πﬂ ¿Ã∆Â∆Æ ¡¬«•.
			m_SfxObj2.m_vPos.y+=1.0f;
			m_bHit = TRUE; // ∏¬¿∫ ∞…∑Œ ∆«¥‹«—¥Ÿ.
		}
	}
	else 
	{
		if( m_SfxObj2.m_nCurFrame == 0 )		// m_SfxObj2¥¬ ∏¬¥¬ º¯∞£ √‚∑¬µ«¥¬ SFX¿Ã¥Ÿ.
		{ 
			DamageToTarget(); // ø©±‚º≠ ∏¬¥¬º¯∞£√≥∏Æ
		}
		if( m_SfxObj2.Process() )		// ∏∂¡ˆ∏∑¿∏∑Œ ∏¬¥¬ º¯∞£ √‚∑¬µ«¥¬ SFX¿« ¿Áª˝¿Ã ≥°≥™∏È
		{ 
			Delete(); // ¿⁄Ω≈¿ª ¡ˆøÓ¥Ÿ.
		}
	}
	m_nFrame++;
	if( m_nFrame >= SEC1 * 10 )		// ≥ π´ø¿∑° ?æ∆∞°¡ˆ æ ∞‘ 10√ ¡ˆ≥™∏È º“∏Í
		Delete();
}
void CSfxItemWandAtk1::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	if( m_bHit == FALSE ) 
	{
		m_pSfxObj->Render2( pd3dDevice );
	}
	else 
	{
		if( !g_Option.m_bSFXRenderOff )
			m_SfxObj2.Render( pd3dDevice );
	}
}


CSfxItemRangeAtk1::CSfxItemRangeAtk1() : CSfxShoot()
{
	m_pTail			= NULL;
}
CSfxItemRangeAtk1::~CSfxItemRangeAtk1()
{
	if( m_pTail )
		g_TailEffectMng.Delete( m_pTail );
	m_pTail = NULL;
}

void CSfxItemRangeAtk1::Process()
{
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );	// ¿Øµµ≈∫ «¸¿Ã±‚∂ßπÆø° ≈∏∞Ÿ¿∫ «◊ªÛ ¿–æÓ∫¡æﬂ «—¥Ÿ.
	if( IsValidObj(pObjDest) )
	{
		m_vPosDest = pObjDest->GetPos() + D3DXVECTOR3( 0, 1.0f, 0 );
	} else
	{
		// ≥Øæ∆∞°¥Ÿ ≈∏∞Ÿ¿Ã æ¯æÓ¡≥¿∏∏È ∏∂¡ˆ∏∑¡¬«•∑Œ ≥Øæ∆∞°º≠ ≈Õ¡ˆ∞‘.
		Delete();	// this¥¬ ªË¡¶.
		int a = 0;
	}
	
	if( m_bHit == FALSE ) 
	{
		if( m_pSfxObj->Process() )		// æ÷¥œ∏ﬁ¿Ãº«¿∫ ∑Á«¡.
		{
			m_pSfxObj->m_nCurFrame = 0;
		}
		D3DXVECTOR3 vPos = GetPos();
		D3DXVECTOR3 vDelta = m_vPosDest - vPos;
		D3DXVec3Normalize( &vDelta, &vDelta );		// ≈∏∞Ÿ¬  ¥‹¿ß∫§≈Õ ±∏«ÿº≠
		vPos += vDelta * 0.7f;			// ±◊ ∫§≈Õ∑Œ º”µµ∏¶ ∞·¡§«ÿº≠ ¡¬«• ¿Ãµø.
		m_pSfxObj->m_vPos = vPos;
		SetPos( vPos );
		
		{
			// ∏∂π˝∞¥√º πÊ«‚∫§≈Õ∑Œ ∏µ® πÊ«‚ µπ∏Æ±‚~~~
			D3DXVECTOR3 vDir      = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
			D3DXVECTOR3 vDestNor  = m_vPosDest - m_pSfxObj->m_vPos;
			D3DXVECTOR3 vAxis;
			D3DXQUATERNION   qRot;
			
			FLOAT       fTheta;
			D3DXVec3Normalize( &vDestNor, &vDestNor );
			D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
			fTheta = D3DXVec3Dot( &vDir, &vDestNor );
			D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );
			
			D3DXVECTOR3 vYPW;
			
			QuaternionRotationToYPW( qRot, vYPW );
			m_pSfxObj->m_vRotate.x = m_SfxObj2.m_vRotate.x = D3DXToDegree(vYPW.x);
			m_pSfxObj->m_vRotate.y = m_SfxObj2.m_vRotate.y = D3DXToDegree(vYPW.y);
			m_pSfxObj->m_vRotate.z = m_SfxObj2.m_vRotate.z = D3DXToDegree(vYPW.z);

		}
		
		if( m_pTail )
		{
			if( m_pTail->GetType() != 2 )	// ª˝º∫µ«æ˙¥¯ ≤ø∏Æ∞Ì ¿œπ›∫∏µÂøÎ¿Ã æ∆¥œ∏È 
				m_pTail->ChangeTexture( D3DDEVICE, "etc_Tail2.bmp", 2 );	// ¿œπ›∫∏µÂøÎ¿∏∑Œ ≈ÿΩ∫√ƒ ±≥√º.
		}
		if( m_pTail == NULL )	// æ∆¡˜ «“¥Á æ»µ∆¿∏∏È «“¥Á«œ∞Ì.
		{
			m_pTail = (CTailEffectBelt*)g_TailEffectMng.AddEffect( g_Neuz.m_pd3dDevice, "etc_Tail2.bmp", 2, 0.35f );
		}
		
		D3DXVECTOR3	vPos1, vPos2;
		FLOAT		fAngXZ = GetAngle();
		FLOAT		fAngH  = GetAngleX();
		
		fAngXZ -= 90.0f;
		if( fAngXZ < 0 )
			fAngXZ += 360.0f;
		AngleToVector( &vPos1, fAngXZ, -fAngH, 0.05f );
		vPos1 += GetPos();
		
		fAngXZ = GetAngle();
		fAngH  = GetAngleX();
		
		fAngXZ += 90.0f;
		if( fAngXZ > 360.0f )
			fAngXZ -= 360.0f;
		AngleToVector( &vPos2, fAngXZ, -fAngH, 0.05f );
		vPos2 += GetPos();

		if( m_pTail )
			m_pTail->CreateTail( vPos1, vPos2 );
		
		vDelta = m_pSfxObj->m_vPos - m_vPosDest;		// ¥ÎªÛ∞˙¿« ∞≈∏Æ
		if( D3DXVec3LengthSq( &vDelta ) < 0.4f * 0.4f )		// ∆¯πﬂ ∞≈∏Æ±Ó¡ˆ ¥Ÿ∞°∞°∏È √Êµπ.
		{
			if( m_dwSndHit != NULL_ID )
				PLAYSND( m_dwSndHit, &GetPos() );		// ªÁøÓµÂ «√∑π¿Ã.
			DamageToTarget();
			// ∆¯πﬂ ø¿∫Í¡ß∆Æ ª˝º∫.
			if( m_dwSfxHit != NULL_ID )
			{
				CSfx *pSfx = CreateSfx( D3DDEVICE, m_dwSfxHit, GetPos(), m_idSrc, m_vPosDest, m_idDest, 0 );
				if( pSfx )
				{
					pSfx->SetAngle( m_pSfxObj->m_vRotate.y );
				}
			}
			Delete();	// this¥¬ ªË¡¶.
		}
	}
	m_nFrame ++;

	if( m_nFrame >= SEC1 * 10 )		// ≥ π´ø¿∑° ?æ∆∞°¡ˆ æ ∞‘ 10√ ¡ˆ≥™∏È º“∏Í
		Delete();	
}
void CSfxItemRangeAtk1::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render2( pd3dDevice );
}

// ø‰ø‰ª˝º∫
D3DXVECTOR3	CSfxItemYoyoAtk::SplineSlerp( D3DXVECTOR3 *v1, D3DXVECTOR3 *v2, D3DXVECTOR3 *v3, D3DXVECTOR3 *v4, float fSlerp )
{
	D3DXVECTOR3		vResult;
	
	float	t = fSlerp;
	float	t2 = t * t;
	float	t3 = t2 * t;
	
	D3DXVECTOR3		m0, m1;
	const float alpha = 0.0f;
	
	m0 = ((1 - alpha) / 2.0f) * 
		((*v2 - *v1) + *v3 - *v2);
	m1 = ((1 - alpha) / 2.0f) *
		((*v3 - *v2) + *v4 - *v3);
	
	vResult = (((2 * t3) - (3 * t2) + 1) * *v2) +
		((t3 - (2 * t2) + t) * m0) + 
		((t3 - t2) * m1) +
		(((-2 * t3) + (3 * t2)) *
		*v3 );
	
	return vResult;
}

#define MAX_DELAY    10
CSfxItemYoyoAtk::CSfxItemYoyoAtk() : CSfxShoot()
{
	m_pTail			= NULL;
	m_nStep			= 0;
	m_nCount        = 0;
	m_nType			= 0;
	m_nDelayCount = 0;
}

CSfxItemYoyoAtk::~CSfxItemYoyoAtk()
{
	if( m_pTail )
		g_TailEffectMng.Delete( m_pTail );
	m_pTail = NULL;
}


void CSfxItemYoyoAtk::MakePath(int nType)
{
	// ±Ê¿Ã 5πÃ≈Õ
	m_fMaxLength = 50.0f;
	
	CMover* pObj = (CMover*)prj.GetCtrl( m_idSrc );	

	if( !IsValidObj(pObj) )
	{
		Delete();	// this¥¬ ªË¡¶.
		return;
	}

	FLOAT fLength;
	
	fLength = D3DXVec3LengthSq( &D3DXVECTOR3(m_vPosDest - GetPos()) );

	if( fLength > 7.0f * 7.0f )  // 7πÃ≈Õ ¿ÃªÛ ∏¯ ≥Ø∂Û∞®
		fLength = 49.0f;

	if( fLength < 3.0f * 3.0f )  // 3πÃ≈Õ∫∏¥Ÿ ¿€¿∏∏È 2πÃ≈Õ
		fLength = 12.0f;

	fLength /= 5.0f;

	m_nType = nType;

	///////////////////////////////////////////////////////////////////////////////////////////////
	D3DXVECTOR3		aNewPos[7];

	m_v3SrcPos = GetPos();
	D3DXVECTOR3 vLocal;
	
	// 5πÃ≈Õ æ’ ¿ßƒ°±∏«‘
	AngleToVectorXZ( &vLocal, 0, fLength );
	aNewPos[0] = vLocal;
	aNewPos[0].y -= 0.2f;
	
	AngleToVectorXZ( &vLocal, 0, fLength+0.5f );
	aNewPos[1] = vLocal;
	aNewPos[1].y -= 0.2f;

	AngleToVectorXZ( &vLocal, 0, fLength+1.0f );
	aNewPos[2] = vLocal;
	aNewPos[2].y -= 0.1f;

	// ≥°¡°
	AngleToVectorXZ( &vLocal, 0, fLength+1.2f );
	aNewPos[3] = vLocal;

	// µ⁄µπæ∆∞°±‚
	AngleToVectorXZ( &vLocal, 0, fLength+1.0f );
	aNewPos[4] = vLocal;
	aNewPos[4].y += 0.1f;
	
	AngleToVectorXZ( &vLocal, 0, fLength+0.5f );
	aNewPos[5] = vLocal;
	aNewPos[5].y += 0.2f;
	
	AngleToVectorXZ( &vLocal, 0, fLength );
	aNewPos[6] = vLocal;
	aNewPos[6].y += 0.2f;

	D3DXVECTOR3 vSlp;
	int		a, b, c, d;
	int		nMaxVertex = 7;

	m_nMaxSpline = 0;
	for( int i = 0; i < nMaxVertex-1; i ++ )
	{
		// i ~ i+1ªÁ¿Ã∏¶ ∫∏∞£«—¥Ÿ.  
		a = i - 1;		if( a < 0 )	a = 0;
		b = i;
		c = i+1;
		d = i+2;		if( d >= nMaxVertex )	d = nMaxVertex - 1;
		for( int j = 0; j < MAX_SF_SLERP+1; j ++ )
		{
			if( m_nMaxSpline >= 30 )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "CSfxItemYoyoAtk Spline : π¸¿ß √ ∞˙ %d" ), m_nMaxSpline );
				break;
			}
			vSlp = SplineSlerp( &aNewPos[a], &aNewPos[b], &aNewPos[c], &aNewPos[d], (float)j / MAX_SF_SLERP );
			m_aSpline[ m_nMaxSpline ] = vSlp;
			m_nMaxSpline ++;
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	D3DXVECTOR3 vDir      = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
	D3DXVECTOR3 vDestNor  = m_vPosDest - GetPos();

	D3DXVECTOR3 vAxis;
	D3DXQUATERNION   qRot;
	
	FLOAT       fTheta;
	D3DXVec3Normalize( &vDestNor, &vDestNor );
	D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
	fTheta = D3DXVec3Dot( &vDir, &vDestNor );
	D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );
	
	D3DXMATRIX mRot;
	D3DXMatrixRotationQuaternion( &mRot, &qRot );
	///////////////////////////////////////////////////////////////////////////////////////////////
				
	// Ω∫«√∂Û¿Œ ∫∏∞£µ» ¡°µÈ ø˘µÂ ∫Ø»Ø
	D3DXVECTOR3 vPosPos    = m_v3SrcPos;
	D3DXMATRIX mScal;
	D3DXMatrixScaling( &mScal, 1.0f, 1.0f, 1.0f );

	for( int i=0; i<m_nMaxSpline; i++ )
	{
		D3DXMATRIX mWorld = mRot * mScal * pObj->GetMatrixTrans();
		mWorld._42 = vPosPos.y;
		D3DXVec3TransformCoord( &m_aSpline[i], &m_aSpline[i], &mWorld );
	}
}

void CSfxItemYoyoAtk::Process()
{
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );	

	if( !IsValidObj(pObjSrc) )
	{
		Delete();	// this¥¬ ªË¡¶.
		return;
	}
	
	if( m_bHit == FALSE ) 
	{
		if( pObjSrc )
		{
			((CModelObject*)pObjSrc->m_pModel)->SetEffect( m_nType, XE_HIDE ); 
		}

		if( m_nDelayCount )
		{
			m_nDelayCount--;
			return;
		}

		// ≥Øæ∆∞°¥¬ ¥‹∞Ë
		if( m_nStep == 0 )
		{
			D3DXVECTOR3 vPos   = GetPos();
			D3DXVECTOR3 vDelta = m_aSpline[0] - vPos;
			D3DXVec3Normalize( &vDelta, &vDelta );	
			vPos += vDelta * 0.3f;		
			m_pSfxObj->m_vPos = vPos;
			SetPos( vPos );
			
			FLOAT fLen = D3DXVec3LengthSq( &D3DXVECTOR3(m_aSpline[0] - vPos) );
			if( fLen <= 0.05f )
			{
				m_nStep = 1;
				m_nDelayCount = 6;
			}
		}
		else
		// »∏¿¸«œ¥¬ ¥‹∞Ë
		if( m_nStep == 1 )
		{
			D3DXVECTOR3 vPosPos    = m_aSpline[m_nCount];
			
			m_pSfxObj->m_vPos = vPosPos;
			SetPos( vPosPos );

			m_nCount++;

			if( m_nCount >= m_nMaxSpline )
			{
				m_nStep = 2;
			}
		}
		else
		// µ«µπæ∆ ø¿¥¬ ¥‹∞Ë
		if( m_nStep == 2 )
		{
			D3DXVECTOR3 v3SrcPos;
			CModelObject *pModel = (CModelObject *)pObjSrc->m_pModel;
			pModel->GetHandPos( &v3SrcPos, m_nType, pObjSrc->GetMatrixWorld() );
			
			D3DXVECTOR3 vPos   = GetPos();
			D3DXVECTOR3 vDelta = v3SrcPos - vPos;
			D3DXVec3Normalize( &vDelta, &vDelta );		// ≈∏∞Ÿ¬  ¥‹¿ß∫§≈Õ ±∏«ÿº≠
			vPos += vDelta * 0.4f;			// ±◊ ∫§≈Õ∑Œ º”µµ∏¶ ∞·¡§«ÿº≠ ¡¬«• ¿Ãµø.
			m_pSfxObj->m_vPos = vPos;
			SetPos( vPos );

			FLOAT fLen = D3DXVec3LengthSq( &D3DXVECTOR3(v3SrcPos - vPos) );
			if( fLen <= 0.1f )
			{
				if( m_dwSndHit != NULL_ID )
					PLAYSND( m_dwSndHit, &GetPos() );		// ªÁøÓµÂ «√∑π¿Ã.

				// ∆¯πﬂ ø¿∫Í¡ß∆Æ ª˝º∫.
				if( m_dwSfxHit != NULL_ID )
				{
					CSfx *pSfx = CreateSfx( D3DDEVICE, m_dwSfxHit, GetPos(), m_idSrc, m_vPosDest, m_idDest, 0 );
					if( pSfx )
					{
						pSfx->SetAngle( m_pSfxObj->m_vRotate.y );
					}
				}
				Delete();	// this¥¬ ªË¡¶.
				return;
			}
		}

		if( m_pSfxObj->Process() )		// æ÷¥œ∏ﬁ¿Ãº«¿∫ ∑Á«¡.
		{
			m_pSfxObj->m_nCurFrame = 0;
		}
		if( m_pTail == NULL )	// æ∆¡˜ «“¥Á æ»µ∆¿∏∏È «“¥Á«œ∞Ì.
		{
			if( m_pSfxObj->m_pSfxBase->Part(0) )
				m_pTail = (CTailEffectModel*)g_TailEffectMng.AddEffect( g_Neuz.m_pd3dDevice, m_pSfxObj->m_pSfxBase->Part(0)->m_strTex, 100, 30.0f );
		}

		if( m_pTail )
		{
			D3DXMATRIX mWorld;
			D3DXMatrixTranslation( &mWorld, GetPos().x, GetPos().y, GetPos().z );
			m_pTail->CreateTail( &mWorld );
		}
	}
	m_nFrame ++;

	if( m_nFrame >= SEC1 * 5 )		// ≥ π´ø¿∑° ?æ∆∞°¡ˆ æ ∞‘ 5√ ¡ˆ≥™∏È º“∏Í
		Delete();	
}

void CSfxItemYoyoAtk::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render2( pd3dDevice );
}



// æ˚≈Õ∏Æ »≠ªÏ ª˝º∫«—¥Ÿ. «œ≥™¥¬ ¡¯¬•~
CSfxItemRangeAtk_JunkBow::CSfxItemRangeAtk_JunkBow() : CSfxShoot()
{
	m_pTail[0]			 = NULL;
	m_pTail[1]			 = NULL;
	m_pTail[2]			 = NULL;
}
CSfxItemRangeAtk_JunkBow::~CSfxItemRangeAtk_JunkBow()
{
	for( int i=0; i<3; i++ )
	{
		if( m_pTail[i] )
			g_TailEffectMng.Delete( m_pTail[i] );

		m_pTail[i] = NULL;
	}
}

void CSfxItemRangeAtk_JunkBow::Process()
{
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );	// ¿Øµµ≈∫ «¸¿Ã±‚∂ßπÆø° ≈∏∞Ÿ¿∫ «◊ªÛ ¿–æÓ∫¡æﬂ «—¥Ÿ.
	if( IsValidObj(pObjDest) )
	{
		m_vPosDest = pObjDest->GetPos() + D3DXVECTOR3( 0, 1.0f, 0 );
	} else
	{
		// ≥Øæ∆∞°¥Ÿ ≈∏∞Ÿ¿Ã æ¯æÓ¡≥¿∏∏È ∏∂¡ˆ∏∑¡¬«•∑Œ ≥Øæ∆∞°º≠ ≈Õ¡ˆ∞‘.
		Delete();	// this¥¬ ªË¡¶.
		int a = 0;
	}
	
	if( m_bHit == FALSE ) 
	{
		if( m_pSfxObj->Process() )		// æ÷¥œ∏ﬁ¿Ãº«¿∫ ∑Á«¡.
		{
			m_pSfxObj->m_nCurFrame = 0;
		}
		D3DXVECTOR3 vPos = GetPos();
		D3DXVECTOR3 vDelta = m_vPosDest - vPos;
		D3DXVec3Normalize( &vDelta, &vDelta );		// ≈∏∞Ÿ¬  ¥‹¿ß∫§≈Õ ±∏«ÿº≠
		vPos += vDelta * 0.7f;			// ±◊ ∫§≈Õ∑Œ º”µµ∏¶ ∞·¡§«ÿº≠ ¡¬«• ¿Ãµø.
		m_pSfxObj->m_vPos = vPos;
		SetPos( vPos );
		
		//if( m_bDir )
		{
			// ∏∂π˝∞¥√º πÊ«‚∫§≈Õ∑Œ ∏µ® πÊ«‚ µπ∏Æ±‚~~~
			D3DXVECTOR3 vDir      = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
			D3DXVECTOR3 vDestNor  = m_vPosDest - m_pSfxObj->m_vPos;
			D3DXVECTOR3 vAxis;
			D3DXQUATERNION   qRot;
			
			FLOAT       fTheta;
			D3DXVec3Normalize( &vDestNor, &vDestNor );
			D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
			fTheta = D3DXVec3Dot( &vDir, &vDestNor );
			D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );
			
			D3DXVECTOR3 vYPW;
			
			QuaternionRotationToYPW( qRot, vYPW );
			m_pSfxObj->m_vRotate.x = m_SfxObj2.m_vRotate.x = D3DXToDegree(vYPW.x);
			m_pSfxObj->m_vRotate.y = m_SfxObj2.m_vRotate.y = D3DXToDegree(vYPW.y);
			m_pSfxObj->m_vRotate.z = m_SfxObj2.m_vRotate.z = D3DXToDegree(vYPW.z);

		}
		
		for( int i=0; i<3; i++ )
		{
			if( m_pTail[i] )
			{
				if( m_pTail[i]->GetType() != 2 )	// ª˝º∫µ«æ˙¥¯ ≤ø∏Æ∞Ì ¿œπ›∫∏µÂøÎ¿Ã æ∆¥œ∏È 
					m_pTail[i]->ChangeTexture( D3DDEVICE, "etc_Tail2.bmp", 2 );	// ¿œπ›∫∏µÂøÎ¿∏∑Œ ≈ÿΩ∫√ƒ ±≥√º.
			}
			if( m_pTail[i] == NULL )	// æ∆¡˜ «“¥Á æ»µ∆¿∏∏È «“¥Á«œ∞Ì.
			{
				m_pTail[i] = (CTailEffectBelt*)g_TailEffectMng.AddEffect( g_Neuz.m_pd3dDevice, "etc_Tail2.bmp", 2, 0.35f );
			}
		}
		
		D3DXVECTOR3	vPos1, vPos2;
		FLOAT		fAngXZ = GetAngle();
		FLOAT		fAngH  = GetAngleX();
		
		fAngXZ -= 90.0f;
		if( fAngXZ < 0 )
			fAngXZ += 360.0f;
		AngleToVector( &vPos1, fAngXZ, -fAngH, 0.05f );
		vPos1 += GetPos();
		
		fAngXZ = GetAngle();
		fAngH  = GetAngleX();
		
		fAngXZ += 90.0f;
		if( fAngXZ > 360.0f )
			fAngXZ -= 360.0f;
		AngleToVector( &vPos2, fAngXZ, -fAngH, 0.05f );
		vPos2 += GetPos();

		vPos1.y -= 0.5f;
		vPos2.y -= 0.5f;
		
		for( int i=0; i<3; i++ )
		{
			if( m_pTail[i] )
			{
				m_pTail[i]->CreateTail( vPos1, vPos2 );
			}

			vPos1.y += 0.5f;
			vPos2.y += 0.5f;
		}
		
		vDelta = m_pSfxObj->m_vPos - m_vPosDest;		// ¥ÎªÛ∞˙¿« ∞≈∏Æ
		if( D3DXVec3LengthSq( &vDelta ) < 0.4f * 0.4f )		// ∆¯πﬂ ∞≈∏Æ±Ó¡ˆ ¥Ÿ∞°∞°∏È √Êµπ.
		{
			if( m_dwSndHit != NULL_ID )
				PLAYSND( m_dwSndHit, &GetPos() );		// ªÁøÓµÂ «√∑π¿Ã.
			DamageToTarget();
			// ∆¯πﬂ ø¿∫Í¡ß∆Æ ª˝º∫.
			if( m_dwSfxHit != NULL_ID )
			{
				CSfx *pSfx = CreateSfx( D3DDEVICE, m_dwSfxHit, GetPos(), m_idSrc, m_vPosDest, m_idDest, 0 );
				if( pSfx )
				{
					pSfx->SetAngle( m_pSfxObj->m_vRotate.y );
				}
			}
			Delete();	// this¥¬ ªË¡¶.
		}
	}
	m_nFrame ++;

	if( m_nFrame >= SEC1 * 10 )		// ≥ π´ø¿∑° ?æ∆∞°¡ˆ æ ∞‘ 10√ ¡ˆ≥™∏È º“∏Í
		Delete();	
}
void CSfxItemRangeAtk_JunkBow::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();

	// ¡¬«• πÈæ˜
	D3DXVECTOR3 vPosBackup = m_pSfxObj->m_vPos;
	// ¡¯¬• »≠ªÏ
	m_pSfxObj->Render2( pd3dDevice );
	
	// ∞°¬• 1
	m_pSfxObj->m_vPos.y += 0.5f;
	m_pSfxObj->Render2( pd3dDevice );

	// ∞°¬• 2
	m_pSfxObj->m_vPos = vPosBackup;
	m_pSfxObj->m_vPos.y -= 0.5f;
	m_pSfxObj->Render2( pd3dDevice );


	// ¡¬«•∫π±∏
	m_pSfxObj->m_vPos = vPosBackup;
	
}


// æ∆∑°∑Œ ∂≥æÓ¡ˆ¥¬ »≠ªÏ ¿Ã∆Â∆Æ
CSfxItemRangeAtk1_Allow::CSfxItemRangeAtk1_Allow() : CSfxShoot()
{
	m_nFrame = 0;
	m_fSpeed = xRandomF( 0.7f ) + 0.2f;
}

CSfxItemRangeAtk1_Allow::~CSfxItemRangeAtk1_Allow()
{
}

void CSfxItemRangeAtk1_Allow::Process()
{
	if( m_pSfxObj->Process() )		// æ÷¥œ∏ﬁ¿Ãº«¿∫ ∑Á«¡.
	{
		m_pSfxObj->m_nCurFrame = 0;
	}

	D3DXVECTOR3 vPos = GetPos();
	D3DXVECTOR3 vDelta = D3DXVECTOR3( 0.0f, -1.0f, 0.0f );
	D3DXVec3Normalize( &vDelta, &vDelta );		// ≈∏∞Ÿ¬  ¥‹¿ß∫§≈Õ ±∏«ÿº≠

	vPos += vDelta * m_fSpeed;			// ±◊ ∫§≈Õ∑Œ º”µµ∏¶ ∞·¡§«ÿº≠ ¡¬«• ¿Ãµø.

	CWorld* pWorld = g_WorldMng.Get();
	FLOAT fHeight = pWorld->GetLandHeight_Fast( vPos.x, vPos.z );

	// ∂• ≥Ù¿Ã∫∏¥Ÿ ≥∑¿∏∏È æ»µ 
	if( vPos.y < fHeight )
	{
		m_nFrame++;
		vPos.y = fHeight;
		
		CSfx *pSfx = CreateSfx( D3DDEVICE, XI_INDEX( 604, XI_SKILL_RAG_BOW_ARROWRAIN01 ), vPos );//, m_idSrc, vPos, m_idDest, 0 );		
		Delete();
	}

	m_pSfxObj->m_vPos = vPos;
		
	SetPos( vPos );
}
void CSfxItemRangeAtk1_Allow::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render2( pd3dDevice );
}

#define MAX_ALLOW		50

// CSfxItemRangeAtk1_Allowª˝º∫ ∞¸∏Æ - Ω«¡¶ ªÁøÎµ«æÓ¡ˆ¥¬ ∫Œ∫–
CSfxItemRangeAtk1_AllowRain::CSfxItemRangeAtk1_AllowRain() : CSfxShoot()
{
	m_nFrame = 0;
	m_nCount = 0;
}

CSfxItemRangeAtk1_AllowRain::~CSfxItemRangeAtk1_AllowRain()
{
}

void CSfxItemRangeAtk1_AllowRain::Process()
{
	D3DXVECTOR3 vPos = GetPos();
	
	CWorld* pWorld = g_WorldMng.Get();
	FLOAT fHeight = pWorld->GetLandHeight_Fast( vPos.x, vPos.z );
	
	// 10πÃ≈Õ¿ßø°º≠ ∂≥æÓ∂ﬂ∏≤
	vPos.y = fHeight + xRandomF(2.0f) + 9.0f;

	m_nFrame++;

	if( m_nCount < MAX_ALLOW )
	{
		if( m_nFrame >= SEC1 * 0.07f )
		{
			m_nFrame = 0;
			vPos.x += (xRandomF( 6.0f ) + -3.0f);
			vPos.z += (xRandomF( 6.0f ) + -3.0f);

			CSfx *pSfx = CreateSfx( D3DDEVICE, XI_INDEX( 603, XI_SKILL_RAG_BOW_ARROWRAIN ), vPos, m_idSrc, m_vPosDest, m_idDest, 0 );

			vPos.x += (xRandomF( 6.0f ) + -3.0f);
			vPos.z += (xRandomF( 6.0f ) + -3.0f);
			
			pSfx = CreateSfx( D3DDEVICE, XI_INDEX( 603, XI_SKILL_RAG_BOW_ARROWRAIN ), vPos, m_idSrc, m_vPosDest, m_idDest, 0 );
			m_nCount++;
		}
	}
	else
		Delete();
}
void CSfxItemRangeAtk1_AllowRain::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
}

CSfxItemRangeAtk1_Stone::CSfxItemRangeAtk1_Stone() : CSfxShoot()
{
	m_nFrame = 0;
	m_fSpeed = xRandomF( 0.7f ) + 0.7f;
}

CSfxItemRangeAtk1_Stone::~CSfxItemRangeAtk1_Stone()
{
}

void CSfxItemRangeAtk1_Stone::Process()
{
	if( m_pSfxObj->Process() )		// æ÷¥œ∏ﬁ¿Ãº«¿∫ ∑Á«¡.
	{
		m_pSfxObj->m_nCurFrame = 0;
	}
	
	D3DXVECTOR3 vPos = GetPos();
	D3DXVECTOR3 vDelta = D3DXVECTOR3( 0.0f, -1.0f, 0.0f );
	D3DXVec3Normalize( &vDelta, &vDelta );		// ≈∏∞Ÿ¬  ¥‹¿ß∫§≈Õ ±∏«ÿº≠
	
	vPos += vDelta * m_fSpeed;			// ±◊ ∫§≈Õ∑Œ º”µµ∏¶ ∞·¡§«ÿº≠ ¡¬«• ¿Ãµø.
	
	CWorld* pWorld = g_WorldMng.Get();
	FLOAT fHeight = pWorld->GetLandHeight_Fast( vPos.x, vPos.z );
	
	// ∂• ≥Ù¿Ã∫∏¥Ÿ ≥∑¿∏∏È æ»µ 
	if( vPos.y < fHeight+2.5f )
	{
		m_nFrame++;
		vPos.y = fHeight;
		
		CSfx *pSfx = CreateSfx( D3DDEVICE, XI_INDEX( 1743, XI_SKILL_CIRCLE_DUST ), vPos );		
		if( pSfx )
		{
			pSfx->SetScale( D3DXVECTOR3( 3.0f, 3.0f, 3.0f ) );					
		}			
		Delete();
	}
	
	m_pSfxObj->m_vPos = vPos;
	
	SetPos( vPos );
}
void CSfxItemRangeAtk1_Stone::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render2( pd3dDevice );
}

#define MAX_STONE		50

// CSfxItemRangeAtk1_Allowª˝º∫ ∞¸∏Æ - Ω«¡¶ ªÁøÎµ«æÓ¡ˆ¥¬ ∫Œ∫–
CSfxItemRangeAtk1_StoneRain::CSfxItemRangeAtk1_StoneRain() : CSfxShoot()
{
	m_nFrame = 0;
	m_nCount = 0;
}

CSfxItemRangeAtk1_StoneRain::~CSfxItemRangeAtk1_StoneRain()
{
}

void CSfxItemRangeAtk1_StoneRain::Process()
{
	D3DXVECTOR3 vPos = GetPos();
	
	CWorld* pWorld = g_WorldMng.Get();
	FLOAT fHeight = pWorld->GetLandHeight_Fast( vPos.x, vPos.z );
	
	// 10πÃ≈Õ¿ßø°º≠ ∂≥æÓ∂ﬂ∏≤
	vPos.y = fHeight + xRandomF(2.0f) + 40.0f;
	
	m_nFrame++;
	
	if( m_nCount < MAX_STONE )
	{
		if( m_nFrame >= SEC1 * 0.2f )
		{
			m_nFrame = 0;
			vPos.x += (xRandomF( 40.0f ) + -20.0f);
			vPos.z += (xRandomF( 40.0f ) + -20.0f);
			
			CSfx *pSfx = CreateSfx( D3DDEVICE, XI_INDEX( 1744, XI_SKILL_DROP_DUST ), vPos, m_idSrc, m_vPosDest, m_idDest, 0 );
			if( pSfx )
			{
				pSfx->SetScale( D3DXVECTOR3( 8.0f, 8.0f, 8.0f ) );					
			}
			m_nCount++;
		}
	}
	else
		Delete();
}
void CSfxItemRangeAtk1_StoneRain::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
}

//
// ¿œ¡˜º±¿∏∑Œ ª∏æÓ≥™∞°¥¬ «¸≈¬¿« ¿Â∞≈∏Æ ∞¯∞›
//
CSfxAtkStraight::CSfxAtkStraight()
{
	m_dwExplosion = NULL_ID;
}
CSfxAtkStraight::~CSfxAtkStraight()
{
}

// πﬂªÁ√º¿« ¿Ãµø¡ı∞°∑Æ¿ª º±∞ËªÍ«‘. 
void CSfxAtkStraight::ShootSfx( float fAngXZ, float fAngH, float fSpeed, DWORD dwExplosion )
{
	m_pSfxObj->m_vPos = GetPos();		// sfx∏µ®¿« ¿ßƒ°∏¶ ºº∆√.
	m_dwExplosion = dwExplosion;

	AngleToVector( &m_vDelta, fAngXZ, fAngH, fSpeed );		// ¿Ãµø∑Æ vDelta∏¶ ±∏«‘.

	// ∏∂π˝∞¥√º πÊ«‚∫§≈Õ∑Œ ∏µ® πÊ«‚ µπ∏Æ±‚~~~
	D3DXVECTOR3 vDir      = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
	D3DXVECTOR3 vDestNor  = m_vDelta;//m_vPosDest - GetPos();
	D3DXVECTOR3 vAxis;
	D3DXQUATERNION   qRot;
	
	FLOAT       fTheta;
	D3DXVec3Normalize( &vDestNor, &vDestNor );
	D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
	fTheta = D3DXVec3Dot( &vDir, &vDestNor );
	D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );
	
	D3DXVECTOR3 vYPW;
	
	QuaternionRotationToYPW( qRot, vYPW );
	m_pSfxObj->m_vRotate.x = D3DXToDegree(vYPW.x);
	m_pSfxObj->m_vRotate.y = D3DXToDegree(vYPW.y);
	m_pSfxObj->m_vRotate.z = D3DXToDegree(vYPW.z);
	
}
				  
void CSfxAtkStraight::Process()
{
	if( m_pSfxObj->Process() )			// SFX æ÷¥œ∏ﬁ¿Ãº«¿ª √≥∏Æ«œ∞Ì
		m_pSfxObj->m_nCurFrame = 0;		// ∏Ò«•¡°ø° ∏¬¿ª∂ß±Ó¡ˆ ∑Á«¡Ω√≈∞±‚ ¿ß«ÿ ¥ŸΩ√ 0«¡∑π¿”¿∏∑Œ µπ∑¡≥ı¥¬¥Ÿ.
	
	m_pSfxObj->m_vPos += m_vDelta;		// ¡¬«• ¿Ãµø.
	SetPos( m_pSfxObj->m_vPos );		// thisµµ ¡¬«• ∞∞¿Ã ¿Ãµø.
	
//sun: ∏ﬁ≈◊ø¿¥œƒø ∆ƒ¿ÃæÓ πﬂªÁ!!!
	#ifdef __CLIENT
		//D3DXVECTOR3 vDir = v3 - v1;
		//D3DXVec3Normalize( &vDir, &vDir );
		D3DXVECTOR3 vTemp = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		D3DXVECTOR3 vScal = D3DXVECTOR3( 0.3f, 0.3f, 0.3f );
		D3DXVECTOR3 vModelScal = GetScale();

		vScal.x *= vModelScal.x;
		vScal.y *= vModelScal.y;
		vScal.z *= vModelScal.z;

		for( int i = 0; i < 2; i ++ )
		{
			//vTemp = vDir * (xRandomF( 0.2f ) + 0.1f );
			//vTemp *= 0.8f;
			//vTemp.y -= (xRandomF( 0.05f ) + 0.01f );
			
			//extern CPartsFireDragon	g_FireDragon;
			//g_FireDragon.Create( D3DDEVICE, m_pSfxObj->m_vPos, XI_NAT_FIRE01_ADV, vScal, vTemp );
		}
	#endif

	if( IsRangeObj( m_vPosDest, 0 ) )							// ∏Ò«•¡¬«•ø° ¥Ÿ¥Ÿ∏£∏È
	{
		CreateSfx( D3DDEVICE, m_dwExplosion, m_vPosDest );		// ∆¯πﬂ ¿Ã∆Â∆Æ
		Delete();												// ≥≠ ªË¡¶.
		return;
	}

	m_nFrame++;
	if( m_nFrame > 60 * 3 )
	{
		Delete();
	}
}
void CSfxAtkStraight::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !IsVisible() || ( IsCull() && GetType() != 1 ))
		return;
	if( IsUpdateMatrix() )
		UpdateMatrix();
	
	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( m_dwType, m_dwIndex );
	// Model¿Ã æ¯¿∏∏È √‚∑¬«œ¡ˆ æ ¿Ω 
	if( lpModelElem == NULL )
		return;
	
	m_pSfxObj->m_vPos = GetPos();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->m_matScale = m_matScale;
	m_pSfxObj->Render2( pd3dDevice, NULL );
	
}


CSfxItemWandAtk2::CSfxItemWandAtk2()
{
	m_SfxObj2.SetSfx("sfx_HitWand01"); // ∏¬¥¬ º¯∞£¿« SFX∏¶ µ˚∑Œ ∑Œµ˘«—¥Ÿ.
	m_bHit=FALSE;
}
CSfxItemWandAtk2::~CSfxItemWandAtk2()
{
}
void CSfxItemWandAtk2::Process()
{
	if(m_nFrame==0) {
		CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
		if(pObjSrc) m_pSfxObj->m_vPos=pObjSrc->GetPos()+D3DXVECTOR3(.0f,1.0f,.0f);
	}
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );
	if(pObjDest) m_SfxObj2.m_vPos=pObjDest->GetPos()+D3DXVECTOR3(.0f,1.0f,.0f);

	if(m_bHit==FALSE) {
		m_pSfxObj->Process();
		if(m_pSfxObj->m_nCurFrame==20)
		{
			m_pSfxObj->m_nCurFrame=0;
		}
		D3DXVECTOR3 vDelta=m_SfxObj2.m_vPos-m_pSfxObj->m_vPos;
		D3DXVec3Normalize(&vDelta,&vDelta);
		m_pSfxObj->m_vPos+=vDelta*.2f;
		SetPos(m_pSfxObj->m_vPos);
		vDelta=m_pSfxObj->m_vPos-m_SfxObj2.m_vPos;
		FLOAT fAngle;
		if(vDelta.z>0) {
			fAngle=(float)(atan(vDelta.x/vDelta.z)*180/3.1415926f)+180;
		}
		else {
			fAngle=(float)(atan(vDelta.x/vDelta.z)*180/3.1415926f);
		}
		m_pSfxObj->m_vRotate.y=m_SfxObj2.m_vRotate.y=fAngle;
		if(D3DXVec3Length(&vDelta)<.2f) {
			m_bHit=TRUE;
		}
	}
	else {
		if(m_SfxObj2.m_nCurFrame==0) {
			// ø©±‚¥Ÿ∞° ∏¬¥¬º¯∞£√≥∏Æ
			DamageToTarget();
		}
		if(m_SfxObj2.Process()) {
			Delete();
		}
	}
	m_nFrame++;
}
void CSfxItemWandAtk2::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	if(m_bHit==FALSE) 
	{
		m_pSfxObj->Render( pd3dDevice );
	}
	else 
	{
		if( !g_Option.m_bSFXRenderOff )
			m_SfxObj2.Render( pd3dDevice );
	}
}
CSfxItemWandAtk3::CSfxItemWandAtk3()
{
	m_SfxObj2.SetSfx("sfx_HitWand01"); // ∏¬¥¬ º¯∞£¿« SFX∏¶ µ˚∑Œ ∑Œµ˘«—¥Ÿ.
	m_bHit=FALSE;
}
CSfxItemWandAtk3::~CSfxItemWandAtk3()
{
}
void CSfxItemWandAtk3::Process()
{
	if(m_nFrame==0) {
		CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
		if(pObjSrc) m_pSfxObj->m_vPos=pObjSrc->GetPos()+D3DXVECTOR3(.0f,1.0f,.0f);
	}
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );
	if(pObjDest) m_SfxObj2.m_vPos=pObjDest->GetPos()+D3DXVECTOR3(.0f,1.0f,.0f);

	if(m_bHit==FALSE) {
		m_pSfxObj->Process();
		if(m_pSfxObj->m_nCurFrame==20)
		{
			m_pSfxObj->m_nCurFrame=0;
		}
		D3DXVECTOR3 vDelta=m_SfxObj2.m_vPos-m_pSfxObj->m_vPos;
		D3DXVec3Normalize(&vDelta,&vDelta);
		m_pSfxObj->m_vPos+=vDelta*.2f;
		SetPos(m_pSfxObj->m_vPos);
		vDelta=m_pSfxObj->m_vPos-m_SfxObj2.m_vPos;
		FLOAT fAngle;
		if(vDelta.z>0) {
			fAngle=(float)(atan(vDelta.x/vDelta.z)*180/3.1415926f)+180;
		}
		else {
			fAngle=(float)(atan(vDelta.x/vDelta.z)*180/3.1415926f);
		}
		m_pSfxObj->m_vRotate.y=m_SfxObj2.m_vRotate.y=fAngle;
		if(D3DXVec3Length(&vDelta)<.2f) {
			m_bHit=TRUE;
		}
	}
	else {
		if(m_SfxObj2.m_nCurFrame==0) {
			// ø©±‚¥Ÿ∞° ∏¬¥¬º¯∞£√≥∏Æ
			DamageToTarget();
		}
		if(m_SfxObj2.Process()) {
			Delete();
		}
	}
	m_nFrame++;
}
void CSfxItemWandAtk3::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	if(m_bHit==FALSE) 
	{
		m_pSfxObj->Render( pd3dDevice );
	}
	else 
	{
		if( !g_Option.m_bSFXRenderOff )
			m_SfxObj2.Render( pd3dDevice );
	}
}
CSfxItemWandAtk4::CSfxItemWandAtk4()
{
	m_SfxObj2.SetSfx("sfx_HitWand01"); // ∏¬¥¬ º¯∞£¿« SFX∏¶ µ˚∑Œ ∑Œµ˘«—¥Ÿ.
	m_bHit=FALSE;
}
CSfxItemWandAtk4::~CSfxItemWandAtk4()
{
}
void CSfxItemWandAtk4::Process()
{
	if(m_nFrame==0) {
		CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
		if(pObjSrc) m_pSfxObj->m_vPos=pObjSrc->GetPos()+D3DXVECTOR3(.0f,1.0f,.0f);
	}
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );
	if(pObjDest) m_SfxObj2.m_vPos=pObjDest->GetPos()+D3DXVECTOR3(.0f,1.0f,.0f);

	if(m_bHit==FALSE) {
		m_pSfxObj->Process();
		if(m_pSfxObj->m_nCurFrame==20)
		{
			m_pSfxObj->m_nCurFrame=0;
		}
		D3DXVECTOR3 vDelta=m_SfxObj2.m_vPos-m_pSfxObj->m_vPos;
		D3DXVec3Normalize(&vDelta,&vDelta);
		m_pSfxObj->m_vPos+=vDelta*.2f;
		SetPos(m_pSfxObj->m_vPos);
		vDelta=m_pSfxObj->m_vPos-m_SfxObj2.m_vPos;
		FLOAT fAngle;
		if(vDelta.z>0) {
			fAngle=(float)(atan(vDelta.x/vDelta.z)*180/3.1415926f)+180;
		}
		else {
			fAngle=(float)(atan(vDelta.x/vDelta.z)*180/3.1415926f);
		}
		m_pSfxObj->m_vRotate.y=m_SfxObj2.m_vRotate.y=fAngle;
		if(D3DXVec3Length(&vDelta)<.2f) {
			m_bHit=TRUE;
		}
	}
	else {
		if(m_SfxObj2.m_nCurFrame==0) {
			// ø©±‚¥Ÿ∞° ∏¬¥¬º¯∞£√≥∏Æ
			DamageToTarget();
		}
		if(m_SfxObj2.Process()) {
			Delete();
		}
	}
	m_nFrame++;
}
void CSfxItemWandAtk4::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	if(m_bHit==FALSE) 
	{
		m_pSfxObj->Render( pd3dDevice );
	}
	else 
	{
		if( !g_Option.m_bSFXRenderOff )
			m_SfxObj2.Render( pd3dDevice );
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Ω∫≈≥ »ø∞˙µÈ

// πÊ∂˚¿⁄ ±‚∫ª Ω∫≈≥ 1
CSfxSkillVagOverCutter::CSfxSkillVagOverCutter()
{
}
CSfxSkillVagOverCutter::~CSfxSkillVagOverCutter()
{
}
void CSfxSkillVagOverCutter::Process()
{
	m_nFrame++;
	{		
		CMover* pObjDst = (CMover*)prj.GetCtrl( m_idDest );
		CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
		if( pObjDst && pObjSrc ) 
		{
			SetPos( pObjDst->GetPos() + D3DXVECTOR3( .0f, 1.0f, .0f) );
			SetAngle( 180.0f - pObjSrc->GetAngle() ); // Ω√¿¸¿⁄¿« πÊ«‚∞˙ ¿œƒ°Ω√≈≤¥Ÿ.
		}
		if( m_pSfxObj->Process() )
			Delete();
	}
}
void CSfxSkillVagOverCutter::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos = GetPos();
	m_pSfxObj->m_vRotate.y = GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}
// πÊ∂˚¿⁄ ±‚∫ª Ω∫≈≥ 2
CSfxSkillVagCleanHit::CSfxSkillVagCleanHit()
{
}
CSfxSkillVagCleanHit::~CSfxSkillVagCleanHit()
{
}
void CSfxSkillVagCleanHit::Process()
{
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if( IsValidObj(pObjSrc) ) 
	{
		SetPos( m_vPosDest );		// ∏Ò«•¡ˆ¡°ø° «◊ªÛ πﬂª˝µ«æÓ ¿÷¿Ω.
		SetAngle( 180.0f - pObjSrc->GetAngle() ); // Ω√¿¸¿⁄¿« πÊ«‚∞˙ ¿œƒ°Ω√≈≤¥Ÿ.
	}
	if( m_pSfxObj->Process() )		// æ÷¥œ∏ﬁ¿Ãº«¿Ã ≥°≥™∏È ≥°.
		Delete();

	m_nFrame++;
	
}
void CSfxSkillVagCleanHit::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate.y=GetAngle();//+180;
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}


// ∞Ì¡§¡¬«• ±‚¡ÿ sfx
CSfxFixed::CSfxFixed()
{
}
CSfxFixed::~CSfxFixed()
{
}
void CSfxFixed::Process()
{
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if( IsValidObj(pObjSrc) ) 
	{
		SetPos( m_vPosDest );		// ∏Ò«•¡ˆ¡°ø° «◊ªÛ πﬂª˝µ«æÓ ¿÷¿Ω.
	}
	if( m_nSec == 0 )	// 0¿∫ 1»∏ «√∑π¿Ã»ƒ ¡æ∑·.
	{
		if( m_pSfxObj->Process() )		// return true¥¬ æ÷¥œ∏ﬁ¿Ãº« ≥°.
			Delete();
	}
	else
	{
		// π›∫π æ÷¥œ∏ﬁ¿Ãº«
		if( m_pSfxObj->Process() )		
			m_pSfxObj->m_nCurFrame = 0;
		if( m_nSec != -1 )		// π´«—π›∫π(-1)¿Ã æ∆¥œ∏È
		{
			if( m_nFrame > ( m_nSec * 60 ) )	// Ω√∞£ √º≈©∏¶ «—¥Ÿ.
				Delete();
		}
	}
	
	m_nFrame++;
}

void CSfxFixed::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos = GetPos();
	m_pSfxObj->m_vRotate.y = GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// πÊ∂˚¿⁄ ±‚∫ª Ω∫≈≥ 3
CSfxSkillVagBrandish::CSfxSkillVagBrandish()
{
}
CSfxSkillVagBrandish::~CSfxSkillVagBrandish()
{
}
void CSfxSkillVagBrandish::Process()
{
	m_nFrame++;
	{ 
		CMover* pObjDst = (CMover*)prj.GetCtrl( m_idDest );
		if( pObjDst ) 
		{
			SetPos( pObjDst->GetPos() + D3DXVECTOR3( .0f, 1.0f, .0f)  );
			SetAngle( 180.0f - pObjDst->GetAngle() );
		}
		if(m_pSfxObj->Process())
			Delete();
	}
}
void CSfxSkillVagBrandish::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate.y=GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// ∏”º≈≥ ∏Æ ±‚∫ª Ω∫≈≥ 1
CSfxSkillMerKeenWheel::CSfxSkillMerKeenWheel()
{
}
CSfxSkillMerKeenWheel::~CSfxSkillMerKeenWheel()
{
}
void CSfxSkillMerKeenWheel::Process()
{
	m_nFrame++;
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxSkillMerKeenWheel::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate.y=GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}
// ∏”º≈≥ ∏Æ ±‚∫ª Ω∫≈≥ 2
CSfxSkillMerSplmash::CSfxSkillMerSplmash()
{
	m_SfxObj2.SetSfx(XI_INDEX( 215, XI_SKILL_MER_ONE_SPLMASH02));
}
CSfxSkillMerSplmash::~CSfxSkillMerSplmash()
{
}
void CSfxSkillMerSplmash::Process()
{
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if(m_nFrame==0 && pObjSrc) 
	{
		SetPos(pObjSrc->GetPos());
		SetAngle(180.0f-pObjSrc->GetAngle());
	}
	m_pSfxObj->Process();
	if( m_nFrame > 85 ) 
	{
		if( m_SfxObj2.Process() )
		{
			Delete();
		}
	}
	m_nFrame++;
}
void CSfxSkillMerSplmash::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate.y=GetAngle();
	m_SfxObj2.m_vPos=GetPos();
	m_SfxObj2.m_vRotate.y=GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_SfxObj2.m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
	if(m_nFrame>85) 
	{
		if( !g_Option.m_bSFXRenderOff )
			m_SfxObj2.Render( pd3dDevice );
	}
}
// ∏”º≈≥ ∏Æ ±‚∫ª Ω∫≈≥ 3
CSfxSkillMerBlindSide::CSfxSkillMerBlindSide()
{
}
CSfxSkillMerBlindSide::~CSfxSkillMerBlindSide()
{
}
void CSfxSkillMerBlindSide::Process()
{
	m_nFrame++;
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxSkillMerBlindSide::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate.y=GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// æÓΩ√ ≥ ≈¨Ω∫≈≥
CSfxSkillAssBurstcrack::CSfxSkillAssBurstcrack()
{
}
CSfxSkillAssBurstcrack::~CSfxSkillAssBurstcrack()
{
}
void CSfxSkillAssBurstcrack::Process()
{
	m_nFrame++;
	if(m_pSfxObj->Process())
	{
		Delete();
	}

	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if( IsValidObj( pObjSrc ) )
	{
		m_pSfxObj->m_vPos = GetPos();
		m_pSfxObj->m_vPos.y += 0.1f;
		m_pSfxObj->m_vRotate.y = 180 - pObjSrc->GetAngle();
		m_pSfxObj->m_vScale    = D3DXVECTOR3( 0.6f, 0.6f, 0.6f );
	}
	else
		Delete();
}
void CSfxSkillAssBurstcrack::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->Render( pd3dDevice );
}

CSfxSkillAssTampinghole::CSfxSkillAssTampinghole()
{
}
CSfxSkillAssTampinghole::~CSfxSkillAssTampinghole()
{
}
void CSfxSkillAssTampinghole::Process()
{
	m_nFrame++;
	if(m_pSfxObj->Process())
	{
		Delete();
	}

	CMover* pObjSrc  = (CMover*)prj.GetCtrl( m_idSrc );
	
	if( IsValidObj( pObjSrc ) )
	{
		m_pSfxObj->m_vPos = GetPos();
		m_pSfxObj->m_vPos.y += 1.0f;
		
		m_pSfxObj->m_vRotate.y = 180 - pObjSrc->GetAngle();
		
		D3DXMATRIX mRot;
		D3DXVECTOR3 vDir = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
		D3DXMatrixRotationY( &mRot, D3DXToRadian( m_pSfxObj->m_vRotate.y ) );
		D3DXVec3TransformCoord( &vDir, &vDir, &mRot );
		
		D3DXVec3Normalize( &vDir, &vDir );
		
		m_pSfxObj->m_vPos += vDir*1.0f;
	}
	else
		Delete();
}
void CSfxSkillAssTampinghole::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->Render( pd3dDevice );
}

CSfxNpcDirSteam::CSfxNpcDirSteam()
{
	m_fInit = TRUE;
}
CSfxNpcDirSteam::~CSfxNpcDirSteam()
{
}
void CSfxNpcDirSteam::Process()
{
	m_nFrame++;
	if(m_pSfxObj->Process())
	{
		Delete();
	}

	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );

	D3DXVECTOR3 vDir = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );

	if( IsInvalidObj(pObjDest) )
	{
		Delete();
		return;
	}
	if( IsValidObj( pObjSrc ) )
	{
		MoverProp* pMoverProp = prj.GetMoverProp( pObjSrc->GetIndex() );

		if( pMoverProp == NULL )
			return;

		switch( pMoverProp->dwClass )
		{
			case RANK_LOW:
			case RANK_NORMAL:
			case RANK_CAPTAIN:
			case RANK_BOSS:
				{
					m_pSfxObj->m_vRotate.y = 180 - pObjSrc->GetAngle();
					
					CModelObject *pModel = (CModelObject *)pObjSrc->m_pModel;
					D3DXVECTOR3 v;
					pModel->GetEventPos( &v, 0 );
					D3DXVec3TransformCoord( &m_pSfxObj->m_vPos, &v, &pObjSrc->GetMatrixWorld() );

					if( pMoverProp->dwClass == RANK_BOSS )
						m_pSfxObj->m_vScale = D3DXVECTOR3( 2.0f, 2.0f, 2.0f );
					else
						m_pSfxObj->m_vScale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
				}
				break;
		}
	}
	else
		Delete();
	
	if( m_fInit )
	{
 		vDir                  = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
		D3DXVECTOR3 vDestNor  = pObjDest->GetPos() - m_pSfxObj->m_vPos;
		D3DXVECTOR3 vAxis;
		D3DXQUATERNION   qRot;
		
		FLOAT       fTheta;
		D3DXVec3Normalize( &vDestNor, &vDestNor );
		D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
		fTheta = D3DXVec3Dot( &vDir, &vDestNor );
		D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );
		
		D3DXVECTOR3 vYPW;
		QuaternionRotationToYPW( qRot, vYPW );
		
		m_pSfxObj->m_vRotate.x = D3DXToDegree(vYPW.x);
		m_pSfxObj->m_vRotate.y = D3DXToDegree(vYPW.y);
		m_pSfxObj->m_vRotate.z = D3DXToDegree(vYPW.z);
		
		if( m_pSfxObj->m_vRotate.x >= 45.0f )
			m_pSfxObj->m_vRotate.x = 45.0f;
		
		if( m_pSfxObj->m_vRotate.z >= 45.0f )
			m_pSfxObj->m_vRotate.z = 45.0f;
		
		m_fInit = FALSE;
	}
}
void CSfxNpcDirSteam::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->Render2( pd3dDevice );
}


// ∏≈¡ˆº« ∫“ ¡÷πÆ ø‹±‚
CSfxSkillMagFireCasting::CSfxSkillMagFireCasting()
{
}
CSfxSkillMagFireCasting::~CSfxSkillMagFireCasting()
{
}
void CSfxSkillMagFireCasting::Process()
{
	m_nFrame++;
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxSkillMagFireCasting::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if( IsValidObj( pObjSrc ) )
	{
		m_pSfxObj->m_vPos = GetPos();
		m_pSfxObj->m_vPos.y += 1.0f;
		m_pSfxObj->m_vRotate.y = 180 - pObjSrc->GetAngle();
		m_pSfxObj->Render( pd3dDevice );
	}
}
// ∏≈¡ˆº« πŸ∂˜ ¡÷πÆ ø‹±‚
CSfxSkillMagWindCasting::CSfxSkillMagWindCasting()
{
}
CSfxSkillMagWindCasting::~CSfxSkillMagWindCasting()
{
}
void CSfxSkillMagWindCasting::Process()
{
	m_nFrame++;
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxSkillMagWindCasting::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vPos.y += 0.2f;
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// ∏≈¡ˆº« ±‚∫ª Ω∫≈≥ 1
CSfxSkillMagStrongWind::CSfxSkillMagStrongWind()
{
	m_SfxObj2.SetSfx(XI_INDEX( 1736, XI_SKILL_MAG_WIND_STRONGWIND01_01));		// ∏¬æ“¿ª∂ß »ø∞˙.
	m_bHit = FALSE;
	m_fAngle = 0;
}
CSfxSkillMagStrongWind::~CSfxSkillMagStrongWind()
{
}
				  
void CSfxSkillMagStrongWind::Process()
{
	if(m_nFrame==0) 
	{
		CMover* pObjSrc = prj.GetMover( m_idSrc );
		if(IsValidObj(pObjSrc)) 
			m_pSfxObj->m_vPos = pObjSrc->GetPos();
	}
	CMover* pObjDest = prj.GetMover( m_idDest );
	if(IsValidObj(pObjDest)) 
		m_SfxObj2.m_vPos = pObjDest->GetPos();

	if( m_bHit == FALSE ) 
	{
		m_pSfxObj->Process();
		if( m_pSfxObj->m_nCurFrame == 60 )
		{
			m_pSfxObj->m_nCurFrame = 30;
		}
		D3DXVECTOR3 vDelta=m_SfxObj2.m_vPos - m_pSfxObj->m_vPos;
		D3DXVec3Normalize( &vDelta, &vDelta );
		m_pSfxObj->m_vPos += vDelta * .2f;
		SetPos( m_pSfxObj->m_vPos );
		vDelta = m_SfxObj2.m_vPos - m_pSfxObj->m_vPos;
		FLOAT fAngle;
		fAngle = atan2( vDelta.x, -vDelta.z );
		fAngle = D3DXToDegree( fAngle );
		m_pSfxObj->m_vRotate.y = m_SfxObj2.m_vRotate.y = fAngle;
		if( D3DXVec3Length(&vDelta) < 0.2f ) 
		{
			PLAYSND( SND_PC_SKILLM_STRONGWIND2, &GetPos() );     
			m_bHit = TRUE;
		} else
			m_fAngle = fAngle;	// elseø°¥Ÿ ≥÷¿∫¿Ã¿Ø¥¬ ≥ π´ ∞°±Ó¿Ã ∫Ÿæ˙¿ª∂ß¿« ∞¢µµ¥¬ ø¿¬˜∞° ¿÷¿ªºˆ ¿÷±‚∂ßπÆø°..
	}
	else {
		if(m_SfxObj2.m_nCurFrame==0)
		{
			float fDmgPower = 0;
			SkillProp *pSkillProp = prj.GetSkillProp( m_dwSkill );
			if( pSkillProp && pSkillProp->dwDmgShift != NULL_ID )
				fDmgPower = (int)pSkillProp->dwDmgShift * 0.2f;		// ¿˚¥Á«—∞™¿∏∑Œ ∫Ø»Ø«ÿº≠ ªÁøÎ.
			int fAngle = (int)( m_fAngle );
			if( IsValidObj(pObjDest) )	// ∫∏Ω∫∏˜¿∫ π–∏Æ¡ˆ æ ¿Ω.
			{
				if( pObjDest->GetProp()->dwClass == RANK_SUPER || pObjDest->GetProp()->dwClass == RANK_MIDBOSS )
				{
					fAngle = 0;		fDmgPower = 0;
				}
			}
			
			DamageToTarget( 0, (float)( fAngle ), fDmgPower );
			// ≈∏∞Ÿø° ∏¬¿∫º¯∞£ ∏¬¿∫ ∞¢µµ∑Œ ø‹∫Œ»˚ 0.x∏¶ ∞°«‘.
			if( IsValidObj(pObjDest) )
			{
				// µ•πÃ¡ˆ«√∂Û¿Ã ¡ﬂø£ 
					if( (pObjDest->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL) == 0 )
						AngleToVectorXZ( &pObjDest->m_pActMover->m_vDeltaE, m_fAngle, fDmgPower );	
			}
		}
		if(m_SfxObj2.Process()) {
			Delete();
		}
	}
	m_nFrame++;
}
void CSfxSkillMagStrongWind::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	if(m_bHit==FALSE) 
	{
		m_pSfxObj->Render( pd3dDevice );
	}
	else 
	{
		if( !g_Option.m_bSFXRenderOff )
			m_SfxObj2.Render( pd3dDevice );
	}
}
// ∏≈¡ˆº« ±‚∫ª Ω∫≈≥ 2
CSfxSkillMagSwordWind::CSfxSkillMagSwordWind()
{
	m_SfxObj2.SetSfx(XI_INDEX( 1737, XI_SKILL_MAG_WIND_SWORDWIND01_01));
	m_bHit=FALSE;
}
CSfxSkillMagSwordWind::~CSfxSkillMagSwordWind()
{
}
void CSfxSkillMagSwordWind::Process()
{
	if(m_nFrame==0) 
	{
		CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
		if(pObjSrc) m_pSfxObj->m_vPos=pObjSrc->GetPos()+D3DXVECTOR3(.0f,0.0f,.0f);
	}
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );
	if(pObjDest) m_SfxObj2.m_vPos=pObjDest->GetPos()+D3DXVECTOR3(.0f,0.0f,.0f);

	if(m_bHit==FALSE) 
	{
		m_pSfxObj->Process();
		if(m_pSfxObj->m_nCurFrame>=30)
		{
			m_pSfxObj->m_nCurFrame=0;
			m_pSfxObj->Process();
		}
		D3DXVECTOR3 vDelta=m_SfxObj2.m_vPos-m_pSfxObj->m_vPos;
		D3DXVec3Normalize(&vDelta,&vDelta);
		m_pSfxObj->m_vPos+=vDelta*.2f;
		SetPos(m_pSfxObj->m_vPos);
		vDelta=m_pSfxObj->m_vPos-m_SfxObj2.m_vPos;

		FLOAT fAngle;
		if(vDelta.z>0) {
			fAngle=(float)(atan(vDelta.x/vDelta.z)*180/3.1415926f)+180;
		}
		else {
			fAngle=(float)(atan(vDelta.x/vDelta.z)*180/3.1415926f);
		}
		m_pSfxObj->m_vRotate.y=m_SfxObj2.m_vRotate.y=fAngle;
		if(D3DXVec3Length(&vDelta)<.4f) 
		{
			PLAYSND( SND_PC_SKILLM_SWORDWIND2, &GetPos() );     
			m_bHit=TRUE;
		}
	}
	else 
	{
		if(m_SfxObj2.m_nCurFrame==0)
			DamageToTarget();
		
		if(m_SfxObj2.Process()) 
			Delete();
	}
	m_nFrame++;
}
void CSfxSkillMagSwordWind::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	if(m_bHit==FALSE) 
	{
		m_pSfxObj->Render( pd3dDevice );
	}
	else 
	{
		if( !g_Option.m_bSFXRenderOff )
			m_SfxObj2.Render( pd3dDevice );
	}
}
// ∏≈¡ˆº« ±‚∫ª Ω∫≈≥ 3
CSfxSkillMagFireBoomerang::CSfxSkillMagFireBoomerang()
{
	m_SfxObj2.SetSfx( XI_INDEX( 1738, XI_SKILL_MAG_FIRE_BOOMERANG01_01 ) );
	m_bHit=FALSE;
}
CSfxSkillMagFireBoomerang::~CSfxSkillMagFireBoomerang()
{
}
void CSfxSkillMagFireBoomerang::Process()
{
	if(m_nFrame==0) 
	{
		CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
		if(pObjSrc) m_pSfxObj->m_vPos=pObjSrc->GetPos()+D3DXVECTOR3(.0f,1.0f,.0f);
	}
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );
	if( pObjDest )
	{
		m_SfxObj2.m_vPos = pObjDest->GetPos() + D3DXVECTOR3(.0f,0.3f,.0f);
		m_SfxObj2.m_vScale = D3DXVECTOR3( 2.0f, 2.0f, 2.0f );
	}
	if(m_bHit==FALSE) 
	{
		m_pSfxObj->Process();
		if( m_pSfxObj->m_nCurFrame == 20 )
		{
			m_pSfxObj->m_nCurFrame = 0;
		}
		D3DXVECTOR3 vDelta = m_SfxObj2.m_vPos - m_pSfxObj->m_vPos;
		D3DXVec3Normalize(&vDelta,&vDelta);
		m_pSfxObj->m_vPos+=vDelta*.2f;
		SetPos(m_pSfxObj->m_vPos);
		vDelta=m_pSfxObj->m_vPos - m_SfxObj2.m_vPos;


		D3DXVECTOR3 vDir      = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
		D3DXVECTOR3 vDestNor  = m_SfxObj2.m_vPos-m_pSfxObj->m_vPos;
		D3DXVECTOR3 vAxis;
		D3DXQUATERNION   qRot;
		
		FLOAT       fTheta;
		D3DXVec3Normalize( &vDestNor, &vDestNor );
		D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
		fTheta = D3DXVec3Dot( &vDir, &vDestNor );
		D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );
		
		D3DXVECTOR3 vYPW;
		
		QuaternionRotationToYPW( qRot, vYPW );
		m_pSfxObj->m_vRotate.x=m_SfxObj2.m_vRotate.x=D3DXToDegree(vYPW.x);
		m_pSfxObj->m_vRotate.y=m_SfxObj2.m_vRotate.y=D3DXToDegree(vYPW.y);
		m_pSfxObj->m_vRotate.z=m_SfxObj2.m_vRotate.z=D3DXToDegree(vYPW.z);

		if( D3DXVec3Length( &vDelta ) < .2f ) 
		{
			PLAYSND( SND_PC_SKILLM_FIREBOOMERANG2, &GetPos() );
			m_bHit=TRUE;
		}
	}
	else 
	{
		if(m_SfxObj2.m_nCurFrame==0)
			DamageToTarget();
		if(m_SfxObj2.Process()) {
			Delete();
		}
	}
	m_nFrame++;
}
void CSfxSkillMagFireBoomerang::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	if(m_bHit==FALSE) 
	{
		m_pSfxObj->Render2( pd3dDevice );
	}
	else 
	{
		if( !g_Option.m_bSFXRenderOff )
			m_SfxObj2.Render( pd3dDevice );
	}
}
// ∏≈¡ˆº« ±‚∫ª Ω∫≈≥ 4
CSfxSkillMagFireBomb::CSfxSkillMagFireBomb()
{
}
CSfxSkillMagFireBomb::~CSfxSkillMagFireBomb()
{
}
void CSfxSkillMagFireBomb::Process()
{
	m_nFrame++;
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxSkillMagFireBomb::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate.y=GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}
// ∏≈¡ˆº« ±‚∫ª Ω∫≈≥ 5
CSfxSkillMagHotAir::CSfxSkillMagHotAir()
{
	m_SfxObj2.SetSfx(XI_INDEX( 1739, XI_SKILL_MAG_FIRE_HOTAIR01_01 ));		// ≈∏∞Ÿ√¯ø°º≠ ∆„ ≈Õ¡ˆ∏Á »∏¿¸«œ¥¬ »ø∞˙
	m_nDmgCnt = 0;
}
CSfxSkillMagHotAir::~CSfxSkillMagHotAir()
{
}
void CSfxSkillMagHotAir::Process()
{
	CMover* pObjSrc = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pObjSrc ) )
	{
		Delete();
		return;
	}
	{
		m_pSfxObj->m_vPos = pObjSrc->GetPos();		// πﬂªÁ√¯ø°º≠ ≈∏∞Ÿ√¯¿∏∑Œ ≥™≈∏≥™¥¬ »ø∞˙
		m_pSfxObj->m_vRotate = D3DXVECTOR3(.0f,-pObjSrc->GetAngle(),.0f);
	}
	BOOL res1 = m_pSfxObj->Process();
	CMover* pObjDest = prj.GetMover( m_idDest );
	if( IsInvalidObj( pObjDest ) )
	{
		Delete();
		return;
	}
#ifdef __CLIENT
//sun, 11, »Æ¿≤Ω∫≈≥ »ø∞˙ºˆ¡§ world,neuz
	else
	{
		DWORD dwTmpID =  pObjDest->GetRemoveSfxObj(XI_INDEX( 265, XI_SKILL_MAG_FIRE_HOTAIR01 ) );
		if( dwTmpID && m_nFrame > 0)
		{
			Delete();
			return;
		}
	}
#endif	// __CLIENT

	m_SfxObj2.m_vPos = pObjDest->GetPos();

	SkillProp *pSkillProp = prj.GetSkillProp( m_dwSkill );
	if( pSkillProp == NULL )		// Ω«∆–«œ∏È ∞¡∏Æ≈œ.
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Ω∫≈≥(%d)¿« «¡∑Œ∆€∆º∞° æ¯¥Ÿ." ), m_dwSkill );
		Delete();
		return;
	}
	AddSkillProp *pAddSkillProp = prj.GetAddSkillProp( pSkillProp->dwSubDefine, m_nMagicPower );	// MagicPower∞° ∑π∫ß 
	if( pAddSkillProp == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Ω∫≈≥(%d)¿« æ÷µÂ «¡∑Œ∆€∆º∞° æ¯¥Ÿ." ), m_dwSkill );
		Delete();
		return;
	}
	
	BOOL bHitFrame = FALSE;
	int	nPainFrame = (int)((pAddSkillProp->dwPainTime / 1000.0f) * SEC1);
	if( (m_nFrame % nPainFrame) == 0 )	// nPainFrame ∏∂¥Ÿ «—π¯æø µ•πÃ¡ˆ ¡‹.
	{
		int	nMaxDmgCnt = (pAddSkillProp->dwSkillTime / pAddSkillProp->dwPainTime) + 1;		// ∏Óπ¯ µ•πÃ¡ˆ∏¶ ∏‘≥ƒ.
		// À¨	2006/11/9	// √ππ¯¬∞∏∏ ¿¸º€, ¿Ã »ƒ ¡ˆº” «««ÿ √≥∏Æ
//		DamageToTarget( m_nDmgCnt++, 0, 0, nMaxDmgCnt );
		if( m_nDmgCnt++ == 0 )
			DamageToTarget( m_nDmgCnt, 0, 0, nMaxDmgCnt );
		if( m_nDmgCnt >= nMaxDmgCnt )
			m_idSfxHit = 0;
		PLAYSND( SND_PC_SKILLM_HOTAIR2, &GetPos() );
	}
	BOOL res2 = m_SfxObj2.Process();
	if( res2 )
		m_SfxObj2.m_nCurFrame = 0;
	int nFrameMax = (int)((pAddSkillProp->dwSkillTime / 1000.0f) * SEC1);	// √÷¥Î ¡ˆº”Ω√∞£(«¡∑π¿”¥‹¿ß).
	if( m_nFrame >= nFrameMax )
	{
		Delete();
	}
	m_nFrame++;
}
void CSfxSkillMagHotAir::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->Render( pd3dDevice );
	m_pSfxObj->m_vScale = GetScale();
	if( !g_Option.m_bSFXRenderOff )
		m_SfxObj2.Render( pd3dDevice );
}



////////////////////////////////////////////////////////////////
void CSfxDuelParty::Process()
{
	CSfx::Process();
	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )	// process¡ﬂ src π´πˆ∞° æ¯æÓ¡≥¿∏∏È ¿⁄µø ªË¡¶
	{
		Delete();
		return;
	}

	switch( m_nType )
	{
	case ENEMY_PVP_PARTY:
		{
			// process¡ﬂ srcπ´πˆ¿« ±ÿ¥‹µ‡æÛ¿Ã ≥°≥µ¿∏∏È ªË¡¶.
			if( pMover->m_nDuel != 2 || ( pMover->m_nDuel == 2 && ( pMover->m_nDuelState == 300 || pMover->m_nDuelState == 0 ) ) )		
			{
				DeleteSfx( pMover );
			}
			break;
		}
	case ENEMY_WAR:
		{
			if( pMover->m_idWar == 0 )		// process¡ﬂ srcπ´πˆ¿« ¿¸¿Ô¿Ã ≥°≥µ¿∏∏È «ÿ¡¶.
			{
				DeleteSfx( pMover );
			}
			break;
		}
	case ENEMY_PK:		
		{
			if( !( g_Neuz.m_NeuzEnemy.IsPKing( pMover->GetId() ) ) )
			{
				DeleteSfx( pMover );
			}
			break;
		}
	case ENEMY_PVP:
		{
			if( pMover->m_nDuel != 1 )
			{
				DeleteSfx( pMover );
			}
			break;
		}
	}
}

void CSfxDuelParty::DeleteSfx( CMover* pEnemy )
{
	pEnemy->m_dwFlag &= (~MVRF_WARMARK);
	Delete();
}

void	CSfxDuelParty::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )	// process¡ﬂ src π´πˆ∞° æ¯æÓ¡≥¿∏∏È ¿⁄µø ªË¡¶
		return;
			
	if( pMover->HasBuff( BUFF_SKILL, SKILL_INDEX( 193, SI_ACR_SUP_DARKILLUSION ) ) || pMover->IsMode( TRANSPARENT_MODE ) )
		return;

	CSfx::Render(pd3dDevice);
}



////////////////////////////////////////////////////////////////
CSfxSetItem::~CSfxSetItem() 
{
	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsValidObj( pMover ) )
	{
		pMover->m_dwFlag &= (~MVRF_SETITEMSFX);
	}
}

void CSfxSetItem::Process()
{
	CSfx::Process();
	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )	// process¡ﬂ src π´πˆ∞° æ¯æÓ¡≥¿∏∏È ¿⁄µø ªË¡¶
	{
		Delete();
		return;
	}

	int nAbilityOption	= pMover->GetSetItemClient();
	if( nAbilityOption < 3 )
	{
		pMover->m_dwFlag &= (~MVRF_SETITEMSFX);
		Delete();
		return;
	}
}

void	CSfxSetItem::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )	// process¡ﬂ src π´πˆ∞° æ¯æÓ¡≥¿∏∏È ¿⁄µø ªË¡¶
		return;
	
	if( ((pMover->IsMode( TRANSPARENT_MODE ) ) == 0) )
		CSfx::Render(pd3dDevice);
}

////////////////////////////////////////////////////////////////
void CSfxCollect::Process()
{
	m_nFrame++;

	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )	// process¡ﬂ src π´πˆ∞° æ¯æÓ¡≥¿∏∏È ¿⁄µø ªË¡¶
	{
		Delete();
		return;
	}
	
	D3DXVECTOR3 vPos;
	((CModelObject *)pMover->m_pModel)->GetForcePos( &vPos, 0, PARTS_RWEAPON, pMover->GetMatrixWorld() );
	
	SetPos( vPos );

	if( m_pSfxObj->Process() )
		m_pSfxObj->m_nCurFrame = 0;


	if( m_pSfxObj->m_nCurFrame >= 145 ) // ±∏∞£ π›∫π.
		m_pSfxObj->m_nCurFrame = 30; // ∏Ò«•¡°ø° ∏¬¿ª∂ß±Ó¡ˆ ∑Á«¡Ω√≈∞±‚ ¿ß«ÿ ¥ŸΩ√ 0«¡∑π¿”¿∏∑Œ µπ∑¡≥ı¥¬¥Ÿ.
	
	// «√∑π¿ÃæÓ¿« ªÛ≈¬∞° √§¡˝¿Ã ≥°≥µ¿∏∏È 
	if( pMover->m_pActMover->GetActionState() != OBJSTA_COLLECT )
	{
		pMover->m_dwFlag &= (~MVRF_COLLECT);		// SFX«ÿ¡¶.
		Delete();		// sfx ªË¡¶.
	}
}

void CSfxCollect::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !IsVisible() || ( IsCull() && GetType() != 1 ))
		return;
	if( IsUpdateMatrix() )
		UpdateMatrix();
	
	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( m_dwType, m_dwIndex );
	// Model¿Ã æ¯¿∏∏È √‚∑¬«œ¡ˆ æ ¿Ω 
	if( lpModelElem == NULL )
		return;
	
	m_pSfxObj->m_vPos = GetPos();
	m_pSfxObj->m_vRotate.y = GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->m_matScale = m_matScale;
	m_pSfxObj->Render( pd3dDevice, NULL );
}


////////////////////////////////////////////////////////////////
void CSfxMushmootCharge::Process()
{
	m_nFrame++;
	
	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )	// process¡ﬂ src π´πˆ∞° æ¯æÓ¡≥¿∏∏È ¿⁄µø ªË¡¶
	{
		Delete();
		return;
	}
	if( pMover->m_dwFlag & MVRF_HITCONT )		// ∫∏Ω∫∞° ∑π¿Ã¿˙∏¶ ΩÓ±‚ Ω√¿€«œ∏È ªË¡¶«‘.
	{
		Delete();
		return;
	}
	
	D3DXVECTOR3 vPos;
	
	((CModelObject *)pMover->m_pModel)->GetEventPos( &vPos, m_nEventPos );			// ∏∂∫˝ ¡¬«•.
	D3DXVec3TransformCoord( &vPos, &vPos, pMover->GetMatrixWorldPtr() );	// ø˘µÂ∞¯∞£¿∏∑Œ ∫Ø»Ø.
	
	SetPos( vPos );
	SetAngle( -pMover->GetAngle() );
	
	if( m_pSfxObj->Process() )
		m_pSfxObj->m_nCurFrame = 0;
}

void CSfxMushmootCharge::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !IsVisible() || ( IsCull() && GetType() != 1 ))
		return;
	if( IsUpdateMatrix() )
		UpdateMatrix();
	
	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( m_dwType, m_dwIndex );
	// Model¿Ã æ¯¿∏∏È √‚∑¬«œ¡ˆ æ ¿Ω 
	if( lpModelElem == NULL )
		return;
	
	m_pSfxObj->m_vPos = GetPos();
	m_pSfxObj->m_vRotate.y = GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->m_matScale = m_matScale;
	m_pSfxObj->Render( pd3dDevice, NULL );
}

////////////////////////////////////////////////////////////////
void CSfxClockWorksCharge::Process()
{
	m_nFrame++;
	
	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )	// process¡ﬂ src π´πˆ∞° æ¯æÓ¡≥¿∏∏È ¿⁄µø ªË¡¶
	{
		Delete();
		return;
	}
	if( pMover->m_dwFlag & MVRF_HITCONT )		// ∫∏Ω∫∞° ∑π¿Ã¿˙∏¶ ΩÓ±‚ Ω√¿€«œ∏È ªË¡¶«‘.
	{
		Delete();
		return;
	}
	
	D3DXVECTOR3 vPos;

	((CModelObject *)pMover->m_pModel)->GetEventPos( &vPos, 2 );			// ∏∂∫˝ ¡¬«•.
	D3DXVec3TransformCoord( &vPos, &vPos, pMover->GetMatrixWorldPtr() );	// ø˘µÂ∞¯∞£¿∏∑Œ ∫Ø»Ø.
	
	SetPos( vPos );
	SetAngle( -pMover->GetAngle() );
	
	if( m_pSfxObj->Process() )
		m_pSfxObj->m_nCurFrame = 0;
}

void CSfxClockWorksCharge::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !IsVisible() || ( IsCull() && GetType() != 1 ))
		return;
	if( IsUpdateMatrix() )
		UpdateMatrix();
	
	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( m_dwType, m_dwIndex );
	// Model¿Ã æ¯¿∏∏È √‚∑¬«œ¡ˆ æ ¿Ω 
	if( lpModelElem == NULL )
		return;
	
	m_pSfxObj->m_vPos = GetPos();
	m_pSfxObj->m_vRotate.y = GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->m_matScale = m_matScale;
	m_pSfxObj->Render( pd3dDevice, NULL );
}

////////////////////////////////////////////////////////////////
// ≈¨∂Ùøˆ≈© ∫∏Ω∫ øﬁ∆»ø°º≠ πﬂªÁ«“∂ß ≥™ø¿¥¬ ¿Ã∆Â∆Æ.
void CSfxClockWorksCannon::Process()
{
	m_nFrame++;
	
	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )	// process¡ﬂ src π´πˆ∞° æ¯æÓ¡≥¿∏∏È ¿⁄µø ªË¡¶
	{
		Delete();
		return;
	}
	
	D3DXVECTOR3 vPos;
	CModelObject *pModel = (CModelObject *)pMover->m_pModel;
	
	pModel->GetEventPos( &vPos, 1 );			// øﬁº’ ¡¬«•.
	D3DXVec3TransformCoord( &vPos, &vPos, pMover->GetMatrixWorldPtr() );	// ø˘µÂ∞¯∞£¿∏∑Œ ∫Ø»Ø.
	
	D3DXMATRIX mLArm = *(pModel->GetMatrixBone( 9 ));		// 9∞° L ForeArm
	D3DXMatrixMultiply( &mLArm, pMover->GetMatrixWorldPtr(), &mLArm );
	D3DXQUATERNION qLArm;
	D3DXVECTOR3 vYPW;
	
	D3DXQuaternionRotationMatrix( &qLArm, &mLArm );			// øﬁ∆» ∏≈∆Æ∏ØΩ∫∏¶ ƒı≈Õ¥œæ¿∏∑Œ πŸ≤ﬁ.
	QuaternionRotationToYPW( qLArm, vYPW );
	vYPW.y += D3DXToRadian( -90.0f );
	m_pSfxObj->m_vRotate.x = D3DXToDegree(vYPW.x);
	m_pSfxObj->m_vRotate.y = D3DXToDegree(vYPW.y);
	m_pSfxObj->m_vRotate.z = D3DXToDegree(vYPW.z);

	SetPos( vPos );
	
	if( m_pSfxObj->Process() )
		Delete();
}

void CSfxClockWorksCannon::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !IsVisible() || ( IsCull() && GetType() != 1 ))
		return;
	if( IsUpdateMatrix() )
		UpdateMatrix();
	
	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( m_dwType, m_dwIndex );
	// Model¿Ã æ¯¿∏∏È √‚∑¬«œ¡ˆ æ ¿Ω 
	if( lpModelElem == NULL )
		return;
	
	m_pSfxObj->m_vPos = GetPos();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->m_matScale = m_matScale;
	m_pSfxObj->Render2( pd3dDevice, NULL );
}


///////////////////////////////////////////////////////////////////




// ∏≈¡ˆº« Ice Missile
CSfxSkillMagIceMissile::CSfxSkillMagIceMissile()
{
	m_SfxObj2.SetSfx( XI_INDEX( 1740, XI_SKILL_MAG_WATER_ICEMISSILE01_01 ) );
	m_bHit=FALSE;
}
CSfxSkillMagIceMissile::~CSfxSkillMagIceMissile()
{
}
void CSfxSkillMagIceMissile::Process()
{
	if(m_nFrame==0) 
	{
		CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
		if(pObjSrc) m_pSfxObj->m_vPos=pObjSrc->GetPos()+D3DXVECTOR3(.0f,1.0f,.0f);
	}
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );
	if( pObjDest )
	{
		m_SfxObj2.m_vPos = pObjDest->GetPos() + D3DXVECTOR3(.0f,0.3f,.0f);
		m_SfxObj2.m_vScale = D3DXVECTOR3( 2.0f, 2.0f, 2.0f );
	}
	if(m_bHit==FALSE) 
	{
		if( m_pSfxObj->Process() )
		{
			m_pSfxObj->m_nCurFrame = 0;
		}
		D3DXVECTOR3 vDelta = m_SfxObj2.m_vPos - m_pSfxObj->m_vPos;
		D3DXVec3Normalize(&vDelta,&vDelta);
		m_pSfxObj->m_vPos+=vDelta*.2f;
		SetPos(m_pSfxObj->m_vPos);
		vDelta=m_pSfxObj->m_vPos - m_SfxObj2.m_vPos;
		
	    // ∏∂π˝∞¥√º πÊ«‚∫§≈Õ∑Œ ∏µ® πÊ«‚ µπ∏Æ±‚~~~
		D3DXVECTOR3 vDir      = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
		D3DXVECTOR3 vDestNor  = m_SfxObj2.m_vPos-m_pSfxObj->m_vPos;
		D3DXVECTOR3 vAxis;
		D3DXQUATERNION   qRot;
		
		FLOAT       fTheta;
		D3DXVec3Normalize( &vDestNor, &vDestNor );
		D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
		fTheta = D3DXVec3Dot( &vDir, &vDestNor );
		D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );
		
		D3DXVECTOR3 vYPW;
		
		QuaternionRotationToYPW( qRot, vYPW );
		m_pSfxObj->m_vRotate.x=m_SfxObj2.m_vRotate.x=D3DXToDegree(vYPW.x);
		m_pSfxObj->m_vRotate.y=m_SfxObj2.m_vRotate.y=D3DXToDegree(vYPW.y);
		m_pSfxObj->m_vRotate.z=m_SfxObj2.m_vRotate.z=D3DXToDegree(vYPW.z);

		if( D3DXVec3Length( &vDelta ) < .2f ) 
		{
			PLAYSND( SND_PC_SKILLM_FIREBOOMERANG2, &GetPos() );
			m_bHit=TRUE;
		}
	}
	else 
	{
		if(m_SfxObj2.m_nCurFrame==0)
			DamageToTarget();
		if(m_SfxObj2.Process()) {
			Delete();
		}
	}
	m_nFrame++;
}
void CSfxSkillMagIceMissile::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	if(m_bHit==FALSE) 
	{
		m_pSfxObj->Render2( pd3dDevice );
	}
	else 
	{
		if( !g_Option.m_bSFXRenderOff )
			m_SfxObj2.Render( pd3dDevice );
	}
}


// ∏≈¡ˆº« Lightning Ball
CSfxSkillMagLightningBall::CSfxSkillMagLightningBall()
{
	m_SfxObj2.SetSfx( XI_INDEX( 1741, XI_SKILL_MAG_ELECTRICITY_LIGHTINGBALL01_01 ) );
	m_bHit=FALSE;
}
CSfxSkillMagLightningBall::~CSfxSkillMagLightningBall()
{
}
void CSfxSkillMagLightningBall::Process()
{
	if(m_nFrame==0) 
	{
		CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
		if(pObjSrc) m_pSfxObj->m_vPos=pObjSrc->GetPos()+D3DXVECTOR3(.0f,1.0f,.0f);
	}
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );
	if( pObjDest )
	{
		m_SfxObj2.m_vPos = pObjDest->GetPos() + D3DXVECTOR3(.0f,0.3f,.0f);
		m_SfxObj2.m_vScale = D3DXVECTOR3( 2.0f, 2.0f, 2.0f );
	}
	if(m_bHit==FALSE) 
	{
		if( m_pSfxObj->Process() )
		{
			m_pSfxObj->m_nCurFrame = 0;
		}
		D3DXVECTOR3 vDelta = m_SfxObj2.m_vPos - m_pSfxObj->m_vPos;
		D3DXVec3Normalize(&vDelta,&vDelta);
		m_pSfxObj->m_vPos+=vDelta*.2f;
		SetPos(m_pSfxObj->m_vPos);
		vDelta=m_pSfxObj->m_vPos - m_SfxObj2.m_vPos;
		
		// ∏∂π˝∞¥√º πÊ«‚∫§≈Õ∑Œ ∏µ® πÊ«‚ µπ∏Æ±‚~~~
		D3DXVECTOR3 vDir      = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
		D3DXVECTOR3 vDestNor  = m_SfxObj2.m_vPos-m_pSfxObj->m_vPos;
		D3DXVECTOR3 vAxis;
		D3DXQUATERNION   qRot;
		
		FLOAT       fTheta;
		D3DXVec3Normalize( &vDestNor, &vDestNor );
		D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
		fTheta = D3DXVec3Dot( &vDir, &vDestNor );
		D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );
		
		D3DXVECTOR3 vYPW;
		
		QuaternionRotationToYPW( qRot, vYPW );
		m_pSfxObj->m_vRotate.x=m_SfxObj2.m_vRotate.x=D3DXToDegree(vYPW.x);
		m_pSfxObj->m_vRotate.y=m_SfxObj2.m_vRotate.y=D3DXToDegree(vYPW.y);
		m_pSfxObj->m_vRotate.z=m_SfxObj2.m_vRotate.z=D3DXToDegree(vYPW.z);
		
		if( D3DXVec3Length( &vDelta ) < .2f ) 
		{
			PLAYSND( SND_PC_SKILLM_FIREBOOMERANG2, &GetPos() );
			m_bHit=TRUE;
		}
	}
	else 
	{
		if(m_SfxObj2.m_nCurFrame==0)
			DamageToTarget();
		if(m_SfxObj2.Process()) {
			Delete();
		}
	}
	m_nFrame++;
}
void CSfxSkillMagLightningBall::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	if(m_bHit==FALSE) 
	{
		m_pSfxObj->Render2( pd3dDevice );
	}
	else 
	{
		if( !g_Option.m_bSFXRenderOff )
			m_SfxObj2.Render( pd3dDevice );
	}
}

// ∏≈¡ˆº« Spike Stone
CSfxSkillMagSpikeStone::CSfxSkillMagSpikeStone()
{
	m_SfxObj2.SetSfx( XI_INDEX( 1742, XI_SKILL_MAG_EARTH_SPIKESTONE01_01 ) );
	m_bHit=FALSE;
}
CSfxSkillMagSpikeStone::~CSfxSkillMagSpikeStone()
{
}
void CSfxSkillMagSpikeStone::Process()
{
	if(m_nFrame==0) 
	{
		CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
		if(pObjSrc) m_pSfxObj->m_vPos=pObjSrc->GetPos()+D3DXVECTOR3(.0f,1.0f,.0f);
	}
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );
	if( pObjDest )
	{
		m_SfxObj2.m_vPos = pObjDest->GetPos() + D3DXVECTOR3(.0f,0.3f,.0f);
		m_SfxObj2.m_vScale = D3DXVECTOR3( 2.0f, 2.0f, 2.0f );
	}
	if(m_bHit==FALSE) 
	{
		if( m_pSfxObj->Process() )
		{
			m_pSfxObj->m_nCurFrame = 0;
		}
		D3DXVECTOR3 vDelta = m_SfxObj2.m_vPos - m_pSfxObj->m_vPos;
		D3DXVec3Normalize(&vDelta,&vDelta);
		m_pSfxObj->m_vPos+=vDelta*.2f;
		SetPos(m_pSfxObj->m_vPos);
		vDelta=m_pSfxObj->m_vPos - m_SfxObj2.m_vPos;
		
		// ∏∂π˝∞¥√º πÊ«‚∫§≈Õ∑Œ ∏µ® πÊ«‚ µπ∏Æ±‚~~~
		D3DXVECTOR3 vDir      = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
		D3DXVECTOR3 vDestNor  = m_SfxObj2.m_vPos-m_pSfxObj->m_vPos;
		D3DXVECTOR3 vAxis;
		D3DXQUATERNION   qRot;
		
		FLOAT       fTheta;
		D3DXVec3Normalize( &vDestNor, &vDestNor );
		D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
		fTheta = D3DXVec3Dot( &vDir, &vDestNor );
		D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );
		
		D3DXVECTOR3 vYPW;
		
		QuaternionRotationToYPW( qRot, vYPW );
		m_pSfxObj->m_vRotate.x=m_SfxObj2.m_vRotate.x=D3DXToDegree(vYPW.x);
		m_pSfxObj->m_vRotate.y=m_SfxObj2.m_vRotate.y=D3DXToDegree(vYPW.y);
		m_pSfxObj->m_vRotate.z=m_SfxObj2.m_vRotate.z=D3DXToDegree(vYPW.z);
		
		if( D3DXVec3Length( &vDelta ) < .2f ) 
		{
			PLAYSND( SND_PC_SKILLM_FIREBOOMERANG2, &GetPos() );
			m_bHit=TRUE;
		}
	}
	else 
	{
		if(m_SfxObj2.m_nCurFrame==0)
			DamageToTarget();
		if(m_SfxObj2.Process()) {
			Delete();
		}
	}
	m_nFrame++;
}
void CSfxSkillMagSpikeStone::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	if(m_bHit==FALSE) 
	{
		m_pSfxObj->Render2( pd3dDevice );
	}
	else 
	{
		if( !g_Option.m_bSFXRenderOff )
			m_SfxObj2.Render( pd3dDevice );
	}
}

////////////////////////////////////////////////////////////////
void CSfxRotate::Process()
{
	m_nFrame++;
	
	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )	// process¡ﬂ src π´πˆ∞° æ¯æÓ¡≥¿∏∏È ¿⁄µø ªË¡¶
	{
		Delete();
		return;
	}
	
	D3DXVECTOR3 vPos;
	CModelObject *pModel = (CModelObject *)pMover->m_pModel;
	pModel->GetEventPos( &vPos, 0 );	// µÓµÂ∏Æ ¡¬«•
	
	D3DXVec3TransformCoord( &vPos, &vPos, pMover->GetMatrixWorldPtr() );

	SetPos( vPos );
	SetAngle( -pMover->GetAngle() );
	
	if( m_pSfxObj->Process() )
		m_pSfxObj->m_nCurFrame = 0;
	
	if( m_nSec != -1 )		// π´«—π›∫π(-1)¿Ã æ∆¥œ∏È
	{
		if( m_nFrame > ( m_nSec * 60 ) )
			Delete();
	}
}

void CSfxRotate::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !IsVisible() || ( IsCull() && GetType() != 1 ))
		return;
	if( IsUpdateMatrix() )
		UpdateMatrix();
	
	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( m_dwType, m_dwIndex );
	// Model¿Ã æ¯¿∏∏È √‚∑¬«œ¡ˆ æ ¿Ω 
	if( lpModelElem == NULL )
		return;
	
	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )
		return;

	D3DXVECTOR3 vPos;
	CModelObject *pModel = (CModelObject *)pMover->m_pModel;
	pModel->GetEventPos( &vPos, 0 );	// µÓµÂ∏Æ ¡¬«•
	
	D3DXVec3TransformCoord( &vPos, &vPos, pMover->GetMatrixWorldPtr() );
	
	SetPos( vPos );
	
	m_pSfxObj->m_vPos = GetPos();
	m_pSfxObj->m_vRotate.y = GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->m_matScale = m_matScale;
	m_pSfxObj->Render( pd3dDevice, NULL );
}


// ≥Øæ∆∞°º≠ ∏¬∞Ì ≈Õ¡ˆ¥¬ ¿œπ›¿˚¿Œ ¿Ã∆Â∆Æ.
CSfxShoot::CSfxShoot()
{
	m_bHit = FALSE;
	m_dwSndHit = NULL_ID;
	m_bDir = FALSE;
	m_fHitScale = 1.0f;
	m_dwSfxHit = NULL_ID;
}
CSfxShoot::~CSfxShoot()
{
	m_bHit = FALSE;
	m_dwSndHit = NULL_ID;
	m_bDir = FALSE;
	m_fHitScale = 1.0f;
	m_dwSfxHit = NULL_ID;
}

// ∏¬æ“¿ª∂ß ¿Ã∆Â∆Æ µÓ∑œ
void CSfxShoot::SetHitSfx( LPCTSTR szFileName, OBJID idSrc, OBJID idDst, FLOAT fScale ) 
{ 
}
void CSfxShoot::SetHitSfx( DWORD dwIndex, OBJID idSrc, OBJID idDst, FLOAT fScale ) 
{ 
}


// πÊ«‚º∫ ¿÷¥¬ sfx ¿Œ∞°.
void CSfxShoot::SetDir( BOOL bDir )
{
	m_bDir = bDir;
}



void CSfxShoot::Process()
{
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );	// ¿Øµµ≈∫ «¸¿Ã±‚∂ßπÆø° ≈∏∞Ÿ¿∫ «◊ªÛ ¿–æÓ∫¡æﬂ «—¥Ÿ.
	if( IsValidObj(pObjDest) )
	{
		m_vPosDest = pObjDest->GetPos() + D3DXVECTOR3( 0, 1.0f, 0 );
	}

	if( m_bHit == FALSE ) 
	{
		if( m_pSfxObj->Process() )		// æ÷¥œ∏ﬁ¿Ãº«¿∫ ∑Á«¡.
		{
			m_pSfxObj->m_nCurFrame = 0;
		}
		D3DXVECTOR3 vDelta = m_vPosDest - m_pSfxObj->m_vPos;
		D3DXVec3Normalize( &vDelta, &vDelta );		// ≈∏∞Ÿ¬  ¥‹¿ß∫§≈Õ ±∏«ÿº≠
		D3DXVECTOR3 vPos = GetPos();
		vPos += vDelta * 0.2f;			// ±◊ ∫§≈Õ∑Œ º”µµ∏¶ ∞·¡§«ÿº≠ ¡¬«• ¿Ãµø.
		m_pSfxObj->m_vPos = vPos;
		SetPos( vPos );
		
		if( m_bDir )
		{
			// ∏∂π˝∞¥√º πÊ«‚∫§≈Õ∑Œ ∏µ® πÊ«‚ µπ∏Æ±‚~~~
			D3DXVECTOR3 vDir      = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
			D3DXVECTOR3 vDestNor  = m_vPosDest - m_pSfxObj->m_vPos;
			D3DXVECTOR3 vAxis;
			D3DXQUATERNION   qRot;
			
			FLOAT       fTheta;
			D3DXVec3Normalize( &vDestNor, &vDestNor );
			D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
			fTheta = D3DXVec3Dot( &vDir, &vDestNor );
			D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );
			
			D3DXVECTOR3 vYPW;
			
			QuaternionRotationToYPW( qRot, vYPW );
			m_pSfxObj->m_vRotate.x = /*m_SfxObj2.m_vRotate.x = */D3DXToDegree(vYPW.x);
			m_pSfxObj->m_vRotate.y = /*m_SfxObj2.m_vRotate.y = */D3DXToDegree(vYPW.y);
			m_pSfxObj->m_vRotate.z = /*m_SfxObj2.m_vRotate.z = */D3DXToDegree(vYPW.z);
		}
		
		vDelta = m_pSfxObj->m_vPos - m_vPosDest;		// ¥ÎªÛ∞˙¿« ∞≈∏Æ
		if( D3DXVec3LengthSq( &vDelta ) < 0.2f * 0.2f )		// ∆¯πﬂ ∞≈∏Æ±Ó¡ˆ ¥Ÿ∞°∞°∏È √Êµπ.
		{
			if( m_dwSndHit != NULL_ID )
				PLAYSND( m_dwSndHit, &GetPos() );		// ªÁøÓµÂ «√∑π¿Ã.
			DamageToTarget();
			// ∆¯πﬂ ø¿∫Í¡ß∆Æ ª˝º∫.
			if( m_dwSfxHit != NULL_ID )
			{
				CSfx *pSfx = CreateSfx( D3DDEVICE, m_dwSfxHit, GetPos(), m_idSrc, m_vPosDest, m_idDest, 0 );
				if( pSfx )
				{
					pSfx->SetAngle( m_pSfxObj->m_vRotate.y );
				}
			}
			Delete();	// this¥¬ ªË¡¶.
		}
	}
	m_nFrame ++;
}
void CSfxShoot::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render2( pd3dDevice );
}


// ∂•πŸ¥⁄ø° ∫ŸæÓº≠ ≥Øæ∆∞°¥¬ ¿Ã∆Â∆Æ.
CSfxShootWave::CSfxShootWave()
{
	m_bHit = FALSE;
	m_dwSndHit = NULL_ID;
	m_bDir = FALSE;
	m_fHitScale = 1.0f;
	memset( m_vTail, 0, sizeof(m_vTail) );
	memset( m_nTailFrame, 0, sizeof(m_nTailFrame) );
}
CSfxShootWave::~CSfxShootWave()
{
	m_bHit = FALSE;
	m_dwSndHit = NULL_ID;
	m_bDir = FALSE;
	m_fHitScale = 1.0f;
	memset( m_vTail, 0, sizeof(m_vTail) );
	memset( m_nTailFrame, 0, sizeof(m_nTailFrame) );
}

// ∏¬æ“¿ª∂ß ¿Ã∆Â∆Æ µÓ∑œ
void CSfxShootWave::SetHitSfx( LPCTSTR szFileName, OBJID idSrc, OBJID idDst, FLOAT fScale ) 
{ 
	m_SfxObj2.SetSfx( szFileName );		// ∏¬∞Ì ≈Õ¡˙∂ß ¿Ã∆Â∆Æ ∑Œµ˘.

	m_fHitScale = fScale;
}
void CSfxShootWave::SetHitSfx( DWORD dwIndex, OBJID idSrc, OBJID idDst, FLOAT fScale ) 
{ 
	m_SfxObj2.SetSfx( dwIndex );		// ∏¬∞Ì ≈Õ¡˙∂ß ¿Ã∆Â∆Æ ∑Œµ˘.
	
	m_fHitScale = fScale;
}

// πÊ«‚º∫ ¿÷¥¬ sfx ¿Œ∞°.
void CSfxShootWave::SetDir( BOOL bDir )
{
	m_bDir = bDir;
}



void CSfxShootWave::Process()
{
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );
	if( IsValidObj(pObjDest) )
	{
		m_SfxObj2.m_vPos = pObjDest->GetPos();
		m_SfxObj2.m_vScale = D3DXVECTOR3( m_fHitScale, m_fHitScale, m_fHitScale );			// ≈©±‚¥¬ fHitScaleπË
	} else
	{
		// ≥Øæ∆∞°¥Ÿ ≈∏∞Ÿ¿Ã æ¯æÓ¡≥¿∏∏È ∏∂¡ˆ∏∑¡¬«•∑Œ ≥Øæ∆∞°º≠ ≈Õ¡ˆ∞‘.
	}

	if( (m_nFrame & 3) == 0 )
	{
		for( int i = MAX_SHOOTWAVE_TAIL-1; i > 0; i -- )
		{
			m_vTail[i] = m_vTail[i-1];	// µ⁄∑Œ «—ƒ≠æø πŒ¥Ÿ.
			m_nTailFrame[i] = m_nTailFrame[i-1];
			m_nTailFrame[i]++;
			if( m_nTailFrame[i] >= 56 )
			{
				m_vTail[i].x = m_vTail[i].y = m_vTail[i].z = 0;
				m_nTailFrame[i] = 0;
			}
		}
		m_vTail[0] = GetPos();
		m_nTailFrame[0] = 0;
	}

	if( m_pSfxObj->Process() )		// æ÷¥œ∏ﬁ¿Ãº«¿∫ ∑Á«¡.
	{
		m_pSfxObj->m_nCurFrame = 0;
	}
	
	if( m_bHit == FALSE ) 
	{
		D3DXVECTOR3 vDelta = m_SfxObj2.m_vPos - m_pSfxObj->m_vPos;
		D3DXVec3Normalize( &vDelta, &vDelta );		// ≈∏∞Ÿ¬  ¥‹¿ß∫§≈Õ ±∏«ÿº≠
		D3DXVECTOR3 vPos = GetPos(), vTemp;

		CWorld *pWorld = GetWorld();
		if( pWorld == NULL )	
		{
			Delete();
			return;
		}
		vPos += vDelta * 0.25f;			// ±◊ ∫§≈Õ∑Œ º”µµ∏¶ ∞·¡§«ÿº≠ ¡¬«• ¿Ãµø.
		vTemp = vPos;
		vTemp.y += 1.5f;
		FLOAT y = pWorld->GetUnderHeight( vTemp );	// πŸ¥⁄¿ª µ˚∂Ûº≠ ∞°∞‘,...
		vPos.y = y;
		vPos.y	= m_SfxObj2.m_vPos.y;		// À¨
		m_pSfxObj->m_vPos = vPos;
		SetPos( vPos );
		
		if( m_bDir )
		{
			// ∏∂π˝∞¥√º πÊ«‚∫§≈Õ∑Œ ∏µ® πÊ«‚ µπ∏Æ±‚~~~
			D3DXVECTOR3 vDir      = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
			D3DXVECTOR3 vDestNor  = m_pSfxObj->m_vPos - m_SfxObj2.m_vPos;
			D3DXVECTOR3 vAxis;
			D3DXQUATERNION   qRot;
			
			FLOAT       fTheta;
			D3DXVec3Normalize( &vDestNor, &vDestNor );
			D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
			fTheta = D3DXVec3Dot( &vDir, &vDestNor );
			D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );
			
			D3DXVECTOR3 vYPW;
			
			QuaternionRotationToYPW( qRot, vYPW );
			m_pSfxObj->m_vRotate.x = m_SfxObj2.m_vRotate.x = D3DXToDegree(vYPW.x);
			m_pSfxObj->m_vRotate.y = m_SfxObj2.m_vRotate.y = D3DXToDegree(vYPW.y);
			m_pSfxObj->m_vRotate.z = m_SfxObj2.m_vRotate.z = D3DXToDegree(vYPW.z);
		}
		
		vDelta = m_pSfxObj->m_vPos - m_SfxObj2.m_vPos;		// ¥ÎªÛ∞˙¿« ∞≈∏Æ
		if( D3DXVec3LengthSq( &vDelta ) < 0.4f * 0.4f )		// ∆¯πﬂ ∞≈∏Æ±Ó¡ˆ ¥Ÿ∞°∞°∏È √Êµπ.
		{
			if( m_dwSndHit != NULL_ID )
				PLAYSND( m_dwSndHit, &GetPos() );		// ªÁøÓµÂ «√∑π¿Ã.
			m_bHit = TRUE;
			DamageToTarget();
		}
	}
	else 
	{
		if( m_SfxObj2.Process() )		// æ÷¥œ∏ﬁ¿Ãº« ¥Ÿ ≥°≥µ¿∏∏È ¡æ∑·.
			Delete();
	}
	m_nFrame ++;
}
void CSfxShootWave::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	int nFrame = m_pSfxObj->m_nCurFrame;

	for( int i = 0; i < MAX_SHOOTWAVE_TAIL; i ++ )
	{
		if( m_vTail[i].x )
		{
			m_pSfxObj->m_vPos = m_vTail[i];
			m_pSfxObj->Render2( pd3dDevice );
		}
	}
	
	if( m_bHit == FALSE ) 
	{
		m_pSfxObj->m_vPos = GetPos();
		m_pSfxObj->Render2( pd3dDevice );
	}
	else 
	{
		if( !g_Option.m_bSFXRenderOff )
			m_SfxObj2.Render( pd3dDevice );
	}
}

////////////////////////////////////////////////////////////////
// π´πˆ¿« ∆Ø¡§ ∆ƒ√˜ø° ∏µ≈©µ«¥¬ Sfx
void CSfxPartsLink::Process()
{
	m_nFrame++;
	
	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )	// process¡ﬂ src π´πˆ∞° æ¯æÓ¡≥¿∏∏È ¿⁄µø ªË¡¶
	{
		Delete();
		return;
	}
	
	D3DXVECTOR3 vPos;

	CModelObject *pModel = (CModelObject *)pMover->m_pModel;
	if( m_nPartsLink == 0 )		// ø¿∏•º’
	{
		pModel->GetHandPos( &vPos, PARTS_RWEAPON, pMover->GetMatrixWorld() );		// ø¿∏•º’ ¡÷∏‘ ø˘µÂ¡¬«• ±∏«‘.
	} else
	{
		pModel->GetHandPos( &vPos, PARTS_LWEAPON, pMover->GetMatrixWorld() );
	}
	
	SetPos( vPos );
	
	if( m_pSfxObj->Process() )
	{
		m_bEndFrame = TRUE;
	}
}

void CSfxPartsLink::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !IsVisible() || ( IsCull() && GetType() != 1 ))
		return;
	if( IsUpdateMatrix() )
		UpdateMatrix();
	
	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( m_dwType, m_dwIndex );
	// Model¿Ã æ¯¿∏∏È √‚∑¬«œ¡ˆ æ ¿Ω 
	if( lpModelElem == NULL )
		return;
	
	m_pSfxObj->m_vPos = GetPos();
	m_pSfxObj->m_vRotate.y = GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->m_matScale = m_matScale;
	m_pSfxObj->Render( pd3dDevice, NULL );
}



void CSfxPartsLinkBlade::Process() // ∫Ì∑π¿ÃµÂ ∆ƒ√˜¿Ã∆Â∆Æ¥¬ π–∏Æ Ω∫≈≥≥°≥™∏È º“∏Í
{
	CSfxPartsLink::Process();

	CMover *pMover = prj.GetMover( m_idSrc );
	
	// π–∏ÆΩ∫≈≥ µø¿€ ≥°≥™∏È «ÿ¡¶.
	if( pMover && (pMover->m_pActMover->GetState() & OBJSTA_ATK_ALL) != OBJSTA_ATK_MELEESKILL )
	{
		Delete();		// sfx ªË¡¶.
	}
}

void CSfxPartsLinkJst::Process() // ¡¶Ω∫≈Õ ∆ƒ√˜¿Ã∆Â∆Æ¥¬ ≥°«¡∑π¿”¿Ã∏È º“∏Í
{
	CSfxPartsLink::Process();
	
	if( m_bEndFrame )
	{
		Delete();		// sfx ªË¡¶.
	}
}

////////////////////////////////////////////////////////////////
// æ∆≈©∑ŒπÓ π´πˆ¿« ∆Ø¡§ ∆ƒ√˜ø° ∏µ≈©µ«¥¬ Sfx
void CSfxAllowPartsLink::Process()
{
	m_nFrame++;
	
	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )	// process¡ﬂ src π´πˆ∞° æ¯æÓ¡≥¿∏∏È ¿⁄µø ªË¡¶
	{
		Delete();
		return;
	}
	
	D3DXVECTOR3 vPos;
	
	CModelObject *pModel = (CModelObject *)pMover->m_pModel;
	if( m_nPartsLink == 0 )		// ø¿∏•º’
	{
		pModel->GetHandPos( &vPos, PARTS_RWEAPON, pMover->GetMatrixWorld() );		// ø¿∏•º’ ¡÷∏‘ ø˘µÂ¡¬«• ±∏«‘.
	} else
	{
		pModel->GetHandPos( &vPos, PARTS_LWEAPON, pMover->GetMatrixWorld() );
	}
	
	SetPos( vPos );
	
	if( m_pSfxObj->Process() )
		Delete();
}

void CSfxAllowPartsLink::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !IsVisible() || ( IsCull() && GetType() != 1 ))
		return;
	if( IsUpdateMatrix() )
		UpdateMatrix();
	
	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( m_dwType, m_dwIndex );
	// Model¿Ã æ¯¿∏∏È √‚∑¬«œ¡ˆ æ ¿Ω 
	if( lpModelElem == NULL )
		return;

	m_pSfxObj->m_vPos = GetPos();
	m_pSfxObj->m_vRotate.y = GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->m_matScale = m_matScale;
	m_pSfxObj->Render( pd3dDevice, NULL );
}

void CSfxReady::Process()
{
	CSfx::Process();

	CMover *pSrc = prj.GetMover( m_idSrc );		// Ω√¿¸¿⁄ø°∞‘ πﬂµø.
	if( IsValidObj( pSrc ) )
	{
		if( pSrc->IsStateMode( STATE_BASEMOTION_MODE ) == FALSE )	// Ω√¿¸µø¿€ ≥°≥µ¿∏∏È 
			Delete();		// ªË¡¶.
	} else
		Delete();		// Ω√¿¸¿⁄∞° ªÁ∂Û¡Æµµ ªË¡¶.
}


#if defined(__ZCK_SFX_AMELIORER) && defined(__CLIENT)
CSfxPartsLinkShoulder::CSfxPartsLinkShoulder(): m_nOldEffectValue( NULL ), m_nOldAbilityOption(0), m_v3Offset(D3DXVECTOR3( 0.0f, 0.0f, 0.0f )), m_pZckSfxMover( NULL )
{
#ifdef __SFX_OPT
	SetSkill(FALSE);
#endif
}
#else
CSfxPartsLinkShoulder::CSfxPartsLinkShoulder(): m_nOldEffectValue( NULL ), m_nOldAbilityOption(0), m_v3Offset(D3DXVECTOR3( 0.0f, 0.0f, 0.0f ))
{
#ifdef __SFX_OPT
	SetSkill(FALSE);
#endif
}
#endif // __ZCK_SFX_AMELIORER && __CLIENT
#if defined(__ZCK_SFX_AMELIORER) && defined(__CLIENT)
CSfxPartsLinkShoulder::~CSfxPartsLinkShoulder() 
{
	if( m_pZckSfxMover != NULL )
		return;

	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsValidObj( pMover ) )
	{
		pMover->m_dwFlag &= (~MVRF_SETITEMSFX);
	}
}
#else
CSfxPartsLinkShoulder::~CSfxPartsLinkShoulder() 
{
	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsValidObj( pMover ) )
	{
		pMover->m_dwFlag &= (~MVRF_SETITEMSFX);
	}
}
#endif // __ZCK_SFX_AMELIORER && __CLIENT

#if defined(__ZCK_SFX_AMELIORER) && defined(__CLIENT)
void CSfxPartsLinkShoulder::Process()
{
	CSfx::Process();

	CMover* pMover = m_pZckSfxMover != NULL ? m_pZckSfxMover : prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )	// processAÅAü src A©ˆA°ÏA©ˆA¢“A°∆A¢Æ A®˙A¢¨A®˙A¢ÁAÅA©¯AÄA¢¨A¢¨A®œ AÄAöA•ÏA¢Ø A°ÌA°ßAÅA|
	{
		if( m_pZckSfxMover != NULL )
			SetDelete( TRUE );
		else
			Delete();
		return;
	}
	
	int nAbilityOption	= pMover->GetSetItemClient();

	if( nAbilityOption < 3 || nAbilityOption != m_nOldAbilityOption )
	{
		if( m_pZckSfxMover != NULL )
		{
			SetDelete( TRUE );
		}
		else
		{
			pMover->m_dwFlag &= (~MVRF_SETITEMSFX);
			Delete();
		}
		return;
	}

	int nEffectValue = pMover->GetSetItemSFX_EffectValue();

	if( nEffectValue != m_nOldEffectValue )
	{
		if( m_pZckSfxMover != NULL )
		{
			SetDelete( TRUE );
		}
		else
		{
			pMover->m_dwFlag &= (~MVRF_SETITEMSFX);
			Delete();
		}
		return;
	}
}
#else
void CSfxPartsLinkShoulder::Process()
{
	CSfx::Process();

	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )	// processAÅAü src A©ˆA°ÏA©ˆA¢“A°∆A¢Æ A®˙A¢¨A®˙A¢ÁAÅA©¯AÄA¢¨A¢¨A®œ AÄAöA•ÏA¢Ø A°ÌA°ßAÅA|
	{
		Delete();
		return;
	}
	
	int nAbilityOption	= pMover->GetSetItemClient();

	if( nAbilityOption < 3 || nAbilityOption != m_nOldAbilityOption )
	{
		pMover->m_dwFlag &= (~MVRF_SETITEMSFX);
		Delete();
		return;
	}

	int nEffectValue = pMover->GetSetItemSFX_EffectValue();

	if( nEffectValue != m_nOldEffectValue )
	{
		pMover->m_dwFlag &= (~MVRF_SETITEMSFX);
		Delete();
		return;
	}
}
#endif // __ZCK_SFX_AMELIORER && __CLIENT


#if defined(__ZCK_SFX_AMELIORER) && defined(__CLIENT)
void	CSfxPartsLinkShoulder::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	CMover* pMover = m_pZckSfxMover != NULL ? m_pZckSfxMover : prj.GetMover( m_idSrc );

	if( IsInvalidObj( pMover ) || pMover->m_pModel == NULL )
		return;

	if( m_pZckSfxMover == NULL )
	{
		if( pMover->IsActiveMover() && CWorld::m_nZoomLevel != 0 )
			return;

		if( pMover->GetWorld() && pMover->GetWorld()->GetID() == WI_WORLD_QUIZ && pMover->IsPlayer() == TRUE && pMover->IsDisguise() == TRUE )
			return;
	}
	
	D3DXVECTOR3 vVel = D3DXVECTOR3( 0.0f, 0.04f, 0.0f );
	D3DXVECTOR3 vPos;
	D3DXMATRIX  mPartMatrix;
	
	CModelObject *pModel = (CModelObject *)pMover->m_pModel;

	if( m_nPartsLink == 0 )		// A¢ØAÄA¢¨A°Õ A®˙A¢ÁA°æA®¨
	{
		D3DXMatrixMultiply( &mPartMatrix, (pModel->GetMatrixBone(15)), &(pMover->GetMatrixWorld()) );
	} 
	else
	if( m_nPartsLink == 1 )		// A¢ØAû A®˙A¢ÁA°æA®¨
	{
		D3DXMatrixMultiply( &mPartMatrix, (pModel->GetMatrixBone(11)), &(pMover->GetMatrixWorld()) );
	}
	else
	if( m_nPartsLink == 2 )    // A¢ØAÄA¢¨A°Õ AÜAàA©˜AûAÑA¢Æ
	{
		D3DXMatrixMultiply( &mPartMatrix, (pModel->GetMatrixBone(16)), &(pMover->GetMatrixWorld()) );
	}
	else
	if( m_nPartsLink == 3 )	   // A¢ØAû AÜAàA©˜AûAÑA¢Æ
	{
		D3DXMatrixMultiply( &mPartMatrix, (pModel->GetMatrixBone(12)), &(pMover->GetMatrixWorld()) );
	}
	else
	if( m_nPartsLink == 4 )   // A¢ØAÄA¢¨A°ÕAÇAäA®˙A¢ÁA°æA®¨ + AÜAàA©˜AûAÑA¢Æ A®¨A¢¨A°∆A°Ã
	{
		D3DXMATRIX  mMatrix1;
		D3DXMATRIX  mMatrix2;

		D3DXMatrixMultiply( &mMatrix1, (pModel->GetMatrixBone(15)), &(pMover->GetMatrixWorld()) );
		D3DXMatrixMultiply( &mMatrix2, (pModel->GetMatrixBone(16)), &(pMover->GetMatrixWorld()) );

		D3DXVECTOR3 vPos1 = D3DXVECTOR3( mMatrix1._41, mMatrix1._42, mMatrix1._43 );
		D3DXVECTOR3 vPos2 = D3DXVECTOR3( mMatrix2._41, mMatrix2._42, mMatrix2._43 );

		D3DXVECTOR3 vPos3;
		D3DXVec3Lerp( &vPos3, &vPos1, &vPos2, 0.6f );

		mPartMatrix = mMatrix1;
		mPartMatrix._41 = vPos3.x; 
		mPartMatrix._42 = vPos3.y; 
		mPartMatrix._43 = vPos3.z; 
	}
	else
	if( m_nPartsLink == 5 )   // A¢ØAûAÇAäA®˙A¢ÁA°æA®¨ + AÜAàA©˜AûAÑA¢Æ A®¨A¢¨A°∆A°Ã
	{
		D3DXMATRIX  mMatrix1;
		D3DXMATRIX  mMatrix2;
		
		D3DXMatrixMultiply( &mMatrix1, (pModel->GetMatrixBone(11)), &(pMover->GetMatrixWorld()) );
		D3DXMatrixMultiply( &mMatrix2, (pModel->GetMatrixBone(12)), &(pMover->GetMatrixWorld()) );
		
		D3DXVECTOR3 vPos1 = D3DXVECTOR3( mMatrix1._41, mMatrix1._42, mMatrix1._43 );
		D3DXVECTOR3 vPos2 = D3DXVECTOR3( mMatrix2._41, mMatrix2._42, mMatrix2._43 );
		
		D3DXVECTOR3 vPos3;
		D3DXVec3Lerp( &vPos3, &vPos1, &vPos2, 0.6f );
		
		mPartMatrix = mMatrix1;
		mPartMatrix._41 = vPos3.x; 
		mPartMatrix._42 = vPos3.y; 
		mPartMatrix._43 = vPos3.z; 
	}
	else
	if( m_nPartsLink == 6 )    // A¢¨A¢“ AÅAüA°∆A°Ã
	{
		D3DXMatrixMultiply( &mPartMatrix, (pModel->GetMatrixBone(2)), &(pMover->GetMatrixWorld()) );
	}
	else
	if( m_nPartsLink == 7 )	   // A¢¨A¢“ A®˙AÜA°§A¢Æ
	{
		D3DXMatrixMultiply( &mPartMatrix, (pModel->GetMatrixBone(3)), &(pMover->GetMatrixWorld()) );
	}
	else
	if( m_nPartsLink == 8 )    // A¢ØAÄA¢¨A°ÕA®˘Aï
	{
		D3DXMatrixMultiply( &mPartMatrix, pModel->GetMatrixBone(pModel->GetRHandIdx()), &(pMover->GetMatrixWorld()) );
	}
	else
	if( m_nPartsLink == 9 )	   // A¢ØAûA®˘Aï
	{
		D3DXMatrixMultiply( &mPartMatrix, pModel->GetMatrixBone(pModel->GetLHandIdx()), &(pMover->GetMatrixWorld()) );
	}
	else
	if( m_nPartsLink == 26 )	   // A¢ØAûA®˘Aï
	{
		D3DXMatrixMultiply( &mPartMatrix, pModel->GetMatrixBone(m_nPartsLink), &(pMover->GetMatrixWorld()) );
	}
	else
	if( m_nPartsLink == 29 )	   // A¢ØAûA®˘Aï
	{
		D3DXMatrixMultiply( &mPartMatrix, pModel->GetMatrixBone(m_nPartsLink), &(pMover->GetMatrixWorld()) );
	}
		
			
	vPos.x = mPartMatrix._41;
	vPos.y = mPartMatrix._42-0.05f;
	vPos.z = mPartMatrix._43;

	vPos -= m_v3Offset;
	
	SetPos( vPos );

	if( IsInvalidObj( pMover ) )	// processAÅAü src A©ˆA°ÏA©ˆA¢“A°∆A¢Æ A®˙A¢¨A®˙A¢ÁAÅA©¯AÄA¢¨A¢¨A®œ AÄAöA•ÏA¢Ø A°ÌA°ßAÅA|
		return;
	if( m_pZckSfxMover != NULL || ( pMover->IsMode( TRANSPARENT_MODE ) == 0 ) )
		CSfx::Render(pd3dDevice);
}
#else
void	CSfxPartsLinkShoulder::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	CMover *pMover = prj.GetMover( m_idSrc );

	if( pMover->IsActiveMover() && CWorld::m_nZoomLevel != 0 )
		return;

	if( pMover->GetWorld()->GetID() == WI_WORLD_QUIZ && pMover->IsPlayer() == TRUE && pMover->IsDisguise() == TRUE )
		return;
	
	D3DXVECTOR3 vVel = D3DXVECTOR3( 0.0f, 0.04f, 0.0f );
	D3DXVECTOR3 vPos;
	D3DXMATRIX  mPartMatrix;
	
	CModelObject *pModel = (CModelObject *)pMover->m_pModel;

	if( m_nPartsLink == 0 )		// A¢ØAÄA¢¨A°Õ A®˙A¢ÁA°æA®¨
	{
		D3DXMatrixMultiply( &mPartMatrix, (pModel->GetMatrixBone(15)), &(pMover->GetMatrixWorld()) );
	} 
	else
	if( m_nPartsLink == 1 )		// A¢ØAû A®˙A¢ÁA°æA®¨
	{
		D3DXMatrixMultiply( &mPartMatrix, (pModel->GetMatrixBone(11)), &(pMover->GetMatrixWorld()) );
	}
	else
	if( m_nPartsLink == 2 )    // A¢ØAÄA¢¨A°Õ AÜAàA©˜AûAÑA¢Æ
	{
		D3DXMatrixMultiply( &mPartMatrix, (pModel->GetMatrixBone(16)), &(pMover->GetMatrixWorld()) );
	}
	else
	if( m_nPartsLink == 3 )	   // A¢ØAû AÜAàA©˜AûAÑA¢Æ
	{
		D3DXMatrixMultiply( &mPartMatrix, (pModel->GetMatrixBone(12)), &(pMover->GetMatrixWorld()) );
	}
	else
	if( m_nPartsLink == 4 )   // A¢ØAÄA¢¨A°ÕAÇAäA®˙A¢ÁA°æA®¨ + AÜAàA©˜AûAÑA¢Æ A®¨A¢¨A°∆A°Ã
	{
		D3DXMATRIX  mMatrix1;
		D3DXMATRIX  mMatrix2;

		D3DXMatrixMultiply( &mMatrix1, (pModel->GetMatrixBone(15)), &(pMover->GetMatrixWorld()) );
		D3DXMatrixMultiply( &mMatrix2, (pModel->GetMatrixBone(16)), &(pMover->GetMatrixWorld()) );

		D3DXVECTOR3 vPos1 = D3DXVECTOR3( mMatrix1._41, mMatrix1._42, mMatrix1._43 );
		D3DXVECTOR3 vPos2 = D3DXVECTOR3( mMatrix2._41, mMatrix2._42, mMatrix2._43 );

		D3DXVECTOR3 vPos3;
		D3DXVec3Lerp( &vPos3, &vPos1, &vPos2, 0.6f );

		mPartMatrix = mMatrix1;
		mPartMatrix._41 = vPos3.x; 
		mPartMatrix._42 = vPos3.y; 
		mPartMatrix._43 = vPos3.z; 
	}
	else
	if( m_nPartsLink == 5 )   // A¢ØAûAÇAäA®˙A¢ÁA°æA®¨ + AÜAàA©˜AûAÑA¢Æ A®¨A¢¨A°∆A°Ã
	{
		D3DXMATRIX  mMatrix1;
		D3DXMATRIX  mMatrix2;
		
		D3DXMatrixMultiply( &mMatrix1, (pModel->GetMatrixBone(11)), &(pMover->GetMatrixWorld()) );
		D3DXMatrixMultiply( &mMatrix2, (pModel->GetMatrixBone(12)), &(pMover->GetMatrixWorld()) );
		
		D3DXVECTOR3 vPos1 = D3DXVECTOR3( mMatrix1._41, mMatrix1._42, mMatrix1._43 );
		D3DXVECTOR3 vPos2 = D3DXVECTOR3( mMatrix2._41, mMatrix2._42, mMatrix2._43 );
		
		D3DXVECTOR3 vPos3;
		D3DXVec3Lerp( &vPos3, &vPos1, &vPos2, 0.6f );
		
		mPartMatrix = mMatrix1;
		mPartMatrix._41 = vPos3.x; 
		mPartMatrix._42 = vPos3.y; 
		mPartMatrix._43 = vPos3.z; 
	}
	else
	if( m_nPartsLink == 6 )    // A¢¨A¢“ AÅAüA°∆A°Ã
	{
		D3DXMatrixMultiply( &mPartMatrix, (pModel->GetMatrixBone(2)), &(pMover->GetMatrixWorld()) );
	}
	else
	if( m_nPartsLink == 7 )	   // A¢¨A¢“ A®˙AÜA°§A¢Æ
	{
		D3DXMatrixMultiply( &mPartMatrix, (pModel->GetMatrixBone(3)), &(pMover->GetMatrixWorld()) );
	}
	else
	if( m_nPartsLink == 8 )    // A¢ØAÄA¢¨A°ÕA®˘Aï
	{
		D3DXMatrixMultiply( &mPartMatrix, pModel->GetMatrixBone(pModel->GetRHandIdx()), &(pMover->GetMatrixWorld()) );
	}
	else
	if( m_nPartsLink == 9 )	   // A¢ØAûA®˘Aï
	{
		D3DXMatrixMultiply( &mPartMatrix, pModel->GetMatrixBone(pModel->GetLHandIdx()), &(pMover->GetMatrixWorld()) );
	}
	else
	if( m_nPartsLink == 26 )	   // A¢ØAûA®˘Aï
	{
		D3DXMatrixMultiply( &mPartMatrix, pModel->GetMatrixBone(m_nPartsLink), &(pMover->GetMatrixWorld()) );
	}
	else
	if( m_nPartsLink == 29 )	   // A¢ØAûA®˘Aï
	{
		D3DXMatrixMultiply( &mPartMatrix, pModel->GetMatrixBone(m_nPartsLink), &(pMover->GetMatrixWorld()) );
	}
		
			
	vPos.x = mPartMatrix._41;
	vPos.y = mPartMatrix._42-0.05f;
	vPos.z = mPartMatrix._43;

	vPos -= m_v3Offset;
	
	SetPos( vPos );

	if( IsInvalidObj( pMover ) )	// processAÅAü src A©ˆA°ÏA©ˆA¢“A°∆A¢Æ A®˙A¢¨A®˙A¢ÁAÅA©¯AÄA¢¨A¢¨A®œ AÄAöA•ÏA¢Ø A°ÌA°ßAÅA|
		return;
	if( ((pMover->IsMode( TRANSPARENT_MODE ) ) == 0) )
		CSfx::Render(pd3dDevice);
}
#endif // __ZCK_SFX_AMELIORER && __CLIENT

//sun: 12, ∏ÛΩ∫≈Õ ª˝º∫ Neuz, World
CSfxCursor::CSfxCursor()
{
}
CSfxCursor::~CSfxCursor()
{
}
void CSfxCursor::Process()
{
	SetPos( g_Neuz.m_vCursorPos );		// ∏∂øÏΩ∫ ¡¬«•
	m_pSfxObj->Process();

	if( m_nSec != 0 )	// 0¿∫ 1»∏ «√∑π¿Ã»ƒ ¡æ∑·.
	{
		// π›∫π æ÷¥œ∏ﬁ¿Ãº«
		if( m_pSfxObj->Process() )		
			m_pSfxObj->m_nCurFrame = 0;
	}

	if( g_Neuz.m_pCreateMonItem == NULL )
		Delete();
}


CSfxLinkMover::CSfxLinkMover( )
{
}

CSfxLinkMover::~CSfxLinkMover( )
{
}

void CSfxLinkMover::Process( )
{
	m_nFrame++;
	
	CMover* pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )	// process¡ﬂ src π´πˆ∞° æ¯æÓ¡≥¿∏∏È ¿⁄µø ªË¡¶
	{
		Delete();
		return;
	}
	
	D3DXVECTOR3 vPos = pMover->GetPos( );
	
	SetPos( vPos );
	
	if( m_pSfxObj->Process() )
	{
		m_pSfxObj->m_nCurFrame = 0;
	}
}

void CSfxLinkMover::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !IsVisible() || ( IsCull() && GetType() != 1 ))
		return;
	if( IsUpdateMatrix() )
		UpdateMatrix();
	
	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( m_dwType, m_dwIndex );
	// Model¿Ã æ¯¿∏∏È √‚∑¬«œ¡ˆ æ ¿Ω 
	if( lpModelElem == NULL )
		return;

	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )
		return;

	D3DXVECTOR3 vPos( 0.0f, 0.0f, 0.0f );
//	CModelObject *pModel = (CModelObject *)pMover->m_pModel;
//	pModel->GetEventPos( &vPos, 0 );	// µÓµÂ∏Æ ¡¬«•
	
	D3DXVec3TransformCoord( &vPos, &vPos, pMover->GetMatrixWorldPtr() );
	
	SetPos( vPos );

	m_pSfxObj->m_vPos = GetPos();
	m_pSfxObj->m_vRotate.y = GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->m_matScale = m_matScale;
	m_pSfxObj->Render( pd3dDevice, NULL );
}

CSfxHitParts::CSfxHitParts( const HIT_PARTS eParts, BOOL bBuff ) 
{
	m_eHitParts = eParts;
	m_bBuff = bBuff;
}

CSfxHitParts::~CSfxHitParts( )
{
}

void CSfxHitParts::Process( )
{
	m_nFrame++;
	
	CMover* pMover = prj.GetMover( m_idDest );
	if( IsInvalidObj( pMover ) )	// process¡ﬂ src π´πˆ∞° æ¯æÓ¡≥¿∏∏È ¿⁄µø ªË¡¶
	{
		Delete();
		return;
	}
	
//	D3DXVECTOR3 vPos = pMover->GetPos( );
//	SetPos( vPos );
	
	if( m_pSfxObj->Process() )
	{
		if( m_bBuff )
			m_pSfxObj->m_nCurFrame = 0;
		else Delete();
	}
}

void CSfxHitParts::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !IsVisible() || ( IsCull() && GetType() != 1 ))
		return;
	if( IsUpdateMatrix() )
		UpdateMatrix();
	
	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( m_dwType, m_dwIndex );
	// Model¿Ã æ¯¿∏∏È √‚∑¬«œ¡ˆ æ ¿Ω 
	if( lpModelElem == NULL )
		return;

	CMover *pMover = prj.GetMover( m_idDest );
	if( IsInvalidObj( pMover ) )
		return;

	D3DXVECTOR3 vPos( 0.0f, 0.0f, 0.0f );
//	CModelObject *pModel = (CModelObject *)pMover->m_pModel;
//	pModel->GetEventPos( &vPos, 0 );	// µÓµÂ∏Æ ¡¬«•

	CModelObject* pModel = (CModelObject *)pMover->m_pModel;
	if( !pModel )
		return;

	float fRadius = pMover->GetRadius();
	if( fRadius > 2.0f )
		fRadius = 2.0f;

	D3DXMATRIX mPartMatrix;

	switch( m_eHitParts )
	{
	case HIT_BODY : 
		D3DXMatrixMultiply( &mPartMatrix, (pModel->GetMatrixBone(2)), &(pMover->GetMatrixWorld()) );
		break;
	case HIT_OVERHEAD:
		D3DXMatrixMultiply( &mPartMatrix, (pModel->GetMatrixBone(6)), &(pMover->GetMatrixWorld()) );
		mPartMatrix._42 += fRadius;

		break;
	}

	//Set Position
	D3DXVec3TransformCoord( &vPos, &vPos, &mPartMatrix );
	SetPos( vPos );

	//Set Roatation
	D3DXQUATERNION qRst;
	D3DXVECTOR3 vYPW;
	
	D3DXQuaternionRotationMatrix( &qRst, &mPartMatrix );			// øﬁ∆» ∏≈∆Æ∏ØΩ∫∏¶ ƒı≈Õ¥œæ¿∏∑Œ πŸ≤ﬁ.
	QuaternionRotationToYPW( qRst, vYPW );
	vYPW.y += D3DXToRadian( -90.0f );
	m_pSfxObj->m_vRotate.x = D3DXToDegree(vYPW.x);
	m_pSfxObj->m_vRotate.y = D3DXToDegree(vYPW.y);
	m_pSfxObj->m_vRotate.z = D3DXToDegree(vYPW.z);
	

	m_pSfxObj->m_vPos = GetPos();
//	m_pSfxObj->m_vRotate.y = GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->m_matScale = m_matScale;
	m_pSfxObj->Render2( pd3dDevice, NULL );
}

CSfxAttackParts::CSfxAttackParts( const HIT_PARTS eParts ) 
{
	m_eHitParts = eParts;
}

CSfxAttackParts::~CSfxAttackParts( )
{
}

void CSfxAttackParts::Process( )
{
	m_nFrame++;
	
	CMover* pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )	// process¡ﬂ src π´πˆ∞° æ¯æÓ¡≥¿∏∏È ¿⁄µø ªË¡¶
	{
		Delete();
		return;
	}
	
//	D3DXVECTOR3 vPos = pMover->GetPos( );
//	SetPos( vPos );
	
	if( m_pSfxObj->Process() )
	{
		Delete();
	}
}

void CSfxAttackParts::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !IsVisible() || ( IsCull() && GetType() != 1 ))
		return;
	if( IsUpdateMatrix() )
		UpdateMatrix();
	
	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( m_dwType, m_dwIndex );
	// Model¿Ã æ¯¿∏∏È √‚∑¬«œ¡ˆ æ ¿Ω 
	if( lpModelElem == NULL )
		return;

	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )
		return;

	D3DXVECTOR3 vPos( 0.0f, 0.0f, 0.0f );
//	CModelObject *pModel = (CModelObject *)pMover->m_pModel;
//	pModel->GetEventPos( &vPos, 0 );	// µÓµÂ∏Æ ¡¬«•

	CModelObject* pModel = (CModelObject *)pMover->m_pModel;
	if( !pModel )
		return;

	D3DXMATRIX mPartMatrix;

	float fRadius = pMover->GetRadius();
	if( fRadius > 2.0f )
		fRadius = 2.0f;

	switch( m_eHitParts )
	{
	case HIT_BODY : 
		D3DXMatrixMultiply( &mPartMatrix, (pModel->GetMatrixBone(2)), &(pMover->GetMatrixWorld()) );
		break;
	case HIT_OVERHEAD:
		D3DXMatrixMultiply( &mPartMatrix, (pModel->GetMatrixBone(6)), &(pMover->GetMatrixWorld()) );
		mPartMatrix._42 += fRadius;
		break;
	case HIT_RHAND:
		D3DXMatrixMultiply( &mPartMatrix, (pModel->GetMatrixBone(pModel->GetRHandIdx())), &(pMover->GetMatrixWorld()) );
		break;
	}

	//Set Position
	D3DXVec3TransformCoord( &vPos, &vPos, &mPartMatrix );
	SetPos( vPos );

	//Set Roatation
	D3DXQUATERNION qRst;
	D3DXVECTOR3 vYPW;
	
	D3DXQuaternionRotationMatrix( &qRst, &mPartMatrix );			// øﬁ∆» ∏≈∆Æ∏ØΩ∫∏¶ ƒı≈Õ¥œæ¿∏∑Œ πŸ≤ﬁ.
	QuaternionRotationToYPW( qRst, vYPW );
//	vYPW.y += D3DXToRadian( -90.0f );
	m_pSfxObj->m_vRotate.x = D3DXToDegree(vYPW.x);
	m_pSfxObj->m_vRotate.y = D3DXToDegree(vYPW.y);
	m_pSfxObj->m_vRotate.z = D3DXToDegree(vYPW.z);
	
	m_pSfxObj->m_vPos = GetPos();
//	m_pSfxObj->m_vRotate.y = GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->m_matScale = m_matScale;
	m_pSfxObj->Render2( pd3dDevice, NULL );
}

void CSfxRotate_New::Process()
{
	CSfx::Process();
//	m_nFrame++;
	
	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )	// process¡ﬂ src π´πˆ∞° æ¯æÓ¡≥¿∏∏È ¿⁄µø ªË¡¶
	{
		Delete();
		return;
	}
	
	D3DXVECTOR3 vPos = pMover->GetPos();
	vPos.y += 0.5f;
	SetPos( vPos );
	SetAngle( -pMover->GetAngle() );
	
	if( m_pSfxObj->Process() )
		m_pSfxObj->m_nCurFrame = 0;
}

void CSfxRotate_New::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !IsVisible() || ( IsCull() && GetType() != 1 ))
		return;
	if( IsUpdateMatrix() )
		UpdateMatrix();
	
	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( m_dwType, m_dwIndex );
	// Model¿Ã æ¯¿∏∏È √‚∑¬«œ¡ˆ æ ¿Ω 
	if( lpModelElem == NULL )
		return;
	
	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )
		return;
	
	m_pSfxObj->m_vPos = GetPos();
	m_pSfxObj->m_vRotate.y = GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->m_matScale = m_matScale;
	m_pSfxObj->Render( pd3dDevice, NULL );
}

//===============================================================================================================
CSfxUnion::CSfxUnion( const OBJID _master, const char* _bone, const char* _sfx, N_PATTERN _pattern 
					 /* = PATTERN_FIXED */, const float _scale /* = 1.0f */, int _sec /* = 0.0f */, float _angle /* = 0.0f */ )
{
	m_pSfxObj = NULL;
	m_idMaster = _master;	
	m_strBone = CString( _bone );
	m_strSfx = CString( _sfx );
	m_fScale = _scale;
	m_nSec = _sec;
	m_fAngle = _angle;
}

CSfxUnion::~CSfxUnion()
{
	
}

void CSfxUnion::Process( )
{
	//old codes BEGIN --->
	++m_nFrame;
	if( m_nSec == 0 )	// 0¿∫ 1»∏ «√∑π¿Ã»ƒ ¡æ∑·.
	{
		if( m_pSfxObj->Process() )		// return true¥¬ æ÷¥œ∏ﬁ¿Ãº« ≥°.
			Delete();
	}
	else
	{
		// π›∫π æ÷¥œ∏ﬁ¿Ãº«
		if( m_pSfxObj->Process() )		
			m_pSfxObj->m_nCurFrame = 0;
		if( m_nSec != -1 )		// π´«—π›∫π(-1)¿Ã æ∆¥œ∏È
		{
			if( m_nFrame > ( m_nSec * 60 ) )	// Ω√∞£ √º≈©∏¶ «—¥Ÿ.
				Delete();
		}
	}

	if(m_pSfxObj->m_pSfxBase != NULL)
	{
		for( int j=0; j<m_pSfxObj->m_pSfxBase->m_apParts.GetSize(); j++ )
		{
			if( m_pSfxObj->m_pSfxBase->Part(j)->m_nType != SFXPARTTYPE_MESH )
				continue;
			
			CModelObject* pMesh = NULL;
			pMesh = g_SfxMeshMng.Mesh( m_pSfxObj->m_pSfxBase->Part(j)->m_strTex );
			
			if( pMesh )
				pMesh->FrameMove();
		}
	}
	//old codes END <---

	CMover* pMover = prj.GetMover( m_idMaster );
	if( IsInvalidObj( pMover ) )
		return;

	//∏∏æ‡ Bone¿Ã º≥¡§µ«æÓ ¿÷¥Ÿ∏È ±◊ ¿ßƒ°∏¶ √£¥¬¥Ÿ.
	D3DXVECTOR3 kPos;
	if( !m_strBone.IsEmpty() )
	{
		CModelObject* pModel = (CModelObject*)pMover->GetModel();
		if( !pModel )
			return;

#ifdef __BS_EFFECT_LUA
		pModel->GetPosBone( &kPos, m_strBone.GetString() );
#endif // __BS_EFFECT_LUA
	}

	switch( m_ePattern )
	{
	case PATTERN_FIXED:			UpdateByPattern_FIXED();		break;
	case PATTERN_FIXED_A:		UpdateByPattern_FIXED_A();		break;
	case PATTERN_LINKED:		UpdateByPattern_LINKED();		break;
	case PATTERN_ANGLED:		UpdateByPattern_ANGLED();		break;
	case PATEERN_PROJECTILE:	UpdateByPattern_PROJECTILE();	break;

	default: FLERROR_LOG( PROGRAM_NAME, _T( "CSfxUnion unknown pattern" ) );
		break;
	}
}

BOOL CSfxUnion::UpdateByPattern_FIXED( )
{
	return TRUE;
}

BOOL CSfxUnion::UpdateByPattern_FIXED_A( )
{
	return TRUE;
}
BOOL CSfxUnion::UpdateByPattern_LINKED( )
{
	return TRUE;
}
BOOL CSfxUnion::UpdateByPattern_ANGLED( )
{
	return TRUE;
}
BOOL CSfxUnion::UpdateByPattern_PROJECTILE( )
{
	return TRUE;
}

void CSfxUnion::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
}




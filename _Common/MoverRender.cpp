#include "stdafx.h"
#include "guild.h"
#include "authorization.h"
#include "AppDefine.h"
#include "sfx.h"
#include "party.h"
#include "defineobj.h"
extern	CPartyMng	g_PartyMng;

#include "eveschool.h"
#include "commonctrl.h"
#if defined(__ZCK_SFX_AMELIORER)
#include "Camera.h"
#endif // __ZCK_SFX_AMELIORER

extern CGuildCombat g_GuildCombatMng;



#ifndef __WORLDSERVER

//////////////////////////////////////////////////////////////////////////////////////////////
// CMover
//////////////////////////////////////////////////////////////////////////////////////////////
void CMover::SetRenderPartsEffect( int nParts )
{
	CModelObject* pModel = (CModelObject*)m_pModel;
	FLItemElem *pItemElem = NULL;
	PT_ITEM_SPEC pItemProp = NULL;

	if( IsActiveMover() )
	{
		pItemElem = GetEquipItem( nParts );	// A¢¯A€A¢¬A¡ÍA‚AŠ A©öA¡ìA¡¾A¡ËA¢¯A¢®
		if( pItemElem )
			pItemProp = pItemElem->GetProp();
	} 
	else
	{
		DWORD dwItemID	= m_aEquipInfo[nParts].dwItemID;
		if( dwItemID != 0 )
			pItemProp = g_xSpecManager->GetSpecItem( dwItemID );
	}
	
	if( pItemProp )
	{
#ifdef __ZCK_ARME_RARE
		const BYTE byRarity = pItemElem ? pItemElem->GetZckArmeRareLevel() : FLItemElem::GetZckArmeRareLevelFromFlag( m_aEquipInfo[nParts].byFlag );
		const ZckArmeRareProp* pRarity = FLItemElem::IsZckArmeRareEligibleKind3( pItemProp->dwItemKind3 ) ?
			prj.GetZckArmeRare( static_cast<int>( byRarity ) ) : NULL;
		const BOOL bZckReflect = pRarity && pRarity->m_bReflect;

		if( pItemProp->nReflect > 0 || bZckReflect )	// A¢¬A¢çA‡AƒA¡¤A¨¬A†A¢çA¡ÆA¢® A¡ÆA‰A¡¤AA€A–A€A¢¬A¢¬A¨Ï
#else
		if( pItemProp->nReflect > 0 )	// A¢¬A¢çA‡AƒA¡¤A¨¬A†A¢çA¡ÆA¢® A¡ÆA‰A¡¤AA€A–A€A¢¬A¢¬A¨Ï
#endif // __ZCK_ARME_RARE
		{
			pModel->SetEffect( nParts, XE_REFLECT );	// A¢¬A¢çA‡AƒA¡¤A¨¬A†A¢ç A¢¯A‰A¨ùA‡A€A¢¬A¡¤AŽ A¡¤A¡íA¢¥A¥ì.

			if( nParts == PARTS_RWEAPON && pItemProp->dwItemKind3 == IK3_YOYO )
				pModel->SetEffect( PARTS_LWEAPON, XE_REFLECT );	// A¢¬A¢çA‡AƒA¡¤A¨¬A†A¢ç A¢¯A‰A¨ùA‡A€A¢¬A¡¤AŽ A¡¤A¡íA¢¥A¥ì.
		}
		
		BOOL bExec = FALSE;
		
		switch( pItemProp->dwSfxElemental )
		{
		case ELEMENTAL_FIRE:	pModel->SetEffect( nParts, XE_ITEM_FIRE | (5 << 24) );	bExec = TRUE; break; // A¨¬A’ A…A¢¬A¢¥A‚ A¢¯A‰A¨ùA‡.
		case ELEMENTAL_ELEC:	pModel->SetEffect( nParts, XE_ITEM_ELEC | (0 << 24) );	bExec = TRUE; break; // A€A¨ùA¡¾A¡Ë A¢¯A‰A¨ùA‡.
		case ELEMENTAL_WATER:	pModel->SetEffect( nParts, XE_ITEM_WATER | (5 << 24));	bExec = TRUE; break; // A©öA¡Æ A¢¯A‰A¨ùA‡
		case ELEMENTAL_WIND:	pModel->SetEffect( nParts, XE_ITEM_WIND  | (5 << 24));	bExec = TRUE; break; // A©öA™A¢ÒA¡¤ A¢¯A‰A¨ùA‡
		case ELEMENTAL_EARTH:	pModel->SetEffect( nParts, XE_ITEM_EARTH | (5 << 24));	bExec = TRUE; break;// A¢ÒA¡Í A¢¯A‰A¨ùA‡
		}
		
		if( pItemProp->dwSfxElemental == -1 )
		{			
			int nAttrLevel = 0;
			int	nAttr = 0;
			int nLevel = 0;
			if( pItemElem )
			{
				nAttrLevel = pItemElem->m_nResistAbilityOption;
				nAttr = pItemElem->m_byItemResist;
				nLevel = pItemElem->GetAbilityOption();
			} else
			{
				nAttr	   = (m_aEquipInfo[nParts].nOption & 0x00FF0000) >> 16;
				nAttrLevel = (m_aEquipInfo[nParts].nOption & 0xFF000000) >> 24;
				nLevel = m_aEquipInfo[nParts].nOption & 0xFF;
			}
			
			int nEffLevel = 0;
//sun:13, AA|A¡¤Aƒ AˆA¢çA€A¡Í(A¨ùA“A¨ùA¨¬, A€AA©öA)
			if( nAttrLevel > 10 )
				nAttrLevel = 10;

			if( nAttr && (nAttrLevel > 10 || nAttrLevel < 0) )	// A¨ùA“A¨ùA¨¬A€A¨¬ A€A–A¢¥A‚A¥ìA¡Í A¨ùA“A¨ùA¨¬A¡¤A©öA¨¬A¡×A¡ÆA¨£A€AŒ A€AŒA¡íA©øA‡A’A¢ÒA¡×.
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "m_nResistAbilityOption=%d %s" ), nAttrLevel, GetName() );
				nAttrLevel = 10;
			}

			DWORD dwItemFire = XE_ITEM_FIRE;
			DWORD dwItemElec = XE_ITEM_ELEC;
			DWORD dwItemWater = XE_ITEM_WATER;
			DWORD dwItemWind = XE_ITEM_WIND;
			DWORD dwItemEarth = XE_ITEM_EARTH;
			DWORD dwItemNone = XE_ITEM_GEN;

//sun: AA|A¡¤Aƒ A€AŒA†A‘A†A¢ç A‡A¡ËA¡íA©ø A©öA¢ÒA€A¨ù
			{
				nEffLevel = nLevel;

				switch( pItemProp->dwReferStat1 )
				{
					case WEAPON_GENERAL:
					case WEAPON_UNIQUE:
						{
							if( prj.m_nEnchantLimitLevel[0] > nAttrLevel )
								return;
						}
						break;
					case WEAPON_ULTIMATE:
						{
							if( prj.m_nEnchantLimitLevel[1] > nAttrLevel )
								return;
							
							dwItemFire = XE_ITEM_FIRE_AL;
							dwItemElec = XE_ITEM_ELEC_AL;
							dwItemWater = XE_ITEM_WATER_AL;
							dwItemWind = XE_ITEM_WIND_AL;
							dwItemEarth = XE_ITEM_EARTH_AL;
							dwItemNone = XE_ITEM_GEN_AL;
						}
						break;
				}
			}

			if( nEffLevel >= 0 )
			{
				nEffLevel <<= 24;
				
				switch( nAttr )
				{
				case 0:						pModel->SetEffect( nParts, dwItemNone  | nEffLevel );	break;	//sun: AA|A¡¤Aƒ A€AŒA†A‘A†A¢ç A‡A¡ËA¡íA©ø A©öA¢ÒA€A¨ù
				case SAI79::FIRE:			pModel->SetEffect( nParts, dwItemFire | nEffLevel );	break;	
				case SAI79::ELECTRICITY:	pModel->SetEffect( nParts, dwItemElec | nEffLevel );	break;
				case SAI79::WATER:			pModel->SetEffect( nParts, dwItemWater | nEffLevel );	break;
				case SAI79::WIND:			pModel->SetEffect( nParts, dwItemWind | nEffLevel );	break;
				case SAI79::EARTH:			pModel->SetEffect( nParts, dwItemEarth | nEffLevel );	break;
				}
			}
		}
	}
}
void CMover::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	MoverProp *pMoverProp = GetProp();
	if( pMoverProp == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "CAA¢®A¢´IA¡§A¢®UA¡§A¢®A¡§A©ö A¡ËA¡þA¨ÏA¨£AA¡§A¡ËAA¡§A¢Ò %s" ), GetName() );
	}

	if( !IsVisible() || IsCull() )
		return;

#ifdef __CLIENT		
	if( IsActiveMover() && m_pActMover->IsFly() )
	{
//		if( g_Neuz.m_camera.m_fZoom < 1.0f )		// A¢®A†A¡ËA¢çA¢®A¨úiAI EA¡ËA¡×A¡ËA¡ÍeA¡ÍA¡þA¡§A¢®AA¢®A¡©A¡ËA’A¡§A A¡§A¨¬EA¡§A¡þA¡ËA¡þAIA¢®A†O CIAU.
//			return;
	}

	D3DXVECTOR3 vPos = GetScrPos();

#endif

	pd3dDevice->SetMaterial( g_TextureMng.GetMaterial( pd3dDevice, 0 ) );

	D3DXMATRIX  mWorld;

/*	
	mWorld = m_matWorld;		// AIEAA¡§A¡þIAIA¡ËA¡ÍA m_matWorldA¡ËA¡þ| A¡§A¨¬A¨ÏA¡¤AoA¡ËA¡þA¢®A¡©A¢®A†i matWorldA¡ËA¡þ| A¡§A¨¬A¨ÏA¡¤AU 

	if( IsPlayer() )
		mWorld._42 += 0.07f;		// A¢®A¨úA¡ËA¡þA¡ÍA¡þIAC Eu A¨ÏA¢¬oAIA¡ËA¡þA¡ËA¡þAA¢®A¨Ï A¡ËA?AA¢®A¢´AAU.
	// AACA¢®A AIA¡ÍA¡þA¡ËA? 
*/
 #ifdef __CLIENT
	if( IsPlayer() )
	{
		//D3DXVECTOR3 vPosHeel = m_vScrPos + D3DXVECTOR3( 0.0f, 0.07f, 0.0f ) ;
		//D3DXMatrixTranslation( &m_matTrans, vPosHeel.x, vPosHeel.y, vPosHeel.z );
	}
//	if( IsUpdateMatrix() )
	{
		//UpdateMatrix();		// AA¢®A¨úA¡ËA’oA¡ËA?A¡ËA¢çA¡§A©öA¢®A¨ú A¡ËA¡þAA¡§A¢®A¡ËA¡×A¡ËA¡þ?A¡§A¢ÒA¡§A¡þ A¢®A†A¢®A¡©A¡§A¢ÒAAA¢®A¡© A¢®A¢´A¢®A¡©A¡ËA¡ÍoA¡ËA?A¡ËA¢çA¡§A©öA¢®A¨Ï CNA¡ËA¡ÍU. A¡§A©öA¢®A¨ÏA¨ÏA¢ÒoA¡ËA?A¡ËA¢çA¡§A©öA¢®A¨ú ProcessA¡ËA?A¡ËA¢çA¡§A©öA¢®A¨Ï CNA¡ËA¡ÍU.
	}
 #endif
	mWorld = m_matWorld;		// AIEAA¡§A¡þIAIA¡ËA¡ÍA m_matWorldA¡ËA¡þ| A¡§A¨¬A¨ÏA¡¤AoA¡ËA¡þA¢®A¡©A¢®A†i matWorldA¡ËA¡þ| A¡§A¨¬A¨ÏA¡¤AU 

#ifdef __CLIENT
	switch( GetIndex() )
	{
	case OBJECT_INDEX( 568, MI_SYLIACA1 ):
	case OBJECT_INDEX( 569, MI_SYLIACA2 ):
	case OBJECT_INDEX( 570, MI_SYLIACA3 ):
	case OBJECT_INDEX( 571, MI_SYLIACA4 ):
		if( m_pModel )
			mWorld._42	= (float)( mWorld._42 + (fabs(m_pModel->m_vMax.y - m_pModel->m_vMin.y) / 2.0) );
		break;
	}
#endif
	
	// A¡§A¢®A¡§A©öA¨ÏA¢¬A¡§A¡Ì A¡§A¢ÒA¡§A¡þAEAI AA¡ËA’AA¡ËA¢¥A¢®A|
	if( GetIndex() == OBJECT_INDEX( 216, MI_MASA_TINA ) )
	{
		ResetScale();
		D3DXMatrixScaling(&m_matScale, GetScale().x, GetScale().y, GetScale().z );
	}

	// A¡ËA¡þAAA¡ËA¡þA¡ËA¡þe EcA¡ÍA¡þeA¡ËA¡þA¡ËA¡×A¡ËA¡ÍA AA¨ÏA¢¬A¡ËA¡þA¡ËA¡×.
	if( IsNPC() && m_nDmgCnt > 0 && pMoverProp->dwClass != RANK_SUPER )		// A¡§A¡þA¡ËA¡þA¡§A¢ÒA¡§A¡þA¡ËA¡þA¢®A€AA¡§A¡þ EcA¡ÍA¡þeA¡ËA¡þA¡ËA¡×Ao A¡§A¨¬EAA¡§A¢Ò.
	{
		{
			D3DXMATRIX matDmg;
			int cx = -25 + (xRandom(50));
			int cz = -25 + (xRandom(50));
			D3DXMatrixTranslation( &matDmg, (float)cx / 100.0f, 0, (float)cz / 100.0f );
			D3DXMatrixMultiply( &mWorld, &mWorld, &matDmg );
		}
	}

	D3DXVECTOR3 vPos2 = GetScrPos() - GetWorld()->m_pCamera->m_vPos;
	int nLength = (int)( D3DXVec3Length( &vPos2 ) );
	if( nLength > CWorld::m_fFarPlane ) 
		nLength = (int)( CWorld::m_fFarPlane );
	if( nLength > CWorld::m_fFarPlane - ( CWorld::m_fFarPlane / 4 ) )
		return;

	FLItemElem *pItemElem = NULL, pItemElemCloak;
	PT_ITEM_SPEC pItemProp = NULL;

	CModelObject* pModel = (CModelObject*)m_pModel;

	OverCoatItemRenderCheck(pModel);	

	// A¡ÍA¡þeA¢®A†iAOA¡ËA¡ÍA A¨ÏA¢ÒA¢®A¡þA¢®A¨úaAC AIA¡§A¢®aA¡§A¢®A¡ËA¡× A¡§A©öA¡§A©öA¡§A¢®A.
	SetRenderPartsEffect( PARTS_RWEAPON );
	SetRenderPartsEffect( PARTS_LWEAPON );

	if( IsActiveMover() )
	{
		pItemElem = GetEquipItem( PARTS_SHIELD );	// A¨ÏA¢ÒA¨ÏA¢®A¡§A¢®A¡§A¡ËA¡ËA?A¡ËA¢ç
		if( pItemElem )
			pItemProp = pItemElem->GetProp();
	} else
	{
		DWORD dwItemID	= m_aEquipInfo[PARTS_SHIELD].dwItemID;	// activemover A¢®A†A¡ËA¢ç A¡§A¨¬A¡§A¢®A¡ËA¡ÍNA¨ÏA¢¬A¨ÏA¡ÌAA¡§A¡þ A¢®A†A¡ËA¢çA¢®A¡©oA¡§A¨¬A¡§A¢®AIAUAA¡ËA¡þA¢®A¢´I A¨ÏA¡¤A¢®A¡×A¨ÏA¢¬A¡§A¢ÒA¡ËA¡ÍU.
		if( dwItemID != 0 )
			pItemProp = g_xSpecManager->GetSpecItem( dwItemID );
	}
	if( pItemProp )
	{
#ifdef __ZCK_ARME_RARE
		const BYTE byShieldRarity = pItemElem ? pItemElem->GetZckArmeRareLevel() : FLItemElem::GetZckArmeRareLevelFromFlag( m_aEquipInfo[PARTS_SHIELD].byFlag );
		const ZckArmeRareProp* pShieldRarity = FLItemElem::IsZckArmeRareEligibleKind3( pItemProp->dwItemKind3 ) ?
			prj.GetZckArmeRare( static_cast<int>( byShieldRarity ) ) : NULL;
		if( pItemProp->nReflect > 0 || ( pShieldRarity && pShieldRarity->m_bReflect ) )
#else
		if( pItemProp->nReflect > 0 )
#endif // __ZCK_ARME_RARE
			pModel->SetEffect( PARTS_SHIELD, XE_REFLECT );	// A¡ËA¡þA¡ËA¡×CAA¢®A¢´A¡§A¡þA¡§A¢®A¡ËA¡× A¡ËA?EA¡§A©öCAA¡ËA¡þA¢®A¢´I A¢®A¢´A¢®A¡©A¡ËA¡Ío.
	}
	// A¡ËA¡þAAa AA¡§A¨£A¡§A¢ÒA¡§A¡þAA AOAA¡ËA¡þA¡ËA¡þe AoAA¡ËA¢¥.
	pModel->SetExtTexture( m_pCloakTexture );

	if( pModel )
	{
		O3D_ELEMENT *pElem = pModel->GetParts( PARTS_HAIR );

		if( pElem )
		{
			if( pElem->m_pObject3D )
			{
				pElem->m_pObject3D->m_fAmbient[0] = m_fHairColorR;
				pElem->m_pObject3D->m_fAmbient[1] = m_fHairColorG;
				pElem->m_pObject3D->m_fAmbient[2] = m_fHairColorB;
			}
		}
	}
	
	if( GetActiveObj() == this ) 
		m_pModel->SetGroup( 0 );
	else
	{	
		if( m_pModel->m_nNoEffect == 0 )
		{
			float fDist = 50.0f;
			switch( g_Option.m_nObjectDetail )
			{
			case 0 :	fDist = 20.0f;	break;
			case 1 :	fDist = 10.0f;	break;
			case 2 :	fDist = 5.0f;	break;
			}
			int nLevel = (int)( m_fDistCamera / fDist );
			if( nLevel >= 2 )	nLevel = 2;
			if( nLevel < 0 )	
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "%s, lod lv=%d %f" ), m_szName, nLevel, m_fDistCamera );
				nLevel = 0;
			}
			m_pModel->SetGroup( nLevel );
		} else
			m_pModel->SetGroup( 2 );		// A¢®A¨úA¢®A¢¯A¡ËA¡þA¨ÏA¡¤AU AiAA¢®A¡©A¡ËA’A¡§A A¢®A†A¡ËA¢çAa A¨ÏA¢¬A¢®A¢´AA¡§A¡þ A¡ËA¡ÍUA¢®A†eA¢®A¢´I AiAU.
	}

	if( pModel->m_pModelElem->m_nTextureEx > 0 )		// EA¡ËA¡×AaAA¡§A¨£A¡§A¢ÒA¡§A¡þAA A¡§A¨¬A¨ÏA¡¤A¡ËA¡ÍAA¨ÏA¢¬NAIA¢®A†A¡ËA¢ç?
		pModel->SetTextureEx( pModel->m_pModelElem->m_nTextureEx );
	else
		pModel->SetTextureEx( 0 );

#ifdef __CLIENT
	int nVisibleParts = PARTS_HAT;
	int nInVisibleParts = PARTS_CAP;
	if(!IsOptionRenderCostume(0) )
	{
		nVisibleParts = PARTS_CAP;
		nInVisibleParts = 0;
	}
	if( IsActiveMover() )
	{
		// CiA¡ËA¡þaAI A¡ËA¡þOA¡ËA¡þA¡ËA¡×AA¢®A¡þA¡ËA’o A¨ÏA¢¬?A¢®A¢´AA¡§A¨¬A¨ÏA¡þCIA¡ËA¡ÍAA¢®A†IAIA¨ÏA¢¬A?  // AIA¡§A¡þA¢®AAI AOA¡ËA¡ÍA A¢®A†A¨ÏA¢®A¡ËA?i 
		FLItemElem* pItemElem	= GetEquipItem( nInVisibleParts );
		O3D_ELEMENT* pElement   = NULL;
		pElement = ((CModelObject*)m_pModel)->GetParts(PARTS_HAIR);
		if( pElement )
			pElement->m_nEffect &= ~XE_HIDE;
		if( pItemElem )
		{
			// A¡ËA?UAoACA¢®A¡©oAA¢®A¡© AOA¡§A¨¬uAA¢®A¡©A¢®A†A¨ÏA¢®A¡ËA?i A¡ËA¡þOA¡ËA¡þA¡ËA¡×A¨ÏA¢¬?A¡ËA¡þA¢®A¨úA¢®A†IAIA¢®A†A¡ËA¢çAC A¢®A¨úaAA¡§A¨£AA¢®A¡© A¡ËA?UAo A¡ËA¡þA¨ÏA¡ÌAUA¡ËA¡þ| A¢®A¨úaAA¡§A¨£AA¡ËA¡þA¢®A¢´I A¨ÏA¢ÒUA¨ÏA¡¤UA¡ËA¡ÍU
			FLItemElem* pItemElemOvercoat	= GetEquipItem( nVisibleParts );

			PT_ITEM_SPEC pItemProp = pItemElem->GetProp();
			if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
			{
				//A¡ËA?UCuA¡§A¡þ?A¢®A†A¨ÏA¢®AI A¡§A¨¬A¡§A¢®A¡ËA¡ÍNA¢®A†A¨ÏA¢®A¡ËA?iA¡ËA¡þA¡ËA¡þ AuA¡ËA?eA¡§A¢ÒAANA¡§A¨¬A¨ÏA¡þCO.
				if( 0 == pItemElem->GetLooksChangeItemID() && NULL == pItemElemOvercoat )		//gmpbigsun( 20120604 ), #24885 : A¡§A¨¬oA¢®A¨úA¡§A©öA¢®A¡©cA¡ËA’oAu A¨ÏA¢ÒoA¢®A¨úA¢®A¢¯A¡§A©öoAA¡ËA¢¥ && NULL == pItemElemOvercoat
					pElement = ((CModelObject*)m_pModel)->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
			}
			
			//gmpbigsun( 20111212 ) : A¢®A¨úaEA¨ÏA¢ÒA¡ËA?aAA¢®A¡© CAPA¢®A¢´e
			if( NULL == pItemElemOvercoat )
				pItemElemOvercoat = pItemElem;
						
			if( pItemElemOvercoat )
			{
				if( !pItemElemOvercoat->IsFlag( FLItemElem::expired ) )
				{
					PT_ITEM_SPEC pItemPropOC = NULL;
					
					//A¡ËA?UCuA¡§A¡þ?A¢®A†A¨ÏA¢®AI A¢®A†A¨ÏA¢®A¡ËA?i CA¡§A¨£A¡ËA¡ÍcA¡§A¨¬A¡§A¢®AIAUAA¡ËA¡þA¢®A¢´I!
					if( 0 != pItemElemOvercoat->GetLooksChangeItemID() )
						pItemPropOC = g_xSpecManager->GetSpecItem( pItemElemOvercoat->GetLooksChangeItemID() );
					else 
						pItemPropOC = pItemElemOvercoat->GetProp();

					if( pItemPropOC && pItemPropOC->dwBasePartsIgnore != -1 )
					{
						if( pItemPropOC->dwBasePartsIgnore == PARTS_HEAD )
							((CModelObject*)m_pModel)->SetEffect(PARTS_HAIR, XE_HIDE );
						
						((CModelObject*)m_pModel)->SetEffect(pItemPropOC->dwBasePartsIgnore, XE_HIDE );
					}
					else
					{
						if( pElement )
							pElement->m_nEffect &= ~XE_HIDE;
					}
				}
			}
			
		}
		else
		{
			// A¡ËA?UAoACA¢®A¡©oAA¢®A¡© AOA¡§A¨¬uAA¢®A¡©A¢®A†A¨ÏA¢®A¡ËA?i A¡ËA¡þOA¡ËA¡þA¡ËA¡×A¨ÏA¢¬?A¡ËA¡þA¢®A¨úA¢®A†IAIA¢®A†A¡ËA¢çAC A¢®A¨úaAA¡§A¨£AA¢®A¡© A¡ËA?UAo A¡ËA¡þA¨ÏA¡ÌAUA¡ËA¡þ| A¢®A¨úaAA¡§A¨£AA¡ËA¡þA¢®A¢´I A¨ÏA¢ÒUA¨ÏA¡¤UA¡ËA¡ÍU
			FLItemElem* pItemElemOvercoat	= GetEquipItem( nVisibleParts );
			
			if( pItemElemOvercoat )
			{
				if( !pItemElemOvercoat->IsFlag( FLItemElem::expired ) )
				{
					PT_ITEM_SPEC pItemPropOC = NULL;

					//A¡ËA?UCuA¡§A¡þ?A¢®A†A¨ÏA¢®AI A¢®A†A¨ÏA¢®A¡ËA?i CA¡§A¨£A¡ËA¡ÍcA¡§A¨¬A¡§A¢®AIAUAA¡ËA¡þA¢®A¢´I!
					if( 0 != pItemElemOvercoat->GetLooksChangeItemID() )
						pItemPropOC = g_xSpecManager->GetSpecItem( pItemElemOvercoat->GetLooksChangeItemID() );
					else 
						pItemPropOC = pItemElemOvercoat->GetProp();

					if( pItemPropOC && pItemPropOC->dwBasePartsIgnore != -1 )
					{
						if( pItemPropOC->dwBasePartsIgnore == PARTS_HEAD )
							((CModelObject*)m_pModel)->SetEffect(PARTS_HAIR, XE_HIDE );

						((CModelObject*)m_pModel)->SetEffect(pItemPropOC->dwBasePartsIgnore, XE_HIDE );
					}
				}
			}	
		}		
	}
	else
	{
		// CiA¡ËA¡þaAI A¡ËA¡þOA¡ËA¡þA¡ËA¡×AA¢®A¡þA¡ËA’o A¨ÏA¢¬?A¢®A¢´AA¡§A¨¬A¨ÏA¡þCIA¡ËA¡ÍAA¢®A†IAIA¨ÏA¢¬A?  // AIA¡§A¡þA¢®AAI A¡§A¨¬A¨ÏA¨£A¡ËA¡ÍAA¢®A†A¨ÏA¢®A¡ËA?i
		DWORD dwId	= m_aEquipInfo[nInVisibleParts].dwItemID;
		O3D_ELEMENT*	pElement = NULL;
		
		if( dwId != 0 )
		{
			PT_ITEM_SPEC pItemProp	= g_xSpecManager->GetSpecItem( dwId );
			
			if( pItemProp && pItemProp->dwBasePartsIgnore != NULL_ID )
			{
				if( 0 == m_aEquipInfo[ nInVisibleParts ].dwLooksChangeItemID && 0 == m_aEquipInfo[nVisibleParts].dwItemID )		//gmpbigsun( 20120604 ), #24885 : A¡§A¨¬oA¢®A¨úA¡§A©öA¢®A¡©cA¡ËA’oAu A¨ÏA¢ÒoA¢®A¨úA¢®A¢¯A¡§A©öoAA¡ËA¢¥
					pElement = pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
			}
			
			// A¡ËA?UAoACA¢®A¡©oAA¢®A¡© AOA¡§A¨¬uAA¢®A¡©A¢®A†A¨ÏA¢®A¡ËA?i A¡ËA¡þOA¡ËA¡þA¡ËA¡×A¨ÏA¢¬?A¡ËA¡þA¢®A¨úA¢®A†IAIA¢®A†A¡ËA¢çAC A¢®A¨úaAA¡§A¨£AA¢®A¡© A¡ËA?UAo A¡ËA¡þA¨ÏA¡ÌAUA¡ËA¡þ| A¢®A¨úaAA¡§A¨£AA¡ËA¡þA¢®A¢´I A¨ÏA¢ÒUA¨ÏA¡¤UA¡ËA¡ÍU
			DWORD dwParts = nVisibleParts;
			dwId	= m_aEquipInfo[dwParts].dwItemID;
					
			//gmpbigsun( 20111212 ) : A¢®A¨úaEA¨ÏA¢ÒA¡ËA?aAA¢®A¡© CAPA¢®A¢´e
			//other moverACA¢®A†A¨ÏA¢®A¡ËA?i A¡ËA?UCuA¡§A¡þ?A¢®A†A¨ÏA¢® AA¨ÏA¢¬A¡ËA¡þA¡ËA¡×
			if( 0 == dwId )
			{
				dwParts = nInVisibleParts;
				dwId = m_aEquipInfo[ dwParts ].dwItemID;

				if( 0 != m_aEquipInfo[ dwParts ].dwLooksChangeItemID )
					dwId = m_aEquipInfo[ dwParts ].dwLooksChangeItemID;
			}
			else
			{
				if( 0 != m_aEquipInfo[ dwParts ].dwLooksChangeItemID )
					dwId = m_aEquipInfo[ dwParts ].dwLooksChangeItemID;
			}

			if( dwId != 0 )
			{
				if( !(m_aEquipInfo[dwParts].byFlag & FLItemElem::expired) )
				{
					pItemProp	= g_xSpecManager->GetSpecItem( dwId );
					if( pItemProp && pItemProp->dwBasePartsIgnore != NULL_ID )
					{
						if( pItemProp->dwBasePartsIgnore == PARTS_HEAD )
							((CModelObject*)m_pModel)->SetEffect(PARTS_HAIR, XE_HIDE );

						((CModelObject*)m_pModel)->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE );
					}
					else
					{
						if( pElement )
							pElement->m_nEffect &= ~XE_HIDE;
					}
				}
			}
	
		}
		else
		{
			// A¡ËA?UAoACA¢®A¡©oAA¢®A¡© AOA¡§A¨¬uAA¢®A¡©A¢®A†A¨ÏA¢®A¡ËA?i A¡ËA¡þOA¡ËA¡þA¡ËA¡×A¨ÏA¢¬?A¡ËA¡þA¢®A¨úA¢®A†IAIA¢®A†A¡ËA¢çAC A¢®A¨úaAA¡§A¨£AA¢®A¡© A¡ËA?UAo A¡ËA¡þA¨ÏA¡ÌAUA¡ËA¡þ| A¢®A¨úaAA¡§A¨£AA¡ËA¡þA¢®A¢´I A¨ÏA¢ÒUA¨ÏA¡¤UA¡ËA¡ÍU
			dwId	= m_aEquipInfo[nVisibleParts].dwItemID;
			if( dwId != 0 )
			{
				if( !(m_aEquipInfo[nVisibleParts].byFlag & FLItemElem::expired) )
				{					
					if( 0 != m_aEquipInfo[ nVisibleParts ].dwLooksChangeItemID )
						dwId = m_aEquipInfo[ nVisibleParts ].dwLooksChangeItemID;

					pItemProp	= g_xSpecManager->GetSpecItem( dwId );
					if( pItemProp && pItemProp->dwBasePartsIgnore != NULL_ID )
					{
						if( pItemProp->dwBasePartsIgnore == PARTS_HEAD )
							((CModelObject*)m_pModel)->SetEffect(PARTS_HAIR, XE_HIDE );
						
						((CModelObject*)m_pModel)->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE );
					}
					else
					{
						if( pElement )
							pElement->m_nEffect &= ~XE_HIDE;
					}
				}
			}
													
		}
	}
#endif //__CLIENT
	
		if( IsMode( TRANSPARENT_MODE ) )		// AoA¡ËA¡þiA¢®A¡©oAAA¢®A†A¡ËA¢ç A¡§A¨¬A¡§A¢®A¡ËA¡ÍOA¡ËA’A¢®A—A¡ËA¡þA¡ËA¡þ A¢®A¢´A¢®A¡©A¡ËA¡Ío.
		{
			if( g_eLocal.GetState( EVE_SCHOOL ) == 1 &&		// CA¡§A¡ËA¢®A¨úA¨ÏA¢¬ A¡ËA¡ÍeCA¢®A¢¯Au A¡§A©öA¢®A¨ÏA¨ÏA¢ÒoA¡ËA?A¡ËA¢ç AA¢®A‹A¡§A©öO,
				g_eLocal.GetState( EVE_SCHOOL_BATTLE ) < SBS_START &&	// A¡ËA¡ÍeCA¢®A¢¯Au A¡§A¢ÒAAU AIAu,
				!IsAuthHigher( AUTH_GAMEMASTER ) )	// AIA¨ÏA¢ÒY A¢®A¡©cA¡ËA?eAU,
			{
				m_pModel->Render( pd3dDevice, &mWorld ); 
			}
			else
			{
				m_pModel->SetBlendFactor( 80 );
				if( IsActiveMover() ||		// AUA¢®A¨úaAUA¡§A¢ÒAAA¡§A¡þ A¨ÏA¢ÒYAoA¡ËA¡þiAA¡ËA¡þA¢®A¢´I AaA¢®A¢´A A¢®A|EA¡ËA¢¥AA¡§A¡þ
					(IsActiveMover() == FALSE && g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER )) )		// AA¡ËA¡þAIAIA¡ÍA¡þA¢®A CAA¢®A¢´A¨ÏA¢ÒAIA¡§A¨¬iA¢®A†A¡ËA¢ç A¢®A†A¢®A¢¯A¡ËA¡þA¡ËA’A¡ËA¡þe.
					m_pModel->Render( pd3dDevice, &mWorld );		// A¨ÏA¢ÒYAoA¡ËA¡þiAA¡ËA¡þA¢®A¢´I AaA¢®A¢´A
				m_pModel->SetBlendFactor( 255 );
			}
		}
		else
		{
			m_pModel->SetBlendFactor(m_wBlendFactor);
//			A¡ËA¡þoA¡§A¢ÒA¡§A¡þAI CA¢®AA¡§A¢ÒA A¡§A¡þ?A¢®A†A¨ÏA¢® A¡§A¢ÒAA¡ÍA¡þA¡ÍA¡þ AUA¡ÍA¡þa
//			if( m_pObjHighlight == this )	m_pModel->m_nHighlight = TRUE;
			m_pModel->Render( pd3dDevice, &mWorld );	// AIA¨ÏA¢ÒY A¢®A¡©oAA AaA¢®A¢´A
		}

#if defined(__ZCK_AILES_SFX) && defined(__CLIENT)
	RenderAilesSfxZacks( pd3dDevice, pModel, &mWorld );
#endif // __ZCK_AILES_SFX && __CLIENT

	if( m_pObjHighlight == this )
	{
	//	m_pObjHighlight = NULL;
		if( m_pModel->m_nNoEffect == 0 )
		{
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		}

		if( IsMode( TRANSPARENT_MODE ) == 0 )		// AoA¡ËA¡þiA¢®A¡©oAAA¢®A†A¡ËA¢ç A¡§A¨¬A¡§A¢®A¡ËA¡ÍOA¡ËA’A¢®A—A¡ËA¡þA¡ËA¡þ A¢®A¢´A¢®A¡©A¡ËA¡Ío.
		{
			if( m_pModel->m_nNoEffect == 0 )
			{
				m_pModel->Render( pd3dDevice, &mWorld ); 
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
				pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
				pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
			}
		}

	}
	if( m_pActMover )
		m_pActMover->Render( pd3dDevice, &mWorld ); 

	// A¨ÏA¢¬?A¢®A†A¨ÏA¢¬A¡ËA¡þ| A¡ËA¡þAAaA¡ËA?A¡ËA¢ç A¡§A¡þIAA¨ÏA¨£COA¢®A¢´AA¢®A†i A¡§A¢ÒAA¡ÍA¡þA¡ÍA¡þCN AUA¡ÍA¡þa		2016.07.11
	//if( !m_pRide && IsMode( TRANSPARENT_MODE ) == 0 )		// AoA¡ËA¡þiA¢®A¡©oAAA¢®A†A¡ËA¢ç A¡§A¨¬A¡§A¢®A¡ËA¡ÍOA¡ËA’A¢®A—A¡ËA¡þA¡ËA¡þ A¢®A¢´A¢®A¡©A¡ËA¡Ío.
	//{
	//	D3DXMATRIX mRide = m_matWorld;
	//	pItemElem = GetEquipItem( PARTS_CLOAK );
	//	//O3D_ELEMENT *pElem = pModel->GetParts( PARTS_CLOAK );;
	//	if( pItemElem )
	//		pItemProp = pItemElem->GetProp();
	//	if( pItemProp && pItemProp->dwItemKind3 == IK3_WING )
	//	{
	//		//((CModelObject*)m_pModel)->SetEffect(PARTS_CLOAK, XE_HIDE );
	//		//pElem->m_nEffect |= XE_HIDE;

	//		static const int SPINE1_BONE = 4;	// A¡ËA¡þo A¡§A¡þIA¢®A¨úU AA¡ËA¡ÍAA¨ÏA¡þ(A¨ÏA¢¬?A¢®A†A¨ÏA¢¬AC A¡ËA?oAA¢®A€AOAA¡§A¡þ AA¨ÏA¢¬A¡ËA¡þ?AI A¡ËA¡þoAC A¡ËA?oAA¢®A€AOAA¢®A¡© A¡ÍA¡þuA¡ËA¡þA¢®AA¡ËA¡ÍU)
	//		static const int SPINE2_BONE = 5;	// A¡ËA¡þn A¡§A¡þIA¢®A¨úU AA¡ËA¡ÍAA¨ÏA¡þ(A¨ÏA¢¬?A¢®A†A¨ÏA¢¬A¢®A†A¡ËA¢ç A¡ËA¡þoAC A¡ËA?oAA¢®A€AOAA¢®A¡© A¡ÍA¡þuA¡ËA¡þA¢®AŒA¡ËA¡ÍoA¡ËA’oA¡ÍA¡þA¡ÍA¡þ AA¢®A—AA¡ËA¢çA¡ËA¡ÍA A¡ËA¡þn A¡§A¡þIA¢®A¨úUA¡ËA?A¡ËA¢ç A¡§A¡þUAIA¡ËA¡ÍU)
	//		D3DXMATRIX* pmatSpine1Bone = pModel->GetMatrixBone( SPINE1_BONE );
	//		D3DXMATRIX* pmatSpine2Bone = pModel->GetMatrixBone( SPINE2_BONE );
	//		if( pmatSpine1Bone && pmatSpine2Bone )
	//		{
	//			D3DXMATRIX matEvent;
	//			D3DXMatrixIdentity( &matEvent );
	//			static float fPP1 = 180.0;
	//			static float fPP2 = -90.0;
	//			static float fPP3 = 0.0;
	//			D3DXMATRIX matRotation;
	//			D3DXMatrixIdentity( &matRotation );
	//			D3DXMatrixRotationX( &matRotation, D3DXToRadian( fPP1 ) );
	//			D3DXMatrixMultiply( &matEvent, &matRotation, &matEvent );
	//			D3DXMatrixRotationZ( &matRotation, D3DXToRadian( fPP2 ) );
	//			D3DXMatrixMultiply( &matEvent, &matRotation, &matEvent );
	//			D3DXMatrixRotationY( &matRotation, D3DXToRadian( fPP3 ) );
	//			D3DXMatrixMultiply( &matEvent, &matRotation, &matEvent );
	//			D3DXMatrixMultiply( &matEvent, &matEvent, pmatSpine1Bone );
	//			
	//			// A¢®A¨úeAA¡§A¡Ë A¨ÏA¢¬?A¢®A†A¨ÏA¢¬A¡ËA?A¡ËA¢ç A¡ËA¡ÍeCA¡§A¨£A¡§A©öA¢®A¨ÏA¡ËA¡þA¡ËA¡þ AOA¡§A¢ÒA AA¢®A—AA¡ËA¢ç A¡§A¡þA¡ËA¡þAA¡ËA¢¥ (AA¨ÏA¡þEA A¨ÏA¢ÒYA¡ÍA¡þaA¡§A¢ÒA A¡ËA¡þA¨ÏA¡ÌA¡ÍA¡þc A¨ÏA¢¬?A¢®A†A¨ÏA¢¬A¢®A†A¡ËA¢ç else AE AUA¡ÍA¡þaA¢®A¢´I AeAIA¡ÍA¡þCA¡§A¨¬iA¡§A¨¬A¨ÏA¡þ CO)
	//			if( pItemProp && ( pItemProp->dwID == ITEM_INDEX( 5819, II_RID_RID_WIN_FEATHER01 ) || pItemProp->dwID == ITEM_INDEX( 5820, II_RID_RID_WIN_FEATHER02 ) ) )
	//			{
	//				matEvent._41 = pmatSpine2Bone->_41;
	//				matEvent._42 = pmatSpine2Bone->_42 - 0.02f;
	//				matEvent._43 = pmatSpine2Bone->_43 + 0.1f;
	//			}
	//			else
	//			{
	//				matEvent._41 = pmatSpine2Bone->_41;
	//				matEvent._42 = pmatSpine2Bone->_42;
	//				matEvent._43 = pmatSpine2Bone->_43;
	//			}

	//			D3DXMatrixMultiply( &mRide, &matEvent, &m_matWorld );
	//		}

	//		

	//		DWORD dwItemID	= m_aEquipInfo[PARTS_CLOAK].dwItemID;
	//		static int nMode = 0;
	//		if ( dwItemID )
	//		{
	//			CModelObject* pwingModel = (CModelObject*)prj.m_modelMng.LoadModel( D3DDEVICE, OT_ITEM, dwItemID );
	//			if ( pwingModel )
	//			{
	//				if( D3DXVec3LengthSq( &m_pActMover->m_vDelta ) < 0.001f )
	//				{
	//					if ( !m_pCloakWing && pwingModel )
	//					{
	//					m_pCloakWing =	pwingModel;
	//					CString strMotion = m_pCloakWing->GetMotionFileName( _T("stand") );
	//					FLASSERT( strMotion != _T("") );
	//					m_pCloakWing->LoadMotion( strMotion );
	//					m_pCloakWing->SetMotionBlending( TRUE );
	//					m_pCloakWing->SetLoop( ANILOOP_LOOP );
	//					nMode = 1;
	//					}
	//				}
	//				m_pCloakWing->Render( pd3dDevice, &mRide );
	//				m_pCloakWing->m_nNoEffect = 0;
	//			}
	//		}
	//	}
	//	
	//}

// 05 start
	/*if( IsMode( TRANSPARENT_MODE ) == 0 )		// AoA¡ËA¡þiA¢®A¡©oAAA¢®A†A¡ËA¢ç A¡§A¨¬A¡§A¢®A¡ËA¡ÍOA¡ËA’A¢®A—A¡ËA¡þA¡ËA¡þ A¢®A¢´A¢®A¡©A¡ËA¡Ío.
	{
		D3DXMATRIX mRide = m_matWorld;
		pItemElem = GetEquipItem( PARTS_MASK );
		if( pItemElem )
			pItemProp = pItemElem->GetProp();
		if( pItemProp && pItemProp->dwItemKind3 == IK3_WING )
		{
			static const int SPINE1_BONE = 4;	// A¡ËA¡þo A¡§A¡þIA¢®A¨úU AA¡ËA¡ÍAA¨ÏA¡þ(A¨ÏA¢¬?A¢®A†A¨ÏA¢¬AC A¡ËA?oAA¢®A€AOAA¡§A¡þ AA¨ÏA¢¬A¡ËA¡þ?AI A¡ËA¡þoAC A¡ËA?oAA¢®A€AOAA¢®A¡© A¡ÍA¡þuA¡ËA¡þA¢®AA¡ËA¡ÍU)
			static const int SPINE2_BONE = 5;	// A¡ËA¡þn A¡§A¡þIA¢®A¨úU AA¡ËA¡ÍAA¨ÏA¡þ(A¨ÏA¢¬?A¢®A†A¨ÏA¢¬A¢®A†A¡ËA¢ç A¡ËA¡þoAC A¡ËA?oAA¢®A€AOAA¢®A¡© A¡ÍA¡þuA¡ËA¡þA¢®AŒA¡ËA¡ÍoA¡ËA’oA¡ÍA¡þA¡ÍA¡þ AA¢®A—AA¡ËA¢çA¡ËA¡ÍA A¡ËA¡þn A¡§A¡þIA¢®A¨úUA¡ËA?A¡ËA¢ç A¡§A¡þUAIA¡ËA¡ÍU)
			D3DXMATRIX* pmatSpine1Bone = pModel->GetMatrixBone( SPINE1_BONE );
			D3DXMATRIX* pmatSpine2Bone = pModel->GetMatrixBone( SPINE2_BONE );
			if( pmatSpine1Bone && pmatSpine2Bone )
			{
				D3DXMATRIX matEvent;
				D3DXMatrixIdentity( &matEvent );

				D3DXMATRIX matRotation;
				D3DXMatrixIdentity( &matRotation );
				D3DXMatrixRotationX( &matRotation, D3DXToRadian( 180 ) );
				D3DXMatrixMultiply( &matEvent, &matRotation, &matEvent );
				D3DXMatrixRotationZ( &matRotation, D3DXToRadian( -90 ) );
				D3DXMatrixMultiply( &matEvent, &matRotation, &matEvent );
				D3DXMatrixMultiply( &matEvent, &matEvent, pmatSpine1Bone );

				// A¢®A¨úeAA¡§A¡Ë A¨ÏA¢¬?A¢®A†A¨ÏA¢¬A¡ËA?A¡ËA¢ç A¡ËA¡ÍeCA¡§A¨£A¡§A©öA¢®A¨ÏA¡ËA¡þA¡ËA¡þ AOA¡§A¢ÒA AA¢®A—AA¡ËA¢ç A¡§A¡þA¡ËA¡þAA¡ËA¢¥ (AA¨ÏA¡þEA A¨ÏA¢ÒYA¡ÍA¡þaA¡§A¢ÒA A¡ËA¡þA¨ÏA¡ÌA¡ÍA¡þc A¨ÏA¢¬?A¢®A†A¨ÏA¢¬A¢®A†A¡ËA¢ç else AE AUA¡ÍA¡þaA¢®A¢´I AeAIA¡ÍA¡þCA¡§A¨¬iA¡§A¨¬A¨ÏA¡þ CO)
				if( pItemProp && ( pItemProp->dwID == ITEM_INDEX( 5819, II_RID_RID_WIN_FEATHER01 ) || pItemProp->dwID == ITEM_INDEX( 5820, II_RID_RID_WIN_FEATHER02 ) ) )
				{
					matEvent._41 = pmatSpine2Bone->_41;
					matEvent._42 = pmatSpine2Bone->_42 - 0.02f;
					matEvent._43 = pmatSpine2Bone->_43 + 0.1f;
				}
				else
				{
					matEvent._41 = pmatSpine2Bone->_41;
					matEvent._42 = pmatSpine2Bone->_42;
					matEvent._43 = pmatSpine2Bone->_43;
				}

				D3DXMatrixMultiply( &mRide, &matEvent, &m_matWorld );
			}
			DWORD dwItemID	= m_aEquipInfo[PARTS_MASK].dwItemID;
			if ( dwItemID )
			{
				CModel* pwingModel = prj.m_modelMng.LoadModel( D3DDEVICE, OT_ITEM, dwItemID );
				if ( pwingModel )
				{
					pwingModel->Render( pd3dDevice, &mRide );
					pwingModel->m_nNoEffect = 0;
				}
			}
		}
		
	}*/


// 05 end
	
#ifdef __CLIENT
	if( IsMode( TRANSPARENT_MODE ) == 0 )		// AoA¡ËA¡þiA¢®A¡©oAAA¢®A†A¡ËA¢ç A¡§A¨¬A¡§A¢®A¡ËA¡ÍOA¡ËA’A¢®A—A¡ËA¡þA¡ËA¡þ A¢®A¢´A¢®A¡©A¡ËA¡Ío.
	{
		D3DXMATRIX mRide = m_matWorld;

		if( m_pRide && m_pActMover->IsFly() )
		{
			m_pRide->m_nNoEffect = m_pModel->m_nNoEffect;
			//D3DXMATRIX mRide = m_matWorld;
			PT_ITEM_SPEC pRideProp = g_xSpecManager->GetSpecItem( m_dwRideItemIdx );		// CoAc AA¡ËA¡þA¢®A†iAOA¡ËA¡ÍA AA¢®A¡©A¢®A†IAC CAA¢®A¢´IA¡§A¢®UA¡§A¢®A¡§A©ö.

			if( pRideProp && pRideProp->dwItemKind3 == IK3_STICK )
			{
				if( pModel->GetMatrixBone(0) )	// A¡§A¡þA¨ÏA¨£AUA¢®A¢´cA¡ËA¡ÍA A¢®A¢´cA¡§A¢®A¡ËA¡× A¡§A¡þA¢®A¡©A¡ËA?A¡ËA¢ç A¡ËA?A¢®A¨úA¢®A†aA¡§A¢ÒAAA¡ËA¡Í.
				{
					D3DXMATRIX mBoundY;
					D3DXMatrixTranslation( &mBoundY, 0, pModel->GetMatrixBone(0)->_42, 0 );
					D3DXMatrixMultiply( &mRide, &mBoundY, &m_matWorld );
				}
			}

			if( pRideProp && pRideProp->dwItemKind3 == IK3_WING )
			{
				static const int SPINE1_BONE = 4;	// A¡ËA¡þo A¡§A¡þIA¢®A¨úU AA¡ËA¡ÍAA¨ÏA¡þ(A¨ÏA¢¬?A¢®A†A¨ÏA¢¬AC A¡ËA?oAA¢®A€AOAA¡§A¡þ AA¨ÏA¢¬A¡ËA¡þ?AI A¡ËA¡þoAC A¡ËA?oAA¢®A€AOAA¢®A¡© A¡ÍA¡þuA¡ËA¡þA¢®AA¡ËA¡ÍU)
				static const int SPINE2_BONE = 5;	// A¡ËA¡þn A¡§A¡þIA¢®A¨úU AA¡ËA¡ÍAA¨ÏA¡þ(A¨ÏA¢¬?A¢®A†A¨ÏA¢¬A¢®A†A¡ËA¢ç A¡ËA¡þoAC A¡ËA?oAA¢®A€AOAA¢®A¡© A¡ÍA¡þuA¡ËA¡þA¢®AŒA¡ËA¡ÍoA¡ËA’oA¡ÍA¡þA¡ÍA¡þ AA¢®A—AA¡ËA¢çA¡ËA¡ÍA A¡ËA¡þn A¡§A¡þIA¢®A¨úUA¡ËA?A¡ËA¢ç A¡§A¡þUAIA¡ËA¡ÍU)
				D3DXMATRIX* pmatSpine1Bone = pModel->GetMatrixBone( SPINE1_BONE );
				D3DXMATRIX* pmatSpine2Bone = pModel->GetMatrixBone( SPINE2_BONE );
				if( pmatSpine1Bone && pmatSpine2Bone )
				{
					D3DXMATRIX matEvent;
					D3DXMatrixIdentity( &matEvent );

					D3DXMATRIX matRotation;
					D3DXMatrixIdentity( &matRotation );
					D3DXMatrixRotationX( &matRotation, D3DXToRadian( 180 ) );
					D3DXMatrixMultiply( &matEvent, &matRotation, &matEvent );
					D3DXMatrixRotationZ( &matRotation, D3DXToRadian( -90 ) );
					D3DXMatrixMultiply( &matEvent, &matRotation, &matEvent );
					D3DXMatrixMultiply( &matEvent, &matEvent, pmatSpine1Bone );

					// A¢®A¨úeAA¡§A¡Ë A¨ÏA¢¬?A¢®A†A¨ÏA¢¬A¡ËA?A¡ËA¢ç A¡ËA¡ÍeCA¡§A¨£A¡§A©öA¢®A¨ÏA¡ËA¡þA¡ËA¡þ AOA¡§A¢ÒA AA¢®A—AA¡ËA¢ç A¡§A¡þA¡ËA¡þAA¡ËA¢¥ (AA¨ÏA¡þEA A¨ÏA¢ÒYA¡ÍA¡þaA¡§A¢ÒA A¡ËA¡þA¨ÏA¡ÌA¡ÍA¡þc A¨ÏA¢¬?A¢®A†A¨ÏA¢¬A¢®A†A¡ËA¢ç else AE AUA¡ÍA¡þaA¢®A¢´I AeAIA¡ÍA¡þCA¡§A¨¬iA¡§A¨¬A¨ÏA¡þ CO)
					if( pRideProp && ( pRideProp->dwID == ITEM_INDEX( 5819, II_RID_RID_WIN_FEATHER01 ) || pRideProp->dwID == ITEM_INDEX( 5820, II_RID_RID_WIN_FEATHER02 ) ) )
					{
						matEvent._41 = pmatSpine2Bone->_41;
						matEvent._42 = pmatSpine2Bone->_42 - 0.02f;
						matEvent._43 = pmatSpine2Bone->_43 + 0.1f;
					}
					else
					{
						matEvent._41 = pmatSpine2Bone->_41;
						matEvent._42 = pmatSpine2Bone->_42;
						matEvent._43 = pmatSpine2Bone->_43;
					}

					D3DXMatrixMultiply( &mRide, &matEvent, &m_matWorld );
				}
			}

			m_pRide->Render( pd3dDevice, &mRide );
			m_pRide->m_nNoEffect = 0;

			// AIA¡§A¡þA¢®A¡©A¡§A¢®CAA¡§A¡þ AIEA¡ËA¡þA¡ËA?eAI A¡§A¨¬A¡§A¢®A¡ËA¡ÍIA¡ËA¡ÍUA¢®A|A¨ÏA¢¬A¡§A¡ÌA¡ËA¡þOAoA¢®A¨úA¨ÏA¢ÒA¢®A†A¡ËA¢çA¡ËA¡ÍA AIEA¡ËA¡þA¡ËA?e~
			if( pRideProp && pRideProp->dwID == ITEM_INDEX( 5801, II_RID_RID_BOR_LADOLF ) || pRideProp->dwID == ITEM_INDEX( 5802, II_RID_RID_BOR_JLADOLF )
				|| pRideProp->dwID == ITEM_INDEX( 30125, II_RID_RID_BOR_JLADOLF_S ) || pRideProp->dwID == ITEM_INDEX( 30124, II_RID_RID_BOR_LADOLF_S ) )	//sun: 9,
			{
				if( m_pLadolf == NULL )
				{ 
					m_pLadolf = new CModelObject;
					m_pLadolf->InitDeviceObjects( pd3dDevice );
					m_pLadolf->LoadBone( "mvr_Ladolf.chr" );
					m_pLadolf->LoadElement( "mvr_Ladolf.o3d", 0 );
					m_pLadolf->LoadMotion( "mvr_Ladolf_stand.ani" );
				}

				D3DXMATRIX mLadolf = mRide;
				D3DXVECTOR3 vLocal;

				FLOAT fAngXZ = GetAngle();
				FLOAT fAngH  = GetAngleX();
				AngleToVector( &vLocal, fAngXZ, -fAngH-10.0f, 1.4f );

				// A¨ÏA¢¬?A¢®A¨úa A¡§A¢ÒAAU - A¢®A†A¡ËA¢çA¡§A©öOAA¨ÏA¡þ
				if( m_pActMover->GetStateFlag() & OBJSTAF_ACC )
				{
					if( m_dwLadolfFlag == 0 )
					{
						m_pLadolf->LoadMotion( "mvr_Ladolf_walk.ani" );
						m_pLadolf->SetMotionBlending( TRUE );
						m_pLadolf->SetLoop( ANILOOP_LOOP );
						m_dwLadolfFlag = 1;
					}
				}
				else
				{
					if( D3DXVec3LengthSq( &m_pActMover->m_vDelta ) < 0.001f )
					{
						if( m_dwLadolfFlag == 1 )
						{
							m_pLadolf->LoadMotion( "mvr_Ladolf_stand.ani" );
							m_pLadolf->SetMotionBlending( TRUE );
							m_pLadolf->SetLoop( ANILOOP_LOOP );
							m_dwLadolfFlag = 0;
						}
					}
				}
			
				mLadolf._41 += vLocal.x;
				mLadolf._42 += vLocal.y;
				mLadolf._43 += vLocal.z;
				
				m_pLadolf->m_nNoEffect = m_pModel->m_nNoEffect;
				m_pLadolf->Render( pd3dDevice, &mLadolf );
				m_pLadolf->m_nNoEffect = 0;
			}
		}

		//gmpbigsun( 20101230 ) : AIA|A¡§A¡þIAI A¨ÏA¢ÒoCAA¡ËA¡ÍA A¨ÏA¢ÒA¢®A¡þA¨ÏA¢ÒoA¢®A†A¡ËA¢ç AA¢®AAOAoA¢®A†i render
		//gmpbigsun( 20120404 ) : A¢®A¨úA¢®A¢¯A¡ËA¡þA¨ÏA¡¤AU AaA¢®A¢´AA¡§A¢ÒAA¡ËA?A¢®AŒ AIA¡§A¢®aA¡§A¢®A¡ËA¡× pass
		if( m_pModel->m_nNoEffect != 2 )
			m_buffs.Render( );
	}

	// A¡§A¡þnCaAA¨ÏA¡þ AuA¨ÏA¢ÒA¨ÏA¢® xxA¢®A†A¢®A‹A¡ÍA¡þA¡ÍA¡þ AIA¨ÏA¢¬A¢®A¡©A¡ËA?A¡ËA¢ç A¡ÍA¡þeA¡§A¨¬iA¡ËA?AA¡ËA¡ÍAA¢®A†IAA¡§A¡þ TABAA¡ËA¡þA¢®A¢´I AUA¡ÍA¡þA¡ËA? AA¡ËA¡þA¢®A†UA¡§A¢®A.

	if(m_pAngelFlag && m_pAngel != NULL)		//sun: 8AA¢®A€ A¡ËA?A¢®AŒAA¡§A A¡§A©öOE? Neuz, World, Trans
	{
		m_pAngel->m_nNoEffect = m_pModel->m_nNoEffect;
		m_pAngel->Render( pd3dDevice, &m_AngelWorldM );
		m_pAngel->m_nNoEffect = 0;
	}

//sun:10, __EVE_BALLOON
	if(m_pBalloonFlag && m_pBalloon != NULL)
	{
		m_pBalloon->m_nNoEffect = m_pModel->m_nNoEffect;
		m_pBalloon->Render( pd3dDevice, &m_BalloonWorldM );
		m_pBalloon->m_nNoEffect = 0;
	}
	
	if( /*IsPlayer() &&*/ IsActiveMover() == FALSE && g_pPlayer->m_pActMover->IsFly() )
	{
		if( GetProp()->bKillable == 1 && GetProp()->dwFlying == 1 )		// AA¢®A¢¯AIA¡ËA¡ÍAA¢®A†O A¢®A†A¡ËA¢çA¡ËA¡ÍECNA¨ÏA¢¬N / A¡§A¡þnCaA¡ËA¡þA¢®A€ A¡ËA¡þA¡ËA¡þ AA¡ËA¡þA¢®A†UAA¡ËA¡þA¢®A¢´I AaEuA¡ËA¡ÍU. 
		{
			D3DXVECTOR3	vCamera = g_Neuz.m_camera.m_vLookAt - g_Neuz.m_camera.GetPos();			// AA¢®A¡þA¡ËA¡þA¡§A¡©A¡ËA’oA¢®A†A¡ËA¢ç A¡§A¡þA¡ËA¡þA¡ËA¡ÍA AA¡ËA¢¥A¡ËA¡þeA¡§A¡þA¡ËA¢¥AI
			D3DXVECTOR3 vObj	= GetScrPos() - g_Neuz.m_camera.GetPos();	// AA¢®A¡þA¡ËA¡þA¡§A¡©A¡ËA’oA¡ËA?A¡ËA¢çA¡§A©öA¢®A¨Ï thisA¡ËA?AA¡§A¡þeAA¢®A—A¡§A¢®A¡ËA¡×AC A¡§A¡þA¡ËA¢¥AI.
			D3DXVec3Normalize( &vCamera, &vCamera );		// A¨ÏA¢¬eA¡ËA¡þA¢®A¡©A¡ËA’oAIAi.
			D3DXVec3Normalize( &vObj, &vObj );
			FLOAT fDot = D3DXVec3Dot( &vCamera, &vObj );
			if( fDot > cosf(D3DXToRadian(15.0f)) )
			{
				CWndWorld *pWndWorld = (CWndWorld *)g_WndMng.m_pWndWorld;
				if( pWndWorld )
				{
					pWndWorld->AddFlyTarget( GetId() );
				}
			}
		}
	}
	
	// A¨ÏA¢ÒA¢®A¡þA¨ÏA¢ÒoA¡ËA?A¡ËA¢ç A¢®A¨úaA¡§A¡þA¢®A¡©AuAA¡ËA¡þA¢®A¢´I A¡ËA¡ÍA¡§A¡©A¢®A¢´AAOA¡ËA¡ÍA AIA¡§A¢®aA¡§A¢®A¡ËA¡×. - AIA¢®A†C AA¢®A¢¯AA¢®A¡©A¡ËA’A¢®A—A¢®A¨úiAo A¡§A¨¬EA¡§A¨¬A¨ÏA¨£A¡§A¨¬iAA¨ÏA¨£A¡ËA¡ÍU.
	if( (m_dwFlag & MVRF_EFFECT1) == 0 )
	{
		CSfx *pSfx = NULL;
		switch( m_dwIndex )
		{
		case OBJECT_INDEX( 564, MI_RISEM1 ):
		case OBJECT_INDEX( 565, MI_RISEM2 ):
		case OBJECT_INDEX( 566, MI_RISEM3 ):
		case OBJECT_INDEX( 567, MI_RISEM4 ):
			pSfx = CreateSfx( D3DDEVICE, XI_INDEX( 1619, XI_NPCRISEMSIGN ), GetPos(), GetId(), D3DXVECTOR3(0,0,0), NULL_ID, -1 );
			if( pSfx )
				pSfx->SetScale( GetScale() );
		}
		if( pSfx )
			m_dwFlag |= MVRF_EFFECT1;
	}
	if( m_pModel->m_nNoEffect == 0 )
		RenderQuestEmoticon( pd3dDevice );

	//sun: A¡ËA¡ÍA¢®A¡þA¢®A¨úoA¨ÏA¢ÒUA¢®A†AA¡ËA¡þA¡ËA¡×A¢®A¨úaA¢®A| Neuz
	//gmpbigsun(20110921) : A¡§A¨¬A¡§A¢®A¡ËA?A A¡§A¢Òa, A¡§A¡þ?A¡§A¢ÒAAA¨ÏA¡þAI A¡§A¨¬A¡§A¢®A¡ËA¡ÍOA¡ËA’A¢®A—A¡ËA¡þA¡ËA¡þ CA¡§A¨£A¡§A¨¬A¨ÏA¡þAo AA¨ÏA¡þA¢®A†A¡ËA¢çCO.
	if( IsActiveMover() && m_pModel && IsDie() && HasBuffByIk3(IK3_TEXT_DISGUISE) == FALSE && IsDisguise( ) == FALSE )
	{
		CModelObject* pModelObj = (CModelObject*)m_pModel;
		
		O3D_ELEMENT* pElement = NULL;
		pElement = pModelObj->GetParts(PARTS_HEAD);
		
		if( pElement && pElement->m_pObject3D )
		{
			GMOBJECT* pGmObj = pElement->m_pObject3D->GetGMOBJECT(0);
			
			if( pGmObj )
			{
				(*pGmObj->m_pMtrlBlkTexture) = CMover::m_pTextureEye[m_bySex][m_dwHeadMesh];
			}
		}
	}
#endif
	
}

#if defined(__ZCK_AILES_SFX) && defined(__CLIENT)
void CMover::RenderAilesSfxZacks( LPDIRECT3DDEVICE9 pd3dDevice, CModelObject* pModelObject, const D3DXMATRIX* pWorld, BOOL bPreserveDeviceState )
{
	if( !pd3dDevice
		|| !pModelObject
		|| !pWorld
		|| !m_pAilesSfxZacks
		|| !m_pAilesSfxZacks->GetModel()
		|| !m_pAilesSfxZacks->GetModel()->m_pModelElem
		|| !m_pAilesSfxZacks->m_pSfxObj
		|| !m_pAilesSfxZacks->m_pSfxObj->m_pSfxBase )
	{
		return;
	}

	if( !IsPlayer()
		|| !pModelObject->m_pBone
		|| !pModelObject->m_mUpdateBone
		|| pModelObject->m_pBone->m_nEventParentIdx[0] < 0
		|| pModelObject->m_pBone->m_nEventParentIdx[0] >= pModelObject->m_pBone->m_nMaxBone )
	{
		return;
	}

	if( !IsOptionRenderMask() || IsMode( TRANSPARENT_MODE ) )
		return;

	D3DXVECTOR3 vLocalPos( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vRenderPos( 0.0f, 0.0f, 0.0f );
	pModelObject->GetEventPos( &vLocalPos, 0 );
	D3DXVec3TransformCoord( &vRenderPos, &vLocalPos, pWorld );

	D3DXVECTOR3 vAxisX( pWorld->_11, pWorld->_12, pWorld->_13 );
	D3DXVECTOR3 vAxisY( pWorld->_21, pWorld->_22, pWorld->_23 );
	D3DXVECTOR3 vAxisZ( pWorld->_31, pWorld->_32, pWorld->_33 );
	D3DXVECTOR3 vRenderScale( D3DXVec3Length( &vAxisX ), D3DXVec3Length( &vAxisY ), D3DXVec3Length( &vAxisZ ) );

	if( vRenderScale.x <= 0.0001f || vRenderScale.y <= 0.0001f || vRenderScale.z <= 0.0001f )
		vRenderScale = GetScale();

	FLOAT fRenderAngle = D3DXToDegree( (FLOAT)atan2( pWorld->_31, pWorld->_33 ) );
	if( fRenderAngle < 0.0f )
		fRenderAngle += 360.0f;

	m_pAilesSfxZacks->SetPos( vRenderPos );
	m_pAilesSfxZacks->SetAngle( fRenderAngle );
	m_pAilesSfxZacks->SetScale( vRenderScale );

	m_pAilesSfxZacks->m_pSfxObj->m_vPos = m_pAilesSfxZacks->GetPos();
	m_pAilesSfxZacks->m_pSfxObj->m_vRotate = D3DXVECTOR3( 0.0f, m_pAilesSfxZacks->GetAngle(), 0.0f );
	m_pAilesSfxZacks->m_pSfxObj->m_vScale = m_pAilesSfxZacks->GetScale();
	m_pAilesSfxZacks->m_pSfxObj->m_matScale = m_pAilesSfxZacks->GetMatrixScale();

	LPDIRECT3DSTATEBLOCK9 pSavedState = NULL;

	if( bPreserveDeviceState )
	{
		HRESULT hrState = pd3dDevice->CreateStateBlock( D3DSBT_ALL, &pSavedState );
		if( FAILED( hrState ) || !pSavedState )
			return;
	}

	m_pAilesSfxZacks->m_pSfxObj->Render( pd3dDevice, NULL );

	if( pSavedState )
	{
		pSavedState->Apply();
		pSavedState->Release();
		pSavedState = NULL;
	}
}
#endif // __ZCK_AILES_SFX && __CLIENT

// ÀÌ°ÍÀÌ È£ÃâµÉ¶§´Â ¹Ù·Î ÀÌÀü¿¡ CMover::Render()°¡ È£ÃâµÇ¾î¾ß ÇÑ´Ù.
void CMover::RenderPartsEffect( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( IsMode( TRANSPARENT_MODE ) )		// Ã?ÃµÂ¸Ã­Â»Ã³Ã?Ã?Â°Â¡ Â¾Ã?Â´Ã?Â¶Â§Â¸Â¸ Â·Â»Â´Ãµ.
		return;

	if( !IsVisible() || IsCull() )
		return;

	pd3dDevice->SetMaterial( g_TextureMng.GetMaterial( pd3dDevice, 0 ) );

	D3DXVECTOR3 vPos = GetScrPos();
	D3DXMATRIX  matWorld;
	
	matWorld = m_matWorld;

	D3DXVECTOR3 vPos2 = GetScrPos() - GetWorld()->m_pCamera->m_vPos;
	int nLength = (int)( D3DXVec3Length( &vPos2 ) );
	if( nLength > CWorld::m_fFarPlane ) 
		nLength = (int)( CWorld::m_fFarPlane );
	if( nLength > CWorld::m_fFarPlane - ( CWorld::m_fFarPlane / 4 ) )
		return;
	//m_pModel->SetBlendFactor( 255 );//255 - nBlendFactor );

	FLItemElem* pItemElem = NULL;
	PT_ITEM_SPEC pItemProp = NULL;

	if( IsActiveMover() )
	{
		pItemElem = GetEquipItem( PARTS_RWEAPON );	// Â¿Ã?Â¸Â¥Ã?Ã? Â¹Â«Â±Ã¢Â¿Â¡
		if( pItemElem )
			pItemProp = pItemElem->GetProp();
	} else
	{
		DWORD dwItemID = m_aEquipInfo[PARTS_RWEAPON].dwItemID;
		if( dwItemID != 0 )
			pItemProp = g_xSpecManager->GetSpecItem( dwItemID );
	}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//gmpbigsun( 20110309 ) : Ã?Â§Â¿Â¡Â¼Â­ Â¾Ã²Ã?Âº pItemPropÃ?Â» Â¹Ã¦Ã?ÃÂ¸Â¦ Ã?Ã·Â°Ã­Ã?Ã?Ã?Â»Â°Ã¦Â¿Ã¬ Â¹Ã¦Ã?ÃÂ·Ã? Â¹Ã?Â²Ã£Â¹Ã¶Â¸Â°Â´Ã? 
//Â¹Â«Â½Â¼ Ã?Ã?ÂµÂµÃ?Ã?ÃÃ¶ Â¾Ã?Â¼Ã¶Â¾Ã¸Â´Ã?.. ÃÃ?Â¼Â®Ã?Â³Â¸Â®Ã?Ã?

//	if( IsActiveMover() )
//	{
//		pItemElem = GetEquipItem( PARTS_SHIELD );	// Â¹Ã¦Ã?ÃÂ¿Â¡
//		if( pItemElem )
//			pItemProp = pItemElem->GetProp();
//	} else
//	{
//		DWORD dwItemID = m_aEquipInfo[PARTS_SHIELD].dwId;	// activemover Â°Â¡ Â¾Ã?Â´Ã?Â³Ã°Ã?Âº Â°Â¡Â»Ã³Â¾Ã?Ã?Ã?Ã?Ã?Ã?Â¸Â·Ã? Â²Â¨Â³Â½Â´Ã?.
//		if( dwItemID != NULL_ID )
//			pItemProp = g_xSpecManager->GetSpecItem( dwItemID );
//	}
//	if( pItemProp )
//	{
//		if( pItemProp->nReflect > 0 )	// Â¸Â®Ã?Ã?Â·ÂºÃ?Â®Â°Â¡ Â°Ã?Â·ÃÃ?Ã?Ã?Â¸Â¸Ã©
//			((CModelObject*)m_pModel)->SetEffect( PARTS_SHIELD, XE_REFLECT );	// Â¸Â®Ã?Ã?Â·ÂºÃ?Â® Â¿Ã?Â¼Ã?Ã?Â¸Â·Ã? Â·Â»Â´Ãµ.
//	}
	
	m_pModel->SetGroup( 0 );
	if( IsMode( TRANSPARENT_MODE ) == 0 )		// Ã?ÃµÂ¸Ã­Â»Ã³Ã?Ã?Â°Â¡ Â¾Ã?Â´Ã?Â¶Â§Â¸Â¸ Â·Â»Â´Ãµ.
	{
		if( pItemProp )
		{
			int nLO = 0, nRO = 0;
			
			if( IsActiveMover() )
			{
				if( GetEquipItem( PARTS_LWEAPON ) )
				{
					nLO = GetEquipItem( PARTS_LWEAPON )->m_nResistAbilityOption;
				}

			}
			else
			{
				nLO = (m_aEquipInfo[PARTS_LWEAPON].nOption & 0xFF000000) >> 24;
			}
			
			if( IsActiveMover() )
			{
				if( GetEquipItem( PARTS_RWEAPON ) )
				{
					nRO = GetEquipItem( PARTS_RWEAPON )->m_nResistAbilityOption;
				}
			}
			else
			{
				nRO = (m_aEquipInfo[PARTS_RWEAPON].nOption & 0xFF000000) >> 24;
			}

			//gmpbigsun: Â¾Ã§Â¼Ã?Â°Ã?, Â¾Ã§Â¼Ã?ÂµÂµÂ³Â¢ ÂµÃ®Ã?Ã? Â¾Ã§Â¼Ã?Â·Ã¹Â´Ã? Ã?Ã?Â¼Ã?Â°ÃºÂ°Â°Ã?Âº IK3_Ã?Â¸Ã?Ã?Ã?Â» Â¾Â²Â°Ã­ Â´Ã?Â¸Â¸ dwHandedÂ¿Â¡ ÂµÃ?Â¼Ã?Ã?Ã?ÃÃ¶ ÂºÃÂ·Ã¹ÂµÃ?Â¾Ã®Ã?Ã?Â´Ã?.
			// Â°Ã­Â·Ã?, 3Â¹Ã¸Ã?Â° Ã?Ã?Ã?Ã? ik3Ã?Â¸Ã?Ã?Â¿Â¡ Â¾Ã§Â¼Ã?Ã?ÃÂ°Ã¦Â¿Ã¬ Ã?Ã?Â´Ã§ Ã?Â¸Ã?Ã?Ã?Â¸Â·Ã? Â¸Â¶Ã?Ã?Â¼Â­ Â³Ã?Â¾Ã®ÃÃ?Â´Ã?.
			DWORD dwIK3 = pItemProp->dwItemKind3;
			if( pItemProp->dwHanded == HD_TWO )
			{
				if( IK3_SWD == dwIK3 )
					dwIK3 = IK3_THSWD;
				else if( IK3_AXE == dwIK3 )
					dwIK3 = IK3_THAXE;
			}

			//Ã?Â©Â·Ã?Â½Âº ÂºÂ¸Â¿Ã¬ Â¶Ã?Ã?Ã? IK3_BOWÂ·Ã? ÂµÃ©Â¾Ã®Â°Â¡Ã?Ã?Ã?Â¸Â¹Ã?Â·Ã?( Â½ÂºÃ?Â³Â¶Â§Â¹Â®Â¿Â¡ ), Ã?Â©Â·Ã?Â½Âº ÂºÂ¸Â¿Ã¬Ã?ÃÂ°Ã¦Â¿Ã¬ IK3_CROSSBOWÂ·Ã? Â¹Ã?Â²Ã£ÃÃ?Â´Ã?.
			if( dwIK3 == IK3_BOW && 100 == pItemProp->nDestData1[0] )
				dwIK3 = IK3_CROSSBOW;
			
			int nType = IET_NONE;
			if( pItemProp->IsBaruna( ) )
				nType += IET_BARUNA;

			//Â¾Ã§Â¼Ã?Â¹Â«Â±Ã¢Ã?Ã?Â¼Âº Ã?Â¼Ã?Â©
			if( pItemElem )
			{
				if( pItemElem->IsSetCombinedOption( ) )
					nType += IET_COMPOSE_TWOHAND;
			}
			else 
			{
				if( m_aEquipInfo[PARTS_RWEAPON].bIsCombined )
					nType += IET_COMPOSE_TWOHAND;
			}
		
			m_pModel->RenderEffect( pd3dDevice, &matWorld, dwIK3, nLO, nRO, nType ); 
		}
		else
			m_pModel->RenderEffect( pd3dDevice, &matWorld, NULL_ID ); 		
	}

	extern CPartsLaser g_Laser;
	D3DXMatrixIdentity( &matWorld );
	g_Laser.Render( pd3dDevice, &matWorld, g_ModelGlobal.m_vCameraPos, g_ModelGlobal.m_vCameraForward );

//sun: Â¸Ã?Ã?Ã?Â¿Ã?Â´ÃÃ?Â¿ Ã?Ã?Ã?Ã?Â¾Ã® Â¹Ã?Â»Ã§!!!
	if( m_dwIndex == OBJECT_INDEX( 715, MI_DU_METEONYKER ) || m_dwIndex == OBJECT_INDEX( 753, MI_DU_METEONYKER2 ) || m_dwIndex == OBJECT_INDEX( 754, MI_DU_METEONYKER3 ) ||
		m_dwIndex == OBJECT_INDEX( 755, MI_DU_METEONYKER4 ) || m_dwIndex == OBJECT_INDEX( 1548, MI_DU_METEONYKER5 ) )
	{
		extern CPartsFireDragon	g_FireDragon;
		g_FireDragon.Render( pd3dDevice, &matWorld );
	}
}

#if defined(__ZCK_SFX_AMELIORER) && defined(__CLIENT)
void CMover::RenderZckWeaponElementSfxViewport( LPDIRECT3DDEVICE9 pd3dDevice, CModelObject* pModel, const D3DXMATRIX* pWorld )
{
	if (pd3dDevice == NULL || pModel == NULL || pWorld == NULL)
		return;

	CModel* pOldModel = m_pModel;
	m_pModel = pModel;

	SetRenderPartsEffect( PARTS_RWEAPON );
	SetRenderPartsEffect( PARTS_LWEAPON );

	FLItemElem* pItemElem = NULL;
	PT_ITEM_SPEC pItemProp = NULL;

	if( IsActiveMover() )
	{
		pItemElem = GetEquipItem( PARTS_RWEAPON );
		if( pItemElem )
			pItemProp = pItemElem->GetProp();
	}
	else
	{
		DWORD dwItemID = m_aEquipInfo[PARTS_RWEAPON].dwItemID;
		if( dwItemID != 0 )
			pItemProp = g_xSpecManager->GetSpecItem( dwItemID );
	}

	if( pItemProp )
	{
		int nLO = 0;
		int nRO = 0;

		if( IsActiveMover() )
		{
			FLItemElem* pLeftWeapon = GetEquipItem( PARTS_LWEAPON );
			FLItemElem* pRightWeapon = GetEquipItem( PARTS_RWEAPON );
			if( pLeftWeapon )
				nLO = pLeftWeapon->m_nResistAbilityOption;
			if( pRightWeapon )
				nRO = pRightWeapon->m_nResistAbilityOption;
		}
		else
		{
			nLO = ( m_aEquipInfo[PARTS_LWEAPON].nOption & 0xFF000000 ) >> 24;
			nRO = ( m_aEquipInfo[PARTS_RWEAPON].nOption & 0xFF000000 ) >> 24;
		}

		DWORD dwIK3 = pItemProp->dwItemKind3;
		if( pItemProp->dwHanded == HD_TWO )
		{
			if( dwIK3 == IK3_SWD )
				dwIK3 = IK3_THSWD;
			else if( dwIK3 == IK3_AXE )
				dwIK3 = IK3_THAXE;
		}

		if( dwIK3 == IK3_BOW && pItemProp->nDestData1[0] == 100 )
			dwIK3 = IK3_CROSSBOW;

		int nType = IET_NONE;
		if( pItemProp->IsBaruna() )
			nType += IET_BARUNA;

		if( pItemElem )
		{
			if( pItemElem->IsSetCombinedOption() )
				nType += IET_COMPOSE_TWOHAND;
		}
		else if( m_aEquipInfo[PARTS_RWEAPON].bIsCombined )
		{
			nType += IET_COMPOSE_TWOHAND;
		}

		pModel->RenderEffect( pd3dDevice, pWorld, dwIK3, nLO, nRO, nType, TRUE );
	}
	else
	{
		pModel->RenderEffect( pd3dDevice, pWorld, NULL_ID, 0, 0, 0, TRUE );
	}

	m_pModel = pOldModel;
}

void CMover::RenderZckSfxViewport( LPDIRECT3DDEVICE9 pd3dDevice, CModelObject* pModel, const D3DXMATRIX* pWorld )
{
	if (pd3dDevice == NULL || pModel == NULL || pWorld == NULL)
		return;

	RefreshZckManagedSfx();

	LPDIRECT3DSTATEBLOCK9 pSavedState = NULL;
	if( FAILED( pd3dDevice->CreateStateBlock( D3DSBT_ALL, &pSavedState ) ) || pSavedState == NULL )
		return;

	const D3DXMATRIX matWorldOld = m_matWorld;
	CModel* pMoverModelOld = m_pModel;
	const D3DXMATRIX matViewOld = g_matView;
	const D3DXMATRIX matInvViewOld = g_matInvView;

	m_matWorld = *pWorld;
	m_pModel = pModel;

	D3DXMatrixIdentity( &g_matInvView );
	for( std::vector<CSfx*>::iterator it = m_vecZckManagedSfx.begin(); it != m_vecZckManagedSfx.end(); ++it )
	{
		if( *it != NULL && ( *it )->IsDelete() == FALSE )
			( *it )->Render( pd3dDevice );
	}

	pSavedState->Apply();

	D3DXMATRIX matView;
	if( FAILED( pd3dDevice->GetTransform( D3DTS_VIEW, &matView ) ) )
		D3DXMatrixIdentity( &matView );

	g_matView = matView;
	if( D3DXMatrixInverse( &g_matInvView, NULL, &matView ) == NULL )
		D3DXMatrixIdentity( &g_matInvView );
	g_matInvView._41 = 0.0f;
	g_matInvView._42 = 0.0f;
	g_matInvView._43 = 0.0f;

	pd3dDevice->SetTransform( D3DTS_WORLD, pWorld );
	pd3dDevice->SetPixelShader( NULL );
	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetVertexDeclaration( NULL );

	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0 );
	pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
	pd3dDevice->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0x0000000F );
	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xFFFFFFFF );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	pd3dDevice->SetTexture( 1, NULL );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );

	RenderZckWeaponElementSfxViewport( pd3dDevice, pModel, pWorld );

	m_pModel = pMoverModelOld;
	m_matWorld = matWorldOld;
	g_matView = matViewOld;
	g_matInvView = matInvViewOld;

	pSavedState->Apply();
	pSavedState->Release();
}
#endif // __ZCK_SFX_AMELIORER && __CLIENT


#ifdef __CLIENT

// nValue ( 1,2,3,4 )
void CMover::RenderGauge( LPDIRECT3DDEVICE9 pd3dDevice, int nValue )
{
	// ¿ùµå ÁÂÇ¥¸¦ ½ºÅ©¸° ÁÂÇ¥·Î ÇÁ·ÎÁ§¼Ç ÇÑ´Ù.
	D3DXVECTOR3 vOut, vPos = GetScrPos(), vPosHeight;
    D3DVIEWPORT9 vp;
	const BOUND_BOX* pBB = m_pModel->GetBBVector();

    pd3dDevice->GetViewport( &vp );
	vp.X = 0;
	vp.Y = 0;

	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y, vPos.z);
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matScale );
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matRotation );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );
	
	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;
	
	D3DXVec3Project( &vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
		&GetWorld()->m_pCamera->m_matView, &matWorld);

	vOut.y -= 40;

	int nGaugeWidth = 80;
	CPoint point( (LONG)( vOut.x - nGaugeWidth / 2 ), (LONG)( vOut.y - 3 ) );
	CRect rect( point.x - 2, point.y - 2, point.x + nGaugeWidth + 2, point.y + 5 + 2);

	CPoint p = CPoint( rect.left, rect.top );
	LONG   y = 200;

	MakeEven( p.x );

	g_Neuz.m_TextureGauge[0].RenderScal(&(g_Neuz.m_2DRender), p, y, 0.8f, 0.55f);

	switch( nValue )
	{
		case 4:
			g_Neuz.m_TextureGauge[4].RenderScal(&(g_Neuz.m_2DRender), p, y, 0.8f, 0.55f);
		case 3:
			g_Neuz.m_TextureGauge[3].RenderScal(&(g_Neuz.m_2DRender), p, y, 0.8f, 0.55f);
		case 2:
			g_Neuz.m_TextureGauge[2].RenderScal(&(g_Neuz.m_2DRender), p, y, 0.8f, 0.55f);
		case 1:
			g_Neuz.m_TextureGauge[1].RenderScal(&(g_Neuz.m_2DRender), p, y, 0.8f, 0.55f);
			break;
		case 0:		// ±×¸®Áö ¾Ê´Â´Ù.
			break;
		default:
			FLASSERT( 0 );
			break;
	}
}


// ºñÇà½Ã ÅÍº¸±×·¡ÇÁ
void CMover::RenderTurboGauge( LPDIRECT3DDEVICE9 pd3dDevice, DWORD nColor, int nValue, int nMaxValue )
{
	if( IsMode( TRANSPARENT_MODE ) )
		return;
	if( !IsVisible() )
		return;
	//	pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	
	// ¿ùµå ÁÂÇ¥¸¦ ½ºÅ©¸° ÁÂÇ¥·Î ÇÁ·ÎÁ§¼Ç ÇÑ´Ù.
	D3DXVECTOR3 vOut, vPos = GetScrPos(), vPosHeight;
    D3DVIEWPORT9 vp;
	const BOUND_BOX* pBB = m_pModel->GetBBVector();
	
    pd3dDevice->GetViewport( &vp );
	vp.X = 0;
	vp.Y = 0;
	
	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y, vPos.z);
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matScale );
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matRotation );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );
	
	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;
	
	D3DXVec3Project( &vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
		&GetWorld()->m_pCamera->m_matView, &matWorld);
	
//	vOut.y -= 40;
	
	CPoint point( (LONG)( vOut.x - 80 / 2 ), (LONG)( vOut.y - 40 - 3 ) );		// °ÔÀÌÁö width 100
	CPoint p = CPoint( point.x - 2, point.y - 2 );

	MakeEven( p.x );
	
	g_Neuz.m_TextureTurboGauge[0].RenderScal(&(g_Neuz.m_2DRender), p, 200, 0.8f, 0.55f);		// °ÔÀÌÁö¹Ù Å×µÎ¸®(¿Í²Ù?)
	
	//int nHitWidth = nValue * nGaugeWidth / nMaxValue;
	
	int nWidth = g_Neuz.m_TextureTurboGauge[1].m_size.cx * nValue / nMaxValue;
	
	CPoint p2 = CPoint( nWidth, g_Neuz.m_TextureTurboGauge[1].m_size.cy );
	
	if( m_pActMover->GetStateFlag() & OBJSTAF_TURBO )
	{
		if( g_nRenderCnt & 1 )
		{
			g_Neuz.m_TextureTurboGauge[1].Render( &g_Neuz.m_2DRender, p, p2, 200, 0.8f, 0.55f);
		}
		else
		{
			int nWidthScale = (int)( (96 * nValue / nMaxValue) * 0.8f );
			CRect rect( p.x + 1, p.y + 1, p.x + nWidthScale, p.y + 6 );
			g_Neuz.m_2DRender.RenderFillRect( rect, 0xffffffff );
		}
		
	} else
		g_Neuz.m_TextureTurboGauge[1].Render( &g_Neuz.m_2DRender, p, p2, 200, 0.8f, 0.55f);

	// ¼Óµµ Ç¥½Ã.

	extern int g_nFlySpeed;
	char szBuff[64];
	int		nLen, i, nIdx;
	FLSPrintf( szBuff, _countof( szBuff ), "%d", g_nFlySpeed );
	nLen = strlen( szBuff );

//	vPosHeight = vPos;
	vPosHeight.x = 0;
	vPosHeight.z = 0;
	vPosHeight.y = 0;
	
	D3DXVec3Project( &vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
		&GetWorld()->m_pCamera->m_matView, &matWorld);
	

	// ¼ýÀÚÆùÆ® Âï±â.
	p.x = (LONG)( vOut.x + 30 );
	p.y = (LONG)( vOut.y + 0 );

	MakeEven( p.x );

	for( i = 0; i < nLen; i ++ )
	{
		nIdx = szBuff[i] - '0';
		g_WndMng.m_pWndWorld->m_texFontDigital.GetAt(nIdx)->Render( &g_Neuz.m_2DRender, p );		// ¼öÆò °ÔÀÌÁö.
		p.x += 16;
		
	}

	g_WndMng.m_pWndWorld->m_texFontDigital.GetAt(10)->Render( &g_Neuz.m_2DRender, p );		// ¼öÆò °ÔÀÌÁö.
	
	
}

void CMover::RenderQuestEmoticon( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( IsPlayer() || m_bShowQuestEmoticon == FALSE )
		return;
	LPCHARACTER lpCharacter = GetCharacter();

	if( lpCharacter == NULL ) return;

	// ¿ùµå ÁÂÇ¥¸¦ ½ºÅ©¸° ÁÂÇ¥·Î ÇÁ·ÎÁ§¼Ç ÇÑ´Ù.
	D3DXVECTOR3 vOut, vPos = GetScrPos(), vPosHeight;
    D3DVIEWPORT9 vp;
	const BOUND_BOX* pBB = m_pModel->GetBBVector();

    pd3dDevice->GetViewport( &vp );

	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y, vPos.z );
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matScale );
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matRotation );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );

	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;

	D3DXVec3Project( &vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
		&GetWorld()->m_pCamera->m_matView, &matWorld);
	//vOutFLOAT fHeight = ( vOut[0].y - vOut[4].y ) - 10f;
	CPoint point;
	point.x = (LONG)( vOut.x );
	point.y = (LONG)( vOut.y );

	MakeEven( point.x );
	//point.x -= pFont->GetTextExtent( szName ).cx / 2;
	point.y -= 50;

	//_SUN_RENDER_QUESTEMOTICON
	if( m_nQuestEmoticonIndex  > 0 )	
	{
		int nQuestEmoState = m_nQuestEmoticonIndex & 0xff;	// 1 ~ 4
		int nQuestIndex = m_nQuestEmoticonIndex >> 8;

		int nHead = _GetHeadQuest( nQuestIndex );
		int nOffset = _CalcQuestEmotionOffset( nHead, 5 );
		nQuestEmoState += nOffset;

        pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
        pd3dDevice->SetRenderState( D3DRS_FOGENABLE,        FALSE );
        pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
        pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

		point.x	= (LONG)( point.x - (16 * m_fQuestEmoticonScale) );
		point.y	= (LONG)( point.y - (16 * m_fQuestEmoticonScale) );
		g_Neuz.m_texQuestEmoticon.GetAt( nQuestEmoState )->RenderScal(&(g_Neuz.m_2DRender), point, 255, m_fQuestEmoticonScale, m_fQuestEmoticonScale );

		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,        TRUE );
	}
}

void CMover::RenderNameBox( LPDIRECT3DDEVICE9 pd3dDevice, const CRect& rectEA )
{
	//ÇÇ¾Æ½Äº°À» À§ÇØ ÇØ´ç¿µ¿ª¿¡ Ã¤¿öÁø »ç°¢ÇüÀ» ±×·ÁÁØ´Ù.
	if( EAPlayers::EA_NOTEAM != m_nEATeam )
	{
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

		if( EAPlayers::EA_REDTEAM == m_nEATeam )
			g_Neuz.m_2DRender.RenderFillRect( rectEA, 0xffff1111 );
		else if( EAPlayers::EA_BLUETEAM == m_nEATeam )
			g_Neuz.m_2DRender.RenderFillRect( rectEA, 0xff1111ff );
	}
}

#endif // __CLIENT

void CMover::RenderChrState(LPDIRECT3DDEVICE9 pd3dDevice)
{
	if( !IsVisible() )
		return;

	if( IsPlayer() )
	{
		if( IsCull() == FALSE )
		{
			// ¿ùµå ÁÂÇ¥¸¦ ½ºÅ©¸° ÁÂÇ¥·Î ÇÁ·ÎÁ§¼Ç ÇÑ´Ù.
			D3DXVECTOR3 vOut, vPos = GetScrPos(), vPosHeight;
			D3DVIEWPORT9 vp;
			const BOUND_BOX* pBB;
			
			if( m_pModel )
				pBB	= m_pModel->GetBBVector();
			else
				return;
			
			pd3dDevice->GetViewport( &vp );

			D3DXMATRIX matTrans;
			D3DXMATRIX matWorld;
			D3DXMatrixIdentity(&matWorld);
			D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y , vPos.z);
			
			D3DXMatrixMultiply( &matWorld, &matWorld, &GetMatrixScale() );
			D3DXMatrixMultiply( &matWorld, &matWorld, &GetMatrixRotation() );
			D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );
			
			vPosHeight = pBB->m_vPos[0];
			vPosHeight.x = 0;
			vPosHeight.z = 0;
			
			D3DXVec3Project( &vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
				&GetWorld()->m_pCamera->m_matView, &matWorld);
			
			CPoint point;
			point.x = (LONG)( vOut.x - 32 / 2 );
			point.y = (LONG)( vOut.y - 32 );
			
			MakeEven( point.x );			

			CTexture* pTexture = NULL;
			if( GetAdjParam( DST_CHRSTATE ) & CHS_STUN )
			{
				pTexture = g_DialogMsg.m_texEmoticon.GetAt(DLGEMOT_STUN);
			}

			if( pTexture )
			{
				g_Neuz.m_2DRender.RenderTexture( point, pTexture, 220 );
			}
		}
	}
}
//sun: 13, ´ÞÀÎ
LPCTSTR	CMover::GetTitle()
{
	//	100805	CSTRING GETBUFFER
	return m_strTitle.GetString();
}

void	CMover::SetTitle(LPCTSTR pTitle)
{
	m_strTitle = pTitle;
}

#ifdef __ZCK_STAFF_GRADE
namespace
{
	LPCTSTR GetZckStaffGradeTitle( DWORD dwAuthorization )
	{
		if( dwAuthorization >= AUTH_ADMINISTRATOR )
			return _T( "Admin" );

		if( dwAuthorization >= AUTH_OPERATOR )
			return _T( "Developer" );

		if( dwAuthorization >= AUTH_GAMEMASTER3 )
			return _T( "Head GM" );

		if( dwAuthorization >= AUTH_GAMEMASTER2 )
			return _T( "Gamemaster" );

		if( dwAuthorization >= AUTH_GAMEMASTER )
			return _T( "Gamemaster" );

		if( dwAuthorization >= AUTH_HELPER )
			return _T( "Support" );

		if( dwAuthorization >= AUTH_JOURNALIST )
			return _T( "Web-Dev" );

		if( dwAuthorization >= AUTH_LOGCHATTING )
			return _T( "Mod" );

		if( dwAuthorization == AUTH_OBSERVER )
			return _T( "Mod" );

		return _T( "" );
	}
}
#endif // __ZCK_STAFF_GRADE

void CMover::RenderName( LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont, DWORD dwColor )
{
#ifdef __ZCK_STAFF_GRADE
	const DWORD dwZckIncomingNameColor = dwColor;
#endif // __ZCK_STAFF_GRADE

	if( !IsVisible() )
		return;
	
	if( g_Option.m_nPlayerName == FALSE && IsActiveMover() )
		return;

	if( g_Option.m_nOtherPlayerName == FALSE && !IsActiveMover() && IsPlayer() )
		return;

	char szName[ 256 ];
	FLStrcpy( szName, _countof( szName ), m_szName );

	BOOL  bOtherColor = FALSE;
	DWORD dwNewColor;

	if( IsPlayer() )
	{
		if( GetWorld()->GetID() == WI_WORLD_GUILDWAR && g_pPlayer->GetGuild() && GetGuild() )
		{
			bOtherColor = TRUE;
			if( g_pPlayer->GetGuild()->GetGuildId() == GetGuild()->GetGuildId() )
				dwNewColor = COLOR_GUILDCOMBAT;		
			else
				dwNewColor = 0xffffffff;							
		}
		// ¸í¼º¿¡ µû¸¥ »öÇ¥½Ã.
//sun: 13, ´ÞÀÎ
		if( IsChaotic() )
			dwColor = prj.m_PKSetting.dwChaoColor;
		else if( IsPKPink() )
			dwColor = prj.m_PKSetting.dwReadyColor;
		else
			dwColor = prj.m_PKSetting.dwGeneralColor;

#ifdef __ZCK_STAFF_GRADE
		if( dwZckIncomingNameColor != 0xffffffff )
			dwColor = dwZckIncomingNameColor;
#endif // __ZCK_STAFF_GRADE
		
		CString strFameName = GetTitle();
		if( strFameName.IsEmpty() == FALSE )
		{
			CString strName;
			strName = "[";
			strName += strFameName;
			strName += "] ";
			strName += m_szName;
			FLStrcpy( szName, _countof( szName ), (LPCTSTR)strName );
		}
#ifdef __ZCK_STAFF_GRADE
		LPCTSTR lpszStaffTitle = GetZckStaffGradeTitle( m_dwAuthorization );
		if( lpszStaffTitle != NULL && lpszStaffTitle[0] != _T( '\0' ) )
		{
			CString strStaffName( szName );
			strStaffName += _T( " [" );
			strStaffName += lpszStaffTitle;
			strStaffName += _T( "]" );
			FLStrcpy( szName, _countof( szName ), (LPCTSTR)strStaffName );
		}

#endif // __ZCK_STAFF_GRADE
	}

	// ¿ùµå ÁÂÇ¥¸¦ ½ºÅ©¸° ÁÂÇ¥·Î ÇÁ·ÎÁ§¼Ç ÇÑ´Ù.
	D3DXVECTOR3 vOut, vPos, vPosHeight;

	/*
	//¼ÒÈ¯¼ö¸¸ GetScrPos·Î À§Ä¡¸¦ ±¸ÇÑ´Ù.
	MoverProp* pMoverProp = GetProp();
	if( pMoverProp && ( pMoverProp->dwAI == AII_PET || pMoverProp->dwAI == AII_EGG ) )
		vPos = GetScrPos();
	else
		vPos = GetPos();
	*/
	vPos = GetScrPos();
	
    D3DVIEWPORT9 vp;
	const BOUND_BOX* pBB = m_pModel->GetBBVector();

    pd3dDevice->GetViewport( &vp );

	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y, vPos.z );
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matScale );
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matRotation );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );

	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;

	D3DXVec3Project( &vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
		&GetWorld()->m_pCamera->m_matView, &matWorld);
	//vOutFLOAT fHeight = ( vOut[0].y - vOut[4].y ) - 10f;
	CPoint point;
	point.x = (LONG)( vOut.x );
	point.y = (LONG)( vOut.y );

	MakeEven( point.x );
	point.x -= pFont->GetTextExtent( szName ).cx / 2;
	point.y -= 20;
	point.x -= 3;
	
	// Æ¯¼ö Ä³¸¯ÅÍ ÀÌ¸§ Ãâ·Â 
	if( m_dwAuthorization >= AUTH_GAMEMASTER )
	{
		if( ( GetIndex() == OBJECT_INDEX( 203, MI_CROWNIBLIS ) || GetIndex() == OBJECT_INDEX( 204, MI_CROWNSHADE ) || GetIndex() == OBJECT_INDEX( 205, MI_CROWNBUBBLE ) ) )
		{
			FLStrcpy( szName, _countof( szName ), GetProp()->szName ); 
			point.x = (LONG)( vOut.x );
			point.y = (LONG)( vOut.y );
			MakeEven( point.x );
			point.x -= pFont->GetTextExtent( szName ).cx / 2;
			point.y -= 20;
			dwColor = 0xffa0a0ff;
		}
		else
		if( GetIndex() == OBJECT_INDEX( 201, MI_DWARPET ) || GetIndex() == OBJECT_INDEX( 202, MI_DWARPETMAS ) )
		{
			TCHAR szStr[ 64 ];
			dwColor = 0xffa0a0ff;
			FLSPrintf( szStr, _countof( szStr ), "[%s]", GetProp()->szName );
			point.x = (LONG)( vOut.x );
			point.y = (LONG)( vOut.y );
			MakeEven( point.x );
			point.x -= pFont->GetTextExtent( szStr ).cx / 2;
			point.y -= 35;
			pFont->DrawText( (FLOAT)( point.x ), (FLOAT)( point.y ), dwColor, szStr );
			point.x = (LONG)( vOut.x );
			point.y = (LONG)( vOut.y );
			MakeEven( point.x );
			point.x -= pFont->GetTextExtent( szName ).cx / 2;
			point.y -= 20;
		}
	}

#ifdef __CLIENT
	if( IsPlayer() )
	{
		if( dwColor == prj.m_PKSetting.dwGeneralColor )		//sun: 8, // __S8_PK // ÇÎÅ©»öÀÌ³ª Ä«¿À»öÀÌ¸é ¹Ù²îÁö ¾Ê°Ô ÇÏÀÚ
		if( g_Party.FindMember( m_idPlayer ) != -1 )		// ÆÄÆ¼¸â¹ö¸é »ö±ò ´Ù¸£°Ô Ç¥½Ã.
			dwColor = COLOR_PARTY;
		
		if( bOtherColor )
			dwColor = dwNewColor;

		//ÀÌº¥Æ® ¾Æ·¹³ª ÇÇ¾Æ½Äº°
		if( EAPlayers::EA_NOTEAM != m_nEATeam )
		{
			LPCTSTR strName = GetName();
			CSize kSize = pFont->GetTextExtent( strName );

			CRect rectForEA( point.x -5, point.y , point.x + kSize.cx + 10, point.y + 18 );
			RenderNameBox( pd3dDevice, rectForEA );
		}

		pFont->DrawText( (FLOAT)( point.x ), (FLOAT)( point.y ), dwColor, szName );
		
		if( IsMode( EVENT_OLDBOY_MODE ) )
		{
			int nWidth = pFont->GetTextExtent( szName ).cx;
			pFont->DrawText( (FLOAT)( point.x+nWidth ), (FLOAT)( point.y ), D3DCOLOR_XRGB( 0, 200, 0 ), " ¡Ú" );
		}
	}
	else
#endif
	{
		if( g_Option.m_nMonName )
		{				
			LPCHARACTER lpCharacter = GetCharacter();
			if( lpCharacter && lpCharacter->m_nStructure != -1 ) 
			{
				LPSTRUCTURE lpStr = &prj.m_aStructure[ lpCharacter->m_nStructure ];
				TCHAR szStr[ 64 ];
				FLSPrintf( szStr, _countof( szStr ), "[%s]", lpStr->szName );
				point.x = (LONG)( vOut.x );
				point.y = (LONG)( vOut.y );
				MakeEven( point.x );
				point.x -= pFont->GetTextExtent( szStr ).cx / 2;
				point.y -= 35;
				pFont->DrawText( (FLOAT)( point.x ), (FLOAT)( point.y ), COLOR_NPC, szStr );
				point.x = (LONG)( vOut.x );
				point.y = (LONG)( vOut.y );
				MakeEven( point.x );
				point.x -= pFont->GetTextExtent( szName ).cx / 2;
				point.y -= 20;
			}
			//sprintf( szNameTemp, "NPC %s", m_szName );
			//pFont->DrawText( vOut.x + 1, vOut.y + 1, 0xff000000, szName );
			if( IsPeaceful() )
			{
				pFont->DrawText( (FLOAT)( point.x ), (FLOAT)( point.y ), COLOR_NPC, szName );
			}
			else
			{
#ifdef __ZCK_WORLD_BOSS
				if( m_bWorldBoss )
				{
					pFont->DrawText( (FLOAT)( point.x ), (FLOAT)( point.y ), COLOR_WORLDBOSS, szName );
				}
				else
#endif // __ZCK_WORLD_BOSS
				if( m_bActiveAttack )	// ¼±°ø ¸ó½ºÅÍ´Â ±ôºý°Å¸°´Ù.
				{
					pFont->DrawText( (FLOAT)( point.x ), (FLOAT)( point.y ), 0xffff0000, szName );						
				} else
				{
					pFont->DrawText( (FLOAT)( point.x ), (FLOAT)( point.y ), COLOR_MONSTER, szName );
				}
			}
		}
	}
	// ÇÃ·¹ÀÌ¾î°¡ °×¸¶¸é µð¹ö±ë Á¤º¸¸¦ º¸¿©ÁØ´Ù.
	if( GetWorld()->m_bViewIdState )
	{
		char szBuff[256];
		CString strName;
		CMover *pAttacker = prj.GetMover( m_idAttacker );
		if( pAttacker )
			strName = pAttacker->m_szName;
		FLSPrintf( szBuff, _countof( szBuff ), "0x%x %08x, %d %d, %d(%s)", m_pActMover->GetStateFlag(), m_pActMover->GetState(), GetId(), m_pActMover->m_bGround, (int)m_idAttacker, strName );
		pFont->DrawText( vOut.x, vOut.y - 50, dwColor, szBuff );
		
	}
	
// Render Guild Name & Logo
	CGuild* pGuild = GetGuild();	

	BOOL bSkip = FALSE;
	// ¿î¿µÀÚÀÌ°í º¯½ÅÁßÀÌ¸é ±æµå¸í°ú ·Î°í¸¦ ¾È±×¸°´Ù.
	if( IsAuthHigher( AUTH_GAMEMASTER ) == TRUE )
	{
		if( HasBuffByIk3(IK3_TEXT_DISGUISE) )
			bSkip = TRUE;
	}
	
//sun: 9Â÷ Àü½Â°ü·Ã Clienet
	//Àü½Â ¹× ¿µ¿õ ¾ÆÀÌÄÜ Ç¥½Ã Ãß°¡.
	point.x = (LONG)( vOut.x );
	point.y = (LONG)( vOut.y );
	MakeEven( point.x );
	//pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	
	if(!bSkip && IsPlayer())
	{
		BYTE checkhero = GetLegendChar();
		CTexture* pTexture;
		CString strPath;
		CSize cs1 = pFont->GetTextExtent( szName );
		CSize cs2;
		
		if(pGuild != NULL)
		{
			cs2 = pFont->GetTextExtent( pGuild->m_szGuild );

			if( cs1.cx > cs2.cx )
				point.x += cs1.cx / 2;
			else
				point.x += cs2.cx / 2;
		}
		else
			point.x += cs1.cx / 2;
	
		if(checkhero == LEGEND_CLASS_MASTER && m_nLevel >= 60 && m_nLevel <= 120) //Àü½ÂÀ» ÇßÀ» °æ¿ì.
		{
			if(/*m_nLevel >= 60 && */m_nLevel < 70) //Level DownµÉ °æ¿ì¸¦ »ý°¢ÇØ¼­ ÁÖ¼®Ã³¸®.
			//	strPath = MakePath( DIR_ICON, "icon_Expert1.dds");
				pTexture = g_Neuz.m_pMasterIcon[0];
			else if(m_nLevel >= 70 && m_nLevel < 80)
			//	strPath = MakePath( DIR_ICON, "icon_Expert2.dds");
				pTexture = g_Neuz.m_pMasterIcon[1];
			else if(m_nLevel >= 80 && m_nLevel < 90)
			//	strPath = MakePath( DIR_ICON, "icon_Expert3.dds");
				pTexture = g_Neuz.m_pMasterIcon[2];
			else if(m_nLevel >= 90 && m_nLevel < 100)
			//	strPath = MakePath( DIR_ICON, "icon_Expert4.dds");
				pTexture = g_Neuz.m_pMasterIcon[3];
			else if(m_nLevel >= 100 && m_nLevel < 110)
			//	strPath = MakePath( DIR_ICON, "icon_Expert5.dds");
				pTexture = g_Neuz.m_pMasterIcon[4];
			else if(m_nLevel >= 110 && m_nLevel <= 120)
			//	strPath = MakePath( DIR_ICON, "icon_Expert6.dds");
				pTexture = g_Neuz.m_pMasterIcon[5];

			//pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
			if(pTexture != NULL)
			{
				point.y -= pTexture->m_size.cy + 5;
				pTexture->Render( &g_Neuz.m_2DRender, point );
			}
		}
		else if(checkhero == LEGEND_CLASS_HERO) //¿µ¿õÀÏ °æ¿ì.
		{
			//strPath = MakePath( DIR_ICON, "icon_Hero.dds");
			//pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
			pTexture = g_Neuz.m_pHeroIcon;
			if(pTexture != NULL)
			{
				point.y -= pTexture->m_size.cy + 5;
				pTexture->Render( &g_Neuz.m_2DRender, point );
			}
		}
		else if( checkhero == LEGEND_CLASS_LEGEND_HERO )
		{
			pTexture = g_Neuz.m_pHeroIcon;
			if(pTexture != NULL)
			{
				point.y -= pTexture->m_size.cy + 5;
				pTexture->Render( &g_Neuz.m_2DRender, point );
			}
		}
	}

	//pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	if( pGuild && bSkip == FALSE )
	{
		point.x = (LONG)( vOut.x );
		point.y = (LONG)( vOut.y );
		MakeEven( point.x );	
		point.x -= pFont->GetTextExtent( pGuild->m_szGuild ).cx / 2;
		point.y -= 32;
		point.x -= 3;

		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );				
		// ±æµå¸¶½ºÅÍ¸é ±æµå¸íÀ» ³ë¶õ»öÀ¸·Î Ãâ·Â
		dwColor = 0xFFFFFFFF;
#ifdef __GUILD_COLOR
		if( pGuild->m_nLevel >= 60 )
        	dwColor = COLOR_GUILD_FARBE_60;
    	else if( pGuild->m_nLevel >= 50 )
        	dwColor = COLOR_GUILD_FARBE_50;    
    	else if( pGuild->m_nLevel >= 40 )
        	dwColor = COLOR_GUILD_FARBE_40;    
   	 	else if( pGuild->m_nLevel >= 30 )
        	dwColor = COLOR_GUILD_FARBE_30;    
    	else if( pGuild->m_nLevel >= 20 )
        	dwColor = COLOR_GUILD_FARBE_20;    
    	else if( pGuild->m_nLevel >= 10 )
        	dwColor = COLOR_GUILD_FARBE_10;    
#endif // __GUILD_COLOR 		
		if( pGuild->IsMaster(m_idPlayer))
			dwColor = 0xFFFFFF99;
		else
		if( bOtherColor )
			dwColor = dwNewColor;

		// ±æµåÄÄ¹îÀÌ ¿­¸°»óÅÂÀÌ¸é¼­ µðÆæ´õ´Â ºÐÈ«»öÀ¸·Î Ãâ·Â
		if( g_GuildCombatMng.m_nGCState >= CGuildCombat::MAINTENANCE_STATE )
		{
			if( pWndWorld && pWndWorld->GetGCStatusDefender(m_idPlayer) != -1 )
			{
				dwColor = 0xFFFEA8FF;
			}		
		}

		// º£½ºÆ® ÇÃ·¹ÀÌ¾î Ç¥½Ã - ¿ì½Â±æµåÀÌ°í º£½ºÆ® ÇÃ·¹ÀÌ¾î¸é º£½ºÆ® ÇÃ·¹ÀÌ¾î¸¸ Ç¥½Ã
		// ¿ì½Â ±æµå´Â ±æµå·Î°í ¾Õ¿¡ Ç¥½ÃÇØÁÜ
		BOOL bRenderCombatBestLogo = FALSE;		
		BOOL bRenderCombatWinLogo  = FALSE;

		// ±æµå´ëÀüÀÌ ¿­¸®Áö ¾Ê¾ÒÀ»¶§¸¸ Ãâ·ÂÇØÁÜ
		if( g_GuildCombatMng.m_nState == CGuildCombat::CLOSE_STATE )
		{
			if( g_GuildCombatMng.m_uBestPlayer == m_idPlayer )
				bRenderCombatBestLogo = TRUE;
			else
			if( g_GuildCombatMng.m_uWinGuildId == pGuild->GetGuildId() )
				bRenderCombatWinLogo = TRUE;
		}

		pFont->DrawText( (FLOAT)( point.x ), (FLOAT)( point.y ), dwColor, pGuild->m_szGuild );	
		
		if( pGuild->m_dwLogo != 0 )
		{
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );

			int nLogoSizey = pWndWorld->m_pTextureLogo[pGuild->m_dwLogo-1].m_size.cy+5;
		
			point.x = (LONG)( vOut.x );
			point.y = (LONG)( vOut.y );
			
			MakeEven( point.x );	
			
			CSize cs1 = pFont->GetTextExtent( szName );
			CSize cs2 = pFont->GetTextExtent( pGuild->m_szGuild );
				
			if( cs1.cx > cs2.cx )
			{
				point.x -= cs1.cx / 2;
				point.y -= nLogoSizey;
			}
			else
			{
				point.x -= cs2.cx / 2;
				point.y -= nLogoSizey;
			}
			
			point.x -= pWndWorld->m_pTextureLogo[pGuild->m_dwLogo-1].m_size.cx+2;
			pWndWorld->m_pTextureLogo[pGuild->m_dwLogo-1].Render( &g_Neuz.m_2DRender, point, 255 );
			
			D3DDEVICE->SetRenderState( D3DRS_DESTBLEND ,D3DBLEND_INVSRCALPHA );			
			
			if( bRenderCombatBestLogo )
			{
				point.x -= pWndWorld->m_pTextureLogo[pGuild->m_dwLogo-1].m_size.cx+2;
				pWndWorld->m_TexGuildBest.Render( &g_Neuz.m_2DRender, point, 255 );
			}	
			else
			if( bRenderCombatWinLogo )
			{
				point.x -= pWndWorld->m_pTextureLogo[pGuild->m_dwLogo-1].m_size.cx+2;
				pWndWorld->m_TexGuildWinner.Render( &g_Neuz.m_2DRender, point, 255 );
			}
#ifdef NEW_GUILD_WINLOG
			point.x -= pWndWorld->m_pTextureLogo[pGuild->m_dwLogo-1].m_size.cx+2;
			pWndWorld->m_TexGuildHoner.Render( &g_Neuz.m_2DRender, point, 255 );
#endif // NEW_GUILD_WINLOG
		}
		else
		{
			// ±æµå·Î°í°¡ ¼³Á¤¾ÈµÈ »óÅÂÀÌÁö¸¸ ¿ì½ÂÇÑ ±æµåÀÏ°æ¿ì
			if( bRenderCombatBestLogo || bRenderCombatWinLogo )
			{
				D3DDEVICE->SetRenderState( D3DRS_DESTBLEND ,D3DBLEND_INVSRCALPHA );
				int nLogoSizey = 30;
				
				point.x = (LONG)( vOut.x );
				point.y = (LONG)( vOut.y );
				
				MakeEven( point.x );			
				
				if( strlen(szName) > strlen(pGuild->m_szGuild) )
				{
					point.x -= pFont->GetTextExtent( szName ).cx / 2;
					point.y -= nLogoSizey;
				}
				else
				{
					point.x -= pFont->GetTextExtent( pGuild->m_szGuild ).cx / 2;
					point.y -= nLogoSizey;
				}
				
				point.x -= nLogoSizey;
				
				if( bRenderCombatBestLogo )
					pWndWorld->m_TexGuildBest.Render( &g_Neuz.m_2DRender, point, 255 );
				else
				if( bRenderCombatWinLogo )
					pWndWorld->m_TexGuildWinner.Render( &g_Neuz.m_2DRender, point, 255 );
#ifdef NEW_GUILD_WINLOG
				static int pointX= 20, pointY = 60;
				point.x -= pointX;
				point.y -= pointY;
				pWndWorld->m_TexGuildHoner.Render( &g_Neuz.m_2DRender, point, 255 );
#endif // NEW_GUILD_WINLOG
			}				
		}
		

		if( IsPlayer() && !IsActiveMover() && g_Neuz.m_NeuzEnemy.IsPKing( GetId() ) )
		{
			if( (m_dwFlag & MVRF_WARMARK) == 0 )	// ÀüÀï¸¶Å©°¡ ¾ÆÁ÷ ¾È¶°ÀÖ´Â »óÅÂ¸é
			{
				RenderFlag( ENEMY_PK );		// »ý¼º ÀüÅõ SFX
			}
		}
		else
#ifdef __CLIENT
		// ±æµåÀü½Ã¿¡ ¸Ó¸®À§¿¡ ¶ß´Â ÀüÅõ¸¶Å©
		if( g_eLocal.GetState( EVE_GUILDWAR ) == 1 )		// ±æµåÀü °¡´ÉÇÑ ¼­¹öÀÎ°¡.
		{
			if( m_idWar && g_pPlayer->m_idWar == m_idWar )	// ³ª¶û °°Àº ÀüÀï¿¡ Âü°¡ÁßÀÎ ³ðÀÎ°¡.
			{
				if( m_idGuild != g_pPlayer->m_idGuild )		// ³ª¶û ´Ù¸¥±æµåÀÎ°¡(µ¿¸ÍÀÎ°æ¿ì´Â ÀÌ°É·Î ¾ÈµÈ´Ù.)
				{
					if( (m_dwFlag & MVRF_WARMARK) == 0 )	// ÀüÀï¸¶Å©°¡ ¾ÆÁ÷ ¾È¶°ÀÖ´Â »óÅÂ¸é
					{
						RenderFlag( ENEMY_WAR );
					}
				} 
			}
		}
#endif // CLIENT
	}
#ifdef __ZCK_WORLD_BOSS
	if( m_bWorldBoss )
	{
		CString strWorldBoss;
		strWorldBoss.Format( _T( "[World Boss]" ) );
		point.x = static_cast<LONG>( vOut.x );
		point.y = static_cast<LONG>( vOut.y );
		MakeEven( point.x );
		point.x -= pFont->GetTextExtent( strWorldBoss ).cx / 2;
		point.y -= 32;
		point.x -= 3;
		pFont->DrawText( static_cast<FLOAT>( point.x ), static_cast<FLOAT>( point.y ), COLOR_WORLDBOSS, strWorldBoss );
	}
#endif // __ZCK_WORLD_BOSS
}

// ÀüÅõ±³ÀüÁßÀÎ »óÅÂ Ç¥½Ã
void CMover::RenderFlag( int nType )
{
	// °áÅõ»ó´ë¶ó´Â sfx¸¦ ¸Ó¸®À§¿¡ ¶ç¿öÁÜ.
	CSfx *pSfx = CreateSfx( D3DDEVICE, XI_INDEX( 1700, XI_GEN_PVP_FLAG01 ), GetPos(), GetId(), D3DXVECTOR3(0,0,0), NULL_ID, -1 );
	if( pSfx )
	{
		pSfx->SetScale( D3DXVECTOR3(1.5f, 1.5f, 1.5f) );
		((CSfxDuelParty*)pSfx)->m_nType = nType;
		m_dwFlag |= MVRF_WARMARK;
	}
}
	
void CMover::RenderHP(LPDIRECT3DDEVICE9 pd3dDevice)
{
	if( !IsVisible() )
		return;
	
	if( IsMode( TRANSPARENT_MODE ) )
		return;

	if( g_Option.m_nPlayerName == FALSE && IsActiveMover() )
		return;
	
	if( g_Option.m_nOtherPlayerName == FALSE && !IsActiveMover() && IsPlayer() )
		return;

	if( g_WorldMng()->m_bViewHP == FALSE )
		return;

	// ¿î¿µÀÚÀÌ°í º¯½ÅÁßÀÌ¸é HP¹Ù ¾È±×¸°´Ù.
	if( IsAuthHigher( AUTH_GAMEMASTER ) == TRUE )
	{
		if( IsPlayer() && HasBuffByIk3(IK3_TEXT_DISGUISE) )
			return;
	}

	if( !IsActiveObj() && ( m_dwMode & QUIZ_RENDER_SKIP_MODE ) )
		return;
	
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	// ¿ùµå ÁÂÇ¥¸¦ ½ºÅ©¸° ÁÂÇ¥·Î ÇÁ·ÎÁ§¼Ç ÇÑ´Ù.
	D3DXVECTOR3		vOut( 0.0f, 0.0f, 0.0f ), 
					vPos( 0.0f, 0.0f, 0.0f ),
					vPosHeight( 0.0f, 0.0f, 0.0f );

	D3DVIEWPORT9	vp;
	ZeroMemory( &vp, sizeof( D3DVIEWPORT9 ) );
	pd3dDevice->GetViewport( &vp );
	vp.X = 0;
	vp.Y = 0;

	/*
	//¼ÒÈ¯¼ö¸¸ GetScrPos·Î À§Ä¡¸¦ ±¸ÇÑ´Ù.
	MoverProp* pMoverProp = GetProp();
	if( pMoverProp && ( pMoverProp->dwAI == AII_PET || pMoverProp->dwAI == AII_EGG ) )
		vPos = GetScrPos();
	else
		vPos = GetPos();
	*/
	vPos = GetScrPos();

	const BOUND_BOX* pBB = m_pModel->GetBBVector();
	if( pBB == NULL )
		return ;

	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matTrans);
	D3DXMatrixIdentity(&matWorld);

	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y, vPos.z);
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matScale );
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matRotation );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );
	
	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;
	
	D3DXVec3Project( &vOut, &vPosHeight, &vp, &GetWorld()->m_matProj, &GetWorld()->m_pCamera->m_matView, &matWorld );
	
	vOut.y -= 34;
	int nGaugeWidth = 80;
	CPoint point( (int)( vOut.x - nGaugeWidth / 2 ), (int)( vOut.y - 3 ) );
	CRect rect( point.x - 2, point.y - 2, point.x + nGaugeWidth + 2, point.y + 5 + 2);
	
	CPoint p = CPoint( rect.left, rect.top );
	MakeEven( p.x );

	BOOL bDbuff = FALSE;

	if( IsStateDbuff() )
		bDbuff = TRUE;

	g_Neuz.m_TextureHPGauge[0].RenderScal(&(g_Neuz.m_2DRender), p, 200, 0.8f, 0.55f);		
	
	int nCurrentHP( GetHitPoint() );
	int nCurrentMaxHP( GetMaxHitPoint() );

	if( nCurrentHP > nCurrentMaxHP )
		nCurrentMaxHP = nCurrentHP;

	int nWidth = g_Neuz.m_TextureHPGauge[1].m_size.cx * nCurrentHP / nCurrentMaxHP;
	
	CPoint p2 = CPoint( nWidth, g_Neuz.m_TextureHPGauge[1].m_size.cy );
	
	if( bDbuff )
		g_Neuz.m_TextureHPGauge[2].Render(&(g_Neuz.m_2DRender), p, p2, 200, 0.8f, 0.55f);		
	else
		g_Neuz.m_TextureHPGauge[1].Render(&(g_Neuz.m_2DRender), p, p2, 200, 0.8f, 0.55f);				

	if(m_pAngelFlag)		//sun: 8Â÷ ¿£Á© ¼ÒÈ¯ Neuz, World, Trans
		RenderAngelStatus( pd3dDevice );
}

void CMover::RenderCltGauge(LPDIRECT3DDEVICE9 pd3dDevice)
{

	if( !IsVisible())	return;
	
	if(!IsCollecting()) return;

	if( g_Option.m_nOtherPlayerName == FALSE && !IsActiveMover() && IsPlayer() )
		return;

	// ¿ùµå ÁÂÇ¥¸¦ ½ºÅ©¸° ÁÂÇ¥·Î ÇÁ·ÎÁ§¼Ç ÇÑ´Ù.
	D3DXVECTOR3 vOut, vPos, vPosHeight;
	D3DVIEWPORT9 vp;

	MoverProp* pMoverProp = GetProp();
	vPos = GetScrPos();

	const BOUND_BOX* pBB = m_pModel->GetBBVector();
	
	pd3dDevice->GetViewport( &vp );
	vp.X = 0;
	vp.Y = 0;
	
	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y, vPos.z);
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matScale );
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matRotation );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );
	
	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x  = 0;
	vPosHeight.z  = 0;
	
	D3DXVec3Project( &vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
		&GetWorld()->m_pCamera->m_matView, &matWorld);
	
	vOut.y -= 48;
	int nGaugeWidth = 80;
	CPoint point( (int)( vOut.x - nGaugeWidth / 2 ), (int)( vOut.y - 3 ) );
	CRect rect( point.x - 2, point.y - 2, point.x + nGaugeWidth + 2, point.y + 5 + 2);
	
	CPoint p = CPoint( rect.left, rect.top );
	MakeEven( p.x );

	g_Neuz.m_TexCltGauge[0].RenderScal(&(g_Neuz.m_2DRender), p, 255, 0.8f, 0.55f);		
	
	int nWidth = g_Neuz.m_TexCltGauge[1].m_size.cx * m_nCltTime / m_nMaxCltTime;
	
	CPoint p2 = CPoint( nWidth, g_Neuz.m_TexCltGauge[1].m_size.cy );
	
	g_Neuz.m_TexCltGauge[1].Render(&(g_Neuz.m_2DRender), p, p2, 255, 0.8f, 0.55f);	

}

//sun: 8Â÷ ¿£Á© ¼ÒÈ¯ Neuz, World, Trans
void CMover::RenderAngelStatus(LPDIRECT3DDEVICE9 pd3dDevice)
{
	if(m_pAngel == NULL)
		return;
	
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	// ¿ùµå ÁÂÇ¥¸¦ ½ºÅ©¸° ÁÂÇ¥·Î ÇÁ·ÎÁ§¼Ç ÇÑ´Ù.
	D3DXVECTOR3 vOut, vPos, vPosHeight;
	vPos = m_AngelPos;
	D3DVIEWPORT9 vp;
	
	pd3dDevice->GetViewport( &vp );
	vp.X = 0;
	vp.Y = 0;
	
	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y, vPos.z);
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matScale );
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matRotation );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );
	
	matWorld._41 = m_AngelPos.x;
	matWorld._42 = m_AngelPos.y;
	matWorld._43 = m_AngelPos.z;

	vPosHeight.y = 1.4f;
	vPosHeight.x = 0;
	vPosHeight.z = 0;
	
	D3DXVec3Project( &vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
		&GetWorld()->m_pCamera->m_matView, &matWorld);
	
	int nGaugeWidth = 40;
	CPoint point( (int)( vOut.x - nGaugeWidth / 2 ), (int)( vOut.y - 3 ) );
	CRect rect( point.x - 2, point.y - 2, point.x + nGaugeWidth + 2, point.y + 5 + 2);
	
	CPoint p = CPoint( rect.left, rect.top );
	MakeEven( p.x );
	
	g_Neuz.m_TextureAngelGauge[0].RenderScal(&(g_Neuz.m_2DRender), p, 200, 0.4f, 0.55f);
	
	int nAngel = 100;
	IBuff* pBuff	= m_buffs.GetBuffByIk3( IK3_ANGEL_BUFF );
	if( pBuff )
	{
		PT_ITEM_SPEC pItemProp = pBuff->GetSpecItem();
		if( pItemProp )
			nAngel = (int)( (float)pItemProp->nAdjParamVal[0] );	// TODO : È¿°ú Àû¿ë ¼öÄ¡·Î »©ÀÚ
	}

	if( nAngel <= 0 || 100 < nAngel  )
		nAngel = 100;
	
	EXPINTEGER maxExp = prj.m_aExpCharacter[m_nAngelLevel].nExp1 / 100 * nAngel;
	if( maxExp > 0)
	{
		int nWidth = (int)( g_Neuz.m_TextureAngelGauge[1].m_size.cx * m_nAngelExp / maxExp );
		CPoint p2 = CPoint( nWidth, g_Neuz.m_TextureAngelGauge[1].m_size.cy );
		g_Neuz.m_TextureAngelGauge[1].Render(&(g_Neuz.m_2DRender), p, p2, 200, 0.4f, 0.55f);		
	}
}


void CMover::RenderCasting(LPDIRECT3DDEVICE9 pd3dDevice)
{
	if( g_pPlayer->IsStateMode( STATE_BASEMOTION_MODE ) == FALSE )
		return;
	if( g_pPlayer->m_nReadyTime == 0 )
		return;
	if( !IsVisible() )
		return;
	
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	// ¿ùµå ÁÂÇ¥¸¦ ½ºÅ©¸° ÁÂÇ¥·Î ÇÁ·ÎÁ§¼Ç ÇÑ´Ù.
	D3DXVECTOR3 vOut, vPos = GetScrPos(), vPosHeight;
	D3DVIEWPORT9 vp;
	const BOUND_BOX* pBB = m_pModel->GetBBVector();
	
	pd3dDevice->GetViewport( &vp );
	vp.X = 0;
	vp.Y = 0;
	
	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y, vPos.z);
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matScale );
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matRotation );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );
	
	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;
	
	D3DXVec3Project( &vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
		&GetWorld()->m_pCamera->m_matView, &matWorld);
	
	vOut.y -= 48;
	int nGaugeWidth = 80;

	CPoint point( (int)( vOut.x - nGaugeWidth / 2 ), (int)( vOut.y - 3 ) );
	CRect rect( point.x - 2, point.y - 2, point.x + nGaugeWidth + 2, point.y + 5 + 2);
	
	CPoint p = CPoint( rect.left, rect.top );
		
	MakeEven( p.x );
	
	g_Neuz.m_TextureCastingGauge[0].RenderScal(&(g_Neuz.m_2DRender), p, 200, 0.8f, 0.55f);		
	
	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( g_pPlayer->m_dwUseItemId );
	if( pItemProp )
	{
		DWORD dwTime = timeGetTime() - g_pPlayer->m_nReadyTime;
		int nWidth = (int)( g_Neuz.m_TextureHPGauge[1].m_size.cx * ( (float)dwTime / (float)pItemProp->dwSkillReadyType ) );
		if( 112 < nWidth )
			nWidth = 112;
		if( nWidth < 0 )
			nWidth = 0;

		CPoint p2 = CPoint( nWidth, g_Neuz.m_TextureHPGauge[1].m_size.cy );
	
		g_Neuz.m_TextureCastingGauge[1].Render(&(g_Neuz.m_2DRender), p, p2, 255, 0.8f, 0.55f);		
	}
}

void CMover::RenderCtrlCasting(LPDIRECT3DDEVICE9 pd3dDevice)
{
//	if( g_pPlayer->IsStateMode( STATE_BASEMOTION_MODE ) == FALSE )
//		return;
	if( g_pPlayer->m_dwCtrlReadyTime  == 0xffffffff )
		return;
	if( !IsVisible() )
		return;
	
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	// ¿ùµå ÁÂÇ¥¸¦ ½ºÅ©¸° ÁÂÇ¥·Î ÇÁ·ÎÁ§¼Ç ÇÑ´Ù.
	D3DXVECTOR3 vOut, vPos = GetScrPos(), vPosHeight;
	D3DVIEWPORT9 vp;
	const BOUND_BOX* pBB = m_pModel->GetBBVector();
	
	pd3dDevice->GetViewport( &vp );
	vp.X = 0;
	vp.Y = 0;
	
	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y, vPos.z);
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matScale );
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matRotation );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );
	
	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;
	
	D3DXVec3Project( &vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
		&GetWorld()->m_pCamera->m_matView, &matWorld);
	
	vOut.y -= 48;
	int nGaugeWidth = 80;
	
	CPoint point( (int)( vOut.x - nGaugeWidth / 2 ), (int)( vOut.y - 3 ) );
	CRect rect( point.x - 2, point.y - 2, point.x + nGaugeWidth + 2, point.y + 5 + 2);
	
	CPoint p = CPoint( rect.left, rect.top );
	
	MakeEven( p.x );
	
	g_Neuz.m_TextureCastingGauge[0].RenderScal(&(g_Neuz.m_2DRender), p, 200, 0.8f, 0.55f);		
	
//	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( g_pPlayer->m_dwUseItemId );
//	if( pItemProp )
	{
		DWORD dwTime =  OPEN_COOLTIME - (g_pPlayer->m_dwCtrlReadyTime - timeGetTime()) ;
		int nWidth = (int)( g_Neuz.m_TextureHPGauge[1].m_size.cx * ((float)dwTime / OPEN_COOLTIME ) );
		if( 112 < nWidth )
			nWidth = 112;
		if( nWidth < 0 )
			nWidth = 0;
		
		CPoint p2 = CPoint( nWidth, g_Neuz.m_TextureHPGauge[1].m_size.cy );
		
		g_Neuz.m_TextureCastingGauge[1].Render(&(g_Neuz.m_2DRender), p, p2, 255, 0.8f, 0.55f);		
	}
}

//sun: 10Â÷ Àü½Â½Ã½ºÅÛ	Neuz, World, Trans
void CMover::RenderSkillCasting(LPDIRECT3DDEVICE9 pd3dDevice)
{
//	if( g_pPlayer->IsStateMode( STATE_BASEMOTION_MODE ) == FALSE )
//		return;
	DWORD dwtmpTick = GetTickCount();
	DWORD dwTmpCastingEndTick = g_pPlayer->m_pActMover->GetCastingEndTick();
	if( dwTmpCastingEndTick < dwtmpTick )
		return;

	int dwTmpSkillID = g_pPlayer->m_pActMover->GetCastingSKillID();
	if( dwTmpSkillID != SKILL_INDEX( 238, SI_KNT_HERO_DRAWING ) &&  dwTmpSkillID != SKILL_INDEX( 244, SI_RIG_HERO_RETURN ) )
		return;

	if( !IsVisible() )
		return;

	DWORD dwTmpCastingTick = (DWORD)g_pPlayer->m_pActMover->GetCastingTick();

	
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	// ¿ùµå ÁÂÇ¥¸¦ ½ºÅ©¸° ÁÂÇ¥·Î ÇÁ·ÎÁ§¼Ç ÇÑ´Ù.
	D3DXVECTOR3 vOut, vPos = GetScrPos(), vPosHeight;
	D3DVIEWPORT9 vp;
	const BOUND_BOX* pBB = m_pModel->GetBBVector();
	
	pd3dDevice->GetViewport( &vp );
	vp.X = 0;
	vp.Y = 0;
	
	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y, vPos.z);
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matScale );
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matRotation );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );
	
	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;
	
	D3DXVec3Project( &vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
		&GetWorld()->m_pCamera->m_matView, &matWorld);
	
	vOut.y -= 48;
	int nGaugeWidth = 80;
	
	CPoint point( (int)( vOut.x - nGaugeWidth / 2 ), (int)( vOut.y - 3 ) );
	CRect rect( point.x - 2, point.y - 2, point.x + nGaugeWidth + 2, point.y + 5 + 2);
	
	CPoint p = CPoint( rect.left, rect.top );
	
	MakeEven( p.x );
	
	g_Neuz.m_TextureCastingGauge[0].RenderScal(&(g_Neuz.m_2DRender), p, 200, 0.8f, 0.55f);		
	
//	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( g_pPlayer->m_dwUseItemId );
//	if( pItemProp )
	{
		DWORD dwTime =  dwTmpCastingTick - (dwTmpCastingEndTick - dwtmpTick) ;
		int nWidth = (int)( g_Neuz.m_TextureHPGauge[1].m_size.cx * ((float)dwTime / dwTmpCastingTick ) );
		if( 112 < nWidth )
			nWidth = 112;
		if( nWidth < 0 )
			nWidth = 0;
		
		CPoint p2 = CPoint( nWidth, g_Neuz.m_TextureHPGauge[1].m_size.cy );
		
		g_Neuz.m_TextureCastingGauge[1].Render(&(g_Neuz.m_2DRender), p, p2, 255, 0.8f, 0.55f);		
	}
}



void CMover::RenderPVPCount(LPDIRECT3DDEVICE9 pd3dDevice)
{
	DWORD dwWidth = g_Neuz.m_d3dsdBackBuffer.Width / 2;
	DWORD dwHeight = g_Neuz.m_d3dsdBackBuffer.Height / 2;
	
	if( g_Neuz.m_dwPVPTime <= GetTickCount() && GetTickCount() <= g_Neuz.m_dwPVPTime + 1500 )
	{
		if( g_Neuz.m_nPVPCount == 0 )
			g_Neuz.m_nPVPCount = 10;
		
		float fScal = (float)( GetTickCount() - g_Neuz.m_dwPVPTime );
		fScal = fScal / 10.0f;
		int nAlpha = 180;
		if( 150 < nAlpha )
		if( fScal < 25 )
		{
			fScal = 1.0f;
		}
		else
		{
			fScal /= 10;
			fScal -= 1.5f;
			nAlpha	= (int)( nAlpha - (fScal * 10) );
		}

		if( 1.5f < fScal )
		{
			if( g_Neuz.m_nPVPCount == 0 )
				fScal = 1.5f;
			else if( 2.0f < fScal )
				fScal = 2.0f;
		}

		FLTRACE_LOG( PROGRAM_NAME, _T( "fScal : %f" ), fScal );

		CPoint p = CPoint( 
					(int)( dwWidth - ( g_Neuz.m_TexturePackPVP.m_ap2DTexture[g_Neuz.m_nPVPCount].m_size.cx * fScal / 2 ) ),
					(int)( dwHeight - ( g_Neuz.m_TexturePackPVP.m_ap2DTexture[g_Neuz.m_nPVPCount].m_size.cy * fScal / 2 ) - 50 )
					);
		 
		MakeEven( p.x );
		g_Neuz.m_TexturePackPVP.m_ap2DTexture[g_Neuz.m_nPVPCount].RenderScal(&(g_Neuz.m_2DRender), p, nAlpha, fScal, fScal);			
	}
	
	
}

#endif // not worldserver
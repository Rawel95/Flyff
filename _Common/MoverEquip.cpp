#include "stdafx.h"
#include "defineText.h"
#include "ModelObject.h"
#include "authorization.h"
#include "CreateObj.h"
#include "lang.h"
#include "party.h"
#include "guild.h"
#include "guildwar.h"

#ifdef __CLIENT
	#include "../Neuz/DPClient.h"
	#include "DialogMsg.h"
	extern	CDPClient	g_DPlay;
	#include "GuildRender.h"
	#include "defineSound.h"
#endif	// __CLIENT

#ifdef	__WORLDSERVER
	#include "../WorldServer/User.h"
	#include "WorldMng.h"

	extern	CWorldMng			g_WorldMng;
#endif	// __WORLDSERVER

#include "accessory.h"		//sun: 11, Ã¤Áý ½Ã½ºÅÛ

extern	CPartyMng		g_PartyMng;
extern	CGuildMng		g_GuildMng;
extern	CGuildWarMng	g_GuildWarMng;

static const TCHAR	LOG_PARTY_ERROR[]	= _T( "PartyError" );


BYTE  nMaleHairColor[10][3] = { 
	{ 144,  99,  101 },
	{ 100, 107, 125 },
	{ 158,119,109 },
	{ 199,156,100 },
	{ 100,123,117 },
	{ 100,100,100 },
	{ 100,100,100 },
	{ 100,100,100 },
	{ 100,100,100 },
	{ 100,100,100 } 
};

BYTE  nFeMaleHairColor[10][3] = { 
	{ 250,250,90 },
	{ 250,250,90 },
	{ 147,118,103 },
	{ 150, 100, 85 },
	{ 213,130,100 },
	{ 100,100,100 },
	{ 100,100,100 },
	{ 100,100,100 },
	{ 100,100,100 },
	{ 100,100,100 } 
};

#define TEX_PART_HAIR( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHair01.dds"  ) : _T( "Part_femaleHair01.dds"  ) )
#define TEX_PART_UPPER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleUpper01.dds" ) : _T( "Part_femaleUpper01.dds" ) )
#define TEX_PART_LOWER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleLower01.dds" ) : _T( "Part_femaleLower01.dds" ) )

#define PARTSMESH_HAIR( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHair%02d.o3d" ) : _T( "Part_femaleHair%02d.o3d" ) )
#define PARTSMESH_HEAD( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHead%02d.o3d" ) : _T( "Part_femaleHead%02d.o3d" ) )
#define PARTSMESH_UPPER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleUpper.o3d"    ) : _T( "Part_femaleUpper.o3d"    ) )
#define PARTSMESH_LOWER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleLower.o3d"    ) : _T( "Part_femaleLower.o3d"    ) )
#define PARTSMESH_HAND( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHand.o3d"     ) : _T( "Part_femaleHand.o3d"     ) )
#define PARTSMESH_FOOT( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleFoot.o3d"     ) : _T( "Part_femaleFoot.o3d"     ) )

#define PARTSTEX_HAIR( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHair%02d.dds"  ) : _T( "Part_femaleHair%02d.dds"  ) )
#define PARTSTEX_UPPER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleUpper%02d.dds" ) : _T( "Part_femaleUpper%02d.dds" ) )
#define PARTSTEX_LOWER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleLower%02d.dds" ) : _T( "Part_femaleLower%02d.dds" ) )
#define PARTSTEX_HAND( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHand.dds"      ) : _T( "Part_femaleHand.dds"      ) )
#define PARTSTEX_FOOT( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleFoot.dds"      ) : _T( "Part_femaleFoot.dds"      ) )

int GetSelfDST( BYTE type )
{
	switch( type )
	{
		case SAI79::FIRE:
			return DST_RESIST_FIRE;
		case SAI79::WATER:
			return DST_RESIST_WATER;
		case SAI79::WIND:
			return DST_RESIST_WIND; 
		case SAI79::ELECTRICITY:
			return DST_RESIST_ELECTRICITY;
		case SAI79::EARTH:
			return DST_RESIST_EARTH;
	}
	return DST_RESIST_FIRE;
}

int GetStrongDST( BYTE type )
{
	switch( type )
	{
		case SAI79::FIRE:
			return DST_RESIST_WIND;
		case SAI79::WATER:
			return DST_RESIST_FIRE;
		case SAI79::WIND:
			return DST_RESIST_EARTH;
		case SAI79::ELECTRICITY:
			return DST_RESIST_WATER;
		case SAI79::EARTH:
			return DST_RESIST_ELECTRICITY;
	}
	return DST_RESIST_FIRE;
}

int GetWeakDST( BYTE type )
{
	switch( type )
	{
		case SAI79::FIRE:
			return DST_RESIST_WATER;
		case SAI79::WATER:
			return DST_RESIST_ELECTRICITY;
		case SAI79::WIND:
			return DST_RESIST_FIRE;
		case SAI79::ELECTRICITY:
			return DST_RESIST_EARTH;
		case SAI79::EARTH:
			return DST_RESIST_WIND;
	}
	return DST_RESIST_FIRE;
}

// ¸Ó¸®Ä«¶ô ¸ð¾ç°ú »ö ¹Ù²Ù±â  
void CMover::SetHair( int nHair )
{
	if( nHair >= 0 && nHair < MAX_HAIR )
	{
		m_dwHairMesh = nHair;
		CModelObject* pModel = (CModelObject*)m_pModel;
		TCHAR lpszTemp[ 64 ] = { 0, };
		// Hair
		FLSPrintf( lpszTemp, _countof( lpszTemp ), PARTSMESH_HAIR( GetSex() ), m_dwHairMesh + 1 );
		pModel->LoadElement( lpszTemp, PARTS_HAIR );
		FLSPrintf( lpszTemp, _countof( lpszTemp ), PARTSTEX_HAIR( GetSex() ), 0 /*nHairColor*/ + 1 );
		pModel->ChangeTexture( PARTS_HAIR, TEX_PART_HAIR( GetSex() ), lpszTemp );
	}
}

void  CMover::SetHairColor( FLOAT r, FLOAT g, FLOAT b )
{ 
#ifndef __Y_HAIR_BUG_FIX	
	if( ( r == 1.0f && g == 1.0f && b == 1.0f ) || 
		( fabs( r - 1.0f ) < 0.001f && fabs( g - 1.0f ) < 0.001f && fabs( b - 1.0f ) < 0.001f ) )
	{
		if( GetSex() == SEX_MALE )
		{
			m_fHairColorR = (nMaleHairColor[m_dwHairMesh][0])/255.f;
			m_fHairColorG = (nMaleHairColor[m_dwHairMesh][1])/255.f;
			m_fHairColorB = (nMaleHairColor[m_dwHairMesh][2])/255.f;
		}
		else
		{
			m_fHairColorR = (nFeMaleHairColor[m_dwHairMesh][0])/255.f;
			m_fHairColorG = (nFeMaleHairColor[m_dwHairMesh][1])/255.f;
			m_fHairColorB = (nFeMaleHairColor[m_dwHairMesh][2])/255.f;
		}
	}
	else
#endif //__Y_HAIR_BUG_FIX
	{
#ifdef __Y_HAIR_BUG_FIX
		m_fHairColorR = ( r < 0.0f ) ? 0.0f : r;
		m_fHairColorG = ( g < 0.0f ) ? 0.0f : g;
		m_fHairColorB = ( b < 0.0f ) ? 0.0f : b;
#else //__Y_HAIR_BUG_FIX
		m_fHairColorR = ( r < 0.3f ) ? 0.3f : r;
		m_fHairColorG = ( g < 0.3f ) ? 0.3f : g;
		m_fHairColorB = ( b < 0.3f ) ? 0.3f : b;
#endif //__Y_HAIR_BUG_FIX
	}
	
	m_dwHairColor = D3DCOLOR_COLORVALUE( m_fHairColorR, m_fHairColorG, m_fHairColorB, 1.0f );
}


void CMover::SetHairColor( DWORD dwHairColor )
{
	m_dwHairColor = dwHairColor;
	m_fHairColorR = ((dwHairColor>>16) & 0xff) / 255.f;
	m_fHairColorG = ((dwHairColor>>8) & 0xff)  / 255.f;
	m_fHairColorB = ((dwHairColor) & 0xff)     / 255.f;

#ifndef __Y_HAIR_BUG_FIX
	if( ( m_fHairColorR == 1.0f && m_fHairColorG == 1.0f && m_fHairColorB == 1.0f ) || 
		( fabs( m_fHairColorR - 1.0f ) < 0.001f && fabs( m_fHairColorG - 1.0f ) < 0.001f && fabs( m_fHairColorB - 1.0f ) < 0.001f ) )
	{
		if( GetSex() == SEX_MALE )
		{
			m_fHairColorR = (nMaleHairColor[m_dwHairMesh][0])/255.f;
			m_fHairColorG = (nMaleHairColor[m_dwHairMesh][1])/255.f;
			m_fHairColorB = (nMaleHairColor[m_dwHairMesh][2])/255.f;
		}
		else
		{
			m_fHairColorR = (nFeMaleHairColor[m_dwHairMesh][0])/255.f;
			m_fHairColorG = (nFeMaleHairColor[m_dwHairMesh][1])/255.f;
			m_fHairColorB = (nFeMaleHairColor[m_dwHairMesh][2])/255.f;
		}

	}
#endif //__Y_HAIR_BUG_FIX

	m_dwHairColor = D3DCOLOR_COLORVALUE( m_fHairColorR, m_fHairColorG, m_fHairColorB, 1.0f );
}

// Ç¥Á¤ ¹Ù²Ù±â 
void CMover::SetHead( int nHead )
{
	if( nHead < MAX_HEAD )
	{
		m_dwHeadMesh = nHead;
		CModelObject* pModel = (CModelObject*)m_pModel;
		TCHAR lpszTemp[ 64 ] = { 0, };
		// Hair
		FLSPrintf( lpszTemp, _countof( lpszTemp ), PARTSMESH_HEAD( GetSex() ), m_dwHeadMesh + 1 );
		pModel->LoadElement( lpszTemp, PARTS_HEAD );
	}
}
void CMover::SetSkinSet( int nSkinSet )
{
	if( nSkinSet < 2 )
	{
		m_dwSkinSet = nSkinSet;
#ifdef __CLIENT
		UpdateParts();
#endif
	}
}


// ÀÌ ÇÔ¼ö´Â ÀüÃ¼ ÀÇ»óÀ» »õ·Î ¿Ï¼ºÇÏµµ·Ï ÇÑ´Ù.
// º¸Åë ÀÌ ÇÔ¼ö´Â Ã³À½¿¡ Ä³¸¯ÅÍ¸¦ »ý¼ºÇÒ ¶§ ÇÑ¹ø È£ÃâÇÑ´Ù.
void CMover::UpdateParts( BOOL bFakeParts )
{
	UpdateParts( GetSex(), m_dwSkinSet, m_dwFace, m_dwHairMesh, m_dwHeadMesh, 
		m_aEquipInfo, (CModelObject*)m_pModel, bFakeParts ? NULL : &m_Inventory, GetProp()->bIfParts, this );
	((CModelObject*)m_pModel)->RestoreDeviceObjects();
	UpdateBoundBox();
	UpdateParam();
}

PT_ITEM_SPEC GetInventoryProp( PEQUIP_INFO pEquipInfo, CItemContainer< FLItemElem  >* pInventory, int nParts )
{
	FLItemElem* pItemElem = NULL;
	PT_ITEM_SPEC pItemProp = NULL;

	if( pInventory == NULL ) // fake only
	{
		if( pEquipInfo[nParts].dwItemID != 0 ) 
			pItemProp = g_xSpecManager->GetSpecItem( pEquipInfo[nParts].dwItemID );
	}
	else
	{
		pItemElem = pInventory->GetEquip( nParts );
		if( pItemElem )
			pItemProp = pItemElem->GetProp();
	}

	return pItemProp;
}

void CMover::UpdateParts( int nSex, int /*nSkinSet*/, int /*nFace*/, int nHairMesh, int nHeadMesh, PEQUIP_INFO pEquipInfo, CModelObject* pModel, CItemContainer< FLItemElem  >* pInventory, BOOL bIfParts, CMover* pMover )
{
	if( pModel == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "pModel is NULL" ) );
		return;
	}
	// ¡¾a¨¬¡í ¨¬IA¡×(¨ùO¡íi, Ci¨úi, ¥ìi¥ìi)A¡í ¨ù¨ù¨¡ACN¢¥U.
	//LPSKINSET lpSkinset = prj.m_PartsMng.GetSkinSet( nSex, nSkinSet );
	//LPCTSTR lpHair = prj.m_PartsMng.GetHairMeshName( nSex, nHairMesh );
	TCHAR lpszTemp[ 64 ] = { 0, };
	if( bIfParts )
	{
		// Hair
		FLSPrintf( lpszTemp, _countof( lpszTemp ), PARTSMESH_HAIR( nSex ), nHairMesh + 1 );
		pModel->LoadElement( lpszTemp, PARTS_HAIR );
		// Head
		FLSPrintf( lpszTemp, _countof( lpszTemp ), PARTSMESH_HEAD( nSex ), nHeadMesh + 1 );
		pModel->LoadElement( lpszTemp, PARTS_HEAD );
	}
	// ¢¬OAu AI¨ö¨¬A¨Ï¡¤c¨öA¨¬e ¢¯A¨¬eA¡×¨¡¢ç¢¬| TakeOffCN¢¥U.
	BOOL abExclusiveParts[ MAX_HUMAN_PARTS ];
	ZeroMemory( abExclusiveParts, sizeof( abExclusiveParts ) );
#ifdef __CLIENT
		if(pMover)
			pMover->m_bExclusiveLowBody = false;
#endif
	for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
	{
		if( i == PARTS_HAIR || i == PARTS_HEAD  )//|| i == PARTS_CAP || i == PARTS_HAND || i == PARTS_FOOT )
			continue;
//		DWORD dwParts = -1;
//		FLItemElem* pItemElem = NULL;
		PT_ITEM_SPEC pItemProp = NULL;
		// CA¡¤I¨¡U¨¡¨ù ©÷¡§©ø¢¯
		pItemProp = GetInventoryProp( pEquipInfo, pInventory, i );
		if( pItemProp )
		{
			if( pInventory )
			{
				FLItemElem* pItemElem	= pInventory->GetAt( i );
				if( pItemElem->IsFlag( FLItemElem::expired ) )
					pItemProp	= NULL;
			}
			else
			{
				if( pEquipInfo[i].byFlag & FLItemElem::expired )
					pItemProp	= NULL;
			}
		}

		if( pItemProp && pItemProp->dwExclusive != NULL_ID )
		{
			
#ifdef __CLIENT
		// AI¨ö¨¬A¡þ¡¤c¨öA¨¬e¡Æ¢® AOA¡í¢Ò¡×
		if( i == PARTS_CLOTH && pMover)
			pMover->m_bExclusiveLowBody = true;
#endif
			pModel->TakeOffParts( pItemProp->dwExclusive );	// AI¨ö¨¬A¡þ¡¤c¨öA¨¬e¢¬| A|¡ÆA
			abExclusiveParts[ pItemProp->dwExclusive ] = TRUE;		// AI¨ö¨¬A¡þ¡¤c¨öA¨¬e ¥ìE©øNA¡í ¡¾a¨úi.
		}
	}
	// AaA©ª¨¬IA¡×¢¬| ¨ù¨ù¨¡ACN¢¥U.
	for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
	{
		if( i == PARTS_HAIR || i == PARTS_HEAD  )//|| i == PARTS_CAP || i == PARTS_HAND || i == PARTS_FOOT )
			continue;
//		DWORD dwParts = -1;
		FLItemElem* pItemElem = NULL;
		PT_ITEM_SPEC pItemProp = NULL;
		if( pInventory == NULL ) // fake only
		{
			if( pEquipInfo[i].dwItemID != 0 ) 
			{
				if( ( pEquipInfo[i].byFlag & FLItemElem::expired ) != FLItemElem::expired )
				{
					//¢¯UCu¨¬?¡Æ©¡ A©ø¢¬¢ç
					DWORD dwIndex = pEquipInfo[i].dwItemID;
				
					if( 0 != pEquipInfo[i].dwLooksChangeItemID )
						dwIndex = pEquipInfo[i].dwLooksChangeItemID;

					pItemProp	= g_xSpecManager->GetSpecItem( dwIndex );
				}
			}
		}
		else
		{
			pItemElem = pInventory->GetEquip( i );//AtId( padwEquipment[ i ] );
			if( pItemElem )
			{
				if( !pItemElem->IsFlag( FLItemElem::expired ) )
				{
					//¢¯UCu¨¬?¡Æ©¡ A©ø¢¬¢ç
					if( pItemElem->GetLooksChangeItemID( ) != 0 )
						pItemProp	= g_xSpecManager->GetSpecItem( pItemElem->GetLooksChangeItemID( ) );
					else
						pItemProp = pItemElem->GetProp();
				}
			}
		}
#if defined(__ZCK_AILES_SFX) && defined(__CLIENT)
		BOOL bAilesSfx = FALSE;
		if( i == PARTS_MASK && pItemProp && pItemProp->dwItemKind3 == IK3_AILES_SFX )
			bAilesSfx = TRUE;

		if( pMover && i == PARTS_MASK )
		{
			if( bAilesSfx )
				pMover->UpdateAilesSfxZacks( pItemProp->dwSfxObj2 );
			else
				pMover->ClearAilesSfxZacks();
		}
#endif // __ZCK_AILES_SFX && __CLIENT

		TCHAR szPartsName[ 32 ];

		// ¡Æ¡Ë ¨¡AA¡ÀAC o3d¢¬| ¡¤I¥ìu.
#if defined(__ZCK_AILES_SFX) && defined(__CLIENT)
		if( bAilesSfx )
		{
			pModel->TakeOffParts( PARTS_MASK );
		}
		else
#endif // __ZCK_AILES_SFX && __CLIENT
		if( pItemProp )
		{
			if( pItemProp->bPartsFile == TRUE )
				prj.m_modelMng.MakePartsName( szPartsName, _countof( szPartsName ), _T( "part" ), pItemProp->dwID, nSex );
			else
				prj.m_modelMng.MakeModelName( szPartsName, _countof( szPartsName ), OT_ITEM, pItemProp->dwID );
			//prj.m_modelMng.MakePartsName( szPartsName, _T( "item" ), pItemProp->dwID );
			if( bIfParts || ( i == PARTS_RWEAPON || i == PARTS_LWEAPON || i == PARTS_SHIELD ) )
			{
				// ©ø?¡Æ©ø¢¬| ¢¬AAa¢¯¢® ¨¬IA©ªCO¡¤A¡Æi ¨öA¥ì¥ìCN AU¥ìa		2016.07.11
				//if ( pItemProp->dwItemKind3 != IK3_WING )			// ¨¬nCaA¨ù ¢¬¡í¡Æi ¢¥U¢¬¡Í ¨¡A¨¡¢ç¢¯¢® ©ø?¡Æ©ø¡Æ¢® AOA¢¬¢¬e Aa¡¤A¨úE¨öAA¢¥	05
					pModel->LoadElement( szPartsName, i /*pItemProp->dwParts*/ );
			}
		}


		if( abExclusiveParts[ i ] == FALSE )
		{
			switch( i )
			{
			case PARTS_RWEAPON: 
				// ¨¬?¨öA A©ø¢¬?AI¢¥A AaA©ª ©ö¡ì¡¾a¢¬| ¨¬¨ù ¨ùo ¨ú©ª¡ÆO CIAU(¨¡AA¡À AaA©ª¢¬¢¬ ¢¬©ªCO. ¨ùoA¢® ¡Æe¡íeA¨¬ Au¢¯e¥ìE)
				if( pItemProp && ( pMover == NULL || ( pMover && pMover->IsDisguise() == FALSE ) ) )
				{
					switch( pItemProp->dwItemKind3 )
					{
					case IK3_KNUCKLEHAMMER:
						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRArmIdx() );
						break;

					case IK3_BOW:
#ifdef __ADDWEAPON_CROSSBOW16
						if( pItemProp->nDestData1[0] == 100 )
							((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
						else
							((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
#else
						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
#endif

						break;
					case IK3_YOYO:
						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
						pModel->LoadElement( szPartsName, PARTS_LWEAPON );
						((CModelObject*)pModel)->SetParent( PARTS_LWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
						break;

#ifdef __ADDWEAPON_CROSSBOW16
					case IK3_CROSSBOW:
						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
						break;
#endif //__ADDWEAPON_CROSSBOW16

					default:
						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
					}
//					if( pItemProp->dwItemKind3 == IK3_KNUCKLEHAMMER )
//						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRArmIdx() );
//					else
//						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
				}
				else 
					pModel->TakeOffParts( i );
				break;
			case PARTS_LWEAPON: 
				// ¨¬?¨öA A©ø¢¬?AI¢¥A AaA©ª ©ö¡ì¡¾a¢¬| ¨¬¨ù ¨ùo ¨ú©ª¡ÆO CIAU(¨¡AA¡À AaA©ª¢¬¢¬ ¢¬©ªCO. ¨ùoA¢® ¡Æe¡íeA¨¬ Au¢¯e¥ìE)
				if( pItemProp && ( pMover == NULL || ( pMover && pMover->IsDisguise() == FALSE ) ) )
				{
					((CModelObject*)pModel)->SetParent( PARTS_LWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
				}
				else 
					pModel->TakeOffParts( i );
				break;
			case PARTS_SHIELD: 
				// ¨¬?¨öA A©ø¢¬?AI¢¥A AaA©ª ©ö¡ì¡¾a¢¬| ¨¬¨ù ¨ùo ¨ú©ª¡ÆO CIAU(¨¡AA¡À AaA©ª¢¬¢¬ ¢¬©ªCO. ¨ùoA¢® ¡Æe¡íeA¨¬ Au¢¯e¥ìE)
				if( pItemProp && ( pMover == NULL || ( pMover && pMover->IsDisguise() == FALSE ) ) )
					((CModelObject*)pModel)->SetParent( PARTS_SHIELD, ((CModelObject*)pModel)->GetLArmIdx() );
				else 
					pModel->TakeOffParts( i );
				break;
			case PARTS_UPPER_BODY:
				if( bIfParts )
					if( pItemProp == NULL )	
						pModel->LoadElement( PARTSMESH_UPPER( nSex ), PARTS_UPPER_BODY );
				break;
			case PARTS_LOWER_BODY:
				if( bIfParts )
					if( pItemProp == NULL ) 
						pModel->LoadElement( PARTSMESH_LOWER( nSex ), PARTS_LOWER_BODY );
				break;
			case PARTS_HAND:
				if( bIfParts )
					if( pItemProp == NULL ) 
						pModel->LoadElement( PARTSMESH_HAND( nSex ), PARTS_HAND );
				break;
			case PARTS_FOOT:
				if( bIfParts )
					if( pItemProp == NULL ) 
						pModel->LoadElement( PARTSMESH_FOOT( nSex ), PARTS_FOOT );
				break;
			default:
				if( bIfParts )
				{
					if( pItemProp == NULL ) 
						pModel->TakeOffParts( i );
				}

			}
		}
	}
}

void CMover::SetDefaultLowerBody(CModelObject* pModel)
{
	if( pModel != NULL )
		pModel->LoadElement( PARTSMESH_LOWER( m_bySex ), PARTS_LOWER_BODY );
}
// Àåºñ¸¦ ÀåÂøÇÑ´Ù. ¿©±â¼­ ÀÇ»ó ±³Ã¼ ÀÛ¾÷À» ÇØÁØ´Ù.
// nOptionÀº Fake¾ÆÀÌÅÛ ÀåÂø¶§¸¸ ¾²ÀÎ´Ù. - xuzhu -
BOOL CMover::DoEquip( int nSex, int nSkinSet, 
					  FLItemElem* pItemElem, int nPart, const EQUIP_INFO & rEquipInfo, CItemContainer< FLItemElem  >* pInventory, 
					  PEQUIP_INFO pEquipInfo, CModelObject* pModel, BOOL bEquip, CMover *pMover ) 
{
	PT_ITEM_SPEC pItemProp = pItemElem ? pItemElem->GetProp() : g_xSpecManager->GetSpecItem( rEquipInfo.dwItemID );

	DWORD dwIndex = pItemElem ? pItemElem->m_dwObjIndex : 0;
	DWORD dwParts = pItemProp->dwParts;
	BOOL bIfParts = pMover ? pMover->GetProp()->bIfParts : 0;
	TCHAR lpszTemp[ 64 ];
//	BOOL bFake = (pInventory == NULL) ? TRUE : FALSE;
	if( dwParts == NULL_ID )
		return FALSE;

	// ¹ÝÁö/±Í°ÉÀÌ ¹þÀ»¶§ Ã³¸®.
	if( !bEquip )
	{
		if( dwParts == PARTS_RING1 || dwParts == PARTS_EARRING1 )
		{
			FLItemElem *pItemElemTemp;
			
			if( pInventory )
			{
				pItemElemTemp = (FLItemElem*)pInventory->GetEquip( dwParts );
				if( pItemElemTemp != pItemElem )
					dwParts = pItemProp->dwPartsub;
			}
		}
	}
	// ½ÖÄ®(ÀÌµµ·ù) Ã³¸®.
	if( pItemProp && pItemProp->dwHanded == HD_ONE && pItemProp->dwParts == PARTS_RWEAPON )	// ¿øÇÚµå ¹«±â¸¦ Â÷·Á°í ÇÏ´Â°¡?
	{
		if( bEquip )
		{
			// µðÆúÆ®´Â ÀåÂøÀ§Ä¡ ¿À¸¥¼Õ
			// ¿Þ¼Õ¿¡ ¹«±â°¡ °¡¾ßÇÏ´Â »óÈ²À» °Ë»ç
			PT_ITEM_SPEC pProp = pMover->GetEquipItemProp( pInventory, pEquipInfo, PARTS_RWEAPON );
//sun: 10Â÷ Àü½Â½Ã½ºÅÛ	Neuz, World, Trans

			if( pMover->GetJob() == JOB_BLADE || pMover->GetJob() == JOB_BLADE_MASTER ||
				pMover->GetJob() == JOB_BLADE_HERO || pMover->GetJob() == JOB_STORMBLADE_HERO )	// ½ÖÄ®¼Ó¼º
			{
				if( pItemProp->dwID != ITEM_INDEX( 6323, II_WEA_KNU_ISHOFIST ) )
				{
					if( pProp && pProp->dwHanded == HD_ONE )	// ¿À¸¥¼Õ¿¡ ¹«±â°¡ ÀÖ³Ä? ±× ¹«±â°¡ ¿øÇÚµå³Ä
					{
						if( pMover->GetEquipItemProp( pInventory, pEquipInfo, PARTS_SHIELD ) == NULL )	//¿Þ¼Õ¿¡ ¹æÆÐ¾ø³Ä?
							dwParts = PARTS_LWEAPON;		// ÀåÂøÀ§Ä¡¸¦ ¿Þ¼ÕÀ¸·Î ¹Ù²Þ
					} 
				}
			}

			if( pProp && pProp->dwItemKind3 == IK3_YOYO )		// ¿À¸¥¼Õ¿¡ ¹«±â°¡ ¿ä¿ä¿´À¸¸é
				pModel->TakeOffParts( PARTS_LWEAPON );			// ¿Þ¼Õ¿¡ ¿ä¿ä¸ðµ¨µµ »èÁ¦ÇÔ. ¿À¸¥¼Õ¸ðµ¨Àº ¹Ø¿¡¼­ ÀÚµ¿À¸·Î 
			
		} 
		else
		{
			//////////////////////////////////////////////////////////////////////////
			if( pItemElem != NULL && pInventory != NULL )
			{
				if( pItemElem->m_dwObjIndex >= pInventory->GetSize() )
				{
					dwParts = pItemElem->m_dwObjIndex - pInventory->GetSize();
				}
				else
				{
					dwParts = pItemElem->m_dwObjIndex;
				}
			}
			//////////////////////////////////////////////////////////////////////////
			// ÇØÁ¦½Ãµµ.
//			if( nPart >= 0  )		// ÆÄÃ÷¸¦ Á÷Á¢ ÁöÁ¤ÇßÀ»¶§¸¸
//				dwParts = nPart;	// ÇØÁ¦ÇÏ¶ó°í ÇÑ ºÎÀ§¸¦ ÇØÁ¦½ÃÅ²´Ù.
		}
	} // ½ÖÄ® Ã³¸®.
	
#ifndef __WORLDSERVER	// <<<< ¿ùµå¿¡¼­ Ã³¸®¾ÈÇÏ¸é ¸Þ¸ð¸® ¸®Å© ³ªÁö ¾Ê³ª? -xuzhu-
	((CModelObject*)pModel)->TakeOffParts( dwParts );	
#endif

	if( pItemProp && pItemProp->dwItemKind3 == IK3_YOYO )	// ¹þÀ¸·Á´ø ¹«±â°¡ ¿ä¿ä¿´À¸¸é
		pModel->TakeOffParts( PARTS_LWEAPON );		// ¿Þ¼Õ¿¡ ÀÖ´ø ¿ä¿ä¸ðµ¨µµ ¾ø¾ÖÁØ´Ù.

	if( bEquip )	// ÀåÂøÇÏ·Á ÇÒ¶§¸¸...
	{
		PT_ITEM_SPEC pHandItemProp	= NULL;
		PT_ITEM_SPEC pLHandItemProp = NULL;

		// µé°íÀÖ´Â ¹«±â ÇÁ·ÎÆÛÆ¼ ²¨³¿.
		pHandItemProp = pMover->GetEquipItemProp( pInventory, pEquipInfo, PARTS_RWEAPON );		// ¿À¸¥¼Õ ¹«±â ÇÁ·ÎÆÛÆ¼.
		pLHandItemProp = pMover->GetEquipItemProp( pInventory, pEquipInfo, PARTS_LWEAPON );		// ¿Þ¼Õ ¹«±â ÇÁ·ÎÆÛÆ¼.
		
		// Á¶°Ç°Ë»ç.
		if( pItemProp->dwItemKind3 == IK3_SHIELD || pItemProp->dwItemKind3 == IK3_ZEMBARUNA
			|| pItemProp->dwItemKind3 == IK3_MAGICBARUNA || pItemProp->dwItemKind3 == IK3_SHILDBARUNA )	// ¹æÆÐ·ù¸¦ Âø¿ëÇÏ·Á ÇßÀ»¶§
		{
			if( pHandItemProp && pHandItemProp->dwHanded == HD_TWO )		// ¼Õ¿¡ Áã°í ÀÖ´Â°Ô ÅõÇÚµå¸é.
			{
				if( pInventory )
				{
					FLItemElem *pItemElemOld = pInventory->GetEquip( PARTS_RWEAPON );
					if( pInventory->UnEquip( PARTS_RWEAPON ) == FALSE )	
						return FALSE;
#ifdef __WORLDSERVER
					// ÅõÇÚµå ¹«±â¹þ±è
					if( pItemElemOld )
					{
						PT_ITEM_SPEC pOldItemProp = pItemElemOld->GetProp();
						pMover->ResetDestParamEquip( pOldItemProp, pItemElemOld );

						g_xApplyItemEffect->RemoveEquipActiveSkill( pMover, pItemElemOld );

						g_xWSUserManager->AddDoEquip( pMover, -1, pItemElemOld, FALSE );
					}
//sun: 9,10Â÷ Á¦·Ã __ULTIMATE
					prj.m_UltimateWeapon.SetDestParamUltimate( pMover, pItemElemOld, FALSE, PARTS_RWEAPON );

#endif // worldserver
					if( pEquipInfo )
						pEquipInfo[PARTS_RWEAPON].dwItemID	= 0;
				} else
				{
					if( pEquipInfo )
						pEquipInfo[PARTS_RWEAPON].dwItemID	= 0;
				}
				pModel->TakeOffParts( PARTS_RWEAPON );
				if( pHandItemProp->dwItemKind3 == IK3_YOYO )	// µé°í ÀÖ´ø ¹«±â°¡ ¿ä¿ä¿´À¸¸é
					pModel->TakeOffParts( PARTS_LWEAPON );		// ¿Þ¼Õ¿¡ ÀÖ´ø ¿ä¿ä¸ðµ¨µµ ¾ø¾ÖÁØ´Ù.
			}
			else
			if( pLHandItemProp && pLHandItemProp->dwParts == PARTS_RWEAPON )	// ¿Þ¼Õ¿¡ ¹«±â°¡ Áã¾îÁ®ÀÖÀ¸¸é. ¿Þ¼Õ¿¡ µå´Â ¹«±âµµ PARTSÀÌ¸§Àº ¸ðµÎ RWEAPONÀÌ´Ù.
			{
				if( pInventory )
				{
					FLItemElem *pItemElemOld = pInventory->GetEquip( PARTS_LWEAPON );
					if( pInventory->UnEquip( PARTS_LWEAPON ) == FALSE )	
						return FALSE;
#ifdef __WORLDSERVER
					// ¿Þ¼Õ ¹«±â¹þ±è
					if( pItemElemOld )
					{
						PT_ITEM_SPEC pOldItemProp = pItemElemOld->GetProp();
						pMover->ResetDestParamEquip( pOldItemProp, pItemElemOld );

						g_xApplyItemEffect->RemoveEquipActiveSkill( pMover, pItemElemOld );

						g_xWSUserManager->AddDoEquip( pMover, PARTS_LWEAPON, pItemElemOld, FALSE );
					}
#endif // worldserver
					if( pEquipInfo )
						pEquipInfo[PARTS_LWEAPON].dwItemID	= 0;
				}
				else
				{
					if( pEquipInfo )
						pEquipInfo[PARTS_LWEAPON].dwItemID	= 0;
				}
				pModel->TakeOffParts( PARTS_LWEAPON );
			}
		} // IK3_SHIELD
		if( pItemProp->dwHanded == HD_TWO )		// ÀåÂøÇÏ·Á´Â ¹«±â°¡ ÅõÇÚµå ¿´À»¶§
		{
			if( pHandItemProp && pHandItemProp->dwItemKind3 == IK3_YOYO )	// µé°í ÀÖ´ø ¹«±â°¡ ¿ä¿ä¿´À¸¸é
				pModel->TakeOffParts( PARTS_LWEAPON );		// ¿Þ¼Õ¿¡ ÀÖ´ø ¿ä¿ä¸ðµ¨µµ ¾ø¾ÖÁØ´Ù.
			if( pInventory == NULL )
			{
				if( pEquipInfo[PARTS_SHIELD].dwItemID != 0 )		// ¹æÆÐ°¡ ÀÖ¾ú´Â°¡ °Ë»çÇØ¼­ ÀÖÀ¸¸é ¹þ±è.
				{
					pModel->TakeOffParts( PARTS_SHIELD );
					if( pEquipInfo )
						pEquipInfo[PARTS_SHIELD].dwItemID	= 0;
				}
				if( pEquipInfo[PARTS_RWEAPON].dwItemID != 0 )		// ¹«±â°¡ ÀÖ¾ú´Â°¡ °Ë»çÇØ¼­ ¹þ±è.
				{
					pModel->TakeOffParts( PARTS_RWEAPON );
					if( pEquipInfo )
						pEquipInfo[PARTS_RWEAPON].dwItemID	= 0;
				}
			}
			else
			{
				FLItemElem *pItemElemOld = pInventory->GetEquip( PARTS_SHIELD );
				if( pItemElemOld )
				{
					if( pInventory->UnEquip( PARTS_SHIELD ) == FALSE )	// ¹æÆÐ ¹þ±è
						return FALSE;	// ÀÎº¥ÀÌ ²ËÂ÷¸é °Á Ãë¼Ò
#ifdef __WORLDSERVER
					if( pItemElemOld )	// ±âÁ¸Àåºñ°¡ ÀÖ¾ú´Ù¸é
					{
						PT_ITEM_SPEC pOldItemProp = pItemElemOld->GetProp();
						pMover->ResetDestParamEquip( pOldItemProp, pItemElemOld );
						
						g_xApplyItemEffect->RemoveEquipActiveSkill( pMover, pItemElemOld );

						g_xWSUserManager->AddDoEquip( pMover, -1, pItemElemOld, FALSE );
					}
#endif
					pModel->TakeOffParts( PARTS_SHIELD );
					if( pEquipInfo )
						pEquipInfo[PARTS_SHIELD].dwItemID	= 0;
				}
				
				pItemElemOld = pInventory->GetEquip( PARTS_RWEAPON );
				if( pInventory->GetEquip( PARTS_RWEAPON ) )
				{
					if( pInventory->UnEquip( PARTS_RWEAPON ) == FALSE )	// ¹«±â¹þ±è
						return FALSE;	// ÀÎº¥ÀÌ ²ËÂ÷¸é °Á Ãë¼Ò
#ifdef __WORLDSERVER
					if( pItemElemOld )	// ±âÁ¸Àåºñ°¡ ÀÖ¾ú´Ù¸é
					{
						PT_ITEM_SPEC pOldItemProp = pItemElemOld->GetProp();
						pMover->ResetDestParamEquip( pOldItemProp, pItemElemOld );

						g_xApplyItemEffect->RemoveEquipActiveSkill( pMover, pItemElemOld );

						g_xWSUserManager->AddDoEquip( pMover, -1, pItemElemOld, FALSE );
					}
//sun: 9,10Â÷ Á¦·Ã __ULTIMATE
					prj.m_UltimateWeapon.SetDestParamUltimate( pMover, pItemElemOld, FALSE, PARTS_RWEAPON );

#endif // __WORLDSERVER
					pModel->TakeOffParts( PARTS_RWEAPON );
					if( pEquipInfo )
						pEquipInfo[PARTS_RWEAPON].dwItemID	= 0;
				}
			}
		} // HD_TWO
	} // bEquip == TRUE

	if( bEquip )
	{
		if( pInventory == NULL )
		{
			// ÀÍ½ºÅ©·ç½Ãºê ¾ÆÀÌÅÛ Á¦°Å 
			if( pItemProp->dwExclusive != NULL_ID )
			{
				pModel->TakeOffParts( pItemProp->dwExclusive );
				if( pEquipInfo )
					pEquipInfo[pItemProp->dwExclusive].dwItemID		= 0;
			}
			// ÆäÀÌÅ© ÀåÂø 
			if( pEquipInfo )
				memcpy( &pEquipInfo[dwParts], &rEquipInfo, sizeof(EQUIP_INFO) );
		}
		else
		{
			// ÀÍ½ºÅ©·ç½Ãºê ¾ÆÀÌÅÛ Á¦°Å 
			if( pItemProp->dwExclusive != NULL_ID )
			{
				FLItemElem* pExclusive	= pInventory->GetEquip( pItemProp->dwExclusive );
				if( pExclusive )
				{
					if( pInventory->UnEquip( pItemProp->dwExclusive ) == FALSE )
						return FALSE;
#ifdef __WORLDSERVER
					PT_ITEM_SPEC pProp	= pExclusive->GetProp();
					pMover->ResetDestParamEquip( pProp, pExclusive );

					g_xApplyItemEffect->RemoveEquipActiveSkill( pMover, pExclusive );

					g_xWSUserManager->AddDoEquip( pMover, -1, pExclusive, FALSE );
#endif	// __WORLDSERVER

				}
				pModel->TakeOffParts( pItemProp->dwExclusive );

			}

			// ±âÁ¸ Àåºñ Á¦°Å 
			if( dwParts == PARTS_RING1 || dwParts == PARTS_EARRING1 )
			{
				DWORD dwElem1 = NULL_ID, dwElem2 = NULL_ID;
				FLItemElem* pItemElemTemp1 = (FLItemElem*)pInventory->GetEquip( dwParts );
				FLItemElem* pItemElemTemp2 = (FLItemElem*)pInventory->GetEquip( dwParts + 1 );
				if( pItemElemTemp1 ) dwElem1 = pItemElemTemp1->m_dwItemId;
				if( pItemElemTemp2 ) dwElem2 = pItemElemTemp2->m_dwItemId;
				if( dwElem1 != NULL_ID && dwElem2 == NULL_ID )
					dwParts = pItemProp->dwPartsub;
			}
			FLItemElem *pItemElemOld = pInventory->GetEquip( dwParts );
			if( pItemElemOld )
			{
				if( pInventory->UnEquip( dwParts ) == FALSE )
					return FALSE;
#ifdef __WORLDSERVER
				PT_ITEM_SPEC pOldItemProp = pItemElemOld->GetProp();
				pMover->ResetDestParamEquip( pOldItemProp, pItemElemOld );

				g_xApplyItemEffect->RemoveEquipActiveSkill( pMover, pItemElemOld );

				if( pItemElemOld->m_nResistSMItemId != 0 )	// À¯·áÈ­ ¾ÆÀÌÅÛÀÌ ™VÆÃµÇ¾î ÀÖ´Ù¸é
					((FLWSUser* )pMover)->DoSMItemUnEquip( pItemElemOld, dwParts );
//sun: 9,10Â÷ Á¦·Ã __ULTIMATE
				prj.m_UltimateWeapon.SetDestParamUltimate( pMover, pItemElemOld, FALSE, dwParts );

#endif // worldserver
				
			}
			
			// »õ Àåºñ ÀåÂø 
			if( TRUE == pInventory->DoEquip( dwIndex, /*MAX_INVENTORY +*/ dwParts ) )
			{
				if( pEquipInfo )
				{
//					pEquipInfo[dwParts].dwId	= pItemElem->m_dwObjId;		// TODO : ÀÇµµ°¡ ¹ºÁö ¾Ë¼ö°¡ ¾ø´Ù.
					pEquipInfo[dwParts].dwItemID	= pItemElem->m_dwItemId;
					pEquipInfo[dwParts].nOption		= pItemElem->GetAttrOption();
				}

#ifdef __WORLDSERVER
				if( pItemElem && pItemElem->m_nResistSMItemId != 0 ) // À¯·áÈ­ ¾ÆÀÌÅÛÀÌ ™VÆÃµÇ¾î ÀÖ´Ù¸é
				{
					((FLWSUser* )pMover)->DoSMItemEquip( pItemElem, dwParts );
				}
//sun: 9,10Â÷ Á¦·Ã __ULTIMATE
				prj.m_UltimateWeapon.SetDestParamUltimate( pMover, pItemElem, TRUE, dwParts );

#endif // __WORLDSERVER
			}
			else
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "" ) );
				return FALSE;
			}
		} // not Fake

#ifndef __WORLDSERVER
		TCHAR szPartsName[ 32 ];
		if( pItemProp->bPartsFile == TRUE )
			prj.m_modelMng.MakePartsName( szPartsName, _countof( szPartsName ), _T( "part" ), pItemProp->dwID, nSex );
		else
			prj.m_modelMng.MakeModelName( szPartsName, _countof( szPartsName ), OT_ITEM, pItemProp->dwID );
			//prj.m_modelMng.MakePartsName( szPartsName, _T( "item" ), pItemProp->dwID );
		switch( dwParts )
		{
		case PARTS_RWEAPON: 
			// º¯½Å Ä³¸¯ÅÍ´Â ÀåÂø ¹«±â¸¦ º¼ ¼ö ¾ø°Ô ÇÏÀÚ(ÆÄÃ÷ ÀåÂø¸¸ ¸øÇÔ. ¼öÄ¡ °è»êÀº Àû¿ëµÊ)
			if( pMover == NULL || ( pMover && pMover->IsDisguise() == FALSE ) )
			{
				((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
				switch( pItemProp->dwItemKind3 )
				{
				case IK3_KNUCKLEHAMMER:
					((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRArmIdx() );
					break;
				case IK3_BOW:
#ifdef __ADDWEAPON_CROSSBOW16
					if( pItemProp->nDestData1[0] == 100 )
						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
					else
						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
#else
					((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
#endif
					break;

#ifdef __ADDWEAPON_CROSSBOW16
				case IK3_CROSSBOW:
					((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
					break;
#endif //__ADDWEAPON_CROSSBOW
				case IK3_YOYO:
					((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
					((CModelObject*)pModel)->LoadElement( szPartsName, PARTS_LWEAPON );
					((CModelObject*)pModel)->SetParent( PARTS_LWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
					break;
				default:
					((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
				}
	//			if( pItemProp->dwItemKind3 == IK3_KNUCKLEHAMMER )
	//				((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRArmIdx() );
	//			else
	//				((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
			}
			break;
		case PARTS_LWEAPON: 
			// º¯½Å Ä³¸¯ÅÍ´Â ÀåÂø ¹«±â¸¦ º¼ ¼ö ¾ø°Ô ÇÏÀÚ(ÆÄÃ÷ ÀåÂø¸¸ ¸øÇÔ. ¼öÄ¡ °è»êÀº Àû¿ëµÊ)
			if( pMover == NULL || ( pMover && pMover->IsDisguise() == FALSE ) )
			{
				((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
				((CModelObject*)pModel)->SetParent( PARTS_LWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
			}
			break;
		case PARTS_SHIELD: 
			// º¯½Å Ä³¸¯ÅÍ´Â ÀåÂø ¹«±â¸¦ º¼ ¼ö ¾ø°Ô ÇÏÀÚ(ÆÄÃ÷ ÀåÂø¸¸ ¸øÇÔ. ¼öÄ¡ °è»êÀº Àû¿ëµÊ)
			if( pMover == NULL || ( pMover && pMover->IsDisguise() == FALSE ) )
			{
				((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
				((CModelObject*)pModel)->SetParent( PARTS_SHIELD, ((CModelObject*)pModel)->GetLArmIdx() );
			}
			break;
		case PARTS_UPPER_BODY:
			if( bIfParts )
			{
				((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
				FLSPrintf( lpszTemp, _countof( lpszTemp ), PARTSTEX_UPPER( nSex ), nSkinSet + 1 );
				pModel->ChangeTexture( PARTS_UPPER_BODY, TEX_PART_UPPER( nSex ), lpszTemp );
			}
			break;
		case PARTS_LOWER_BODY:
			if( bIfParts )
			{
				{
					((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
					FLSPrintf( lpszTemp, _countof( lpszTemp ), PARTSTEX_LOWER( nSex ), nSkinSet + 1 );
					pModel->ChangeTexture( PARTS_LOWER_BODY, TEX_PART_LOWER( nSex ), lpszTemp );
				}
			}
			break;
		default:
			if( bIfParts )
			{
				// ³¯°³¸¦ ¸ÁÅä¿¡ ºÎÂøÇÒ·Á°í ½ÃµµÇÑ ÄÚµå		2016.07.11
				//if ( pItemProp->dwItemKind3 != IK3_WING )				// ºñÇàÃ¼ ¸»°í ´Ù¸¥ ÆÄÆ®¿¡ ³¯°³°¡ ÀÖÀ¸¸é Ãâ·Â¾È½ÃÅ´	05
					((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
			}
			break;

		}

#ifdef __CLIENT
		if( PARTS_LWEAPON == dwParts || PARTS_RWEAPON == dwParts || PARTS_SHIELD == dwParts )
			pMover->SetMotion( MTI_STAND, ANILOOP_1PLAY , MOP_NO_TRANS );

#endif //__CLIENT
		
#endif // not WorldServer
	} // ÀåÂø.
	else
	// Å»Âø.
	{
		if( bIfParts )
		{
			if( pItemProp->dwExclusive != NULL_ID )
			{
				switch( pItemProp->dwExclusive )
				{
				case PARTS_UPPER_BODY:
					pModel->LoadElement( PARTSMESH_UPPER( nSex ), PARTS_UPPER_BODY );
					FLSPrintf( lpszTemp, _countof( lpszTemp ), PARTSTEX_UPPER( nSex ), nSkinSet + 1 );
					pModel->ChangeTexture( PARTS_UPPER_BODY, TEX_PART_UPPER( nSex ), lpszTemp );
					break;
				case PARTS_LOWER_BODY:
					{

						PT_ITEM_SPEC pItemPropEquip = NULL;
						if( pItemProp->dwParts == PARTS_CLOTH )
						{
							pItemPropEquip = pMover->GetEquipItemProp( pInventory, pEquipInfo, PARTS_UPPER_BODY );
						}
						else
						{
							pItemPropEquip = pMover->GetEquipItemProp( pInventory, pEquipInfo, PARTS_CLOTH );
						}

						if( pItemPropEquip == NULL )
						{
							pModel->LoadElement( PARTSMESH_LOWER( nSex ), PARTS_LOWER_BODY );
							FLSPrintf( lpszTemp, _countof( lpszTemp ), PARTSTEX_LOWER( nSex ), nSkinSet + 1 );
							pModel->ChangeTexture( PARTS_LOWER_BODY, TEX_PART_LOWER( nSex ), lpszTemp );
						}					
					}					
					break;
				case PARTS_HAND:
					pModel->LoadElement( PARTSMESH_HAND( nSex ), PARTS_HAND );
					break;
				case PARTS_FOOT:
					pModel->LoadElement( PARTSMESH_FOOT( nSex ), PARTS_FOOT );
					break;
				}
			}
		}
		if( pInventory && pInventory->GetEquip( dwParts ) )
		{
			if( pInventory->UnEquip( dwParts ) == FALSE )
			{
#ifdef __WORLDSERVER
				if( TRUE == pMover->IsPlayer() )
					( (FLWSUser*)pMover )->AddDefinedText(TID_GAME_LACKSPACE, "" );
#endif	// __WORLDSERVER				
				return FALSE;
			}

#ifdef __WORLDSERVER
			g_xApplyItemEffect->RemoveEquipActiveSkill( pMover, pItemElem );

//sun: 9,10Â÷ Á¦·Ã __ULTIMATE
			prj.m_UltimateWeapon.SetDestParamUltimate( pMover, pItemElem, FALSE, dwParts );
			if( pItemElem && pItemElem->m_nResistSMItemId != 0 )	// À¯·áÈ­ ¾ÆÀÌÅÛÀÌ ™VÆÃµÇ¾î ÀÖ´Ù¸é
			{
				((FLWSUser* )pMover)->DoSMItemUnEquip( pItemElem, dwParts );
			}
#endif // __WORLDSERVER
		}

		if( pEquipInfo )
		{
			pEquipInfo[dwParts].dwItemID			= 0;
			pEquipInfo[dwParts].nOption				= 0;
			pEquipInfo[dwParts].byFlag				= 0;
			pEquipInfo[dwParts].dwLooksChangeItemID	= 0;
			pEquipInfo[dwParts].bIsCombined			= false;
		}

#ifndef __WORLDSERVER
		if( bIfParts )
		{
			switch( dwParts )
			{
			case PARTS_UPPER_BODY:
				pModel->LoadElement( PARTSMESH_UPPER( nSex ), PARTS_UPPER_BODY );
				FLSPrintf( lpszTemp, _countof( lpszTemp ), PARTSTEX_UPPER( nSex ), nSkinSet + 1 );
				pModel->ChangeTexture( PARTS_UPPER_BODY, TEX_PART_UPPER( nSex ), lpszTemp );
				break;
			case PARTS_LOWER_BODY:
				pModel->LoadElement( PARTSMESH_LOWER( nSex ), PARTS_LOWER_BODY );
				FLSPrintf( lpszTemp, _countof( lpszTemp ), PARTSTEX_LOWER( nSex ), nSkinSet + 1 );
				pModel->ChangeTexture( PARTS_LOWER_BODY, TEX_PART_LOWER( nSex ), lpszTemp );
				break;
			case PARTS_HAND:
				pModel->LoadElement( PARTSMESH_HAND( nSex ), PARTS_HAND );
				break;
			case PARTS_FOOT:
				pModel->LoadElement( PARTSMESH_FOOT( nSex ), PARTS_FOOT );
				break;
			}
		}
#endif

#ifdef __CLIENT
		if( PARTS_LWEAPON == dwParts || PARTS_RWEAPON == dwParts || PARTS_SHIELD == dwParts )
			pMover->SetMotion( MTI_STAND, ANILOOP_1PLAY , MOP_FIXED );
#endif //__CLIENT

	} // Å»Âø


	// ¹þ±æ¶§
	if( bEquip == FALSE )
	{
		// À§¿¡¼­ ¿À¸¥¼Õ ¹«±â¸¦ ÇØÁ¦ Çß¾ú´Ù. 
		if( nPart == PARTS_RWEAPON )
		{
			if( pInventory )
			{
				FLItemElem* pItemLeft = pInventory->GetEquip( PARTS_LWEAPON );
				if( pItemLeft )	// ¿Þ¼Õ¿¡ ¹«±â°¡ ³²¾ÆÀÖ´Ù.
				{
					//¿À¸¥¼ÕÀ¸·Î ¿Å°ÜÁÜ.
					#ifdef __WORLDSERVER
					if( pMover->IsSMMode( SM_RESIST_ATTACK_LEFT ) )
					{
						pMover->m_nAttackResistRight = pMover->m_nAttackResistLeft;
						pMover->m_nAttackResistLeft = 0xff;
						DWORD dwSMTime = pMover->m_dwSMTime[SM_RESIST_ATTACK_LEFT];
						pMover->SetSMMode( SM_RESIST_ATTACK_LEFT, 1 );
						pMover->SetSMMode( SM_RESIST_ATTACK_RIGHT, dwSMTime );
						((FLWSUser*)pMover)->AddResistSMMode( pMover->m_nAttackResistLeft, pMover->m_nAttackResistRight, pMover->m_nDefenseResist );
					}
					#endif // __WORLDSERVER
					
					pInventory->Swap( PARTS_LWEAPON, PARTS_RWEAPON );	
#ifdef __WORLDSERVER			
//sun: 9,10Â÷ Á¦·Ã __ULTIMATE
					prj.m_UltimateWeapon.SetDestParamUltimate( pMover, pItemLeft, TRUE, PARTS_RWEAPON );
#endif // __WORLDSERVER

				#ifdef __CLIENT
					((CModelObject*)pModel)->MovePart( PARTS_RWEAPON, PARTS_LWEAPON );
					((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
				#endif //__CLIENT
				}
			}
			else
			{	// fake
				if( pEquipInfo )
				{
					if( pEquipInfo[PARTS_LWEAPON].dwItemID != 0 )
					{
						memcpy( &pEquipInfo[PARTS_RWEAPON], &pEquipInfo[PARTS_LWEAPON], sizeof(EQUIP_INFO) );
						pEquipInfo[PARTS_LWEAPON].dwItemID	= 0;
						pEquipInfo[PARTS_LWEAPON].nOption	= 0;
						pEquipInfo[PARTS_LWEAPON].byFlag	= 0;
					#ifdef __CLIENT
						((CModelObject*)pModel)->MovePart( PARTS_RWEAPON, PARTS_LWEAPON );
						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
					#endif	// __CLIENT
					}
				}
			}
		}
	}

#ifdef __CLIENT
	D3DXVECTOR3 vPos = pMover->GetPos();
				
	if( bEquip )
	{
		if( pItemProp )
		{
			if( pItemProp->dwItemKind1 == IK1_WEAPON )
			{
				PLAYSND( SND_INF_EQUIPWEAPON, &vPos );
			}
			else
			if( pItemProp->dwItemKind1 == IK1_GENERAL && pItemProp->dwItemKind2 == IK2_JEWELRY )
			{
				PLAYSND( SND_INF_EQUIPACCESSORY, &vPos );
			}
			else
			if( pItemProp->dwItemKind1 == IK1_ARMOR && pItemProp->dwItemKind2 == IK2_CLOTH )
			{
				PLAYSND( SND_INF_EQUIPCLOTH, &vPos );
			}
			else
			if( pItemProp->dwItemKind1 == IK1_ARMOR && pItemProp->dwItemKind2 == IK2_ARMOR 
				|| pItemProp->dwItemKind2 == IK2_ARMORETC )
			{
				PLAYSND( SND_INF_EQUIPARMOR, &vPos );
			}	
		}
	}
	else
	{
		PLAYSND( SND_INF_INVENTORYDROP, &vPos );		
	}
#endif //__CLIENT

	return TRUE;
}

// ÀåÂø °¡´ÉÇÑ°¡¸¦ °Ë»ç.
//sun: 11, °¢¼º, Ãàº¹
BOOL CMover::IsEquipAble( FLItemElem* pItem,BOOL bIgnoreLevel )
{
//sun: 11, °¢¼º, Ãàº¹
	if( !pItem )
		return FALSE;
	PT_ITEM_SPEC pItemProp	= pItem->GetProp();
//	OBJID dwObjid	= pItem->m_dwObjId;

	// ºøÀÚ·ç·ù °Ë»ç.
	if( pItemProp->dwParts == PARTS_RIDE )
	{
		int nLimitLv = pItemProp->dwFlightLimit;
		if( nLimitLv == NULL_ID )
			nLimitLv = 1;
//sun: all, __NOLIMIT_RIDE_ITEM
		if( pItemProp->dwID == ITEM_INDEX( 26514, II_RID_RID_STI_MAGIC01 ) )
			nLimitLv = 0;
		
		CWorld* pWorld = GetWorld();
		if( pWorld )
		{
			int nAttr	= pWorld->GetHeightAttribute( GetPos().x, GetPos().z );
			
			if( GetFlightLv() < nLimitLv )		// ºñÇà·¹º§ÀÌ ¾ÈµÇ¸é ¸øÅº´Ù.
			{
#ifdef __WORLDSERVER
				if( TRUE == IsPlayer() )
					( (FLWSUser*)this )->AddDefinedText( TID_GAME_USEAIRCRAFT, "" );	// Àá½Ã¸¸ ±â´Ù·ÁÁÖ¼¼¿ä.
#else // __WORLDSERVER
				g_WndMng.PutString( prj.GetText( TID_GAME_USEAIRCRAFT ), NULL, prj.GetTextColor( TID_GAME_USEAIRCRAFT ) );
#endif // __WORLDSERVER
				return FALSE;
			}
			
			if( !pWorld->m_bFly )
			{
#ifdef __WORLDSERVER
				( (FLWSUser*)this )->AddDefinedText( TID_ERROR_NOFLY, "" );	// ºñÇà±ÝÁö±¸¿ªÀÔ´Ï´Ù.
#else // __WORLDSERVER
				g_WndMng.PutString( prj.GetText( TID_ERROR_NOFLY ), NULL, prj.GetTextColor( TID_ERROR_NOFLY ) );
#endif // __WORLDSERVER
				return FALSE;
			}

			if( HasBuffByIk3( IK3_TEXT_DISGUISE ) )
			{
#ifdef __WORLDSERVER
				( (FLWSUser*)this )->AddDefinedText( TID_QUEST_DISQUISE_NOTFLY, "" );	// º¯½ÅÁß¿¡´Â ºñÇàÀ» ÇÒ¼ö ¾ø½À´Ï´Ù
#else // __WORLDSERVER
				g_WndMng.PutString( prj.GetText( TID_QUEST_DISQUISE_NOTFLY ), NULL, prj.GetTextColor( TID_QUEST_DISQUISE_NOTFLY ) );
#endif // __WORLDSERVER
				return FALSE;
			}
			
			// ÀåÂø ÀÌÀü¿¡ °Ë»çÇÏ¿©¾ß ÇÑ´Ù.
			if( nAttr == HATTR_NOFLY )		// ºñÇà±ÝÁö±¸¿ª¿¡¼± ¸øÅº´Ù.
				return FALSE;
			
		#ifdef __WORLDSERVER
			if( m_pActMover->IsState( OBJSTA_STAND ) == FALSE )
				return FALSE;
		#endif 
		}

//sun: 8, // __S8_PK
#ifdef __WORLDSERVER
		if( IsChaotic() )
		{
			CHAO_PROPENSITY Propensity = prj.GetPropensityPenalty( GetPKPropensity() );
			if( !Propensity.nFly )
			{
				((FLWSUser*)this)->AddDefinedText( TID_GAME_CHAOTIC_NOT_FLY );
				return FALSE;
			}
		}
#endif // __WORLDSERVER

//sun: 9, 9-10Â÷ Æê
#ifdef __WORLDSERVER
		if( HasActivatedEatPet() || HasActivatedSystemPet() )	// ÆêÀÌ ¼ÒÈ¯µÈ »óÅÂ¶ó¸é ºñÇà ºÒ°¡
		{
			( (FLWSUser*)this )->AddDefinedText( TID_GAME_CANNOT_FLY_WITH_PET );
			return FALSE;
		}
#endif	// __WORLDSERVER
	}

	// ¹«±â·ù°¡ ¾Æ´Ñ°Í(¹æ¾î±¸)´Â ¼ºº°À» È®ÀÎÇÑ´Ù.
	{
		// ¼ºº° È®ÀÎ( ¹«±â´Â ºüÁü )
		if( pItemProp->dwItemSex != NULL_ID && pItemProp->dwItemSex != GetSex() )
		{
#ifdef __WORLDSERVER
			if( TRUE == IsPlayer() )
				( (FLWSUser*)this )->AddDefinedText( TID_GAME_WRONGSEX, "\"%s\"", pItemProp->szName );
#endif	// __WORLDSERVER
			return FALSE;
		} // ¼ºº°
	}// ¹«±â
	
	// ¹æ¾î±¸ ÀÔÀ»¶§ Á÷¾÷È®ÀÎ.
	if( pItemProp->dwItemKind1 != IK1_WEAPON )
	{
		if( pItemProp->dwItemJob != NULL_ID && FALSE == IsInteriorityJob( pItemProp->dwItemJob ) ) {
#ifdef __WORLDSERVER
			if( TRUE == IsPlayer() )
				( (FLWSUser*)this )->AddDefinedText( TID_GAME_WRONGJOB, "\"%s\"", pItemProp->szName );
#endif	// __WORLDSERVER
			return FALSE;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// mirchang_100820 ±â°£Á¦ ¾ÆÀÌÅÛ ¼³Á¤..
	if( pItem->IsFlag( FLItemElem::expired ) == TRUE )
	{
#ifdef __WORLDSERVER
		if( IsPlayer() == TRUE )
		{
			( (FLWSUser*)this )->AddDefinedText( TID_GAME_ITEM_EXPIRED );
		}
#endif	// __WORLDSERVER
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////

	// ¾ÆÀÌÅÛ ÇÊ¿äÁ÷¾÷°Ë»ç.
	if( pItemProp->dwItemKind1 == IK1_WEAPON )		// ¹«±â·ù¸¦ ÀåÂøÇÏ·ÁÇÒ¶§
	{
		if( pItemProp->dwItemJob != NULL_ID && FALSE == IsInteriorityJob( pItemProp->dwItemJob ) )	// ¾ÆÀÌÅÛ¿¡ ÇÊ¿äÁ÷¾÷ÀÌ ÁöÁ¤µÇ¾î ÀÖ°í
		{
#ifdef __WORLDSERVER
			if( TRUE == IsPlayer() )
				( (FLWSUser*)this )->AddDefinedText( TID_GAME_WRONGJOB, "\"%s\"", pItemProp->szName );
#endif	// __WORLDSERVER
			return FALSE;
		}
	}

	if( !g_eLocal.GetState( EVE_SCHOOL ) && !bIgnoreLevel )
	{

		// ¾ÆÀÌÅÛ ÇÊ¿ä·¹º§ °Ë»ç.
		if( pItem->CanUseLevel( this ) == false )	// ·¹º§ÀÌ ¾ÈµÇ¸é ¸øÂü.
		{
#ifdef __WORLDSERVER
			if( TRUE == IsPlayer() )
			{
				//sun: 11, °¢¼º, Ãàº¹
				( (FLWSUser*)this )->AddDefinedText( TID_GAME_REQLEVEL, "\"%d\"", pItem->GetMinLimitLevel() );
			}
#endif	// __WORLDSERVER
			return FALSE;
		}
		// º¸¼®·ù¸¦ ÀåÂø
		if( pItemProp->dwItemKind2 == IK2_JEWELRY )
		{
			if( pItem->CanUseLevel( this ) == false )	// ·¹º§ÀÌ ¾ÈµÇ¸é ¸øÂü.
			{
#ifdef __WORLDSERVER
				if( TRUE == IsPlayer() )
				{
					//sun: 11, °¢¼º, Ãàº¹
					( (FLWSUser*)this )->AddDefinedText( TID_GAME_REQLEVEL, "\"%d\"", pItem->GetMinLimitLevel() );
				}
#endif	// __WORLDSERVER
				return FALSE;
			}
		}

	}

	PT_ITEM_SPEC pHandItemProp	= NULL;
	
	// µé°íÀÖ´Â ¹«±â ÇÁ·ÎÆÛÆ¼ ²¨³¿.
	FLItemElem *pItemElem = m_Inventory.GetEquip( PARTS_RWEAPON );		
	if( pItemElem )
	{
		pHandItemProp = pItemElem->GetProp();
	}

	// È­»ìÀ» Âø¿ëÇÏ·Á ÇßÀ»¶§ º¸¿ìÀÏ¶§¸¸ Âø¿ëµÅ¾ß ÇÔ
	if( pItemProp->dwItemKind3 == IK3_ARROW && ( pHandItemProp == NULL || pHandItemProp->dwItemKind3 != IK3_BOW ) )
		return FALSE;			

#ifdef __ADDWEAPON_CROSSBOW16
	if( pItemProp->dwItemKind3 == IK3_CROSSARROW && ( pHandItemProp == NULL || pHandItemProp->dwItemKind3 != IK3_CROSSBOW ) )
		return FALSE;
#endif //__ADDWEAPON_CROSSBOW16

	return TRUE;
}

//
//
// pItemElemÀÌ ¹þ´Â°Ô °¡´ÉÇÑ°¡¸¦ °Ë»ç.
BOOL CMover::IsUnEquipAble( PT_ITEM_SPEC pItemProp )
{
	if( pItemProp->dwParts == PARTS_RIDE )
	{
		if( m_pActMover->IsSit() )	// ºøÀÚ·ç´Â ¾ÉÀº »óÅÂ¿¡¼­´Â Å»Âø ±ÝÁö
			return FALSE;

		if( GetWorld() )	// RedoEquip¿¡¼­ ºÒ·¶À»¶§ NULLÀÎ°æ¿ì ÀÖÀ½. NULLÀÌ¸é °Á ¹þ±è
		{
			int nAttr = GetWorld()->GetHeightAttribute( GetPos().x, GetPos().z );		// ÀÌµ¿ÇÒ À§Ä¡ÀÇ ¼Ó¼º ÀÐÀ½.
			// ºñÇà±ÝÁö or °È±â±ÝÁö or ÀÌµ¿±ÝÁö Áö¿ª¿¡¼­ ´Â ¸ø³»¸².
			if( (nAttr == HATTR_NOFLY || nAttr == HATTR_NOWALK || nAttr == HATTR_NOMOVE ) )		
				return FALSE;
		}
	}

	if( g_eLocal.GetState( EVE_SCHOOL ) && pItemProp->dwItemKind3 == IK3_CLOAK )
		return FALSE;
	
	return TRUE;
}

//
//
// Á¤»óÀûÀÎ ¾ÆÀÌÅÛ ÀåÂø, Å»Âø 
BOOL CMover::DoEquip( FLItemElem* pItemElem, BOOL bEquip, int nPart )
{
	PT_ITEM_SPEC pItemProp = pItemElem->GetProp();
	
	if( IsDie() )	// Á×Àº »óÅÂ¿¡¼­´Â Àå/Å»Âø ±ÝÁö
	{
		return FALSE;
	}

	if( bEquip )
	{
		if( pItemProp->dwParts == PARTS_RIDE && m_Inventory.GetEquip( PARTS_RIDE ) )
			return FALSE;
		if( pItemProp->dwParts == PARTS_RIDE && pItemElem && pItemElem->IsFlag( FLItemElem::expired ) )
			return FALSE;
//sun: 11, °¢¼º, Ãàº¹
		if( IsEquipAble( pItemElem ) == FALSE )
			return FALSE;
	}
	else
	{
		if( IsUnEquipAble( pItemProp ) == FALSE )
			return FALSE;
	}

	EQUIP_INFO equipInfo;
	equipInfo.dwItemID	= pItemElem->m_dwItemId;
	equipInfo.nOption	= pItemElem->GetAttrOption();
	equipInfo.byFlag	= pItemElem->m_byFlag;
	BOOL bResult = DoEquip( GetSex(), m_dwSkinSet, pItemElem, nPart, equipInfo, &m_Inventory, m_aEquipInfo, (CModelObject*)m_pModel, bEquip, this );
		
	if( !bResult )	// 2004/04/27
		return FALSE;

#ifndef __CLIENT
	if( bEquip )
	{
		SetDestParamEquip( pItemProp, pItemElem );		// ÀåÂøÇÑ ÀåºñÀÇ DestParam¼³Á¤

		if( pItemProp->nBindCondition == BIND_CONDITION_USE
			&& pItemElem->IsFlag( FLItemElem::binds ) == FALSE
			&& pItemElem->IsFlag( FLItemElem::reset_bind ) == FALSE )
		{
			pItemElem->SetFlag( FLItemElem::binds );
			UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_FLAG, MAKELONG( pItemElem->m_dwObjIndex, pItemElem->m_byFlag ) );
		}

		else if( pItemProp->nResetBindCondition == BIND_CONDITION_USE
			&& pItemElem->IsFlag( FLItemElem::binds ) == FALSE
			&& pItemElem->IsFlag( FLItemElem::reset_bind ) == TRUE )
		{
			pItemElem->SetFlag( FLItemElem::binds );
			UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_FLAG, MAKELONG( pItemElem->m_dwObjIndex, pItemElem->m_byFlag ) );
		}
	}
	else
	if( bEquip == FALSE )
	{
		ResetDestParamEquip( pItemProp, pItemElem );	// ÇØÁ¦ÇÏ´Â ÀåºñÀÇ DestParam¸®¼Â
		
		if( pItemProp->dwItemKind1 == IK1_WEAPON ) 
		{
			if( m_pActMover->IsStateFlag( OBJSTAF_COMBAT ) )
				SendActMsg( OBJMSG_MODE_PEACE );
		}

	}
#endif	// __CLIENT

	if( pItemProp->dwParts == PARTS_RIDE )
	{
		if( bEquip )
		{
#ifdef __CLIENT	
			if( IsPlayer() )
			{
				if( g_WndMng.m_pWndWorld->m_bFirstFlying == FALSE )
				{
					PlayMusic(BGM_TH_FLYING);
					g_WndMng.m_pWndWorld->m_bFirstFlying = TRUE;
				}
				else
				{
					PlayMusic(BGM_EV_LIGHTWING);
				}
				g_WndMng.m_bAutoRun = FALSE;
			}
#endif//__CLIENT				
			SendActMsg( OBJMSG_MODE_FLY, pItemProp->dwID, 0, 1 );
		}
		else
		{
			SendActMsg( OBJMSG_MODE_GROUND, 0, 0, 1 );
		}
		ClearDest();	// 2004/04/30
	}

	// ¸ÁÅä ÀÔÀ»¶§ Ã³¸®.
	if( bEquip )
	{
		if( pItemProp->dwID == ITEM_INDEX( 4601, II_ARM_S_CLO_CLO_BLANK ) )		// ±æµåÀü¿ë ¹ÎÂ¥ ¸ÁÅä.
		{
			m_idGuildCloak = pItemElem->m_idGuild;			// ¸ÁÅä¿¡ ¹ÚÇôÀÖ´ø ±æµå¾ÆÀÌµð¸¦ ¹ÞÀ½.
#ifdef __XCLOAK
			m_pCloakTexture = g_GuildTexture.LoadGuildTexture( pItemElem->m_idGuild );
#endif
		} 
		else
		{
#ifdef __XCLOAK
			m_pCloakTexture = NULL;
			m_idGuildCloak = 0;			// ¸ÁÅä¿¡ ¹ÚÇôÀÖ´ø ±æµå¾ÆÀÌµð¸¦ ¹ÞÀ½.
#endif
		}
	}

	UpdateBoundBox();
	UpdateParam();

	return bResult;
}


#ifdef __CLIENT
// ÀåÂø½Ã dwItemIndex¸¸À» »ç¿ëÇÏ´Â ÀÌ Method´Â Fake ¾ÆÀÌÅÛÀ» À§ÇØ »ç¿ë 
// pModel¿¡ °ªÀÌ ÀÖ´Â °æ¿ì´Â Á» Æ¯º°ÇÑ °æ¿ì´Ù. ±× °æ¿ì´Â ÀåÂø/Å»Âø ÈÄ¿¡µµ ³»ºÎ ÀåÂø¿¡ °üÇÑ ¼¼ÆÃ¿¡ º¯°æÀÌ ¾ø°Ô µÈ´Ù.
BOOL CMover::DoFakeEquip( const EQUIP_INFO & rEquipInfo, BOOL bEquip, int nPart, CModelObject* pModel )
{
	BOOL bResult;
	if( pModel )
		bResult	= CMover::DoEquip( GetSex(), m_dwSkinSet, NULL, nPart, rEquipInfo, NULL, m_aEquipInfo, (CModelObject*)pModel, bEquip, this );
	else
		bResult = CMover::DoEquip( GetSex(), m_dwSkinSet, NULL, nPart, rEquipInfo, NULL, m_aEquipInfo, (CModelObject*)m_pModel, bEquip, this );

	if( !bResult )	// 2004/04/27
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "return FALSE" ) );
		return FALSE;
	}

	UpdateBoundBox();
	UpdateParam();

	PT_ITEM_SPEC pProp = g_xSpecManager->GetSpecItem( rEquipInfo.dwItemID );
#ifndef __CLIENT
	if( pProp && pProp->dwItemKind1 >= IK2_WEAPON_DIRECT && pProp->dwItemKind1 <= IK2_WEAPON_MAGIC )
	{
		if( bEquip ) {
			if( !m_pActMover->IsStateFlag( OBJSTAF_COMBAT ) )
				SendActMsg( OBJMSG_MODE_COMBAT );
		}
		else {
			if( m_pActMover->IsStateFlag( OBJSTAF_COMBAT ) )
				SendActMsg( OBJMSG_MODE_PEACE );
		}
	}
#endif	// __CLIENT
	
	
	if( pProp->dwParts == PARTS_RIDE )
	{
		if( bEquip )
		{
			SendActMsg( OBJMSG_MODE_FLY, pProp->dwID, 0, 1 );
			ClearDest();
		}
		else
		{
			SendActMsg( OBJMSG_MODE_GROUND, 0, 0, 1 );
			ClearDest();
		}
	}
	// ¸ÁÅä ÀÔÀ»¶§ Ã³¸®.
	if( bEquip )
	{
		if( pProp->dwID == ITEM_INDEX( 4601, II_ARM_S_CLO_CLO_BLANK ) )		// ±æµåÀü¿ë ¹ÎÂ¥ ¸ÁÅä.
		{
#ifdef __XCLOAK
			m_pCloakTexture = g_GuildTexture.LoadGuildTexture( m_idGuildCloak );
#endif
		} else
		{
#ifdef __XCLOAK
			m_pCloakTexture = NULL;
#endif
		}
	}

	return bResult;
}
#endif // CLIENT

void CMover::RedoEquip( BOOL fFakeParts, BOOL bDestParam )
{
	FLItemElem* pItemElem = NULL;
	PT_ITEM_SPEC pItemProp	= NULL;
	BOOL bEquip;

	// ÀåÂøµÉ ¾ÆÀÌÅÛÀÇ ¼± Ã³¸® 
	for( DWORD i = 0; i < MAX_HUMAN_PARTS; i++ )
	{
		bEquip	= fFakeParts? ( m_aEquipInfo[i].dwItemID != 0 ) : ( m_Inventory.GetEquip( i ) != NULL );
		if( bEquip )
		{
			pItemElem = NULL;			pItemProp = NULL;
			
			// ÀåÂøµÈ ¾ÆÀÌÅÛÀÇ ÇÁ·ÎÆÛÆ¼ ²¨³¿.
			if( !fFakeParts )	// ÁøÂ¥ Àåºñ³Ä.
			{
				pItemElem	= m_Inventory.GetEquip( i );
				if( NULL != pItemElem )
					pItemProp	= pItemElem->GetProp();
			}
			else
			{
				pItemProp	= g_xSpecManager->GetSpecItem( m_aEquipInfo[i].dwItemID );
			}

			
				
			if( ( pItemProp != NULL && pItemProp->dwID > 0 && 
				( pItemProp->dwParts == i || pItemProp->dwPartsub == i || 
				  (pItemProp->dwParts == PARTS_RWEAPON && i == PARTS_LWEAPON) )		// RWEAPONÀº LWEAPON°ú °°Àº°ÍÀ¸·Î Ä£´Ù.
				) &&
				( fFakeParts || m_Inventory.IsEquip( pItemElem->m_dwObjId ) ) )
			{
				// ºøÀÚ·ç¸¦ ÀåÂøÇÒ²«µ¥
				if( i == PARTS_RIDE )
				{
					if( FALSE == m_pActMover->IsFly() )	// ºñÇàÁßÀÌ ¾Æ´Ï¸é
					{
//						FLERROR_LOG( PROGRAM_NAME, _T( "OBJSTAF_FLY" ) );
						SendActMsg( OBJMSG_MODE_FLY, pItemProp->dwID, 0, 1 );	// ºñÇà¸ðµå·Î ÀüÈ¯
					}
					if( m_dwRideItemIdx == 0 )	// ºøÀÚ·ç ID°¡ ¾øÀ¸¸é
					{
//						FLERROR_LOG( PROGRAM_NAME, _T( "m_dwRideItemIdx	= pItemProp->dwID" ) );
						m_dwRideItemIdx	= pItemProp->dwID;	// °ª º¸Á¤.
					}
				} 
				if( pItemProp->dwID == ITEM_INDEX( 4601, II_ARM_S_CLO_CLO_BLANK ) )		// ±æµåÀü¿ë ¹ÎÂ¥ ¸ÁÅä.
				{
					if( pItemElem )		// ¼­¹ö / Å¬¶ó(¾×Æ¼ºê¹«¹ö)
						m_idGuildCloak = pItemElem->m_idGuild;
 #ifdef __XCLOAK
					m_pCloakTexture = g_GuildTexture.LoadGuildTexture( m_idGuildCloak );
 #endif
				}
				if( pItemElem )
					m_aEquipInfo[i].nOption	= pItemElem->GetAttrOption();

				if( bDestParam )
				{
					SetDestParamEquip( pItemProp, pItemElem, TRUE );
//sun: 9,10Â÷ Á¦·Ã __ULTIMATE
					if( i == PARTS_RWEAPON )
						prj.m_UltimateWeapon.SetDestParamUltimate( this, pItemElem, TRUE, PARTS_RWEAPON );
				}
			}
			else
			{
				//gmpbigsun( 20120227 ) : logÁ¦°ÅµÊ
				//FLERROR_LOG( PROGRAM_NAME, _T( "Illegal dwParts, %s, %d" ), m_szName, i );
				m_aEquipInfo[i].dwItemID	= 0;
			}
		}
		else 
		{	// ÀåÂøµÉ°Ô ¾øÀ½.
			if( i == PARTS_RIDE )	// ºøÀÚ·ç°¡ ÀåÂøµÇ¾î ÀÖÁö ¾ÊÀºµ¥
			{
				if( m_pActMover->IsFly() )	// ºñÇàÁßÀÌ´Ù.
				{
//					FLERROR_LOG( PROGRAM_NAME, _T( "OBJSTAF_GROUND" ) );
					SendActMsg( OBJMSG_MODE_GROUND, 0, 0, 1 );	// ±×·¯¸é Áö»ó¸ðµå·Î º¯È¯ ½ÃÄÑ¾ßÁö.
				}
				if( m_dwRideItemIdx != 0 )		// ºøÀÚ·ç ÀÎµ¦½ºµµ 0ÀÌ¾î¾ß ÇÏ´Âµ¥ ¹º°¡ ÀÖ´Ù.
				{
//					FLERROR_LOG( PROGRAM_NAME, _T( "m_dwRideItemIdx	= 0" ) );
					m_dwRideItemIdx		= 0;	// ±×·¯¸é 0À¸·Î Å¬¸®¾î.
				}
				if( m_Inventory.GetEquip( i ) )
				{
//					FLERROR_LOG( PROGRAM_NAME, _T( "pRide is not null" ) );
					m_Inventory.UnEquip( i );
				}
			}
		}
	}


	pItemElem	= m_Inventory.GetEquip( PARTS_RWEAPON );		// À§¿¡ ·çÇÁ¿¡¼­ ¸ðµçÆÄÃ÷¸¦ ÀÌ·¸°Ô ÀÏ°ýÀûÀ¸·Î ³Ö¾îµµ µÉµí ½Í´Ù.
	if( pItemElem )
	{
		m_aEquipInfo[PARTS_RWEAPON].nOption	= pItemElem->GetAttrOption();
	}
	pItemElem	= m_Inventory.GetEquip( PARTS_LWEAPON );
	if( pItemElem )
	{
		m_aEquipInfo[PARTS_LWEAPON].nOption	= pItemElem->GetAttrOption();
	}

	// ¾ÆÀÌÅÛÀÌ ½ÇÁ¦ ÀåÂø.
	UpdateParts( fFakeParts );

#ifdef __WORLDSERVER
	InvalidEquipOff( fFakeParts );		// ÀåÂøµÇ¾î¼³ ¾ÈµÉ Àåºñ°¡ ÀåÂøµÇ¾î ÀÖ´Ù¸é ¹þ±è
#endif

	if( bDestParam )
	{
		int nAbilityOption	= GetSetItem();
		if( nAbilityOption > 0 )
			SetSetItemAvail( nAbilityOption );

		
		SetDestParamSetItem( NULL );
	}
}

// ÀåÂøµÇ¾î¼³ ¾ÈµÉ Àåºñ°¡ ÀåÂøµÇ¾î ÀÖ´Ù¸é ¹þ±è
int	CMover::InvalidEquipOff( BOOL bFakeParts )
{
	FLItemElem* pItemElem = NULL;
	PT_ITEM_SPEC pItemProp	= NULL;
	int		i;

	for( i = 0; i < MAX_HUMAN_PARTS; i ++ )
	{
		pItemElem = NULL;		
		pItemProp = NULL;

		// ÀåÂøµÈ ¾ÆÀÌÅÛÀÇ ÇÁ·ÎÆÛÆ¼ ²¨³¿.
		if( bFakeParts )	// Fake Àåºñ
		{
			if( m_aEquipInfo[i].dwItemID	== 0 )	continue;
			pItemProp	= g_xSpecManager->GetSpecItem( m_aEquipInfo[i].dwItemID );
		}
		else
		{
			pItemElem = m_Inventory.GetEquip( i );
			if( pItemElem )
				pItemProp  = pItemElem->GetProp();	
		}

		if( pItemProp == NULL )	
			continue;	// ÇÁ·ÎÆÛÆ¼ ¾øÀ¸¸é ½ÇÆÐ.

//sun: 8, // __S8_PK
//sun: 11, °¢¼º, Ãàº¹
		if( IsEquipAble( pItemElem,TRUE ) == FALSE )	// ÀåÂøÇÒ ¼ö ¾ø´Â ¾ÆÀÌÅÛÀÌ´Ù.
//!		if( IsEquipAble( pItemProp, pItemElem->m_dwObjId, TRUE ) == FALSE )	// ÀåÂøÇÒ ¼ö ¾ø´Â ¾ÆÀÌÅÛÀÌ´Ù.
//!		if( IsEquipAble( pItemProp, pItemElem->m_dwObjId) == FALSE )	// ÀåÂøÇÒ ¼ö ¾ø´Â ¾ÆÀÌÅÛÀÌ´Ù.
		{
			if( pItemElem )
			{
				if( DoEquip( pItemElem, FALSE ) == FALSE )		// ¹þ±è.
				{
				#ifdef __WORLDSERVER
					// ¹þ±â´Âµ¥ ½ÇÆÐÇÔ. ÀÎº¥ÀÌ ²ËÃ¡´Ù°Å³ª ±âÅ¸µîµî ÀÌÀ¯
					#ifndef _DEBUG
						FLERROR_LOG( PROGRAM_NAME, _T( "¾ÆÅÛ ¹þ±â´Âµ¥ ½ÇÆÐ:%s" ), GetName() );
					#endif
				#endif
				}
			} 
		}
	} // for
	
	return FALSE;
}

#ifdef __ZCK_ARME_RARE
static int GetZckArmeRareEquipParamValue( FLItemElem* pItemElem, int nBaseValue )
{
	if( pItemElem == NULL || pItemElem->IsZckArmeRareEligible() == FALSE )
		return nBaseValue;

	const int nRarity = static_cast<int>( pItemElem->GetZckArmeRareLevel() );
	const ZckArmeRareProp* pRarity = prj.GetZckArmeRare( nRarity );
	if( pRarity == NULL || nRarity <= ZCK_ARME_RARE_MIN_LEVEL )
		return nBaseValue;

	return static_cast<int>( static_cast<float>( nBaseValue ) * pRarity->m_fStatsRate );
}
#endif // __ZCK_ARME_RARE

void CMover::SetDestParamEquip( PT_ITEM_SPEC pItemProp, FLItemElem* pItemElem, BOOL bIgnoreSetItem )
{
	if( pItemProp == NULL || pItemElem == NULL )
	{
		return;
	}

	if( pItemElem->IsFlag( FLItemElem::expired ) )
	{
		return;
	}

	for( size_t Nth = 0; Nth < MAX_ITEM_PARAM; ++Nth )
	{
		if( pItemProp->dwDestParam[ Nth ] != NULL_ID )
		{
#ifdef __ZCK_ARME_RARE
			const int nAdjParamVal = GetZckArmeRareEquipParamValue( pItemElem, pItemProp->nAdjParamVal[ Nth ] );
			SetDestParam( pItemProp->dwDestParam[ Nth ], nAdjParamVal, pItemProp->dwChgParamVal[ Nth ] );
#else
			SetDestParam( pItemProp->dwDestParam[ Nth ], pItemProp->nAdjParamVal[ Nth ], pItemProp->dwChgParamVal[ Nth ] );
#endif // __ZCK_ARME_RARE
		}
	}
// 	if( pItemProp->dwDestParam[0] != -1 )
// 	{
// 		SetDestParam( pItemProp->dwDestParam[0], pItemProp->nAdjParamVal[0], pItemProp->dwChgParamVal[0] );
// 	}
// 	if( pItemProp->dwDestParam[1] != -1 )
// 	{
// 		SetDestParam( pItemProp->dwDestParam[1], pItemProp->nAdjParamVal[1], pItemProp->dwChgParamVal[1] );
// 	}
// 	if( pItemProp->dwDestParam[2] != -1 )
// 	{
// 		SetDestParam( pItemProp->dwDestParam[2], pItemProp->nAdjParamVal[2], pItemProp->dwChgParamVal[2] );
// 	}
	
	// A¢¯A¢¬A¨ùA’A¨¬A¡Æ A¨ùA“A¨ùA¨¬A€AŒ A¨¬A™A€A¨¬ A¨úA†A€AŒA…A›A€AA¡ÆA|A¢¯A¡þ A¨ùA¨ùA†AƒA‡A”.
	
	// A©öA|A¨úA¢çA¡¾A¢¬A€AŒA¡ÆA¡© A¨ùA“A¨ùA¨¬A¢¯A‰A¨ùA‡A¡ÆA¨£A€AŒ A€A–A€A¢¬A¢¬A¨Ï...
	if( pItemElem && pItemElem->m_nResistAbilityOption && pItemElem->GetProp()->dwItemKind1 == IK1_ARMOR )
	{
		if( pItemElem->m_byItemResist != SAI79::NO_PROP )
		{
			int nStrong = GetStrongDST( pItemElem->m_byItemResist );
			int nWeak   = GetWeakDST( pItemElem->m_byItemResist );

			SetDestParam( nWeak,  -pItemElem->m_nResistAbilityOption*2, NULL_CHGPARAM );
			SetDestParam( nStrong, pItemElem->m_nResistAbilityOption*2, NULL_CHGPARAM );
		}
	}
	else
	{
		if( pItemProp->nItemResistElecricity )
			SetDestParam( DST_RESIST_ELECTRICITY, pItemProp->nItemResistElecricity, NULL_CHGPARAM );
		if( pItemProp->nItemResistFire )
			SetDestParam( DST_RESIST_FIRE, pItemProp->nItemResistFire, NULL_CHGPARAM );
		if( pItemProp->nItemResistWind )
			SetDestParam( DST_RESIST_WIND, pItemProp->nItemResistWind, NULL_CHGPARAM );
		if( pItemProp->nItemResistWater )
			SetDestParam( DST_RESIST_WATER, pItemProp->nItemResistWater, NULL_CHGPARAM );
		if( pItemProp->nItemResistEarth )
			SetDestParam( DST_RESIST_EARTH, pItemProp->nItemResistEarth, NULL_CHGPARAM );
	}

//sun: 11, AƒA¢´AA¨ö A¨öAƒA¨öA¨¬A…A›
	CAccessoryProperty* pProperty	= CAccessoryProperty::GetInstance();
	if( pItemElem && pItemElem->IsAccessory() )		// A¨úA—A¨ùA¨ùA¨ùA¡©A¢¬A¢ç
	{
		const std::vector<SINGLE_DST>* pDst	= pProperty->GetDst( pItemElem->m_dwItemId, pItemElem->GetAbilityOption() );
		if( pDst != NULL && pDst->empty() == false )
		{
			const size_t nSize	= pDst->size();
			for( size_t i = 0; i < nSize; i++ )
			{
				SetDestParam( (*pDst).at( i ).nDst, (*pDst).at( i ).nAdj, NULL_CHGPARAM );
			}
		}
	}

#ifdef __WORLDSERVER
	for( size_t Nth = 0; Nth < MAX_ITEM_PARAM; ++Nth )
	{
		if( pItemProp->dwDestParam[ Nth ] == DST_GIFTBOX )
		{
			AddBuff( BUFF_EQUIP, static_cast<WORD>( pItemProp->dwID ), 1, 999999999, 0, BT_TICK );	// TODO
			break;
		}
	}

// 	if( pItemProp->dwDestParam[0] == DST_GIFTBOX || pItemProp->dwDestParam[1] == DST_GIFTBOX || pItemProp->dwDestParam[2] == DST_GIFTBOX )
// 		AddBuff( BUFF_EQUIP, (WORD)( pItemProp->dwID ), 1, 999999999, 0, BT_TICK );	// TODO
#endif // __WORLDSERVER

	if( !bIgnoreSetItem )
	{
		if( pItemElem )
		{
			int nAbilityOption	= GetSetItem( pItemElem );
			if( nAbilityOption > 0 )
				SetSetItemAvail( nAbilityOption );
			SetDestParamSetItem( pItemElem );
		}
	}

	if( pItemElem && !pItemElem->IsFlag( FLItemElem::expired ) )
	{
		DestParamPiercingAvail( pItemElem, TRUE );
		SetDestParamRandomOpt( pItemElem );
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
		DestParamGemAvail( pItemElem, TRUE );
#endif
	}
}

void CMover::ResetDestParamEquip( PT_ITEM_SPEC pItemProp, FLItemElem* pItemElem )
{
	if( pItemProp == NULL || pItemElem == NULL )
	{
		return;
	}

	if( pItemElem->IsFlag( FLItemElem::expired ) )
	{
		return;
	}

	for( size_t Nth = 0; Nth < MAX_ITEM_PARAM; ++Nth )
	{
		if( pItemProp->dwDestParam[ Nth ] != NULL_ID )
		{
#ifdef __ZCK_ARME_RARE
			const int nAdjParamVal = GetZckArmeRareEquipParamValue( pItemElem, pItemProp->nAdjParamVal[ Nth ] );
			ResetDestParam( pItemProp->dwDestParam[ Nth ], nAdjParamVal );
#else
			ResetDestParam( pItemProp->dwDestParam[ Nth ], pItemProp->nAdjParamVal[ Nth ] );
#endif // __ZCK_ARME_RARE
		}
	}

// 	if( pItemProp->dwDestParam1 != -1 )
// 		ResetDestParam( pItemProp->dwDestParam1, pItemProp->nAdjParamVal1 );
// 	if( pItemProp->dwDestParam2 != -1 )
// 		ResetDestParam( pItemProp->dwDestParam2, pItemProp->nAdjParamVal2 );
// //sun: 10, __PROP_0827
// 	if( pItemProp->dwDestParam3 != -1 )
// 		ResetDestParam( pItemProp->dwDestParam3, pItemProp->nAdjParamVal3 );
	
	if( pItemElem->m_nResistAbilityOption && pItemElem->GetProp()->dwItemKind1 == IK1_ARMOR )
	{
		if( pItemElem->m_byItemResist != SAI79::NO_PROP )
		{
			int nSelf   = GetSelfDST( pItemElem->m_byItemResist );
			int nStrong = GetStrongDST( pItemElem->m_byItemResist );
			int nWeak   = GetWeakDST( pItemElem->m_byItemResist );

			if( g_xFlyffConfig->GetMainLanguage() == LANG_THA )			
			{
				float fResult = ((float)pItemElem->m_nResistAbilityOption+1.2f) * (0.7f+(float)(pItemElem->m_nResistAbilityOption*0.01f));
				int nResult = (int)( fResult * fResult );

				ResetDestParam( nSelf,   nResult );
				ResetDestParam( nWeak,  -nResult/2 );
				ResetDestParam( nStrong, nResult/2 );
			}
			else
			{
				ResetDestParam( nWeak,  -pItemElem->m_nResistAbilityOption*2 );
				ResetDestParam( nStrong, pItemElem->m_nResistAbilityOption*2 );
			}
		}
	} 
	else
	{	
		if( pItemProp->nItemResistElecricity )
			ResetDestParam( DST_RESIST_ELECTRICITY, pItemProp->nItemResistElecricity);
		if( pItemProp->nItemResistFire )
			ResetDestParam( DST_RESIST_FIRE, pItemProp->nItemResistFire );
		if( pItemProp->nItemResistWind )
			ResetDestParam( DST_RESIST_WIND, pItemProp->nItemResistWind );
		if( pItemProp->nItemResistWater )
			ResetDestParam( DST_RESIST_WATER, pItemProp->nItemResistWater );
		if( pItemProp->nItemResistEarth )
			ResetDestParam( DST_RESIST_EARTH, pItemProp->nItemResistEarth );
	}

	if( pItemElem )
	{
//sun: 11, AƒA¢´AA¨ö A¨öAƒA¨öA¨¬A…A›
		CAccessoryProperty* pProperty	= CAccessoryProperty::GetInstance();
		if( pItemElem->IsAccessory() )		// A¨úA—A¨ùA¨ùA¨ùA¡©A¢¬A¢ç
		{
			const std::vector<SINGLE_DST>* pDst	= pProperty->GetDst( pItemElem->m_dwItemId, pItemElem->GetAbilityOption() );
			if( pDst != NULL && pDst->empty() == false )
			{
				const size_t nSize	= pDst->size();
				for( size_t i = 0; i < nSize; i++ )
				{
					ResetDestParam( (*pDst).at( i ).nDst, (*pDst).at( i ).nAdj );
				}
			}
		}

#ifdef __WORLDSERVER
		if( pItemProp->dwDestParam[0] == DST_GIFTBOX || pItemProp->dwDestParam[1] == DST_GIFTBOX || pItemProp->dwDestParam[2] == DST_GIFTBOX )
		{
			((FLWSUser*)this)->RemoveBuff( BUFF_EQUIP, (WORD)( pItemProp->dwID ) );
		}
#endif // __WORLDSERVER

		int nAbilityOption	= GetSetItem( pItemElem );
		if( nAbilityOption > 0 )
			ResetSetItemAvail( nAbilityOption );
		DestParamPiercingAvail( pItemElem, FALSE );
		ResetDestParamSetItem( pItemElem );
		ResetDestParamRandomOpt( pItemElem );
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
		DestParamGemAvail( pItemElem, FALSE );
#endif
	}
}


// µ·È¹µæ 	
void CMover::PickupGold( int nGold, BOOL bDropMob )
{
	FLASSERT( nGold > 0 );
	if( nGold <= 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s PickupGold - %d" ), GetName(), nGold );
		return;
	}

#ifdef __WORLDSERVER
	CParty *pParty	= g_PartyMng.GetParty( m_idparty );
	if( pParty && pParty->IsMember( m_idPlayer ) && bDropMob )	// ÆÄÆ¼°¡ ÀÖ°í ¸÷ÀÌ ¶³±º µ·¸¸ ºÐ¹èµÈ´Ù.
	{
		// ÆÄÆ¼°¡ ÀÖ´Â »óÅÂ¿¡¼­ µ·À» ÁýÀ¸·Á¸é µ·ÀÌ ºÐ¹èµÈ´Ù.
		// ¸ó½ºÅÍ°¡ µå·ÓÇÑµ·¸¸ ºÐ¹èµÇ¾î¾ß ÇÑ´Ù.
//		int nNumInsideMember = 1;		// µ· ÁÝ´Â»ç¶÷ Áß½ÉÀ¸·Î ¹Ý°æ xx¹ÌÅÍ ÀÌ³»ÀÖ´Â»ç¶÷ÀÌ ¸î¸íÀÌ³ª µÇ´ÂÁö °Ë»ç. 
		
		int	nMaxListMember = 0;
		FLWSUser* pMember	= NULL;
		int i;
//		float fDist;
		D3DXVECTOR3	vDist;

		// µ·ÁÝ´Â»ç¶÷ ¹Ý°æ¾È¿¡ µå´Â ¸â¹öµé¸¸ Ãß·Á³½´Ù.
		//mem_set( pListMember, 0, sizeof(pListMember) );
		FLWSUser *pListMember[MAX_PTMEMBER_SIZE] = { 0 };
		for( i = 0; i < pParty->m_nSizeofMember; i++ )
		{
			pMember	= g_xWSUserManager->GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
			/*
			if( IsValidObj( pMember ) )
			{
				vDist = pMember->GetPos() - GetPos();
				fDist = D3DXVec3LengthSq( &vDist );		// µ· ÁÝ´Â»ç¶÷°ú ¸â¹ö°£ÀÇ °Å¸®.
				if( fDist < 32.0f * 32.0f )				// xx¹ÌÅÍº¸´Ù ÀÛÀ¸¸é
				{
					pListMember[ nMaxListMember++ ] = pMember;
				}
			}
			*/ //2009.01.21 // ¹Ý°æ °Ë»ç¿¡ ·¹ÀÌ¾î Ãß°¡
			if( IsValidArea( pMember, 32.0f ) )
				pListMember[ nMaxListMember++ ] = pMember;
		}

		if( nMaxListMember > 0 )
		{
			int nShare = nGold / nMaxListMember;		// ¸â¹ö °¢°¢ÀÇ ¸ò
			int nRest  = nGold % nMaxListMember;		// ³ª´©°í³­ ÈÄ ³ª¸ÓÁö.
			if( nShare )	// ¸òÀÌ ÀÖÀ»¶§.
			{
				for( i = 0; i < nMaxListMember; i++ )
				{
					pMember = pListMember[i];
					pMember->PickupGoldCore( nShare );
				}
			}

			// ³ª¸ÓÁö´Â ·£´ýÀ¸·Î ÇÑ»ç¶÷ÀÌ °¡Áü.
			if( nRest )
			{
				pMember = pListMember[ random(nMaxListMember) ];	
				pMember->PickupGoldCore( nRest );
			}
		}
		else
		{
			FLERROR_LOG( LOG_PARTY_ERROR, _T( "party pick money error:%s %d %d" ), GetName(), pParty->m_nSizeofMember, nMaxListMember );
			for( i = 0; i < pParty->m_nSizeofMember; i ++ )
			{
				pMember		= g_xWSUserManager->GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
				if( IsValidObj( pMember ) )
				{
					FLERROR_LOG( LOG_PARTY_ERROR, _T( "%d %s" ), i, pMember->GetName() );
				}
			}
		}
	} 
	else
#endif // WORLDSERVER	
	{
		// ÆÄÆ¼ ¾øÀ»¶§
		PickupGoldCore( nGold );
	}
}


void CMover::PickupGoldCore( int nGold )
{
	if( AddGold( nGold ) == FALSE )
		return;

#ifdef __WORLDSERVER
	if( IsPlayer() )
	{
		((FLWSUser*)this)->AddGoldText( nGold );
		if( nGold >= 500 )
		{
			LogItemInfo aLogItem;
			//aLogItem.Action = "R";
			//aLogItem.SendName = "GROUND";
			//aLogItem.RecvName = GetName();
			FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "R" );
			FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), "GROUND" );
			FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), GetName() );

			aLogItem.WorldId = GetWorld()->GetID();
			aLogItem.Gold = GetGold() - nGold;
			aLogItem.Gold2 = GetGold();
			//aLogItem.ItemName = "SEED";
			FLSPrintf( aLogItem.kLogItem.szItemName, _countof( aLogItem.kLogItem.szItemName ), "%d", ITEM_INDEX( 12, II_GOLD_SEED1 ) );
			aLogItem.kLogItem.nQuantity = nGold;
			g_DPSrvr.OnLogItem( aLogItem );
		}										
	}
#endif	// __WORLDSERVER
}


#ifdef __WORLDSERVER
// µ·À» ¶¥¿¡ ¶³±º´Ù.  bPK - PK´çÇÑ °æ¿ì 
CItem* CMover::DropGold( DWORD dwGold, const D3DXVECTOR3& vPos, BOOL bPK )
{
	if( IsDropableState( bPK ) == FALSE )
		return NULL;

	CWorld *pWorld = GetWorld();

	int nGold	= (int)dwGold;
//	if( nGold > 0 && nGold <= (int)GetGold() )
	if( nGold > 0 && CheckUserGold( nGold, false ) == true )
	{
		if( nGold >= 500 )
		{
			LogItemInfo aLogItem;
			//aLogItem.Action = "D";
			//aLogItem.SendName = GetName();
			//aLogItem.RecvName = "GROUND";	// ¶¥¿¡ ¹ö¸°°Í

			FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "D" );
			FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), GetName() );
			FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "GROUND" );

			aLogItem.WorldId = pWorld->GetID();
			aLogItem.Gold = GetGold();
			aLogItem.Gold2 = GetGold() - nGold;
			//aLogItem.ItemName = "SEED";
			FLSPrintf( aLogItem.kLogItem.szItemName, _countof( aLogItem.kLogItem.szItemName ), "%d", ITEM_INDEX( 12, II_GOLD_SEED1 ) );
			aLogItem.kLogItem.nQuantity = nGold;
			g_DPSrvr.OnLogItem( aLogItem );
		}

		AddGold( -nGold );

		FLItemElem* pItemElem	= new FLItemElem;
		
		if( nGold <= (int)g_xSpecManager->GetSpecItem( ITEM_INDEX( 12, II_GOLD_SEED1 ) )->dwAbilityMax )
			pItemElem->m_dwItemId	= ITEM_INDEX( 12, II_GOLD_SEED1 );
		else if( nGold <= (int)g_xSpecManager->GetSpecItem( ITEM_INDEX( 13, II_GOLD_SEED2 ) )->dwAbilityMax )
			pItemElem->m_dwItemId	= ITEM_INDEX( 13, II_GOLD_SEED2 );
		else if( nGold <= (int)g_xSpecManager->GetSpecItem( ITEM_INDEX( 14, II_GOLD_SEED3 ) )->dwAbilityMax )
			pItemElem->m_dwItemId	= ITEM_INDEX( 14, II_GOLD_SEED3 );
		else
			pItemElem->m_dwItemId	= ITEM_INDEX( 15, II_GOLD_SEED4 );

		pItemElem->m_nItemNum	= (int)nGold;				// __NPP_050308 ÀÌÈÄ¿¡´Â »ç¿ëÇÏÁö ¾ÊÀ» °Í
		pItemElem->m_nHitPoint  = nGold;					// intÇüÀ¸·Î µ·À» ¹ö¸®°Ô ÇÏ±â À§ÇØ¼­ ÀÌ º¯¼ö¸¦ »ç¿ë 

		CItem* pItem	= new CItem;
		pItem->m_pItemBase	= pItemElem;
		pItem->SetIndex( D3DDEVICE, pItemElem->m_dwItemId );
		pItem->SetPos( vPos );
		pItem->SetAngle( (float)xRandom( 360 ) );
		pItem->m_dwDropTime		= timeGetTime();	
		if( pWorld->ADDOBJ( pItem, TRUE, GetLayer() ) == TRUE )
		{
			return pItem;
		}
		else
		{
			SAFE_DELETE( pItem );
		}
	}
	
	return NULL;
}
#endif // __WORLDSERVER



#ifdef __CLIENT
BOOL CMover::GetEquipFlag( int nParts, BYTE* pbyFlag )
{
	if( IsActiveMover() )
	{
		FLItemElem* pItemElem	= GetEquipItem( nParts );
		if( pItemElem )
		{
			*pbyFlag	= pItemElem->m_byFlag;
			return TRUE;
		}
		return FALSE;
	}
	else
	{
		if( nParts < 0 || nParts >= MAX_HUMAN_PARTS )
			return FALSE;

		if( m_aEquipInfo[nParts].dwItemID != 0 )
		{
			*pbyFlag	= m_aEquipInfo[nParts].byFlag;
			return TRUE;
		}
		return FALSE;
	}
}
#endif	// __CLIENT


// Àåºñ¸¦ Àå/Å»ÂøÇÑ´Ù.
// pItemElem - ÀåÂø/Å»Âø ÇÒ ¾ÆÀÌÅÛ
// bEquip - TRUE ÀåÂø
// nPart - ÆÄÃ÷¹øÈ£ 
void CMover::EquipItem( FLItemElem *pItemElem, BOOL bEquip, int nPart )
{
	if( m_pActMover && m_pActMover->IsActAttack() )
		return;

	if( DoEquip( pItemElem, bEquip, nPart ) )
	{
	#ifdef __WORLDSERVER
		
		g_xApplyItemEffect->RefreshAllEquipActiveSkill( this );

		g_xWSUserManager->AddDoEquip( this, nPart, pItemElem, static_cast< BYTE >( bEquip ) );
		if( pItemElem->GetProp()->dwParts == PARTS_RIDE && pItemElem->GetProp()->dwItemJob == JOB_VAGRANT && bEquip == FALSE )
//			( ( pItemElem->m_dwItemId == II_RID_RID_BOR_EVEINSHOVER || pItemElem->m_dwItemId == ITEM_INDEX( 5801, II_RID_RID_BOR_LADOLF ) ) && bEquip == FALSE )
			RemoveItem( pItemElem->m_dwObjId, pItemElem->m_nItemNum );
	#endif // __WORLDSERVER
	}
}

// Å» °Í¿¡¼­ ³»¸°´Ù.
void CMover::UnequipRide()
{
	if( IsPlayer() )
	{
		FLItemElem* pItemElem = GetEquipItem( PARTS_RIDE );
		if( pItemElem )
			EquipItem( pItemElem, FALSE, -1 );	// -1 ÆÄÃ÷¹øÈ£ 
	}
}

void CMover::ClearEquipInfo()
{
	memset( m_aEquipInfo, 0, sizeof(m_aEquipInfo) );
// 	for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
// 		m_aEquipInfo[i].dwId = NULL_ID;
}

#if defined(__ZCK_SFX_AMELIORER)
BOOL CMover::RefreshZckSfxEquipInfo( FLItemElem* pItemElem )
{
	if( pItemElem == NULL )
		return FALSE;

	BOOL bEquipped = FALSE;
	for( int nPart = 0; nPart < MAX_HUMAN_PARTS; ++nPart )
	{
		if( m_Inventory.GetEquip( nPart ) != pItemElem )
			continue;

		m_aEquipInfo[nPart].Copy( *pItemElem );
		bEquipped = TRUE;
	}

	return bEquipped;
}
#endif // __ZCK_SFX_AMELIORER

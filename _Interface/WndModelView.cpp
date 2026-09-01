#include "StdAfx.h"

#ifdef __ZCK_VISU_MODELS
#include "WndModelView.h"
#include "ResData.h"

CWndModelView::CWndModelView()
	: m_pPet( NULL )
	, m_pEgg( NULL )
	, m_fRot( 0.0f )
	, m_fWheel( 6.0f )
	, m_bLButtonDownRot( FALSE )
{
	m_Mover.m_bPlayer = TRUE;
}

CWndModelView::~CWndModelView()
{
	SAFE_DELETE( m_pPet );
	SAFE_DELETE( m_pEgg );
}

BOOL CWndModelView::Initialize( CWndBase* pWndParent, DWORD )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_MODEL_VIEW, 0, CPoint( 0, 0 ), pWndParent );
}

void CWndModelView::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	MoveParentCenter();

	if( g_pPlayer != NULL )
		SetSex( g_pPlayer->GetSex() );
}

BOOL CWndModelView::Process()
{
	if( m_Mover.m_pModel != NULL )
		m_Mover.m_pModel->FrameMove();
	if( m_pPet != NULL )
		m_pPet->FrameMove();
	if( m_pEgg != NULL )
		m_pEgg->FrameMove();

#ifdef __ZCK_AILES_SFX
	m_Mover.ProcessAilesSfxZacks();
#endif // __ZCK_AILES_SFX
#ifdef __ZCK_SFX_AMELIORER
	m_Mover.ProcessZckManagedSfx();
#endif // __ZCK_SFX_AMELIORER
	return TRUE;
}

void CWndModelView::OnDraw( C2DRender* p2DRender )
{
	if( p2DRender == NULL )
		return;

	LPWNDCTRL pViewCtrl = GetWndCtrl( WIDC_CUSTOM1 );
	if( pViewCtrl == NULL )
		return;

	pViewCtrl->rect = GetClientRect();

	if( m_pPet != NULL )
	{
		if( m_pPet->m_pModelElem != NULL )
			m_pPet->SetTextureEx( m_pPet->m_pModelElem->m_nTextureEx );
		RenderPreviewModel( p2DRender, m_pPet, NULL );
	}
	else if( m_pEgg != NULL )
	{
		if( m_pEgg->m_pModelElem != NULL )
			m_pEgg->SetTextureEx( m_pEgg->m_pModelElem->m_nTextureEx );
		RenderPreviewModel( p2DRender, m_pEgg, NULL );
	}
	else if( m_Mover.m_pModel != NULL )
	{
		RenderPreviewModel( p2DRender, static_cast<CModelObject*>( m_Mover.m_pModel ), &m_Mover );
	}
}

void CWndModelView::RenderPreviewModel( C2DRender* p2DRender, CModelObject* pModel, CMover* pMover )
{
	if( p2DRender == NULL || pModel == NULL )
		return;

	LPWNDCTRL pViewCtrl = GetWndCtrl( WIDC_CUSTOM1 );
	if( pViewCtrl == NULL || pViewCtrl->rect.Width() <= 0 || pViewCtrl->rect.Height() <= 0 )
		return;

	LPDIRECT3DDEVICE9 pDevice = p2DRender->m_pd3dDevice;
	if( pDevice == NULL )
		return;

	D3DVIEWPORT9 oldViewport;
	pDevice->GetViewport( &oldViewport );

	D3DVIEWPORT9 viewport;
	viewport.X = p2DRender->m_ptOrigin.x + pViewCtrl->rect.left;
	viewport.Y = p2DRender->m_ptOrigin.y + pViewCtrl->rect.top;
	viewport.Width = pViewCtrl->rect.Width();
	viewport.Height = pViewCtrl->rect.Height();
	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.0f;
	pDevice->SetViewport( &viewport );
	pDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER, CWorld::m_dwBgColor, 1.0f, 0 );

	pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRot;
	D3DXMATRIXA16 matTrans;
	D3DXMATRIXA16 matView;
	D3DXMATRIXA16 matProj;
	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixIdentity( &matScale );
	D3DXMatrixIdentity( &matRot );
	D3DXMatrixIdentity( &matTrans );
	D3DXMatrixIdentity( &matView );
	D3DXMatrixIdentity( &matProj );

	D3DXVECTOR3 vecLookAt( 0.0f, 0.0f, 3.0f );
	D3DXVECTOR3 vecPos( 0.0f, 0.7f, -3.5f );
	D3DXVECTOR3 vecUp( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH( &matView, &vecPos, &vecLookAt, &vecUp );

	const FLOAT fAspect = static_cast<FLOAT>( viewport.Width ) / static_cast<FLOAT>( viewport.Height );
	const FLOAT fov = D3DX_PI / m_fWheel;
	const FLOAT h = cosf( fov / 2.0f ) / sinf( fov / 2.0f );
	const FLOAT w = h * fAspect;
	D3DXMatrixOrthoLH( &matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
	D3DXMatrixRotationY( &matRot, D3DXToRadian( m_fRot ) );
	D3DXMatrixScaling( &matScale, 1.0f, 1.0f, 1.0f );
	D3DXMatrixTranslation( &matTrans, 0.0f, -0.6f, 0.0f );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matScale );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matRot );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );

	pDevice->SetTransform( D3DTS_VIEW, &matView );
	pDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	pDevice->SetTransform( D3DTS_WORLD, &matWorld );
	pDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	::SetLight( FALSE );
	::SetFog( FALSE );
	SetDiffuse( 1.0f, 1.0f, 1.0f );
	SetAmbient( 1.0f, 1.0f, 1.0f );
	::SetTransformView( matView );
	::SetTransformProj( matProj );

	if( pMover != NULL )
	{
		O3D_ELEMENT* pHair = pModel->GetParts( PARTS_HAIR );
		if( pHair != NULL && pHair->m_pObject3D != NULL )
		{
			pHair->m_pObject3D->m_fAmbient[0] = pMover->m_fHairColorR;
			pHair->m_pObject3D->m_fAmbient[1] = pMover->m_fHairColorG;
			pHair->m_pObject3D->m_fAmbient[2] = pMover->m_fHairColorB;
		}
	}

	pModel->SetGroup( 0 );
	pModel->Render( pDevice, &matWorld );

#ifdef __ZCK_AILES_SFX
	if( pMover != NULL )
	{
		const D3DXMATRIX oldInvView = g_matInvView;
		D3DXMatrixIdentity( &g_matInvView );
		pMover->RenderAilesSfxZacks( pDevice, pModel, &matWorld, TRUE );
		g_matInvView = oldInvView;
	}
#endif // __ZCK_AILES_SFX

#ifdef __ZCK_SFX_AMELIORER
	if( pMover != NULL )
	{
		pMover->RenderZckSfxViewport( pDevice, pModel, &matWorld );
	}
#endif // __ZCK_SFX_AMELIORER

	pDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	pDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pDevice->SetViewport( &oldViewport );
}

BOOL CWndModelView::SetSex( BOOL bSex )
{
	if( g_pPlayer == NULL )
		return FALSE;

	m_Mover.ClearEquipInfo();
	for( int i = 0; i < MAX_HUMAN_PARTS; ++i )
	{
		FLItemElem* pEquipItem = g_pPlayer->GetEquipItem( i );
		if( pEquipItem == NULL )
			continue;

		if( bSex == g_pPlayer->GetSex() || i == PARTS_RWEAPON || i == PARTS_LWEAPON || i == PARTS_SHIELD )
			m_Mover.m_aEquipInfo[i].Copy( *pEquipItem );
	}

	m_Mover.m_bySex = static_cast<BYTE>( bSex );
	const DWORD dwMoverIndex = ( bSex == SEX_FEMALE ) ? OBJECT_INDEX( 12, MI_FEMALE ) : OBJECT_INDEX( 11, MI_MALE );

	m_Mover.m_dwIndex = dwMoverIndex;

	SAFE_DELETE( m_Mover.m_pModel );
	m_Mover.m_pModel = static_cast<CModelObject*>( prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, dwMoverIndex, TRUE ) );
	if( m_Mover.m_pModel == NULL )
		return FALSE;

	prj.m_modelMng.LoadMotion( m_Mover.m_pModel, OT_MOVER, dwMoverIndex, MTI_STAND );
	UpdateMover();
	m_Mover.m_pModel->InitDeviceObjects( g_Neuz.GetDevice() );
	return TRUE;
}

void CWndModelView::UpdateMover()
{
	if( g_pPlayer == NULL || m_Mover.m_pModel == NULL )
		return;

	m_Mover.m_fHairColorR = g_pPlayer->m_fHairColorR;
	m_Mover.m_fHairColorG = g_pPlayer->m_fHairColorG;
	m_Mover.m_fHairColorB = g_pPlayer->m_fHairColorB;

	CMover::UpdateParts(
		m_Mover.m_bySex,
		g_pPlayer->m_dwSkinSet,
		g_pPlayer->m_dwFace,
		g_pPlayer->m_dwHairMesh,
		g_pPlayer->m_dwHeadMesh,
		m_Mover.m_aEquipInfo,
		static_cast<CModelObject*>( m_Mover.m_pModel ),
		NULL,
		TRUE,
		&m_Mover );

#ifdef __ZCK_SFX_AMELIORER
	m_Mover.CreateZckManagedSfx();
#endif // __ZCK_SFX_AMELIORER
}

void CWndModelView::UpdateItem( DWORD dwParts, FLItemElem& itemElem )
{
	const int aEquipClassic[4] = { PARTS_CAP, PARTS_UPPER_BODY, PARTS_HAND, PARTS_FOOT };
	const int aEquipFashion[4] = { PARTS_HAT, PARTS_CLOTH, PARTS_GLOVE, PARTS_BOOTS };
	for( int i = 0; i < 4; ++i )
	{
		if( dwParts == static_cast<DWORD>( aEquipClassic[i] ) )
		{
			m_Mover.m_aEquipInfo[aEquipFashion[i]].Init();
			break;
		}
		if( dwParts == static_cast<DWORD>( aEquipFashion[i] ) )
		{
			m_Mover.m_aEquipInfo[aEquipClassic[i]].Init();
			break;
		}
	}

	PT_ITEM_SPEC pItemSpec = itemElem.GetProp();
	if( pItemSpec == NULL )
		return;

	const BOOL bShieldLike =
		pItemSpec->dwItemKind3 == IK3_SHIELD
		|| pItemSpec->dwItemKind3 == IK3_ZEMBARUNA
		|| pItemSpec->dwItemKind3 == IK3_MAGICBARUNA
		|| pItemSpec->dwItemKind3 == IK3_SHILDBARUNA;

	if( bShieldLike )
	{
		PT_ITEM_SPEC pRight = g_xSpecManager->GetSpecItem( m_Mover.m_aEquipInfo[PARTS_RWEAPON].dwItemID );
		if( pRight != NULL && pRight->dwHanded == HD_TWO )
			m_Mover.m_aEquipInfo[PARTS_RWEAPON].Init();

		PT_ITEM_SPEC pLeft = g_xSpecManager->GetSpecItem( m_Mover.m_aEquipInfo[PARTS_LWEAPON].dwItemID );
		if( pLeft != NULL && pLeft->dwParts == PARTS_RWEAPON )
			m_Mover.m_aEquipInfo[PARTS_LWEAPON].Init();
	}

	if( pItemSpec->dwHanded == HD_TWO )
	{
		m_Mover.m_aEquipInfo[PARTS_SHIELD].Init();
		m_Mover.m_aEquipInfo[PARTS_LWEAPON].Init();
		m_Mover.m_aEquipInfo[PARTS_RWEAPON].Init();
	}

	m_Mover.m_aEquipInfo[dwParts].Copy( itemElem );
}

BOOL CWndModelView::CanPreview( PT_ITEM_SPEC pItemSpec )
{
	if( pItemSpec == NULL )
		return FALSE;

	if( pItemSpec->dwParts > 1 && pItemSpec->dwParts < MAX_HUMAN_PARTS && pItemSpec->dwParts != PARTS_RIDE )
	{
		if( pItemSpec->dwParts >= PARTS_NECKLACE1 && pItemSpec->dwParts <= PARTS_BULLET )
			return FALSE;
		return TRUE;
	}

	return pItemSpec->dwItemKind3 == IK3_PET || pItemSpec->dwItemKind3 == IK3_EGG;
}

BOOL CWndModelView::SetItem( DWORD dwItemID )
{
	if( g_xSpecManager == NULL )
		return FALSE;

	PT_ITEM_SPEC pItemSpec = g_xSpecManager->GetSpecItem( dwItemID );
	if( !CanPreview( pItemSpec ) )
		return FALSE;

	FLItemElem itemElem;
	itemElem.m_dwItemId = dwItemID;
	return SetItem( &itemElem );
}

BOOL CWndModelView::SetItem( FLItemElem* pItemElem )
{
	if( g_pPlayer == NULL || pItemElem == NULL || g_xSpecManager == NULL )
		return FALSE;

	PT_ITEM_SPEC pItemSpec = pItemElem->GetProp();
	if( !CanPreview( pItemSpec ) )
		return FALSE;

	SAFE_DELETE( m_pPet );
	SAFE_DELETE( m_pEgg );

	if( pItemSpec->dwParts > 1 && pItemSpec->dwParts < MAX_HUMAN_PARTS && pItemSpec->dwParts != PARTS_RIDE )
	{
		const EQUIP_INFO rightWeaponInfo = m_Mover.m_aEquipInfo[PARTS_RWEAPON];
		const EQUIP_INFO leftWeaponInfo = m_Mover.m_aEquipInfo[PARTS_LWEAPON];
		const EQUIP_INFO shieldInfo = m_Mover.m_aEquipInfo[PARTS_SHIELD];

		BOOL bSetSexWillBeCalled = FALSE;
		BOOL bPreviewSex = m_Mover.m_bySex;

		if( pItemSpec->dwItemSex != NULL_ID )
		{
			bPreviewSex = static_cast<BOOL>( pItemSpec->dwItemSex );
			bSetSexWillBeCalled = bPreviewSex != m_Mover.m_bySex;
		}
		else if( pItemSpec->dwItemKind1 != IK1_WEAPON )
		{
			bPreviewSex = g_pPlayer->GetSex();
			bSetSexWillBeCalled = TRUE;
		}

		if( bSetSexWillBeCalled )
		{
			if( !SetSex( bPreviewSex ) )
				return FALSE;

			m_Mover.m_aEquipInfo[PARTS_RWEAPON] = rightWeaponInfo;
			m_Mover.m_aEquipInfo[PARTS_LWEAPON] = leftWeaponInfo;
			m_Mover.m_aEquipInfo[PARTS_SHIELD] = shieldInfo;
		}

		UpdateItem( pItemSpec->dwParts, *pItemElem );
		UpdateMover();
		return TRUE;
	}

	if( pItemSpec->dwItemKind3 == IK3_PET )
	{
		if( pItemSpec->dwLinkKind == NULL_ID )
			return FALSE;

		m_pPet = static_cast<CModelObject*>( prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, pItemSpec->dwLinkKind ) );
		if( m_pPet == NULL )
			return FALSE;

		m_pPet->LoadMotion( m_pPet->GetMotionFileName( _T( "stand" ) ) );
		return TRUE;
	}

	if( pItemSpec->dwItemKind3 == IK3_EGG )
	{
		DWORD dwEggModel = OBJECT_INDEX( 731, MI_PET_EGG );
		LPCTSTR pszEggMotion = _T( "idle" );

		if( pItemElem->m_pPet != NULL )
		{
			dwEggModel = pItemElem->m_pPet->GetIndex();
			if( pItemElem->m_pPet->GetLevel() != PL_EGG )
				pszEggMotion = _T( "stand" );
		}
		else
		{
			CPetProperty* pPetProperty = CPetProperty::GetInstance();
			if( pPetProperty != NULL )
			{
				for( int i = 0; i < MAX_PET_KIND; ++i )
				{
					PPETAVAILPARAM pPetParam = pPetProperty->GetAvailParam( static_cast<BYTE>( i ) );
					if( pPetParam != NULL && pPetParam->m_dwItemId == pItemSpec->dwID && pPetParam->m_adwIndex[0] != 0 )
					{
						dwEggModel = pPetParam->m_adwIndex[0];
						pszEggMotion = _T( "stand" );
						break;
					}
				}
			}
		}

		m_pEgg = static_cast<CModelObject*>( prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, dwEggModel ) );
		if( m_pEgg == NULL )
			return FALSE;

		m_pEgg->LoadMotion( m_pEgg->GetMotionFileName( pszEggMotion ) );
		return TRUE;
	}

	return FALSE;
}

BOOL CWndModelView::OnMouseWheel( UINT, short zDelta, CPoint pt )
{
	if( m_pWndRoot != NULL && m_pWndRoot->GetWindowRect().PtInRect( pt ) )
	{
		m_fWheel += ( zDelta < 0 ) ? -0.1f : 0.1f;
		if( m_fWheel < 1.0f || m_fWheel > 18.0f )
			m_fWheel = 6.0f;
	}
	return TRUE;
}

void CWndModelView::OnLButtonDown( UINT, CPoint point )
{
	LPWNDCTRL pViewCtrl = GetWndCtrl( WIDC_CUSTOM1 );
	if( pViewCtrl != NULL && pViewCtrl->rect.PtInRect( point ) )
	{
		SetCapture();
		m_ptOld = point;
		m_bLButtonDownRot = TRUE;
	}
}

void CWndModelView::OnLButtonUp( UINT, CPoint )
{
	ReleaseCapture();
	m_bLButtonDownRot = FALSE;
}

void CWndModelView::OnMouseMove( UINT, CPoint point )
{
	if( m_bLButtonDownRot )
	{
		m_fRot += static_cast<FLOAT>( m_ptOld.x - point.x ) * 0.75f;
		m_ptOld = point;
	}
}

void CWndModelView::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	AdjustMinRect( &rectWnd, 200, 300 );
	AdjustMaxRect( &rectWnd, 495, 590 );
	CWndNeuz::SetWndRect( rectWnd, bOnSize );
}
#endif // __ZCK_VISU_MODELS
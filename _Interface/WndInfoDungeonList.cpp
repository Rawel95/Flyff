#include "stdafx.h"
#include "resData.h"
#include "WndInfoDungeonList.h"
#include <iostream>
#include "InstanceDungeonBase.h"
#ifdef __INFO_DUNGEONS
CWndInfoDungeonList::CWndInfoDungeonList()
{
	nPlayer = 0;
}
CWndInfoDungeonList::~CWndInfoDungeonList()
{
}

BOOL CWndInfoDungeonList::Process()
{
	if (!g_pPlayer)
	{
		nPlayer = 0;
		vInfoDungeonList.clear();
	}

	if (IsValidObj(g_pPlayer))
	{
	if (g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->GetID() != worldID )
	{
		nPlayer = 0;
		vInfoDungeonList.clear();
	}
	}
	return TRUE;
}

void CWndInfoDungeonList::OnInfoDungeonList(CAr & ar)
{
	vInfoDungeonList.clear();
	int nSize;
	ar >> nSize;
	ar >> nPlayer;
#ifdef __DUNGEON_PLAYER_LIMIT
	ar >> nMaxPlayer;
#endif
	ar >> g_pPlayer->m_bShowDungeonInfo;
	ar >> nStats;
	ar >> worldID;
	
	if(g_pPlayer->m_bShowDungeonInfo == FALSE)
		nPlayer = 0;
	
	for(int i = 0; i < nSize; i++)
	{
		INFO_DUNGEONLIST InfoDungeonList;
		ar >> InfoDungeonList.monsterID;
		ar >> InfoDungeonList.bIsKill;
		ar >> InfoDungeonList.nPhase;
		vInfoDungeonList.push_back(InfoDungeonList);
	}
}
void CWndInfoDungeonList::RenderDungeonList(C2DRender* p2DRender)
{
	if( !g_pPlayer )
		return;
	
	if(vInfoDungeonList.size() == 0 || nPlayer == 0 )
		return;

	int PosY = 270;
	int PosX = g_Option.m_nResWidth - 20;
	DWORD dwRenderColor = D3DCOLOR_TEMP(30, 0, 0, 0);
	DWORD dwNameColor = D3DCOLOR_TEMP(255, 255, 255, 250);
	CD3DFont* pOldFont = p2DRender->GetFont();
	char szTitle[255];

	p2DRender->RenderFillRect(CRect(PosX, PosY-46, PosX - 230, PosY-23), dwRenderColor, dwRenderColor, dwRenderColor, dwRenderColor);
#ifdef __DUNGEON_PLAYER_LIMIT
	sprintf(szTitle, "Number of Player(s): %d/%d", nPlayer, nMaxPlayer);
#else
	sprintf(szTitle, "Number of Player(s): %d", nPlayer);
#endif
	CWndBase::m_Theme.m_pFontSRGiuld->DrawText((float)PosX - 185, (float)PosY - 42, 1.0f, 1.0f, 0xFFa3a3a2, szTitle , D3DFONT_BOLD);
	p2DRender->SetFont(pOldFont);

	p2DRender->RenderFillRect(CRect(PosX, PosY-23, PosX - 230, PosY), dwRenderColor, dwRenderColor, dwRenderColor, dwRenderColor);
	
	char szPhase[255];
	switch(nStats)
	{
		case ID_NORMAL: sprintf(szPhase, "Kill the Mini Boss :"); break;
		case ID_MIDBOSS: sprintf(szPhase, "Kill the Normal Boss :"); break;
		case ID_BOSS: sprintf(szPhase, "Kill the Hard Boss :"); break;
		default: sprintf(szPhase, "The Dungeon is Finished"); break;
	}
	
	CWndBase::m_Theme.m_pFontSRGiuld->DrawText((float)PosX - 226, (float)PosY - 19, 1.0f, 1.0f, 0xFFa3a3a2, szPhase);
	p2DRender->SetFont(pOldFont);
	int nSize = 0;
	for (int i = 0; i < (int)vInfoDungeonList.size(); i++)
	{
		MoverProp* pMoverProp = prj.GetMoverProp(vInfoDungeonList[i].monsterID);
		if(vInfoDungeonList[i].nPhase == nStats)
		{	
			nSize++;
			
			if(vInfoDungeonList[i].bIsKill)
				CWndBase::m_Theme.m_pFontSRGiuld->DrawText((float)PosX - 225, (float)PosY + 4, 1.0f, 1.0f, 0xFF1c1c1c, pMoverProp->szName);
			else
				CWndBase::m_Theme.m_pFontSRGiuld->DrawText((float)PosX - 225, (float)PosY + 4, 1.0f, 1.0f, 0xFFff0000, pMoverProp->szName);
			
			p2DRender->SetFont(pOldFont);
			
			PosY += 23;
			p2DRender->RenderFillRect(CRect(PosX, PosY-23, PosX - 230, PosY), dwRenderColor, dwRenderColor, dwRenderColor, dwRenderColor);
		}
	}

	if(g_pPlayer->GetWorld()->GetID() == WI_INSTANCE_CONTAMINTRAILS
		|| g_pPlayer->GetWorld()->GetID() == WI_WORLD_COLOSSEUM
		|| g_pPlayer->GetWorld()->GetID() == WI_INSTANCE_OMINOUS
		|| g_pPlayer->GetWorld()->GetID() == WI_INSTANCE_OMINOUS_1
		|| g_pPlayer->GetWorld()->GetID() == WI_INSTANCE_RUSTIA
		|| g_pPlayer->GetWorld()->GetID() == WI_INSTANCE_RUSTIA_1
		|| g_pPlayer->GetWorld()->GetID() == WI_INSTANCE_DREADFULCAVE
		|| g_pPlayer->GetWorld()->GetID() == WI_INSTANCE_UPRESIA_1
		|| g_pPlayer->GetWorld()->GetID() == WI_INSTANCE_UPRESIA
		|| g_pPlayer->GetWorld()->GetID() == WI_INSTANCE_HERNEOS
		|| g_pPlayer->GetWorld()->GetID() == WI_INSTANCE_HERNEOS_1
		|| g_pPlayer->GetWorld()->GetID() == WI_INSTANCE_SANPRES
		|| g_pPlayer->GetWorld()->GetID() == WI_INSTANCE_SANPRES_1


		)
	{
		//PosY += 23;
		//CWndBase::m_Theme.m_pFontSRGiuld->DrawText((float)PosX - 185, (float)PosY - 19, 1.0f, 1.0f, 0xFFE78494, "Top 5 Hunter");
		//PosY += 23;
		//CWndBase::m_Theme.m_pFontSRGiuld->DrawText((float)PosX - 215, (float)PosY - 19, 1.0f, 1.0f, 0xFFdadc2b, "ALDRIX");
		//PosY += 23;
		//CWndBase::m_Theme.m_pFontSRGiuld->DrawText((float)PosX - 215, (float)PosY - 19, 1.0f, 1.0f, 0xFFdadc2b, "ALDRIX");	
		//PosY += 23;
		//CWndBase::m_Theme.m_pFontSRGiuld->DrawText((float)PosX - 215, (float)PosY - 19, 1.0f, 1.0f, 0xFFdadc2b, "ALDRIX");	
		//PosY += 23;
		//CWndBase::m_Theme.m_pFontSRGiuld->DrawText((float)PosX - 215, (float)PosY - 19, 1.0f, 1.0f, 0xFFdadc2b, "ALDRIX");
		//PosY += 23;
		//CWndBase::m_Theme.m_pFontSRGiuld->DrawText((float)PosX - 215, (float)PosY - 19, 1.0f, 1.0f, 0xFFdadc2b, "ALDRIX");
	}	
}

CWndInfoDungeonList g_InfoDungeonList;
#endif // __INFO_DUNGEONS




















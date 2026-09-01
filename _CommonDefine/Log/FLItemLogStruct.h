
#ifndef __FLITEMLOGSTRUCT_H__
#define __FLITEMLOGSTRUCT_H__

#include <FLItemElem.h>

#pragma pack( 1 )

typedef struct	_T_LOG_ITEM
{
	DWORD			dwItemID;
	SERIALNUMBER	dwSerialNumber;
	int				nHitPoint;
	int				nMaxHitPoint;
	TCHAR			szItemName[_MAX_LOGSTR_ITEMNAME_LEN];
	int				nQuantity;
	int				nAbilityOption;
	int				nItemResist;
	int				nResistAbilityOption;
	BOOL			bCharged;
	DWORD			dwKeepTime;
	__int64			n64RandomOptID;		// TODO : 미사용

	size_t 			nGeneralPiercedSize;
	DWORD			arrGeneralPiercedItemID[MAX_PIERCING];
	size_t			nUltimatePiercedSize;
	DWORD			arrUltimatePiercedItemID[MAX_PIERCING_ULTIMATE];

	int				nPetKind;
	int				nPetLevel;
	DWORD			dwPetExp;
	DWORD			dwPetEnergy;
	DWORD			dwPetLife;
	int				nPetAL_D;
	int				nPetAL_C;
	int				nPetAL_B;
	int				nPetAL_A;
	int				nPetAL_S;

	DWORD			dwCouplePlayerID;
	int				nLevelDown;
	DWORD			dwRandomOptionOriginID;
	int				nRandomOptionExtensionSize;
	DWORD			dwRandomOptionExtensionFlag;
	DWORD			arrDstID[MAX_RANDOMOPTION_DB_COLUMN];
	int				arrAdjValue[MAX_RANDOMOPTION_DB_COLUMN];

	int				nGuildID;
	int				nResistSMItemID;

	DWORD			dwLooksChangeItemID;
	BOOL			bIsCombined;
	DWORD			dwCombinedAddDamage;
	DWORD			dwCombinedRandomOptionOriginID;
	size_t			nCombinedGeneralPiercingSize;
	DWORD			arrCombinedGeneralPiercingID[MAX_PIERCING];
	size_t			nCombinedRandomOptionExtensionSize;
	DWORD			arrCombinedDstID[MAX_RANDOMOPTION_DB_COLUMN];
	int				arrCombinedAdjValue[MAX_RANDOMOPTION_DB_COLUMN];
	
	_T_LOG_ITEM()
	{
		dwItemID							= 0;
		dwSerialNumber						= 0;
		nHitPoint							= 0;
		nMaxHitPoint						= 0;
//		szItemName[ 0 ]						= '\0';
		FLSPrintf( szItemName, _countof( szItemName ), "%d", -1 );
		nQuantity							= 0;
		nAbilityOption						= 0;
		nItemResist							= 0;
		nResistAbilityOption				= 0;
		bCharged							= FALSE;
		dwKeepTime							= 0;
		n64RandomOptID						= 0;
		nGeneralPiercedSize					= 0;
		memset( arrGeneralPiercedItemID, 0, sizeof( arrGeneralPiercedItemID ) );
		nUltimatePiercedSize				= 0;
		memset( arrUltimatePiercedItemID, 0, sizeof( arrUltimatePiercedItemID ) );
		nPetKind							= 0;
		nPetLevel							= 0;
		dwPetExp							= 0;
		dwPetEnergy							= 0;
		dwPetLife 							= 0;
		nPetAL_D 							= 0;
		nPetAL_C 							= 0;
		nPetAL_B 							= 0;
		nPetAL_A 							= 0;
		nPetAL_S 							= 0;
		dwCouplePlayerID					= 0;
		nLevelDown							= 0;
		dwRandomOptionOriginID				= 0;
		nRandomOptionExtensionSize			= 0;
		dwRandomOptionExtensionFlag			= 0;
		memset( arrDstID, 0, sizeof( arrDstID ) );
		memset( arrAdjValue, 0, sizeof( arrAdjValue ) );

		nGuildID							= 0;
		nResistSMItemID						= 0;

		dwLooksChangeItemID					= 0;
		bIsCombined							= FALSE;
		dwCombinedAddDamage					= 0;
		dwCombinedRandomOptionOriginID		= 0;
		nCombinedGeneralPiercingSize		= 0;
		memset( arrCombinedGeneralPiercingID, 0, sizeof( arrCombinedGeneralPiercingID ) );
		nCombinedRandomOptionExtensionSize	= 0;
		memset( arrCombinedDstID, 0, sizeof( arrCombinedDstID ) );
		memset( arrCombinedAdjValue, 0, sizeof( arrCombinedAdjValue ) );
	}

	void	Serialize( CAr & ar )
	{
		ar << dwItemID;
		ar << dwSerialNumber; // 아이템 고유번호
		ar << nHitPoint; // 내구도 
		ar << nMaxHitPoint; // 내구도 

		ar.WriteString( szItemName );
		ar << nQuantity;
		ar << nAbilityOption;
		ar << nItemResist;
		ar << nResistAbilityOption;
		ar << bCharged;
		ar << dwKeepTime;

		ar << nGeneralPiercedSize;
		for( size_t i=0; i<MAX_PIERCING; i++ )
		{
			ar << arrGeneralPiercedItemID[i];
		}

		ar << nUltimatePiercedSize;
		for( size_t i=0; i<MAX_PIERCING_ULTIMATE; i++ )
		{
			ar << arrUltimatePiercedItemID[i];
		}

		ar << nPetKind;
		ar << nPetLevel;
		ar << dwPetExp;
		ar << dwPetEnergy;
		ar << dwPetLife;
		ar << nPetAL_D;
		ar << nPetAL_C;
		ar << nPetAL_B;
		ar << nPetAL_A;
		ar << nPetAL_S;

		ar << dwCouplePlayerID;
		ar << nLevelDown;
		ar << dwRandomOptionOriginID;
		ar << nRandomOptionExtensionSize;
		ar << dwRandomOptionExtensionFlag;
		for( size_t nIndex = 0; nIndex < MAX_RANDOMOPTION_DB_COLUMN; ++nIndex )
		{
			ar << arrDstID[nIndex];
			ar << arrAdjValue[nIndex];
		}

		ar << nGuildID;
		ar << nResistSMItemID;

		ar << dwLooksChangeItemID;
		ar << bIsCombined;
		ar << dwCombinedAddDamage;
		ar << dwCombinedRandomOptionOriginID;

		ar << nCombinedGeneralPiercingSize;
		for( size_t Nth = 0; Nth < MAX_PIERCING; ++Nth )
		{
			ar << arrCombinedGeneralPiercingID[Nth];
		}

		ar << nCombinedRandomOptionExtensionSize;
		for( size_t Nth = 0; Nth < MAX_RANDOMOPTION_DB_COLUMN; ++Nth )
		{
			ar << arrCombinedDstID[Nth];
			ar << arrCombinedAdjValue[Nth];
		}
	}

	void	Deserialize( CAr & ar )
	{
		ar >> dwItemID;
		ar >> dwSerialNumber; // 아이템 고유번호
		ar >> nHitPoint; // 내구도 
		ar >> nMaxHitPoint; // 내구도 

		ar.ReadString( szItemName, _countof( szItemName ) );
		ar >> nQuantity;
		ar >> nAbilityOption;
		ar >> nItemResist;
		ar >> nResistAbilityOption;
		ar >> bCharged;
		ar >> dwKeepTime;

		ar >> nGeneralPiercedSize;
		for( size_t i=0; i<MAX_PIERCING; i++ )
		{
			ar >> arrGeneralPiercedItemID[i];
		}

		ar >> nUltimatePiercedSize;
		for( size_t i=0; i<MAX_PIERCING_ULTIMATE; i++ )
		{
			ar >> arrUltimatePiercedItemID[i];
		}

		ar >> nPetKind;
		ar >> nPetLevel;
		ar >> dwPetExp;
		ar >> dwPetEnergy;
		ar >> dwPetLife;
		ar >> nPetAL_D;
		ar >> nPetAL_C;
		ar >> nPetAL_B;
		ar >> nPetAL_A;
		ar >> nPetAL_S;

		ar >> dwCouplePlayerID;
		ar >> nLevelDown;
		ar >> dwRandomOptionOriginID;
		ar >> nRandomOptionExtensionSize;
		ar >> dwRandomOptionExtensionFlag;
		for( size_t nIndex = 0; nIndex < MAX_RANDOMOPTION_DB_COLUMN; ++nIndex )
		{
			ar >> arrDstID[nIndex];
			ar >> arrAdjValue[nIndex];
		}

		ar >> nGuildID;
		ar >> nResistSMItemID;

		ar >> dwLooksChangeItemID;
		ar >> bIsCombined;
		ar >> dwCombinedAddDamage;
		ar >> dwCombinedRandomOptionOriginID;

		ar >> nCombinedGeneralPiercingSize;
		for( size_t Nth = 0; Nth < MAX_PIERCING; ++Nth )
		{
			ar >> arrCombinedGeneralPiercingID[Nth];
		}

		ar >> nCombinedRandomOptionExtensionSize;
		for( size_t Nth = 0; Nth < MAX_RANDOMOPTION_DB_COLUMN; ++Nth )
		{
			ar >> arrCombinedDstID[Nth];
			ar >> arrCombinedAdjValue[Nth];
		}
	}

	void	CopyItemInfo( FLItemElem & kItemElem )
	{
		dwItemID					= kItemElem.m_dwItemId;
		dwSerialNumber				= kItemElem.GetSerialNumber();
		FLSPrintf( szItemName, _countof( szItemName ), "%d", kItemElem.GetProp()->dwID );
		nQuantity					= kItemElem.m_nItemNum;
		nItemResist					= kItemElem.GetItemResist();
		nResistAbilityOption		= kItemElem.GetResistAbilityOption();
		nAbilityOption				= kItemElem.GetAbilityOption();
		nHitPoint					= kItemElem.m_nHitPoint;
		nMaxHitPoint				= kItemElem.m_nRepair;
		bCharged					= kItemElem.m_bCharged;
		dwKeepTime					= kItemElem.m_dwKeepTime;

		nGeneralPiercedSize			= kItemElem.GetGeneralPiercingSize();
		for( size_t Nth = 0; Nth < MAX_PIERCING; ++Nth )
		{
			arrGeneralPiercedItemID[Nth] = kItemElem.GetGeneralPiercingItemID( Nth );
		}

		nUltimatePiercedSize = kItemElem.GetUltimatePiercingSize();
		for( size_t Nth = 0; Nth < MAX_PIERCING_ULTIMATE; ++Nth )
		{
			arrUltimatePiercedItemID[Nth] = kItemElem.GetUltimatePiercingItemID( Nth );
		}

		if( kItemElem.m_pPet != NULL )
		{
			nPetKind		= kItemElem.m_pPet->GetKind();
			nPetLevel		= kItemElem.m_pPet->GetLevel();
			dwPetExp		= kItemElem.m_pPet->GetExp();
			dwPetEnergy		= kItemElem.m_pPet->GetEnergy();
			dwPetLife 		= kItemElem.m_pPet->GetLife();
			nPetAL_D 		= kItemElem.m_pPet->GetAvailLevel( PL_D );
			nPetAL_C 		= kItemElem.m_pPet->GetAvailLevel( PL_C );
			nPetAL_B 		= kItemElem.m_pPet->GetAvailLevel( PL_B );
			nPetAL_A 		= kItemElem.m_pPet->GetAvailLevel( PL_A );
			nPetAL_S 		= kItemElem.m_pPet->GetAvailLevel( PL_S );
		}
		// mirchang_100514 TransformVisPet_Log
		else if( kItemElem.IsTransformVisPet() == TRUE )
		{
			nPetKind = 100;
		}
		// mirchang_100514 TransformVisPet_Log

		//////////////////////////////////////////////////////////////////////////
		dwCouplePlayerID				= kItemElem.GetCoupleId();
		nLevelDown						= kItemElem.GetLevelDown();
		dwRandomOptionOriginID			= kItemElem.GetRandomOptionOriginID();
		nRandomOptionExtensionSize		= kItemElem.GetRandomOptionExtensionSize();
		dwRandomOptionExtensionFlag		= kItemElem.GetRandomOptionExtensionFlag();

		for( size_t Nth = 0; Nth < MAX_RANDOMOPTION_DB_COLUMN; ++Nth )
		{
			arrDstID[Nth]		= kItemElem.GetRandomOptionExtensionDstID( Nth*2, Nth*2+1 );
			arrAdjValue[Nth]	= kItemElem.GetRandomOptionExtensionAdjValue( Nth*2, Nth*2+1 );
		}
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		nGuildID						= kItemElem.m_idGuild;
		nResistSMItemID					= kItemElem.m_nResistSMItemId;

		dwLooksChangeItemID				= kItemElem.GetLooksChangeItemID();

		if( kItemElem.IsSetCombinedOption() == true )
		{
			bIsCombined						= TRUE;
			dwCombinedAddDamage				= kItemElem.GetCombinedAddDamage();
			dwCombinedRandomOptionOriginID	= kItemElem.GetCombinedRandomOptionOriginID();

			nCombinedGeneralPiercingSize	= kItemElem.GetCombinedGeneralPiercingSize();
			for( size_t Nth = 0; Nth < MAX_PIERCING; ++Nth )
			{
				arrCombinedGeneralPiercingID[Nth]	= kItemElem.GetCombinedGeneralPiercingItemID( Nth );
			}

			nCombinedRandomOptionExtensionSize	= kItemElem.GetCombinedRandomOptionExtensionSize();
			for( size_t Nth = 0; Nth < MAX_RANDOMOPTION_DB_COLUMN; ++Nth )
			{
				arrCombinedDstID[Nth]		= kItemElem.GetCombinedRandomOptionExtensionDstID( Nth*2, Nth*2+1 );
				arrCombinedAdjValue[Nth]	= kItemElem.GetCombinedRandomOptionExtensionAdjValue( Nth*2, Nth*2+1 );
			}
		}
		//////////////////////////////////////////////////////////////////////////
	}

}	T_LOG_ITEM, *PT_LOG_ITEM;


typedef	struct	_LogItemInfo		// ItemLog쓰임
{
	TCHAR		Action[ 2 ];
	TCHAR		SendName[ _MAX_LOGSTR_PLAYERNAME_LEN ];
	TCHAR		RecvName[ _MAX_LOGSTR_PLAYERNAME_LEN ];

	u_long		idSendPlayer;
	u_long		idRecvPlayer;

	DWORD		WorldId;
	int			Gold;
	int			Gold2;
	int			Gold_1;

	int			nSlot;
	int			nSlot1;

	T_LOG_ITEM	kLogItem;

	_LogItemInfo()
	{
		Action[ 0 ]		= '\0';
		SendName[ 0 ]	= '\0';
		RecvName[ 0 ]	= '\0';

		idSendPlayer	= NULL_PLAYER_ID;
		idRecvPlayer	= NULL_PLAYER_ID;

		WorldId			= 0;
		Gold			= 0;
		Gold2			= 0;
		Gold_1			= 0;
		nSlot			= 0;
		nSlot1			= 0;
	}

	void	Serialize( CAr & ar )
	{
		ar.WriteString( Action );
		ar.WriteString( SendName );
		ar.WriteString( RecvName );

		ar << idSendPlayer;
		ar << idRecvPlayer;

		ar << WorldId;
		ar << Gold;
		ar << Gold2;
		ar << Gold_1;
		ar << nSlot;
		ar << nSlot1;

		kLogItem.Serialize( ar );
	}

	void	Deserialize( CAr & ar )
	{
		ar.ReadString( Action, _countof( Action ) );
		ar.ReadString( SendName, _countof( SendName ) );
		ar.ReadString( RecvName, _countof( RecvName ) );

		CString strSendName		= SendName;
		CString strRecvName		= RecvName;

		strSendName.Replace( '\'', ' ' );
		FLStrcpy( SendName, _countof( SendName ), strSendName );
		strRecvName.Replace( '\'', ' ' );
		FLStrcpy( RecvName, _countof( RecvName ), strRecvName );
		
		ar >> idSendPlayer;
		ar >> idRecvPlayer;

		ar >> WorldId;
		ar >> Gold;
		ar >> Gold2;
		ar >> Gold_1;
		ar >> nSlot;
		ar >> nSlot1;

		kLogItem.Deserialize( ar );
	}

}	LogItemInfo,	*PLogItemInfo;


// typedef struct _T_TRADE_ITEM_LOG
// {
// 	int				nFlag;
// 	int				nTradeID;
// 	DWORD			dwWorldID;
// 	u_long			idPlayer;
// 	DWORD			dwTradeGold;
// 	TCHAR			szAddress[16];
// 	int				nLevel;
// 	int				nJob;
// 
// 	T_LOG_ITEM		kLogItem;
// 
// 
// 	_T_TRADE_ITEM_LOG()
// 	{
// 		nFlag								= 0;
// 		nTradeID							= 0;
// 		dwWorldID							= 0;
// 		idPlayer							= 0;
// 		dwTradeGold							= 0;
// 		szAddress[0]						= '\0';
// 		nLevel								= 0;
// 		nJob								= 0;
// 	}
// 
// } T_TRADE_ITEM_LOG, *PT_TRADE_ITEM_LOG;


typedef struct	_T_LOG_TRADE
{
	TCHAR		szPlayerName[ _MAX_LOGSTR_PLAYERNAME_LEN ];
	u_long		idPlayerID;
	DWORD		dwWorldID;
	int			nTradeGold;
	int			nHaveGold;
	int			nPlayerLevel;
	int			nPlayerJob;
	TCHAR		szIP[16];
	DWORD		dwTradeItemCount;

	
	_T_LOG_TRADE()
	{
		szPlayerName[0]		= '\0';
		idPlayerID			= NULL_PLAYER_ID;
		dwWorldID			= 0;
		nTradeGold			= 0;
		nHaveGold			= 0;
		nPlayerLevel		= 0;
		nPlayerJob			= 0;
		szIP[0]				= '\0';
		dwTradeItemCount	= 0;
	}

	void	Serialize( CAr & ar )
	{
		ar.WriteString( szPlayerName );
		ar << idPlayerID;
		ar << dwWorldID;
		ar << nTradeGold;
		ar << nHaveGold;
		ar << nPlayerLevel;
		ar << nPlayerJob;
		ar.WriteString( szIP );
		ar << dwTradeItemCount;
	}

	void	Deserialize( CAr & ar )
	{
		ar.ReadString( szPlayerName, _countof( szPlayerName ) );

		CString strPlayerName	= szPlayerName;
		strPlayerName.Replace( '\'', ' ' );
		FLStrcpy( szPlayerName, _countof( szPlayerName ), strPlayerName );

		ar >> idPlayerID;
		ar >> dwWorldID;
		ar >> nTradeGold;
		ar >> nHaveGold;
		ar >> nPlayerLevel;
		ar >> nPlayerJob;
		ar.ReadString( szIP, _countof( szIP ) );
		ar >> dwTradeItemCount;
	}

}	T_LOG_TRADE, *PT_LOG_TRADE;

#pragma pack()

#endif // __FLITEMLOGSTRUCT_H__

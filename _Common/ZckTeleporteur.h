#pragma once

#ifdef __ZCK_TELEPORTEUR

#include <map>
#include <vector>

#ifdef __WORLDSERVER
class FLWSUser;
#endif // __WORLDSERVER

enum ZckTeleportType : BYTE
{
    ZCK_TELEPORT_TYPE_CITY = 1,
    ZCK_TELEPORT_TYPE_ZONE,
    ZCK_TELEPORT_TYPE_DUNGEON
};

struct ZckTeleportDestination
{
    DWORD dwWorldID;
    D3DXVECTOR3 vPos;
#ifdef __CLIENT
    BYTE byType;
    CString strName;
    CString strTextureName;
    CString strDescription;
    std::vector<DWORD> vecItems;
#endif // __CLIENT

    ZckTeleportDestination()
        : dwWorldID( NULL_ID )
        , vPos( 0.0f, 0.0f, 0.0f )
#ifdef __CLIENT
        , byType( ZCK_TELEPORT_TYPE_CITY )
#endif // __CLIENT
    {
    }
};

class CZckTeleporteur
{
private:
    std::map<int, ZckTeleportDestination> m_mapDestinations;

public:
    CZckTeleporteur();
    ~CZckTeleporteur();

    static CZckTeleporteur* GetInstance();

    BOOL LoadScript( LPCTSTR pszFileName = _T( "ZckTeleporteur.inc" ) );
    const std::map<int, ZckTeleportDestination>& GetDestinations() const { return m_mapDestinations; }

#ifdef __WORLDSERVER
    void DoTeleport( int nKey, FLWSUser* pUser );
#endif // __WORLDSERVER
};

#ifdef __CLIENT
class CWndZckTeleporteur : public CWndNeuz
{
private:
    struct FilteredDestination
    {
        int nKey;
        CString strName;
        CString strTextureName;
        CString strDescription;
        std::vector<DWORD> vecItems;

        FilteredDestination()
            : nKey( -1 )
        {
        }
    };

    std::vector<FilteredDestination> m_vecTeleports;
    CWndListBox* m_pWndListLocations;
    CWndListBox* m_pWndListItems;
    CWndComboBox* m_pWndComboFilter;
    CWndStatic* m_pLocationName;
    CWndText* m_pWndDescription;
    CTexture* m_pTexture;

    const FilteredDestination* GetCurrentDestination() const;

public:
    CWndZckTeleporteur();
    virtual ~CWndZckTeleporteur();

    virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwType = MB_OK );
    virtual void OnInitialUpdate();
    virtual void OnDraw( C2DRender* p2DRender );
    virtual void OnSize( UINT nType, int cx, int cy );
    virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
    virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );

    void Refresh();
    void SelectWorld();
    CString GetTypeName( BYTE byType ) const;
    const std::vector<DWORD>* GetCurrentItems() const;
};
#endif // __CLIENT

#endif // __ZCK_TELEPORTEUR
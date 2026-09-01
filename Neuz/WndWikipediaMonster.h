#ifndef __WND__WIKI_MOB
#define __WND__WIKI_MOB
#include <vector>

class CWndNeuz;
class CWndListBox;
class CWndEdit;
class CWndComboBox;
class MoverProp;

#ifdef __WIKIPEDIA_MONSTER

class CWndWikiMobCreate : public CWndNeuz {
private:
    MoverProp* m_pMoverProp;
    CWndEdit* m_pEditAmount;
    CWndEdit* m_pEditAggro;
    CWndStatic* m_pStcName;

public:
    CWndWikiMobCreate();
    virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwStyle = 0);
    virtual void OnInitialUpdate();
    virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
    void SetMoverProp(MoverProp* pMoverPropCreate);
};

class CWndWikiMob : public CWndNeuz {
private:
    CWndWikiMobCreate* m_pWikiMobCreate;
    std::vector<MoverProp*> m_vecMobs;
    CWndListBox* m_pListMobs;
    CWndEdit* m_pEditFilter;
    CWndComboBox* m_pFilter;
    int m_nSortType;
    BOOL m_bIsGreater;

public:
    std::vector<MoverProp*>* GetMobsList() { return &m_vecMobs; }
    CWndWikiMob();
    virtual ~CWndWikiMob();
    virtual void OnInitialUpdate();
    virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
    virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
    void UpdateMobs();
    void Sort();
    void SortByName();
    void SortByLevel();
    void SortByHp();
    void SortByAtk();
    void SortByElement();
};

#endif // __WIKIPEDIA_MONSTER


#endif //__WND__WIKI_MOB
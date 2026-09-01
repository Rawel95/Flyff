#pragma once

#ifdef __ZCK_WIKI
class CWndWikiCreate : public CWndNeuz
{
private:
	DWORD m_dwItemID;
	CWndEdit* m_pWndEditName;
	CWndEdit* m_pWndEditCount;

public:
	explicit CWndWikiCreate( DWORD dwItemID );
	virtual ~CWndWikiCreate();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwStyle = 0 );
	virtual void OnInitialUpdate();
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
};

class CWndWikiItems : public CWndNeuz
{
private:
	CWndWikiCreate* m_pWndWikiCre;
	CWndEdit* m_pNameFilter;
	CWndEdit* m_pMinLevelFilter;
	CWndEdit* m_pMaxLevelFilter;
	CWndComboBox* m_pComboSex;
	CWndComboBox* m_pComboJob;
	CWndListBox* m_pListItems;
	CWndComboBox* m_pWndComboFilter;
	CString m_strKeyword;
	std::vector<PT_ITEM_SPEC> m_vecItems;

public:
	CWndWikiItems();
	virtual ~CWndWikiItems();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwStyle = 0 );
	virtual void OnInitialUpdate();
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual void OnDestroyChildWnd( CWndBase* pWndChild );

	void ApplyFilters();
	void SortByItemLevel( BOOL bHighest );
	std::vector<PT_ITEM_SPEC>* GetItemList() { return &m_vecItems; }
};
#endif // __ZCK_WIKI
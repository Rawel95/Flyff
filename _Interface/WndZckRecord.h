#ifndef __WND_ZCK_RECORD_H__
#define __WND_ZCK_RECORD_H__

#pragma once

#ifdef __ZCK_RECORD

#include "WndNeuz.h"
#include "../_Common/ZckRecord.h"
#include <vector>

class CWndButton;
class CWndListBox;

class CZckRecordClientData
{
private:
    DWORD m_dwVersion;
    __int64 m_nWeekStart;
    std::vector<ZCK_RECORD_ENTRY> m_rankings[ZCK_RECORD_COUNT];

    CZckRecordClientData();

public:
    static CZckRecordClientData* GetInstance();
    void Deserialize( CAr& ar );

    DWORD GetVersion() const { return m_dwVersion; }
    __int64 GetWeekStart() const { return m_nWeekStart; }
    const std::vector<ZCK_RECORD_ENTRY>& GetRanking( ZCK_RECORD_TYPE type ) const { return m_rankings[type]; }
};

class CWndZckRecord : public CWndNeuz
{
private:
    enum
    {
        ZCK_RECORD_COLUMN_RANK = 0,
        ZCK_RECORD_COLUMN_NAME,
        ZCK_RECORD_COLUMN_LEVEL,
        ZCK_RECORD_COLUMN_JOB,
        ZCK_RECORD_COLUMN_VALUE,
        ZCK_RECORD_COLUMN_COUNT
    };

    ZCK_RECORD_TYPE m_type;
    DWORD m_dwLastRequestTick;
    DWORD m_dwDisplayedVersion;
    CWndButton* m_pCategoryButtons[ZCK_RECORD_COUNT];
    CWndListBox* m_pColumnLists[ZCK_RECORD_COLUMN_COUNT];

    void RequestRefresh( bool bForce );
    void UpdateCategoryButtons();
    void UpdateList();
    CString FormatValue( const ZCK_RECORD_ENTRY& entry ) const;

public:
    CWndZckRecord();
    virtual ~CWndZckRecord();

    virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwType = 0 );
    virtual void OnInitialUpdate();
    virtual BOOL Process();
    virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
    virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
};

#endif // __ZCK_RECORD

#endif // __WND_ZCK_RECORD_H__
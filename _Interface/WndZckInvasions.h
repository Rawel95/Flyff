#pragma once

#ifdef __ZCK_INVASIONS

class CWndZckInvasions : public CWndNeuz
{
private:
	DWORD m_dwInvasionID;
	BOOL m_bOnSurface;

public:
	CWndZckInvasions();
	virtual ~CWndZckInvasions();

	virtual BOOL Process();
	virtual void OnDraw( C2DRender* p2DRender );
	virtual void OnInitialUpdate();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );

	void SetInvasionID( DWORD dwInvasionID ) { m_dwInvasionID = dwInvasionID; }
	DWORD GetInvasionID() const { return m_dwInvasionID; }
};

#endif // __ZCK_INVASIONS
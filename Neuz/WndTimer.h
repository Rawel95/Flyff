#pragma once

#ifdef __COOLDOWN_TIMER

class CWndTimer : public CWndNeuz
{
public:
	virtual BOOL Initialize(CWndBase *pWndParent = NULL, DWORD dwStyle = 0);
	virtual void OnInitialUpdate();
};

#endif // __COOLDOWN_TIMER
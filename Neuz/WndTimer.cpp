#include "stdafx.h"

#ifdef __COOLDOWN_TIMER

#include "resData.h"
#include "WndTimer.h"


BOOL CWndTimer::Initialize(CWndBase *pWndParent, DWORD dwStyle)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(),APP_TIMER,0,0,pWndParent);
}

void CWndTimer::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	Move(0,150);
}

#endif // __COOLDOWN_TIMER
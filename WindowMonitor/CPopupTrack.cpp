#include "stdafx.h"
#include "CPopupTrack.h"
#include <atlstr.h>
#include "Util.h"
#include "WinMonitorManager.h"


CPopupTrack::CPopupTrack()
{
	//
	m_fClassNameHashSetInit = FALSE;
}


CPopupTrack::~CPopupTrack()
{
	// 
}

VOID CPopupTrack::OnReceiveWndEvent(HWINEVENTHOOK hWInEventHook,
	DWORD	event,
	HWND	hWnd,
	LONG	idObject,
	LONG	idChild,
	DWORD	dwEventThread,
	DWORD	dwmsEventTime)
{
	// 先来一把硬的
	WndEventInfo tmp;
	
	tmp.dwEventThread = dwEventThread;
	tmp.dwmsEventTime = dwmsEventTime;
	tmp.event = event;
	tmp.hWInEventHook = hWInEventHook;
	tmp.hWnd = hWnd;
	tmp.idChild = idChild;
	tmp.idObject = idObject;

	ProcessEventFilter(&tmp);
	
	// 
	ProcessEventNoFilter(&tmp);
}


VOID CPopupTrack::ProcessEventFilter(lpWndEventInfo lpWndE)
{
	RECT rect;

	// 先查询下cache
	BOOL binCache = FALSE;
	binCache = ProcessEventFilterCache(lpWndE);

	if (binCache || lpWndE->event == EVENT_OBJECT_HIDE)
	{
		return;
	}
	
	if (!(lpWndE->hWnd && ::GetWindowRect(lpWndE->hWnd, &rect)))
	{
		return;
	}

	if (!((rect.right - rect.left >= 20 || rect.bottom - rect.top >= 20) && rect.top >= 0 && rect.right >= 0))
	{
		return;
	}

	WCHAR lpClassName[MAX_PATH]; 
	if (!(lpWndE->hWnd && ::GetClassNameW(lpWndE->hWnd, lpClassName, MAX_PATH)))
	{
		return;
	}

	InitHardCodeWhiteClassNameList();

	if(m_ClassNameHashSet.find(BKDRHash(::CharUpperW(lpClassName))) != m_ClassNameHashSet.end())
		return;

	if (lpWndE->event == EVENT_OBJECT_LOCATIONCHANGE)
	{
		ProcessLocationChangeEvent(lpWndE);
	}

	else
	{
		DoRealProcessEvent(lpWndE);
	}
	
}

VOID CPopupTrack::ProcessEventNoFilter(lpWndEventInfo lpWndE)
{
	return VOID();
}

VOID CPopupTrack::ProcessLocationChangeEvent(lpWndEventInfo lpWndE)
{
	RECT rect;
	if (!(lpWndE->hWnd && ::GetWindowRect(lpWndE->hWnd, &rect)))
	{
		return;
	}

	// 先简单这么处理下;
	DoRealProcessEvent(lpWndE);


	//DoRealProcessEvent(lpWndE);
}

VOID CPopupTrack::DoRealProcessEvent(lpWndEventInfo lpWndE)
{
	if(!IsWindowVisible(lpWndE->hWnd))
		return VOID();
	
	CString strWndTxt;
	CString strWndClassName;

	// 对需要的信息的收集工作
	wchar_t szbuffer[MAX_PATH];
	ZeroMemory(szbuffer, sizeof(szbuffer));
	::GetWindowTextW(lpWndE->hWnd, szbuffer, MAX_PATH);
	strWndTxt = szbuffer;

	// 下一步是获取进程名字和父进程的名字
	// 

	// 获取ClassName
	ZeroMemory(szbuffer, sizeof(szbuffer));
	::GetClassNameW(lpWndE->hWnd, szbuffer, MAX_PATH);
	strWndClassName = szbuffer;
    
	// 
	RECT rect;
	::GetWindowRect(lpWndE->hWnd, &rect);

	// 
	LONG nExstyle = ::GetWindowLongW(lpWndE->hWnd, GWL_EXSTYLE);
	LONG nStyle = ::GetWindowLongW(lpWndE->hWnd, GWL_STYLE);

	//
	POINT point;
	::GetCursorPos(&point);

	CString strHeader;
	strHeader.Format(L"Event ID %x ", lpWndE->event);

	DebugWinMessage(strHeader.GetString(), lpWndE->hWnd);

	


}

BOOL CPopupTrack::ProcessEventFilterCache(lpWndEventInfo lpWndE)
{
	BOOL bRet = FALSE;
	if (lpWndE->event == EVENT_OBJECT_HIDE)
	{
		WndEventInfo  tmp;
		WndEventCache tmpCache;

		memcpy(&tmp, lpWndE, sizeof(WndEventInfo));
		DWORD dwTickCount = GetTickCount();
		tmp.event = EVENT_OBJECT_SHOW;

		if (!m_WndEventCache.QueryCache(lpWndE, &tmpCache))
		{
			bRet = TRUE;
			goto __Exit;
		}

		if (dwTickCount - tmpCache.dwTickCount > 2000)
		{
			// Just Del
			m_WndEventCache.DelCache(lpWndE->hWnd);
		}

		bRet = TRUE;
		goto __Exit;
	}

	{

	}




__Exit:
	return bRet;
}

VOID CPopupTrack::InitHardCodeWhiteClassNameList()
{
	if (m_fClassNameHashSetInit)
	{
		return;
	}

	wchar_t *ClassNameList[] = { 
		    L"button", L"edit", L"static", L"combobox",
			L"listbox", L"mdiclient", L"richedit", L"scrollbar", L"richedit_class", L"sysshadow",
			L"tooltips_class32", L"msctls_trackbar32", L"msctls_progress32", L"msctls_hotkey32",
			L"SysListView32", L"GenericPane", L"SysTreeView32", L"SysTabControl32",
			L"SysAnimate32", L"RichEdit20A", L"SysDateTimePick32", L"SysMonthCal32",
	        L"SysIPAddress32", L"SysLink", L"msctls_netaddress", L"MFCButton",
			L"MFCColorButton", L"MFCEditBrowse", L"MFCVSListBox", L"MFCFontComboBox",
			L"MFCMaskedEdit", L"MFCMenuButton", L"MFCPropertyGrid", L"MFCShellList",
			L"MFCShellTree", L"MFCLink", L"IME", L"ComboLBox",
			L"VBFloatingPalette", L"MsoCommandBarShadow"};

	
	for (size_t i = 0; i < sizeof(ClassNameList)/sizeof(ClassNameList[0]); i++)
	{
		CString strClassName = ClassNameList[i];
		strClassName.MakeUpper();
		m_ClassNameHashSet.insert(BKDRHash(strClassName.GetString()));
	}

	m_fClassNameHashSetInit = TRUE;
}





////// CMemCache
CMemCache::CMemCache()
{
}


CMemCache::~CMemCache()
{
}

BOOL CMemCache::QueryCache(lpWndEventInfo lpWndEvent, lpWndEventCache lpWndEventCache)
{
	m_CacheLock.Lock();

	WndMemCache::iterator it;
	BOOL bRet = FALSE;
	BOOL bFound = FALSE;

	for (it = m_WndCache.begin(); it != m_WndCache.end(); it++)
	{
		BOOL tmp = (*it).WndEventInfot.event != lpWndEvent->event || (*it).WndEventInfot.hWnd != lpWndEvent->hWnd ||
			(*it).WndEventInfot.idChild != lpWndEvent->idChild || (*it).WndEventInfot.idObject != lpWndEvent->idObject;

		if (!tmp)
		{
			bFound = TRUE;
			break;
		}
	}

	if (bFound)
	{
		bRet = TRUE;
		memcpy(lpWndEventCache, &(*it), sizeof(WndEventCache));
	}

	m_CacheLock.Unlock();
	return bRet;
}

VOID CMemCache::DelCache(HWND hwnd)
{
	m_CacheLock.Lock();

	WndMemCache::iterator it;
	BOOL bFound = FALSE;

	for (it = m_WndCache.begin(); it != m_WndCache.end(); it++)
	{
		BOOL tmp = (*it).WndEventInfot.hWnd == hwnd;
		if (tmp)
		{
			bFound = TRUE;
			break;
		}
	}

	if (bFound)
	{
		m_WndCache.erase(it);
	}
	m_CacheLock.Unlock();
	return VOID();
}

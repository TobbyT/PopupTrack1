#pragma once

#include "CMemCache.h"
#include <atlbase.h>
#include <list>
#include <set>
using namespace std;


typedef struct _WndEventInfo
{
	HWINEVENTHOOK hWInEventHook;
	DWORD	event;
	HWND	hWnd;
	LONG	idObject;
	LONG	idChild;
	DWORD	dwEventThread;
	DWORD	dwmsEventTime;
}WndEventInfo, *lpWndEventInfo;

typedef struct _WndEventCache
{
	_WndEventInfo WndEventInfot;
	DWORD	dwTickCount;
}WndEventCache, *lpWndEventCache;


typedef list<WndEventCache> WndMemCache;


class CMemCache
{
public:
	CMemCache();
	~CMemCache();

	BOOL QueryCache(lpWndEventInfo lpWndEvent, lpWndEventCache lpWndEventCachet);

	VOID DelCache(HWND hwnd);

private:
	CComAutoCriticalSection m_CacheLock;
	WndMemCache   m_WndCache;
};



class CPopupTrack
{
public:
	CPopupTrack();
	~CPopupTrack();
	static CPopupTrack* Instance()
	{
		static CPopupTrack s_Instance;
		return &s_Instance;
	}

public:
	VOID OnReceiveWndEvent(HWINEVENTHOOK hWInEventHook,
		DWORD	event,
		HWND	hWnd,
		LONG	idObject,
		LONG	idChild,
		DWORD	dwEventThread,
		DWORD	dwmsEventTime);

private:
	VOID ProcessEventFilter(lpWndEventInfo lpWndE);

	VOID ProcessEventNoFilter(lpWndEventInfo lpWndE);

	VOID ProcessLocationChangeEvent(lpWndEventInfo lpWndE);

	VOID DoRealProcessEvent(lpWndEventInfo lpWndE);

	BOOL ProcessEventFilterCache(lpWndEventInfo lpWndE);

private:
	VOID InitHardCodeWhiteClassNameList();

private:
	set<DWORD> m_ClassNameHashSet;
	BOOL m_fClassNameHashSetInit;

	CMemCache m_WndEventCache;

};


#include "stdafx.h"
#include "WinEvent.h"
#include "CPopupTrack.h"


CWinEvent::CWinEvent()
:m_pEventSink(NULL)
{
	ZeroMemory(&m_hEvent, sizeof(m_hEvent));
}

CWinEvent::~CWinEvent()
{

}

CWinEvent CWinEvent::s_winEvent;

CWinEvent& CWinEvent::GetInstance()
{
	return s_winEvent;
}

void CALLBACK  WinEventHookProc(HWINEVENTHOOK hWInEventHook, 
								DWORD	event,
								HWND	hWnd,
								LONG	idObject,
								LONG	idChild,
								DWORD	dwEventThread,
								DWORD	dwmsEventTime)
{
	if(hWnd == NULL  || !::IsWindow(hWnd))
		return;

	if(idObject != OBJID_WINDOW
		&& idChild != CHILDID_SELF)
		return;

	CPopupTrack::Instance()->OnReceiveWndEvent(hWInEventHook, event, hWnd, idObject, idChild, dwEventThread, dwmsEventTime);

	switch(event)
	{
	case EVENT_OBJECT_FOCUS:
		CWinEvent::GetInstance().GetEventSink()->OnFocusChanged(hWnd);
		break;

	case EVENT_SYSTEM_FOREGROUND:
		CWinEvent::GetInstance().GetEventSink()->OnForegroundChanged(hWnd);
		break;

	case EVENT_OBJECT_CREATE:
		CWinEvent::GetInstance().GetEventSink()->OnWinCreate(hWnd);
		break;

	case EVENT_OBJECT_DESTROY:
		CWinEvent::GetInstance().GetEventSink()->OnWinDestroy(hWnd);
		break;

	case EVENT_OBJECT_SHOW:
		CWinEvent::GetInstance().GetEventSink()->OnWinShow(hWnd);
		break;

	case EVENT_OBJECT_HIDE:
		CWinEvent::GetInstance().GetEventSink()->OnWinHide(hWnd);
		break;
			
	default:
		break;
	}
}

IWinEventSink* CWinEvent::GetEventSink() const
{
	return m_pEventSink;
}

BOOL CWinEvent::InstallEventHook(IWinEventSink* pSink)
{
	if(m_hEvent[0] != 0) return TRUE;

	m_pEventSink = pSink;

	m_hEvent[EVENT_INDEX_LOCATIONCHANGE] = (HWINEVENTHOOK)SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, NULL, WinEventHookProc, 0, 0, WINEVENT_SKIPOWNPROCESS);
	m_hEvent[EVENT_INDEX_NAMECHANGE] = (HWINEVENTHOOK)SetWinEventHook(EVENT_OBJECT_NAMECHANGE, EVENT_OBJECT_NAMECHANGE, NULL, WinEventHookProc, 0, 0, WINEVENT_SKIPOWNPROCESS);
	
	//m_hEvent[EVENT_INDEX_CREATE] = (HWINEVENTHOOK)SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_CREATE, NULL, WinEventHookProc, 0, 0, WINEVENT_OUTOFCONTEXT);
	//m_hEvent[EVENT_INDEX_DESTROY] = (HWINEVENTHOOK)SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_DESTROY, NULL, WinEventHookProc, 0, 0, WINEVENT_OUTOFCONTEXT);
	
	//  
	m_hEvent[EVENT_INDEX_SHOW] = (HWINEVENTHOOK)SetWinEventHook(EVENT_OBJECT_SHOW, EVENT_OBJECT_SHOW, NULL, WinEventHookProc, 0, 0, WINEVENT_SKIPOWNPROCESS);
	m_hEvent[EVENT_INDEX_HIDE] = (HWINEVENTHOOK)SetWinEventHook(EVENT_OBJECT_HIDE, EVENT_OBJECT_HIDE, NULL, WinEventHookProc, 0, 0, WINEVENT_SKIPOWNPROCESS);

	// EVENT_OBJECT_HIDE

	return TRUE;
}

VOID CWinEvent::UninstallEventHook()
{
	for(INT i=0; i<EVENT_INDEX_COUNT; ++i)
	{
		if(m_hEvent[i] != 0)
		{
			UnhookWinEvent(m_hEvent[i]);
		}
	}

	ZeroMemory(&m_hEvent, sizeof(m_hEvent));
}
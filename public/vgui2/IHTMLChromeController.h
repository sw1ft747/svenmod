//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef SDK_IHTMLCHROMECONTROLLER_H
#define SDK_IHTMLCHROMECONTROLLER_H

#ifdef _WIN32
#pragma once
#endif

#include "../interface.h"

#include <stdint.h>

class CUtlString;
class IHTMLResponses;
class HTMLCommandBuffer_t;
class IHTMLSerializer;
class IHTMLResponses_HL1;

enum EHTMLCommands
{
	eHTMLCommands_KeyUp = 0,
	eHTMLCommands_KeyDown = 1,
	eHTMLCommands_KeyChar = 2,
	eHTMLCommands_MouseDown = 3,
	eHTMLCommands_MouseUp = 4,
	eHTMLCommands_MouseDblClick = 5,
	eHTMLCommands_MouseWheel = 6,
	eHTMLCommands_MouseMove = 7,
	eHTMLCommands_MouseLeave = 8,
	eHTMLCommands_BrowserCreate = 9,
	eHTMLCommands_BrowserRemove = 10,
	eHTMLCommands_BrowserErrorStrings = 11,
	eHTMLCommands_BrowserSize = 12,
	eHTMLCommands_BrowserPosition = 13,
	eHTMLCommands_PostURL = 14,
	eHTMLCommands_StopLoad = 15,
	eHTMLCommands_Reload = 16,
	eHTMLCommands_GoForward = 17,
	eHTMLCommands_GoBack = 18,
	eHTMLCommands_Copy = 19,
	eHTMLCommands_Paste = 20,
	eHTMLCommands_ExecuteJavaScript = 21,
	eHTMLCommands_SetFocus = 22,
	eHTMLCommands_HorizontalScrollBarSize = 23,
	eHTMLCommands_VerticalScrollBarSize = 24,
	eHTMLCommands_Find = 25,
	eHTMLCommands_StopFind = 26,
	eHTMLCommands_SetHorizontalScroll = 27,
	eHTMLCommands_SetVerticalScroll = 28,
	eHTMLCommands_SetZoomLevel = 29,
	eHTMLCommands_ViewSource = 30,
	eHTMLCommands_NeedsPaintResponse = 31,
	eHTMLCommands_AddHeader = 32,
	eHTMLCommands_GetZoom = 33,
	eHTMLCommands_FileLoadDialogResponse = 34,
	eHTMLCommands_LinkAtPosition = 35,
	eHTMLCommands_ZoomToElementAtPosition = 36,
	eHTMLCommands_SavePageToJPEG = 37,
	eHTMLCommands_JSAlert = 38,
	eHTMLCommands_JSConfirm = 39,
	eHTMLCommands_CanGoBackandForward = 40,
	eHTMLCommands_OpenSteamURL = 41,
	eHTMLCommands_SizePopup = 42,
	eHTMLCommands_SetCookie = 43,
	eHTMLCommands_SetTargetFrameRate = 44,
	eHTMLCommands_FullRepaint = 45,
	eHTMLCommands_SetPageScale = 46,
	eHTMLCommands_RequestFullScreen = 47,
	eHTMLCommands_ExitFullScreen = 48,
	eHTMLCommands_GetCookiesForURL = 49,
	eHTMLCommands_ZoomToCurrentlyFocusedNode = 50,
	eHTMLCommands_CloseFullScreenFlashIfOpen = 51,
	eHTMLCommands_PauseFullScreenFlashMovieIfOpen = 52,
	eHTMLCommands_GetFocusedNodeValue = 53,
	eHTMLCommands_BrowserCreateResponse = 54,
	eHTMLCommands_BrowserReady = 55,
	eHTMLCommands_URLChanged = 56,
	eHTMLCommands_FinishedRequest = 57,
	eHTMLCommands_StartRequest = 58,
	eHTMLCommands_ShowPopup = 59,
	eHTMLCommands_HidePopup = 60,
	eHTMLCommands_OpenNewTab = 61,
	eHTMLCommands_PopupHTMLWindow = 62,
	eHTMLCommands_PopupHTMLWindowResponse = 63,
	eHTMLCommands_SetHTMLTitle = 64,
	eHTMLCommands_LoadingResource = 65,
	eHTMLCommands_StatusText = 66,
	eHTMLCommands_SetCursor = 67,
	eHTMLCommands_FileLoadDialog = 68,
	eHTMLCommands_ShowToolTip = 69,
	eHTMLCommands_UpdateToolTip = 70,
	eHTMLCommands_HideToolTip = 71,
	eHTMLCommands_SearchResults = 72,
	eHTMLCommands_Close = 73,
	eHTMLCommands_VerticalScrollBarSizeResponse = 74,
	eHTMLCommands_HorizontalScrollBarSizeResponse = 75,
	eHTMLCommands_GetZoomResponse = 76,
	eHTMLCommands_StartRequestResponse = 77,
	eHTMLCommands_NeedsPaint = 78,
	eHTMLCommands_LinkAtPositionResponse = 79,
	eHTMLCommands_ZoomToElementAtPositionResponse = 80,
	eHTMLCommands_JSDialogResponse = 81,
	eHTMLCommands_ScaleToValueResponse = 82,
	eHTMLCommands_RequestFullScreenResponse = 83,
	eHTMLCommands_GetCookiesForURLResponse = 84,
	eHTMLCommands_NodeGotFocus = 85,
	eHTMLCommands_SavePageToJPEGResponse = 86,
	eHTMLCommands_GetFocusedNodeValueResponse = 87,
	eHTMLCommands_None = 88,
};

class IHTMLChromeController : public IBaseInterface
{
public:
    virtual bool Init(const char *pchHTMLCacheDir, const char *pchCookiePath) = 0;
    virtual void Shutdown() = 0;
    virtual bool RunFrame() = 0;
    virtual void SetWebCookie(const char *pchHostname, const char *pchKey, const char *pchValue, const char *pchPath, int nExpires) = 0;
    virtual void GetWebCookiesForURL(CUtlString *pstrValue, const char *pchURL, const char *pchName) = 0;
    virtual void SetClientBuildID(uint64_t ulBuildID) = 0;
    virtual bool BHasPendingMessages() = 0;
    virtual void CreateBrowser(IHTMLResponses *pBrowser, bool bPopupWindow, const char *pchUserAgentIdentifier) = 0;
    virtual void RemoveBrowser(IHTMLResponses *pBrowser) = 0;
    virtual void WakeThread() = 0;
    virtual HTMLCommandBuffer_t *GetFreeCommandBuffer(EHTMLCommands eCmd, int iBrowser) = 0;
    virtual void PushCommand(HTMLCommandBuffer_t *pCmd) = 0;
    virtual void SetCefThreadTargetFrameRate(uint32_t nFPS) = 0;
    virtual IHTMLSerializer *CreateSerializer(IHTMLResponses_HL1 *pResponseTarget) = 0;
};

#define HTML_CHROME_CONTROLLER_INTERFACE_VERSION "ChromeHTML_Controller_001"

#endif // SDK_IHTMLCHROMECONTROLLER_H

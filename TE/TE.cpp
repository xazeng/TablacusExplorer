// TE.cpp
// Tablacus Explorer (C)2011 Gaku
// MIT Lisence
// Visual Studio Express 2017 for Windows Desktop
// Windows SDK v7.1
// https://tablacus.github.io/

#include "stdafx.h"
#include "TE.h"

// Global Variables:
HINSTANCE hInst;								// current instance
WCHAR	g_szTE[MAX_LOADSTRING];
WCHAR	g_szText[MAX_STATUS];
WCHAR	g_szStatus[MAX_STATUS];
WCHAR	g_szTitle[MAX_STATUS];
HWND	g_hwndMain = NULL;
CteTabCtrl *g_pTC = NULL;
std::vector<CteTabCtrl *> g_ppTC;
HINSTANCE	g_hShell32 = NULL;
HWND		g_hDialog = NULL;
IShellWindows *g_pSW = NULL;

LPFNSHRunDialog lpfnSHRunDialog = NULL;
LPFNRegenerateUserEnvironment lpfnRegenerateUserEnvironment = NULL;
LPFNChangeWindowMessageFilterEx lpfnChangeWindowMessageFilterEx = NULL;
LPFNRtlGetVersion lpfnRtlGetVersion = NULL;
LPFNSetDefaultDllDirectories lpfnSetDefaultDllDirectories = NULL;
LPFNAllowDarkModeForApp lpfnAllowDarkModeForApp = NULL;
LPFNAllowDarkModeForWindow lpfnAllowDarkModeForWindow = NULL;
LPFNDwmSetWindowAttribute lpfnDwmSetWindowAttribute = NULL;
LPFNShouldAppsUseDarkMode lpfnShouldAppsUseDarkMode = NULL;
LPFNIsDarkModeAllowedForWindow lpfnIsDarkModeAllowedForWindow = NULL;
#ifdef _2000XP
LPFNSetDllDirectoryW lpfnSetDllDirectoryW = NULL;
LPFNIsWow64Process lpfnIsWow64Process = NULL;
LPFNPSPropertyKeyFromString lpfnPSPropertyKeyFromString = NULL;
LPFNPSGetPropertyKeyFromName lpfnPSGetPropertyKeyFromName = NULL;
LPFNPSPropertyKeyFromString lpfnPSPropertyKeyFromStringEx = NULL;
LPFNPSGetPropertyDescription lpfnPSGetPropertyDescription = NULL;
LPFNPSStringFromPropertyKey lpfnPSStringFromPropertyKey = NULL;
LPFNPropVariantToVariant lpfnPropVariantToVariant = NULL;
LPFNVariantToPropVariant lpfnVariantToPropVariant = NULL;
LPFNSHCreateItemFromIDList lpfnSHCreateItemFromIDList = NULL;
LPFNSHGetIDListFromObject lpfnSHGetIDListFromObject = NULL;
LPFNChangeWindowMessageFilter lpfnChangeWindowMessageFilter = NULL;
LPFNAddClipboardFormatListener lpfnAddClipboardFormatListener = NULL;
LPFNRemoveClipboardFormatListener lpfnRemoveClipboardFormatListener = NULL;
#else
#define lpfnPSPropertyKeyFromString PSPropertyKeyFromString
#define lpfnPSGetPropertyKeyFromName PSGetPropertyKeyFromName
#define lpfnPSGetPropertyDescription PSGetPropertyDescription
#define lpfnPSPropertyKeyFromStringEx tePSPropertyKeyFromStringEx
#define lpfnSHCreateItemFromIDList SHCreateItemFromIDList
#define lpfnSHGetIDListFromObject SHGetIDListFromObject
#endif
#ifdef _USE_APIHOOK
LPFNRegQueryValueExW lpfnRegQueryValueExW = NULL;
#endif

FORMATETC HDROPFormat = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
FORMATETC IDLISTFormat = {0, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
FORMATETC DROPEFFECTFormat = {0, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
FORMATETC UNICODEFormat = {CF_UNICODETEXT, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
FORMATETC TEXTFormat = {CF_TEXT, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
GUID		g_ClsIdStruct;
GUID		g_ClsIdSB;
GUID		g_ClsIdTC;
GUID		g_ClsIdFI;
IUIAutomation *g_pAutomation = NULL;
PROPERTYID	g_PID_ItemIndex;
EXCEPINFO   g_ExcepInfo;

CTE			*g_pTE;
std::vector<CteShellBrowser *> g_ppSB;
std::vector<CteTreeView *> g_ppTV;
CteWebBrowser *g_pWebBrowser = NULL;
#ifdef _USE_OBJECTAPI
IDispatch	*g_pAPI = NULL;
#endif

LPITEMIDLIST g_pidls[MAX_CSIDL2];
BSTR		 g_bsPidls[MAX_CSIDL2];

IDispatch	*g_pOnFunc[Count_OnFunc];
IDispatch	*g_pUnload = NULL;
std::list<HMODULE>	g_pFreeLibrary;
IDispatch	*g_pJS = NULL;
IDispatch	*g_pSubWindows = NULL;
IStream		*g_pStrmJS = NULL;
std::list<TEFS *>	g_pFolderSize;
CRITICAL_SECTION g_csFolderSize;
IDropTargetHelper *g_pDropTargetHelper = NULL;
IUnknown	*g_pRelease = NULL;
IUnknown	*g_pDraggingCtrl = NULL;
IDataObject	*g_pDraggingItems = NULL;
IDispatchEx *g_pCM = NULL;
ULONG_PTR g_Token;
IWICImagingFactory *g_pWICFactory = NULL;
HHOOK	g_hHook;
HHOOK	g_hMouseHook;
HHOOK	g_hMessageHook;
HHOOK	g_hMenuKeyHook;
HMENU	g_hMenu = NULL;
BSTR	g_bsCmdLine = NULL;
BSTR	g_bsDocumentWrite = NULL;
BSTR	g_bsClipRoot = NULL;
BSTR	g_bsDateTimeFormat = NULL;
HTREEITEM	g_hItemDown = NULL;
SORTCOLUMN g_pSortColumnNull[3];
HBRUSH	g_hbrBackground = NULL;
HMODULE	g_hDwmapi = NULL;

UINT	g_uCrcTable[256];
LONG	g_nSize = 0;
LONG	g_nLockUpdate = 0;
LONG    g_nCountOfThreadFolderSize = 0;
DWORD	g_paramFV[SB_Count];
DWORD	g_dwMainThreadId;
DWORD	g_dwSWCookie = 0;
DWORD	g_dwSessionId = 0;
DWORD	g_dwTickKey;
long	g_nProcKey	   = 0;
long	g_nProcMouse   = 0;
long	g_nProcDrag    = 0;
long	g_nProcFV      = 0;
long	g_nProcTV      = 0;
long	g_nProcFI      = 0;
long	g_nProcIMG     = 0;
long	g_nThreads	   = 0;
long	g_lSWCookie = 0;
int		g_nReload = 0;
int		g_nException = 256;
int		*g_maps[MAP_LENGTH];
int		g_param[Count_TE_params];
int		g_x = MAXINT;
int		g_nTCCount = 0;
int		g_nTCIndex = 0;
BOOL	g_nDropState = 0;
BOOL	g_bLabelsMode;
BOOL	g_bMessageLoop = TRUE;
BOOL	g_bDialogOk = FALSE;
BOOL	g_bInit = FALSE;
BOOL	g_bUnload = FALSE;
BOOL	g_bSetRedraw;
BOOL	g_bShowParseError = TRUE;
BOOL	g_bDragging = FALSE;
BOOL	g_bCanLayout = FALSE;
BOOL	g_bUpper10;
BOOL	g_bDarkMode = FALSE;
#ifdef _2000XP
int		g_nCharWidth = 7;
BOOL	g_bCharWidth = FALSE;
BOOL	g_bUpperVista;
HWND	g_hwndNextClip = NULL;
LPWSTR  g_szTotalFileSizeXP = GetUserDefaultLCID() == 1041 ? L"総ファイル サイズ" : L"Total file size";
LPWSTR  g_szLabelXP = GetUserDefaultLCID() == 1041 ? L"ラベル" : L"Label";
LPWSTR	g_szTotalFileSizeCodeXP = L"System.TotalFileSize";
LPWSTR	g_szLabelCodeXP = L"System.Contact.Label";
#endif
#ifdef _W2000
BOOL	g_bIsXP;
BOOL	g_bIs2000;
#endif
#ifdef _DEBUG
LPWSTR	g_strException;
WCHAR	g_pszException[MAX_PATH];
#endif
#ifdef _CHECK_HANDLELEAK
int g_nLeak = 0;
#endif

TEmethod tesNULL[] =
{
	{ 0, NULL }
};

TEmethod tesBITMAP[] =
{
	{ (VT_I4 << TE_VT) + offsetof(BITMAP, bmType), "bmType" },
	{ (VT_I4 << TE_VT) + offsetof(BITMAP, bmWidth), "bmWidth" },
	{ (VT_I4 << TE_VT) + offsetof(BITMAP, bmHeight), "bmHeight" },
	{ (VT_I4 << TE_VT) + offsetof(BITMAP, bmWidthBytes), "bmWidthBytes" },
	{ (VT_UI2 << TE_VT) + offsetof(BITMAP, bmPlanes), "bmPlanes" },
	{ (VT_UI2 << TE_VT) + offsetof(BITMAP, bmBitsPixel), "bmBitsPixel" },
	{ (VT_PTR << TE_VT) + offsetof(BITMAP, bmBits), "bmBits" },
	{ 0, NULL }
};

TEmethod tesCHOOSECOLOR[] =
{
	{ (VT_I4 << TE_VT) + offsetof(CHOOSECOLOR, lStructSize), "lStructSize" },
	{ (VT_PTR << TE_VT) + offsetof(CHOOSECOLOR, hwndOwner), "hwndOwner" },
	{ (VT_PTR << TE_VT) + offsetof(CHOOSECOLOR, hInstance), "hInstance" },
	{ (VT_I4 << TE_VT) + offsetof(CHOOSECOLOR, rgbResult), "rgbResult" },
	{ (VT_PTR << TE_VT) + offsetof(CHOOSECOLOR, lpCustColors), "lpCustColors" },
	{ (VT_I4 << TE_VT) + offsetof(CHOOSECOLOR, Flags), "Flags" },
	{ (VT_PTR << TE_VT) + offsetof(CHOOSECOLOR, lCustData), "lCustData" },
	{ (VT_PTR << TE_VT) + offsetof(CHOOSECOLOR, lpfnHook), "lpfnHook" },
	{ (VT_BSTR << TE_VT) + offsetof(CHOOSECOLOR, lpTemplateName), "lpTemplateName" },
	{ 0, NULL }
};

TEmethod tesCHOOSEFONT[] =
{
	{ (VT_I4 << TE_VT) + offsetof(CHOOSEFONT, lStructSize), "lStructSize" },
	{ (VT_PTR << TE_VT) + offsetof(CHOOSEFONT, hwndOwner), "hwndOwner" },
	{ (VT_PTR << TE_VT) + offsetof(CHOOSEFONT, hDC), "hDC" },
	{ (VT_PTR << TE_VT) + offsetof(CHOOSEFONT, lpLogFont), "lpLogFont" },
	{ (VT_I4 << TE_VT) + offsetof(CHOOSEFONT, iPointSize), "iPointSize" },
	{ (VT_I4 << TE_VT) + offsetof(CHOOSEFONT, Flags), "Flags" },
	{ (VT_I4 << TE_VT) + offsetof(CHOOSEFONT, rgbColors), "rgbColors" },
	{ (VT_PTR << TE_VT) + offsetof(CHOOSEFONT, lCustData), "lCustData" },
	{ (VT_PTR << TE_VT) + offsetof(CHOOSEFONT, lpfnHook), "lpfnHook" },
	{ (VT_BSTR << TE_VT) + offsetof(CHOOSEFONT, lpTemplateName), "lpTemplateName" },
	{ (VT_PTR << TE_VT) + offsetof(CHOOSEFONT, hInstance), "hInstance" },
	{ (VT_BSTR << TE_VT) + offsetof(CHOOSEFONT, lpszStyle), "lpszStyle" },
	{ (VT_UI2 << TE_VT) + offsetof(CHOOSEFONT, nFontType), "nFontType" },
	{ (VT_UI2 << TE_VT) + offsetof(CHOOSEFONT, ___MISSING_ALIGNMENT__), "___MISSING_ALIGNMENT__" },
	{ (VT_I4 << TE_VT) + offsetof(CHOOSEFONT, nSizeMin), "nSizeMin" },
	{ (VT_I4 << TE_VT) + offsetof(CHOOSEFONT, nSizeMax), "nSizeMax" },
	{ 0, NULL }
};

TEmethod tesCOPYDATASTRUCT[] =
{
	{ (VT_PTR << TE_VT) + offsetof(COPYDATASTRUCT, dwData), "dwData" },
	{ (VT_I4 << TE_VT) + offsetof(COPYDATASTRUCT, cbData), "cbData" },
	{ (VT_PTR << TE_VT) + offsetof(COPYDATASTRUCT, lpData), "lpData" },
	{ 0, NULL }
};

TEmethod tesDIBSECTION[] =
{
	{ (VT_PTR << TE_VT) + offsetof(DIBSECTION, dsBm), "dsBm" },
	{ (VT_PTR << TE_VT) + offsetof(DIBSECTION, dsBmih), "dsBmih" },
	{ (VT_I4 << TE_VT) + offsetof(DIBSECTION, dsBitfields), "dsBitfields0" },
	{ (VT_I4 << TE_VT) + offsetof(DIBSECTION, dsBitfields) + sizeof(DWORD), "dsBitfields1" },
	{ (VT_I4 << TE_VT) + offsetof(DIBSECTION, dsBitfields) + sizeof(DWORD) * 2, "dsBitfields2" },
	{ (VT_PTR << TE_VT) + offsetof(DIBSECTION, dshSection), "dshSection" },
	{ (VT_I4 << TE_VT) + offsetof(DIBSECTION, dsOffset), "dsOffset" },
	{ 0, NULL }
};

TEmethod tesEXCEPINFO[] =
{
	{ (VT_UI2 << TE_VT) + offsetof(EXCEPINFO, wCode), "wCode" },
	{ (VT_UI2 << TE_VT) + offsetof(EXCEPINFO, wReserved), "wReserved" },
	{ (VT_BSTR << TE_VT) + offsetof(EXCEPINFO, bstrSource), "bstrSource" },
	{ (VT_BSTR << TE_VT) + offsetof(EXCEPINFO, bstrDescription), "bstrDescription" },
	{ (VT_BSTR << TE_VT) + offsetof(EXCEPINFO, bstrHelpFile), "bstrHelpFile" },
	{ (VT_I4 << TE_VT) + offsetof(EXCEPINFO, dwHelpContext), "dwHelpContext" },
	{ (VT_PTR << TE_VT) + offsetof(EXCEPINFO, pvReserved), "pvReserved" },
	{ (VT_PTR << TE_VT) + offsetof(EXCEPINFO, pfnDeferredFillIn), "pfnDeferredFillIn" },
	{ (VT_I4 << TE_VT) + offsetof(EXCEPINFO, scode), "scode" },
	{ 0, NULL }
};

TEmethod tesFINDREPLACE[] =
{
	{ (VT_I4 << TE_VT) + offsetof(FINDREPLACE, lStructSize), "lStructSize" },
	{ (VT_PTR << TE_VT) + offsetof(FINDREPLACE, hwndOwner), "hwndOwner" },
	{ (VT_PTR << TE_VT) + offsetof(FINDREPLACE, hInstance), "hInstance" },
	{ (VT_I4 << TE_VT) + offsetof(FINDREPLACE, Flags), "Flags" },
	{ (VT_BSTR << TE_VT) + offsetof(FINDREPLACE, lpstrFindWhat), "lpstrFindWhat" },
	{ (VT_BSTR << TE_VT) + offsetof(FINDREPLACE, lpstrReplaceWith), "lpstrReplaceWith" },
	{ (VT_UI2 << TE_VT) + offsetof(FINDREPLACE, wFindWhatLen), "wFindWhatLen" },
	{ (VT_UI2 << TE_VT) + offsetof(FINDREPLACE, wReplaceWithLen), "wReplaceWithLen" },
	{ (VT_PTR << TE_VT) + offsetof(FINDREPLACE, lCustData), "lCustData" },
	{ (VT_PTR << TE_VT) + offsetof(FINDREPLACE, lpfnHook), "lpfnHook" },
	{ (VT_BSTR << TE_VT) + offsetof(FINDREPLACE, lpTemplateName), "lpTemplateName" },
	{ 0, NULL }
};

TEmethod tesFOLDERSETTINGS[] =
{
	{ (VT_I4 << TE_VT) + offsetof(FOLDERSETTINGS, ViewMode), "ViewMode" },
	{ (VT_I4 << TE_VT) + offsetof(FOLDERSETTINGS, fFlags), "fFlags" },
	{ (VT_I4 << TE_VT) + (SB_Options - 1) * 4, "Options" },
	{ (VT_I4 << TE_VT) + (SB_ViewFlags - 1) * 4, "ViewFlags" },
	{ (VT_I4 << TE_VT) + (SB_IconSize - 1) * 4, "ImageSize" },
	{ 0, NULL }
};

TEmethod tesHDITEM[] =
{
	{ (VT_I4 << TE_VT) + offsetof(HDITEM, mask), "mask" },
	{ (VT_I4 << TE_VT) + offsetof(HDITEM, cxy), "cxy" },
	{ (VT_BSTR << TE_VT) + offsetof(HDITEM, pszText), "pszText" },
	{ (VT_PTR << TE_VT) + offsetof(HDITEM, hbm), "hbm" },
	{ (VT_I4 << TE_VT) + offsetof(HDITEM, cchTextMax), "cchTextMax" },
	{ (VT_I4 << TE_VT) + offsetof(HDITEM, fmt), "fmt" },
	{ (VT_I4 << TE_VT) + offsetof(HDITEM, lParam), "lParam" },
	{ (VT_I4 << TE_VT) + offsetof(HDITEM, iImage), "iImage" },
	{ (VT_I4 << TE_VT) + offsetof(HDITEM, iOrder), "iOrder" },
	{ (VT_I4 << TE_VT) + offsetof(HDITEM, type), "type" },
	{ (VT_PTR << TE_VT) + offsetof(HDITEM, pvFilter), "pvFilter" },
	{ (VT_I4 << TE_VT) + offsetof(HDITEM, state), "state" },
	{ 0, NULL }
};

TEmethod tesICONINFO[] =
{
	{ (VT_BOOL << TE_VT) + offsetof(ICONINFO, fIcon), "fIcon" },
	{ (VT_I4 << TE_VT) + offsetof(ICONINFO, xHotspot), "xHotspot" },
	{ (VT_I4 << TE_VT) + offsetof(ICONINFO, yHotspot), "yHotspot" },
	{ (VT_PTR << TE_VT) + offsetof(ICONINFO, hbmMask), "hbmMask" },
	{ (VT_PTR << TE_VT) + offsetof(ICONINFO, hbmColor), "hbmColor" },
	{ 0, NULL }
};

TEmethod tesICONMETRICS[] =
{
	{ (VT_I4 << TE_VT) + offsetof(ICONMETRICS, iHorzSpacing), "iHorzSpacing" },
	{ (VT_I4 << TE_VT) + offsetof(ICONMETRICS, iVertSpacing), "iVertSpacing" },
	{ (VT_I4 << TE_VT) + offsetof(ICONMETRICS, iTitleWrap), "iTitleWrap" },
	{ (VT_PTR << TE_VT) + offsetof(ICONMETRICS, lfFont), "lfFont" },
	{ 0, NULL }
};

TEmethod tesKEYBDINPUT[] =
{
	{ (VT_I4 << TE_VT), "type" },
	{ (VT_I4 << TE_VT) + offsetof(KEYBDINPUT, wVk), "wVk" },
	{ (VT_I4 << TE_VT) + offsetof(KEYBDINPUT, wScan), "wScan" },
	{ (VT_I4 << TE_VT) + offsetof(KEYBDINPUT, dwFlags), "dwFlags" },
	{ (VT_I4 << TE_VT) + offsetof(KEYBDINPUT, time), "time" },
	{ (VT_PTR << TE_VT) + offsetof(KEYBDINPUT, dwExtraInfo), "dwExtraInfo" },
	{ 0, NULL }
};

TEmethod tesLOGFONT[] =
{
	{ (VT_I4 << TE_VT) + offsetof(LOGFONT, lfHeight), "lfHeight" },
	{ (VT_I4 << TE_VT) + offsetof(LOGFONT, lfWidth), "lfWidth" },
	{ (VT_I4 << TE_VT) + offsetof(LOGFONT, lfEscapement), "lfEscapement" },
	{ (VT_I4 << TE_VT) + offsetof(LOGFONT, lfOrientation), "lfOrientation" },
	{ (VT_I4 << TE_VT) + offsetof(LOGFONT, lfWeight), "lfWeight" },
	{ (VT_UI1 << TE_VT) + offsetof(LOGFONT, lfItalic), "lfItalic" },
	{ (VT_UI1 << TE_VT) + offsetof(LOGFONT, lfUnderline), "lfUnderline" },
	{ (VT_UI1 << TE_VT) + offsetof(LOGFONT, lfStrikeOut), "lfStrikeOut" },
	{ (VT_UI1 << TE_VT) + offsetof(LOGFONT, lfCharSet), "lfCharSet" },
	{ (VT_UI1 << TE_VT) + offsetof(LOGFONT, lfOutPrecision), "lfOutPrecision" },
	{ (VT_UI1 << TE_VT) + offsetof(LOGFONT, lfClipPrecision), "lfClipPrecision" },
	{ (VT_UI1 << TE_VT) + offsetof(LOGFONT, lfQuality), "lfQuality" },
	{ (VT_UI1 << TE_VT) + offsetof(LOGFONT, lfPitchAndFamily), "lfPitchAndFamily" },
	{ (VT_LPWSTR << TE_VT) + offsetof(LOGFONT, lfFaceName), "lfFaceName" },
	{ 0, NULL }
};

TEmethod tesLVBKIMAGE[] =
{
	{ (VT_I4 << TE_VT) + offsetof(LVBKIMAGE, ulFlags), "ulFlags" },
	{ (VT_PTR << TE_VT) + offsetof(LVBKIMAGE, hbm), "hbm" },
	{ (VT_BSTR << TE_VT) + offsetof(LVBKIMAGE, pszImage), "pszImage" },
	{ (VT_I4 << TE_VT) + offsetof(LVBKIMAGE, cchImageMax), "cchImageMax" },
	{ (VT_I4 << TE_VT) + offsetof(LVBKIMAGE, xOffsetPercent), "xOffsetPercent" },
	{ (VT_I4 << TE_VT) + offsetof(LVBKIMAGE, yOffsetPercent), "yOffsetPercent" },
	{ 0, NULL }
};

TEmethod tesLVFINDINFO[] =
{
	{ (VT_I4 << TE_VT) + offsetof(LVFINDINFO, flags), "flags" },
	{ (VT_BSTR << TE_VT) + offsetof(LVFINDINFO, psz), "psz" },
	{ (VT_I4 << TE_VT) + offsetof(LVFINDINFO, lParam), "lParam" },
	{ (VT_CY << TE_VT) + offsetof(LVFINDINFO, pt), "pt" },
	{ (VT_I4 << TE_VT) + offsetof(LVFINDINFO, vkDirection), "vkDirection" },
	{ 0, NULL }
};

TEmethod tesLVGROUP[] =
{
	{ (VT_I4 << TE_VT) + offsetof(LVGROUP, cbSize), "cbSize" },
	{ (VT_I4 << TE_VT) + offsetof(LVGROUP, mask), "mask" },
	{ (VT_BSTR << TE_VT) + offsetof(LVGROUP, pszHeader), "pszHeader" },
	{ (VT_I4 << TE_VT) + offsetof(LVGROUP, cchHeader), "cchHeader" },
	{ (VT_BSTR << TE_VT) + offsetof(LVGROUP, pszFooter), "pszFooter" },
	{ (VT_I4 << TE_VT) + offsetof(LVGROUP, cchFooter), "cchFooter" },
	{ (VT_I4 << TE_VT) + offsetof(LVGROUP, iGroupId), "iGroupId" },
	{ (VT_I4 << TE_VT) + offsetof(LVGROUP, stateMask), "stateMask" },
	{ (VT_I4 << TE_VT) + offsetof(LVGROUP, state), "state" },
	{ (VT_I4 << TE_VT) + offsetof(LVGROUP, uAlign), "uAlign" },
	{ (VT_BSTR << TE_VT) + offsetof(LVGROUP, pszSubtitle), "pszSubtitle" },
	{ (VT_I4 << TE_VT) + offsetof(LVGROUP, cchSubtitle), "cchSubtitle" },
	{ (VT_BSTR << TE_VT) + offsetof(LVGROUP, pszTask), "pszTask" },
	{ (VT_I4 << TE_VT) + offsetof(LVGROUP, cchTask), "cchTask" },
	{ (VT_BSTR << TE_VT) + offsetof(LVGROUP, pszDescriptionTop), "pszDescriptionTop" },
	{ (VT_I4 << TE_VT) + offsetof(LVGROUP, cchDescriptionTop), "cchDescriptionTop" },
	{ (VT_BSTR << TE_VT) + offsetof(LVGROUP, pszDescriptionBottom), "pszDescriptionBottom" },
	{ (VT_I4 << TE_VT) + offsetof(LVGROUP, cchDescriptionBottom), "cchDescriptionBottom" },
	{ (VT_I4 << TE_VT) + offsetof(LVGROUP, iTitleImage), "iTitleImage" },
	{ (VT_I4 << TE_VT) + offsetof(LVGROUP, iExtendedImage), "iExtendedImage" },
	{ (VT_I4 << TE_VT) + offsetof(LVGROUP, iFirstItem), "iFirstItem" },
	{ (VT_I4 << TE_VT) + offsetof(LVGROUP, cItems), "cItems" },
	{ (VT_BSTR << TE_VT) + offsetof(LVGROUP, pszSubsetTitle), "pszSubsetTitle" },
	{ (VT_I4 << TE_VT) + offsetof(LVGROUP, cchSubsetTitle), "cchSubsetTitle" },
	{ 0, NULL }
};

TEmethod tesLVHITTESTINFO[] =
{
	{ (VT_CY << TE_VT) + offsetof(LVHITTESTINFO, pt), "pt" },
	{ (VT_I4 << TE_VT) + offsetof(LVHITTESTINFO, flags), "flags" },
	{ (VT_I4 << TE_VT) + offsetof(LVHITTESTINFO, iItem), "iItem" },
	{ (VT_I4 << TE_VT) + offsetof(LVHITTESTINFO, iSubItem ), "iSubItem" },
	{ (VT_I4 << TE_VT) + offsetof(LVHITTESTINFO, iGroup), "iGroup" },
	{ 0, NULL }
};

TEmethod tesLVITEM[] =
{
	{ (VT_I4 << TE_VT) + offsetof(LVITEM, mask), "mask" },
	{ (VT_I4 << TE_VT) + offsetof(LVITEM, iItem), "iItem" },
	{ (VT_I4 << TE_VT) + offsetof(LVITEM, iSubItem), "iSubItem" },
	{ (VT_I4 << TE_VT) + offsetof(LVITEM, state), "state" },
	{ (VT_I4 << TE_VT) + offsetof(LVITEM, stateMask), "stateMask" },
	{ (VT_BSTR << TE_VT) + offsetof(LVITEM, pszText), "pszText" },
	{ (VT_I4 << TE_VT) + offsetof(LVITEM, cchTextMax), "cchTextMax" },
	{ (VT_I4 << TE_VT) + offsetof(LVITEM, iImage), "iImage" },
	{ (VT_I4 << TE_VT) + offsetof(LVITEM, lParam), "lParam" },
	{ (VT_I4 << TE_VT) + offsetof(LVITEM, iIndent), "iIndent" },
	{ (VT_I4 << TE_VT) + offsetof(LVITEM, iGroupId), "iGroupId" },
	{ (VT_I4 << TE_VT) + offsetof(LVITEM, cColumns), "cColumns" },
	{ (VT_PTR << TE_VT) + offsetof(LVITEM, puColumns), "puColumns" },
	{ (VT_PTR << TE_VT) + offsetof(LVITEM, piColFmt), "piColFmt" },
	{ (VT_I4 << TE_VT) + offsetof(LVITEM, iGroup), "iGroup" },
	{ 0, NULL }
};

TEmethod tesMENUITEMINFO[] =
{
	{ (VT_I4 << TE_VT) + offsetof(MENUITEMINFO, cbSize), "cbSize" },
	{ (VT_I4 << TE_VT) + offsetof(MENUITEMINFO, fMask), "fMask" },
	{ (VT_I4 << TE_VT) + offsetof(MENUITEMINFO, fType), "fType" },
	{ (VT_I4 << TE_VT) + offsetof(MENUITEMINFO, fState), "fState" },
	{ (VT_I4 << TE_VT) + offsetof(MENUITEMINFO, wID), "wID" },
	{ (VT_PTR << TE_VT) + offsetof(MENUITEMINFO, hSubMenu), "hSubMenu" },
	{ (VT_PTR << TE_VT) + offsetof(MENUITEMINFO, hbmpChecked), "hbmpChecked" },
	{ (VT_PTR << TE_VT) + offsetof(MENUITEMINFO, hbmpUnchecked), "hbmpUnchecked" },
	{ (VT_PTR << TE_VT) + offsetof(MENUITEMINFO, dwItemData), "dwItemData" },
	{ (VT_BSTR << TE_VT) + offsetof(MENUITEMINFO, dwTypeData), "dwTypeData" },
	{ (VT_I4 << TE_VT) + offsetof(MENUITEMINFO, cch), "cch" },
	{ (VT_PTR << TE_VT) + offsetof(MENUITEMINFO, hbmpItem), "hbmpItem" },
	{ 0, NULL }
};

TEmethod tesMONITORINFOEX[] =
{
	{ (VT_I4 << TE_VT), "cbSize" },
	{ (VT_CARRAY << TE_VT) + offsetof(MONITORINFOEX, rcMonitor), "rcMonitor" },
	{ (VT_CARRAY << TE_VT) + offsetof(MONITORINFOEX, rcWork), "rcWork" },
	{ (VT_I4 << TE_VT) + offsetof(MONITORINFOEX, dwFlags), "dwFlags" },
	{ (VT_LPWSTR << TE_VT) + offsetof(MONITORINFOEX, szDevice), "szDevice" },
	{ 0, NULL }
};


TEmethod tesMOUSEINPUT[] =
{
	{ (VT_I4 << TE_VT), "type" },
	{ (VT_I4 << TE_VT) + offsetof(MOUSEINPUT, dx), "dx" },
	{ (VT_I4 << TE_VT) + offsetof(MOUSEINPUT, dy), "dy" },
	{ (VT_I4 << TE_VT) + offsetof(MOUSEINPUT, mouseData), "mouseData" },
	{ (VT_I4 << TE_VT) + offsetof(MOUSEINPUT, dwFlags), "dwFlags" },
	{ (VT_I4 << TE_VT) + offsetof(MOUSEINPUT, time), "time" },
	{ (VT_PTR << TE_VT) + offsetof(MOUSEINPUT, dwExtraInfo), "dwExtraInfo" },
	{ 0, NULL }
};

TEmethod tesMSG[] =
{
	{ (VT_PTR << TE_VT) + offsetof(MSG, hwnd), "hwnd" },
	{ (VT_I4 << TE_VT) + offsetof(MSG, message), "message" },
	{ (VT_PTR << TE_VT) + offsetof(MSG, wParam), "wParam" },
	{ (VT_PTR << TE_VT) + offsetof(MSG, lParam), "lParam" },
	{ (VT_I4 << TE_VT) + offsetof(MSG, time), "time" },
	{ (VT_CY << TE_VT) + offsetof(MSG, pt), "pt" },
	{ 0, NULL }
};

TEmethod tesNONCLIENTMETRICS[] =
{
	{ (VT_I4 << TE_VT) + offsetof(NONCLIENTMETRICS, cbSize), "cbSize" },
	{ (VT_I4 << TE_VT) + offsetof(NONCLIENTMETRICS, iBorderWidth), "iBorderWidth" },
	{ (VT_I4 << TE_VT) + offsetof(NONCLIENTMETRICS, iScrollWidth), "iScrollWidth" },
	{ (VT_I4 << TE_VT) + offsetof(NONCLIENTMETRICS, iScrollHeight), "iScrollHeight" },
	{ (VT_I4 << TE_VT) + offsetof(NONCLIENTMETRICS, iCaptionWidth), "iCaptionWidth" },
	{ (VT_I4 << TE_VT) + offsetof(NONCLIENTMETRICS, iCaptionHeight), "iCaptionHeight" },
	{ (VT_PTR << TE_VT) + offsetof(NONCLIENTMETRICS, lfCaptionFont), "lfCaptionFont" },
	{ (VT_I4 << TE_VT) + offsetof(NONCLIENTMETRICS, iSmCaptionWidth), "iSmCaptionWidth" },
	{ (VT_I4 << TE_VT) + offsetof(NONCLIENTMETRICS, iSmCaptionHeight), "iSmCaptionHeight" },
	{ (VT_PTR << TE_VT) + offsetof(NONCLIENTMETRICS, lfSmCaptionFont), "lfSmCaptionFont" },
	{ (VT_I4 << TE_VT) + offsetof(NONCLIENTMETRICS, iMenuWidth), "iMenuWidth" },
	{ (VT_I4 << TE_VT) + offsetof(NONCLIENTMETRICS, iMenuHeight), "iMenuHeight" },
	{ (VT_PTR << TE_VT) + offsetof(NONCLIENTMETRICS, lfMenuFont), "lfMenuFont" },
	{ (VT_PTR << TE_VT) + offsetof(NONCLIENTMETRICS, lfStatusFont), "lfStatusFont" },
	{ (VT_PTR << TE_VT) + offsetof(NONCLIENTMETRICS, lfMessageFont), "lfMessageFont" },
	{ (VT_I4 << TE_VT) + offsetof(NONCLIENTMETRICS, iPaddedBorderWidth), "iPaddedBorderWidth" },
	{ 0, NULL }
};

TEmethod tesNOTIFYICONDATA[] =
{
	{ (VT_I4 << TE_VT) + offsetof(NOTIFYICONDATA, cbSize), "cbSize" },
	{ (VT_PTR << TE_VT) + offsetof(NOTIFYICONDATA, hWnd), "hWnd" },
	{ (VT_I4 << TE_VT) + offsetof(NOTIFYICONDATA, uID), "uID" },
	{ (VT_I4 << TE_VT) + offsetof(NOTIFYICONDATA, uFlags), "uFlags" },
	{ (VT_I4 << TE_VT) + offsetof(NOTIFYICONDATA, uCallbackMessage), "uCallbackMessage" },
	{ (VT_PTR << TE_VT) + offsetof(NOTIFYICONDATA, hIcon), "hIcon" },
	{ (VT_LPWSTR << TE_VT) + offsetof(NOTIFYICONDATA, szTip), "szTip" },
	{ (VT_I4 << TE_VT) + offsetof(NOTIFYICONDATA, dwState), "dwState" },
	{ (VT_I4 << TE_VT) + offsetof(NOTIFYICONDATA, dwState), "dwState" },
	{ (VT_I4 << TE_VT) + offsetof(NOTIFYICONDATA, dwStateMask), "dwStateMask" },
	{ (VT_LPWSTR << TE_VT) + offsetof(NOTIFYICONDATA, szInfo), "szInfo" },
	{ (VT_I4 << TE_VT) + offsetof(NOTIFYICONDATA, uTimeout), "uTimeout" },
	{ (VT_I4 << TE_VT) + offsetof(NOTIFYICONDATA, uVersion), "uVersion" },
	{ (VT_LPWSTR << TE_VT) + offsetof(NOTIFYICONDATA, szInfoTitle), "szInfoTitle" },
	{ (VT_I4 << TE_VT) + offsetof(NOTIFYICONDATA, dwInfoFlags), "dwInfoFlags" },
	{ (VT_PTR << TE_VT) + offsetof(NOTIFYICONDATA, guidItem), "guidItem" },
	{ (VT_PTR << TE_VT) + offsetof(NOTIFYICONDATA, hBalloonIcon), "hBalloonIcon" },
	{ 0, NULL }
};

TEmethod tesNMCUSTOMDRAW[] =
{
	{ (VT_PTR << TE_VT) + offsetof(NMCUSTOMDRAW, hdr), "hdr" },
	{ (VT_I4 << TE_VT) + offsetof(NMCUSTOMDRAW, dwDrawStage), "dwDrawStage" },
	{ (VT_PTR << TE_VT) + offsetof(NMCUSTOMDRAW, hdc), "hdc" },
	{ (VT_CARRAY << TE_VT) + offsetof(NMCUSTOMDRAW, rc), "rc" },
	{ (VT_PTR << TE_VT) + offsetof(NMCUSTOMDRAW, dwItemSpec), "dwItemSpec" },
	{ (VT_I4 << TE_VT) + offsetof(NMCUSTOMDRAW, uItemState), "uItemState" },
	{ (VT_PTR << TE_VT) + offsetof(NMCUSTOMDRAW, lItemlParam), "lItemlParam" },
	{ 0, NULL }
};

TEmethod tesNMLVCUSTOMDRAW[] =
{
	{ (VT_PTR << TE_VT) + offsetof(NMLVCUSTOMDRAW, nmcd), "nmcd" },
	{ (VT_I4 << TE_VT) + offsetof(NMLVCUSTOMDRAW, clrText), "clrText" },
	{ (VT_I4 << TE_VT) + offsetof(NMLVCUSTOMDRAW, clrTextBk), "clrTextBk" },
	{ (VT_I4 << TE_VT) + offsetof(NMLVCUSTOMDRAW, iSubItem), "iSubItem" },
	{ (VT_I4 << TE_VT) + offsetof(NMLVCUSTOMDRAW, dwItemType), "dwItemType" },
	{ (VT_I4 << TE_VT) + offsetof(NMLVCUSTOMDRAW, clrFace), "clrFace" },
	{ (VT_I4 << TE_VT) + offsetof(NMLVCUSTOMDRAW, iIconEffect), "iIconEffect" },
	{ (VT_I4 << TE_VT) + offsetof(NMLVCUSTOMDRAW, iIconPhase), "iIconPhase" },
	{ (VT_I4 << TE_VT) + offsetof(NMLVCUSTOMDRAW, iPartId), "iPartId" },
	{ (VT_I4 << TE_VT) + offsetof(NMLVCUSTOMDRAW, iStateId), "iStateId" },
	{ (VT_CARRAY << TE_VT) + offsetof(NMLVCUSTOMDRAW, rcText), "rcText" },
	{ (VT_I4 << TE_VT) + offsetof(NMLVCUSTOMDRAW, uAlign), "uAlign" },
	{ 0, NULL }
};

TEmethod tesNMTVCUSTOMDRAW[] =
{
	{ (VT_PTR << TE_VT) + offsetof(NMTVCUSTOMDRAW, nmcd), "nmcd" },
	{ (VT_I4 << TE_VT) + offsetof(NMTVCUSTOMDRAW, clrText), "clrText" },
	{ (VT_I4 << TE_VT) + offsetof(NMTVCUSTOMDRAW, clrTextBk), "clrTextBk" },
	{ (VT_I4 << TE_VT) + offsetof(NMTVCUSTOMDRAW, iLevel), "iLevel" },
	{ 0, NULL }
};

TEmethod tesNMHDR[] =
{
	{ (VT_PTR << TE_VT) + offsetof(NMHDR, hwndFrom), "hwndFrom" },
	{ (VT_I4 << TE_VT) + offsetof(NMHDR, idFrom), "idFrom" },
	{ (VT_I4 << TE_VT) + offsetof(NMHDR, code), "code" },
	{ 0, NULL }
};

TEmethod tesOSVERSIONINFOEX[] =
{
	{ (VT_I4 << TE_VT) + offsetof(OSVERSIONINFOEX, dwOSVersionInfoSize), "dwOSVersionInfoSize" },
	{ (VT_I4 << TE_VT) + offsetof(OSVERSIONINFOEX, dwMajorVersion), "dwMajorVersion" },
	{ (VT_I4 << TE_VT) + offsetof(OSVERSIONINFOEX, dwMinorVersion), "dwMinorVersion" },
	{ (VT_I4 << TE_VT) + offsetof(OSVERSIONINFOEX, dwBuildNumber), "dwBuildNumber" },
	{ (VT_I4 << TE_VT) + offsetof(OSVERSIONINFOEX, dwPlatformId), "dwPlatformId" },
	{ (VT_LPWSTR << TE_VT) + offsetof(OSVERSIONINFOEX, szCSDVersion), "szCSDVersion" },
	{ (VT_UI2 << TE_VT) + offsetof(OSVERSIONINFOEX, wServicePackMajor), "wServicePackMajor" },
	{ (VT_UI2 << TE_VT) + offsetof(OSVERSIONINFOEX, wServicePackMinor), "wServicePackMinor" },
	{ (VT_UI2 << TE_VT) + offsetof(OSVERSIONINFOEX, wSuiteMask), "wSuiteMask" },
	{ (VT_UI1 << TE_VT) + offsetof(OSVERSIONINFOEX, wProductType), "wProductType" },
	{ (VT_UI1 << TE_VT) + offsetof(OSVERSIONINFOEX, wReserved), "wReserved" },
	{ 0, NULL }
};

TEmethod tesPAINTSTRUCT[] =
{
	{ (VT_PTR << TE_VT) + offsetof(PAINTSTRUCT, hdc), "hdc" },
	{ (VT_BOOL << TE_VT) + offsetof(PAINTSTRUCT, fErase), "fErase" },
	{ (VT_CARRAY << TE_VT) + offsetof(PAINTSTRUCT, rcPaint), "rcPaint" },
	{ (VT_BOOL << TE_VT) + offsetof(PAINTSTRUCT, fRestore), "fRestore" },
	{ (VT_BOOL << TE_VT) + offsetof(PAINTSTRUCT, fIncUpdate), "fIncUpdate" },
	{ (VT_UI1 << TE_VT) + offsetof(PAINTSTRUCT, rgbReserved), "rgbReserved" },
	{ 0, NULL }
};

TEmethod tesPOINT[] =
{
	{ (VT_I4 << TE_VT) + offsetof(POINT, x), "x" },
	{ (VT_I4 << TE_VT) + offsetof(POINT, y), "y" },
	{ 0, NULL }
};

TEmethod tesRECT[] =
{
	{ (VT_I4 << TE_VT) + offsetof(RECT, left), "left" },
	{ (VT_I4 << TE_VT) + offsetof(RECT, top), "top" },
	{ (VT_I4 << TE_VT) + offsetof(RECT, right), "right" },
	{ (VT_I4 << TE_VT) + offsetof(RECT, bottom), "bottom" },
	{ 0, NULL }
};

TEmethod tesSHELLEXECUTEINFO[] =
{
	{ (VT_I4 << TE_VT) + offsetof(SHELLEXECUTEINFO, cbSize), "cbSize" },
	{ (VT_I4 << TE_VT) + offsetof(SHELLEXECUTEINFO, fMask), "fMask" },
	{ (VT_PTR << TE_VT) + offsetof(SHELLEXECUTEINFO, hwnd), "hwnd" },
	{ (VT_BSTR << TE_VT) + offsetof(SHELLEXECUTEINFO, lpVerb), "lpVerb" },
	{ (VT_BSTR << TE_VT) + offsetof(SHELLEXECUTEINFO, lpFile), "lpFile" },
	{ (VT_BSTR << TE_VT) + offsetof(SHELLEXECUTEINFO, lpParameters), "lpParameters" },
	{ (VT_BSTR << TE_VT) + offsetof(SHELLEXECUTEINFO, lpDirectory), "lpDirectory" },
	{ (VT_I4 << TE_VT) + offsetof(SHELLEXECUTEINFO, nShow), "nShow" },
	{ (VT_PTR << TE_VT) + offsetof(SHELLEXECUTEINFO, hInstApp), "hInstApp" },
	{ (VT_PTR << TE_VT) + offsetof(SHELLEXECUTEINFO, lpIDList), "lpIDList" },
	{ (VT_BSTR << TE_VT) + offsetof(SHELLEXECUTEINFO, lpClass), "lpClass" },
	{ (VT_PTR << TE_VT) + offsetof(SHELLEXECUTEINFO, hkeyClass), "hkeyClass" },
	{ (VT_I4 << TE_VT) + offsetof(SHELLEXECUTEINFO, dwHotKey), "dwHotKey" },
	{ (VT_PTR << TE_VT) + offsetof(SHELLEXECUTEINFO, hIcon), "hIcon" },
	{ (VT_PTR << TE_VT) + offsetof(SHELLEXECUTEINFO, hMonitor), "hMonitor" },
	{ (VT_PTR << TE_VT) + offsetof(SHELLEXECUTEINFO, hProcess), "hProcess" },
	{ 0, NULL }
};

TEmethod tesSHFILEINFO[] =
{
	{ (VT_PTR << TE_VT) + offsetof(SHFILEINFO, hIcon), "hIcon" },
	{ (VT_I4 << TE_VT) + offsetof(SHFILEINFO, iIcon), "iIcon" },
	{ (VT_I4 << TE_VT) + offsetof(SHFILEINFO, dwAttributes), "dwAttributes" },
	{ (VT_LPWSTR << TE_VT) + offsetof(SHFILEINFO, szDisplayName), "szDisplayName" },
	{ (VT_LPWSTR << TE_VT) + offsetof(SHFILEINFO, szTypeName), "szTypeName" },
	{ 0, NULL }
};

TEmethod tesSHFILEOPSTRUCT[] =
{
	{ (VT_PTR << TE_VT) + offsetof(SHFILEOPSTRUCT, hwnd), "hwnd" },
	{ (VT_I4 << TE_VT) + offsetof(SHFILEOPSTRUCT, wFunc), "wFunc" },
	{ (VT_BSTR << TE_VT) + offsetof(SHFILEOPSTRUCT, pFrom), "pFrom" },
	{ (VT_BSTR << TE_VT) + offsetof(SHFILEOPSTRUCT, pTo), "pTo" },
	{ (VT_UI2 << TE_VT) + offsetof(SHFILEOPSTRUCT, fFlags), "fFlags" },
	{ (VT_BOOL << TE_VT) + offsetof(SHFILEOPSTRUCT, fAnyOperationsAborted), "fAnyOperationsAborted" },
	{ (VT_PTR << TE_VT) + offsetof(SHFILEOPSTRUCT, hNameMappings), "hNameMappings" },
	{ (VT_BSTR << TE_VT) + offsetof(SHFILEOPSTRUCT, lpszProgressTitle), "lpszProgressTitle" },
	{ 0, NULL }
};

TEmethod tesSIZE[] =
{
	{ (VT_I4 << TE_VT) + offsetof(SIZE, cx), "cx" },
	{ (VT_I4 << TE_VT) + offsetof(SIZE, cy), "cy" },
	{ 0, NULL }
};

TEmethod tesTCHITTESTINFO[] =
{
	{ (VT_CY << TE_VT) + offsetof(TCHITTESTINFO, pt), "pt" },
	{ (VT_I4 << TE_VT) + offsetof(TCHITTESTINFO, flags), "flags" },
	{ 0, NULL }
};

TEmethod tesTCITEM[] =
{
	{ (VT_I4 << TE_VT) + offsetof(TCITEM, mask), "mask" },
	{ (VT_I4 << TE_VT) + offsetof(TCITEM, dwState), "dwState" },
	{ (VT_I4 << TE_VT) + offsetof(TCITEM, dwStateMask), "dwStateMask" },
	{ (VT_BSTR << TE_VT) + offsetof(TCITEM, pszText), "pszText" },
	{ (VT_I4 << TE_VT) + offsetof(TCITEM, cchTextMax), "cchTextMax" },
	{ (VT_I4 << TE_VT) + offsetof(TCITEM, iImage), "iImage" },
	{ (VT_PTR << TE_VT) + offsetof(TCITEM, lParam), "lParam" },
	{ 0, NULL }
};

TEmethod tesTOOLINFO[] =
{
	{ (VT_I4 << TE_VT) + offsetof(TOOLINFO, cbSize), "cbSize" },
	{ (VT_I4 << TE_VT) + offsetof(TOOLINFO, uFlags), "uFlags" },
	{ (VT_PTR << TE_VT) + offsetof(TOOLINFO, hwnd), "hwnd" },
	{ (VT_PTR << TE_VT) + offsetof(TOOLINFO, uId), "uId" },
	{ (VT_CARRAY << TE_VT) + offsetof(TOOLINFO, rect), "rect" },
	{ (VT_PTR << TE_VT) + offsetof(TOOLINFO, hinst), "hinst" },
	{ (VT_BSTR << TE_VT) + offsetof(TOOLINFO, uId), "lpszText" },
	{ (VT_PTR << TE_VT) + offsetof(TOOLINFO, uId), "lParam" },
	{ (VT_PTR << TE_VT) + offsetof(TOOLINFO, uId), "lpReserved" },
	{ 0, NULL }
};

TEmethod tesTVHITTESTINFO[] =
{
	{ (VT_CY << TE_VT) + offsetof(TVHITTESTINFO, pt), "pt" },
	{ (VT_I4 << TE_VT) + offsetof(TVHITTESTINFO, flags), "flags" },
	{ (VT_PTR << TE_VT) + offsetof(TVHITTESTINFO, hItem), "hItem" },
	{ 0, NULL }
};

TEmethod tesTVITEM[] =
{
	{ (VT_I4 << TE_VT) + offsetof(TVITEM, mask), "mask" },
	{ (VT_PTR << TE_VT) + offsetof(TVITEM, hItem), "hItem" },
	{ (VT_I4 << TE_VT) + offsetof(TVITEM, state), "state" },
	{ (VT_I4 << TE_VT) + offsetof(TVITEM, stateMask), "stateMask" },
	{ (VT_BSTR << TE_VT) + offsetof(TVITEM, pszText), "pszText" },
	{ (VT_I4 << TE_VT) + offsetof(TVITEM, cchTextMax), "cchTextMax" },
	{ (VT_I4 << TE_VT) + offsetof(TVITEM, iImage), "iImage" },
	{ (VT_I4 << TE_VT) + offsetof(TVITEM, iSelectedImage), "iSelectedImage" },
	{ (VT_I4 << TE_VT) + offsetof(TVITEM, cChildren), "cChildren" },
	{ (VT_PTR << TE_VT) + offsetof(TVITEM, lParam), "lParam" },
	{ 0, NULL }
};

TEmethod tesWIN32_FIND_DATA[] =
{
	{ (VT_I4 << TE_VT) + offsetof(WIN32_FIND_DATA, dwFileAttributes), "dwFileAttributes" },
	{ (VT_FILETIME << TE_VT) + offsetof(WIN32_FIND_DATA, ftCreationTime), "ftCreationTime" },
	{ (VT_FILETIME << TE_VT) + offsetof(WIN32_FIND_DATA, ftLastAccessTime), "ftLastAccessTime" },
	{ (VT_FILETIME << TE_VT) + offsetof(WIN32_FIND_DATA, ftLastWriteTime), "ftLastWriteTime" },
	{ (VT_I4 << TE_VT) + offsetof(WIN32_FIND_DATA, nFileSizeHigh), "nFileSizeHigh" },
	{ (VT_I4 << TE_VT) + offsetof(WIN32_FIND_DATA, nFileSizeLow), "nFileSizeLow" },
	{ (VT_I4 << TE_VT) + offsetof(WIN32_FIND_DATA, dwReserved0), "dwReserved0" },
	{ (VT_I4 << TE_VT) + offsetof(WIN32_FIND_DATA, dwReserved1), "dwReserved1" },
	{ (VT_LPWSTR << TE_VT) + offsetof(WIN32_FIND_DATA, cFileName), "cFileName" },
	{ (VT_LPWSTR << TE_VT) + offsetof(WIN32_FIND_DATA, cAlternateFileName), "cAlternateFileName" },
	{ 0, NULL }
};

TEmethod tesDRAWITEMSTRUCT[] =
{
	{ (VT_I4 << TE_VT) + offsetof(DRAWITEMSTRUCT, CtlType), "CtlType" },
	{ (VT_I4 << TE_VT) + offsetof(DRAWITEMSTRUCT, CtlID), "CtlID" },
	{ (VT_I4 << TE_VT) + offsetof(DRAWITEMSTRUCT, itemID), "itemID" },
	{ (VT_I4 << TE_VT) + offsetof(DRAWITEMSTRUCT, itemAction), "itemAction" },
	{ (VT_I4 << TE_VT) + offsetof(DRAWITEMSTRUCT, itemState), "itemState" },
	{ (VT_PTR << TE_VT) + offsetof(DRAWITEMSTRUCT, hwndItem), "hwndItem" },
	{ (VT_PTR << TE_VT) + offsetof(DRAWITEMSTRUCT, hDC), "hDC" },
	{ (VT_CARRAY << TE_VT) + offsetof(DRAWITEMSTRUCT, rcItem), "rcItem" },
	{ (VT_PTR << TE_VT) + offsetof(DRAWITEMSTRUCT, itemData), "itemData" },
	{ 0, NULL }
};

TEmethod tesMEASUREITEMSTRUCT[] =
{
	{ (VT_I4 << TE_VT) + offsetof(MEASUREITEMSTRUCT, CtlType), "CtlType" },
	{ (VT_I4 << TE_VT) + offsetof(MEASUREITEMSTRUCT, CtlID), "CtlID" },
	{ (VT_I4 << TE_VT) + offsetof(MEASUREITEMSTRUCT, itemID), "itemID" },
	{ (VT_I4 << TE_VT) + offsetof(MEASUREITEMSTRUCT, itemWidth), "itemWidth" },
	{ (VT_I4 << TE_VT) + offsetof(MEASUREITEMSTRUCT, itemHeight), "itemHeight" },
	{ (VT_PTR << TE_VT) + offsetof(MEASUREITEMSTRUCT, itemData), "itemData" },
	{ 0, NULL }
};

TEmethod tesMENUINFO[] =
{
	{ (VT_I4 << TE_VT) + offsetof(MENUINFO, cbSize), "cbSize" },
	{ (VT_I4 << TE_VT) + offsetof(MENUINFO, fMask), "fMask" },
	{ (VT_I4 << TE_VT) + offsetof(MENUINFO, dwStyle), "dwStyle" },
	{ (VT_I4 << TE_VT) + offsetof(MENUINFO, cyMax), "cyMax" },
	{ (VT_PTR << TE_VT) + offsetof(MENUINFO, hbrBack), "hbrBack" },
	{ (VT_I4 << TE_VT) + offsetof(MENUINFO, dwContextHelpID), "dwContextHelpID" },
	{ (VT_PTR << TE_VT) + offsetof(MENUINFO, dwMenuData), "dwMenuData" },
	{ 0, NULL }
};

TEmethod tesGUITHREADINFO[] =
{
	{ (VT_I4 << TE_VT) + offsetof(GUITHREADINFO, cbSize), "cbSize" },
	{ (VT_I4 << TE_VT) + offsetof(GUITHREADINFO, flags), "flags" },
	{ (VT_PTR << TE_VT) + offsetof(GUITHREADINFO, hwndActive), "hwndActive" },
	{ (VT_PTR << TE_VT) + offsetof(GUITHREADINFO, hwndFocus), "hwndFocus" },
	{ (VT_PTR << TE_VT) + offsetof(GUITHREADINFO, hwndCapture), "hwndCapture" },
	{ (VT_PTR << TE_VT) + offsetof(GUITHREADINFO, hwndMenuOwner), "hwndMenuOwner" },
	{ (VT_PTR << TE_VT) + offsetof(GUITHREADINFO, hwndMoveSize), "hwndMoveSize" },
	{ (VT_PTR << TE_VT) + offsetof(GUITHREADINFO, hwndCaret), "hwndCaret" },
	{ (VT_CARRAY << TE_VT) + offsetof(GUITHREADINFO, hwndCaret), "rcCaret" },
	{ 0, NULL }
};

TEStruct pTEStructs[] = {
	{ sizeof(BITMAP), "BITMAP", tesBITMAP },
	{ sizeof(BSTR), "BSTR", tesNULL },
	{ sizeof(BYTE), "BYTE", tesNULL },
	{ sizeof(char), "char", tesNULL },
	{ sizeof(CHOOSECOLOR), "CHOOSECOLOR", tesCHOOSECOLOR },
	{ sizeof(CHOOSEFONT), "CHOOSEFONT", tesCHOOSEFONT },
	{ sizeof(COPYDATASTRUCT), "COPYDATASTRUCT", tesCOPYDATASTRUCT },
	{ sizeof(DIBSECTION), "DIBSECTION", tesDIBSECTION },
	{ sizeof(DRAWITEMSTRUCT), "DRAWITEMSTRUCT", tesDRAWITEMSTRUCT },
	{ sizeof(DWORD), "DWORD", tesNULL },
	{ sizeof(EXCEPINFO), "EXCEPINFO", tesEXCEPINFO },
	{ sizeof(FINDREPLACE), "FINDREPLACE", tesFINDREPLACE },
	{ sizeof(FOLDERSETTINGS), "FOLDERSETTINGS", tesFOLDERSETTINGS },
	{ sizeof(GUID), "GUID", tesNULL },
	{ sizeof(GUITHREADINFO), "GUITHREADINFO", tesGUITHREADINFO },
	{ sizeof(HANDLE), "HANDLE", tesNULL },
	{ sizeof(HDITEM), "HDITEM", tesHDITEM },
	{ sizeof(ICONINFO), "ICONINFO", tesICONINFO },
	{ sizeof(ICONMETRICS), "ICONMETRICS", tesICONMETRICS },
	{ sizeof(int), "int", tesNULL },
	{ sizeof(KEYBDINPUT) + sizeof(DWORD), "KEYBDINPUT", tesKEYBDINPUT },
	{ 256, "KEYSTATE", tesNULL },
	{ sizeof(LOGFONT), "LOGFONT", tesLOGFONT },
	{ sizeof(LPWSTR), "LPWSTR", tesNULL },
	{ sizeof(LVBKIMAGE), "LVBKIMAGE", tesLVBKIMAGE },
	{ sizeof(LVFINDINFO), "LVFINDINFO", tesLVFINDINFO },
	{ sizeof(LVGROUP), "LVGROUP", tesLVGROUP },
	{ sizeof(LVHITTESTINFO), "LVHITTESTINFO", tesLVHITTESTINFO },
	{ sizeof(LVITEM), "LVITEM", tesLVITEM },
	{ sizeof(MEASUREITEMSTRUCT), "MEASUREITEMSTRUCT", tesMEASUREITEMSTRUCT },
	{ sizeof(MENUINFO), "MENUINFO", tesMENUINFO },
	{ sizeof(MENUITEMINFO), "MENUITEMINFO", tesMENUITEMINFO },
	{ sizeof(MONITORINFOEX), "MONITORINFOEX", tesMONITORINFOEX },
	{ sizeof(MOUSEINPUT) + sizeof(DWORD), "MOUSEINPUT", tesMOUSEINPUT },
	{ sizeof(MSG), "MSG", tesMSG },
	{ sizeof(NMCUSTOMDRAW), "NMCUSTOMDRAW", tesNMCUSTOMDRAW },
	{ sizeof(NMLVCUSTOMDRAW), "NMLVCUSTOMDRAW", tesNMLVCUSTOMDRAW },
	{ sizeof(NMTVCUSTOMDRAW), "NMTVCUSTOMDRAW", tesNMTVCUSTOMDRAW },
	{ sizeof(NMHDR), "NMHDR", tesNMHDR },
	{ sizeof(NONCLIENTMETRICS), "NONCLIENTMETRICS", tesNONCLIENTMETRICS },
	{ sizeof(NOTIFYICONDATA), "NOTIFYICONDATA", tesNOTIFYICONDATA },
	{ sizeof(OSVERSIONINFO), "OSVERSIONINFO", tesOSVERSIONINFOEX },
	{ sizeof(OSVERSIONINFOEX), "OSVERSIONINFOEX", tesOSVERSIONINFOEX },
	{ sizeof(PAINTSTRUCT), "PAINTSTRUCT", tesPAINTSTRUCT },
	{ sizeof(POINT), "POINT", tesPOINT },
	{ sizeof(RECT), "RECT", tesRECT },
	{ sizeof(SHELLEXECUTEINFO), "SHELLEXECUTEINFO", tesSHELLEXECUTEINFO },
	{ sizeof(SHFILEINFO), "SHFILEINFO", tesSHFILEINFO },
	{ sizeof(SHFILEOPSTRUCT), "SHFILEOPSTRUCT", tesSHFILEOPSTRUCT },
	{ sizeof(SIZE), "SIZE", tesSIZE },
	{ sizeof(TCHITTESTINFO), "TCHITTESTINFO", tesTCHITTESTINFO },
	{ sizeof(TCITEM), "TCITEM", tesTCITEM },
	{ sizeof(TOOLINFO), "TOOLINFO", tesTOOLINFO },
	{ sizeof(TVHITTESTINFO), "TVHITTESTINFO", tesTVHITTESTINFO },
	{ sizeof(TVITEM), "TVITEM", tesTVITEM },
	{ sizeof(VARIANT), "VARIANT", tesNULL },
	{ sizeof(WCHAR), "WCHAR", tesNULL },
	{ sizeof(WIN32_FIND_DATA), "WIN32_FIND_DATA", tesWIN32_FIND_DATA },
	{ sizeof(WORD), "WORD", tesNULL },
};

TEmethod methodMem2[] = {
	{ VT_I4  << TE_VT, "int" },
	{ VT_UI4 << TE_VT, "DWORD" },
	{ VT_UI1 << TE_VT, "BYTE" },
	{ VT_UI2 << TE_VT, "WORD" },
	{ VT_UI2 << TE_VT, "WCHAR" },
	{ VT_PTR << TE_VT, "HANDLE" },
	{ VT_PTR << TE_VT, "LPWSTR" },
	{ 0, NULL }
};

TEmethod methodTE[] = {
	{ 1001, "Data" },
	{ 1002, "hwnd" },
	{ 1004, "About" },
	{ TE_METHOD + 1005, "Ctrl" },
	{ TE_METHOD + 1006, "Ctrls" },
	{ 1007, "Version" },
	{ TE_METHOD + 1008, "ClearEvents" },
	{ TE_METHOD + 1009, "Reload" },
	{ TE_METHOD + 1010, "CreateObject" },
	{ TE_METHOD + 1020, "GetObject" },
	{ 1030, "WindowsAPI" },
	{ 1031, "WindowsAPI0" },
	{ 1131, "CommonDialog" },
	{ 1132, "WICBitmap" },
	{ 1132, "GdiplusBitmap" },
	{ 1137, "ProgressDialog" },
	{ 1138, "DateTimeFormat" },
	{ 1140, "Background" },
	{ TE_METHOD + 1133, "FolderItems" },
	{ TE_METHOD + 1134, "Object" },
	{ TE_METHOD + 1135, "Array" },
	{ TE_METHOD + 1136, "Collection" },
	{ TE_METHOD + 1050, "CreateCtrl" },
	{ TE_METHOD + 1040, "CtrlFromPoint" },
	{ TE_METHOD + 1060, "MainMenu" },
	{ TE_METHOD + 1070, "CtrlFromWindow" },
	{ TE_METHOD + 1080, "LockUpdate" },
	{ TE_METHOD + 1090, "UnlockUpdate" },
	{ TE_METHOD + 1100, "HookDragDrop" },//Deprecated
#ifdef _USE_TESTOBJECT
	{ 1200, "TestObj" },
#endif
	{ TE_OFFSET + TE_Type   , "Type" },
	{ TE_OFFSET + TE_Left   , "offsetLeft" },
	{ TE_OFFSET + TE_Top    , "offsetTop" },
	{ TE_OFFSET + TE_Right  , "offsetRight" },
	{ TE_OFFSET + TE_Bottom , "offsetBottom" },
	{ TE_OFFSET + TE_Tab, "Tab" },
	{ TE_OFFSET + TE_CmdShow, "CmdShow" },
	{ TE_OFFSET + TE_Layout, "Layout" },
	{ TE_OFFSET + TE_NetworkTimeout, "NetworkTimeout" },
	{ TE_OFFSET + TE_SizeFormat, "SizeFormat" },
	{ START_OnFunc + TE_Labels, "Labels" },
	{ START_OnFunc + TE_ColumnsReplace, "ColumnsReplace" },
	{ START_OnFunc + TE_OnBeforeNavigate, "OnBeforeNavigate" },
	{ START_OnFunc + TE_OnViewCreated, "OnViewCreated" },
	{ START_OnFunc + TE_OnKeyMessage, "OnKeyMessage" },
	{ START_OnFunc + TE_OnMouseMessage, "OnMouseMessage" },
	{ START_OnFunc + TE_OnCreate, "OnCreate" },
	{ START_OnFunc + TE_OnDefaultCommand, "OnDefaultCommand" },
	{ START_OnFunc + TE_OnItemClick, "OnItemClick" },
	{ START_OnFunc + TE_OnGetPaneState, "OnGetPaneState" },
	{ START_OnFunc + TE_OnMenuMessage, "OnMenuMessage" },
	{ START_OnFunc + TE_OnSystemMessage, "OnSystemMessage" },
	{ START_OnFunc + TE_OnShowContextMenu, "OnShowContextMenu" },
	{ START_OnFunc + TE_OnSelectionChanged, "OnSelectionChanged" },
	{ START_OnFunc + TE_OnClose, "OnClose" },
	{ START_OnFunc + TE_OnDragEnter, "OnDragEnter" },
	{ START_OnFunc + TE_OnDragOver, "OnDragOver" },
	{ START_OnFunc + TE_OnDrop, "OnDrop" },
	{ START_OnFunc + TE_OnDragLeave, "OnDragLeave" },
	{ START_OnFunc + TE_OnAppMessage, "OnAppMessage" },
	{ START_OnFunc + TE_OnStatusText, "OnStatusText" },
	{ START_OnFunc + TE_OnToolTip, "OnToolTip" },
	{ START_OnFunc + TE_OnNewWindow, "OnNewWindow" },
	{ START_OnFunc + TE_OnWindowRegistered, "OnWindowRegistered" },
	{ START_OnFunc + TE_OnSelectionChanging, "OnSelectionChanging" },
	{ START_OnFunc + TE_OnClipboardText, "OnClipboardText" },
	{ START_OnFunc + TE_OnCommand, "OnCommand" },
	{ START_OnFunc + TE_OnInvokeCommand, "OnInvokeCommand" },
	{ START_OnFunc + TE_OnArrange, "OnArrange" },
	{ START_OnFunc + TE_OnHitTest, "OnHitTest" },
	{ START_OnFunc + TE_OnVisibleChanged, "OnVisibleChanged" },
	{ START_OnFunc + TE_OnTranslatePath, "OnTranslatePath" },
	{ START_OnFunc + TE_OnNavigateComplete, "OnNavigateComplete" },
	{ START_OnFunc + TE_OnILGetParent, "OnILGetParent" },
	{ START_OnFunc + TE_OnViewModeChanged, "OnViewModeChanged" },
	{ START_OnFunc + TE_OnColumnsChanged, "OnColumnsChanged" },
	{ START_OnFunc + TE_OnItemPrePaint, "OnItemPrePaint" },
	{ START_OnFunc + TE_OnColumnClick, "OnColumnClick" },
	{ START_OnFunc + TE_OnBeginDrag, "OnBeginDrag" },
	{ START_OnFunc + TE_OnBeforeGetData, "OnBeforeGetData" },
	{ START_OnFunc + TE_OnIconSizeChanged, "OnIconSizeChanged" },
	{ START_OnFunc + TE_OnFilterChanged, "OnFilterChanged" },
	{ START_OnFunc + TE_OnBeginLabelEdit, "OnBeginLabelEdit" },
	{ START_OnFunc + TE_OnEndLabelEdit, "OnEndLabelEdit" },
	{ START_OnFunc + TE_OnReplacePath, "OnReplacePath" },
	{ START_OnFunc + TE_OnBeginNavigate, "OnBeginNavigate" },
	{ START_OnFunc + TE_OnSort, "OnSort" },
	{ START_OnFunc + TE_OnFromFile, "OnFromFile" },
	{ START_OnFunc + TE_OnFromStream, "OnFromStream" },
	{ START_OnFunc + TE_OnEndThread, "OnEndThread" },
	{ START_OnFunc + TE_OnItemPostPaint, "OnItemPostPaint" },
	{ START_OnFunc + TE_OnHandleIcon, "OnHandleIcon" },
	{ START_OnFunc + TE_OnSorting, "OnSorting" },
	{ START_OnFunc + TE_OnSetName, "OnSetName" },
	{ 0, NULL }
};

TEmethod methodSB[] = {
	{ 0x10000001, "Data" },
	{ 0x10000002, "hwnd" },
	{ 0x10000003, "Type" },
	{ 0x10000004, "Navigate" },
	{ 0x10000007, "Navigate2" },
	{ 0x10000008, "Index" },
	{ 0x10000009, "FolderFlags" },
	{ 0x1000000B, "History" },
	{ 0x10000010, "CurrentViewMode" },
	{ 0x10000011, "IconSize" },
	{ 0x10000012, "Options" },
	{ 0x10000013, "SizeFormat" }, //Deprecated
	{ 0x10000014, "NameFormat" }, //Deprecated
	{ 0x10000016, "ViewFlags" },
	{ 0x10000017, "Id" },
	{ 0x10000018, "FilterView" },
	{ 0x10000020, "FolderItem" },
	{ 0x10000021, "TreeView" },
	{ 0x10000024, "Parent" },
	{ 0x10000031, "Close" },
	{ 0x10000032, "Title" },
	{ 0x10000033, "Suspend" },
	{ 0x10000040, "Items" },
	{ 0x10000041, "SelectedItems" },
	{ 0x10000050, "ShellFolderView" },
	{ 0x10000058, "Droptarget" },
	{ 0x10000059, "Columns" },
//	{ 0x1000005A, "Searches" },
	{ 0x1000005B, "MapColumnToSCID" },
	{ 0x10000102, "hwndList" },
	{ 0x10000103, "hwndView" },
	{ 0x10000104, "SortColumn" },
	{ 0x10000105, "GroupBy" },
	{ 0x10000106, "Focus" },
	{ 0x10000107, "HitTest" },
	{ 0x10000108, "hwndAlt" },
	{ 0x10000110, "ItemCount" },
	{ 0x10000111, "Item" },
	{ 0x10000206, "Refresh" },
	{ 0x10000207, "ViewMenu" },
	{ 0x10000208, "TranslateAccelerator" },
	{ 0x10000209, "GetItemPosition" },
	{ 0x1000020A, "SelectAndPositionItem" },
	{ 0x10000280, "SelectItem" },
	{ 0x10000281, "FocusedItem" },
	{ 0x10000282, "GetFocusedItem" },
	{ 0x10000283, "GetItemRect" },
	{ 0x10000300, "Notify" },
	{ 0x10000400, "NavigateComplete" },
	{ 0x10000501, "AddItem" },
	{ 0x10000502, "RemoveItem" },
	{ 0x10000503, "AddItems" },
	{ 0x10000504, "RemoveAll" },
	{ 0x10000505, "SessionId" },
	{ START_OnFunc + SB_TotalFileSize, "TotalFileSize" },
	{ START_OnFunc + SB_ColumnsReplace, "ColumnsReplace" },
	{ START_OnFunc + SB_OnIncludeObject, "OnIncludeObject" },
	{ START_OnFunc + SB_AltSelectedItems, "AltSelectedItems" },
	{ START_OnFunc + SB_VirtualName, "VirtualName" }, //Deprecated
	{ 0, NULL }
};

TEmethod methodWB[] = {
	{ 0x10000001, "Data" },
	{ 0x10000002, "hwnd" },
	{ 0x10000003, "Type" },
	{ 0x10000004, "TranslateAccelerator" },
	{ 0x10000005, "Application" },
	{ 0x10000006, "Document" },
	{ 0x10000007, "Window" },
	{ 0x10000008, "Focus" },
//	{ 0x10000009, "Close" },
	{ 0, NULL }
};

TEmethod methodTC[] = {
	{ 1, "Data" },
	{ 2, "hwnd" },
	{ 3, "Type" },
	{ 6, "HitTest" },
	{ 7, "Move" },
	{ 8, "Selected" },
	{ 9, "Close" },
	{ 10, "SelectedIndex" },
	{ 11, "Visible" },
	{ 12, "Id" },
	{ 13, "LockUpdate" },
	{ 14, "UnlockUpdate" },
	{ DISPID_NEWENUM, "_NewEnum" },
	{ DISPID_TE_ITEM, "Item" },
	{ DISPID_TE_COUNT, "Count" },
	{ DISPID_TE_COUNT, "length" },
	{ TE_OFFSET + TE_Left, "Left" },
	{ TE_OFFSET + TE_Top, "Top" },
	{ TE_OFFSET + TE_Width, "Width" },
	{ TE_OFFSET + TE_Height, "Height" },
	{ TE_OFFSET + TE_Flags, "Style" },
	{ TE_OFFSET + TC_Align, "Align" },
	{ TE_OFFSET + TC_TabWidth, "TabWidth" },
	{ TE_OFFSET + TC_TabHeight, "TabHeight" },
	{ 0, NULL }
};

TEmethod methodFIs[] = {
	{ 2, "Application" },
	{ 3, "Parent" },
	{ 8, "AddItem" },
	{ 9, "hDrop" },
	{ 10, "GetData" },
	{ 11, "SetData" },
	{ DISPID_NEWENUM, "_NewEnum" },
	{ DISPID_TE_ITEM, "Item" },
	{ DISPID_TE_COUNT, "Count" },
	{ DISPID_TE_COUNT, "length" },
	{ DISPID_TE_INDEX, "Index" },
//	{ 0x10000001, "lEvent" },
	{ 0x10000001, "dwEffect" },
	{ 0x10000002, "pdwEffect" },
	{ 0x10000003, "Data" },
	{ 0, NULL }
};

TEmethod methodDT[] = {
	{ 1, "DragEnter" },
	{ 2, "DragOver" },
	{ 3, "Drop" },
	{ 4, "DragLeave" },
	{ 5, "Type" },
	{ 6, "FolderItem" },
	{ 0, NULL }
};

TEmethod methodTV[] = {
	{ 0x10000001, "Data" },
	{ 0x10000002, "Type" },
	{ 0x10000003, "hwnd" },
	{ 0x10000004, "Close" },
	{ 0x10000005, "hwndTree" },
	{ 0x10000007, "FolderView" },
	{ 0x10000008, "Align" },
	{ 0x10000009, "Visible" },
	{ 0x10000106, "Focus" },
	{ 0x10000107, "HitTest" },
	{ 0x10000283, "GetItemRect" },
	{ 0x10000300, "Notify" },
	{ TE_OFFSET + SB_TreeWidth, "Width" },
	{ TE_OFFSET + SB_TreeFlags, "Style" },
	{ TE_OFFSET + SB_EnumFlags, "EnumFlags" },
	{ TE_OFFSET + SB_RootStyle, "RootStyle" },
	{ 0x20000000, "SelectedItem" },
	{ 0x20000001, "SelectedItems" },
	{ 0x20000002, "Root" },
	{ 0x20000003, "SetRoot" },
	{ 0x20000004, "Expand" },
	{ 0x20000005, "Columns" },
	{ 0x20000006, "CountViewTypes" },
	{ 0x20000007, "Depth" },
	{ 0x20000008, "EnumOptions" },
	{ 0x20000009, "Export" },
	{ 0x2000000a, "Flags" },
	{ 0x2000000b, "Import" },
	{ 0x2000000c, "Mode" },
	{ 0x2000000d, "ResetSort" },
	{ 0x2000000e, "SetViewType" },
	{ 0x2000000f, "Synchronize" },
	{ 0x20000010, "TVFlags" },
	{ 0, NULL }
};

TEmethod methodFI[] = {
	{ 1, "Name" },
	{ 2, "Path" },
	{ 3, "Alt" },
//	{ 4, "FocusedItem" },
	{ 5, "Unavailable" },
	{ 6, "Enum" },
	{ 9, "_BLOB" },	//To be necessary
	{ 9, "FolderItem" },
	{ 0, NULL }
};

TEmethod methodMem[] = {
	{ 1, "P" },
	{ 4, "Read" },
	{ 5, "Write" },
	{ 6, "Size" },
	{ 7, "Free" },
	{ 8, "Clone" },
	{ 9, "_BLOB" },
	{ DISPID_NEWENUM, "_NewEnum" },
	{ DISPID_TE_ITEM, "Item" },
	{ DISPID_TE_COUNT, "Count" },
	{ DISPID_TE_COUNT, "length" },
	{ 0, NULL }
};

TEmethod methodCM[] = {
	{ 1, "QueryContextMenu" },
	{ 2, "InvokeCommand" },
	{ 3, "Items" },
	{ 4, "GetCommandString" },
	{ 5, "FolderView" },
	{ 6, "HandleMenuMsg" },
	{ 10, "hmenu" },
	{ 11, "indexMenu" },
	{ 12, "idCmdFirst" },
	{ 13, "idCmdLast" },
	{ 14, "uFlags" },
	{ 0, NULL }
};

TEmethod methodCD[] = {
	{ 40, "ShowOpen" },
	{ 41, "ShowSave" },
//	{ 42, "ShowFolder" },
	{ 10, "FileName" },
	{ 13, "Filter" },
	{ 20, "InitDir" },
	{ 21, "DefExt" },
	{ 22, "Title" },
	{ 30, "MaxFileSize" },
	{ 31, "Flags" },
	{ 32, "FilterIndex" },
	{ 31, "FlagsEx" },
	{ 0, NULL }
};

TEmethod methodGB[] = {
	{ 1, "FromHBITMAP" },
	{ 2, "FromHICON" },
	{ 3, "FromResource" },
	{ 4, "FromFile" },
	{ 5, "FromStream" },
	{ 6, "FromArchive" },
	{ 7, "FromItem" },
	{ 8, "FromClipboard" },
	{ 99, "Free" },

	{ 100, "Save" },
	{ 101, "Base64" },
	{ 102, "DataURI" },
	{ 110, "GetWidth" },
	{ 111, "GetHeight" },
	{ 112, "GetPixel" },
	{ 113, "SetPixel" },
	{ 114, "GetPixelFormat" },
	{ 120, "GetThumbnailImage" },
	{ 130, "RotateFlip" },
	{ 140, "GetFrameCount" },
	{ 150, "Frame" },
	{ 160, "GetMetadata" },
	{ 161, "GetFrameMetadata" },

	{ 210, "GetHBITMAP" },
	{ 211, "GetHICON" },
	{ 212, "DrawEx" },

	{ 900, "GetCodecInfo" },
	{ 0, NULL }
};

TEmethod methodPD[] = {
	{ 0x60010001, "HasUserCancelled" },
	{ 0x60010002, "SetCancelMsg" },
	{ 0x60010003, "SetLine" },
	{ 0x60010004, "SetProgress" },
	{ 0x60010005, "SetTitle" },
	{ 0x60010006, "StartProgressDialog" },
	{ 0x60010007, "StopProgressDialog" },
	{ 0x60010008, "Timer" },
	{ 0x60010009, "SetAnimation" },
	{ 0, NULL }
};

TEmethod methodCO[] = {
	{ 0x60010001, "Free" },
	{ 0, NULL }
};

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance, LPWSTR szClassName, WNDPROC lpfnWndProc);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
VOID ArrangeWindow();

BOOL teGetIDListFromObject(IUnknown *punk, LPITEMIDLIST *ppidl);
VOID CALLBACK teTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
VOID CALLBACK teTimerProcParse(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
static void threadParseDisplayName(void *args);
LPITEMIDLIST teILCreateFromPath(LPWSTR pszPath);
int teGetModuleFileName(HMODULE hModule, BSTR *pbsPath);
BOOL GetVarArrayFromIDList(VARIANT *pv, LPITEMIDLIST pidl);
HRESULT teGetDisplayNameFromIDList(BSTR *pbs, LPITEMIDLIST pidl, SHGDNF uFlags);

//Unit

HRESULT teGetWindow(PVOID pObj, HWND *phwnd)
{
	IOleWindow *pOleWindow;
	HRESULT hr = static_cast<IUnknown *>(pObj)->QueryInterface(IID_PPV_ARGS(&pOleWindow));
	if SUCCEEDED(hr) {
		hr = pOleWindow->GetWindow(phwnd);
		pOleWindow->Release();
	}
	return hr;
}

VOID teTranslateAccelerator(IDispatch *pdisp, MSG *pMsg, HRESULT *phr)
{
	if (pMsg->message == WM_KEYDOWN && GetKeyState(VK_CONTROL) < 0 && StrChr(L"LNOP\x6b\x6d\xbb\xbd", (WCHAR)pMsg->wParam)) {
		return;
	}
	IWebBrowser2 *pWB = NULL;
	if SUCCEEDED(pdisp->QueryInterface(IID_PPV_ARGS(&pWB))) {
		IOleInPlaceActiveObject *pActiveObject = NULL;
		if SUCCEEDED(pWB->QueryInterface(IID_PPV_ARGS(&pActiveObject))) {
			if (pActiveObject->TranslateAcceleratorW(pMsg) == S_OK) {
				*phr = S_OK;
			}
			pActiveObject->Release();
		}
		pWB->Release();
	}
}

VOID PushFolderSizeList(TEFS* pFS)
{
	EnterCriticalSection(&g_csFolderSize);
	try {
		g_pFolderSize.push_front(pFS);
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"PushFolderSizeList";
#endif
	}
	LeaveCriticalSection(&g_csFolderSize);
}

BOOL PopFolderSizeList(TEFS** ppFS)
{
	BOOL bResult = FALSE;
	EnterCriticalSection(&g_csFolderSize);
	try {
		bResult = !g_pFolderSize.empty();
		if (bResult) {
			*ppFS = *g_pFolderSize.begin();
			g_pFolderSize.pop_front();
		}
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"PopFolderSizeList";
#endif
	}
	LeaveCriticalSection(&g_csFolderSize);
	return bResult;
}

BOOL teIsSameSort(IFolderView2 *pFV2, SORTCOLUMN *pCol1, int nCount1)
{
	int i;
	SORTCOLUMN pCol2[8];
	BOOL bResult = SUCCEEDED(pFV2->GetSortColumnCount(&i)) && nCount1 == i && i <= _countof(pCol2) && SUCCEEDED(pFV2->GetSortColumns(pCol2, i));
	while (bResult && i-- > 0) {
		bResult = (pCol1[i].direction == pCol2[i].direction && IsEqualPropertyKey(pCol1[i].propkey, pCol2[i].propkey));
	}
	return bResult;
}

HRESULT teGetCodecInfo(IWICBitmapCodecInfo *pCodecInfo, int nMode, UINT cch, WCHAR *wz, UINT *pcchActual)
{
	switch (nMode) {
		case 0:
			return pCodecInfo->GetFileExtensions(cch, wz, pcchActual);
		case 1:
			return pCodecInfo->GetMimeTypes(cch, wz, pcchActual);
		case 2:
			return pCodecInfo->GetFriendlyName(cch, wz, pcchActual);
		case 3:
			return pCodecInfo->GetAuthor(cch, wz, pcchActual);
		case 4:
			return pCodecInfo->GetSpecVersion(cch, wz, pcchActual);
	}
	return E_NOTIMPL;
}

VOID teQueryFolderItem(FolderItem **ppfi, CteFolderItem **ppid)
{
	if FAILED((*ppfi)->QueryInterface(g_ClsIdFI, (LPVOID *)ppid)) {
		*ppid = new CteFolderItem(NULL);
		LPITEMIDLIST pidl;
		if (teGetIDListFromObject(*ppfi, &pidl)) {
			if SUCCEEDED((*ppid)->Initialize(pidl)) {
				(*ppfi)->Release();
				(*ppid)->QueryInterface(IID_PPV_ARGS(ppfi));
			}
		}
	}
}

BOOL teGetHTMLWindow(IWebBrowser2 *pWB2, REFIID riid, void **ppvObject)
{
	*ppvObject = NULL;
	IDispatch *pdisp;
	if (pWB2->get_Document(&pdisp) == S_OK) {
		IHTMLDocument2 *pdoc;
		if SUCCEEDED(pdisp->QueryInterface(IID_PPV_ARGS(&pdoc))) {
			IHTMLWindow2 *pwin;
			if SUCCEEDED(pdoc->get_parentWindow(&pwin)) {
				pwin->QueryInterface(riid, ppvObject);
			}
			pdoc->Release();
		}
		pdisp->Release();
	}
	return *ppvObject != NULL;
}

BSTR teMultiByteToWideChar(UINT CodePage, LPCSTR lpA, int nLenA)
{
	int nLenW = MultiByteToWideChar(CodePage, 0, lpA, nLenA, NULL, NULL);
	BSTR bs = ::SysAllocStringLen(NULL, nLenW);
	MultiByteToWideChar(CodePage, 0, lpA, nLenA, bs, nLenW);
	return bs;
}

BSTR teSysAllocStringByteLen(LPCSTR psz, UINT len, UINT org)
{
	if (!len) {
		return NULL;
	}
	BSTR bs = ::SysAllocStringByteLen(NULL, len);
	if (psz && bs && org) {
		::CopyMemory(bs, psz, org < len ? org : len);
	}
	return bs;
}

BSTR teLoadFromFile(BSTR bsFile)
{
	BSTR bsResult = NULL;
	HANDLE hFile = CreateFile(bsFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		DWORD dwFileSize, dwFileSize2;
		dwFileSize = GetFileSize(hFile, &dwFileSize2);
		if(dwFileSize != INVALID_FILE_SIZE) {
			BSTR bsUTF8 = ::SysAllocStringByteLen(NULL, dwFileSize);
			if (ReadFile(hFile, bsUTF8, dwFileSize, &dwFileSize2, NULL)) {
				bsResult = teMultiByteToWideChar(CP_UTF8, (LPCSTR)bsUTF8, -1);
			}
			::SysFreeString(bsUTF8);
		}
		CloseHandle(hFile);
	}
	return bsResult;
}

LONGLONG teGetU(LONGLONG ll)
{
	if ((ll & (~MAXINT)) == (~MAXINT)) {
		return ll & MAXUINT;
	}
	return ll & MAXINT64;
}

VOID SafeRelease(PVOID ppObj)
{
	try {
		IUnknown **ppunk = static_cast<IUnknown **>(ppObj);
		if (*ppunk) {
			(*ppunk)->Release();
			*ppunk = NULL;
		}
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"SafeRelease";
#endif
	}
}

VOID teCopyStream(IStream *pSrc, IStream *pDst)
{
	ULARGE_INTEGER uliSrc, uliDst;
	LARGE_INTEGER liOffset;
	liOffset.QuadPart = 0;
	pSrc->Seek(liOffset, STREAM_SEEK_CUR, &uliSrc);
	pSrc->Seek(liOffset, STREAM_SEEK_SET, NULL);
	pDst->Seek(liOffset, STREAM_SEEK_CUR, &uliDst);
	pDst->Seek(liOffset, STREAM_SEEK_SET, NULL);
	ULONG cbRead;
	BYTE pszData[SIZE_BUFF];
	while (SUCCEEDED(pSrc->Read(pszData, SIZE_BUFF, &cbRead)) && cbRead) {
		pDst->Write(pszData, cbRead, NULL);
	}
	pSrc->Seek(*(LARGE_INTEGER *)&uliSrc, STREAM_SEEK_SET, NULL);
	pDst->Seek(*(LARGE_INTEGER *)&uliDst, STREAM_SEEK_SET, NULL);
}

BOOL teShowWindow(HWND hWnd, int nCmdShow)
{
	ANIMATIONINFO ai;
	ai.cbSize = sizeof(ANIMATIONINFO);
	int iMinAnimate = 0;
	if (nCmdShow != SW_SHOWMINIMIZED || IsIconic(hWnd)) {
		if (SystemParametersInfo(SPI_GETANIMATION, sizeof(ai), &ai, 0)) {
			iMinAnimate = ai.iMinAnimate;
		}
		if (iMinAnimate) {
			ai.iMinAnimate = FALSE;
			SystemParametersInfo(SPI_SETANIMATION, sizeof(ai), &ai, 0);
		}
	}
	BOOL bResult = ShowWindow(hWnd, nCmdShow);
	if (iMinAnimate) {
		ai.iMinAnimate = iMinAnimate;
		SystemParametersInfo(SPI_SETANIMATION, sizeof(ai), &ai, 0);
	}
	return bResult;
}

VOID teCommaSize(LPWSTR pszIn, LPWSTR pszOut, UINT cchBuf, int nDigits)
{
	NUMBERFMT nbf;
	WCHAR pszDecimal[4];
	WCHAR pszThousand[4];
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, pszDecimal, _countof(pszDecimal));
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, pszThousand, _countof(pszThousand));

	nbf.NumDigits = nDigits;
	nbf.LeadingZero = 1;
	nbf.Grouping = 3;
	nbf.lpDecimalSep = pszDecimal;
	nbf.lpThousandSep = pszThousand;
	nbf.NegativeOrder = 1;
	GetNumberFormat(LOCALE_USER_DEFAULT, 0, pszIn, &nbf, pszOut, cchBuf);
}

VOID teStrFormatSize(DWORD dwFormat, LONGLONG qdw, LPWSTR pszBuf, UINT cchBuf)
{
	if (dwFormat <= 1) {
		StrFormatKBSize(qdw, pszBuf, cchBuf);
		return;
	}
	if (dwFormat == 2) {
		StrFormatByteSize(qdw, pszBuf, cchBuf);
		return;
	}
	if (dwFormat == 3) {
		WCHAR pszNum[32];
		swprintf_s(pszNum, 32, L"%llu", qdw);
		teCommaSize(pszNum, pszBuf, cchBuf, 0);
		return;
	}
	LPCSTR pszPrefix = "\0KMGTPE\0";
	int i = (dwFormat >> 4) & 7;
	int j = i;
	LONGLONG llPow = 1;
	if (i < 7) {
		while (i-- > 0) {
			llPow *= 1024;
		}
	}
	WCHAR pszNum[32];
	swprintf_s(pszNum, 32, L"%f", double(qdw) / llPow);
	teCommaSize(pszNum, pszBuf, cchBuf, dwFormat & 0xf);
	swprintf_s(&pszBuf[lstrlen(pszBuf)], 4, L" %cB", pszPrefix[j]);
}

BSTR teSysAllocStringLen(const OLECHAR *strIn, UINT uSize)
{
	UINT uOrg = lstrlen(strIn);
	if (strIn && uSize > uOrg) {
		BSTR bs = ::SysAllocStringLen(NULL, uSize);
		lstrcpy(bs, strIn);
		return bs;
	}
	return ::SysAllocStringLen(strIn, uSize);
}

BSTR teSysAllocStringLenEx(const BSTR strIn, UINT uSize)
{
	UINT uOrg = ::SysStringLen(strIn);
	if (uSize > uOrg) {
		BSTR bs = ::SysAllocStringLen(NULL, uSize);
		if (bs) {
			::CopyMemory(bs, strIn, ::SysStringByteLen(strIn) + sizeof(WORD));
		}
		return bs;
	}
	return ::SysAllocStringLen(strIn, uSize);
}

VOID teSysFreeString(BSTR *pbs)
{
	if (*pbs) {
		::SysFreeString(*pbs);
		*pbs = NULL;
	}
}

VOID teSetLong(VARIANT *pv, LONG i)
{
	if (pv) {
		pv->lVal = i;
		pv->vt = VT_I4;
	}
}

VOID teSetBool(VARIANT *pv, BOOL b)
{
	if (pv) {
		pv->boolVal = b ? VARIANT_TRUE : VARIANT_FALSE;
		pv->vt = VT_BOOL;
	}
}

VOID teSetBSTR(VARIANT *pv, BSTR *pbs, int nLen)
{
	if (pv) {
		pv->vt = VT_BSTR;
		if (*pbs) {
			if (nLen < 0) {
				nLen = lstrlen(*pbs);
			}
			if (::SysStringLen(*pbs) == nLen) {
				pv->bstrVal = *pbs;
				*pbs = NULL;
				return;
			}
			pv->bstrVal = teSysAllocStringLenEx(*pbs, nLen);
			teSysFreeString(pbs);
			return;
		}
		pv->bstrVal = NULL;
		return;
	}
	teSysFreeString(pbs);
}

VOID teSetLL(VARIANT *pv, LONGLONG ll)
{
	if (pv) {
		pv->lVal = static_cast<int>(ll);
		if (ll == static_cast<LONGLONG>(pv->lVal)) {
			pv->vt = VT_I4;
			return;
		}
		pv->dblVal = static_cast<DOUBLE>(ll);
		if (ll == static_cast<LONGLONG>(pv->dblVal)) {
			pv->vt = VT_R8;
			return;
		}
		SAFEARRAY *psa = SafeArrayCreateVector(VT_I4, 0, sizeof(LONGLONG) / sizeof(int));
		if (psa) {
			PVOID pvData;
			if (::SafeArrayAccessData(psa, &pvData) == S_OK) {
				::CopyMemory(pvData, &ll, sizeof(LONGLONG));
				::SafeArrayUnaccessData(psa);
				pv->vt = VT_ARRAY | VT_I4;
				pv->parray = psa;
			}
		}
	}
}

VOID teSetULL(VARIANT *pv, LONGLONG ll)
{
	if (pv) {
		pv->ulVal = static_cast<UINT>(ll);
		if (ll == static_cast<LONGLONG>(pv->ulVal)) {
			pv->vt = VT_UI4;
			return;
		}
		teSetLL(pv, ll);
	}
}

VOID teSetSZ(VARIANT *pv, LPCWSTR lpstr)
{
	if (pv) {
		pv->bstrVal = ::SysAllocString(lpstr);
		pv->vt = VT_BSTR;
	}
}

VOID teCoTaskMemFree(LPVOID pv)
{
	if (pv) {
		try {
			::CoTaskMemFree(pv);
		} catch (...) {
			g_nException = 0;
#ifdef _DEBUG
			g_strException = L"CoTaskMemFree";
#endif
		}
	}
}

HRESULT teCreateInstance(CLSID clsid, LPWSTR lpszDllFile, HMODULE *phDll, REFIID riid, PVOID *ppvObj)
{
	*ppvObj = NULL;
	HMODULE hDll = NULL;
	HRESULT hr = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, riid, ppvObj);
	if FAILED(hr) {
		BSTR bsDllFile = NULL;
		WCHAR pszPath[MAX_PATH];
		if (lstrlen(lpszDllFile) == 0) {
			HKEY hKey;
			lstrcpy(pszPath, L"CLSID\\");
			LPOLESTR lpClsid;
			StringFromCLSID(clsid, &lpClsid);
			lstrcat(pszPath, lpClsid);
			teCoTaskMemFree(lpClsid);
			lstrcat(pszPath, L"\\InprocServer32");
			if (RegOpenKeyEx(HKEY_CLASSES_ROOT, pszPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
				DWORD dwSize = sizeof(pszPath);
				if (RegQueryValueEx(hKey, NULL, NULL, NULL, (LPBYTE)&pszPath, &dwSize) == ERROR_SUCCESS) {
					ExpandEnvironmentStrings(pszPath, pszPath, MAX_PATH);
					lpszDllFile = pszPath;
				}
				RegCloseKey(hKey);
			}
		}
		hDll = LoadLibrary(lpszDllFile);
		if (hDll) {
			LPFNDllGetClassObject lpfnDllGetClassObject = (LPFNDllGetClassObject)GetProcAddress(hDll, "DllGetClassObject");
			if (lpfnDllGetClassObject) {
				IClassFactory *pCF;
				hr = lpfnDllGetClassObject(clsid, IID_PPV_ARGS(&pCF));
				if (hr == S_OK) {
					hr = pCF->CreateInstance(NULL, riid, ppvObj);
					pCF->Release();
					if (hr == S_OK && phDll) {
						*phDll = hDll;
					}
				}
			} else {
				FreeLibrary(hDll);
			}
		}
	}
	return hr;
}

BSTR tePSGetNameFromPropertyKeyEx(PROPERTYKEY propKey, int nFormat, IShellView *pSV)
{
	if (nFormat == 2) {
		WCHAR szProp[64];
#ifdef _2000XP
		if (lpfnPSStringFromPropertyKey) {
			lpfnPSStringFromPropertyKey(propKey, szProp, 64);
		} else {
			StringFromGUID2(propKey.fmtid, szProp, 39);
			wchar_t pszId[8];
			swprintf_s(pszId, 8, L" %u", propKey.pid);
			lstrcat(szProp, pszId);
		}
#else
		PSStringFromPropertyKey(propKey, szProp, 64);
#endif
		return ::SysAllocString(szProp);
	}
#ifdef _2000XP
	if (lpfnPSGetPropertyDescription) {
#endif
		BSTR bs = NULL;
		IPropertyDescription *pdesc;
		if SUCCEEDED(lpfnPSGetPropertyDescription(propKey, IID_PPV_ARGS(&pdesc))) {
			LPWSTR psz = NULL;
			CM_COLUMNINFO cmci = { sizeof(CM_COLUMNINFO), CM_MASK_NAME };
			cmci.wszName[0] = NULL;
			if (nFormat) {
				pdesc->GetCanonicalName(&psz);
			} else {
				IColumnManager *pColumnManager;
				if (pSV && SUCCEEDED(pSV->QueryInterface(IID_PPV_ARGS(&pColumnManager)))) {
					pColumnManager->GetColumnInfo(propKey, &cmci);
					pColumnManager->Release();
				}
				if (cmci.wszName[0]) {
					bs = ::SysAllocString(cmci.wszName);
				} else {
					pdesc->GetDisplayName(&psz);
				}
			}
			if (psz) {
				bs = ::SysAllocString(psz);
				teCoTaskMemFree(psz);
			}
			pdesc->Release();
		} else {
			bs = tePSGetNameFromPropertyKeyEx(propKey, 2, NULL);
		}
		return bs;
#ifdef _2000XP
	}
	if (IsEqualPropertyKey(propKey, PKEY_TotalFileSize)) {
		return ::SysAllocString(nFormat ? g_szTotalFileSizeCodeXP : g_szTotalFileSizeXP);
	}
	if (IsEqualPropertyKey(propKey, PKEY_Contact_Label)) {
		return ::SysAllocString(nFormat ? g_szLabelCodeXP : g_szLabelXP);
	}
	WCHAR szProp[MAX_PROP];
	IPropertyUI *pPUI;
	if SUCCEEDED(teCreateInstance(CLSID_PropertiesUI, NULL, NULL, IID_PPV_ARGS(&pPUI))) {
		if (nFormat) {
			pPUI->GetCannonicalName(propKey.fmtid, propKey.pid, szProp, MAX_PROP);
		} else {
			pPUI->GetDisplayName(propKey.fmtid, propKey.pid, PUIFNF_DEFAULT, szProp, MAX_PROP);
		}
		pPUI->Release();
	}
	return ::SysAllocString(szProp);
#endif
	return NULL;
}

BOOL teSetForegroundWindow(HWND hwnd)
{
	BOOL bResult = SetForegroundWindow(hwnd);
	if (!bResult) {
		DWORD dwForeThreadId = GetWindowThreadProcessId(GetForegroundWindow(), NULL);
		AttachThreadInput(g_dwMainThreadId, dwForeThreadId, TRUE);
		SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		bResult = SetForegroundWindow(hwnd);
		AttachThreadInput(g_dwMainThreadId, dwForeThreadId, FALSE);
		if (!bResult) {
			DWORD dwTimeout = 0;
			SystemParametersInfo(SPI_GETFOREGROUNDLOCKTIMEOUT, 0, &dwTimeout, 0);
			SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, NULL, 0);
			SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			bResult = SetForegroundWindow(hwnd);
			SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, &dwTimeout, 0);
			if (!bResult) {
				Sleep(500);
				bResult = SetForegroundWindow(hwnd);
				if (!bResult && hwnd == g_hwndMain) {
					SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) & ~WS_EX_TOOLWINDOW);
					ShowWindow(hwnd, SW_SHOWNORMAL);
					bResult = SetForegroundWindow(hwnd);
				}
			}
		}
	}
	return bResult;
}

BOOL teChangeWindowMessageFilterEx(HWND hwnd, UINT message, DWORD action, PCHANGEFILTERSTRUCT pChangeFilterStruct)
{
	if (lpfnChangeWindowMessageFilterEx && hwnd) {
		return lpfnChangeWindowMessageFilterEx(hwnd, message, action, pChangeFilterStruct);
	}
#ifdef _2000XP
	if (lpfnChangeWindowMessageFilter) {
		return lpfnChangeWindowMessageFilter(message, action);
	}
#else
	return ChangeWindowMessageFilter(message, action);
#endif
	return FALSE;
}

BOOL teGetStrFromFolderItem(BSTR *pbs, IUnknown *punk)
{
	*pbs = NULL;
	CteFolderItem *pid;
	if (punk && SUCCEEDED(punk->QueryInterface(g_ClsIdFI, (LPVOID *)&pid))) {
		*pbs = pid->GetStrPath();
		pid->Release();
	}
	return *pbs != NULL;
}

VOID teAdvise(IUnknown *punk, IID diid, IUnknown *punk2, PDWORD pdwCookie)
{
	IConnectionPointContainer *pCPC;
	if (SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pCPC)))) {
		IConnectionPoint *pCP;
		if (SUCCEEDED(pCPC->FindConnectionPoint(diid, &pCP))) {
			pCP->Advise(punk2, pdwCookie);
			pCP->Release();
		}
		pCPC->Release();
	}
}

VOID teUnadviseAndRelease(IUnknown *punk, IID diid, PDWORD pdwCookie)
{
	if (punk) {
		IConnectionPointContainer *pCPC;
		if SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pCPC))) {
			IConnectionPoint *pCP;
			if (SUCCEEDED(pCPC->FindConnectionPoint(diid, &pCP))) {
				pCP->Unadvise(*pdwCookie);
				pCP->Release();
			}
			pCPC->Release();
		}
		punk->Release();
	}
	*pdwCookie = 0;
}

VOID teRevoke()
{
	if (g_pSW) {
		try {
			if (g_lSWCookie) {
				g_pSW->Revoke(g_lSWCookie);
				g_lSWCookie = 0;
			}
			if (g_dwSWCookie) {
				teUnadviseAndRelease(g_pSW, DIID_DShellWindowsEvents, &g_dwSWCookie);
			}
		} catch (...) {
			g_nException = 0;
#ifdef _DEBUG
			g_strException = L"Revoke";
#endif
		}
	}
	g_pSW = NULL;
}

VOID teRegister(BOOL bInit)
{
	if (bInit || g_pSW) {
		IShellWindows *pSW;
		if (SUCCEEDED(teCreateInstance(CLSID_ShellWindows,
			NULL, NULL, IID_PPV_ARGS(&pSW)))) {
			if (pSW != g_pSW) {
				teRevoke();
				g_pSW = pSW;
				if (g_pWebBrowser) {
					pSW->Register(g_pWebBrowser->m_pWebBrowser, (LONG)g_hwndMain, SWC_3RDPARTY, &g_lSWCookie);
					BSTR bsPath;
					teGetModuleFileName(NULL, &bsPath);//Executable Path
					LPITEMIDLIST pidl = SHSimpleIDListFromPath(bsPath);
					VARIANT v;
					if (GetVarArrayFromIDList(&v, pidl)) {
#ifdef _DEBUG
						HRESULT hr = pSW->OnNavigate(g_lSWCookie, &v);
						if (hr != S_OK) {
							WCHAR pszNum[10];
							swprintf_s(pszNum, 9, L"%08x", hr);
							MessageBox(g_hwndMain, pszNum, _T(PRODUCTNAME), MB_OK | MB_ICONERROR);
						}
#else
						pSW->OnNavigate(g_lSWCookie, &v);
#endif
						::VariantClear(&v);
					}
					teCoTaskMemFree(pidl);
					teSysFreeString(&bsPath);
				}
				teAdvise(pSW, DIID_DShellWindowsEvents, static_cast<IDispatch *>(g_pTE), &g_dwSWCookie);
			} else {
				pSW->Release();
			}
		}
	}
}

VOID teSetRedraw(BOOL bSetRedraw)
{
	if (g_pWebBrowser) {
		SendMessage(g_pWebBrowser->m_hwndBrowser, WM_SETREDRAW, bSetRedraw, 0);
		if (bSetRedraw && !g_bSetRedraw) {
			RedrawWindow(g_pWebBrowser->m_hwndBrowser, NULL, 0, RDW_NOERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
		}
	}
	g_bSetRedraw = bSetRedraw;
}

HRESULT teExceptionEx(EXCEPINFO *pExcepInfo, LPCSTR pszObjA, LPCSTR pszNameA)
{
	g_nException = 0;
#ifdef _DEBUG
	if (!g_strException) {
		g_strException = L"Exception";
	}
#endif
	if (pExcepInfo) {
		BSTR bsObj = teMultiByteToWideChar(CP_UTF8, pszObjA, -1);
		BSTR bsName = teMultiByteToWideChar(CP_UTF8, pszNameA, -1);
		int nLen = ::SysStringLen(bsObj) + ::SysStringLen(bsName) + 15;
		pExcepInfo->bstrDescription = ::SysAllocStringLen(NULL, nLen);
		swprintf_s(pExcepInfo->bstrDescription, nLen, L"Exception in %s.%s", bsObj, pszNameA ? bsName : L"");
		pExcepInfo->bstrSource = ::SysAllocString(g_szTE);
		pExcepInfo->scode = E_UNEXPECTED;
		::SysFreeString(bsName);
		::SysFreeString(bsObj);
		return DISP_E_EXCEPTION;
	}
	return E_UNEXPECTED;
}

HRESULT teException(EXCEPINFO *pExcepInfo, LPCSTR pszObjA, TEmethod* pMethod, DISPID dispIdMember)
{
	LPSTR pszNameA = NULL;
	if (pMethod) {
		int i = 0;
		while (pMethod[i].id && pMethod[i].id != dispIdMember) {
			i++;
		}
		pszNameA = pMethod[i].name;
	}
	return teExceptionEx(pExcepInfo, pszObjA, pszNameA);
}

BOOL teStrSameIFree(BSTR bs, LPWSTR lpstr2)
{
	BOOL b = lstrcmpi(bs, lpstr2) == 0;
	::SysFreeString(bs);
	return b;
}

VOID teILCloneReplace(LPITEMIDLIST *ppidl, LPCITEMIDLIST pidl)
{
	if (ppidl) {
		LPITEMIDLIST pidl2 = *ppidl;
		*ppidl = ILClone(pidl);
		teCoTaskMemFree(pidl2);
	}
}

VOID teILFreeClear(LPITEMIDLIST *ppidl)
{
	if (ppidl) {
		teCoTaskMemFree(*ppidl);
		*ppidl = NULL;
	}
}

#ifdef _USE_TESTPATHMATCHSPEC
BOOL tePathMatchSpec2(LPCWSTR pszFile, LPWSTR pszSpec)
{
	switch (*pszSpec)
	{
		case NULL:
		case ';':
			return !pszFile[0];
		case '*':
			return tePathMatchSpec2(pszFile, pszSpec + 1) || (pszFile[0] && tePathMatchSpec2(pszFile + 1, pszSpec));
		case '?':
			return pszFile[0] && tePathMatchSpec2(pszFile + 1, pszSpec + 1);
		default:
			return (towlower(pszFile[0]) == towlower(pszSpec[0])) && tePathMatchSpec2(pszFile + 1, pszSpec + 1);
	}
}
#endif

BOOL teStartsText(LPWSTR pszSub, LPCWSTR pszFile)
{
	BOOL bResult = pszFile ? TRUE : FALSE;
	WCHAR wc;
	while (bResult && (wc = *pszSub++)) {
		bResult = towlower(wc) == towlower(*pszFile++);
	}
	return bResult;
}

VOID teGetJunctionLinkTarget(BSTR bsPath, LPWSTR *ppszText, int cchTextMax)
{
	HANDLE hFile = CreateFile(bsPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		BYTE buf[MAXIMUM_REPARSE_DATA_BUFFER_SIZE];
		REPARSE_DATA_BUFFER *tpReparseDataBuffer = (_REPARSE_DATA_BUFFER*)buf;
		DWORD dwRet = 0;
		if (DeviceIoControl(hFile, FSCTL_GET_REPARSE_POINT, NULL, 0, tpReparseDataBuffer,
			MAXIMUM_REPARSE_DATA_BUFFER_SIZE, &dwRet, NULL)) {
			if (IsReparseTagMicrosoft(tpReparseDataBuffer->ReparseTag)) {
				if (tpReparseDataBuffer->ReparseTag == IO_REPARSE_TAG_MOUNT_POINT) {
					LPWSTR lpPathBuffer = (LPWSTR)tpReparseDataBuffer->MountPointReparseBuffer.PathBuffer;
					LPWSTR lpPath = &lpPathBuffer[tpReparseDataBuffer->MountPointReparseBuffer.SubstituteNameOffset / 2];
					if (teStartsText(L"\\??\\", lpPath)) {
						lpPath += 4;
					}
					if (*ppszText) {
						lstrcpyn(*ppszText, lpPath, cchTextMax);
					} else {
						*ppszText = ::SysAllocString(lpPath);
					}
				}
			}
		}
		CloseHandle(hFile);
	}
}

BOOL tePathMatchSpec1(LPCWSTR pszFile, LPWSTR pszSpec, WCHAR wSpecEnd)
{
	WCHAR wc = *pszSpec;
	if (wc == wSpecEnd) {
		return !*pszFile;
	}
	if (!*pszFile && wc != '*') {
		return FALSE;
	}
	for (; *pszFile; pszFile++) {
		wc = *pszSpec++;
		if (wc == '*') {
			wc = towlower(*pszSpec++);
			if (wc == wSpecEnd) {
				return TRUE;
			}
			do {
				if (!*pszFile) {
					return FALSE;
				}
				if (wc != '*' && wc != '?') {
					while (towlower(*pszFile) != wc) {
						if (!*(++pszFile)) {
							return FALSE;
						}
					}
				}
			} while (!tePathMatchSpec1(++pszFile, pszSpec, wSpecEnd));
			return TRUE;
		}
		if (wc != '?') {
			if (wc == wSpecEnd || towlower(*pszFile) != towlower(wc)) {
				return FALSE;
			}
		}
	}
	for (; (wc = *pszSpec) == '*'; pszSpec++);
	return *pszFile == (wc == wSpecEnd ? NULL : wc);
}

BOOL tePathMatchSpec(LPCWSTR pszFile, LPWSTR pszSpec)
{
	LPWSTR pszSpecEnd;
	if (!pszSpec || !pszSpec[0]) {
		return TRUE;
	}
	if (!pszFile) {
		return FALSE;
	}
	do {
		pszSpecEnd = StrChr(pszSpec, ';');
#ifdef _USE_TESTPATHMATCHSPEC
		BOOL b1 = !!tePathMatchSpec1(pszFile, pszSpec, pszSpecEnd ? ';' : NULL);
		BOOL b2 = !!tePathMatchSpec2(pszFile, pszSpec);
		if (b1 != b2) {
			b2 = !!tePathMatchSpec1(pszFile, pszSpec, pszSpecEnd ? ';' : NULL);
		}
#endif
		if (tePathMatchSpec1(pszFile, pszSpec, pszSpecEnd ? ';' : NULL)) {
			return TRUE;
		}
		pszSpec = pszSpecEnd + 1;
	} while (pszSpecEnd);
	return FALSE;
}

BOOL tePathIsNetworkPath(LPCWSTR pszPath)//PathIsNetworkPath is slow in DRIVE_NO_ROOT_DIR.
{
	WCHAR pszDrive[4];
	lstrcpyn(pszDrive, pszPath, 4);
	if (pszDrive[0] >= 'A' && pszDrive[1] == ':' && pszDrive[2] == '\\') {
		UINT uDriveType = GetDriveType(pszDrive);
		return uDriveType == DRIVE_REMOTE || uDriveType == DRIVE_NO_ROOT_DIR;
	}
	return tePathMatchSpec(pszPath, L"\\\\*;*://*") && !teStartsText(L"\\\\\\", pszPath);
}

HRESULT STDAPICALLTYPE tePSPropertyKeyFromStringEx(__in LPCWSTR pszString,  __out PROPERTYKEY *pkey)
{
	HRESULT hr = lpfnPSPropertyKeyFromString(pszString, pkey);
	return SUCCEEDED(hr) ? hr : lpfnPSGetPropertyKeyFromName(pszString, pkey);
}

int CalcCrc32(BYTE *pc, int nLen, UINT c)
{
	c ^= MAXUINT;
	for (int i = 0; i < nLen; i++) {
		c = g_uCrcTable[LOBYTE(c ^ pc[i])] ^ (c >> 8);
	}
	return c ^ MAXUINT;
}

int teGetModuleFileName(HMODULE hModule, BSTR *pbsPath)
{
	int i = 0;
	*pbsPath = NULL;
	for (int nSize = MAX_PATH; nSize < MAX_PATHEX; nSize += MAX_PATH) {
		SysReAllocStringLen(pbsPath, NULL, nSize);
		i = GetModuleFileName(hModule, *pbsPath, nSize);
		if (i + 1 < nSize) {
			(*pbsPath)[0] = towupper((*pbsPath)[0]);
			(*pbsPath)[i] = NULL;
			break;
		}
	}
	return i;
}

int teDragQueryFile(HDROP hDrop, UINT iFile, BSTR *pbsPath)
{
	UINT i = 0;
	for (UINT nSize = MAX_PATH; nSize < MAX_PATHEX; nSize += MAX_PATH) {
		*pbsPath = ::SysAllocStringLen(NULL, nSize);
		i = DragQueryFile(hDrop, iFile, *pbsPath, nSize);
		if (i + 1 < nSize) {
			(*pbsPath)[i] = NULL;
			break;
		}
		::SysFreeString(*pbsPath);
	}
	return i;
}

VOID tePathAppend(BSTR *pbsPath, LPCWSTR pszPath, LPWSTR pszFile)
{
	BSTR bsPath = teSysAllocStringLen(pszPath, lstrlen(pszPath) + lstrlen(pszFile) + 1);
	PathAppend(bsPath, pszFile);
	*pbsPath = ::SysAllocString(bsPath);
	teSysFreeString(&bsPath);
}

BOOL GetShellFolder(IShellFolder **ppSF, LPCITEMIDLIST pidl)
{
	if (ILIsEmpty(pidl)) {
		SHGetDesktopFolder(ppSF);
		return TRUE;
	}
	IShellFolder *pSF;
	LPCITEMIDLIST pidlPart;
	if SUCCEEDED(SHBindToParent(pidl, IID_PPV_ARGS(&pSF), &pidlPart)) {
		pSF->BindToObject(pidlPart, NULL, IID_PPV_ARGS(ppSF));
		pSF->Release();
	}
	if (*ppSF == NULL) {
		return FALSE;
	}
#ifdef _2000XP
	if (!g_bUpperVista) {
		if (ILIsEqual(pidl, g_pidls[CSIDL_RESULTSFOLDER])) {
			IPersistFolder *pPF;
			if SUCCEEDED((*ppSF)->QueryInterface(IID_PPV_ARGS(&pPF))) {
				pPF->Initialize(pidl);
				pPF->Release();
			}
		}
	}
#endif
	return TRUE;
}

BOOL teSFIsShellFSFolder(IShellFolder *pSF)
{
	BOOL bResult = FALSE;
	IPersist *pPersist;
	if SUCCEEDED(pSF->QueryInterface(IID_PPV_ARGS(&pPersist))) {
		CLSID clsid;
		if SUCCEEDED(pPersist->GetClassID(&clsid)) {
			bResult = IsEqualCLSID(clsid, CLSID_ShellFSFolder);
		}
		pPersist->Release();
	}
	return bResult;
}

BOOL teIsShellFSFolder(LPITEMIDLIST pidl, BOOL bParent)
{
	BOOL bResult = FALSE;
	if (pidl) {
		IShellFolder *pSF;
		LPCITEMIDLIST pidlPart;
		if (bParent ? SUCCEEDED(SHBindToParent(pidl, IID_PPV_ARGS(&pSF), &pidlPart)) : GetShellFolder(&pSF, pidl)) {
			bResult = teSFIsShellFSFolder(pSF);
			pSF->Release();
		}
	}
	return bResult;
}

LPITEMIDLIST teSHSimpleIDListFromPath(LPWSTR lpstr, BOOL bFolder, WORD wAttr)
{
	LPITEMIDLIST pidl = NULL;
	int n = lstrlen(lpstr);
	if (bFolder || (n > 0 && lpstr[n - 1] == '\\')) {
		BSTR bs;
		tePathAppend(&bs, lpstr, L"a");
		pidl = SHSimpleIDListFromPath(bs);
		ILRemoveLastID(pidl);
		::SysFreeString(bs);
		if (!teIsShellFSFolder(pidl, FALSE)) {
			teILFreeClear(&pidl);
		}
	}
	if (!pidl) {
		pidl = SHSimpleIDListFromPath(lpstr);
	}
	if (wAttr && teIsShellFSFolder(pidl, TRUE)) {
		LPWORD pwIdl = (LPWORD)ILFindLastID(pidl);
		if (pwIdl[6] == 0) {
			pwIdl[6] = wAttr;
		}
	}
	return pidl;
}

LPITEMIDLIST teSHSimpleIDListFromPathEx(LPWSTR lpstr, BOOL bFolder, WORD wAttr, DWORD nSizeLow, DWORD nSizeHigh, FILETIME ft)
{
	LPITEMIDLIST pidl = teSHSimpleIDListFromPath(lpstr, bFolder, wAttr);
	if (teIsShellFSFolder(pidl, TRUE)) {
		LPWORD pwIdl = (LPWORD)ILFindLastID(pidl);
		if (ft.dwLowDateTime || ft.dwHighDateTime) {
			if (*((DWORD *)&pwIdl[4]) == 0) {
				::FileTimeToDosDateTime(&ft, &pwIdl[4], &pwIdl[5]);
			}
		}
		if (nSizeLow && *((DWORD *)&pwIdl[2]) == 0) {
			*((DWORD *)&pwIdl[2]) = nSizeLow;
		}
#ifdef _2000XP
		if (g_bUpperVista) {
#endif
			if (nSizeHigh && *((DWORD *)&pwIdl[27]) == 0) {
				*((DWORD *)&pwIdl[27]) = nSizeHigh;
			}
#ifdef _2000XP
		}
#endif
	}
	return pidl;
}

ULONGLONG teGetFolderSize(LPCWSTR szPath, int nItems, PDWORD pdwSessionId, DWORD dwSessionId)
{
	if (PathIsRoot(szPath)) {
		ULARGE_INTEGER FreeBytesOfAvailable;
		ULARGE_INTEGER TotalNumberOfBytes;
		ULARGE_INTEGER TotalNumberOfFreeBytes;
		return GetDiskFreeSpaceEx(szPath, &FreeBytesOfAvailable, &TotalNumberOfBytes, &TotalNumberOfFreeBytes) ? TotalNumberOfBytes.QuadPart - TotalNumberOfFreeBytes.QuadPart : 0;
	}
	ULONGLONG Result = 0;
	std::list<BSTR>	pFolders;
	WIN32_FIND_DATA wfd;
	pFolders.push_front(::SysAllocString(szPath));

	while (g_bMessageLoop && *pdwSessionId == dwSessionId && !pFolders.empty()) {
		BSTR bsPath = *pFolders.begin();
		pFolders.pop_front();
		BSTR bs2;
		tePathAppend(&bs2, bsPath, L"*");
		HANDLE hFind = FindFirstFile(bs2, &wfd);
		teSysFreeString(&bs2);
		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				if (nItems-- < 0) {
					dwSessionId++;
					break;
				}
				if (tePathMatchSpec(wfd.cFileName, L".;..")) {
					continue;
				}
				if ((wfd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) == FILE_ATTRIBUTE_DIRECTORY) {
					tePathAppend(&bs2, bsPath, wfd.cFileName);
					pFolders.push_back(bs2);
					continue;
				}
				ULARGE_INTEGER uli;
				uli.HighPart = wfd.nFileSizeHigh;
				uli.LowPart = wfd.nFileSizeLow;
				Result += uli.QuadPart;
			} while (g_bMessageLoop && *pdwSessionId == dwSessionId && FindNextFile(hFind, &wfd));
			FindClose(hFind);
		}
		teSysFreeString(&bsPath);
	}
	while (!pFolders.empty()) {
		::SysFreeString(*pFolders.begin());
		pFolders.pop_front();
	}
	return Result;
}

BOOL teSetObject(VARIANT *pv, PVOID pObj)
{
	if (pv && pObj) {
		try {
			IUnknown *punk = static_cast<IUnknown *>(pObj);
			if SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pv->pdispVal))) {
				pv->vt = VT_DISPATCH;
				return TRUE;
			}
			if SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pv->punkVal))) {
				pv->vt = VT_UNKNOWN;
				return TRUE;
			}
		} catch (...) {
			g_nException = 0;
#ifdef _DEBUG
			g_strException = L"SetObject";
#endif
		}
	}
	return false;
}

BOOL teSetObjectRelease(VARIANT *pv, PVOID pObj)
{
	if (pObj) {
		try {
			IUnknown *punk = static_cast<IUnknown *>(pObj);
			if (pv) {
				if SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pv->pdispVal))) {
					pv->vt = VT_DISPATCH;
					punk->Release();
					return TRUE;
				}
				if SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pv->punkVal))) {
					pv->vt = VT_UNKNOWN;
					punk->Release();
					return TRUE;
				}
			}
			punk->Release();
		} catch (...) {
			g_nException = 0;
#ifdef _DEBUG
			g_strException = L"SetObjectRelease";
#endif
		}
	}
	return false;
}

BOOL teVariantTimeToFileTime(DOUBLE dt, LPFILETIME pft)
{
	SYSTEMTIME SysTime;
	if (::VariantTimeToSystemTime(dt, &SysTime)) {
		FILETIME ft;
		if (::SystemTimeToFileTime(&SysTime, &ft)) {
			return ::LocalFileTimeToFileTime(&ft, pft);
		}
	}
	return FALSE;
}

BOOL teFileTimeToVariantTime(LPFILETIME pft, DOUBLE *pdt)
{
	FILETIME ft;
	if (::FileTimeToLocalFileTime(pft, &ft)) {
		SYSTEMTIME SysTime;
		if (::FileTimeToSystemTime(&ft, &SysTime)) {
			return ::SystemTimeToVariantTime(&SysTime, pdt);
		}
	}
	return FALSE;
}

VOID GetDragItems(CteFolderItems **ppDragItems, IDataObject *pDataObj)
{
	SafeRelease(ppDragItems);
	*ppDragItems = new CteFolderItems(pDataObj, NULL);
}

BOOL teIsFileSystem(LPOLESTR bs)
{
	return tePathMatchSpec(bs, L"?:\\*;\\\\*\\*");
}

VARIANTARG* GetNewVARIANT(int n)
{
	VARIANT *pv = new VARIANTARG[n];
	while (n--) {
		VariantInit(&pv[n]);
	}
	return pv;
}

BOOL FindUnknown(VARIANT *pv, IUnknown **ppunk)
{
	if (pv) {
		if (pv->vt == VT_DISPATCH || pv->vt == VT_UNKNOWN) {
			*ppunk = pv->punkVal;
			return *ppunk != NULL;
		}
		if (pv->vt == (VT_VARIANT | VT_BYREF)) {
			return FindUnknown(pv->pvarVal, ppunk);
		}
		if (pv->vt == (VT_DISPATCH | VT_BYREF) || pv->vt == (VT_UNKNOWN | VT_BYREF)) {
			*ppunk = *pv->ppunkVal;
			return *ppunk != NULL;
		}
	}
	*ppunk = NULL;
	return false;
}

VOID GetFolderItemFromVariant(FolderItem **ppid, VARIANT *pv)
{
	IUnknown *punk;
	if (!FindUnknown(pv, &punk) || FAILED(punk->QueryInterface(IID_PPV_ARGS(ppid)))) {
		*ppid = new CteFolderItem(pv);
	}
}

BOOL GetDispatch(VARIANT *pv, IDispatch **ppdisp)
{
	IUnknown *punk;
	if (FindUnknown(pv, &punk)) {
		return SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(ppdisp)));
	}
	return FALSE;
}

VOID teRegisterDragDrop(HWND hwnd, IDropTarget *pDropTarget, IDropTarget **ppDropTarget)
{
	*ppDropTarget = static_cast<IDropTarget *>(GetPropA(hwnd, "OleDropTargetInterface"));
	if (*ppDropTarget) {
		(*ppDropTarget)->AddRef();
		RevokeDragDrop(hwnd);
		RegisterDragDrop(hwnd, pDropTarget);
	}
}

VOID teSetParent(HWND hwnd, HWND hwndParent)
{
	if (GetParent(hwnd) != hwndParent) {
		SetParent(hwnd, hwndParent);
	}
}

int ILGetCount(LPITEMIDLIST pidl)
{
	if (ILIsEmpty(pidl)) {
		return 0;
	}
	return ILGetCount(ILGetNext(pidl)) + 1;
}

HRESULT teGetDisplayNameBSTR(IShellFolder *pSF, PCUITEMID_CHILD pidl, SHGDNF uFlags, BSTR *pbs)
{
	STRRET strret;
	HRESULT hr = pSF->GetDisplayNameOf(pidl, uFlags & 0x3fffffff, &strret);
	if SUCCEEDED(hr) {
		hr = StrRetToBSTR(&strret, pidl, pbs);
	}
	return hr;
}

LPITEMIDLIST teILCreateFromPathEx(LPWSTR pszPath)
{
	LPITEMIDLIST pidl = NULL;
	IBindCtx *pbc = NULL;
	ULONG chEaten;
	ULONG dwAttributes;
	IShellFolder *pSF;
	if SUCCEEDED(SHGetDesktopFolder(&pSF)) {
		if SUCCEEDED(CreateBindCtx(0, &pbc)) {
			pbc->RegisterObjectParam(STR_PARSE_PREFER_FOLDER_BROWSING, static_cast<IDropSource *>(g_pTE));
		}
		try {
			pSF->ParseDisplayName(NULL, pbc, pszPath, &chEaten, &pidl, &dwAttributes);
		} catch (...) {
			pidl = NULL;
		}
		SafeRelease(&pbc);
		pSF->Release();
	}
	return pidl;
}

LPITEMIDLIST teILCreateFromPath3(IShellFolder *pSF, LPWSTR pszPath, HWND hwnd, int nDog)
{
	LPITEMIDLIST pidlResult = NULL;
	IEnumIDList *peidl = NULL;
	if (nDog-- < 0) {
		return NULL;
	}
	LPWSTR lpDelimiter = StrChr(pszPath, '\\');
	int nDelimiter = 0;
	if (lpDelimiter) {
		nDelimiter = (int)(lpDelimiter - pszPath);
	}
	if SUCCEEDED(pSF->EnumObjects(hwnd, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS | SHCONTF_INCLUDEHIDDEN | SHCONTF_INCLUDESUPERHIDDEN | SHCONTF_NAVIGATION_ENUM, &peidl)) {
		int ashgdn[] = { SHGDN_FORPARSING, SHGDN_INFOLDER, SHGDN_INFOLDER | SHGDN_FORPARSING, SHGDN_FORADDRESSBAR | SHGDN_FORPARSING | SHGDN_INFOLDER };
		BSTR bstr = NULL;
		LPWSTR lpfn = NULL;
		LPITEMIDLIST pidlPart;
		while (!pidlResult && peidl->Next(1, &pidlPart, NULL) == S_OK) {
			for (int j = _countof(ashgdn); !pidlResult && j--; teSysFreeString(&bstr)) {
				if SUCCEEDED(teGetDisplayNameBSTR(pSF, pidlPart, ashgdn[j], &bstr)) {
					lpfn = ashgdn[j] & SHGDN_INFOLDER ? bstr : PathFindFileName(bstr);
					if (nDelimiter && nDelimiter == lstrlen(lpfn) && StrCmpNI(const_cast<LPCWSTR>(pszPath), (LPCWSTR)lpfn, nDelimiter) == 0) {
						IShellFolder *pSF1;
						if SUCCEEDED(pSF->BindToObject(pidlPart, NULL, IID_PPV_ARGS(&pSF1))) {
							pidlResult = teILCreateFromPath3(pSF1, &lpDelimiter[1], NULL, nDog);
							pSF1->Release();
						}
						if (!pidlResult) {
							BSTR bsFull;
							if SUCCEEDED(teGetDisplayNameBSTR(pSF, pidlPart, SHGDN_FORPARSING, &bsFull)) {
								LPITEMIDLIST pidlFull = teILCreateFromPathEx(bsFull);
								teSysFreeString(&bsFull);
								if (pidlFull) {
									if (GetShellFolder(&pSF1, pidlFull)) {
										pidlResult = teILCreateFromPath3(pSF1, &lpDelimiter[1], NULL, nDog);
										pSF1->Release();
									}
									teILFreeClear(&pidlFull);
								}
							}
						}
						continue;
					}
					if (lstrcmpi(lpfn, pszPath) == 0) {
						LPITEMIDLIST pidlParent;
						if (teGetIDListFromObject(pSF, &pidlParent)) {
							pidlResult = ILCombine(pidlParent, pidlPart);
						}
						teCoTaskMemFree(pidlParent);
						continue;
					}
				}
			}
			teCoTaskMemFree(pidlPart);
		}
		peidl->Release();
	}	
	return pidlResult;
}

LPITEMIDLIST teILCreateFromPath2(LPITEMIDLIST pidlParent, LPWSTR pszPath, HWND hwnd)
{
	LPITEMIDLIST pidlResult = NULL;
	IShellFolder *pSF;
	if (GetShellFolder(&pSF, pidlParent)) {
		pidlResult = teILCreateFromPath3(pSF, pszPath, hwnd, 256);
		pSF->Release();
	}
	return pidlResult;
}

VOID teReleaseILCreate(TEILCreate *pILC)
{
	if (::InterlockedDecrement(&pILC->cRef) == 0) {
		teCoTaskMemFree(pILC->pidlResult);
		CloseHandle(pILC->hEvent);
		delete [] pILC;
	}
}

BOOL GetCSIDLFromPath(int *i, LPWSTR pszPath)
{
	int n = lstrlen(pszPath);
	if (n < 3 && pszPath[0] >= '0' && pszPath[0] <= '9') {
		swscanf_s(pszPath, L"%d", i);
		return (*i <= 9 && n == 1) || (*i >= 10 && *i < MAX_CSIDL2);
	}
	return FALSE;
}

HRESULT teGetDisplayNameFromIDList(BSTR *pbs, LPITEMIDLIST pidl, SHGDNF uFlags)
{
	HRESULT hr = E_FAIL;
	IShellFolder *pSF;
	LPCITEMIDLIST pidlPart;

	if SUCCEEDED(SHBindToParent(pidl, IID_PPV_ARGS(&pSF), &pidlPart)) {
		hr = teGetDisplayNameBSTR(pSF, pidlPart, uFlags & (~(SHGDN_FORPARSINGEX | SHGDN_ORIGINAL)), pbs);
		if (hr == S_OK) {
			if (tePathMatchSpec(*pbs, L"?:\\*")) {
				if (uFlags & SHGDN_FORPARSINGEX) {
					BOOL bSpecial = FALSE;
					BSTR bs2;
					if SUCCEEDED(teGetDisplayNameBSTR(pSF, pidlPart, SHGDN_FORADDRESSBAR | SHGDN_FORPARSING, &bs2)) {
						bSpecial = tePathMatchSpec(bs2, L"search-ms:*;*://*");
						teSysFreeString(&bs2);
					}
					if (!bSpecial) {
						LPITEMIDLIST pidl2 = SHSimpleIDListFromPath(*pbs);
						if (!ILIsEqual(pidl, pidl2)) {
							teILFreeClear(&pidl2);
							int n;
							if (!GetCSIDLFromPath(&n, *pbs)) {
								teILCloneReplace(&pidl2, pidl);
								teSysFreeString(pbs);
								while (!ILIsEmpty(pidl2)) {
									BSTR bs;
									if SUCCEEDED(teGetDisplayNameFromIDList(&bs, pidl2, SHGDN_INFOLDER | SHGDN_FORPARSING | SHGDN_ORIGINAL)) {
										if (*pbs) {
											tePathAppend(&bs2, bs, *pbs);
											::SysFreeString(bs);
											::SysFreeString(*pbs);
											*pbs = bs2;
										} else {
											*pbs = bs;
										}
										::ILRemoveLastID(pidl2);
									}
								}
							}
						}
						teILFreeClear(&pidl2);
					}
				}
			} else if (((uFlags & (SHGDN_FORADDRESSBAR | SHGDN_FORPARSING)) == (SHGDN_FORADDRESSBAR | SHGDN_FORPARSING))) {
				if (ILGetCount(pidl) == 1 || tePathMatchSpec(*pbs, L"search-ms:*\\*")) {
					LPITEMIDLIST pidl2 = teILCreateFromPath2(g_pidls[CSIDL_DESKTOP], *pbs, NULL);
					if (!ILIsEqual(pidl, pidl2)) {
						teSysFreeString(pbs);
						hr = teGetDisplayNameBSTR(pSF, pidlPart, uFlags & (~(SHGDN_FORPARSINGEX | SHGDN_ORIGINAL | SHGDN_FORADDRESSBAR)), pbs);
					}
					teCoTaskMemFree(pidl2);
				}
			}
		}
		pSF->Release();
	}
	if (hr == S_OK && !(uFlags & SHGDN_ORIGINAL)) {
		if (!::ILIsParent(g_pidls[CSIDL_INTERNET], pidl, FALSE) && !tePathMatchSpec(*pbs, L"ftp:*;http:*;https:*")) {
			int j = 0;
			for (int i = ::SysStringLen(*pbs); i-- > 0;) {
				if (((uFlags & SHGDN_INFOLDER) && pbs[0][i] == '\\') || pbs[0][i] < 0x20 || StrChr(L"\"<>|", pbs[0][i])) {
					j++;
				}
			}
			if (j) {
				BSTR bs = ::SysAllocStringLen(NULL, ::SysStringLen(*pbs) + j * 2);
				j = 0;
				for (UINT i = 0; i < ::SysStringLen(*pbs); i++) {
					if (((uFlags & SHGDN_INFOLDER) && pbs[0][i] == '\\') || pbs[0][i] < 0x20 || StrChr(L"\"<>|", pbs[0][i])) {
						swprintf_s(&bs[j], 4, L"%%%02hhX", pbs[0][i]);
						j += 3;
					} else {
						bs[j++] = pbs[0][i];
					}
				}
				teSysFreeString(pbs);
				*pbs = bs;
			}
		}
	}
	return hr;
}

BOOL teILIsFileSystem(LPITEMIDLIST pidl)
{
	BOOL bResult = FALSE;
	BSTR bs;
	if SUCCEEDED(teGetDisplayNameFromIDList(&bs, pidl, SHGDN_FORPARSING)) {
		bResult = teIsFileSystem(bs);
		::SysFreeString(bs);
	}
	return bResult;
}

BOOL teILIsFileSystemEx(LPCITEMIDLIST pidl)
{
	LPCITEMIDLIST pidlPart;
	IShellFolder *pSF;
	if SUCCEEDED(SHBindToParent(pidl, IID_PPV_ARGS(&pSF), &pidlPart)) {
		SFGAOF sfAttr = SFGAO_FILESYSTEM | SFGAO_FILESYSANCESTOR | SFGAO_STORAGEANCESTOR | SFGAO_NONENUMERATED | SFGAO_DROPTARGET;
		if FAILED(pSF->GetAttributesOf(1, &pidlPart, &sfAttr)) {
			sfAttr = 0;
		}
		pSF->Release();
		if (sfAttr & (SFGAO_FILESYSTEM | SFGAO_FILESYSANCESTOR | SFGAO_STORAGEANCESTOR | SFGAO_NONENUMERATED | SFGAO_DROPTARGET)) {
			return TRUE;
		}
	}
	return FALSE;
}

void GetVarPathFromIDList(VARIANT *pVarResult, LPITEMIDLIST pidl, int uFlags)
{
	int i;

	if (uFlags & SHGDN_FORPARSINGEX) {
		for (i = 0; i < MAX_CSIDL; i++) {
			if (g_pidls[i] && ::ILIsEqual(pidl, g_pidls[i])) {
				LPITEMIDLIST pidl2 = SHSimpleIDListFromPath(g_bsPidls[i]);
				if (!::ILIsEqual(pidl, pidl2)) {
					teSetLong(pVarResult, i);
				}
				teCoTaskMemFree(pidl2);
				break;
			}
		}
	}
	if (pVarResult->vt != VT_I4) {
		if SUCCEEDED(teGetDisplayNameFromIDList(&pVarResult->bstrVal, pidl, uFlags)) {
			pVarResult->vt = VT_BSTR;
		}
	}
}

// VARIANT Clean-up of an array
VOID teClearVariantArgs(int nArgs, VARIANTARG *pvArgs)
{
	if (pvArgs && nArgs > 0) {
		for (int i = nArgs ; i-- >  0;){
			VariantClear(&pvArgs[i]);
		}
		delete[] pvArgs;
		pvArgs = NULL;
	}
}

HRESULT Invoke5(IDispatch *pdisp, DISPID dispid, WORD wFlags, VARIANT *pvResult, int nArgs, VARIANTARG *pvArgs)
{
	HRESULT hr;
	// DISPPARAMS
	DISPPARAMS dispParams;
	dispParams.rgvarg = pvArgs;
	dispParams.cArgs = abs(nArgs);
	DISPID dispidName = DISPID_PROPERTYPUT;
	if (wFlags & DISPATCH_PROPERTYPUT) {
		dispParams.cNamedArgs = 1;
		dispParams.rgdispidNamedArgs = &dispidName;
	} else {
		dispParams.rgdispidNamedArgs = NULL;
		dispParams.cNamedArgs = 0;
	}
	try {
		hr = pdisp->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT,
			wFlags, &dispParams, pvResult, NULL, NULL);
	} catch (...) {
		hr = E_FAIL;
	}
	teClearVariantArgs(nArgs, pvArgs);
	return hr;
}

VOID teInvokeObject(IDispatch **ppdisp, WORD wFlags, VARIANT *pVarResult, int nArg, VARIANTARG *pvArgs)
{
	if (wFlags & DISPATCH_METHOD) {
		if (*ppdisp) {
			Invoke5(*ppdisp, DISPID_VALUE, wFlags, pVarResult, - nArg - 1, pvArgs);
		}
		return;
	}
	if (nArg >= 0) {
		SafeRelease(ppdisp);
		GetDispatch(&pvArgs[nArg], ppdisp);
	}
	teSetObject(pVarResult, *ppdisp);
}

HRESULT teDelProperty(IUnknown *punk, LPOLESTR sz)
{
	HRESULT hr = E_FAIL;
	IDispatchEx *pdex;
	if SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pdex))) {
		BSTR bs = ::SysAllocString(sz);
		hr = pdex->DeleteMemberByName(bs, fdexNameCaseSensitive);
		::SysFreeString(bs);
		pdex->Release();
	}
	return hr;
}

HRESULT tePutProperty0(IUnknown *punk, LPOLESTR sz, VARIANT *pv, DWORD grfdex)
{
	HRESULT hr = E_FAIL;
	DISPID dispid, putid;
	DISPPARAMS dispparams;
	IDispatchEx *pdex;
	if SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pdex))) {
		BSTR bs = ::SysAllocString(sz);
		hr = pdex->GetDispID(bs, grfdex, &dispid);
		if SUCCEEDED(hr) {
			putid = DISPID_PROPERTYPUT;
			dispparams.rgvarg = pv;
			dispparams.rgdispidNamedArgs = &putid;
			dispparams.cArgs = 1;
			dispparams.cNamedArgs = 1;
			hr = pdex->InvokeEx(dispid, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUTREF, &dispparams, NULL, NULL, NULL);
		}
		::SysFreeString(bs);
		pdex->Release();
	}
	return hr;
}

HRESULT tePutProperty(IUnknown *punk, LPOLESTR sz, VARIANT *pv)
{
	HRESULT hr = tePutProperty0(punk, sz, pv, fdexNameEnsure);
	VariantClear(pv);
	return hr;
}

BOOL teHasProperty(IDispatch *pdisp, LPOLESTR sz)
{
	DISPID dispid;
	HRESULT hr = pdisp->GetIDsOfNames(IID_NULL, &sz, 1, LOCALE_USER_DEFAULT, &dispid);
	return hr == S_OK;
}

HRESULT teGetProperty(IDispatch *pdisp, LPOLESTR sz, VARIANT *pv)
{
	DISPID dispid;
	HRESULT hr = pdisp->GetIDsOfNames(IID_NULL, &sz, 1, LOCALE_USER_DEFAULT, &dispid);
	if (hr == S_OK) {
		hr = Invoke5(pdisp, dispid, DISPATCH_PROPERTYGET, pv, 0, NULL);
	}
	return hr;
}

//GetProperty(Case-insensitive)
HRESULT teGetPropertyI(IDispatch *pdisp, LPOLESTR sz, VARIANT *pv)
{
	DISPID dispid;
	HRESULT hr = pdisp->GetIDsOfNames(IID_NULL, &sz, 1, LOCALE_USER_DEFAULT, &dispid);
	if FAILED(hr) {
		IDispatchEx *pdex;
		if SUCCEEDED(pdisp->QueryInterface(IID_PPV_ARGS(&pdex))) {
			BSTR bs = ::SysAllocString(sz);
			hr = pdex->GetDispID(bs, fdexNameCaseInsensitive, &dispid);
			::SysFreeString(bs);
			pdex->Release();
		}
	}
	if (hr == S_OK) {
		hr = Invoke5(pdisp, dispid, DISPATCH_PROPERTYGET, pv, 0, NULL);
	}
	return hr;
}

HRESULT teExecMethod(IDispatch *pdisp, LPOLESTR sz, VARIANT *pvResult, int nArg, VARIANTARG *pvArgs)
{
	DISPID dispid;
	HRESULT hr = pdisp->GetIDsOfNames(IID_NULL, &sz, 1, LOCALE_USER_DEFAULT, &dispid);
	if (hr == S_OK) {
		return Invoke5(pdisp, dispid, DISPATCH_METHOD, pvResult, nArg, pvArgs);
	}
	teClearVariantArgs(nArg, pvArgs);
	return hr;
}

BOOL teGetDispatchFromString(VARIANT *pv, IDispatch **ppdisp)
{
	if (pv->vt == VT_BSTR) {
		VARIANT v;
		VariantInit(&v);
		teExecMethod(g_pJS, L"_c", &v, -1, pv);
		if (v.vt == VT_DISPATCH) {
			*ppdisp = v.pdispVal;
			return TRUE;
		}
	}
	return FALSE;
}

HRESULT teGetPropertyAt(IDispatch *pdisp, int i, VARIANT *pv)
{
	wchar_t pszName[8];
	swprintf_s(pszName, 8, L"%d", i);
	return teGetProperty(pdisp, pszName, pv);
}

HRESULT tePutPropertyAt(PVOID pObj, int i, VARIANT *pv)
{
	wchar_t pszName[8];
	swprintf_s(pszName, 8, L"%d", i);
	return tePutProperty((IUnknown *)pObj, pszName, pv);
}

HRESULT teGetPropertyAtLLX(IDispatch *pdisp, LONGLONG i, VARIANT *pv)
{
	wchar_t pszName[20];
	swprintf_s(pszName, 20, L"%llx", i);
	return teGetProperty(pdisp, pszName, pv);
}

HRESULT tePutPropertyAtLLX(PVOID pObj, LONGLONG i, VARIANT *pv)
{
	wchar_t pszName[20];
	swprintf_s(pszName, 20, L"%llx", i);
	return tePutProperty((IUnknown *)pObj, pszName, pv);
}

HRESULT teDelPropertyAtLLX(PVOID pObj, LONGLONG i)
{
	wchar_t pszName[20];
	swprintf_s(pszName, 20, L"%llx", i);
	return teDelProperty((IUnknown *)pObj, pszName);
}

char* GetpcFromVariant(VARIANT *pv, VARIANT *pvMem)
{
	char *pc1 = NULL;
	CteMemory *pMem;
	IUnknown *punk;
	if (FindUnknown(pv, &punk)) {
		if SUCCEEDED(punk->QueryInterface(g_ClsIdStruct, (PVOID *)&pMem)) {
			char *pc = pMem->m_pc;
			pMem->Release();
			return pc;
		}
	}
	if (pvMem) {
		IDispatch *pdisp;
		if (GetDispatch(pv, &pdisp)) {
			teGetProperty(pdisp, L"_BLOB", pvMem);
			pdisp->Release();
			if (pvMem->vt == VT_BSTR) {
				return (char *)pvMem->bstrVal;
			}
		}
	}
	return NULL;
}

LONGLONG GetLLFromVariant(VARIANT *pv)
{
	if (pv) {
		if (pv->vt == (VT_VARIANT | VT_BYREF)) {
			return GetLLFromVariant(pv->pvarVal);
		}
		if (pv->vt == VT_I4) {
			return pv->lVal;
		}
		if (pv->vt == VT_R8) {
			return (LONGLONG)pv->dblVal;
		}
		if (pv->vt == (VT_ARRAY | VT_I4)) {
			LONGLONG ll = 0;
			PVOID pvData;
			if (::SafeArrayAccessData(pv->parray, &pvData) == S_OK) {
				::CopyMemory(&ll, pvData, sizeof(LONGLONG));
				::SafeArrayUnaccessData(pv->parray);
				return ll;
			}
		}
		VARIANT vo;
		VariantInit(&vo);
		if SUCCEEDED(VariantChangeType(&vo, pv, 0, VT_I8)) {
			return vo.llVal;
		}
#ifdef _W2000
		if SUCCEEDED(VariantChangeType(&vo, pv, 0, VT_R8)) {
			return (int)(LONGLONG)vo.dblVal;
		}
#endif
		char *pc = GetpcFromVariant(pv, NULL);
		if (pc) {
			return (LONGLONG)pc;
		}
	}
	return 0;
}

LONGLONG GetLLFromVariantClear(VARIANT *pv)
{
	LONGLONG ll = GetLLFromVariant(pv);
	VariantClear(pv);
	return ll;
}

LONGLONG GetLLPFromVariant(VARIANT *pv, VARIANT *pvMem)
{
	if (pv) {
		if (pv->vt == (VT_VARIANT | VT_BYREF)) {
			return GetLLPFromVariant(pv->pvarVal, pvMem);
		}
		if (pv->vt == VT_I4) {
			return pv->lVal;
		}
		if (pv->vt == VT_R8) {
			return (LONGLONG)pv->dblVal;
		}
		if (pv->vt == (VT_ARRAY | VT_I4)) {
			LONGLONG ll = 0;
			PVOID pvData;
			if (::SafeArrayAccessData(pv->parray, &pvData) == S_OK) {
				::CopyMemory(&ll, pvData, sizeof(LONGLONG));
				::SafeArrayUnaccessData(pv->parray);
				return ll;
			}
		}
		char *pc = GetpcFromVariant(pv, pvMem);
		if (pc) {
			return (LONGLONG)pc;
		}
		if (pv->vt != VT_DISPATCH) {
			VARIANT vo;
			VariantInit(&vo);
			if SUCCEEDED(VariantChangeType(&vo, pv, 0, VT_I8)) {
				return vo.llVal;
			}
#ifdef _W2000
			if SUCCEEDED(VariantChangeType(&vo, pv, 0, VT_R8)) {
				return (LONGLONG)vo.dblVal;
			}
#endif
		}
	}
	return 0;
}

VOID teWriteBack(VARIANT *pvArg, VARIANT *pvDat)
{
	IUnknown *punk;
	if (pvDat->vt == VT_BSTR && pvDat->bstrVal && FindUnknown(pvArg, &punk)) {
		tePutProperty(punk, L"_BLOB", pvDat);
	} else {
		VariantClear(pvDat);
	}
}

VOID teVariantChangeType(__out VARIANTARG * pvargDest,
				__in const VARIANTARG * pvarSrc, __in VARTYPE vt)
{
	VariantInit(pvargDest);
	if (pvarSrc->vt == (VT_ARRAY | VT_I4)) {
		VARIANT v;
		v.llVal = GetLLFromVariant((VARIANT *)pvarSrc);
		v.vt = VT_I8;
		if FAILED(VariantChangeType(pvargDest, &v, 0, vt)) {
			pvargDest->llVal = 0;
		}
		return;
	}
	if FAILED(VariantChangeType(pvargDest, pvarSrc, 0, vt)) {
		pvargDest->llVal = 0;
	}
}

BOOL teGetVariantTime(VARIANT *pv, DATE *pdt)
{
	VARIANT v;
	VariantInit(&v);
	VariantCopy(&v, pv);
	IDispatch *pdisp;
	if (GetDispatch(&v, &pdisp)) {
		VariantClear(&v);
		teExecMethod(pdisp, L"getVarDate", &v, 0, NULL);
		pdisp->Release();
	}
	if (v.vt != VT_DATE) {
		VariantClear(&v);
		teVariantChangeType(&v, pv, VT_DATE);
	}
	if (v.vt == VT_DATE) {
		*pdt = v.date;
		return TRUE;
	}
	VariantClear(&v);
	return FALSE;
}

BOOL teGetSystemTime(VARIANT *pv, SYSTEMTIME *pst)
{
	DATE dt;
	if (teGetVariantTime(pv, &dt)) {
		return ::VariantTimeToSystemTime(dt, pst);
	}
	return FALSE;
}

BOOL teVariantTimeToSystemTime(DATE dt, SYSTEMTIME *pst)
{
	if (::VariantTimeToSystemTime(dt, pst)) {
		FILETIME ft, ft2;
		if (::SystemTimeToFileTime(pst, &ft)) {
			if (::FileTimeToLocalFileTime(&ft, &ft2)) {
				return ::FileTimeToSystemTime(&ft2, pst);
			}
		}
	}
	return FALSE;
}

VOID teExtraLongPath(BSTR *pbs)
{
	int nLen = lstrlen(*pbs);
	if (nLen < MAX_PATH || StrChr(*pbs, '?')) {
		return;
	}
	BSTR bs = tePathMatchSpec(*pbs, L"\\\\*\\*") ? teSysAllocStringLen(L"\\\\?\\UNC", nLen + 7) : teSysAllocStringLen(L"\\\\?\\", nLen + 4);
	lstrcat(bs, *pbs);
	teSysFreeString(pbs);
	*pbs = bs;
}

HRESULT teInvokeAPI(TEDispatchApi *pApi, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo)
{
	int nArg = pDispParams ? pDispParams->cArgs : 0;
	if (pVarResult) {
		VariantInit(pVarResult);
	}
	teParam param[14] = { 0 };
	param[TE_EXCEPINFO].pExcepInfo = pExcepInfo;
	if (nArg-- >= pApi->nArgs) {
		VARIANT vParam[12] = { 0 };
		for (int i = nArg < 11 ? nArg : 11; i >= 0; i--) {
			VariantInit(&vParam[i]);
			if (i == (pApi->nStr1 % 10) || i == (pApi->nStr2 % 10) || i == (pApi->nStr3 % 10)) {
				teVariantChangeType(&vParam[i], &pDispParams->rgvarg[nArg - i], VT_BSTR);
				if (i == (pApi->nStr1 - 10) || i == (pApi->nStr2 - 10) || i == (pApi->nStr3 - 10)) {
					teExtraLongPath(&vParam[i].bstrVal);
				}
				param[i].bstrVal = vParam[i].bstrVal;
			} else {
				param[i].llVal = GetLLPFromVariant(&pDispParams->rgvarg[nArg - i], &vParam[i]);
			}
		}
#ifdef _CHECK_HANDLELEAK
		HANDLE hProcess;
		DWORD dwHandle1, dwHandle2;
		if (hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,FALSE, GetCurrentProcessId())) {
			GetProcessHandleCount(hProcess, &dwHandle1);
			++g_nLeak;
			if (g_nLeak == 19357) {
				Sleep(0);
			}
#endif
			pApi->fn(nArg, param, pDispParams, pVarResult);
#ifdef _CHECK_HANDLELEAK
			GetProcessHandleCount(hProcess, &dwHandle2);
			if (dwHandle2 > dwHandle1) {
				LPWSTR lpwstr = param[0].lpwstr;
				Sleep(0 * param[0].uintVal * param[1].uintVal * param[2].uintVal * param[3].uintVal * param[4].uintVal * g_nLeak);
			}
			CloseHandle(hProcess);
		}
#endif
		for (int i = nArg < 11 ? nArg : 11; i >= 0; i--) {
			if (i != pApi->nStr1 && i != pApi->nStr2 && i != pApi->nStr3) {
				teWriteBack(&pDispParams->rgvarg[nArg - i], &vParam[i]);
			} else {
				VariantClear(&vParam[i]);
			}
		}
	}
	return param[TE_API_RESULT].lVal;
}

LPWSTR teGetCommandLine()
{
	if (!g_bsCmdLine) {
		LPWSTR strCmdLine = GetCommandLine();
		int nSize = lstrlen(strCmdLine) + MAX_PROP;
		BSTR bsCmdLine = ::SysAllocStringLen(NULL, nSize);
		int j = 0;
		int i = 0;
		while (i < nSize && strCmdLine[j]) {
			if (StrCmpNI(&strCmdLine[j], L"/idlist,:", 9) == 0) {
				LONGLONG hData;
				DWORD dwProcessId;
				WCHAR sz[MAX_PATH + 2];
				swscanf_s(&strCmdLine[j + 9], L"%32[1234567890]s", sz, _countof(sz));
				swscanf_s(sz, L"%lld", &hData);
				int k = lstrlen(sz);
				swscanf_s(&strCmdLine[j + k + 10], L"%32[1234567890]s", sz, _countof(sz));
				swscanf_s(sz, L"%d", &dwProcessId);
				j += k + lstrlen(sz) + 11;
#ifdef _2000XP
				LPITEMIDLIST pidl = (LPITEMIDLIST)SHLockShared((HANDLE)hData, dwProcessId);
				if (pidl) {
					VARIANT v, v2;
					GetVarPathFromIDList(&v, pidl, SHGDN_FORPARSING | SHGDN_FORPARSINGEX | SHGDN_ORIGINAL);
					SHUnlockShared(pidl);
					SHFreeShared((HANDLE)hData, dwProcessId);
					teVariantChangeType(&v2, &v, VT_BSTR);
					lstrcpy(sz, v2.bstrVal);
					PathQuoteSpaces(sz);
					lstrcpy(&bsCmdLine[i], sz);
					i += lstrlen(sz);
					VariantClear(&v);
					VariantClear(&v2);
					while (strCmdLine[j] && strCmdLine[j] != _T(',')) {
						j++;
					}
				}
#endif
			} else {
				bsCmdLine[i++] = strCmdLine[j++];
			}
		}
		g_bsCmdLine = teSysAllocStringLenEx(bsCmdLine, i);
		::SysFreeString(bsCmdLine);
	}
	return ::SysAllocStringLen(g_bsCmdLine, ::SysStringLen(g_bsCmdLine));
}

HRESULT teCLSIDFromProgID(__in LPCOLESTR lpszProgID, __out LPCLSID lpclsid)
{
	if (lstrcmpi(lpszProgID, L"ads") == 0) {
		*lpclsid = CLSID_ADODBStream;
		return S_OK;
	}
	if (lstrcmpi(lpszProgID, L"fso") == 0) {
		*lpclsid = CLSID_ScriptingFileSystemObject;
		return S_OK;
	}
	if (lstrcmpi(lpszProgID, L"sha") == 0) {
		*lpclsid = CLSID_Shell;
		return S_OK;
	}
	if (lstrcmpi(lpszProgID, L"wsh") == 0) {
		*lpclsid = CLSID_WScriptShell;
		return S_OK;
	}
	if (StrCmpNI(lpszProgID, L"new:", 4) == 0) {
		return CLSIDFromString(&lpszProgID[4], lpclsid);
	}
	return CLSIDFromProgID(lpszProgID, lpclsid);
}

HRESULT teCLSIDFromString(__in LPCOLESTR lpsz, __out LPCLSID lpclsid)
{
	HRESULT hr = CLSIDFromString(lpsz, lpclsid);
	if (hr == S_OK) {
		return S_OK;
	}
	return teCLSIDFromProgID(lpsz, lpclsid);
}

HWND FindTreeWindow(HWND hwnd)
{
	HWND hwnd1 = FindWindowExA(hwnd, 0, WC_TREEVIEWA, NULL);
	return hwnd1 ? hwnd1 : FindWindowExA(FindWindowExA(hwnd, 0, "NamespaceTreeControl", NULL), 0, WC_TREEVIEWA, NULL);
}

BOOL teSetRect(HWND hwnd, int left, int top, int right, int bottom)
{
	RECT rc;
	GetClientRect(hwnd, &rc);
	MoveWindow(hwnd, left, top, right - left, bottom - top, FALSE);
	return (rc.right - rc.left != right - left || rc.bottom - rc.top != bottom - top);
}

HRESULT teILFolderExists(LPITEMIDLIST pidl)
{
	LPCITEMIDLIST pidlPart;
	IShellFolder *pSF;
	HRESULT hr = SHBindToParent(pidl, IID_PPV_ARGS(&pSF), &pidlPart);
	if FAILED(hr) {
		return hr & MAXWORD;
	}
	SFGAOF sfAttr = SFGAO_FOLDER | SFGAO_FILESYSTEM;
	if FAILED(pSF->GetAttributesOf(1, &pidlPart, &sfAttr)) {
		sfAttr = 0;
	}
	hr = sfAttr & SFGAO_FILESYSTEM ? E_FAIL : S_FALSE;
	if ((sfAttr & SFGAO_FOLDER) && FAILED(hr)) {
		IShellFolder *pSF1;
		hr = pSF->BindToObject(pidlPart, NULL, IID_PPV_ARGS(&pSF1));
		if SUCCEEDED(hr) {
			IEnumIDList *peidl = NULL;
			hr = pSF1->EnumObjects(NULL, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS | SHCONTF_INCLUDEHIDDEN | SHCONTF_INCLUDESUPERHIDDEN | SHCONTF_NAVIGATION_ENUM, &peidl);
			if (peidl) {
				LPITEMIDLIST pidlPart;
				hr = peidl->Next(1, &pidlPart, NULL);
				if (hr == S_FALSE) {
					hr = S_OK;
				}
				teCoTaskMemFree(pidlPart);
				peidl->Release();
			}
			if (hr == E_INVALID_PASSWORD || hr == E_CANCELLED) {
				hr &= MAXWORD;
			}
			pSF1->Release();
		}
	}
	pSF->Release();
	return hr;
}

HRESULT tePathIsDirectory2(LPWSTR pszPath, int iUseFS)
{
	if (!(iUseFS & 2)) {
		WCHAR pszDrive[0x80];
		lstrcpyn(pszDrive, pszPath, 4);
		if (pszDrive[0] >= 'A' && pszDrive[1] == ':' && pszDrive[2] == '\\') {
			if (!GetVolumeInformation(pszDrive, NULL, 0, NULL, NULL, NULL, pszDrive, sizeof(pszDrive))) {
				return E_NOT_READY;
			}
		}
	}
	LPITEMIDLIST pidl = iUseFS ? teILCreateFromPathEx(pszPath) : teILCreateFromPath(pszPath);
	if (pidl) {
		HRESULT hr = teILFolderExists(pidl);
		teCoTaskMemFree(pidl);
		return hr;
	}
	return E_FAIL;
}

VOID teReleaseExists(TEExists *pExists)
{
	if (::InterlockedDecrement(&pExists->cRef) == 0) {
		CloseHandle(pExists->hEvent);
		delete [] pExists;
	}
}

static void threadExists(void *args)
{
	try {
		TEExists *pExists = (TEExists *)args;
		pExists->hr = tePathIsDirectory2(pExists->pszPath, pExists->iUseFS);
		SetEvent(pExists->hEvent);
		teReleaseExists(pExists);
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"threadExists";
#endif
	}
	::_endthread();
}

HRESULT tePathIsDirectory(LPWSTR pszPath, int dwms, int iUseFS)
{
	if (g_dwMainThreadId == GetCurrentThreadId()) {
		if (dwms <= 0) {
			dwms = g_param[TE_NetworkTimeout];
		}
#ifdef _CHECK_HANDLELEAK
		dwms = 0;
#endif
		if (dwms) {
			TEExists *pExists = new TEExists[1];
			pExists->cRef = 2;
			pExists->hr = E_ABORT;
			pExists->pszPath = pszPath;
			pExists->hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			pExists->iUseFS = iUseFS;
			if (_beginthread(&threadExists, 0, pExists) != -1) {
				WaitForSingleObject(pExists->hEvent, dwms);
			} else {
				pExists->cRef--;
			}
			HRESULT hr = pExists->hr;
			teReleaseExists(pExists);
			return hr;
		}
	}
	return tePathIsDirectory2(pszPath, iUseFS);
}

LPITEMIDLIST teILCreateFromPath1(LPWSTR pszPath)
{
	LPITEMIDLIST pidl = NULL;
	BSTR bstr = NULL;
	int n;

	if (pszPath) {
		BSTR bsPath2 = NULL;
		if (pszPath[0] == _T('"')) {
			bsPath2 = teSysAllocStringLen(pszPath, lstrlen(pszPath) + 1);
			PathUnquoteSpaces(bsPath2);
			pszPath = bsPath2;
		}
		BSTR bsPath3 = NULL;
		if (tePathMatchSpec(pszPath, L"search-ms:*&crumb=location:*")) {
			LPWSTR lp1, lp2;
			lp1 = StrChr(pszPath, ':');
			while (lp2 = StrChr(lp1 + 1, ':')) {
				lp1 = lp2;
			}
			lp1 -= 4;
			BSTR bs = ::SysAllocString(lp1);
			bs[0] = 'f';
			bs[1] = 'i';
			bs[2] = 'l';
			bs[3] = 'e';
			DWORD dwLen = ::SysStringLen(bs);
			bsPath3 = ::SysAllocStringLen(NULL, dwLen);
			if SUCCEEDED(PathCreateFromUrl(bs, bsPath3, &dwLen, 0)) {
				pszPath = bsPath3;
			}
		} else if (tePathMatchSpec(pszPath, L"*\\..\\*;*\\..;*\\.\\*;*\\.;*%*%*")) {
			DWORD dwLen = lstrlen(pszPath) + MAX_PATH;
			bsPath3 = ::SysAllocStringLen(NULL, dwLen);
			if (PathSearchAndQualify(pszPath, bsPath3, dwLen)) {
				pszPath = bsPath3;
			}
		} else if (lstrlen(pszPath) == 1 && pszPath[0] >= 'A') {
			bsPath3 = teMultiByteToWideChar(CP_UTF8, "?:\\", -1);
			bsPath3[0] = pszPath[0];
			pszPath = bsPath3;
		}
		if (GetCSIDLFromPath(&n, pszPath)) {
			pidl = ::ILClone(g_pidls[n]);
			pszPath = NULL;
		}
		if (pszPath) {
			if (!pidl) {
				pidl = teILCreateFromPathEx(pszPath);
				if (pidl) {
					if (tePathIsNetworkPath(pszPath) && PathIsRoot(pszPath) && FAILED(tePathIsDirectory(pszPath, 0, 3))) {
						teILFreeClear(&pidl);
					}
				} else if (PathGetDriveNumber(pszPath) >= 0 && !PathIsRoot(pszPath)) {
					WCHAR pszDrive[4];
					lstrcpyn(pszDrive, pszPath, 4);
					int n = GetDriveType(pszDrive);
					if (n == DRIVE_NO_ROOT_DIR && SUCCEEDED(tePathIsDirectory(pszDrive, 0, 3))) {
						pidl = teILCreateFromPathEx(pszPath);
					}
				} else if (tePathMatchSpec(pszPath, L"\\\\*\\*")) {
					WIN32_FIND_DATA wfd;
					HANDLE hFind = FindFirstFile(pszPath, &wfd);
					if (hFind != INVALID_HANDLE_VALUE) {
						FindClose(hFind);
						LPWSTR lpDelimiter = StrChr(&pszPath[2], '\\');
						BSTR bsServer = teSysAllocStringLen(pszPath, int(lpDelimiter - pszPath));
						LPITEMIDLIST pidlServer = teILCreateFromPathEx(bsServer);
						if (pidlServer) {
							pidl = teILCreateFromPath2(pidlServer, &lpDelimiter[1], g_hwndMain);
							teCoTaskMemFree(pidlServer);
						}
						::SysFreeString(bsServer);
					}
				}
			}
			if (pidl == NULL && !teIsFileSystem(pszPath)) {
				for (int i = 0; i < MAX_CSIDL; i++) {
					if (g_pidls[i]) {
						if (!lstrcmpi(pszPath, g_bsPidls[i])) {
							pidl = ILClone(g_pidls[i]);
							break;
						}
					}
				}
				if (!pidl) {
					pidl = teILCreateFromPath2(g_pidls[CSIDL_DESKTOP], pszPath, NULL);
					if (!pidl) {
						pidl = teILCreateFromPath2(g_pidls[CSIDL_DRIVES], pszPath, NULL);
						if (!pidl) {
							pidl = teILCreateFromPath2(g_pidls[CSIDL_USER], pszPath, NULL);
							if (!pidl && PathMatchSpec(pszPath, L"::{*")) {
								int nSize = lstrlen(pszPath) + 6;
								BSTR bsPath4 = teSysAllocStringLen(L"shell:", nSize);
								lstrcat(bsPath4, pszPath);
								pidl = teILCreateFromPathEx(bsPath4);
								::SysFreeString(bsPath4);
							}
						}
					}
				}
			}
		}
		teSysFreeString(&bsPath3);
		teSysFreeString(&bsPath2);
	}
	return pidl;
}

static void threadILCreate(void *args)
{
	try {
		TEILCreate *pILC = (TEILCreate *)args;
		pILC->pidlResult = teILCreateFromPath1(pILC->pszPath);
		SetEvent(pILC->hEvent);
		teReleaseILCreate(pILC);
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"threadILCreate";
#endif
	}
	::_endthread();
}

LPITEMIDLIST teILCreateFromPath0(LPWSTR pszPath, BOOL bForceLimit)
{
	DWORD dwms = g_pTE ? g_param[TE_NetworkTimeout] : 2000;
	if (bForceLimit && (!dwms || dwms > 500)) {
		dwms = 500;
	}
#ifdef _CHECK_HANDLELEAK
	dwms = 0;
#endif
	if (dwms && (bForceLimit || g_dwMainThreadId == GetCurrentThreadId())) {
		TEILCreate *pILC = new TEILCreate[1];
		pILC->cRef = 2;
		pILC->pidlResult = NULL;
		pILC->pszPath = pszPath;
		pILC->hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		LPITEMIDLIST pidl = NULL;
		if (_beginthread(&threadILCreate, 0, pILC) != -1) {
			if (WaitForSingleObject(pILC->hEvent, dwms) != WAIT_TIMEOUT) {
				pidl = pILC->pidlResult;
				if (pidl) {
					pILC->pidlResult = NULL;
				} else {
					pidl = teILCreateFromPath1(pszPath);
				}
			}
		} else {
			pILC->cRef--;
			pidl = teILCreateFromPath1(pszPath);
		}
		teReleaseILCreate(pILC);
		return pidl;
	}
	return teILCreateFromPath1(pszPath);
}

LPITEMIDLIST teILCreateFromPath(LPWSTR pszPath)
{
	return teILCreateFromPath0(pszPath, FALSE);
}

BOOL teCreateItemFromPath(LPWSTR pszPath, IShellItem **ppSI)
{
	BOOL Result = FALSE;
#ifdef _2000XP
	if (lpfnSHCreateItemFromIDList) {
#endif
		LPITEMIDLIST pidl = teILCreateFromPath(const_cast<LPWSTR>(pszPath));
		if (pidl) {
			Result = SUCCEEDED(lpfnSHCreateItemFromIDList(pidl, IID_PPV_ARGS(ppSI)));
			teCoTaskMemFree(pidl);
		}
#ifdef _2000XP
	}
#endif
	return Result;
}

BOOL GetDestAndName(LPWSTR pszPath, IShellItem **ppSI, LPWSTR *ppsz)
{
	if (teCreateItemFromPath(pszPath, ppSI)) {
		*ppsz = NULL;
		return TRUE;
	}
	BOOL Result = FALSE;
	BSTR bs = ::SysAllocString(pszPath);
	if (PathRemoveFileSpec(bs)) {
		Result = teCreateItemFromPath(bs, ppSI);
		*ppsz = PathFindFileName(pszPath);
	}
	::SysFreeString(bs);
	return Result;
}

int teSHFileOperation(LPSHFILEOPSTRUCT pFOS)
{
	HRESULT hr = E_NOTIMPL;
	LPWSTR pszFrom = const_cast<LPWSTR>(pFOS->pFrom);
	if (pszFrom && !(pFOS->fFlags & FOF_WANTMAPPINGHANDLE)) {
		IFileOperation *pFO;
		if (
#ifdef _2000XP
			lpfnSHCreateItemFromIDList &&
#endif
			SUCCEEDED(teCreateInstance(CLSID_FileOperation, NULL, NULL, IID_PPV_ARGS(&pFO)))) {
			if SUCCEEDED(pFO->SetOperationFlags(pFOS->fFlags & ~FOF_MULTIDESTFILES)) {
				try {
					pFO->SetOwnerWindow(pFOS->hwnd);
					LPWSTR pszTo = const_cast<LPWSTR>(pFOS->pTo);
					IShellItem *psiFrom = NULL;
					IShellItem *psiTo = NULL;
					LPWSTR pszName = NULL;
					while (*pszFrom) {
						if (teCreateItemFromPath(pszFrom, &psiFrom)) {
							if (pFOS->wFunc == FO_DELETE) {
								hr = pFO->DeleteItem(psiFrom, NULL);
							} else if (pszTo && *pszTo) {
								if (pFOS->wFunc == FO_RENAME) {
									hr = pFO->RenameItem(psiFrom, pszTo, NULL);
								} else if (psiTo || GetDestAndName(pszTo, &psiTo, &pszName)) {
									if (pFOS->wFunc == FO_COPY) {
										hr = pFO->CopyItem(psiFrom, psiTo, pszName, NULL);
									} else if (pFOS->wFunc == FO_MOVE) {
										if (!pszName || PathFindFileName(pszFrom) - pszFrom != pszName - pszTo || StrCmpNI(pszFrom, pszTo, (int)(pszName - pszTo))) {
											hr = pFO->MoveItem(psiFrom, psiTo, pszName, NULL);
										} else {
											hr = pFO->RenameItem(psiFrom, pszName, NULL);
										}
									}
								}
								if (pFOS->fFlags & FOF_MULTIDESTFILES) {
									SafeRelease(&psiTo);
									while (*(pszTo++));
								}
							}
							psiFrom->Release();
						}
						while (*(pszFrom++));
					}
					SafeRelease(&psiTo);
					if SUCCEEDED(hr) {
						hr = pFO->PerformOperations();
						pFO->GetAnyOperationsAborted(&pFOS->fAnyOperationsAborted);
					}
				} catch (...) {
					g_nException = 0;
#ifdef _DEBUG
					g_strException = L"SHFileOperation";
#endif
				}
			}
			pFO->Release();
		}
	}
	return hr == E_NOTIMPL ? ::SHFileOperation(pFOS) : hr;
}

static void threadFileOperation(void *args)
{
	::CoInitialize(NULL);
	LPSHFILEOPSTRUCT pFO = (LPSHFILEOPSTRUCT)args;
	::InterlockedIncrement(&g_nThreads);
	try {
		teSHFileOperation(pFO);
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"threadFileOperation";
#endif
	}
	::InterlockedDecrement(&g_nThreads);
	::SysFreeString(const_cast<BSTR>(pFO->pTo));
	::SysFreeString(const_cast<BSTR>(pFO->pFrom));
	delete [] pFO;
	SetTimer(g_hwndMain, TET_EndThread, 100, teTimerProc);
	::CoUninitialize();
	::_endthread();
}

int teGetMenuString(BSTR *pbs, HMENU hMenu, UINT uIDItem, BOOL fByPosition)
{
	MENUITEMINFO mii = { sizeof(MENUITEMINFO), MIIM_STRING };
	GetMenuItemInfo(hMenu, uIDItem, fByPosition, &mii);
	if (mii.cch) {
		*pbs = SysAllocStringLen(NULL, mii.cch++);
		mii.dwTypeData = *pbs;
		GetMenuItemInfo(hMenu, uIDItem, fByPosition, &mii);
	}
	return mii.cch;
}

VOID teMenuText(LPWSTR sz)
{
	int i = 0, j = 0;
	while (sz[i]) {
		if (sz[i] == '&') {
			i++;
			continue;
		}
		if (sz[i] == '(') {
			sz[j++] = NULL;
			break;
		}
		sz[j++] = sz[i++];
	}
}

int tePos(WCHAR wc, WCHAR *sz)
{
	int i = 0;
	while (sz[i]) {
		if (sz[i] == wc) {
			return i;
		}
		i++;
	}
	return -1;
}

VOID ToMinus(BSTR *pbs)
{
	int nLen = SysStringByteLen(*pbs) + sizeof(WCHAR);
	BSTR bs = SysAllocStringByteLen(NULL, nLen);
	bs[0] = L'-';
	::CopyMemory(&bs[1], *pbs, nLen);
	::SysFreeString(*pbs);
	*pbs = bs;
}

int SizeOfvt(VARTYPE vt)
{
	switch (LOBYTE(vt)) {
		case VT_BOOL:
			return sizeof(VARIANT_BOOL);
		case VT_I1:
		case VT_UI1:
			return sizeof(CHAR);
		case VT_I2:
		case VT_UI2:
			return sizeof(SHORT);
		case VT_I4:
		case VT_UI4:
			return sizeof(LONG);
		case VT_I8:
		case VT_UI8:
			return sizeof(LONGLONG);
		case VT_PTR:
		case VT_LPWSTR:
		case VT_BSTR:
		case VT_LPSTR:
			return sizeof(HANDLE);
		case VT_R4:
			return sizeof(FLOAT);
		case VT_R8:
		case VT_FILETIME:
		case VT_DATE:
			return sizeof(double);
		case VT_VARIANT:
			return sizeof(VARIANT);
	}//end_switch
	return 0;
}

void teCopyMenu(HMENU hDest, HMENU hSrc, UINT fState)
{
	int n = GetMenuItemCount(hSrc);
	while (--n >= 0) {
		MENUITEMINFO mii = { sizeof(MENUITEMINFO), MIIM_STRING };
		GetMenuItemInfo(hSrc, n, TRUE, &mii);
		BSTR bsTypeData = SysAllocStringLen(NULL, mii.cch++);
		mii.dwTypeData = bsTypeData;
		mii.fMask  = MIIM_ID | MIIM_TYPE | MIIM_SUBMENU | MIIM_STATE;
		GetMenuItemInfo(hSrc, n, TRUE, &mii);
		HMENU hSubMenu = mii.hSubMenu;
		if (hSubMenu) {
			mii.hSubMenu = CreateMenu();
		}
		mii.fState = fState;
		InsertMenuItem(hDest, 0, TRUE, &mii);
		if (hSubMenu) {
			teCopyMenu(mii.hSubMenu, hSubMenu, fState);
		}
		::SysFreeString(bsTypeData);
	}
}

CteShellBrowser* SBfromhwnd(HWND hwnd)
{
	for (UINT i = 0; i < g_ppSB.size(); i++) {
		CteShellBrowser *pSB = g_ppSB[i];
		if (pSB->m_hwnd == hwnd || IsChild(pSB->m_hwnd, hwnd)) {
			return pSB;
		}
	}
	return NULL;
}

CteTabCtrl* TCfromhwnd(HWND hwnd)
{
	for (UINT i = 0; i < g_ppTC.size(); i++) {
		CteTabCtrl *pTC = g_ppTC[i];
		if (pTC->m_hwnd == hwnd || pTC->m_hwndStatic == hwnd || pTC->m_hwndButton == hwnd) {
			return pTC;
		}
	}
	return NULL;
}

CteTreeView* TVfromhwnd2(HWND hwnd)
{
	for (UINT i = 0; i < g_ppTV.size(); i++) {
		CteTreeView *pTV = g_ppTV[i];
		HWND hwndTV = pTV->m_hwnd;
		if (hwndTV == hwnd || IsChild(hwndTV, hwnd)) {
			return pTV->m_bMain ? pTV : NULL;
		}
	}
	return NULL;
}

CteTreeView* TVfromhwnd(HWND hwnd)
{
	for (UINT i = 0; i < g_ppSB.size(); i++) {
		CteShellBrowser *pSB = g_ppSB[i];
		HWND hwndTV = pSB->m_pTV->m_hwnd;
		if (hwndTV == hwnd || IsChild(hwndTV, hwnd)) {
			return pSB->m_pTV->m_bMain ? pSB->m_pTV : NULL;
		}
	}
	return TVfromhwnd2(hwnd);
}

void CheckChangeTabSB(HWND hwnd)
{
	if (g_pTC) {
		CteShellBrowser *pSB = SBfromhwnd(hwnd);
		if (!pSB) {
			CteTreeView *pTV = TVfromhwnd(hwnd);
			if (pTV) {
				pSB = pTV->m_pFV;
			}
		}
		if (pSB && pSB->m_pTC->m_bVisible) {
			if (g_pTC != pSB->m_pTC) {
				g_pTC = pSB->m_pTC;
				pSB->m_pTC->TabChanged(FALSE);
			}
		}
	}
}

void CheckChangeTabTC(HWND hwnd, BOOL bFocusSB)
{
	CteTabCtrl *pTC = TCfromhwnd(hwnd);
	if (pTC && pTC->m_bVisible) {
		if (g_pTC != pTC) {
			g_pTC = pTC;
			pTC->TabChanged(FALSE);
		}
		if (bFocusSB) {
			CteShellBrowser *pSB;
			pSB = pTC->GetShellBrowser(pTC->m_nIndex);
			if (pSB) {
				pSB->SetActive(TRUE);
			}
		}
	}
}

BOOL AdjustIDList(LPITEMIDLIST *ppidllist, int nCount)
{
	if (ppidllist == NULL || nCount <= 0) {
		return FALSE;
	}
	if (ppidllist[0]) {
#ifdef _2000XP
		if (g_bUpperVista || !ILIsEqual(ppidllist[0], g_pidls[CSIDL_RESULTSFOLDER])) {
			return FALSE;
		}
		for (int i = nCount; i > 1; i--) {
			teILCloneReplace(&ppidllist[i], ILCombine(ppidllist[0], ppidllist[i]));
		}
		teILFreeClear(&ppidllist[0]);
#else
		return FALSE;
#endif
	}
	ppidllist[0] = ::ILClone(ppidllist[1]);
	ILRemoveLastID(ppidllist[0]);
	int nCommon = ILGetCount(ppidllist[0]);
	int nBase = nCommon;

	for (int i = nCount; i > 1 && nCommon; i--) {
		LPITEMIDLIST pidl = ::ILClone(ppidllist[i]);
		ILRemoveLastID(pidl);
		int nLevel = ILGetCount(pidl);
		while (nLevel > nCommon) {
			ILRemoveLastID(pidl);
			nLevel--;
		}
		while (nLevel < nCommon) {
			ILRemoveLastID(ppidllist[0]);
			nCommon--;
		}
		while (nCommon > 0 && !ILIsEqual(pidl, ppidllist[0])) {
			ILRemoveLastID(pidl);
			ILRemoveLastID(ppidllist[0]);
			nCommon--;
		}
		teCoTaskMemFree(pidl);
	}

	if (nCommon) {
		for (int i = nCount; i > 0; i--) {
			LPITEMIDLIST pidl = ppidllist[i];
			for (int j = nCommon; j-- > 0;) {
				pidl = ILGetNext(pidl);
			}
			teILCloneReplace(&ppidllist[i], pidl);
		}
	}
	return nCommon == nBase;
}

LPITEMIDLIST* IDListFormDataObj(IDataObject *pDataObj, long *pnCount)
{
	LPITEMIDLIST *ppidllist = NULL;
	*pnCount = 0;
	if (pDataObj) {
		STGMEDIUM Medium;
		if (pDataObj->GetData(&IDLISTFormat, &Medium) == S_OK) {
			try {
				CIDA *pIDA = (CIDA *)GlobalLock(Medium.hGlobal);
				if (pIDA) {
					*pnCount = pIDA->cidl;
					ppidllist = new LPITEMIDLIST[*pnCount + 1];
					for (int i = *pnCount; i >= 0; i--) {
						ppidllist[i] = ::ILClone((LPCITEMIDLIST)((BYTE *)pIDA + pIDA->aoffset[i]));
					}
				}
			} catch(...) {
				if (ppidllist) {
					delete [] ppidllist;
					ppidllist = NULL;
				}
				*pnCount = 0;
			}
			GlobalUnlock(Medium.hGlobal);
			ReleaseStgMedium(&Medium);
			if (ppidllist) {
				return ppidllist;
			}
		}
		if (pDataObj->GetData(&HDROPFormat, &Medium) == S_OK) {
			*pnCount = DragQueryFile((HDROP)Medium.hGlobal, (UINT)-1, NULL, 0);
			ppidllist = new LPITEMIDLIST[*pnCount + 1];
			ppidllist[0] = NULL;
			for (int i = *pnCount; i-- > 0;) {
				BSTR bsPath;
				teDragQueryFile((HDROP)Medium.hGlobal, i, &bsPath);
				ppidllist[i + 1] = teILCreateFromPath(bsPath);
				::SysFreeString(bsPath);
			}
			ReleaseStgMedium(&Medium);
		}
	}
	return ppidllist;
}

#ifdef _USE_APIHOOK
LSTATUS APIENTRY teRegQueryValueExW(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{
	return lpfnRegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}
#endif

#ifdef _2000XP
HRESULT STDAPICALLTYPE teGetIDListFromObjectXP(IUnknown *punk, PIDLIST_ABSOLUTE *ppidl)
{
	IPersistFolder2 *pPF2;
	if SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pPF2))) {
		pPF2->GetCurFolder(ppidl);
		pPF2->Release();
		return *ppidl ? S_OK : E_FAIL;
	}
	IPersistIDList *pPI;
	if SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pPI))) {
		pPI->GetIDList(ppidl);
		pPI->Release();
		return *ppidl ? S_OK : E_FAIL;
	}
	FolderItem *pFI;
	if SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pFI))) {
		BSTR bstr;
		if SUCCEEDED(pFI->get_Path(&bstr)) {
			*ppidl = teILCreateFromPath(bstr);
			::SysFreeString(bstr);
		}
		pFI->Release();
		return *ppidl ? S_OK : E_FAIL;
	}
	return E_NOTIMPL;
}

LPITEMIDLIST teILCreateResultsXP(LPITEMIDLIST pidl)
{
	LPITEMIDLIST pidl2 = NULL;
	LPCITEMIDLIST pidlLast;
	IShellFolder *pSF;
	if SUCCEEDED(SHBindToParent(pidl, IID_PPV_ARGS(&pSF), &pidlLast)) {
		SFGAOF sfAttr = SFGAO_FILESYSTEM | SFGAO_FILESYSANCESTOR | SFGAO_STORAGE | SFGAO_STREAM;
		if (SUCCEEDED(pSF->GetAttributesOf(1, &pidlLast, &sfAttr)) &&
			(sfAttr & (SFGAO_FILESYSTEM | SFGAO_FILESYSANCESTOR | SFGAO_STORAGE | SFGAO_STREAM))) {
			UINT uSize = ILGetSize(pidl) + 28;
			pidl2 = (LPITEMIDLIST)::CoTaskMemAlloc(uSize + sizeof(USHORT));
			::ZeroMemory(pidl2, uSize + sizeof(USHORT));

			UINT uSize2 = ILGetSize(pidlLast);
			::CopyMemory(pidl2, pidlLast, uSize2);
			*(PUSHORT)pidl2 = uSize - 2;
			UINT uSize3 = uSize - uSize2 - 28;
			PBYTE p = (PBYTE)pidl2;
			*(PUSHORT)&p[uSize2 - 2] = uSize3 + 28;
			*(PDWORD)&p[uSize2 + 2] = 0xbeef0005;
			::CopyMemory(&p[uSize2 + 22], pidl, uSize3);
			*(PUSHORT)&p[uSize - 4] = uSize2 - 2;
			if (teSFIsShellFSFolder(pSF)) {
				*(PUSHORT)&p[uSize2 + 24 + uSize3] = *(PUSHORT)&p[uSize2 - 4];
			}
			STRRET strret;
			if SUCCEEDED(pSF->GetDisplayNameOf(pidl2, SHGDN_NORMAL, &strret)) {
				if (strret.uType == STRRET_WSTR) {
					teCoTaskMemFree(strret.pOleStr);
				}
			} else {
				teILFreeClear(&pidl2);
			}
		}
		pSF->Release();
	}
	return pidl2;
}

HRESULT STDAPICALLTYPE tePSPropertyKeyFromStringXP(__in LPCWSTR pszString,  __out PROPERTYKEY *pkey)
{
	if (lstrcmpi(pszString, g_szTotalFileSizeCodeXP) == 0) {
		*pkey = PKEY_TotalFileSize;
		return S_OK;
	}
	if (lstrcmpi(pszString, g_szLabelCodeXP) == 0) {
		*pkey = PKEY_Contact_Label;
		return S_OK;
	}
	HRESULT hr = E_NOTIMPL;
	ULONG chEaten = 0;
	IPropertyUI *pPUI;
	if SUCCEEDED(teCreateInstance(CLSID_PropertiesUI, NULL, NULL, IID_PPV_ARGS(&pPUI))) {
		hr = pPUI->ParsePropertyName(pszString, &pkey->fmtid, &pkey->pid, &chEaten);
		pPUI->Release();
	}
	return hr;
}

VOID teStripAmp(LPWSTR lpstr)
{
	LPWSTR lp1 = lpstr;
	WCHAR wc;
	while (wc = *lp1 = *(lpstr++)) {
		if (wc == '(') {
			*lp1 = NULL;
			return;
		}
		if (wc != '&') {
			lp1++;
		}
	}
}

#endif
#ifdef _W2000
HRESULT STDAPICALLTYPE teSHGetImageList2000(int iImageList, REFIID riid, void **ppvObj)
{
	SHFILEINFO sfi;
	UINT uFlags = 0;
	switch (iImageList) {
		case SHIL_LARGE:
			uFlags = SHGFI_SYSICONINDEX | SHGFI_LARGEICON;
			break;
		case SHIL_SMALL:
			uFlags = SHGFI_SYSICONINDEX | SHGFI_SMALLICON;
			break;
		case SHIL_SYSSMALL:
			uFlags = SHGFI_SYSICONINDEX | SHGFI_SHELLICONSIZE;
			break;
		default:
			return E_NOTIMPL;
	}//end_switch
	*(DWORD_PTR *)ppvObj = SHGetFileInfo(NULL, 0, &sfi, sizeof(sfi), uFlags);
	return S_OK;
}

#endif

HMODULE teCreateInstanceV(LPVARIANTARG pvDllFile, LPVARIANTARG pvClass, REFIID riid, PVOID *ppvObj)
{
	VARIANT v;
	CLSID clsid;
	teVariantChangeType(&v, pvClass, VT_BSTR);
	teCLSIDFromString(v.bstrVal, &clsid);
	VariantClear(&v);
	teVariantChangeType(&v, pvDllFile, VT_BSTR);
	HMODULE hDll = NULL;
	teCreateInstance(clsid, v.bstrVal, &hDll, riid, ppvObj);
	VariantClear(&v);
	return hDll;
}

BOOL teGetIDListFromObject(IUnknown *punk, LPITEMIDLIST *ppidl)
{
	*ppidl = NULL;
	if (!punk) {
		return FALSE;
	}
#ifdef _CHECK_HANDLELEAK
	if SUCCEEDED(teGetIDListFromObjectXP(punk, ppidl)) {
		return TRUE;
	}
#endif
	if SUCCEEDED(lpfnSHGetIDListFromObject(punk, ppidl)) {
		return TRUE;
	}
	IServiceProvider *pSP;
	if SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pSP))) {
		IShellBrowser *pSB;
		if SUCCEEDED(pSP->QueryService(SID_SShellBrowser, IID_PPV_ARGS(&pSB))) {
			IShellView *pSV;
			if SUCCEEDED(pSB->QueryActiveShellView(&pSV)) {
				if FAILED(lpfnSHGetIDListFromObject(pSV, ppidl)) {
#ifdef _W2000
					//Windows 2000
					IDataObject *pDataObj;
					if SUCCEEDED(pSV->GetItemObject(SVGIO_ALLVIEW, IID_PPV_ARGS(&pDataObj))) {
						long nCount;
						LPITEMIDLIST *pidllist = IDListFormDataObj(pDataObj, &nCount);
						*ppidl = pidllist[0];
						while (--nCount >= 1) {
							if (pidllist) {
								teCoTaskMemFree(pidllist[nCount]);
							}
						}
						delete [] pidllist;
						pDataObj->Release();
					}
#endif
				}
				pSV->Release();
			}
			pSB->Release();
		}
		pSP->Release();
	}
	return *ppidl != NULL;
}

BOOL teGetIDListFromObjectForFV(IUnknown *punk, LPITEMIDLIST *ppidl)
{
	*ppidl = NULL;
	if (!punk) {
		return FALSE;
	}
	CteFolderItem *pid1 = NULL;
	if SUCCEEDED(punk->QueryInterface(g_ClsIdFI, (LPVOID *)&pid1)) {
		*ppidl = ILClone(pid1->GetAlt());
		pid1->Release();
	}
	return *ppidl ? TRUE : teGetIDListFromObject(punk, ppidl);
}


BOOL teILIsEqual(IUnknown *punk1, IUnknown *punk2)
{
	BOOL bResult = FALSE;
	if (punk1 && punk2) {
		LPITEMIDLIST pidl1, pidl2;
		if (teGetIDListFromObject(punk1, &pidl1)) {
			if (teGetIDListFromObject(punk2, &pidl2)) {
				bResult = ::ILIsEqual(pidl1, pidl2);
				teCoTaskMemFree(pidl2);
				if (bResult && (::ILIsEqual(pidl1, g_pidls[CSIDL_RESULTSFOLDER]) || ILIsEmpty(pidl1))) {
					BSTR bs1, bs2;
					teGetStrFromFolderItem(&bs2, punk1);
					teGetStrFromFolderItem(&bs1, punk2);
					bResult = lstrcmpi(bs1, bs2) == 0;
				}
			}
			teCoTaskMemFree(pidl1);
		}
	}
	return bResult;
}

int GetIntFromVariant(VARIANT *pv)
{
	if (pv) {
		if (pv->vt == (VT_VARIANT | VT_BYREF)) {
			return GetIntFromVariant(pv->pvarVal);
		}
		if (pv->vt == VT_I4) {
			return pv->lVal;
		}
		if (pv->vt == VT_R8) {
			return (int)(LONGLONG)pv->dblVal;
		}
		VARIANT vo;
		VariantInit(&vo);
		if SUCCEEDED(VariantChangeType(&vo, pv, 0, VT_I4)) {
			return vo.lVal;
		}
		if SUCCEEDED(VariantChangeType(&vo, pv, 0, VT_I8)) {
			return (int)vo.llVal;
		}
		if SUCCEEDED(VariantChangeType(&vo, pv, 0, VT_R8)) {
			return (int)(LONGLONG)vo.dblVal;
		}
	}
	return 0;
}

int GetIntFromVariantClear(VARIANT *pv)
{
	int i = GetIntFromVariant(pv);
	VariantClear(pv);
	return i;
}

VOID teGetFileTimeFromItem(IShellFolder2 *pSF2, LPCITEMIDLIST pidl, const SHCOLUMNID *pscid, FILETIME *pft)
{
	VARIANT v;
	VariantInit(&v);
	SYSTEMTIME SysTime;
	if (SUCCEEDED(pSF2->GetDetailsEx(pidl, pscid, &v)) && v.vt == VT_DATE) {
		if (::VariantTimeToSystemTime(v.date, &SysTime)) {
			::SystemTimeToFileTime(&SysTime, pft);
		}
	}
	VariantClear(&v);
}

HRESULT teSHGetDataFromIDList(IShellFolder *pSF, LPCITEMIDLIST pidlPart, int nFormat, void *pv, int cb)
{
	HRESULT hr = SHGetDataFromIDList(pSF, pidlPart, nFormat, pv, cb);
	if (hr != S_OK && nFormat == SHGDFIL_FINDDATA) {
		::ZeroMemory(pv, cb);
		WIN32_FIND_DATA *pwfd = (WIN32_FIND_DATA *)pv;

		BSTR bs = NULL;
		if SUCCEEDED(teGetDisplayNameBSTR(pSF, pidlPart, SHGDN_FORADDRESSBAR | SHGDN_FORPARSING | SHGDN_INFOLDER, &bs)) {
			lstrcpyn(pwfd->cFileName, bs, MAX_PATH);
			::SysFreeString(bs);
			hr = S_OK;

			IShellFolder2 *pSF2;
			if SUCCEEDED(pSF->QueryInterface(IID_PPV_ARGS(&pSF2))) {
				VARIANT v;
				VariantInit(&v);
				if SUCCEEDED(pSF2->GetDetailsEx(pidlPart, &PKEY_FileAttributes, &v)) {
					int nAttritutes = GetIntFromVariantClear(&v);
					if (nAttritutes != -1) {
						pwfd->dwFileAttributes = nAttritutes;
					}
				}
				if SUCCEEDED(pSF2->GetDetailsEx(pidlPart, &PKEY_Size, &v)) {
					ULARGE_INTEGER uli;
					uli.QuadPart = GetLLFromVariantClear(&v);
					pwfd->nFileSizeHigh = uli.HighPart;
					pwfd->nFileSizeLow = uli.LowPart;
				}
				teGetFileTimeFromItem(pSF2, pidlPart, &PKEY_DateCreated, &pwfd->ftCreationTime);
				teGetFileTimeFromItem(pSF2, pidlPart, &PKEY_DateAccessed, &pwfd->ftLastAccessTime);
				teGetFileTimeFromItem(pSF2, pidlPart, &PKEY_DateModified, &pwfd->ftLastWriteTime);
				pSF2->Release();
			}
		}
	}
	return hr;
}

BOOL teGetIDListFromVariant(LPITEMIDLIST *ppidl, VARIANT *pv, BOOL bForceLimit = FALSE)
{
	*ppidl = NULL;
	switch(pv->vt) {
		case VT_UNKNOWN:
		case VT_DISPATCH:
			teGetIDListFromObject(pv->punkVal, ppidl);
			break;
		case VT_UNKNOWN | VT_BYREF:
		case VT_DISPATCH | VT_BYREF:
			teGetIDListFromObject(*pv->ppunkVal, ppidl);
			break;
		case VT_ARRAY | VT_I1:
		case VT_ARRAY | VT_UI1:
		case VT_ARRAY | VT_I1 | VT_BYREF:
		case VT_ARRAY | VT_UI1 | VT_BYREF:
			LONG lUBound, lLBound, nSize;
			SAFEARRAY *psa;
			PVOID pvData;
			psa = (pv->vt & VT_BYREF) ? pv->pparray[0] : pv->parray;
			if (::SafeArrayAccessData(psa, &pvData) == S_OK) {
				SafeArrayGetUBound(psa, 1, &lUBound);
				SafeArrayGetLBound(psa, 1, &lLBound);
				nSize = lUBound - lLBound + 1;
				*ppidl = (LPITEMIDLIST)::CoTaskMemAlloc(nSize);
				::CopyMemory(*ppidl, pvData, nSize);
				::SafeArrayUnaccessData(psa);
			}
			break;
		case VT_NULL:
			return FALSE;
		case VT_BSTR:
		case VT_LPWSTR:
			*ppidl = teILCreateFromPath0(pv->bstrVal, bForceLimit);
			break;
		case VT_BSTR | VT_BYREF:
		case VT_LPWSTR | VT_BYREF:
			*ppidl = teILCreateFromPath0(*pv->pbstrVal, bForceLimit);
			break;
		case VT_VARIANT | VT_BYREF:
			return teGetIDListFromVariant(ppidl, pv->pvarVal, bForceLimit);
	}
	if (*ppidl == NULL) {
		VARIANT v;
		VariantInit(&v);
		if (SUCCEEDED(VariantChangeType(&v, pv, 0, VT_I4))) {
			if (v.lVal < MAX_CSIDL2) {
				*ppidl = ::ILClone(g_pidls[v.lVal]);
			}
		}
	}
	return (*ppidl != NULL);
}

BOOL teCreateSafeArray(VARIANT *pv, PVOID pSrc, DWORD dwSize, BOOL bBSTR)
{
	if (bBSTR) {
		pv->vt = VT_BSTR;
		pv->bstrVal = ::SysAllocStringByteLen(NULL, dwSize);
		if (pSrc && pv->bstrVal) {
			::CopyMemory(pv->bstrVal, pSrc, dwSize);
		}
		return TRUE;
	} else {
		pv->parray = SafeArrayCreateVector(VT_UI1, 0, dwSize);
		if (pv->parray) {
			pv->vt = VT_ARRAY | VT_UI1;
			PVOID pvData;
			if (pSrc && ::SafeArrayAccessData(pv->parray, &pvData) == S_OK) {
				::CopyMemory(pvData, pSrc, dwSize);
				::SafeArrayUnaccessData(pv->parray);
			}
			return TRUE;
		}
	}
	pv->vt = VT_EMPTY;
	return FALSE;
}

BOOL GetVarArrayFromIDList(VARIANT *pv, LPITEMIDLIST pidl)
{
	return teCreateSafeArray(pv, pidl, ILGetSize(pidl), FALSE);
}

HRESULT GetFolderObjFromIDList(LPITEMIDLIST pidl, Folder** ppsdf)
{
	VARIANT v;
	GetVarArrayFromIDList(&v, pidl);
	IShellDispatch *psha;
	HRESULT hr = teCreateInstance(CLSID_Shell, NULL, NULL, IID_PPV_ARGS(&psha));
	if SUCCEEDED(hr) {
		hr = psha->NameSpace(v, ppsdf);
	}
	psha->Release();
	VariantClear(&v);
	return hr;
}

BOOL GetFolderItemFromIDList2(FolderItem **ppid, LPITEMIDLIST pidl)
{
	BOOL Result;
	Result = FALSE;
	*ppid = NULL;
	IPersistFolder *pPF = NULL;
	if SUCCEEDED(teCreateInstance(CLSID_FolderItem, NULL, NULL, IID_PPV_ARGS(&pPF))) {
		if SUCCEEDED(pPF->Initialize(pidl)) {
			Result = SUCCEEDED(pPF->QueryInterface(IID_PPV_ARGS(ppid)));
		}
		pPF->Release();
		return Result;
	}
#ifdef _W2000
	Folder *pFolder = NULL;
	if (GetFolderObjFromIDList(pidl, &pFolder) == S_OK) {
		Folder2 *pFolder2 = NULL;
		if SUCCEEDED(pFolder->QueryInterface(IID_PPV_ARGS(&pFolder2))) {
			pFolder2->get_Self(ppid);
			pFolder2->Release();
		}
		pFolder->Release();
		if (*ppid) {
			return TRUE;
		}
	}
	LPITEMIDLIST pidlParent = ::ILClone(pidl);
	if (ILRemoveLastID(pidlParent) && GetFolderObjFromIDList(pidlParent, &pFolder) == S_OK) {
		BSTR bs;
		if SUCCEEDED(teGetDisplayNameFromIDList(&bs, pidl, SHGDN_FORPARSING | SHGDN_INFOLDER)) {
			pFolder->ParseName(bs, ppid);
			::SysFreeString(bs);
		}
		pFolder->Release();
	}
	teCoTaskMemFree(pidlParent);
	if (*ppid) {
		return TRUE;
	}
#endif
	return FALSE;
}

BOOL GetFolderItemFromIDList(FolderItem **ppid, LPITEMIDLIST pidl)
{
	BOOL Result = FALSE;
	CteFolderItem *pPF = new CteFolderItem(NULL);
	if SUCCEEDED(pPF->Initialize(pidl)) {
		Result = SUCCEEDED(pPF->QueryInterface(IID_PPV_ARGS(ppid)));
	}
	pPF->Release();
	return Result;
}

BOOL GetFolderItemFromFolderItems(FolderItem **ppFolderItem, FolderItems *pFolderItems, int nIndex)
{
	VARIANT v;
	teSetLong(&v, nIndex);
	if (pFolderItems->Item(v, ppFolderItem) == S_OK) {
		return TRUE;
	}
	GetFolderItemFromIDList(ppFolderItem, g_pidls[CSIDL_DESKTOP]);
	return TRUE;
}

/*//
VOID GetFolderItemsFromPCZZSTR(CteFolderItems **ppFolderItems, LPCWSTR pszPath)
{
	*ppFolderItems = new CteFolderItems(NULL, NULL, TRUE);
	VARIANT v;
	VariantInit(&v);
	while (pszPath[0]) {
		v.vt = VT_BSTR;
		v.bstrVal = ::SysAllocString(pszPath);
		(*ppFolderItems)->ItemEx(-1, NULL, &v);
		VariantClear(&v);
		while (*(pszPath++));
	}
}
//*/


HRESULT GetFolderItemFromObject(FolderItem **ppid, IUnknown *pid)
{
	if (pid) {
		LPITEMIDLIST pidl;
		if (teGetIDListFromObject(pid, &pidl)) {
			GetFolderItemFromIDList(ppid, pidl);
			teCoTaskMemFree(pidl);
			return S_OK;
		}
	}
	return E_FAIL;
}

VOID teSetIDList(VARIANT *pv, LPITEMIDLIST pidl)
{
	FolderItem *pFI;
	if (GetFolderItemFromIDList(&pFI, pidl)) {
		teSetObjectRelease(pv, pFI);
	}
}

VOID teSetIDListRelease(VARIANT *pv, LPITEMIDLIST *ppidl)
{
	teSetIDList(pv, *ppidl);
	teILFreeClear(ppidl);
}

int teGetObjectLength(IDispatch *pdisp)
{
	VARIANT v;
	VariantInit(&v);
	teGetProperty(pdisp, L"length", &v);
	return GetIntFromVariantClear(&v);
}

BOOL GetDataObjFromObject(IDataObject **ppDataObj, IUnknown *punk)
{
	*ppDataObj = NULL;
	if SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(ppDataObj))) {
		return TRUE;
	}
	FolderItems *pItems;
	if SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pItems))) {
		long lCount = 0;
		pItems->get_Count(&lCount);
		if (lCount) {
			CteFolderItems *pFolderItems = new CteFolderItems(NULL, NULL);
			VARIANT v, v2;
			VariantInit(&v2);
			v.vt = VT_I4;
			for (v.lVal = 0; v.lVal < lCount; v.lVal++) {
				FolderItem *pItem;
				if SUCCEEDED(pItems->Item(v, &pItem)) {
					teSetObjectRelease(&v2, pItem);
					pFolderItems->ItemEx(-1, NULL, &v2);
					VariantClear(&v2);
				}
			}
			pFolderItems->QueryInterface(IID_PPV_ARGS(ppDataObj));
			pFolderItems->Release();
		}
		pItems->Release();
		if (*ppDataObj) {
			return TRUE;
		}
	}
	IDispatchEx *pdex;
	if SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pdex))) {
		long lCount = teGetObjectLength(pdex);
		if (lCount) {
			VARIANT v;
			VariantInit(&v);
			CteFolderItems *pFolderItems = new CteFolderItems(NULL, NULL);
			for (int i = 0; i < lCount; i++) {
				teGetPropertyAt(pdex, i, &v);
				pFolderItems->ItemEx(-1, NULL, &v);
				VariantClear(&v);
			}
			pFolderItems->QueryInterface(IID_PPV_ARGS(ppDataObj));
			pFolderItems->Release();
		}
		pdex->Release();
		if (*ppDataObj) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL GetDataObjFromVariant(IDataObject **ppDataObj, VARIANT *pv)
{
	*ppDataObj = NULL;
	IUnknown *punk;
	if (FindUnknown(pv, &punk) && GetDataObjFromObject(ppDataObj, punk)) {
		return TRUE;
	}
	LPITEMIDLIST pidl;
	pidl = NULL;
	if (teGetIDListFromVariant(&pidl, pv)) {
		IShellFolder *pSF;
		LPCITEMIDLIST pidlPart;
		if SUCCEEDED(SHBindToParent(pidl, IID_PPV_ARGS(&pSF), &pidlPart)) {
			pSF->GetUIObjectOf(g_hwndMain, 1, &pidlPart, IID_IDataObject, NULL, (LPVOID*)ppDataObj);
			pSF->Release();
		}
		teCoTaskMemFree(pidl);
	}
	return *ppDataObj != NULL;
}

#ifdef _USE_BSEARCHAPI
int* teSortDispatchApi(TEDispatchApi *method, int nCount)
{
	int *pi = new int[nCount];

	for (int j = 0; j < nCount; j++) {
		LPSTR pszNameA = method[j].name;
		int nMin = 0;
		int nMax = j - 1;
		int nIndex;
		while (nMin <= nMax) {
			nIndex = (nMin + nMax) / 2;
			if (lstrcmpiA(pszNameA, method[pi[nIndex]].name) < 0) {
				nMax = nIndex - 1;
			} else {
				nMin = nIndex + 1;
			}
		}
		for (int i = j; i > nMin; i--) {
			pi[i] = pi[i - 1];
		}
		pi[nMin] = j;
	}
	return pi;
}

int teBSearchApi(TEDispatchApi *method, int nSize, int* pMap, LPOLESTR bs)
{
	int nMin = 0;
	int nMax = nSize - 1;
	int nIndex, nCC;
	CHAR pszNameA[32];
	WideCharToMultiByte(CP_TE, 0, (LPCWSTR)bs, -1, pszNameA, sizeof(pszNameA) - 1, NULL, NULL);

	while (nMin <= nMax) {
		nIndex = (nMin + nMax) / 2;
		nCC = lstrcmpiA(pszNameA, method[pMap[nIndex]].name);
		if (nCC < 0) {
			nMax = nIndex - 1;
			continue;
		}
		if (nCC > 0) {
			nMin = nIndex + 1;
			continue;
		}
		return pMap[nIndex];
	}
	return -1;
}
#endif

int* SortTEMethod(TEmethod *method, int nCount)
{
	int *pi = new int[nCount];

	for (int j = 0; j < nCount; j++) {
		LPSTR pszNameA = method[j].name;
		int nMin = 0;
		int nMax = j - 1;
		int nIndex;
		while (nMin <= nMax) {
			nIndex = (nMin + nMax) / 2;
			if (lstrcmpiA(pszNameA, method[pi[nIndex]].name) < 0) {
				nMax = nIndex - 1;
			} else {
				nMin = nIndex + 1;
			}
		}
		for (int i = j; i > nMin; i--) {
			pi[i] = pi[i - 1];
		}
		pi[nMin] = j;
	}
	return pi;
}

int teBSearch(TEmethod *method, int nSize, int* pMap, LPOLESTR bs)
{
	int nMin = 0;
	int nMax = nSize - 1;
	int nIndex, nCC;
	CHAR pszNameA[32];
	WideCharToMultiByte(CP_TE, 0, (LPCWSTR)bs, -1, pszNameA, sizeof(pszNameA) - 1, NULL, NULL);

	while (nMin <= nMax) {
		nIndex = (nMin + nMax) / 2;
		nCC = lstrcmpiA(pszNameA, method[pMap[nIndex]].name);
		if (nCC < 0) {
			nMax = nIndex - 1;
			continue;
		}
		if (nCC > 0) {
			nMin = nIndex + 1;
			continue;
		}
		return pMap[nIndex];
	}
	return -1;
}

int* SortTEStruct(TEStruct *method, int nCount)
{
	int *pi = new int[nCount];

	for (int j = 0; j < nCount; j++) {
		LPSTR pszNameA = method[j].name;
		int nMin = 0;
		int nMax = j - 1;
		int nIndex;
		while (nMin <= nMax) {
			nIndex = (nMin + nMax) / 2;
			if (lstrcmpiA(pszNameA, method[pi[nIndex]].name) < 0) {
				nMax = nIndex - 1;
			} else {
				nMin = nIndex + 1;
			}
		}
		for (int i = j; i > nMin; i--) {
			pi[i] = pi[i - 1];
		}
		pi[nMin] = j;
	}
	return pi;
}

int teBSearchStruct(TEStruct *method, int nSize, int* pMap, LPOLESTR bs)
{
	int nMin = 0;
	int nMax = nSize - 1;
	int nIndex, nCC;
	CHAR pszNameA[32];
	WideCharToMultiByte(CP_TE, 0, (LPCWSTR)bs, -1, pszNameA, sizeof(pszNameA) - 1, NULL, NULL);

	while (nMin <= nMax) {
		nIndex = (nMin + nMax) / 2;
		nCC = lstrcmpiA(pszNameA, method[pMap[nIndex]].name);
		if (nCC < 0) {
			nMax = nIndex - 1;
			continue;
		}
		if (nCC > 0) {
			nMin = nIndex + 1;
			continue;
		}
		return pMap[nIndex];
	}
	return -1;
}

int* SortTEMethodW(TEmethodW *method, int nCount)
{
	int *pi = new int[nCount];

	for (int j = 0; j < nCount; j++) {
		BSTR bs = method[j].name;
		int nMin = 0;
		int nMax = j - 1;
		int nIndex;
		while (nMin <= nMax) {
			nIndex = (nMin + nMax) / 2;
			if (lstrcmpi(bs, method[pi[nIndex]].name) < 0) {
				nMax = nIndex - 1;
			} else {
				nMin = nIndex + 1;
			}
		}
		for (int i = j; i > nMin; i--) {
			pi[i] = pi[i - 1];
		}
		pi[nMin] = j;
	}
	return pi;
}

int teBSearchW(TEmethodW *method, int nSize, int* pMap, LPOLESTR bs)
{
	int nMin = 0;
	int nMax = nSize - 1;
	int nIndex, nCC;

	while (nMin <= nMax) {
		nIndex = (nMin + nMax) / 2;
		nCC = lstrcmpi(bs, method[pMap[nIndex]].name);
		if (nCC < 0) {
			nMax = nIndex - 1;
			continue;
		}
		if (nCC > 0) {
			nMin = nIndex + 1;
			continue;
		}
		return pMap[nIndex];
	}
	return -1;
}

HRESULT teGetDispId(TEmethod *method, int nCount, int* pMap, LPOLESTR bs, DISPID *rgDispId, BOOL bNum)
{
	if (pMap) {
		int nIndex = teBSearch(method, nCount, pMap, bs);
		if (nIndex >= 0) {
			*rgDispId = method[nIndex].id;
			return S_OK;
		}
	} else {
		CHAR pszNameA[32];
		WideCharToMultiByte(CP_TE, 0, (LPCWSTR)bs, -1, pszNameA, sizeof(pszNameA) - 1, NULL, NULL);
		for (int i = 0; method[i].name; i++) {
			if (lstrcmpiA(pszNameA, method[i].name) == 0) {
				*rgDispId = method[i].id;
				return S_OK;
			}
		}
	}
	if (bNum) {
		VARIANT v, vo;
		teSetSZ(&v, bs);
		VariantInit(&vo);
		if (SUCCEEDED(VariantChangeType(&vo, &v, 0, VT_I4))) {
			*rgDispId = vo.lVal + DISPID_COLLECTION_MIN;
			VariantClear(&vo);
			VariantClear(&v);
			return *rgDispId < DISPID_TE_MAX ? S_OK : S_FALSE;
		}
		VariantClear(&v);
	}
	return DISP_E_UNKNOWNNAME;
}

HRESULT teGetMemberName(DISPID id, BSTR *pbstrName)
{
	if (id >= DISPID_COLLECTION_MIN && id <= DISPID_TE_MAX) {
		wchar_t pszName[8];
		swprintf_s(pszName, 8, L"%d", id - DISPID_COLLECTION_MIN);
		*pbstrName = ::SysAllocString(pszName);
		return S_OK;
	}
	return E_NOTIMPL;
}

int GetSizeOfStruct(LPOLESTR bs)
{
	int nIndex = teBSearchStruct(pTEStructs, _countof(pTEStructs), g_maps[MAP_SS], bs);
	if (nIndex >= 0) {
		return pTEStructs[nIndex].lSize;
	}
	return 0;
}

int GetSizeOf(VARIANT *pv)
{
	if (pv->vt == VT_BSTR) {
		return GetSizeOfStruct(pv->bstrVal);
	}
	int i = SizeOfvt(static_cast<VARTYPE>(GetIntFromVariant(pv)));
	if (i) {
		return i;
	}
	if (pv->vt & VT_ARRAY) {
		return pv->parray->rgsabound[0].cElements * SizeOfvt(pv->vt);
	}
	return SizeOfvt(pv->vt);
}

//Left-Bottom "%" is 100 times, on the other 0x4000 times
int GetIntFromVariantPP(VARIANT *pv, int nIndex)
{
	if (pv->vt == (VT_VARIANT | VT_BYREF)) {
		return GetIntFromVariantPP(pv->pvarVal, nIndex);
	}
	if (nIndex >= TE_Left && nIndex <= TE_Bottom) {
		int i = 0;
		if (pv->vt == VT_BSTR) {
			if (tePathMatchSpec(pv->bstrVal, L"*%")) {
				float f = 0;
				if (swscanf_s(pv->bstrVal, L"%g%%", &f) != EOF) {
					i = (int)(f * 100);
					if (i > 10000) {
						i = 10000;	//Not exceed 100%
					}
				}
			}
		}
		if (i == 0) {
			return GetIntFromVariant(pv) * 0x4000;
		}
		return i;
	} else {
		return GetIntFromVariant(pv);
	}
}

//There is no need for SysFreeString
BSTR GetLPWSTRFromVariant(VARIANT *pv)
{
	switch (pv->vt) {
		case VT_BSTR:
		case VT_LPWSTR:
			return pv->bstrVal;
		case VT_VARIANT | VT_BYREF:
			return GetLPWSTRFromVariant(pv->pvarVal);
		default:
			return (BSTR)GetPtrFromVariant(pv);
	}//end_switch
}

VOID GetpDataFromVariant(UCHAR **ppc, int *pnLen, VARIANT *pv)
{
	if (pv->vt == (VT_VARIANT | VT_BYREF)) {
		return GetpDataFromVariant(ppc, pnLen, pv->pvarVal);
	}
	if (pv->vt & VT_ARRAY) {
		*ppc = (UCHAR *)pv->parray->pvData;
		*pnLen = pv->parray->rgsabound[0].cElements * SizeOfvt(pv->vt);
		return;
	}
	char *pc = GetpcFromVariant(pv, NULL);
	if (pc) {
		IDispatch *pdisp;
		if (GetDispatch(pv, &pdisp)) {
			VARIANT v;
			VariantInit(&v);
			if (teGetProperty(pdisp, L"Size", &v) == S_OK) {
				*ppc = (UCHAR *)pc;
				*pnLen = GetIntFromVariantClear(&v);
			}
			pdisp->Release();
			return;
		}
	}
	*ppc = (UCHAR *)GetLPWSTRFromVariant(pv);
	if (pv->vt == VT_BSTR) {
		*pnLen = ::SysStringByteLen(pv->bstrVal);
	}
}

VOID Invoke4(IDispatch *pdisp, VARIANT *pvResult, int nArgs, VARIANTARG *pvArgs)
{
	Invoke5(pdisp, DISPID_VALUE, DISPATCH_METHOD, pvResult, nArgs, pvArgs);
}

VOID teCustomDraw(int nFunc, CteShellBrowser *pSB, CteTreeView *pTV, IShellItem *psi, LPNMCUSTOMDRAW lpnmcd, PVOID pvcd, LRESULT *plres)
{
	if (lpnmcd->rc.top == 0 && lpnmcd->rc.bottom == 0) {
#ifdef _2000XP
		if (g_bUpperVista) {
			return;
		}
#else
		return;
#endif
	}
	LPITEMIDLIST pidl = NULL;
	PVOID pvcd2 = NULL;
	if (nFunc == TE_OnItemPrePaint && g_pOnFunc[TE_OnItemPostPaint]) {
		*plres = CDRF_NOTIFYPOSTPAINT;
	}
	if (g_pOnFunc[nFunc]) {
		VARIANTARG *pv = GetNewVARIANT(5);
		if (pSB) {
			teSetObject(&pv[4], pSB);
			pvcd2 = new CteMemory(sizeof(NMLVCUSTOMDRAW), pvcd, 1, L"NMLVCUSTOMDRAW");
			IFolderView *pFV;
			if SUCCEEDED(pSB->m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV))) {
				LPITEMIDLIST pidlChild;
				if SUCCEEDED(pFV->Item((int)lpnmcd->dwItemSpec, &pidlChild)) {
					pidl = ILCombine(pSB->m_pidl, pidlChild);
					teCoTaskMemFree(pidlChild);
				}
				pFV->Release();
			}
		} else {
			teSetObject(&pv[4], pTV);
			pvcd2 = new CteMemory(sizeof(NMTVCUSTOMDRAW), pvcd, 1, L"NMTVCUSTOMDRAW");
			if (psi && lpnmcd->rc.bottom) {
				teGetIDListFromObject(psi, &pidl);
				TVHITTESTINFO ht = { 0 };
				ht.pt.x = (lpnmcd->rc.left + lpnmcd->rc.right) / 2;
				ht.pt.y = (lpnmcd->rc.top + lpnmcd->rc.bottom) / 2;
				TreeView_HitTest(pTV->m_hwndTV, &ht);
				lpnmcd->dwItemSpec = (DWORD_PTR)ht.hItem;
			}
#ifdef _2000XP
			if (!pidl && pTV->m_pShellNameSpace && (lpnmcd->uItemState & CDIS_SELECTED)) {
				IDispatch *pid;
				if (pTV->m_pShellNameSpace->get_SelectedItem(&pid) == S_OK) {
					teGetIDListFromObject(pid, &pidl);
					pid->Release();
				}
			}
#endif
		}
		if (pidl) {
			teSetIDListRelease(&pv[3], &pidl);
			teSetObjectRelease(&pv[2], new CteMemory(sizeof(NMCUSTOMDRAW), lpnmcd, 1, L"NMCUSTOMDRAW"));
			teSetObjectRelease(&pv[1], pvcd2);
			teSetObjectRelease(&pv[0], new CteMemory(sizeof(HANDLE), plres, 1, L"HANDLE"));
			Invoke4(g_pOnFunc[nFunc], NULL, 5, pv);
		} else {
			VariantClear(&pv[4]);
			SafeRelease(&pvcd2);
			delete [] pv;
		}
	}
}

#ifdef _LOG
VOID teLog(HANDLE hFile, LPWSTR lpLog)
{
	DWORD dwWriteByte;
	WriteFile(hFile, lpLog, lstrlen(lpLog) * 2, &dwWriteByte, NULL);
}
#endif

BOOL teFreeLibrary(HMODULE hDll, UINT uElpase)
{
	if (hDll) {
		if (uElpase) {
			g_pFreeLibrary.push_back(hDll);
			SetTimer(g_hwndMain, TET_FreeLibrary, uElpase, teTimerProc);
			return TRUE;
		}
		return FreeLibrary(hDll);
	}
	return FALSE;
}

HRESULT ParseScript(LPOLESTR lpScript, LPOLESTR lpLang, VARIANT *pv, IDispatch **ppdisp, EXCEPINFO *pExcepInfo)
{
	HRESULT hr = E_FAIL;
	CLSID clsid;
	IActiveScript *pas = NULL;

	if (tePathMatchSpec(lpLang, L"J*Script")) {
		teCreateInstance(CLSID_JScriptChakra, NULL, NULL, IID_PPV_ARGS(&pas));
	}
	if (pas == NULL && teCLSIDFromProgID(lpLang, &clsid) == S_OK) {
		teCreateInstance(clsid, NULL, NULL, IID_PPV_ARGS(&pas));
	}
	if (pas) {
		IActiveScriptProperty *paspr;
		if SUCCEEDED(pas->QueryInterface(IID_PPV_ARGS(&paspr))) {
			VARIANT v;
			teSetLong(&v, 0);
			while (++v.lVal <= 256 && paspr->SetProperty(SCRIPTPROP_INVOKEVERSIONING, NULL, &v) == S_OK) {
			}
		}
		IUnknown *punk = NULL;
		FindUnknown(pv, &punk);
		CteActiveScriptSite *pass = new CteActiveScriptSite(punk, pExcepInfo, &hr);
		pas->SetScriptSite(pass);
		pass->Release();
		IActiveScriptParse *pasp;
		if SUCCEEDED(pas->QueryInterface(IID_PPV_ARGS(&pasp))) {
			hr = pasp->InitNew();
			if (punk) {
				IDispatchEx *pdex;
				if SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pdex))) {
					DISPID dispid;
					HRESULT hr2 = pdex->GetNextDispID(fdexEnumAll, DISPID_STARTENUM, &dispid);
					while (hr2 == S_OK) {
						BSTR bs;
						if (pdex->GetMemberName(dispid, &bs) == S_OK) {
							pas->AddNamedItem(bs, SCRIPTITEM_ISVISIBLE | SCRIPTITEM_ISSOURCE | SCRIPTITEM_GLOBALMEMBERS);
							::SysFreeString(bs);
						}
						hr2 = pdex->GetNextDispID(fdexEnumAll, dispid, &dispid);
					}
					pdex->Release();
				}
			} else if (pv && pv->boolVal) {
				if (g_pWebBrowser && g_dwMainThreadId == GetCurrentThreadId()) {
					pas->AddNamedItem(L"window", SCRIPTITEM_ISVISIBLE | SCRIPTITEM_ISSOURCE | SCRIPTITEM_GLOBALMEMBERS);
				}
			}
			if (g_dwMainThreadId != GetCurrentThreadId()) {
				pas->AddNamedItem(L"api", SCRIPTITEM_ISVISIBLE | SCRIPTITEM_ISSOURCE);
			}
			VARIANT v;
			VariantInit(&v);
			pasp->ParseScriptText(lpScript, NULL, NULL, NULL, 0, 1, SCRIPTTEXT_ISPERSISTENT | SCRIPTTEXT_ISVISIBLE, &v, NULL);
			if (hr == S_OK) {
				pas->SetScriptState(SCRIPTSTATE_CONNECTED);
				if (ppdisp) {
					hr = E_FAIL;
					IDispatch *pdisp;
					if (pas->GetScriptDispatch(NULL, &pdisp) == S_OK) {
						CteDispatch *odisp = new CteDispatch(pdisp, 2);
						pdisp->Release();
						if SUCCEEDED(pas->QueryInterface(IID_PPV_ARGS(&odisp->m_pActiveScript))) {
							hr = odisp->QueryInterface(IID_PPV_ARGS(ppdisp));
						}
						odisp->Release();
					}
				}
			}
			pasp->Release();
		}
		if (!ppdisp || !*ppdisp) {
			pas->SetScriptState(SCRIPTSTATE_CLOSED);
			pas->Close();
		}
		pas->Release();
	}
	return hr;
}

VOID GetPointFormVariant(POINT *ppt, VARIANT *pv)
{
	IDispatch *pdisp;
	if (GetDispatch(pv, &pdisp)) {
		VARIANT v;
		VariantInit(&v);
		teGetProperty(pdisp, L"x", &v);
		ppt->x = GetIntFromVariantClear(&v);
		teGetProperty(pdisp, L"y", &v);
		ppt->y = GetIntFromVariantClear(&v);
		pdisp->Release();
		return;
	}
	int npt = GetIntFromVariant(pv);
	ppt->x = GET_X_LPARAM(npt);
	ppt->y = GET_Y_LPARAM(npt);
}

HRESULT DoFunc1(int nFunc, PVOID pObj, VARIANT *pVarResult)
{
	if (g_pOnFunc[nFunc]) {
		VARIANTARG *pv = GetNewVARIANT(1);
		teSetObject(&pv[0], pObj);
		Invoke4(g_pOnFunc[nFunc], pVarResult, 1, pv);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT DoFunc(int nFunc, PVOID pObj, HRESULT hr)
{
	VARIANT vResult;
	VariantInit(&vResult);
	if (SUCCEEDED(DoFunc1(nFunc, pObj, &vResult)) && vResult.vt != VT_EMPTY) {
		hr = GetIntFromVariantClear(&vResult);
	}
	return hr;
}

VOID teSetProgress(IProgressDialog *ppd, int nCurrent, int nCount)
{
	WCHAR pszMsg[8];
	ppd->SetProgress(nCurrent, nCount);
	swprintf_s(pszMsg, 8, L"%d%%", nCurrent * 100 / nCount);
	ppd->SetTitle(pszMsg);
}

#ifdef _USE_THREADADDITEMS
static void threadAddItems(void *args)
{
	::OleInitialize(NULL);
	WCHAR pszMsg[MAX_PATH];
	LPITEMIDLIST pidl;
	IProgressDialog *ppd = NULL;

	TEAddItems *pAddItems = (TEAddItems *)args;
	IDispatch *pSB;
	CoGetInterfaceAndReleaseStream(pAddItems->pStrmSB, IID_PPV_ARGS(&pSB));
	IDispatch *pArray;
	CoGetInterfaceAndReleaseStream(pAddItems->pStrmArray, IID_PPV_ARGS(&pArray));
	IDispatch *pOnCompleted = NULL;
	if (pAddItems->pStrmOnCompleted) {
		CoGetInterfaceAndReleaseStream(pAddItems->pStrmOnCompleted, IID_PPV_ARGS(&pOnCompleted));
	}
	teCreateInstance(CLSID_ProgressDialog, NULL, NULL, IID_PPV_ARGS(&ppd));
	if (ppd) {
		try {
			ppd->StartProgressDialog(g_hwndMain, NULL, PROGDLG_NORMAL | PROGDLG_AUTOTIME, NULL);
#ifdef _2000XP
			ppd->SetAnimation(g_hShell32, 150);
#endif
			if (!LoadString(g_hShell32, 13585, pszMsg, MAX_PATH)) {
				LoadString(g_hShell32, 6478, pszMsg, MAX_PATH);
			}
			ppd->SetLine(1, pszMsg, TRUE, NULL);

			FolderItems *pItems;
			if SUCCEEDED(pArray->QueryInterface(IID_PPV_ARGS(&pItems))) {
				long lCount = 0;
				pItems->get_Count(&lCount);
				if (lCount) {
					VARIANT v;
					v.vt = VT_I4;
					for (v.lVal = 0; v.lVal < lCount && !ppd->HasUserCancelled(); v.lVal++) {
						teSetProgress(ppd, v.lVal, lCount);
						FolderItem *pItem;
						if SUCCEEDED(pItems->Item(v, &pItem)) {
							BSTR bs;
							if SUCCEEDED(pItem->get_Name(&bs)) {
								ppd->SetLine(2, bs, TRUE, NULL);
								teSysFreeString(&bs);
							}
							BOOL bAdd = TRUE;
							if (!pAddItems->bDeleted) {
								CteFolderItem *pid;
								if SUCCEEDED(pItem->QueryInterface(g_ClsIdFI, (LPVOID *)&pid)) {
									bAdd = !pid->m_dwUnavailable;
									pid->Release();
								}
							}
							if (bAdd) {
								VariantClear(&pAddItems->pv[1]);
								teSetObjectRelease(&pAddItems->pv[1], pItem);
								if (Invoke5(pSB, 0x10000501, DISPATCH_METHOD, NULL, -2, pAddItems->pv) == E_ACCESSDENIED) {
									break;
								}
							} else {
								SafeRelease(&pItem);
							}
						}
					}
					VariantClear(&pAddItems->pv[1]);
				}
				pItems->Release();
			} else {
				int nCount = teGetObjectLength(pArray);
				for (int nCurrent = 0; nCurrent < nCount && !ppd->HasUserCancelled(); nCurrent++) {
					teSetProgress(ppd, nCurrent, nCount);
					if (SUCCEEDED(teGetPropertyAt(pArray, nCurrent, &pAddItems->pv[1])) && pAddItems->pv[1].vt != VT_EMPTY) {
						if (!teGetIDListFromVariant(&pidl, &pAddItems->pv[1], TRUE) && pAddItems->bDeleted && pAddItems->pv[1].vt == VT_BSTR) {
							pidl = teSHSimpleIDListFromPath(pAddItems->pv[1].bstrVal, FALSE, FILE_ATTRIBUTE_HIDDEN);
						}
						if (pidl) {
							if (SHGetPathFromIDList(pidl, pszMsg)) {
								ppd->SetLine(2, PathFindFileName(pszMsg), TRUE, NULL);
							}
							VariantClear(&pAddItems->pv[1]);
							teSetIDListRelease(&pAddItems->pv[1], &pidl);
							if (Invoke5(pSB, 0x10000501, DISPATCH_METHOD, NULL, -2, pAddItems->pv) == E_ACCESSDENIED) {
								break;
							}
						}
						VariantClear(&pAddItems->pv[1]);
					}
				}
			}
			teSetProgress(ppd, 100, 100);
			ppd->SetLine(2, L"", TRUE, NULL);

			if (pAddItems->bNavigateComplete) {
				Invoke5(pSB, 0x10000500, DISPATCH_METHOD, NULL, 0, NULL);
			}
			if (pOnCompleted) {
				VARIANT *pv = GetNewVARIANT(3);
				teSetObject(&pv[2], pSB);
				teSetObject(&pv[1], pArray);
				teSetObjectRelease(&pv[0], new CteProgressDialog(ppd));
				Invoke4(pOnCompleted, NULL, 3, pv);
			}
			SetTimer(g_hwndMain, TET_Status, 500, teTimerProc);
		} catch (...) {
			g_nException = 0;
#ifdef _DEBUG
			g_strException = L"threadAddItems";
#endif
		}
	}
	teClearVariantArgs(2, pAddItems->pv);
	delete [] pAddItems;
	SafeRelease(&pOnCompleted);
	SafeRelease(&pArray);
	SafeRelease(&pSB);
	if (ppd) {
		ppd->SetLine(2, L"", TRUE, NULL);
		ppd->StopProgressDialog();
		SafeRelease(&ppd);
	}
	::OleUninitialize();
	::_endthread();
}
#endif

BOOL teILGetParent(FolderItem *pid, FolderItem **ppid)
{
	BOOL bResult =FALSE;
	VARIANT v;
	VariantInit(&v);
	if SUCCEEDED(DoFunc1(TE_OnILGetParent, pid, &v)) {
		if (v.vt != VT_EMPTY) {
			GetFolderItemFromVariant(ppid, &v);
			return TRUE;
		}
	}
	LPITEMIDLIST pidl = NULL;
	if (teGetIDListFromObject(pid, &pidl)) {
		bResult = ::ILRemoveLastID(pidl);
		if (::ILIsEqual(pidl, g_pidls[CSIDL_INTERNET])) {
			bResult = ::ILRemoveLastID(pidl);
		}
		if (!bResult || ILIsEmpty(pidl)) {
			BSTR bs;
			if SUCCEEDED(pid->get_Path(&bs)) {
				if (tePathMatchSpec(bs, L"?:\\*")) {
					if (PathRemoveFileSpec(bs)) {
						CteFolderItem *pid1 = new CteFolderItem(NULL);
						pid1->m_v.vt = VT_BSTR;
						pid1->m_v.bstrVal = bs;
						pid1->QueryInterface(IID_PPV_ARGS(ppid));
						pid1->Release();
						teCoTaskMemFree(pidl);
						return TRUE;
					}
				}
				::SysFreeString(bs);
			}
		}
		GetFolderItemFromIDList(ppid, pidl);
		teCoTaskMemFree(pidl);
	}
	return bResult;
}

VOID GetNewObject1(LPOLESTR sz, IDispatch **ppObj)
{
	VARIANT v;
	VariantInit(&v);
	SafeRelease(ppObj);
	if (g_dwMainThreadId == GetCurrentThreadId()) {
		if (teExecMethod(g_pJS, sz, &v, 0, NULL) == S_OK) {
			GetDispatch(&v, ppObj);
			VariantClear(&v);
			return;
		}
	}
	IDispatch *pJS;
	if SUCCEEDED(CoGetInterfaceAndReleaseStream(g_pStrmJS, IID_PPV_ARGS(&pJS))) {
		if SUCCEEDED(CoMarshalInterThreadInterfaceInStream(IID_IDispatch, pJS, &g_pStrmJS)) {
			if (teExecMethod(pJS, sz, &v, 0, NULL) == S_OK) {
				GetDispatch(&v, ppObj);
				VariantClear(&v);
				SafeRelease(&pJS);
				return;
			}
		}
		pJS->Release();
	}
#ifdef _DEBUG
	MessageBox(g_hwndMain, L"Failed to get object.", NULL, MB_ICONERROR | MB_OK);
#endif
}

VOID GetNewArray(IDispatch **ppArray)
{
	GetNewObject1(L"_a", ppArray);
}

VOID GetNewObject(IDispatch **ppObj)
{
	GetNewObject1(L"_o", ppObj);
}

VOID ClearEvents()
{
	g_dwSessionId++;
	for (int j = Count_OnFunc; j-- > 1;) {
		SafeRelease(&g_pOnFunc[j]);
	}

	for (UINT i = 0; i < g_ppSB.size(); i++) {
		CteShellBrowser *pSB = g_ppSB[i];
		for (int j = Count_SBFunc; j-- > 1;) {
			SafeRelease(&pSB->m_ppDispatch[j]);
		}
	}
	g_param[TE_Tab] = TRUE;
	EnableWindow(g_pWebBrowser->get_HWND(), TRUE);
	SetWindowPos(g_hwndMain, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

VOID teArrayPush(IDispatch *pdisp, PVOID pObj)
{
	VARIANT *pv = GetNewVARIANT(1);
	teSetObject(pv, pObj);
	teExecMethod(pdisp, L"push", NULL, 1, pv);
}

HRESULT DoStatusText(PVOID pObj, LPCWSTR lpszStatusText, int iPart)
{
	HRESULT hr = E_NOTIMPL;
	if (g_pOnFunc[TE_OnStatusText]) {
		VARIANT vResult;
		VariantInit(&vResult);
		VARIANTARG *pv = GetNewVARIANT(3);
		teSetObject(&pv[2], pObj);
		teSetSZ(&pv[1], lpszStatusText);
		teSetLong(&pv[0], iPart);
		Invoke4(g_pOnFunc[TE_OnStatusText], &vResult, 3, pv);
		hr = GetIntFromVariantClear(&vResult);
	}
	g_szStatus[0] = NULL;
	return hr;
}

LONG_PTR DoHitTest(PVOID pObj, POINT pt, UINT flags)
{
	if (g_pOnFunc[TE_OnHitTest]) {
		VARIANT vResult;
		VariantInit(&vResult);
		VARIANTARG *pv = GetNewVARIANT(3);
		teSetObject(&pv[2], pObj);
		CteMemory *pstPt = new CteMemory(2 * sizeof(int), NULL, 1, L"POINT");
		pstPt->SetPoint(pt.x, pt.y);
		teSetObject(&pv[1], pstPt);
		pstPt->Release();
		teSetLong(&pv[0], flags);
		Invoke4(g_pOnFunc[TE_OnHitTest], &vResult, 3, pv);
		if (vResult.vt != VT_EMPTY) {
			return GetPtrFromVariantClear(&vResult);
		}
	}
	return -1;
}

HRESULT DragSub(int nFunc, PVOID pObj, CteFolderItems *pDragItems, PDWORD pgrfKeyState, POINTL pt, PDWORD pdwEffect)
{
	HRESULT hr = E_FAIL;
	VARIANT vResult;
	VARIANTARG *pv;
	CteMemory *pstPt;

	if (g_pOnFunc[nFunc]) {
		try {
			if (InterlockedIncrement(&g_nProcDrag) < 10) {
				pv = GetNewVARIANT(5);
				teSetObject(&pv[4], pObj);
				teSetObject(&pv[3], pDragItems);
				teSetObjectRelease(&pv[2], new CteMemory(sizeof(int), pgrfKeyState, 1, L"DWORD"));
				pstPt = new CteMemory(2 * sizeof(int), NULL, 1, L"POINT");
				pstPt->SetPoint(pt.x, pt.y);
				teSetObjectRelease(&pv[1], pstPt);
				teSetObjectRelease(&pv[0], new CteMemory(sizeof(int), pdwEffect, 1, L"DWORD"));
				Invoke4(g_pOnFunc[nFunc], &vResult, 5, pv);
				hr = GetIntFromVariant(&vResult);
			}
		} catch(...) {
			g_nException = 0;
#ifdef _DEBUG
			g_strException = L"DragSub";
#endif
		}
		::InterlockedDecrement(&g_nProcDrag);
	}
	return hr;
}

HRESULT DragLeaveSub(PVOID pObj, CteFolderItems **ppDragItems)
{
	if (ppDragItems && *ppDragItems) {
		CteFolderItems *pDragItems = *ppDragItems;
		pDragItems->Release();
		*ppDragItems = NULL;
	}
	return DoFunc(TE_OnDragLeave, pObj, E_NOTIMPL);
}

BOOL MessageSub(int nFunc, PVOID pObj, MSG *pMsg, HRESULT *phr)
{
	VARIANT vResult;
	VARIANTARG *pv;

	if (g_pOnFunc[nFunc]) {
		pv = GetNewVARIANT(5);
		teSetObject(&pv[4], pObj);
		teSetPtr(&pv[3], pMsg->hwnd);
		teSetLong(&pv[2], pMsg->message);
		teSetPtr(&pv[1], pMsg->wParam);
		teSetPtr(&pv[0], pMsg->lParam);
		Invoke4(g_pOnFunc[nFunc], &vResult, 5, pv);
		if (vResult.vt != VT_EMPTY) {
			if (phr) {
				*phr = GetIntFromVariant(&vResult);
			}
			VariantClear(&vResult);
			return TRUE;
		}
	}
	return FALSE;
}

HRESULT teDoCommand(PVOID pObj, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (g_pOnFunc[TE_OnCommand]) {
		MSG msg1;
		msg1.hwnd = hwnd;
		msg1.message = msg;
		msg1.wParam = wParam;
		msg1.lParam = lParam;
		HRESULT hr;
		if (MessageSub(TE_OnCommand, pObj, &msg1, &hr)) {
			return hr;
		}
	}
	return S_FALSE;
}

HRESULT tePSFormatForDisplay(PROPERTYKEY *ppropKey, VARIANT *pv, DWORD pdfFlags, LPWSTR *ppszDisplay)
{
	HRESULT hr = E_FAIL;
	*ppszDisplay = NULL;
	if (g_bsDateTimeFormat && pv->vt == VT_DATE) {
		SYSTEMTIME st;
		if (teVariantTimeToSystemTime(pv->date, &st)) {
			LPWSTR pszDisplay = (LPWSTR)::CoTaskMemAlloc(MAX_PROP);
			pszDisplay[0] = NULL;
			*ppszDisplay = pszDisplay;
			BOOL bDate = TRUE;
			WCHAR wc = 0xffff;
			for (int i = 0, j = 0; wc; i++) {
				wc = g_bsDateTimeFormat[i];
				if (bDate) {
					if (!wc || StrChr(L"hHmst", wc)) {
						if (i != j) {
							g_bsDateTimeFormat[i] = NULL;
							int k = lstrlen(*ppszDisplay);
							GetDateFormat(LOCALE_USER_DEFAULT, 0, &st, &g_bsDateTimeFormat[j], &pszDisplay[k], MAX_PROP - k);
							g_bsDateTimeFormat[i] = wc;
							j = i;
						}
						bDate = FALSE;
					}
				} else {
					if (!wc || StrChr(L"dMyg", wc)) {
						if (i != j) {
							g_bsDateTimeFormat[i] = NULL;
							int k = lstrlen(*ppszDisplay);
							GetTimeFormat(LOCALE_USER_DEFAULT, 0, &st, &g_bsDateTimeFormat[j], &pszDisplay[k], MAX_PROP - k);
							g_bsDateTimeFormat[i] = wc;
							j = i;
						}
						bDate = TRUE;
					}
				}
			}
			hr = S_OK;
		}
		return hr;
	}
	if (g_param[TE_SizeFormat] && IsEqualPropertyKey(*ppropKey, PKEY_Size)) {
		*ppszDisplay = (LPWSTR)::CoTaskMemAlloc(MAX_PROP);
		teStrFormatSize(g_param[TE_SizeFormat], GetLLFromVariant(pv), *ppszDisplay, MAX_PROP);
		return S_OK;
	}
	PROPVARIANT propVar;
#ifdef _2000XP
	if (lpfnVariantToPropVariant) {
		lpfnVariantToPropVariant(pv, &propVar);
	} else {
		propVar = *((PROPVARIANT *)pv);
	}
#else
	VariantToPropVariant(pv, &propVar);
#endif
#ifdef _2000XP
	if (lpfnPSGetPropertyDescription) {
#endif
		IPropertyDescription *pdesc;
		hr = lpfnPSGetPropertyDescription(*ppropKey, IID_PPV_ARGS(&pdesc));
		if SUCCEEDED(hr) {
			hr = pdesc->FormatForDisplay(propVar, (PROPDESC_FORMAT_FLAGS)pdfFlags, ppszDisplay);
			pdesc->Release();
		}
#ifdef _2000XP
	} else {
		*ppszDisplay = (LPWSTR)::CoTaskMemAlloc(MAX_PROP);
		IPropertyUI *pPUI;
		hr = teCreateInstance(CLSID_PropertiesUI, NULL, NULL, IID_PPV_ARGS(&pPUI));
		if SUCCEEDED(hr) {
			hr = pPUI->FormatForDisplay(ppropKey->fmtid, ppropKey->pid, &propVar, pdfFlags, *ppszDisplay, MAX_PROP);
			pPUI->Release();
		}
	}
	if (lpfnVariantToPropVariant) {
#endif
		PropVariantClear(&propVar);
#ifdef _2000XP
	}
#endif
	return hr;
}

HRESULT MessageSubPt(int nFunc, PVOID pObj, MSG *pMsg)
{
	VARIANT vResult;
	if (g_pOnFunc[nFunc]) {
		VARIANTARG *pv = GetNewVARIANT(5);
		teSetObject(&pv[4], pObj);
		teSetPtr(&pv[3], pMsg->hwnd);
		teSetLong(&pv[2], pMsg->message);
		teSetLong(&pv[1], (LONG)pMsg->wParam);
		CteMemory *pstPt = new CteMemory(2 * sizeof(int), NULL, 1, L"POINT");
		pstPt->SetPoint(pMsg->pt.x, pMsg->pt.y);
		teSetObjectRelease(&pv[0], pstPt);
		Invoke4(g_pOnFunc[nFunc], &vResult, 5, pv);
		if (vResult.vt != VT_EMPTY) {
			return GetIntFromVariantClear(&vResult);
		}
	}
	return S_FALSE;
}

int GetShellBrowser2(CteShellBrowser *pSB1)
{
	for (UINT i = 0; i < g_ppSB.size(); i++) {
		CteShellBrowser *pSB = g_ppSB[i];
		if (pSB == pSB1 && !pSB->m_bEmpty) {
			return i;
		}
	}
	return 0;
}

CteShellBrowser* GetNewShellBrowser(CteTabCtrl *pTC)
{
	CteShellBrowser *pSB = NULL;
	BOOL bNew = TRUE;
	for (UINT i = 0; i < g_ppSB.size(); i++) {
		if (g_ppSB[i]->m_bEmpty) {
			pSB = g_ppSB[i];
			pSB->Init(pTC, FALSE);
			break;
		}
	}
	if (!pSB) {
		pSB = new CteShellBrowser(pTC);
		g_ppSB.push_back(pSB);
		pSB->m_nSB = static_cast<int>(g_ppSB.size());
	}
	if (pTC) {
		TC_ITEM tcItem;
		tcItem.mask = TCIF_PARAM;
		tcItem.lParam = pSB->m_nSB;
		if (pTC) {
			TabCtrl_InsertItem(pTC->m_hwnd, pTC->m_nIndex + 1, &tcItem);
			if (pTC->m_nIndex < 0) {
				pTC->m_nIndex = 0;
			}
		}
		return pSB;
	}
	return NULL;
}

CteTabCtrl* GetNewTC()
{
	for (UINT i = 0; i < g_ppTC.size(); i++) {
		if (g_ppTC[i]->m_bEmpty) {
			return g_ppTC[i];
		}
	}
	CteTabCtrl *pTC = new CteTabCtrl();
	g_ppTC.push_back(pTC);
	pTC->m_nTC = static_cast<int>(g_ppTC.size());
	return pTC;
}

CteTreeView* GetNewTV()
{
	for (UINT i = 0; i < g_ppTV.size(); i++) {
		if (g_ppTV[i]->m_bEmpty) {
			return g_ppTV[i];
		}
	}
	CteTreeView *pTV = new CteTreeView();
	g_ppTV.push_back(pTV);
	return pTV;
}

HRESULT ControlFromhwnd(IDispatch **ppdisp, HWND hwnd)
{
	*ppdisp = NULL;
	CteShellBrowser *pSB = SBfromhwnd(hwnd);
	if (pSB) {
		return pSB->QueryInterface(IID_PPV_ARGS(ppdisp));
	}
	CteTabCtrl *pTC = TCfromhwnd(hwnd);
	if (pTC) {
		return pTC->QueryInterface(IID_PPV_ARGS(ppdisp));
	}
	CteTreeView *pTV = TVfromhwnd(hwnd);
	if (pTV) {
		return pTV->QueryInterface(IID_PPV_ARGS(ppdisp));
	}
	if (g_hwndMain == hwnd && g_pTE) {
		return g_pTE->QueryInterface(IID_PPV_ARGS(ppdisp));
	}
	if (g_pWebBrowser && IsChild(g_pWebBrowser->get_HWND(), hwnd)) {
		return g_pWebBrowser->QueryInterface(IID_PPV_ARGS(ppdisp));
	}
	return E_FAIL;
}

VOID teSetDarkMode(HWND hwnd)
{
	if (lpfnAllowDarkModeForWindow) {
		lpfnAllowDarkModeForWindow(hwnd, g_bDarkMode);
	}
	if (lpfnDwmSetWindowAttribute) {
		lpfnDwmSetWindowAttribute(hwnd, 19, &g_bDarkMode, sizeof(g_bDarkMode));
	}
}

VOID teSetDarkTheme(HWND hwnd, LPCWSTR pszApp)
{
	if (lpfnIsDarkModeAllowedForWindow && lpfnAllowDarkModeForWindow) {
		if ((BOOL(lpfnIsDarkModeAllowedForWindow(hwnd)) ^ g_bDarkMode & 1)) {
			lpfnAllowDarkModeForWindow(hwnd, g_bDarkMode);
			SetWindowTheme(hwnd, pszApp, NULL);
		}
	}
}

BOOL teVerifyVersion(int nMajor, int nMinor, int nBuild)
{
	DWORDLONG dwlConditionMask = 0;
    VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
   VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_BUILDNUMBER, VER_GREATER_EQUAL);
	OSVERSIONINFOEX osvi = { sizeof(OSVERSIONINFOEX), nMajor, nMinor, nBuild };
	return VerifyVersionInfo(&osvi, VER_MAJORVERSION | VER_BUILDNUMBER, dwlConditionMask);
}

LRESULT CALLBACK MenuKeyProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = 1;
	if (nCode >= 0) {
		if (nCode == HC_ACTION) {
			g_dwTickKey = GetTickCount();
			MSG msg;
			msg.hwnd = GetFocus();
			msg.message = (lParam & 0xc0000000) ? WM_KEYUP : WM_KEYDOWN;
			msg.wParam = wParam;
			msg.lParam = lParam;
			try {
				if (InterlockedIncrement(&g_nProcKey) == 1) {
					MessageSub(TE_OnKeyMessage, g_pTE, &msg, (HRESULT *)&lResult);
				}
			} catch(...) {
				g_nException = 0;
#ifdef _DEBUG
				g_strException = L"MenuKeyProc";
#endif
			}
			::InterlockedDecrement(&g_nProcKey);
		}
	}
	return lResult ? CallNextHookEx(g_hMenuKeyHook, nCode, wParam, lParam) : lResult;
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	static UINT dwDoubleTime;
	static WPARAM wParam2;

	HRESULT hrResult = S_FALSE;
	try {
		IDispatch *pdisp = NULL;
		if (nCode >= 0 && g_x == MAXINT) {
			if (nCode == HC_ACTION) {
				g_dwTickKey = 0;
				if (wParam != WM_MOUSEWHEEL && !g_nDropState) {
					if (g_pOnFunc[TE_OnMouseMessage]) {
						MOUSEHOOKSTRUCTEX *pMHS = (MOUSEHOOKSTRUCTEX *)lParam;
						if (ControlFromhwnd(&pdisp, pMHS->hwnd) == S_OK) {
							try {
								if (InterlockedIncrement(&g_nProcMouse) == 1 || wParam != wParam2) {
									wParam2 = wParam;
									CteTreeView *pTV = NULL;
									TVHITTESTINFO ht;
									pTV = TVfromhwnd(pMHS->hwnd);
									if (pTV) {
										if (wParam != WM_MOUSEMOVE && wParam != WM_MOUSEWHEEL) {
											ht.pt.x = pMHS->pt.x;
											ht.pt.y = pMHS->pt.y;
											ht.flags = TVHT_ONITEM;
											ScreenToClient(pMHS->hwnd, &ht.pt);
											TreeView_HitTest(pMHS->hwnd, &ht);
											if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN ||
												wParam == WM_MBUTTONDOWN || wParam == WM_XBUTTONDOWN) {
												g_hItemDown = ht.hItem;
											} else if (wParam == WM_LBUTTONUP || wParam == WM_LBUTTONDBLCLK ||
												wParam == WM_RBUTTONUP || wParam == WM_RBUTTONDBLCLK ||
												wParam == WM_MBUTTONUP || wParam == WM_MBUTTONDBLCLK ||
												wParam == WM_XBUTTONUP || wParam == WM_XBUTTONDBLCLK) {
												if (ht.hItem == g_hItemDown) {
													SetFocus(pMHS->hwnd);
													if (ht.flags & TVHT_ONITEM) {
														TreeView_SelectItem(pMHS->hwnd, ht.hItem);
													}
												}
											}
										}
									}
									MSG msg;
									msg.hwnd = pMHS->hwnd;
									msg.message = (LONG)wParam;
									if (msg.message == WM_LBUTTONDOWN) {
										CHAR szClassA[MAX_CLASS_NAME];
										GetClassNameA(msg.hwnd, szClassA, MAX_CLASS_NAME);
										if (lstrcmpA(szClassA, WC_LISTVIEWA) == 0) {
											msg.hwnd = WindowFromPoint(pMHS->pt);
										} else if (lstrcmpA(szClassA, "DirectUIHWND") == 0) {
											DWORD dwTick = GetTickCount();
											if (dwTick - dwDoubleTime < GetDoubleClickTime()) {
												msg.message = WM_LBUTTONDBLCLK;
											} else {
												dwDoubleTime = dwTick;
											}
											msg.hwnd = WindowFromPoint(pMHS->pt);
										}
									}

									msg.wParam = pMHS->mouseData;
									msg.pt.x = pMHS->pt.x;
									msg.pt.y = pMHS->pt.y;
									hrResult = MessageSubPt(TE_OnMouseMessage, pdisp, &msg);
#ifdef _2000XP
									if (hrResult != S_OK && pTV) {
										if (wParam == WM_LBUTTONUP || wParam == WM_LBUTTONDBLCLK || wParam == WM_MBUTTONUP) {
											if (pTV->m_pShellNameSpace && ht.hItem) {
												NSTCSTYLE nStyle = NSTCECT_LBUTTON;
												if (wParam == WM_LBUTTONDBLCLK) {
													nStyle = NSTCECT_LBUTTON | NSTCECT_DBLCLICK;
												} else if (wParam == WM_MBUTTONUP) {
													nStyle = NSTCECT_MBUTTON;
												}
												hrResult = pTV->OnItemClick(NULL, ht.flags, nStyle);
											}
										}
									}
#endif
								}
							} catch(...) {
								g_nException = 0;
#ifdef _DEBUG
								g_strException = L"MouseProc1";
#endif
							}
							::InterlockedDecrement(&g_nProcMouse);
							pdisp->Release();
						}
					}
				}
			}
		}
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"MouseProc2";
#endif
	}
	return (hrResult != S_OK) ? CallNextHookEx(g_hMouseHook, nCode, wParam, lParam) : TRUE;
}

#ifdef _2000XP
LRESULT CALLBACK TETVProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	BOOL bHandled = FALSE;
	try {
		CteTreeView *pTV = (CteTreeView *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (hwnd != GetParent(pTV->m_hwndTV)) {
			return 0;
		}
		if (msg == WM_NOTIFY) {
			LPNMTVCUSTOMDRAW lptvcd = (LPNMTVCUSTOMDRAW)lParam;
/// Custom Draw
			if (lptvcd->nmcd.hdr.code == NM_CUSTOMDRAW) {
				if (g_pOnFunc[TE_OnItemPrePaint] || g_pOnFunc[TE_OnItemPostPaint]) {
					if (lptvcd->nmcd.dwDrawStage == CDDS_PREPAINT) {
						return CDRF_NOTIFYITEMDRAW;
					}
					if (lptvcd->nmcd.dwDrawStage == CDDS_ITEMPREPAINT) {
						LRESULT lRes = CDRF_DODEFAULT;
						teCustomDraw(TE_OnItemPrePaint, NULL, pTV, NULL, &lptvcd->nmcd, lptvcd, &lRes);
						return lRes;
					}
					if (lptvcd->nmcd.dwDrawStage == CDDS_ITEMPOSTPAINT) {
						LRESULT lRes = CDRF_DODEFAULT;
						teCustomDraw(TE_OnItemPostPaint, NULL, pTV, NULL, &lptvcd->nmcd, lptvcd, &lRes);
						return lRes;
					}
				}
			}
			if (lptvcd->nmcd.hdr.code == NM_RCLICK) {
				try {
					if (InterlockedIncrement(&g_nProcTV) < 5) {
						MSG msg1;
						msg1.hwnd = hwnd;
						msg1.message = WM_CONTEXTMENU;
						msg1.wParam = (WPARAM)hwnd;
						GetCursorPos(&msg1.pt);
						if (MessageSubPt(TE_OnShowContextMenu, pTV, &msg1) == S_OK) {
							bHandled = TRUE;
						}
					}
				} catch(...) {
					g_nException = 0;
#ifdef _DEBUG
					g_strException = L"NM_RCLICK";
#endif
				}
				::InterlockedDecrement(&g_nProcTV);
			}
		}
		return bHandled ? 1 : CallWindowProc(pTV->m_DefProc, hwnd, msg, wParam, lParam);
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"TETVProc";
#endif
	}
	return 0;
}
#endif

LRESULT CALLBACK TETVProc2(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	try {
		CteTreeView *pTV = (CteTreeView *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (hwnd != pTV->m_hwndTV) {
			return 0;
		}
		if (pTV->m_bMain) {
			if (msg == WM_ENTERMENULOOP) {
				pTV->m_bMain = FALSE;
			}
		} else {
			if (msg == WM_EXITMENULOOP) {
				pTV->m_bMain = TRUE;
			} else if (msg >= TV_FIRST && msg < TV_FIRST + 0x100) {
				return 0;
			}
		}
		if (msg == WM_COMMAND) {
			LRESULT Result = S_FALSE;
			try {
				if (InterlockedIncrement(&g_nProcTV) == 1) {
					Result = teDoCommand(pTV, hwnd, msg, wParam, lParam);
				}
			} catch(...) {
				g_nException = 0;
#ifdef _DEBUG
				g_strException = L"TETVProc2/DoCommand";
#endif
			}
			::InterlockedDecrement(&g_nProcTV);
			if (Result == 0) {
				return 0;
			}
		}
#ifdef _2000XP
		if (msg == TVM_INSERTITEM) {
			TVINSERTSTRUCT *pTVInsert = (TVINSERTSTRUCT *)lParam;
			if (pTVInsert->item.cChildren == 1) {
				pTVInsert->item.cChildren = I_CHILDRENCALLBACK;
			}
		}
#endif
		return CallWindowProc(pTV->m_DefProc2, hwnd, msg, wParam, lParam);
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"TETVProc2";
#endif
	}
	return 0;
}

LRESULT CALLBACK TELVProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndEdit = NULL;
	static FolderItem *pidEdit = NULL;

	CteShellBrowser *pSB = (CteShellBrowser *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	try {
		if (hwnd != pSB->m_hwndDV) {
			return 0;
		}
		BOOL bDoCallProc = TRUE;
		LRESULT lResult = S_FALSE;
		if (msg == WM_NOTIFY) {
			if (((LPNMHDR)lParam)->code == LVN_GETDISPINFO) {
				NMLVDISPINFO *lpDispInfo = (NMLVDISPINFO *)lParam;
				if (lpDispInfo->item.mask & LVIF_TEXT) {
					if (pSB->m_param[SB_ViewMode] == FVM_DETAILS || lpDispInfo->item.iSubItem == 0) {
						IDispatch *pdisp = lpDispInfo->item.iSubItem < pSB->m_iColumns ? pSB->m_ppColumns[lpDispInfo->item.iSubItem] : NULL;
						if (pdisp) {
							bDoCallProc = FALSE;
							lResult = CallWindowProc((WNDPROC)pSB->m_DefProc, hwnd, msg, wParam, lParam);
							if (teHasProperty(pdisp, L"push")) {
								VARIANT v;
								VariantInit(&v);
								for (int nLen = teGetObjectLength(pdisp); nLen-- > 0;) {
									if SUCCEEDED(teGetPropertyAt(pdisp, nLen, &v)) {
										IDispatch *pdisp2;
										if (GetDispatch(&v, &pdisp2)) {
											if (pSB->ReplaceColumns(pdisp2, lpDispInfo)) {
												nLen = 0;
											}
											pdisp2->Release();
										}
										VariantClear(&v);
									}
								}
							} else {
								pSB->ReplaceColumns(pdisp, lpDispInfo);
							}
						}
						if (lpDispInfo->item.iSubItem == pSB->m_nFolderSizeIndex) {
							if (bDoCallProc) {
								bDoCallProc = FALSE;
								lResult = CallWindowProc((WNDPROC)pSB->m_DefProc, hwnd, msg, wParam, lParam);
							}
							IFolderView *pFV;
							LPITEMIDLIST pidl;
							if SUCCEEDED(pSB->m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV))) {
								if SUCCEEDED(pFV->Item(lpDispInfo->item.iItem, &pidl)) {
									pSB->SetFolderSize(pSB->m_pSF2, pidl, lpDispInfo->item.pszText, lpDispInfo->item.cchTextMax);
									teCoTaskMemFree(pidl);
								}
								pFV->Release();
							}
						}
						if (lpDispInfo->item.iSubItem == pSB->m_nLabelIndex && g_pOnFunc[TE_Labels]) {
							IFolderView *pFV;
							LPITEMIDLIST pidl;
							if SUCCEEDED(pSB->m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV))) {
								if SUCCEEDED(pFV->Item(lpDispInfo->item.iItem, &pidl)) {
									pSB->SetLabel(pidl, lpDispInfo->item.pszText, lpDispInfo->item.cchTextMax);
									teCoTaskMemFree(pidl);
									pFV->Release();
								}
							}
							return 0;
						}
						if (lpDispInfo->item.iSubItem == pSB->m_nSizeIndex && g_param[TE_SizeFormat]) {
							if (bDoCallProc) {
								bDoCallProc = FALSE;
								lResult = CallWindowProc((WNDPROC)pSB->m_DefProc, hwnd, msg, wParam, lParam);
							}
							if (lpDispInfo->item.pszText[0]) {
								IFolderView *pFV;
								LPITEMIDLIST pidl;
								if SUCCEEDED(pSB->m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV))) {
									if SUCCEEDED(pFV->Item(lpDispInfo->item.iItem, &pidl)) {
										pSB->SetSize(pidl, lpDispInfo->item.pszText, lpDispInfo->item.cchTextMax);
									}
									teCoTaskMemFree(pidl);
									pFV->Release();
								}
							}
						}
						if (lpDispInfo->item.iSubItem == pSB->m_nLinkTargetIndex) {
							if (bDoCallProc) {
								bDoCallProc = FALSE;
								lResult = CallWindowProc((WNDPROC)pSB->m_DefProc, hwnd, msg, wParam, lParam);
							}
							if (!lpDispInfo->item.pszText[0]) {
								IFolderView *pFV;
								LPITEMIDLIST pidl;
								if SUCCEEDED(pSB->m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV))) {
									if SUCCEEDED(pFV->Item(lpDispInfo->item.iItem, &pidl)) {
										WIN32_FIND_DATA wfd;
										if SUCCEEDED(SHGetDataFromIDList(pSB->m_pSF2, pidl, SHGDFIL_FINDDATA, &wfd, sizeof(WIN32_FIND_DATA))) {
											if (wfd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
												BSTR bs;
												if SUCCEEDED(teGetDisplayNameBSTR(pSB->m_pSF2, pidl, SHGDN_FORPARSING, &bs)) {
													teGetJunctionLinkTarget(bs, &lpDispInfo->item.pszText, lpDispInfo->item.cchTextMax);
													teSysFreeString(&bs);
												}
											}
										}
									}
									teCoTaskMemFree(pidl);
									pFV->Release();
								}
							}
						}
					}
					if (g_bsDateTimeFormat && pSB->m_pDTColumns && lpDispInfo->item.iSubItem >= 0) {
						WORD ix = pSB->m_pDTColumns[lpDispInfo->item.iSubItem];
						if (ix) {
							VARIANT v;
							VariantInit(&v);
							IFolderView *pFV;
							LPITEMIDLIST pidl;
							if SUCCEEDED(pSB->m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV))) {
								if SUCCEEDED(pFV->Item(lpDispInfo->item.iItem, &pidl)) {
									SHCOLUMNID scid;
									if SUCCEEDED(pSB->m_pSF2->MapColumnToSCID(ix, &scid)) {
										pSB->m_pSF2->GetDetailsEx(pidl, &scid, &v);
									}
									teCoTaskMemFree(pidl);
									pFV->Release();
								}
							}
							if (v.vt == VT_DATE) {
								LPWSTR pszDisplay;
								if SUCCEEDED(tePSFormatForDisplay(NULL, &v, PDFF_DEFAULT, &pszDisplay)) {
									lstrcpyn(lpDispInfo->item.pszText, pszDisplay, lpDispInfo->item.cchTextMax);
									teCoTaskMemFree(pszDisplay);
								}
								VariantClear(&v);
								return 0;
							}
							if (v.vt != VT_EMPTY) {
								VariantClear(&v);
								pSB->m_pDTColumns[lpDispInfo->item.iSubItem] = 0;
							}
						}
					}
				}
				if (lpDispInfo->item.mask & LVIF_IMAGE && g_pOnFunc[TE_OnHandleIcon]) {
					IFolderView *pFV;
					LPITEMIDLIST pidl;
					if SUCCEEDED(pSB->m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV))) {
						VARIANT vResult;
						VariantInit(&vResult);
						if SUCCEEDED(pFV->Item(lpDispInfo->item.iItem, &pidl)) {
							VARIANTARG *pv = GetNewVARIANT(2);
							teSetObject(&pv[1], pSB);
							LPITEMIDLIST pidlFull = ILCombine(pSB->m_pidl, pidl);
							teSetIDListRelease(&pv[0], &pidlFull);
							teCoTaskMemFree(pidl);
							Invoke4(g_pOnFunc[TE_OnHandleIcon], &vResult, 2, pv);
						}
						pFV->Release();
						if (GetIntFromVariantClear(&vResult)) {
							lpDispInfo->item.mask &= ~LVIF_IMAGE;
							if (bDoCallProc) {
								bDoCallProc = FALSE;
								lResult = CallWindowProc((WNDPROC)pSB->m_DefProc, hwnd, msg, wParam, lParam);
							}
							lpDispInfo->item.mask |= LVIF_IMAGE;
							lpDispInfo->item.iImage = -1;
						}
					}
				}
			} else if (((LPNMHDR)lParam)->code == LVN_COLUMNCLICK) {
				if (g_pOnFunc[TE_OnColumnClick]) {
					LPNMLISTVIEW pnmv = (LPNMLISTVIEW)lParam;
					HWND hHeader = ListView_GetHeader(pSB->m_hwndLV);
					if (hHeader) {
						UINT nHeader = Header_GetItemCount(hHeader);
						if (nHeader) {
							int *piOrderArray = new int[nHeader];
							ListView_GetColumnOrderArray(pSB->m_hwndLV, nHeader, piOrderArray);
							VARIANTARG *pv = GetNewVARIANT(2);
							teSetObject(&pv[1], pSB);
							teSetLong(&pv[0], piOrderArray[pnmv->iSubItem]);
							delete [] piOrderArray;
							VARIANT vResult;
							VariantInit(&vResult);
							Invoke4(g_pOnFunc[TE_OnColumnClick], &vResult, 2, pv);
							if (vResult.vt != VT_EMPTY) {
								if (!GetIntFromVariantClear(&vResult)) {
									return 1;
								}
							}
						}
					}
				}
			} else if (((LPNMHDR)lParam)->code == LVN_INCREMENTALSEARCH) {
				NMLVFINDITEM *lpFindItem = (NMLVFINDITEM *)lParam;
				if (lpFindItem->lvfi.flags & (LVFI_STRING | LVFI_PARTIAL)) {
					DoStatusText(pSB, lpFindItem->lvfi.psz, 0);
				}
			} else if (((LPNMHDR)lParam)->code == LVN_BEGINLABELEDIT) {
				VARIANT vResult;
				VariantInit(&vResult);
				if SUCCEEDED(DoFunc1(TE_OnBeginLabelEdit, pSB, &vResult)) {
					if (GetIntFromVariantClear(&vResult)) {
						return TRUE;
					}
				}
			} else if (((LPNMHDR)lParam)->code == LVN_ENDLABELEDIT) {
				NMLVDISPINFO *lpDispInfo = (NMLVDISPINFO *)lParam;
				if (g_pOnFunc[TE_OnEndLabelEdit]) {
					VARIANTARG *pv = GetNewVARIANT(2);
					teSetObject(&pv[1], pSB);
					teSetSZ(&pv[0], lpDispInfo->item.pszText);
					VARIANT vResult;
					VariantInit(&vResult);
					Invoke4(g_pOnFunc[TE_OnEndLabelEdit], &vResult, 2, pv);
					if (GetIntFromVariantClear(&vResult) && lpDispInfo->item.pszText) {
						lpDispInfo->item.pszText[0] = NULL;
					}
				}
				if (lpDispInfo->item.pszText) {
					if (lpDispInfo->item.pszText[0] == '.' && !StrChr(&lpDispInfo->item.pszText[1], '.')) {
						int i = lstrlen(lpDispInfo->item.pszText);
						if (i > 1 && i < lpDispInfo->item.cchTextMax - 1) {
							lpDispInfo->item.pszText[i] = '.';
							lpDispInfo->item.pszText[i + 1] = NULL;
						}
					}
				}
			} else if (((LPNMHDR)lParam)->code == LVN_GETINFOTIP) {
				LPNMLVGETINFOTIP lpInfotip = ((LPNMLVGETINFOTIP)lParam);
				if (g_pOnFunc[TE_OnToolTip] && lpInfotip->cchTextMax) {
					VARIANT vResult;
					VariantInit(&vResult);
					VARIANTARG *pv = GetNewVARIANT(2);
					teSetObject(&pv[1], pSB);
					teSetLong(&pv[0], lpInfotip->iItem);
					Invoke4(g_pOnFunc[TE_OnToolTip], &vResult, 2, pv);
					if (vResult.vt == VT_BSTR) {
						lstrcpyn(lpInfotip->pszText, vResult.bstrVal, lpInfotip->cchTextMax);
						lResult = 0;
					}
					VariantClear(&vResult);
				}
			} else if (((LPNMHDR)lParam)->code == HDN_ITEMCHANGED) {
				pSB->SetLVSettings();
			}
/// Custom Draw
			if (pSB->m_pShellView) {
				if (g_pOnFunc[TE_OnItemPrePaint] || g_pOnFunc[TE_OnItemPostPaint]) {
					LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)lParam;
					if (lplvcd->nmcd.hdr.code == NM_CUSTOMDRAW) {
						if (lplvcd->nmcd.dwDrawStage == CDDS_PREPAINT) {
							return CDRF_NOTIFYITEMDRAW;
						}
						if (lplvcd->nmcd.dwDrawStage == CDDS_ITEMPREPAINT) {
							LRESULT lRes = CDRF_DODEFAULT;
							teCustomDraw(TE_OnItemPrePaint, pSB, NULL, NULL, &lplvcd->nmcd, lplvcd, &lRes);
							return lRes;
						}
						if (lplvcd->nmcd.dwDrawStage == CDDS_ITEMPOSTPAINT) {
							LRESULT lRes = CDRF_DODEFAULT;
							teCustomDraw(TE_OnItemPostPaint, pSB, NULL, NULL, &lplvcd->nmcd, lplvcd, &lRes);
							return lRes;
						}
					}
				}
			}
//*/
		}
		if (msg == WM_CONTEXTMENU || msg == SB_SETTEXT || msg == WM_COMMAND || msg == WM_COPYDATA) {
			if (pSB->m_hwndAlt && msg == WM_CONTEXTMENU) {
				return TRUE;
			}
			try {
				if (InterlockedIncrement(&g_nProcFV) < 5) {
					switch (msg) {
						case WM_CONTEXTMENU:
							IDispatch *pdisp;
							if (ControlFromhwnd(&pdisp, hwnd) == S_OK) {
								MSG msg1;
								msg1.hwnd = hwnd;
								msg1.message = msg;
								msg1.wParam = wParam;
								HWND hwndLV = pSB->m_hwndLV;
								if ((lParam & MAXDWORD) == MAXDWORD) {
									msg1.pt.x = 0;
									msg1.pt.y = 0;
									if (hwndLV) {
										ClientToScreen(hwndLV, &msg1.pt);
										int nSelected = ListView_GetNextItem(hwndLV, -1, LVNI_ALL | LVNI_SELECTED);
										if (nSelected >= 0) {
											RECT rc;
											ListView_GetItemRect(hwndLV, nSelected, &rc, LVIR_ICON);
											OffsetRect(&rc, msg1.pt.x, msg1.pt.y);
											msg1.pt.x = (rc.left + rc.right) / 2;
											msg1.pt.y = (rc.top + rc.bottom) / 2;
										}
									} else {
										ClientToScreen(pSB->m_hwndDV, &msg1.pt);
									}
								} else {
									if (GetKeyState(VK_SHIFT) < 0 && ListView_GetNextItem(hwndLV, -1, LVNI_ALL | LVNI_SELECTED) >= 0) {
										int nFocused = ListView_GetNextItem(hwndLV, -1, LVNI_ALL | LVNI_FOCUSED);
										if (!ListView_GetItemState(hwndLV, nFocused, LVIS_SELECTED)) {
											ListView_SetItemState(hwndLV, ListView_GetNextItem(hwndLV, -1, LVNI_ALL | LVNI_SELECTED), LVIS_FOCUSED, LVIS_FOCUSED);
											ListView_SetItemState(hwndLV, nFocused, 0, LVIS_FOCUSED);
										}
									}
									msg1.pt.x = GET_X_LPARAM(lParam);
									msg1.pt.y = GET_Y_LPARAM(lParam);
								}
								lResult = MessageSubPt(TE_OnShowContextMenu, pdisp, &msg1);
							}
							break;
						case SB_SETTEXT:
							pSB->SetStatusTextSB((LPCWSTR)lParam);
							break;
						case WM_COMMAND:
							lResult = teDoCommand(pSB, hwnd, msg, wParam, lParam);
							if (wParam == 0x7103) {//Refresh
								if (ILIsEqual(pSB->m_pidl, g_pidls[CSIDL_RESULTSFOLDER])) {
//									pSB->BrowseObject(NULL, SBSP_RELATIVE | SBSP_SAMEBROWSER);
									pSB->Refresh(FALSE);
								} else {
									pSB->m_bRefreshing = TRUE;
								}
							}
							break;
						case WM_COPYDATA:
							if (g_pOnFunc[TE_OnSystemMessage]) {
								MSG msg1;
								msg1.hwnd = hwnd;
								msg1.message = msg;
								msg1.wParam = wParam;
								msg1.lParam = lParam;
								MessageSub(TE_OnSystemMessage, pSB, &msg1, (HRESULT *)&lResult);
							}
							break;
					}//end_switch
				}
			} catch(...) {
				g_nException = 0;
#ifdef _DEBUG
				g_strException = L"TELVProc";
#endif
			}
			::InterlockedDecrement(&g_nProcFV);
		}
		if (msg == WM_WINDOWPOSCHANGED) {
			pSB->SetFolderFlags(FALSE);
		}
		if (msg == WM_SETTINGCHANGE) {
			pSB->InitFolderSize();
		}
		if (hwnd != pSB->m_hwndDV) {
			return 0;
		}
		return bDoCallProc && lResult ? CallWindowProc((WNDPROC)pSB->m_DefProc, hwnd, msg, wParam, lParam) : lResult;
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		DWORD dw = 0;
		if (msg == WM_NOTIFY) {
			LPNMTVCUSTOMDRAW lptvcd = (LPNMTVCUSTOMDRAW)lParam;
			dw = lptvcd->nmcd.hdr.code;
		}
		swprintf_s(g_pszException, MAX_PATH, L"TELVProc/%x %x", msg, dw);
		g_strException = g_pszException;
#endif
	}
	return 0;
}

/*/// Shift to HookProc
LRESULT CALLBACK TELVProc2(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	try {
		CteShellBrowser *pSB = (CteShellBrowser *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (hwnd != pSB->m_hwndLV) {
			return 0;
		}
		if (msg == WM_HSCROLL || msg == WM_VSCROLL) {
			g_dwTickScroll = GetTickCount();
		}
		return CallWindowProc((WNDPROC)pSB->m_DefProc2, hwnd, msg, wParam, lParam);
	} catch (...) {
		g_nException = 0;
	}
	return 0;
}
//*///
VOID teSetTreeWidth(CteShellBrowser	*pSB, HWND hwnd, LPARAM lParam)
{
	int nWidth = pSB->m_param[SB_TreeWidth] + GET_X_LPARAM(lParam) - g_x;
	RECT rc;
	GetWindowRect(hwnd, &rc);
	int nMax = rc.right - rc.left - 1;
	if (nWidth > nMax) {
		nWidth = nMax;
	} else if (nWidth < 3) {
		nWidth = 3;
	} else {
		g_x =GET_X_LPARAM(lParam);
	}
	if (pSB->m_param[SB_TreeWidth] != nWidth) {
		pSB->m_param[SB_TreeWidth] = nWidth;
		ArrangeWindow();
	}
}

VOID teSetTabWidth(CteTabCtrl *pTC, LPARAM lParam)
{
	int nWidth = pTC->m_param[TC_TabWidth];
	RECT rc;
	GetWindowRect(pTC->m_hwndStatic, &rc);
	int nMax = rc.right - rc.left - 1;
	int nDiff = GET_X_LPARAM(lParam) - g_x;
	nWidth += (pTC->m_param[TC_Align] == 4) ? nDiff : -nDiff;
	if (nWidth > nMax) {
		nWidth = nMax;
	} else if (nWidth < 1) {
		nWidth = 1;
	} else {
		g_x = GET_X_LPARAM(lParam);
	}
	if (pTC->m_param[TC_TabWidth] != nWidth) {
		pTC->m_param[TC_TabWidth] = nWidth;
		ArrangeWindow();
	}
}

LRESULT CALLBACK TESTProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = 1;
	try {
		CteTabCtrl *pTC = (CteTabCtrl *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (hwnd != pTC->m_hwndStatic) {
			return 0;
		}
		CteShellBrowser	*pSB = pTC->GetShellBrowser(pTC->m_nIndex);
		RECT rc;
		if (pSB && pSB->m_pidl && GetWindowRect(pSB->m_hwnd, &rc) && rc.top != rc.bottom && IsWindowVisible(pSB->m_pTV->m_hwndTV)) {
			switch (msg) {
				case WM_MOUSEMOVE:
					if (g_x == MAXINT) {
						SetCursor(LoadCursor(NULL, IDC_SIZEWE));
					} else {
						teSetTreeWidth(pSB, hwnd, lParam);
					}
					break;
				case WM_LBUTTONDOWN:
					SetCapture(hwnd);
					g_x = GET_X_LPARAM(lParam);
					SetCursor(LoadCursor(NULL, IDC_SIZEWE));
					break;
				case WM_LBUTTONUP:
					if (g_x != MAXINT) {
						ReleaseCapture();
						teSetTreeWidth(pSB, hwnd, lParam);
						g_x = MAXINT;
					}
					break;
			}//end_switch
		}
		return lResult ? CallWindowProc(pTC->m_DefSTProc, hwnd, msg, wParam, lParam) : 0;
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"TESTProc";
#endif
	}
	return 0;
}

LRESULT TETCProc2(CteTabCtrl *pTC, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = 1;
	switch (msg) {
		case WM_LBUTTONDOWN:
			if ((pTC->m_param[TC_Align] == 4 && WORD(lParam) >= pTC->m_param[TC_TabWidth] - pTC->m_nScrollWidth - 2 && WORD(lParam) < pTC->m_param[TC_TabWidth]) ||
				(pTC->m_param[TC_Align] == 5 && WORD(lParam) < 2)) {
				SetCapture(hwnd);
				g_x = GET_X_LPARAM(lParam);
				SetCursor(LoadCursor(NULL, IDC_SIZEWE));
				lResult = 0;
			}
		case WM_SETFOCUS:
		case WM_RBUTTONDOWN:
			CheckChangeTabTC(hwnd, TRUE);
			break;
		case WM_MOUSEMOVE:
			if (g_x == MAXINT) {
				if ((pTC->m_param[TC_Align] == 4 && WORD(lParam) >= pTC->m_param[TC_TabWidth] - pTC->m_nScrollWidth - 2 && WORD(lParam) < pTC->m_param[TC_TabWidth]) ||
					(pTC->m_param[TC_Align] == 5 && WORD(lParam) < 2)) {
					SetCursor(LoadCursor(NULL, IDC_SIZEWE));
				}
			} else {
				if (pTC->m_param[TC_Align] == 4) {
					teSetTabWidth(pTC, lParam);
				}
				lResult = 0;
			}
			break;
		case WM_LBUTTONUP:
			if (g_x != MAXINT) {
				ReleaseCapture();
				teSetTabWidth(pTC, lParam);
				g_x = MAXINT;
			}
			break;
	}
	return lResult;
}

LRESULT CALLBACK TEBTProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = 1;
	try {
		CteTabCtrl *pTC = (CteTabCtrl *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (hwnd != pTC->m_hwndButton) {
			return 0;
		}
		switch (msg) {
			case WM_NOTIFY:
				switch (((LPNMHDR)lParam)->code) {
					case TCN_SELCHANGE:
						if (g_pTC != pTC && pTC->m_bVisible) {
							g_pTC = pTC;
						}
						pTC->TabChanged(TRUE);
						lResult = 0;
						break;
					case TCN_SELCHANGING:
						lResult = DoFunc(TE_OnSelectionChanging, pTC, 0);
						break;
					case TTN_GETDISPINFO:
						if (g_pOnFunc[TE_OnToolTip]) {
							VARIANT vResult;
							VariantInit(&vResult);
							VARIANTARG *pv = GetNewVARIANT(2);
							teSetObject(&pv[1], pTC);
							teSetPtr(&pv[0], ((LPNMHDR)lParam)->idFrom);
							Invoke4(g_pOnFunc[TE_OnToolTip], &vResult, 2, pv);
							VARIANT vText;
							teVariantChangeType(&vText, &vResult, VT_BSTR);
							lstrcpyn(g_szText, vText.bstrVal, MAX_STATUS);
							((LPTOOLTIPTEXT)lParam)->lpszText = g_szText;
							VariantClear(&vResult);
							VariantClear(&vText);
						}
						break;
				}
				break;
			case WM_CONTEXTMENU:
				MSG msg1;
				msg1.hwnd = pTC->m_hwnd;
				msg1.message = msg;
				msg1.wParam = wParam;
				msg1.pt.x = GET_X_LPARAM(lParam);
				msg1.pt.y = GET_Y_LPARAM(lParam);
				MessageSubPt(TE_OnShowContextMenu, pTC, &msg1);
				lResult = 0;
				break;
			case WM_VSCROLL:
				switch(LOWORD(wParam)){
					case SB_LINEUP:
						pTC->m_si.nPos -= 16;
						break;
					case SB_LINEDOWN:
						pTC->m_si.nPos += 16;
						break;
					case SB_PAGEUP:
						pTC->m_si.nPos -= pTC->m_si.nPage;
						break;
					case SB_PAGEDOWN:
						pTC->m_si.nPos += pTC->m_si.nPage;
						break;
					case SB_THUMBPOSITION:
					case SB_THUMBTRACK:
						pTC->m_si.nPos = HIWORD(wParam);
						break;
				}//end_switch
				if (pTC->m_si.nPos > pTC->m_si.nMax) {
					pTC->m_si.nPos = pTC->m_si.nMax;
				}
				if (pTC->m_si.nPos < 0) {
					pTC->m_si.nPos = 0;
				}
				SetScrollInfo(pTC->m_hwndButton, SB_VERT, &pTC->m_si, TRUE);
				ArrangeWindow();
				break;
		}//end_switch
		if (lResult) {
			lResult = TETCProc2(pTC, hwnd, msg, wParam, lParam);
		}
		return lResult ? CallWindowProc(pTC->m_DefBTProc, hwnd, msg, wParam, lParam) : 0;
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"TEBTProc";
#endif
	}
	return 0;
}

LRESULT CALLBACK TETCProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT Result = 1;
	int nIndex, nCount;
	static BOOL bCancelPaint = FALSE;
	try {
		CteTabCtrl *pTC = (CteTabCtrl *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (hwnd != pTC->m_hwnd) {
			return 0;
		}
		switch (msg) {
			case TCM_DELETEITEM:
				if (lParam) {
					lParam = 0;
				} else {
					CteShellBrowser *pSB = NULL;
					pSB = pTC->GetShellBrowser((int)wParam);
					CallWindowProc(pTC->m_DefTCProc, hwnd, msg, wParam, lParam);
					Result = 0;
					if (pSB) {
						pSB->Close(TRUE);
					}
					nIndex = TabCtrl_GetCurSel(hwnd);
					if (nIndex >= 0) {
						pTC->m_nIndex = nIndex;
					} else {
						nCount = TabCtrl_GetItemCount(hwnd);
						nIndex = pTC->m_nIndex;
						if ((int)wParam > nIndex) {
							nIndex--;
						}
						if (nIndex >= nCount) {
							nIndex = nCount - 1;
						}
						if (nIndex < 0) {
							nIndex = 0;
						}
						pTC->m_nIndex = -1;
						CallWindowProc(pTC->m_DefTCProc, hwnd, TCM_SETCURSEL, nIndex, 0);
					}
					pTC->TabChanged(TRUE);
					ArrangeWindow();
				}
				break;
			case TCM_SETCURSEL:
				if (wParam != (UINT_PTR)TabCtrl_GetCurSel(hwnd)) {
					CallWindowProc(pTC->m_DefTCProc, hwnd, msg, wParam, lParam);
					Result = 0;
					pTC->TabChanged(TRUE);
					if (g_param[TE_Tab] && pTC->m_param[TC_Align] == 1) {
						if (pTC->m_param[TE_Flags] & TCS_SCROLLOPPOSITE) {
							ArrangeWindow();
						}
					}
				}
				break;
			case TCM_SETITEM:
				if (pTC->m_param[TE_Flags] & TCS_FIXEDWIDTH) {
					CallWindowProc(pTC->m_DefTCProc, hwnd, msg, wParam, lParam);
					Result = 0;
					CallWindowProc(pTC->m_DefTCProc, pTC->m_hwnd, TCM_SETITEMSIZE, 0, MAKELPARAM(pTC->m_param[TC_TabWidth], pTC->m_param[TC_TabHeight] + 1));
					CallWindowProc(pTC->m_DefTCProc, pTC->m_hwnd, TCM_SETITEMSIZE, 0, pTC->m_dwSize);
				}
				break;
		}//end_switch

		//Fix for high CPU usage on single line tab.
		if (!(pTC->m_param[TE_Flags] & TCS_MULTILINE)) {
			if (msg == WM_PAINT) {
				if (bCancelPaint) {
					Result = 0;
				}
				bCancelPaint = TRUE;
			} else if (msg != WM_ERASEBKGND && msg != WM_NCPAINT && msg != TCM_GETITEM) {
				bCancelPaint = FALSE;
			}
		}

		if (Result) {
			Result = TETCProc2(pTC, hwnd, msg, wParam, lParam);
		}
		return Result ? CallWindowProc(pTC->m_DefTCProc, hwnd, msg, wParam, lParam) : 0;
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"TETCProc";
#endif
	}
	return 0;
}

HRESULT MessageProc(MSG *pMsg)
{
	HRESULT hrResult = S_FALSE;
	IDispatch *pdisp = NULL;
	IShellBrowser *pSB = NULL;
	IShellView *pSV = NULL;

	if (pMsg->message == WM_MOUSEWHEEL) {
		try {
			if (InterlockedIncrement(&g_nProcMouse) < 10) {
				if (ControlFromhwnd(&pdisp, pMsg->hwnd) == S_OK) {
					if (MessageSubPt(TE_OnMouseMessage, pdisp, pMsg) == S_OK) {
						hrResult = S_OK;
					}
					pdisp->Release();
				}
			}
		} catch(...) {
			g_nException = 0;
#ifdef _DEBUG
			g_strException = L"MessageProc1";
#endif
		}
		::InterlockedDecrement(&g_nProcMouse);
	}

	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST) {
		try {
			g_dwTickKey = GetTickCount();
			if (pMsg->message == WM_KEYUP && g_szStatus[0]) {
				SetTimer(g_hwndMain, TET_Status, 100, teTimerProc);
			}
			if (InterlockedIncrement(&g_nProcKey) == 1) {
				if (ControlFromhwnd(&pdisp, pMsg->hwnd) == S_OK) {
					MessageSub(TE_OnKeyMessage, pdisp, pMsg, &hrResult);
					if (hrResult != S_OK) {
						if SUCCEEDED(pdisp->QueryInterface(IID_PPV_ARGS(&pSB))) {
							if SUCCEEDED(pSB->QueryActiveShellView(&pSV)) {
								if (pSV->TranslateAcceleratorW(pMsg) == S_OK) {
									hrResult = S_OK;
								}
								pSV->Release();
							}
							pSB->Release();
						} else {
							teTranslateAccelerator(pdisp, pMsg, &hrResult);
						}
					}
					pdisp->Release();
				}
				VARIANT v;
				if SUCCEEDED(teGetPropertyAtLLX(g_pSubWindows, (LONGLONG)GetParent((HWND)GetWindowLongPtr(GetParent(pMsg->hwnd), GWLP_HWNDPARENT)), &v)) {
					teTranslateAccelerator(v.pdispVal, pMsg, &hrResult);
					VariantClear(&v);
				}
			}
		} catch(...) {
			g_nException = 0;
#ifdef _DEBUG
			g_strException = L"MessageProc2";
#endif
		}
		::InterlockedDecrement(&g_nProcKey);
	}
	return hrResult;
}

VOID Finalize()
{
	try {
		teRevoke();
		for (int i = _countof(g_maps); i--;) {
			delete[] g_maps[i];
		}
#ifdef _2000XP
		lpfnSHCreateItemFromIDList = NULL;
#endif
		for (int i = MAX_CSIDL2; i--;) {
			teILFreeClear(&g_pidls[i]);
			teSysFreeString(&g_bsPidls[i]);
		}
		teSysFreeString(&g_bsCmdLine);
		if (g_hDwmapi) {
			FreeLibrary(g_hDwmapi);
		}
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"Finalize";
#endif
	}
	try {
		::OleUninitialize();
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"OleUninitialize";
#endif
	}
}

HRESULT teExtract(IStorage *pStorage, LPWSTR lpszFolderPath, IProgressDialog *ppd, int *pnItems, int nCount, int nBase)
{
	STATSTG statstg;
	IEnumSTATSTG *pEnumSTATSTG = NULL;
	BYTE pszData[SIZE_BUFF];
	IStream *pStream;
	ULONG uRead;
	DWORD dwWriteByte;
#ifdef _2000XP
	IShellFolder2 *pSF2;
	LPITEMIDLIST pidl;
	ULONG chEaten;
	ULONG dwAttributes;
#endif
	if (ppd->HasUserCancelled()) {
		return E_ABORT;
	}
	if (nCount >= 0) {
		CreateDirectory(lpszFolderPath, NULL);
	}
	HRESULT hr = pStorage->EnumElements(NULL, NULL, NULL, &pEnumSTATSTG);
	while (SUCCEEDED(hr) && pEnumSTATSTG->Next(1, &statstg, NULL) == S_OK) {
		BSTR bsPath;
		tePathAppend(&bsPath, lpszFolderPath, statstg.pwcsName);
		if (statstg.type == STGTY_STORAGE) {
			IStorage *pStorageNew;
			pStorage->OpenStorage(statstg.pwcsName, NULL, STGM_READ, 0, 0, &pStorageNew);
			hr = teExtract(pStorageNew, bsPath, ppd, pnItems, nCount, nBase);
			pStorageNew->Release();
		} else if (statstg.type == STGTY_STREAM) {
			ppd->SetLine(2, &bsPath[nBase], TRUE, NULL);
			pnItems[0]++;
			if (ppd->HasUserCancelled()) {
				hr = E_ABORT;
			} else if (nCount >= 0) {
				teSetProgress(ppd, pnItems[0], nCount);
				hr = pStorage->OpenStream(statstg.pwcsName, NULL, STGM_READ, NULL, &pStream);
				if SUCCEEDED(hr) {
					HANDLE hFile = CreateFile(bsPath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					if (hFile != INVALID_HANDLE_VALUE) {
						while (SUCCEEDED(pStream->Read(pszData, SIZE_BUFF, &uRead)) && uRead) {
							WriteFile(hFile, pszData, uRead, &dwWriteByte, NULL);
						}
#ifdef _2000XP
						if (statstg.mtime.dwLowDateTime == 0) {
							if SUCCEEDED(pStorage->QueryInterface(IID_PPV_ARGS(&pSF2))) {
								if SUCCEEDED(pSF2->ParseDisplayName(NULL, NULL, statstg.pwcsName, &chEaten, &pidl, &dwAttributes)) {
									teGetFileTimeFromItem(pSF2, pidl, &PKEY_DateCreated, &statstg.ctime);
									teGetFileTimeFromItem(pSF2, pidl, &PKEY_DateAccessed, &statstg.atime);
									teGetFileTimeFromItem(pSF2, pidl, &PKEY_DateModified, &statstg.mtime);
									teCoTaskMemFree(pidl);
								}
								pSF2->Release();
							}
						}
#endif
						SetFileTime(hFile, &statstg.ctime, &statstg.atime, &statstg.mtime);
						CloseHandle(hFile);
						hr = S_OK;
					} else {
						hr = E_ACCESSDENIED;
					}
					pStream->Release();
				}
			}
		}
		::SysFreeString(bsPath);
		teCoTaskMemFree(statstg.pwcsName);
	}
	SafeRelease(&pEnumSTATSTG);
	return hr;
}

BOOL teLocalizePath(LPWSTR pszPath, BSTR *pbsPath)
{
	*pbsPath = NULL;
	if (PathMatchSpec(pszPath, L"::{*}*")) {
		BSTR bsPath = ::SysAllocString(pszPath);
		LPWSTR lpEnd = &bsPath[::SysStringLen(bsPath) - 1];
		while (lpEnd > bsPath && !*pbsPath) {
			LPWSTR lp = StrRChr(bsPath, lpEnd, '\\');
			lpEnd = lp;
			if (lp) {
				lp[0] = NULL;
				LPITEMIDLIST pidl = teILCreateFromPathEx(bsPath);
				if (pidl) {
					BSTR bs;
					if SUCCEEDED(teGetDisplayNameFromIDList(&bs, pidl, SHGDN_FORADDRESSBAR)) {
						if (StrChr(bsPath, '\\') && !StrChr(bs, '\\')) {
							::SysFreeString(bs);
							teGetDisplayNameFromIDList(&bs, pidl, SHGDN_FORADDRESSBAR | SHGDN_FORPARSING);
						}
						lp[0] = '\\';
						*pbsPath = teSysAllocStringLenEx(bs, SysStringLen(bs) + lstrlen(lp) + 1);
						lstrcat(*pbsPath, lp);
						::SysFreeString(bs);
					}
					teCoTaskMemFree(pidl);
				}
				lp[0] = '\\';
			}
		}
		::SysFreeString(bsPath);
	}
	return *pbsPath != NULL;
}

VOID teFreeLibraries()
{
	while (!g_pFreeLibrary.empty()) {
		FreeLibrary(*g_pFreeLibrary.begin());
		g_pFreeLibrary.pop_front();
	}
}

VOID teGetDisplayNameOf(VARIANT *pv, int uFlags, VARIANT *pVarResult)
{
	try {
		IUnknown *punk;
		if (pv->vt == VT_BSTR) {
			if (!(uFlags & SHGDN_INFOLDER)) {
				VariantCopy(pVarResult, pv);
				return;
			}
		} else if (FindUnknown(pv, &punk)) {
			BSTR bs;
			if (teGetStrFromFolderItem(&bs, punk)) {
				if (uFlags & SHGDN_INFOLDER) {
					if (bs && lstrlen(bs) > 2) {
						LPWSTR pszName = PathFindFileName(bs);
						if (pszName && pszName[0] != ':') {
							teSetSZ(pVarResult, PathFindFileName(bs));
							return;
						}
					}
				}
				if (uFlags & SHGDN_FORADDRESSBAR) {
					BSTR bsLocal;
					if (teLocalizePath(bs, &bsLocal)) {
						teSetSZ(pVarResult, bsLocal);
						::SysFreeString(bsLocal);
						return;
					}
				}
				if (!(uFlags & SHGDN_INFOLDER)) {
					teSetSZ(pVarResult, bs);
					return;
				}
			}
		}
		LPITEMIDLIST pidl;
		if (teGetIDListFromVariant(&pidl, pv)) {
			GetVarPathFromIDList(pVarResult, pidl, uFlags);
			teCoTaskMemFree(pidl);
		}
	} catch(...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"teGetDisplayNameOf";
#endif
	}
}

BOOL GetVarPathFromFolderItem(FolderItem *pFolderItem, VARIANT *pVarResult)
{
	FolderItem *pid = NULL;
	VARIANT v;
	if (pFolderItem) {
		VariantInit(&v);
		if SUCCEEDED(pFolderItem->QueryInterface(IID_PPV_ARGS(&v.pdispVal))) {
			v.vt = VT_DISPATCH;
			VariantInit(pVarResult);
			teGetDisplayNameOf(&v, SHGDN_FORADDRESSBAR | SHGDN_FORPARSING | SHGDN_FORPARSINGEX | SHGDN_ORIGINAL, pVarResult);
			VariantClear(&v);
			if (pVarResult->vt == VT_BSTR && teStartsText(L"search-ms:", pVarResult->bstrVal)) {
				LPITEMIDLIST pidl;
				if (teGetIDListFromObject(pFolderItem, &pidl)) {
					VariantClear(pVarResult);
					GetVarArrayFromIDList(pVarResult, pidl);
					teCoTaskMemFree(pidl);
				}
			}
			return TRUE;
		}
	}
	return FALSE;
}

VOID teGetRootFromDataObj(BSTR *pbs, IDataObject *pDataObj)
{
	FolderItems *pFolderItems;
	if SUCCEEDED(pDataObj->QueryInterface(IID_PPV_ARGS(&pFolderItems))) {
		VARIANT v;
		teSetLong(&v, -1);
		FolderItem *pFolderItem;
		if SUCCEEDED(pFolderItems->Item(v, &pFolderItem)) {
			pFolderItem->get_Path(pbs);
			pFolderItem->Release();
		}
		pFolderItems->Release();
	}
}

//Dispatch API

VOID teApiMemory(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	CteMemory *pMem;
	char *pc = NULL;;
	LPWSTR sz = NULL;
	int nCount = 1;
	IUnknown *punk;
	IStream *pStream = NULL;
	int i = 0;
	if (pDispParams->rgvarg[nArg].vt == VT_BSTR) {
		sz = pDispParams->rgvarg[nArg].bstrVal;
		i = GetSizeOfStruct(sz);
	} else if (pDispParams->rgvarg[nArg].vt & VT_ARRAY) {
		pc = pDispParams->rgvarg[nArg].pcVal;
		nCount = pDispParams->rgvarg[nArg].parray->rgsabound[0].cElements;
		i = SizeOfvt(pDispParams->rgvarg[nArg].vt);
		sz = L"SAFEARRAY";
	} else if (nArg >= 1 && FindUnknown(&pDispParams->rgvarg[nArg], &punk)) {
		if SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pStream))) {
			sz = L"IStream";
			i = 1;
			ULARGE_INTEGER uliSize;
			LARGE_INTEGER liOffset;
			liOffset.QuadPart = 0;
			pStream->Seek(liOffset, STREAM_SEEK_END, &uliSize);
			pStream->Seek(liOffset, STREAM_SEEK_SET, NULL);
			nCount = uliSize.QuadPart < param[1].lVal ? uliSize.LowPart : param[1].lVal;
		}
	}
	if (i == 0) {
		i = param[0].lVal;
		if (i == 0) {
			return;
		}
	}
	BSTR bs = NULL;
	if (nArg >= 1 && !pStream) {
		if (i == 2 && pDispParams->rgvarg[nArg - 1].vt == VT_BSTR) {
			bs = pDispParams->rgvarg[nArg - 1].bstrVal;
			nCount = SysStringByteLen(bs) / 2 + 1;
		} else {
			nCount = param[1].lVal;
		}
		if (nCount < 1) {
			nCount = 1;
		}
		if (nArg >= 2) {
			pc = param[2].pcVal;
		}
	}
	pMem = new CteMemory(i * nCount, pc, nCount, sz);
	if (bs) {
		::CopyMemory(pMem->m_pc, bs, nCount * 2);
	}
	if (pStream) {
		ULONG ulBytesRead;
		pStream->Read(pMem->m_pc, nCount, &ulBytesRead);
		pStream->Release();
	}
	teSetObjectRelease(pVarResult, pMem);
}

VOID teApiContextMenu(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IDataObject *pDataObj = NULL;
	CteContextMenu *pdispCM;
	if (nArg >= 6) {
		IShellExtInit *pSEI;
		HMODULE hDll = teCreateInstanceV(&pDispParams->rgvarg[nArg], &pDispParams->rgvarg[nArg - 1], IID_PPV_ARGS(&pSEI));
		if (pSEI) {
			LPITEMIDLIST pidl;
			if (teGetIDListFromVariant(&pidl, &pDispParams->rgvarg[nArg - 2])) {
				if (GetDataObjFromVariant(&pDataObj, &pDispParams->rgvarg[nArg - 3])) {
					HKEY hKey;
					if (RegOpenKeyEx(param[4].hkeyVal, param[5].lpcwstr, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
						if SUCCEEDED(pSEI->Initialize(pidl, pDataObj, hKey)) {
							IUnknown *punk = NULL;
							FindUnknown(&pDispParams->rgvarg[nArg - 6], &punk);
							pdispCM = new CteContextMenu(pSEI, pDataObj, punk);
							pDataObj = NULL;
							pdispCM->m_pDll = new CteDll(hDll);
							hDll = NULL;
							teSetObjectRelease(pVarResult, pdispCM);
						}
						RegCloseKey(hKey);
					}
					SafeRelease(&pDataObj);
				}
				teCoTaskMemFree(pidl);
			}
			pSEI->Release();
		}
		teFreeLibrary(hDll, 0);
	} else if (GetDataObjFromVariant(&pDataObj, &pDispParams->rgvarg[nArg])) {
		long nCount;
		LPITEMIDLIST *ppidllist = IDListFormDataObj(pDataObj, &nCount);
#ifdef _2000XP
		if (nCount >= 2 && !g_bUpperVista && ppidllist[0] && ILIsEmpty(ppidllist[0])) {
			for (int i = nCount; i-- > 0;) {
				LPITEMIDLIST pidlFull = ILCombine(ppidllist[0], ppidllist[i + 1]);
				BSTR bs;
				if SUCCEEDED(teGetDisplayNameFromIDList(&bs, pidlFull, SHGDN_FORADDRESSBAR | SHGDN_FORPARSING)) {
					teILCloneReplace(&ppidllist[i + 1], teILCreateFromPath(bs));
					SysFreeString(bs);
				}
				teCoTaskMemFree(ppidllist[i + 1]);
				teCoTaskMemFree(pidlFull);
			}
			teILFreeClear(&ppidllist[0]);
		}
#endif
		AdjustIDList(ppidllist, nCount);
		if (nCount >= 1) {
			IShellFolder *pSF;
			if (GetShellFolder(&pSF, ppidllist[0])) {
				IContextMenu *pCM;
//				if SUCCEEDED(CDefFolderMenu_Create2(ppidllist[0], g_hwndMain, nCount, const_cast<LPCITEMIDLIST *>(&ppidllist[1]), pSF, NULL, 0, NULL, &pCM)) {
				if SUCCEEDED(pSF->GetUIObjectOf(g_hwndMain, nCount, const_cast<LPCITEMIDLIST *>(&ppidllist[1]), IID_IContextMenu, NULL, (LPVOID*)&pCM)) {
					IUnknown *punk = NULL;
					if (nArg >= 1) {
						FindUnknown(&pDispParams->rgvarg[nArg - 1], &punk);
					}
					pdispCM = new CteContextMenu(pCM, pDataObj, punk);
					pDataObj = NULL;
					teSetObjectRelease(pVarResult, pdispCM);
					pCM->Release();
				}
			}
			do {
				teCoTaskMemFree(ppidllist[nCount]);
			} while (--nCount >= 0);
			delete [] ppidllist;
		}
	}
	if (pDataObj) {
		pDataObj->Release();
	}
}

VOID teApiDropTarget(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	FolderItem *pid;
	GetFolderItemFromVariant(&pid, &pDispParams->rgvarg[nArg]);
	LPITEMIDLIST pidl;
	if (teGetIDListFromObject(pid, &pidl)) {
		IShellFolder *pSF;
		LPCITEMIDLIST pidlPart;
		if SUCCEEDED(SHBindToParent(pidl, IID_PPV_ARGS(&pSF), &pidlPart)) {
			IDropTarget *pDT = NULL;
			pSF->GetUIObjectOf(g_hwndMain, 1, &pidlPart, IID_IDropTarget, NULL, (LPVOID*)&pDT);
			teSetObjectRelease(pVarResult, new CteDropTarget(pDT, pid));
			pDT && pDT->Release();
			pSF->Release();
		}
		teCoTaskMemFree(pidl);
	}
	pid->Release();
}

VOID teApiDataObject(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetObjectRelease(pVarResult, new CteFolderItems(NULL, NULL));
}

VOID teApiOleCmdExec(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IUnknown *punk;
	if (FindUnknown(&pDispParams->rgvarg[nArg], &punk)) {
		IOleCommandTarget *pCT;
		if SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pCT))) {
			GUID *pguid = NULL;
			teCLSIDFromString(param[1].lpcolestr, pguid);
			pCT->Exec(pguid, param[2].olecmdid, param[3].olecmdexecopt, &pDispParams->rgvarg[nArg - 4], pVarResult);
			pCT->Release();
		}
	}
}

VOID teApisizeof(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, GetSizeOf(&pDispParams->rgvarg[nArg]));
}

VOID teApiLowPart(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, param[0].li.LowPart);
}

VOID teApiULowPart(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetULL(pVarResult, param[0].uli.LowPart);
}

VOID teApiHighPart(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, param[0].li.HighPart);
}

VOID teApiQuadPart(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	if (nArg >= 1) {
		LARGE_INTEGER li;
		li.LowPart = param[0].dword;
		li.HighPart = param[1].lVal;
		teSetLL(pVarResult, li.QuadPart);
		return;
	}
	teSetLL(pVarResult, param[0].llVal);
}

VOID teApiUQuadPart(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	if (nArg >= 1) {
		ULARGE_INTEGER li;
		li.LowPart = param[0].dword;
		li.HighPart = param[1].lVal;
		teSetULL(pVarResult, li.QuadPart);
		return;
	}
	teSetULL(pVarResult, teGetU(param[0].llVal));
}

VOID teApipvData(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	if (pDispParams->rgvarg[nArg].vt & VT_ARRAY) {
		teSetPtr(pVarResult, pDispParams->rgvarg[nArg].parray->pvData);
	}
}

VOID teApiExecMethod(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IDispatch *pdisp;
	if (GetDispatch(&pDispParams->rgvarg[nArg], &pdisp)) {
		IDispatch *pdisp1 = NULL;
		if (GetDispatch(&pDispParams->rgvarg[nArg - 2], &pdisp1)) {
			int nCount = teGetObjectLength(pdisp);
			VARIANTARG *pv = GetNewVARIANT(nCount);
			for (int i = nCount; i-- > 0;) {
				teGetPropertyAt(pdisp1, nCount - i - 1, &pv[i]);//Reverse argument
			}
			teExecMethod(pdisp, param[1].lpolestr, pVarResult, -nCount, pv);
			for (int i = nCount; i-- > 0;) {
				tePutPropertyAt(pdisp1, nCount - i - 1, &pv[i]);//for Reference
			}
			teClearVariantArgs(nCount, pv);
			pdisp1->Release();
		}
		pdisp->Release();
	}
}

HRESULT teInitStorage(LPVARIANTARG pvDllFile, LPVARIANTARG pvClass, LPWSTR lpwstr, HMODULE *phDll, IStorage **ppStorage)
{
	HRESULT hr = E_FAIL;
	*phDll = teCreateInstanceV(pvDllFile, pvClass, IID_PPV_ARGS(ppStorage));
	if (*ppStorage) {
		IPersistFile *pPF;
		hr = (*ppStorage)->QueryInterface(IID_PPV_ARGS(&pPF));
		if SUCCEEDED(hr) {
			hr = pPF->Load(lpwstr, STGM_READ);
			pPF->Release();
		} else {
			IPersistFolder *pPF2;
			hr = (*ppStorage)->QueryInterface(IID_PPV_ARGS(&pPF2));
			if SUCCEEDED(hr) {
				LPITEMIDLIST pidl = teILCreateFromPath(lpwstr);
				if (pidl) {
					hr = pPF2->Initialize(pidl);
					teCoTaskMemFree(pidl);
				}
				pPF2->Release();
			}
		}
	}
	return hr;
}

VOID teApiExtract(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	HRESULT hr = E_FAIL;
	IStorage *pStorage = NULL;
	HMODULE hDll = NULL;
	IProgressDialog *ppd = NULL;
	teCreateInstance(CLSID_ProgressDialog, NULL, NULL, IID_PPV_ARGS(&ppd));
	try {
		if (ppd) {
			ppd->StartProgressDialog(g_hwndMain, NULL, PROGDLG_NORMAL | PROGDLG_AUTOTIME, NULL);
#ifdef _2000XP
			ppd->SetAnimation(g_hShell32, 161);
#endif
			ppd->SetLine(1, PathFindFileName(param[2].lpwstr), TRUE, NULL);
		}
		hr = teInitStorage(&pDispParams->rgvarg[nArg], &pDispParams->rgvarg[nArg - 1], param[2].lpwstr, &hDll, &pStorage);
		if SUCCEEDED(hr) {
			int nItems = 0, nCount = 0, nBase = lstrlen(param[3].lpwstr) + 1;
			hr = teExtract(pStorage, param[3].lpwstr, ppd, &nCount, -1, nBase);
			if SUCCEEDED(hr) {
				hr = teExtract(pStorage, param[3].lpwstr, ppd, &nItems, nCount, nBase);
			}
		}
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"ApiExtract";
#endif
	}
	SafeRelease(&pStorage);
	teFreeLibrary(hDll, 0);
	if (ppd) {
		ppd->StopProgressDialog();
		SafeRelease(&ppd);
	}
	teSetLong(pVarResult, hr);
}

VOID teApiQuadAdd(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLL(pVarResult, param[0].llVal + param[1].llVal);
}

VOID teApiUQuadAdd(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetULL(pVarResult, teGetU(param[0].llVal) + teGetU(param[1].llVal));
}

VOID teApiUQuadSub(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLL(pVarResult, teGetU(param[0].llVal) - teGetU(param[1].llVal));
}

VOID teApiUQuadCmp(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	LONGLONG ll = teGetU(param[0].llVal) - teGetU(param[1].llVal);
	int i = 0;
	if (ll > 0) {
		i = 1;
	} else if (ll < 0) {
		i = -1;
	}
	teSetLong(pVarResult, i);
}

VOID teApiFindWindow(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, FindWindow(param[0].lpcwstr, param[1].lpcwstr));
}

VOID teApiFindWindowEx(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, FindWindowEx(param[0].hwnd, param[1].hwnd, param[2].lpcwstr, param[3].lpcwstr));
}

VOID teApiOleGetClipboard(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	if (pVarResult) {
		IDataObject *pDataObj;
		if (OleGetClipboard(&pDataObj) == S_OK) {
			FolderItems *pFolderItems = new CteFolderItems(pDataObj, NULL);
			if (teSetObject(pVarResult, pFolderItems)) {
				STGMEDIUM Medium;
				if (pDataObj->GetData(&DROPEFFECTFormat, &Medium) == S_OK) {
					VARIANT v;
					DWORD *pdwEffect = (DWORD *)GlobalLock(Medium.hGlobal);
					try {
						teSetLong(&v, *pdwEffect);
					} catch (...) {
						g_nException = 0;
#ifdef _DEBUG
						g_strException = L"ApiOleGetClipboard";
#endif
					}
					GlobalUnlock(Medium.hGlobal);
					ReleaseStgMedium(&Medium);
					IUnknown *punk;
					if (FindUnknown(pVarResult, &punk)) {
						tePutProperty(punk, L"dwEffect", &v);
					}
				}
			}
			pFolderItems->Release();
			pDataObj->Release();
		}
	}
}

VOID teApiOleSetClipboard(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	LONG lResult = E_FAIL;
	IDataObject *pDataObj;
	if (GetDataObjFromVariant(&pDataObj, &pDispParams->rgvarg[nArg])) {
		lResult = OleSetClipboard(pDataObj);
		teSysFreeString(&g_bsClipRoot);
		teGetRootFromDataObj(&g_bsClipRoot, pDataObj);
//		Don't release pDataObj.
//		pDataObj->Release();
	}
	teSetLong(pVarResult, lResult);
}

VOID teApiPathMatchSpec(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, tePathMatchSpec(param[0].lpcwstr, param[1].lpwstr));
}

VOID teApiCommandLineToArgv(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	int nLen = 0;
	LPTSTR *lplpszArgs = NULL;
	IDispatch *pArray = NULL;
	GetNewArray(&pArray);
	if (param[0].lpcwstr && param[0].lpcwstr[0]) {
		lplpszArgs = CommandLineToArgvW(param[0].lpcwstr, &nLen);
		for (int i = nLen; i-- > 0;) {
			VARIANT v;
			teSetSZ(&v, lplpszArgs[i]);
			int n = ::SysStringLen(v.bstrVal);
			if (v.bstrVal[n - 1] == '"') {
				v.bstrVal[n - 1] = '\\';
			}
			tePutPropertyAt(pArray, i, &v);
		}
		LocalFree(lplpszArgs);
	}
	teSetObjectRelease(pVarResult, new CteDispatchEx(pArray));
	pArray->Release();
}

VOID teApiILIsEqual(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	FolderItem *pid1, *pid2;
	GetFolderItemFromVariant(&pid1, &pDispParams->rgvarg[nArg]);
	GetFolderItemFromVariant(&pid2, &pDispParams->rgvarg[nArg - 1]);
	teSetBool(pVarResult, teILIsEqual(pid1, pid2));
	pid2->Release();
	pid1->Release();
}

VOID teApiILClone(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	FolderItem *pid;
	GetFolderItemFromVariant(&pid, &pDispParams->rgvarg[nArg]);
	teSetObjectRelease(pVarResult, pid);
}

VOID teApiILIsParent(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	LPITEMIDLIST pidl, pidl2;
	if (teGetIDListFromVariant(&pidl, &pDispParams->rgvarg[nArg])) {
		if (teGetIDListFromVariant(&pidl2, &pDispParams->rgvarg[nArg - 1])) {
			teSetBool(pVarResult, ::ILIsParent(pidl, pidl2, param[2].boolVal));
			teCoTaskMemFree(pidl2);
		}
		teCoTaskMemFree(pidl);
	}
}

VOID teApiILRemoveLastID(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	LPITEMIDLIST pidl;
	if (teGetIDListFromVariant(&pidl, &pDispParams->rgvarg[nArg])) {
		if (!ILIsEmpty(pidl)) {
			if (::ILRemoveLastID(pidl)) {
				teSetIDList(pVarResult, ILIsEqual(pidl, g_pidls[CSIDL_INTERNET]) ? g_pidls[CSIDL_DESKTOP] : pidl);
			}
		}
		teCoTaskMemFree(pidl);
	}
}

VOID teApiILFindLastID(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	LPITEMIDLIST pidl, pidlLast;
	if (teGetIDListFromVariant(&pidl, &pDispParams->rgvarg[nArg])) {
		pidlLast = ILFindLastID(pidl);
		if (pidlLast) {
			teSetIDList(pVarResult, pidl);
		}
		teCoTaskMemFree(pidl);
	}
}

VOID teApiILIsEmpty(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	FolderItem *pid1;
	GetFolderItemFromVariant(&pid1, &pDispParams->rgvarg[nArg]);
	CteFolderItem *pid2 = new CteFolderItem(NULL);
	pid2->Initialize(g_pidls[CSIDL_DESKTOP]);
	teSetBool(pVarResult, teILIsEqual(pid1, static_cast<FolderItem *>(pid2)));
	pid2->Release();
	pid1->Release();
}

VOID teApiILGetParent(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	if (pVarResult) {
		FolderItem *pFolderItem, *pFolderItem1;
		GetFolderItemFromVariant(&pFolderItem, &pDispParams->rgvarg[nArg]);
		if (teILGetParent(pFolderItem, &pFolderItem1)) {
			teSetObjectRelease(pVarResult, pFolderItem1);
		}
		pFolderItem->Release();
	}
}

VOID teApiFindFirstFile(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, FindFirstFile(param[0].lpcwstr, param[1].lpwin32_find_data));
}

VOID teApiWindowFromPoint(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	POINT pt;
	GetPointFormVariant(&pt, &pDispParams->rgvarg[nArg]);
	teSetPtr(pVarResult, WindowFromPoint(pt));
}

VOID teApiGetThreadCount(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, g_nThreads);
}

VOID teApiDoEvents(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	MSG msg;
	BOOL bResult = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	if (bResult) {
		if (MessageProc(&msg) != S_OK) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	teSetBool(pVarResult, bResult);
}

VOID teApisscanf(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	if (pVarResult) {
		LPWSTR pszData = param[0].lpwstr;
		LPWSTR pszFormat = param[1].lpwstr;
		if (pszData && pszFormat) {
			int nPos = 0;
			while (pszFormat[nPos] && pszFormat[nPos++] != '%') {
			}
			while (WCHAR wc = pszFormat[nPos]) {
				nPos++;
				if (wc == '%') {
					break;
				}
				if (StrChrIW(L"diuoxc", wc)) {//Integer
					LONGLONG Result = 0;
					swscanf_s(pszData, pszFormat, &Result);
					teSetLL(pVarResult, Result);
					return;
				}
				if (StrChrIW(L"efga", wc)) {//Real
					pVarResult->vt = VT_R8;
					pVarResult->dblVal = 0;
					swscanf_s(pszData, pszFormat, &pVarResult->dblVal);
					return;
				}
				if (StrChrIW(L"s", wc)) {//String
					int nLen = lstrlen(pszData);
					BSTR bs = ::SysAllocStringLen(NULL, nLen);
					swscanf_s(pszData, pszFormat, bs, nLen);
					teSetBSTR(pVarResult, &bs, -1);
				}
			}
		}
	}
}

VOID teApiSetFileTime(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	BOOL bResult = FALSE;
	BSTR bs = NULL;
	if (pDispParams->rgvarg[nArg].vt == VT_BSTR) {
		bs = ::SysAllocString(pDispParams->rgvarg[nArg].bstrVal);
	} else {
		LPITEMIDLIST pidl;
		if (teGetIDListFromVariant(&pidl, &pDispParams->rgvarg[nArg])) {
			teGetDisplayNameFromIDList(&bs, pidl, SHGDN_FORPARSING);
			teCoTaskMemFree(pidl);
		}
	}
	if (bs) {
		HANDLE hFile = ::CreateFile(bs, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0);
		if (hFile != INVALID_HANDLE_VALUE) {
			FILETIME *ppft[3];
			for (int i = _countof(ppft); i--;) {
				ppft[i] = NULL;
				DATE dt;
				if (teGetVariantTime(&pDispParams->rgvarg[nArg - i - 1], &dt) && dt >= 0) {
					ppft[i] = new FILETIME[1];
					teVariantTimeToFileTime(dt, ppft[i]);
				}
			}
			bResult = ::SetFileTime(hFile, ppft[0], ppft[1], ppft[2]);
			for (int i = _countof(ppft); i--;) {
				if (ppft[i]) {
					delete [] ppft[i];
				}
			}
			::CloseHandle(hFile);
		}
		teSysFreeString(&bs);
	}
	teSetBool(pVarResult, bResult);
}

VOID teApiILCreateFromPath(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	if (param[1].boolVal) {
		if (pVarResult) {
			LPITEMIDLIST pidl;
			if (teGetIDListFromVariant(&pidl, &pDispParams->rgvarg[nArg])) {
				GetVarArrayFromIDList(pVarResult, pidl);
				teCoTaskMemFree(pidl);
			}
		}
	} else {
		FolderItem *pid;
		GetFolderItemFromVariant(&pid, &pDispParams->rgvarg[nArg]);
		teSetObjectRelease(pVarResult, pid);
	}
}

VOID teApiGetProcObject(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	HMODULE hDll = LoadLibrary(param[0].lpcwstr);
	if (hDll) {
		CHAR szProcNameA[MAX_LOADSTRING];
		LPSTR lpProcNameA = szProcNameA;
		if (pDispParams->rgvarg[nArg - 1].vt == VT_BSTR) {
			WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)pDispParams->rgvarg[nArg - 1].bstrVal, -1, lpProcNameA, MAX_LOADSTRING, NULL, NULL);
		} else {
			lpProcNameA = MAKEINTRESOURCEA(param[1].word);
		}
		LPFNGetProcObjectW lpfnGetProcObjectW = (LPFNGetProcObjectW)GetProcAddress(hDll, lpProcNameA);
		if (lpfnGetProcObjectW) {
			if (nArg >= 2) {
				VariantCopy(pVarResult, &pDispParams->rgvarg[nArg - 2]);
			}
			lpfnGetProcObjectW(pVarResult);
			IDispatch *pdisp;
			if (GetDispatch(pVarResult, &pdisp)) {
				CteDispatch *odisp = new CteDispatch(pdisp, 4);
				odisp->m_pDll = new CteDll(hDll);
				hDll = 0;
				VariantClear(pVarResult);
				teSetObjectRelease(pVarResult, odisp);
			}
		}
		teFreeLibrary(hDll, 0);
	}
}

VOID teApiSetCurrentDirectory(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, SetCurrentDirectory(param[0].lpcwstr));//use wsh.CurrentDirectory
}

VOID teApiSetDllDirectory(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
#ifdef _2000XP
	if (lpfnSetDllDirectoryW) {
		teSetBool(pVarResult, lpfnSetDllDirectoryW(param[0].lpcwstr));
	}
#else
	teSetBool(pVarResult, SetDllDirectory(param[0].lpcwstr));
#endif
}

VOID teApiPathIsNetworkPath(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, tePathIsNetworkPath(param[0].lpcwstr));
}

VOID teApiRegisterWindowMessage(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, RegisterWindowMessage(param[0].lpcwstr));
}

VOID teApiStrCmpI(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, lstrcmpi(param[0].lpcwstr, param[1].lpcwstr));
}

VOID teApiStrLen(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, lstrlen(param[0].lpcwstr));
}

VOID teApiStrCmpNI(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, StrCmpNI(param[0].lpcwstr, param[1].lpcwstr, param[2].intVal));
}

VOID teApiStrCmpLogical(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, StrCmpLogicalW(param[0].lpcwstr, param[1].lpcwstr));
}

VOID teApiPathQuoteSpaces(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	if (param[0].bstrVal) {
		BSTR bsResult = teSysAllocStringLen(param[0].bstrVal, ::SysStringLen(param[0].bstrVal) + 2);
		PathQuoteSpaces(bsResult);
		teSetBSTR(pVarResult, &bsResult, -1);
	}
}

VOID teApiPathUnquoteSpaces(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	if (param[0].lpolestr) {
		BSTR bsResult = ::SysAllocString(param[0].lpolestr);
		PathUnquoteSpaces(bsResult);
		teSetBSTR(pVarResult, &bsResult, -1);
	}
}

VOID teApiGetShortPathName(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	if (param[0].lpcwstr) {
		int nLen = GetShortPathName(param[0].lpcwstr, NULL, 0);
		BSTR bsResult = ::SysAllocStringLen(NULL, nLen + MAX_PATH);
		nLen = GetShortPathName(param[0].lpcwstr, bsResult, nLen);
		teSetBSTR(pVarResult, &bsResult, nLen);
	}
}

VOID teApiPathCreateFromUrl(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	if (param[0].bstrVal) {
		DWORD dwLen = ::SysStringLen(param[0].bstrVal) + MAX_PATH;
		BSTR bsResult = ::SysAllocStringLen(NULL, dwLen);
		if SUCCEEDED(PathCreateFromUrl(param[0].lpcwstr, bsResult, &dwLen, NULL)) {
			teSetBSTR(pVarResult, &bsResult, dwLen);
		} else {
			::SysFreeString(bsResult);
			teSetSZ(pVarResult, NULL);
		}
	}
}

VOID teApiUrlCreateFromPath(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	if (param[0].bstrVal) {
		DWORD dwLen = ::SysStringLen(param[0].bstrVal) + MAX_PATH;
		BSTR bsResult = ::SysAllocStringLen(NULL, dwLen);
		if SUCCEEDED(UrlCreateFromPath(param[0].lpcwstr, bsResult, &dwLen, NULL)) {
			teSetBSTR(pVarResult, &bsResult, dwLen);
		} else {
			::SysFreeString(bsResult);
			teSetSZ(pVarResult, NULL);
		}
	}
}

VOID teApiPathSearchAndQualify(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	if (param[0].bstrVal) {
		UINT uLen = ::SysStringLen(param[0].bstrVal) + MAX_PATH;
		BSTR bsResult = teSysAllocStringLen(param[0].lpolestr, uLen);
		PathSearchAndQualify(param[0].lpcwstr, bsResult, uLen);
		teSetBSTR(pVarResult, &bsResult, -1);
	}
}

VOID teApiPSFormatForDisplay(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	if (param[0].lpcwstr) {
		PROPERTYKEY propKey;
		if SUCCEEDED(lpfnPSPropertyKeyFromStringEx(param[0].lpcwstr, &propKey)) {
			LPWSTR psz;
			if SUCCEEDED(tePSFormatForDisplay(&propKey, &pDispParams->rgvarg[nArg - 1], param[2].dword, &psz)) {
				teSetSZ(pVarResult, psz);
				teCoTaskMemFree(psz);
			}
		}
	}
}

VOID teApiPSGetDisplayName(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	if (param[0].lpcwstr && pVarResult) {
		PROPERTYKEY propKey;
		if SUCCEEDED(lpfnPSPropertyKeyFromStringEx(param[0].lpcwstr, &propKey)) {
			int nFormat = param[1].intVal;
			IShellView *pSV = NULL;
			if (nFormat == 0 && g_pTC) {
				CteShellBrowser *pSB = g_pTC->GetShellBrowser(g_pTC->m_nIndex);
				if (pSB) {
					pSV = pSB->m_pShellView;
				}
			}
			pVarResult->bstrVal = tePSGetNameFromPropertyKeyEx(propKey, nFormat, pSV);
			pVarResult->vt = VT_BSTR;
		}
	}
}

VOID teApiGetCursorPos(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, GetCursorPos(param[0].lppoint));
}

VOID teApiGetKeyboardState(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, GetKeyboardState(param[0].pbVal));
}

VOID teApiSetKeyboardState(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, SetKeyboardState(param[0].pbVal));
}

VOID teApiGetVersionEx(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, lpfnRtlGetVersion ? !lpfnRtlGetVersion(param[0].prtl_osversioninfoexw) : GetVersionEx(param[0].lposversioninfo));
}

VOID teApiChooseFont(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, ChooseFont(param[0].lpchoosefont));
}

VOID teApiChooseColor(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, ::ChooseColor(param[0].lpchoosecolor));
}

VOID teApiTranslateMessage(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, ::TranslateMessage(param[0].lpmsg));
}

VOID teApiShellExecuteEx(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, ::ShellExecuteEx(param[0].lpshellexecuteinfo));
}

VOID teApiCreateFontIndirect(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, ::CreateFontIndirect(param[0].plogfont));
}

VOID teApiCreateIconIndirect(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, ::CreateIconIndirect(param[0].piconinfo));
}

VOID teApiFindText(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, ::FindText(param[0].lpfindreplace));
}

VOID teApiFindReplace(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, ::ReplaceText(param[0].lpfindreplace));
}

VOID teApiDispatchMessage(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, ::DispatchMessage(param[0].lpmsg));
}

VOID teApiPostMessage(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	PostMessage(param[0].hwnd, param[1].uintVal, param[2].wparam, param[3].lparam);
}

VOID teApiSHFreeNameMappings(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	SHFreeNameMappings(param[0].handle);
}

VOID teApiCoTaskMemFree(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teCoTaskMemFree(param[0].lpvoid);
}

VOID teApiSleep(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	Sleep(param[0].dword);
#ifdef _DEBUG
	teSetPtr(pVarResult, GetCurrentThreadId());
#endif
}

VOID teApiShRunDialog(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	if (lpfnSHRunDialog) {
		lpfnSHRunDialog(param[0].hwnd, param[1].hicon, param[2].lpwstr, param[3].lpwstr, param[4].lpwstr, param[5].dword);
	}
}

VOID teApiDragAcceptFiles(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	DragAcceptFiles(param[0].hwnd, param[1].boolVal);
}

VOID teApiDragFinish(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	DragFinish(param[0].hdrop);
}

VOID teApimouse_event(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	mouse_event(param[0].dword, param[1].dword, param[2].dword, param[3].dword, param[4].ulong_ptr);
}

VOID teApikeybd_event(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	keybd_event(param[0].bVal, param[1].bVal, param[2].dword, param[3].ulong_ptr);
}

VOID teApiSHChangeNotify(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	VARIANT pv[2];
	BYTE type = param[1].bVal;
	for (int i = 2; i--;) {
		VariantInit(&pv[i]);
		pv[i].bstrVal = NULL;
		if (type == SHCNF_IDLIST) {
			teGetIDListFromVariant((LPITEMIDLIST *)&pv[i].bstrVal, &pDispParams->rgvarg[nArg - i - 2]);
		} else if (type == SHCNF_PATH) {
			teVariantChangeType(&pv[i], &pDispParams->rgvarg[nArg - i - 2], VT_BSTR);
		}
	}
	SHChangeNotify(param[0].lVal, param[1].uintVal, pv[0].bstrVal, pv[1].bstrVal);
	for (int i = 2; i--;) {
		if (pv[i].vt != VT_BSTR) {
			teCoTaskMemFree(pv[i].bstrVal);
		} else {
			VariantClear(&pv[i]);
		}
	}
}

VOID teApiDrawIcon(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, DrawIcon(param[0].hdc, param[1].intVal, param[2].intVal, param[3].hicon));
}

VOID teApiDestroyMenu(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, DestroyMenu(param[0].hmenu));
}

VOID teApiFindClose(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, FindClose(param[0].handle));
}

VOID teApiFreeLibrary(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, teFreeLibrary(param[0].hmodule, param[1].uintVal));
}

VOID teApiImageList_Destroy(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	if (nArg >= 0 && param[1].boolVal) {
		try {
			teSetLong(pVarResult, param[0].pImageList->Release());
		} catch (...) {
			g_nException = 0;
#ifdef _DEBUG
			g_strException = L"ApiImageList_Destroy";
#endif
		}
		return;
	}
	teSetBool(pVarResult, ImageList_Destroy(param[0].himagelist));
}

VOID teApiDeleteObject(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, DeleteObject(param[0].hgdiobj));
}

VOID teApiDestroyIcon(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, DestroyIcon(param[0].hicon));
}

VOID teApiIsWindow(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, IsWindow(param[0].hwnd));
}

VOID teApiIsWindowVisible(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, IsWindowVisible(param[0].hwnd));
}

VOID teApiIsZoomed(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, IsZoomed(param[0].hwnd));
}

VOID teApiIsIconic(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, IsIconic(param[0].hwnd));
}

VOID teApiOpenIcon(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, OpenIcon(param[0].hwnd));
}

VOID teApiSetForegroundWindow(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, teSetForegroundWindow(param[0].hwnd));
}

VOID teApiBringWindowToTop(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, BringWindowToTop(param[0].hwnd));
}

VOID teApiDeleteDC(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, DeleteDC(param[0].hdc));
}

VOID teApiCloseHandle(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, CloseHandle(param[0].handle));
}

VOID teApiIsMenu(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, IsMenu(param[0].hmenu));
}

VOID teApiMoveWindow(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, MoveWindow(param[0].hwnd, param[1].intVal, param[2].intVal, param[3].intVal, param[4].intVal, param[5].boolVal));
#ifdef _2000XP
	CteTreeView *pTV = TVfromhwnd2(param[0].hwnd);
	if (pTV) {
		pTV->SetObjectRect();
	}
#endif
}

VOID teApiSetMenuItemBitmaps(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, SetMenuItemBitmaps(param[0].hmenu, param[1].uintVal, param[2].uintVal, param[3].hbitmap, param[4].hbitmap));
}

VOID teApiShowWindow(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, teShowWindow(param[0].hwnd, param[1].intVal));
}

VOID teApiDeleteMenu(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, DeleteMenu(param[0].hmenu, param[1].uintVal, param[2].uintVal));
}

VOID teApiRemoveMenu(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, RemoveMenu(param[0].hmenu, param[1].uintVal, param[2].uintVal));
}

VOID teApiDrawIconEx(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, DrawIconEx(param[0].hdc, param[1].intVal, param[2].intVal, param[3].hicon,
	param[4].intVal, param[5].intVal, param[6].uintVal, param[7].hbrush, param[8].uintVal));
}

VOID teApiEnableMenuItem(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, EnableMenuItem(param[0].hmenu, param[1].uintVal, param[2].uintVal));
}

VOID teApiImageList_Remove(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, ImageList_Remove(param[0].himagelist, param[1].intVal));
}

VOID teApiImageList_SetIconSize(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, ImageList_SetIconSize(param[0].himagelist, param[1].intVal, param[2].intVal));
}

VOID teApiImageList_Draw(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, ImageList_Draw(param[0].himagelist, param[1].intVal, param[2].hdc,
	param[3].intVal, param[4].intVal, param[5].uintVal));
}

VOID teApiImageList_DrawEx(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, ImageList_DrawEx(param[0].himagelist, param[1].intVal, param[2].hdc,
	param[3].intVal, param[4].intVal, param[5].intVal, param[6].intVal,
	param[7].colorref, param[8].colorref, param[9].uintVal));
}

VOID teApiImageList_SetImageCount(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, ImageList_SetImageCount(param[0].himagelist, param[1].uintVal));
}

VOID teApiImageList_SetOverlayImage(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, ImageList_SetOverlayImage(param[0].himagelist, param[1].intVal, param[2].intVal));
}

VOID teApiImageList_Copy(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, ImageList_Copy(param[0].himagelist, param[1].intVal,
	param[2].himagelist, param[3].intVal, param[4].uintVal));
}

VOID teApiDestroyWindow(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, DestroyWindow(param[0].hwnd));
}

VOID teApiLineTo(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, LineTo(param[0].hdc, param[1].intVal, param[2].intVal));
}

VOID teApiReleaseCapture(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, ReleaseCapture());
}

VOID teApiSetCursorPos(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, SetCursorPos(param[0].intVal, param[1].intVal));
}

VOID teApiDestroyCursor(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, DestroyCursor(param[0].hcursor));
}

VOID teApiSHFreeShared(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, SHFreeShared(param[0].handle, param[1].dword));
}

VOID teApiEndMenu(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, EndMenu());
}

VOID teApiSHChangeNotifyDeregister(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, SHChangeNotifyDeregister(param[0].ulVal));
}

VOID teApiSHChangeNotification_Unlock(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, TRUE);
}

VOID teApiIsWow64Process(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	BOOL bResult = FALSE;
#ifdef _2000XP
	if (lpfnIsWow64Process) {
		lpfnIsWow64Process(param[0].handle, &bResult);
	}
#else
	IsWow64Process(param[0].handle, &bResult);
#endif
	teSetBool(pVarResult, bResult);
}

VOID teApiBitBlt(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, BitBlt(param[0].hdc, param[1].intVal, param[2].intVal, param[3].intVal, param[4].intVal,
	param[5].hdc, param[6].intVal, param[7].intVal, param[8].dword));
}

VOID teApiImmSetOpenStatus(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, ImmSetOpenStatus(param[0].himc, param[1].boolVal));
}

VOID teApiImmReleaseContext(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, ImmReleaseContext(param[0].hwnd, param[1].himc));
}

VOID teApiIsChild(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, IsChild(param[0].hwnd, param[1].hwnd));
}

VOID teApiKillTimer(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, KillTimer(param[0].hwnd, param[1].uint_ptr));
}

VOID teApiAllowSetForegroundWindow(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, AllowSetForegroundWindow(param[0].dword));
}

VOID teApiSetWindowPos(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, SetWindowPos(param[0].hwnd, param[1].hwnd, param[2].intVal, param[3].intVal, param[4].intVal, param[5].intVal, param[6].uintVal));
}

VOID teApiInsertMenu(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, InsertMenu(param[0].hmenu, param[1].uintVal, param[2].uintVal,
	param[3].uint_ptr, param[4].lpcwstr));
}

VOID teApiSetWindowText(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	if (param[0].hwnd == g_hwndMain && !param[2].boolVal) {
		if (param[1].lpcwstr) {
			lstrcpyn(g_szTitle, param[1].lpcwstr, _countof(g_szTitle) - 1);
		} else {
			g_szTitle[0] = NULL;
		}
		SetTimer(g_hwndMain, TET_Title, 100, teTimerProc);
		return;
	}
	teSetBool(pVarResult, SetWindowText(param[0].hwnd, param[1].lpcwstr));
}

VOID teApiRedrawWindow(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	if (!g_bSetRedraw && g_hwndMain == param[0].hwnd) {
		teSetBool(pVarResult, FALSE);
	} else {
		teSetBool(pVarResult, RedrawWindow(param[0].hwnd, param[1].lprect, param[2].hrgn, param[3].uintVal));
	}
}

VOID teApiMoveToEx(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, MoveToEx(param[0].hdc, param[1].intVal, param[2].intVal, param[3].lppoint));
}

VOID teApiInvalidateRect(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, InvalidateRect(param[0].hwnd, param[1].lprect, param[2].boolVal));
}

VOID teApiSendNotifyMessage(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, SendNotifyMessage(param[0].hwnd, param[1].uintVal, param[2].wparam, param[3].lparam));
}

VOID teApiPeekMessage(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, PeekMessage(param[0].lpmsg, param[1].hwnd, param[2].uintVal, param[3].uintVal, param[4].uintVal));
}

VOID teApiSHFileOperation(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	if (nArg >= 4) {
		LPSHFILEOPSTRUCT pFO = new SHFILEOPSTRUCT[1];
		::ZeroMemory(pFO, sizeof(SHFILEOPSTRUCT));
		pFO->hwnd = g_hwndMain;
		pFO->wFunc = param[0].uintVal;
		VARIANT *pv = &pDispParams->rgvarg[nArg - 1];
		BSTR bs = GetLPWSTRFromVariant(pv);
		int nLen = (pv->vt == VT_BSTR ? ::SysStringLen(bs): lstrlen(bs)) + 1;
		bs = pv->vt == VT_BSTR ? teSysAllocStringLenEx(bs, nLen) : teSysAllocStringLen(bs, nLen);
		bs[nLen] = 0;
		pFO->pFrom = bs;
		pv = &pDispParams->rgvarg[nArg - 2];
		bs = GetLPWSTRFromVariant(pv);
		nLen = (pv->vt == VT_BSTR ? ::SysStringLen(bs): lstrlen(bs)) + 1;
		bs = pv->vt == VT_BSTR ? teSysAllocStringLenEx(bs, nLen) : teSysAllocStringLen(bs, nLen);
		bs[nLen] = 0;
		pFO->pTo = bs;
		pFO->fFlags = param[3].fileop_flags;
		if (param[4].boolVal) {
			teSetPtr(pVarResult, _beginthread(&threadFileOperation, 0, pFO));
			return;
		}
		try {
			teSetLong(pVarResult, teSHFileOperation(pFO));
		} catch (...) {
			g_nException = 0;
#ifdef _DEBUG
			g_strException = L"ApiSHFileOperation";
#endif
		}
		::SysFreeString(const_cast<BSTR>(pFO->pTo));
		::SysFreeString(const_cast<BSTR>(pFO->pFrom));
		delete [] pFO;
		return;
	}
	teSetLong(pVarResult, teSHFileOperation(param[0].lpshfileopstruct));
}

VOID teApiGetMessage(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, GetMessage(param[0].lpmsg, param[1].hwnd, param[2].uintVal, param[3].uintVal));
}

VOID teApiGetWindowRect(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, GetWindowRect(param[0].hwnd, param[1].lprect));
}

VOID teApiGetWindowThreadProcessId(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, GetWindowThreadProcessId(param[0].hwnd, param[1].lpdword));
}

VOID teApiGetClientRect(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, GetClientRect(param[0].hwnd, param[1].lprect));
}

VOID teApiSendInput(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, SendInput(param[0].uintVal, param[1].lpinput, param[2].intVal));
}

VOID teApiScreenToClient(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, ScreenToClient(param[0].hwnd, param[1].lppoint));
}

VOID teApiMsgWaitForMultipleObjectsEx(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, MsgWaitForMultipleObjectsEx(param[0].dword, param[1].lphandle, param[2].dword, param[3].dword, param[4].dword));
}

VOID teApiClientToScreen(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, ClientToScreen(param[0].hwnd, param[1].lppoint));
}

VOID teApiGetIconInfo(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, GetIconInfo(param[0].hicon, param[1].piconinfo));
}

VOID teApiFindNextFile(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, FindNextFile(param[0].handle, param[1].lpwin32_find_data));
}

VOID teApiFillRect(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, FillRect(param[0].hdc, param[1].lprect, param[2].hbrush));
}

VOID teApiShell_NotifyIcon(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, Shell_NotifyIcon(param[0].dword, param[1].pnotifyicondata));
}

VOID teApiEndPaint(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, EndPaint(param[0].hwnd, param[1].lppaintstruct));
}

VOID teApiImageList_GetIconSize(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, ImageList_GetIconSize(param[0].himagelist, (int *)&(param[1].lpsize->cx), (int *)&(param[1].lpsize->cy)));
}

VOID teApiGetMenuInfo(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, GetMenuInfo(param[0].hmenu, param[1].lpmenuinfo));
}

VOID teApiSetMenuInfo(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, SetMenuInfo(param[0].hmenu, param[1].lpcmenuinfo));
}

VOID teApiSystemParametersInfo(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, SystemParametersInfo(param[0].uintVal, param[1].uintVal, param[2].lpvoid, param[3].uintVal));
}

VOID teApiGetTextExtentPoint32(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, GetTextExtentPoint32(param[0].hdc, param[1].lpcwstr, lstrlen(param[1].lpcwstr), param[2].lpsize));
}

VOID teApiSHGetDataFromIDList(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	LPITEMIDLIST pidl;
	teGetIDListFromVariant(&pidl, &pDispParams->rgvarg[nArg]);
	if (pidl) {
		HRESULT hr = E_FAIL;
		IShellFolder *pSF;
		LPCITEMIDLIST pidlPart;
		if SUCCEEDED(SHBindToParent(pidl, IID_PPV_ARGS(&pSF), &pidlPart)) {
			hr = teSHGetDataFromIDList(pSF, pidlPart, param[1].intVal, param[2].lpvoid, param[3].intVal);
			pSF->Release();
		}
		teSetLong(pVarResult, hr);
		teCoTaskMemFree(pidl);
	}
}

VOID teApiInsertMenuItem(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, InsertMenuItem(param[0].hmenu, param[1].uintVal, param[2].boolVal, param[3].lpcmenuiteminfo));
}

VOID teApiGetMenuItemInfo(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, GetMenuItemInfo(param[0].hmenu, param[1].uintVal, param[2].boolVal, param[3].lpmenuiteminfo));
}

VOID teApiSetMenuItemInfo(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, SetMenuItemInfo(param[0].hmenu, param[1].uintVal, param[2].boolVal, param[3].lpmenuiteminfo));
}

VOID teApiChangeWindowMessageFilterEx(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, teChangeWindowMessageFilterEx(param[0].hwnd, param[1].uintVal, param[2].dword, param[3].pchangefilterstruct));
}

VOID teApiImageList_SetBkColor(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, ImageList_SetBkColor(param[0].himagelist, param[1].colorref));
}

VOID teApiImageList_AddIcon(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, ImageList_AddIcon(param[0].himagelist, param[1].hicon));
}

VOID teApiImageList_Add(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, ImageList_Add(param[0].himagelist, param[1].hbitmap, param[2].hbitmap));
}

VOID teApiImageList_AddMasked(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, ImageList_AddMasked(param[0].himagelist, param[1].hbitmap, param[2].colorref));
}

VOID teApiGetKeyState(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, GetKeyState(param[0].intVal));
}

VOID teApiGetSystemMetrics(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, GetSystemMetrics(param[0].intVal));
}

VOID teApiGetSysColor(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, GetSysColor(param[0].intVal));
}

VOID teApiGetMenuItemCount(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, GetMenuItemCount(param[0].hmenu));
}

VOID teApiImageList_GetImageCount(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, ImageList_GetImageCount(param[0].himagelist));
}

VOID teApiReleaseDC(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, ReleaseDC(param[0].hwnd, param[1].hdc));
}

VOID teApiGetMenuItemID(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, GetMenuItemID(param[0].hmenu, param[1].intVal));
}

VOID teApiImageList_Replace(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, ImageList_Replace(param[0].himagelist, param[1].intVal,
		param[2].hbitmap, param[3].hbitmap));
}

VOID teApiImageList_ReplaceIcon(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, ImageList_ReplaceIcon(param[0].himagelist, param[1].intVal, param[2].hicon));
}

VOID teApiSetBkMode(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, SetBkMode(param[0].hdc, param[1].intVal));
}

VOID teApiSetBkColor(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, SetBkColor(param[0].hdc, param[1].colorref));
}

VOID teApiSetTextColor(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, SetTextColor(param[0].hdc, param[1].colorref));
}

VOID teApiMapVirtualKey(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, MapVirtualKey(param[0].uintVal, param[1].uintVal));
}

VOID teApiWaitForInputIdle(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, WaitForInputIdle(param[0].handle, param[1].dword));
}

VOID teApiWaitForSingleObject(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, WaitForSingleObject(param[0].handle, param[1].dword));
}

VOID teApiGetMenuDefaultItem(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, GetMenuDefaultItem(param[0].hmenu, param[1].uintVal, param[2].uintVal));
}

VOID teApiSetMenuDefaultItem(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, SetMenuDefaultItem(param[0].hmenu, param[1].uintVal, param[2].uintVal));
}

VOID teApiCRC32(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	BYTE *pc;
	int nLen = 0;
	GetpDataFromVariant(&pc, &nLen, &pDispParams->rgvarg[nArg]);
	teSetLong(pVarResult, CalcCrc32(pc, nLen, param[1].uintVal));
}

VOID teApiSHEmptyRecycleBin(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, SHEmptyRecycleBin(param[0].hwnd, param[1].lpcwstr, param[2].dword));
}

VOID teApiGetMessagePos(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, GetMessagePos());
}

VOID teApiImageList_GetOverlayImage(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	int iResult = -1;
	try {
		param[0].pImageList->GetOverlayImage(param[1].intVal, &iResult);
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"ApiImageList_GetOverlayImage";
#endif
	}
	teSetLong(pVarResult, iResult);
}

VOID teApiImageList_GetBkColor(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, ImageList_GetBkColor(param[0].himagelist));
}

VOID teApiSetWindowTheme(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, SetWindowTheme(param[0].hwnd, param[1].lpcwstr, param[2].lpcwstr));
}

VOID teApiImmGetVirtualKey(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, ImmGetVirtualKey(param[0].hwnd));
}

VOID teApiGetAsyncKeyState(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, GetAsyncKeyState(param[0].intVal));
}

VOID teApiTrackPopupMenuEx(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IUnknown *punk;
	if (nArg >= 6) {
		if (FindUnknown(&pDispParams->rgvarg[nArg - 6], &punk)) {
			IContextMenu *pCM;
			if SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pCM))) {
				IDispatch *pdisp = NULL;
				GetNewArray(&pdisp);
				teArrayPush(pdisp, punk);
				pCM->Release();
				pdisp->QueryInterface(IID_PPV_ARGS(&g_pCM));
				pdisp->Release();
			} else {
				punk->QueryInterface(IID_PPV_ARGS(&g_pCM));
			}
		}
	}
	g_hMenuKeyHook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)MenuKeyProc, hInst, g_dwMainThreadId);
	teSetLong(pVarResult, TrackPopupMenuEx(param[0].hmenu, param[1].uintVal, param[2].intVal, param[3].intVal,
		param[4].hwnd, param[5].lptpmparams));
	UnhookWindowsHookEx(g_hMenuKeyHook);
	SafeRelease(&g_pCM);
}

VOID teApiExtractIconEx(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, ExtractIconEx(param[0].lpcwstr, param[1].intVal, param[2].phicon, param[3].phicon, param[4].uintVal));
}

VOID teApiGetObject(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, GetObject(param[0].hwnd, param[1].intVal, param[2].lpvoid));
}

VOID teApiMultiByteToWideChar(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, MultiByteToWideChar(param[0].uintVal, param[1].dword,
		param[2].lpcstr, param[3].intVal, param[4].lpwstr, param[5].intVal));
}

VOID teApiWideCharToMultiByte(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, WideCharToMultiByte(param[0].uintVal, param[1].dword, param[2].lpcwstr, param[3].intVal,
		param[4].lpstr, param[5].intVal, param[6].lpcstr, param[7].lpbool));
}

VOID teApiGetAttributesOf(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	ULONG lResult = 0;
	IDataObject *pDataObj;
	if (GetDataObjFromVariant(&pDataObj, &pDispParams->rgvarg[nArg])) {
		long nCount;
		LPITEMIDLIST *ppidllist = IDListFormDataObj(pDataObj, &nCount);
		if (ppidllist) {
			AdjustIDList(ppidllist, nCount);
			if (nCount >= 1) {
				IShellFolder *pSF;
				if (GetShellFolder(&pSF, ppidllist[0])) {
					lResult = param[1].ulVal;
					if SUCCEEDED(pSF->GetAttributesOf(nCount, (LPCITEMIDLIST *)&ppidllist[1], (SFGAOF *)(&lResult))) {
						lResult &= param[1].ulVal;
					} else {
						lResult = 0;
					}
					pSF->Release();
				}
			}
			do {
				teCoTaskMemFree(ppidllist[nCount]);
			} while (--nCount >= 0);
			delete [] ppidllist;
		}
		pDataObj->Release();
	}
	teSetLong(pVarResult, lResult);
}

VOID teApiDoDragDrop(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IDataObject *pDataObj;
	if (GetDataObjFromVariant(&pDataObj, &pDispParams->rgvarg[nArg])) {
		DWORD dwEffect = param[1].dword;
		g_nDropState = 1;
		try {
			teSetLong(pVarResult, SHDoDragDrop(NULL, pDataObj, static_cast<IDropSource *>(g_pTE), dwEffect, &dwEffect));
		} catch(...) {
			g_nException = 0;
#ifdef _DEBUG
			g_strException = L"DoDragDrop";
#endif
		}
		IDispatch *pEffect;
		if (nArg >= 2 && GetDispatch(&pDispParams->rgvarg[nArg - 2], &pEffect)) {
			VARIANT v;
			teSetLong(&v, dwEffect);
			tePutPropertyAt(pEffect, 0, &v);
			pEffect->Release();
		}
		pDataObj->Release();
	}
}

VOID teApiSHDoDragDrop(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IDataObject *pDataObj;
	if (GetDataObjFromVariant(&pDataObj, &pDispParams->rgvarg[nArg - 1])) {
		g_pDraggingItems = pDataObj;
		FindUnknown(&pDispParams->rgvarg[nArg - 2], &g_pDraggingCtrl);
		DWORD dwEffect = param[3].dword;
		g_nDropState = param[5].boolVal ? 2 : 1;
		try {
			teSetLong(pVarResult, SHDoDragDrop(param[0].hwnd, pDataObj, static_cast<IDropSource *>(g_pTE), dwEffect, &dwEffect));
		} catch(...) {
			g_nException = 0;
#ifdef _DEBUG
			g_strException = L"SHDoDragDrop";
#endif
		}
		g_pDraggingCtrl = NULL;
		g_pDraggingItems = NULL;
		IDispatch *pEffect;
		if (nArg >= 4 && GetDispatch(&pDispParams->rgvarg[nArg - 4], &pEffect)) {
			VARIANT v;
			teSetLong(&v, dwEffect);
			tePutPropertyAt(pEffect, 0, &v);
			pEffect->Release();
		}
		pDataObj->Release();
	}
}

VOID teApiCompareIDs(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	LPITEMIDLIST pidl1, pidl2;
	LPCITEMIDLIST pidlPart;
	if (teGetIDListFromVariant(&pidl1, &pDispParams->rgvarg[nArg - 1])) {
		if (teGetIDListFromVariant(&pidl2, &pDispParams->rgvarg[nArg - 2])) {
			IShellFolder *pSF;
			LPITEMIDLIST pidlParent = ::ILClone(pidl1);
			::ILRemoveLastID(pidlParent);
			if (ILIsParent(pidlParent, pidl2, TRUE) && SUCCEEDED(SHBindToParent(pidl1, IID_PPV_ARGS(&pSF), &pidlPart))) {
				teSetLong(pVarResult, pSF->CompareIDs(param[0].lparam, pidlPart, ILFindLastID(pidl2)));
			} else {
				SHGetDesktopFolder(&pSF);
				teSetLong(pVarResult, pSF->CompareIDs(param[0].lparam, pidl1, pidl2));
			}
			pSF->Release();
			teCoTaskMemFree(pidlParent);
			teCoTaskMemFree(pidl2);
		}
		teCoTaskMemFree(pidl1);
	}
}

static void threadExecScript(void *args)
{
	::OleInitialize(NULL);
	VARIANT v;
	VariantInit(&v);
	TEExecScript *pES = (TEExecScript *)args;
	try {
		if (pES->pStream) {
			if SUCCEEDED(CoGetInterfaceAndReleaseStream(pES->pStream, IID_PPV_ARGS(&v.pdispVal))) {
				v.vt = VT_DISPATCH;
			}
		}
		ParseScript(pES->bsScript, pES->bsLang, &v, NULL, NULL);
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"threadExecScript";
#endif
	}
	VariantClear(&v);
	teSysFreeString(&pES->bsLang);
	teSysFreeString(&pES->bsScript);
	::OleUninitialize();
	::_endthread();
}

VOID teApiExecScript(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	VARIANT *pv = NULL;
	if (nArg >= 2) {
		pv = &pDispParams->rgvarg[nArg - 2];
		if (param[3].boolVal) {
			TEExecScript *pES = new TEExecScript();
			pES->bsScript = ::SysAllocString(param[0].lpolestr);
			pES->bsLang = ::SysAllocString(param[1].lpolestr);
			if (pv && pv->vt == VT_DISPATCH) {
				CoMarshalInterThreadInterfaceInStream(IID_IDispatch, pv->pdispVal, &pES->pStream);
			}
			teSetPtr(pVarResult, _beginthread(&threadExecScript, 0, pES));
			return;
		}
	}
	param[TE_API_RESULT].lVal = ParseScript(param[0].lpolestr, param[1].lpolestr, pv, NULL, param[TE_EXCEPINFO].pExcepInfo);
	teSetLong(pVarResult, param[TE_API_RESULT].lVal);
}

VOID teApiGetScriptDispatch(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	if (pVarResult) {
		VARIANT *pv = NULL;
		if (nArg >= 2) {
			pv = &pDispParams->rgvarg[nArg - 2];
		}
		IDispatch *pdisp = NULL;
		param[TE_API_RESULT].lVal = ParseScript(param[0].lpolestr, param[1].lpolestr, pv, &pdisp, param[TE_EXCEPINFO].pExcepInfo);
		teSetObjectRelease(pVarResult, pdisp);
	}
}

VOID teApiGetDispatch(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	if (pVarResult) {
		IDispatch *pdisp;
		if (GetDispatch(&pDispParams->rgvarg[nArg], &pdisp)) {
			DISPID dispid = DISPID_UNKNOWN;
			LPOLESTR lp = param[1].lpolestr;
			if (pdisp->GetIDsOfNames(IID_NULL, &lp, 1, LOCALE_USER_DEFAULT, &dispid) == S_OK) {
				CteDispatch *oDisp = new CteDispatch(pdisp, 0);
				oDisp->m_dispIdMember = dispid;
				teSetObjectRelease(pVarResult, oDisp);
			}
			pdisp->Release();
		}
	}
}

VOID teApiSHChangeNotifyRegister(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	SHChangeNotifyEntry entry;
	teGetIDListFromVariant(const_cast<LPITEMIDLIST *>(&entry.pidl), &pDispParams->rgvarg[nArg - 4]);
	entry.fRecursive = param[5].boolVal;
	if (entry.pidl) {
		teSetLong(pVarResult, SHChangeNotifyRegister(param[0].hwnd, param[1].intVal, param[2].lVal, param[3].uintVal, 1, &entry));
		teChangeWindowMessageFilterEx(param[0].hwnd, param[3].uintVal, MSGFLT_ALLOW, NULL);
		teCoTaskMemFree(const_cast<LPITEMIDLIST>(entry.pidl));
	}
}

VOID teApiMessageBox(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, MessageBox(param[0].hwnd, param[1].lpcwstr, param[2].lpcwstr, param[3].uintVal));
}

VOID teApiImageList_GetIcon(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, ImageList_GetIcon(param[0].himagelist, param[1].intVal, param[2].uintVal));
}

VOID teApiImageList_Create(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, ImageList_Create(param[0].intVal, param[1].intVal, param[2].uintVal, param[3].intVal, param[4].intVal));
}

VOID teApiGetWindowLongPtr(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, GetWindowLongPtr(param[0].hwnd, param[1].intVal));
}

VOID teApiGetClassLongPtr(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, GetClassLongPtr(param[0].hwnd, param[1].intVal));
}

VOID teApiGetSubMenu(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, GetSubMenu(param[0].hmenu, param[1].intVal));
}

VOID teApiSelectObject(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, SelectObject(param[0].hdc, param[1].hgdiobj));
}

VOID teApiGetStockObject(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, GetStockObject(param[0].intVal));
}

VOID teApiGetSysColorBrush(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, GetSysColorBrush(param[0].intVal));
}

VOID teApiSetFocus(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, SetFocus(param[0].hwnd));
	if (param[0].hwnd == g_hwndMain) {
		CteShellBrowser *pSB = g_pTC->GetShellBrowser(g_pTC->m_nIndex);
		if (pSB) {
			pSB->SetActive(FALSE);
		}
	}
}

VOID teApiGetDC(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, GetDC(param[0].hwnd));
}

VOID teApiCreateCompatibleDC(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, CreateCompatibleDC(param[0].hdc));
}

VOID teCreateMenu(HMENU hMenu, VARIANT *pVarResult)
{
	MENUINFO menuInfo;
	menuInfo.cbSize = sizeof(MENUINFO);
	menuInfo.fMask = MIM_STYLE;
	GetMenuInfo(hMenu, &menuInfo);
	menuInfo.dwStyle = (menuInfo.dwStyle & ~MNS_NOCHECK) | MNS_CHECKORBMP;
	SetMenuInfo(hMenu, &menuInfo);
	teSetPtr(pVarResult, hMenu);
}

VOID teApiCreatePopupMenu(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teCreateMenu(CreatePopupMenu(), pVarResult);
}

VOID teApiCreateMenu(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teCreateMenu(CreateMenu(), pVarResult);
}

VOID teApiCreateCompatibleBitmap(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, CreateCompatibleBitmap(param[0].hdc, param[1].intVal, param[2].intVal));
}

VOID teApiSetWindowLongPtr(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, SetWindowLongPtr(param[0].hwnd, param[1].intVal, param[2].long_ptr));
}

VOID teApiSetClassLongPtr(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, SetClassLongPtr(param[0].hwnd, param[1].intVal, param[2].long_ptr));
}

VOID teApiImageList_Duplicate(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, ImageList_Duplicate(param[0].himagelist));
}

VOID teApiSendMessage(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, SendMessage(param[0].hwnd, param[1].uintVal, param[2].wparam, param[3].lparam));
}

VOID teApiGetSystemMenu(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, GetSystemMenu(param[0].hwnd, param[1].boolVal));
}

VOID teApiGetWindowDC(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, GetWindowDC(param[0].hwnd));
}

VOID teApiCreatePen(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, CreatePen(param[0].intVal, param[1].intVal, param[2].colorref));
}

VOID teApiSetCapture(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, SetCapture(param[0].hwnd));
}

VOID teApiSetCursor(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, SetCursor(param[0].hcursor));
}

VOID teApiCallWindowProc(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, CallWindowProc(param[0].wndproc, param[1].hwnd, param[2].uintVal, param[3].wparam, param[4].lparam));
}

VOID teApiGetWindow(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	if (nArg >= 1) {
		teSetPtr(pVarResult, GetWindow(param[0].hwnd, param[1].uintVal));
		return;
	}
	IUnknown *punk;
	if (FindUnknown(&pDispParams->rgvarg[nArg], &punk)) {
		HWND hwnd;
		IUnknown_GetWindow(punk, &hwnd);
		teSetPtr(pVarResult, hwnd);
	}
}

VOID teApiGetTopWindow(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, GetTopWindow(param[0].hwnd));
}

VOID teApiOpenProcess(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, OpenProcess(param[0].dword, param[1].boolVal, param[2].dword));
}

VOID teApiGetParent(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, GetParent(param[0].hwnd));
}

VOID teApiGetCapture(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, GetCapture());
}

VOID teApiGetModuleHandle(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, GetModuleHandle(param[0].lpcwstr));
}

VOID teApiSHGetImageList(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	HANDLE h = 0;
	if FAILED(SHGetImageList(param[0].intVal, IID_IImageList, (LPVOID *)&h)) {
		h = 0;
	}
	teSetPtr(pVarResult, h);
}

VOID teApiCopyImage(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, CopyImage(param[0].handle, param[1].uintVal, param[2].intVal, param[3].intVal, param[4].uintVal));
}

VOID teApiGetCurrentProcess(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, GetCurrentProcess());
}

VOID teApiImmGetContext(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, ImmGetContext(param[0].hwnd));
}

VOID teApiGetFocus(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, GetFocus());
}

VOID teApiGetForegroundWindow(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, GetForegroundWindow());
}

VOID teApiSetTimer(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, SetTimer(param[0].hwnd, param[1].uint_ptr, param[2].uintVal, NULL));
}

VOID teApiLoadMenu(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, LoadMenu(param[0].hinstance, GetLPWSTRFromVariant(&pDispParams->rgvarg[nArg - 1])));
}

VOID teApiLoadIcon(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, LoadIcon(param[0].hinstance, GetLPWSTRFromVariant(&pDispParams->rgvarg[nArg - 1])));
}

VOID teApiLoadLibraryEx(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, LoadLibraryEx(GetLPWSTRFromVariant(&pDispParams->rgvarg[nArg]), param[1].handle, param[2].dword));
}

VOID teApiLoadImage(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, LoadImage(param[0].hinstance, GetLPWSTRFromVariant(&pDispParams->rgvarg[nArg - 1]),
		param[2].uintVal,	param[3].intVal, param[4].intVal, param[5].uintVal));
}

VOID teApiImageList_LoadImage(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, ImageList_LoadImage(param[0].hinstance, GetLPWSTRFromVariant(&pDispParams->rgvarg[nArg - 1]),
	param[2].intVal, param[3].intVal, param[4].colorref, param[5].uintVal, param[6].uintVal));
}

VOID teApiSHGetFileInfo(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teParam	Path;
	IUnknown *punk;
	BOOL bItemIDList = FindUnknown(&pDispParams->rgvarg[nArg], &punk);
	if (bItemIDList) {
		bItemIDList = teGetIDListFromVariant(&Path.lpitemidlist, &pDispParams->rgvarg[nArg]);
	} else {
		Path.lpcwstr = param[0].lpcwstr;
	}
	teSetPtr(pVarResult, SHGetFileInfo(Path.lpcwstr, param[1].dword, param[2].lpshfileinfo, param[3].uintVal, param[4].uintVal));
	if (bItemIDList) {
		teCoTaskMemFree(Path.lpitemidlist);
	}
}

VOID teApiCreateWindowEx(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, CreateWindowEx(param[0].dword, param[1].lpcwstr, param[2].lpcwstr,
		param[3].dword, param[4].intVal, param[5].intVal, param[6].intVal, param[7].intVal,
		param[8].hwnd, param[9].hmenu, param[10].hinstance, param[11].lpvoid));
}

VOID teApiShellExecute(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, ShellExecute(param[0].hwnd, GetLPWSTRFromVariant(&pDispParams->rgvarg[nArg - 1]),
		GetLPWSTRFromVariant(&pDispParams->rgvarg[nArg - 2]), param[3].lpcwstr, param[4].lpcwstr, param[5].intVal));
}

VOID teApiBeginPaint(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, BeginPaint(param[0].hwnd, param[1].lppaintstruct));
}

VOID teApiLoadCursor(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, LoadCursor(param[0].hinstance, GetLPWSTRFromVariant(&pDispParams->rgvarg[nArg - 1])));
}

VOID teApiLoadCursorFromFile(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, LoadCursorFromFile(GetLPWSTRFromVariant(&pDispParams->rgvarg[nArg - 1])));
}

VOID teAsyncInvoke(WORD wMode, int nArg, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	TEInvoke *pInvoke = new TEInvoke[1];
	if (GetDispatch(&pDispParams->rgvarg[nArg], &pInvoke->pdisp)) {
		int dwms = GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]);
		if (dwms == 0) {
			dwms = g_param[TE_NetworkTimeout];
		}
		pInvoke->dispid = DISPID_VALUE;
		pInvoke->cRef = 2;
		pInvoke->cArgs = nArg - 1;
		pInvoke->pv = GetNewVARIANT(nArg);
		pInvoke->wErrorHandling = 3;
		pInvoke->wMode = wMode;
		pInvoke->pidl = NULL;
		pInvoke->hr = E_ABORT;
		for (int i = nArg - 1; i--;) {
			VariantCopy(&pInvoke->pv[i], &pDispParams->rgvarg[i]);
		}
		if ((pInvoke->pv[pInvoke->cArgs - 1].vt == VT_BSTR) || (wMode && teGetIDListFromVariant(&pInvoke->pidl, &pInvoke->pv[pInvoke->cArgs - 1]))) {
			if (dwms > 0) {
				SetTimer(g_hwndMain, (UINT_PTR)pInvoke, dwms, teTimerProcParse);
			}
			teSetPtr(pVarResult, _beginthread(&threadParseDisplayName, 0, pInvoke));
			return;
		}
		FolderItem *pid;
		GetFolderItemFromVariant(&pid, &pInvoke->pv[pInvoke->cArgs - 1]);
		VariantClear(&pInvoke->pv[pInvoke->cArgs - 1]);
		teSetObjectRelease(&pInvoke->pv[pInvoke->cArgs - 1], pid);
		Invoke5(pInvoke->pdisp, DISPID_VALUE, DISPATCH_METHOD, NULL, pInvoke->cArgs, pInvoke->pv);
		pInvoke->pdisp->Release();
		teSetPtr(pVarResult, 0);
	}
	delete [] pInvoke;
}

VOID teApiSHParseDisplayName(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teAsyncInvoke(0, nArg, pDispParams, pVarResult);
}

VOID teApiSHChangeNotification_Lock(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IDispatch *pdisp;
	if (GetDispatch(&pDispParams->rgvarg[nArg - 2], &pdisp)) {
		PIDLIST_ABSOLUTE *ppidl;
		LONG lEvent;
		HANDLE hLock = SHChangeNotification_Lock(param[0].handle, param[1].dword, &ppidl, &lEvent);
		if (hLock) {
			VARIANT v;
			VariantInit(&v);
			if (ppidl) {
				for (int i = (lEvent & (SHCNE_RENAMEFOLDER | SHCNE_RENAMEITEM)) ? 2 : 1; i--;) {
					CteFolderItem *pPF = new CteFolderItem(NULL);
					pPF->Initialize(ppidl[i]);
					teSetObjectRelease(&v, pPF);
					tePutPropertyAt(pdisp, i, &v);
					VariantClear(&v);
				}
			}
			SHChangeNotification_Unlock(hLock);
			teSetPtr(pVarResult, hLock);
			teSetLong(&v, lEvent);
			tePutProperty(pdisp, L"lEvent", &v);
		}
		pdisp->Release();
	}
}

VOID teApiGetWindowText(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	if (param[0].hwnd == g_hwndMain) {
		teSetSZ(pVarResult, g_szTitle);
		return;
	}
	BSTR bs = NULL;
	int nLen = GetWindowTextLength(param[0].hwnd);
	if (nLen) {
		bs = ::SysAllocStringLen(NULL, nLen);
		nLen = GetWindowText(param[0].hwnd, bs, nLen + 1);
	}
	teSetBSTR(pVarResult, &bs, nLen);
}

VOID teApiGetClassName(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	BSTR bsResult = ::SysAllocStringLen(NULL, MAX_CLASS_NAME);
	int nLen = GetClassName(param[0].hwnd, bsResult, MAX_CLASS_NAME);
	teSetBSTR(pVarResult, &bsResult, nLen);
}

VOID teApiGetModuleFileName(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	BSTR bsResult = NULL;
	int nLen = teGetModuleFileName(param[0].hmodule, &bsResult);
	teSetBSTR(pVarResult, &bsResult, nLen);
}

VOID teApiGetCommandLine(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	if (pVarResult) {
		pVarResult->bstrVal = teGetCommandLine();
		pVarResult->vt = VT_BSTR;
	}
}

VOID teApiGetCurrentDirectory(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	BSTR bsResult = ::SysAllocStringLen(NULL, MAX_PATH);
	int nLen = GetCurrentDirectory(MAX_PATH, bsResult);
	teSetBSTR(pVarResult, &bsResult, nLen);
}

VOID teApiGetMenuString(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	BSTR bsResult = NULL;
	int nLen = teGetMenuString(&bsResult, param[0].hmenu, param[1].uintVal, param[2].lVal != MF_BYCOMMAND);
	teSetBSTR(pVarResult, &bsResult, nLen);
}

VOID teApiGetDisplayNameOf(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teGetDisplayNameOf(&pDispParams->rgvarg[nArg], param[1].intVal, pVarResult);
}

VOID teApiGetKeyNameText(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	BSTR bsResult = ::SysAllocStringLen(NULL, MAX_PATH);
	int nLen = GetKeyNameText(param[0].lVal, bsResult, MAX_PATH);
	teSetBSTR(pVarResult, &bsResult, nLen);
}

VOID teApiDragQueryFile(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	if (param[1].lVal == -1) {
		teSetLong(pVarResult, DragQueryFile(param[0].hdrop, param[1].uintVal, NULL, 0));
		return;
	}
	BSTR bsResult = NULL;
	int nLen = teDragQueryFile(param[0].hdrop, param[1].uintVal, &bsResult);
	teSetBSTR(pVarResult, &bsResult, nLen);
}

VOID teApiSysAllocString(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetSZ(pVarResult, GetLPWSTRFromVariant(&pDispParams->rgvarg[nArg]));
}

VOID teApiSysAllocStringLen(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	if (pVarResult) {
		VARIANT *pv = &pDispParams->rgvarg[nArg];
		pVarResult->bstrVal = (pv->vt == VT_BSTR) ? teSysAllocStringLenEx(pv->bstrVal, param[1].uintVal) : teSysAllocStringLen(GetLPWSTRFromVariant(pv), param[1].uintVal);
		pVarResult->vt = VT_BSTR;
	}
}

VOID teApiSysAllocStringByteLen(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	if (pVarResult) {
		UCHAR *pc;
		int nLen = param[2].intVal;
		GetpDataFromVariant(&pc, &nLen, &pDispParams->rgvarg[nArg]);
		pVarResult->bstrVal = teSysAllocStringByteLen((LPCSTR)pc, param[1].uintVal, nLen);
		pVarResult->vt = VT_BSTR;
	}
}

VOID teApisprintf(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	int nSize = param[0].intVal;
	LPWSTR pszFormat = param[1].lpwstr;
	if (pszFormat && nSize > 0) {
		BSTR bsResult = ::SysAllocStringLen(NULL, nSize);
		int nLen = 0;
		int nIndex = 1;
		int nPos = 0;
		while (pszFormat[nPos] && nLen < nSize) {
			while (pszFormat[nPos] && pszFormat[nPos++] != '%') {
			}
			while (WCHAR wc = pszFormat[nPos]) {
				nPos++;
				if (wc == '%') {//Escape %
					lstrcpyn(&bsResult[nLen], pszFormat, nPos);
					nLen += nPos - 1;
					pszFormat += nPos;
					nPos = 0;
					break;
				}
				if (nIndex <= nArg) {
					if (StrChrIW(L"diuoxc", wc)) {//Integer
						wc = pszFormat[nPos];
						pszFormat[nPos] = NULL;
						swprintf_s(&bsResult[nLen], nSize - nLen, pszFormat, GetLLFromVariant(&pDispParams->rgvarg[nArg - ++nIndex]));
						nLen += lstrlen(&bsResult[nLen]);
						pszFormat += nPos;
						nPos = 0;
						pszFormat[0] = wc;
						break;
					}
					if (StrChrIW(L"efga", wc)) {//Real
						wc = pszFormat[nPos];
						pszFormat[nPos] = NULL;
						VARIANT v;
						teVariantChangeType(&v, &pDispParams->rgvarg[nArg - ++nIndex], VT_R8);
						swprintf_s(&bsResult[nLen], nSize - nLen, pszFormat, v.dblVal);
						nLen += lstrlen(&bsResult[nLen]);
						pszFormat += nPos;
						nPos = 0;
						pszFormat[0] = wc;
						break;
					}
					if (towlower(wc) == 's') {//String
						wc = pszFormat[nPos];
						pszFormat[nPos] = NULL;
						VARIANT v;
						teVariantChangeType(&v, &pDispParams->rgvarg[nArg - ++nIndex], VT_BSTR);
						swprintf_s(&bsResult[nLen], nSize - nLen, pszFormat, v.bstrVal);
						VariantClear(&v);
						nLen += lstrlen(&bsResult[nLen]);
						pszFormat += nPos;
						nPos = 0;
						pszFormat[0] = wc;
						break;
					}
					if (!StrChrIW(L"0123456789-+#.hljzt ", wc)) {//not Specifier
						lstrcpyn(&bsResult[nLen], pszFormat, nPos + 1);
						nLen += nPos;
						pszFormat += nPos;
						nPos = 0;
						break;
					}
				}
			}
		}
		if (nLen < nSize && pszFormat[0]) {
			lstrcpyn(&bsResult[nLen], pszFormat, nSize - nLen);
			nLen += lstrlen(&bsResult[nLen]);
		}
		teSetBSTR(pVarResult, &bsResult, nLen);
	}
}

VOID teApibase64_encode(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	UCHAR *pc;
	int nLen = 0;
	GetpDataFromVariant(&pc, &nLen, &pDispParams->rgvarg[nArg]);
	if (nLen) {
		DWORD dwSize;
		CryptBinaryToString(pc, nLen, CRYPT_STRING_BASE64, NULL, &dwSize);
		BSTR bsResult = NULL;
		if (dwSize > 0) {
			bsResult = ::SysAllocStringLen(NULL, dwSize - 1);
			CryptBinaryToString(pc, nLen, CRYPT_STRING_BASE64, bsResult, &dwSize);
		}
		teSetBSTR(pVarResult, &bsResult, -1);
	}
}

VOID teApiLoadString(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	BSTR bsResult = ::SysAllocStringLen(NULL, SIZE_BUFF);
	int nLen = LoadString(param[0].hinstance, param[1].uintVal, bsResult, SIZE_BUFF);
	teSetBSTR(pVarResult, &bsResult, -1);
}

VOID teApiAssocQueryString(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IQueryAssociations *pQA = NULL;
	DWORD cch = 0;
	BSTR bsResult = NULL;
	int nLen = -1;
	IDataObject *pDataObj;
	if (GetDataObjFromVariant(&pDataObj, &pDispParams->rgvarg[nArg - 2])) {
		long nCount;
		LPITEMIDLIST *ppidllist = IDListFormDataObj(pDataObj, &nCount);
		AdjustIDList(ppidllist, nCount);
		if (nCount >= 1) {
			IShellFolder *pSF;
			if (GetShellFolder(&pSF, ppidllist[0])) {
				if SUCCEEDED(pSF->GetUIObjectOf(g_hwndMain, nCount, const_cast<LPCITEMIDLIST *>(&ppidllist[1]), IID_IQueryAssociations, NULL, (LPVOID*)&pQA)) {
					if SUCCEEDED(pQA->GetString(param[0].assocf, param[1].assocstr, param[3].lpcwstr, NULL, &cch)) {
						if (cch) {
							nLen = cch - 1;
							bsResult = ::SysAllocStringLen(NULL, nLen);
							pQA->GetString(param[0].assocf, param[1].assocstr, param[3].lpcwstr, bsResult, &cch);
						}
						SafeRelease(&pQA);
					}
				}
				pSF->Release();
			}
			do {
				teCoTaskMemFree(ppidllist[nCount]);
			} while (--nCount >= 0);
		}
		delete [] ppidllist;
		pDataObj->Release();
	}
	teSetBSTR(pVarResult, &bsResult, nLen);
}

VOID teApiStrFormatByteSize(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	BSTR bsResult = ::SysAllocStringLen(NULL, MAX_COLUMN_NAME_LEN);
	teStrFormatSize(nArg >= 1 ? param[1].dword : 2, param[0].llVal, bsResult, MAX_COLUMN_NAME_LEN);
	teSetBSTR(pVarResult, &bsResult, -1);
}

VOID teApiGetDateFormat(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	SYSTEMTIME SysTime;
	if (teGetSystemTime(&pDispParams->rgvarg[nArg - 2], &SysTime)) {
		int cch = GetDateFormat(param[0].lcid, param[1].dword, &SysTime, param[3].lpcwstr, NULL, 0);
		if (cch > 0) {
			BSTR bsResult = ::SysAllocStringLen(NULL, cch - 1);
			GetDateFormat(param[0].lcid, param[1].dword, &SysTime, param[3].lpcwstr, bsResult, cch);
			teSetBSTR(pVarResult, &bsResult, cch - 1);
		}
	}
}

VOID teApiGetTimeFormat(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	SYSTEMTIME SysTime;
	if (teGetSystemTime(&pDispParams->rgvarg[nArg - 2], &SysTime)) {
		int cch = GetTimeFormat(param[0].lcid, param[1].dword, &SysTime, param[3].lpcwstr, NULL, 0);
		if (cch > 0) {
			BSTR bsResult = ::SysAllocStringLen(NULL, cch - 1);
			GetTimeFormat(param[0].lcid, param[1].dword, &SysTime, param[3].lpcwstr, bsResult, cch);
			teSetBSTR(pVarResult, &bsResult, cch - 1);
		}
	}
}

VOID teApiGetLocaleInfo(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	int cch = GetLocaleInfo(param[0].lcid, param[1].lctype, NULL, 0);
	if (cch > 0) {
		BSTR bsResult = ::SysAllocStringLen(NULL, cch - 1);
		GetLocaleInfo(param[0].lcid, param[1].lctype, bsResult, cch);
		teSetBSTR(pVarResult, &bsResult, -1);
	}
}

VOID teApiMonitorFromPoint(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	POINT pt;
	GetPointFormVariant(&pt, &pDispParams->rgvarg[nArg]);
	teSetPtr(pVarResult, MonitorFromPoint(pt, param[1].dword));
}

VOID teApiMonitorFromRect(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, MonitorFromRect(param[0].lpcrect, param[1].dword));
}

VOID teApiGetMonitorInfo(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, GetMonitorInfo(param[0].hmonitor, param[1].lpmonitorinfo));
}

VOID teApiGlobalAddAtom(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, GlobalAddAtom(param[0].lpcwstr));
}

VOID teApiGlobalGetAtomName(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	TCHAR szBuf[256];
	UINT uSize = GlobalGetAtomName(param[0].atom, szBuf, sizeof(szBuf) / sizeof(TCHAR));
	teSetSZ(pVarResult, szBuf);
}

VOID teApiGlobalFindAtom(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, GlobalFindAtom(param[0].lpcwstr));
}

VOID teApiGlobalDeleteAtom(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, GlobalDeleteAtom(param[0].atom));
}

VOID teApiRegisterHotKey(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, RegisterHotKey(param[0].hwnd, param[1].intVal, param[2].uintVal, param[3].uintVal));
}

VOID teApiUnregisterHotKey(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, UnregisterHotKey(param[0].hwnd, param[1].intVal));
}

VOID teApiILGetCount(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	LPITEMIDLIST pidl;
	if (teGetIDListFromVariant(&pidl, &pDispParams->rgvarg[nArg])) {
		teSetLong(pVarResult, ILGetCount(pidl));
		teCoTaskMemFree(pidl);
	}
}

VOID teApiSHTestTokenMembership(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, SHTestTokenMembership(param[0].handle, param[1].ulVal));
}

VOID teApiObjGetI(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IDispatch *pdisp;
	if (GetDispatch(&pDispParams->rgvarg[nArg], &pdisp)) {
		teGetPropertyI(pdisp, param[1].lpolestr, pVarResult);
		pdisp->Release();
	}
}

VOID teApiObjPutI(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IUnknown *punk;
	if (FindUnknown(&pDispParams->rgvarg[nArg], &punk)) {
		teSetLong(pVarResult, tePutProperty0(punk, param[1].lpolestr, &pDispParams->rgvarg[nArg - 2], fdexNameEnsure | fdexNameCaseInsensitive));
	}
}

VOID teApiObjDispId(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	DISPID dispid = DISPID_UNKNOWN;
	IDispatch *pdisp;
	if (GetDispatch(&pDispParams->rgvarg[nArg], &pdisp)) {
		LPOLESTR lpsz = param[1].lpolestr;
		HRESULT hr = pdisp->GetIDsOfNames(IID_NULL, &lpsz, 1, LOCALE_USER_DEFAULT, &dispid);
		pdisp->Release();
	}
	teSetLong(pVarResult, dispid);
}

VOID teApiDrawText(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, DrawText(param[0].hdc, param[1].lpcwstr, param[2].intVal, param[3].lprect, param[4].uintVal));
}

VOID teApiRectangle(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, Rectangle(param[0].hdc, param[1].intVal, param[2].intVal, param[3].intVal, param[4].intVal));
}

VOID teApiPathIsDirectory(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	if (nArg >= 2 && pDispParams->rgvarg[nArg].vt == VT_DISPATCH) {
		teAsyncInvoke(1, nArg, pDispParams, pVarResult);
		return;
	}
	HRESULT hr = tePathIsDirectory(param[0].lpwstr, param[1].dword, param[2].intVal);
	if (hr != E_ABORT) {
		if (hr <= 0) {
			teSetBool(pVarResult, SUCCEEDED(hr));
		} else {
			teSetLong(pVarResult, hr);
		}
	}
}

VOID teApiPathIsSameRoot(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, PathIsSameRoot(param[0].lpcwstr, param[1].lpcwstr));
}

VOID teApiSHSimpleIDListFromPath(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	FILETIME ft = { 0 };
	if (nArg >= 2) {
		DATE dt;
		if (teGetVariantTime(&pDispParams->rgvarg[nArg - 2], &dt) && dt >= 0) {
			teVariantTimeToFileTime(dt, &ft);
		}
	}
	LPITEMIDLIST pidl = teSHSimpleIDListFromPathEx(param[0].lpwstr, nArg >= 1 && (param[1].dword & FILE_ATTRIBUTE_DIRECTORY), param[1].word, param[3].uintVal, param[3].uli.HighPart, ft);
	teSetIDListRelease(pVarResult, &pidl);
}

VOID teApiOutputDebugString(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	::OutputDebugString(param[0].lpcwstr);
}

VOID teApiDllGetClassObject(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IDispatch *pdisp;
	HMODULE hDll = teCreateInstanceV(&pDispParams->rgvarg[nArg], &pDispParams->rgvarg[nArg - 1], IID_PPV_ARGS(&pdisp));
	if (hDll) {
		CteDispatch *odisp = new CteDispatch(pdisp, 4);
		odisp->m_pDll = new CteDll(hDll);
		hDll = 0;
		VariantClear(pVarResult);
		teSetObjectRelease(pVarResult, odisp);
	}
}

VOID teApiIsAppThemed(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, IsAppThemed());
}

VOID teApiSHDefExtractIcon(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, SHDefExtractIcon(param[0].lpcwstr, param[1].intVal, param[2].uintVal, param[3].phicon, param[4].phicon, param[5].uintVal));
}

VOID teApiURLDownloadToFile(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IDispatch *pdisp = NULL;
	IUnknown *punk = NULL;
	if (GetDispatch(&pDispParams->rgvarg[nArg - 1], &pdisp)) {
		IStream *pDst, *pSrc;
		HRESULT hr = pdisp->QueryInterface(IID_PPV_ARGS(&pSrc));
		if SUCCEEDED(hr) {
			hr = SHCreateStreamOnFileEx(param[2].bstrVal, STGM_WRITE | STGM_CREATE | STGM_SHARE_DENY_WRITE, FILE_ATTRIBUTE_NORMAL, TRUE, NULL, &pDst);
			if SUCCEEDED(hr) {
				teCopyStream(pSrc, pDst);
				pDst->Release();
			}
			pSrc->Release();
		}
		if FAILED(hr) {
			VARIANT v;
			if SUCCEEDED(teGetProperty(pdisp, L"responseBody", &v)) {
				UCHAR *pc;
				int nLen = 0;
				GetpDataFromVariant(&pc, &nLen, &v);
				if (nLen) {
					HANDLE hFile = CreateFile(param[2].bstrVal, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					if (hFile != INVALID_HANDLE_VALUE) {
						DWORD dwWriteByte;
						if (WriteFile(hFile, pc, nLen, &dwWriteByte, NULL)) {
							hr = S_OK;
						}
						CloseHandle(hFile);
					}
				}
				VariantClear(&v);
			}
		}
		pdisp->Release();
		teSetLong(pVarResult, hr);
		return;
	}
	FindUnknown(&pDispParams->rgvarg[nArg], &punk);
	teSetLong(pVarResult, URLDownloadToFile(punk, param[1].bstrVal, param[2].bstrVal, param[3].dword, NULL));
}

VOID teApiMoveFileEx(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, MoveFileEx(param[0].lpcwstr, param[1].lpcwstr, param[2].dword));
}

VOID teApiObject(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IDispatch *pdisp = NULL;
	GetNewObject(&pdisp);
	teSetObjectRelease(pVarResult, pdisp);
}

VOID teApiArray(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IDispatch *pdisp = NULL;;
	GetNewArray(&pdisp);
	teSetObjectRelease(pVarResult, pdisp);
}

VOID teApiPathIsRoot(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, PathIsRoot(param[0].lpcwstr));
}

VOID teApiEnableWindow(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, EnableWindow(param[0].hwnd, param[1].boolVal));
}

VOID teApiIsWindowEnabled(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, IsWindowEnabled(param[0].hwnd));
}

VOID teApiCryptProtectData(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	DATA_BLOB blob[3];
	for (int i = 2; i--;) {
		blob[i].cbData = ::SysStringByteLen(param[i].bstrVal);
		blob[i].pbData = param[i].pbVal;
	}
	if (CryptProtectData(&blob[0], NULL, &blob[1], NULL, NULL, 0, &blob[2])) {
		teCreateSafeArray(pVarResult, blob[2].pbData, blob[2].cbData, param[2].boolVal);
		LocalFree(blob[2].pbData);
	}
}

VOID teApiCryptUnprotectData(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	UCHAR *pc;
	int nLen = 0;
	GetpDataFromVariant(&pc, &nLen, &pDispParams->rgvarg[nArg]);
	if (nLen && pVarResult) {
		DATA_BLOB blob[3];
		blob[0].cbData = nLen;
		blob[0].pbData = pc;
		blob[1].cbData = ::SysStringByteLen(param[1].bstrVal);
		blob[1].pbData = param[1].pbVal;
		if (CryptUnprotectData(&blob[0], NULL, &blob[1], NULL, NULL, 0, &blob[2])) {
			teCreateSafeArray(pVarResult, blob[2].pbData, blob[2].cbData, param[2].boolVal);
			LocalFree(blob[2].pbData);
		}
	}
}

VOID teApibase64_decode(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	DWORD dwData;
	if (pVarResult) {
		if (CryptStringToBinary(param[0].bstrVal, ::SysStringLen(param[0].bstrVal), CRYPT_STRING_BASE64, NULL, &dwData, NULL, NULL) && dwData > 0) {
			if (param[1].boolVal) {
				pVarResult->vt = VT_BSTR;
				pVarResult->bstrVal = ::SysAllocStringByteLen(NULL, dwData);
				CryptStringToBinary(param[0].bstrVal, ::SysStringLen(param[0].bstrVal), CRYPT_STRING_BASE64, (BYTE *)pVarResult->bstrVal, &dwData, NULL, NULL);
			} else {
				SAFEARRAY *psa = SafeArrayCreateVector(VT_UI1, 0, dwData);
				if (psa) {
					PVOID pvData;
					if (::SafeArrayAccessData(psa, &pvData) == S_OK) {
						pVarResult->vt = VT_ARRAY | VT_UI1;
						pVarResult->parray = psa;
						CryptStringToBinary(param[0].bstrVal, ::SysStringLen(param[0].bstrVal), CRYPT_STRING_BASE64, (BYTE *)pvData, &dwData, NULL, NULL);
						::SafeArrayUnaccessData(psa);
					}
				}
			}
		}
	}
}

VOID teApiAlphaBlend(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	SetStretchBltMode(param[0].hdc, HALFTONE);
	teSetBool(pVarResult, AlphaBlend(param[0].hdc, param[1].intVal, param[2].intVal, param[3].intVal, param[4].intVal,
		param[5].hdc, param[6].intVal, param[7].intVal, param[8].intVal, param[9].intVal, *(BLENDFUNCTION*)&param[10].lVal));
}

VOID teApiStretchBlt(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	SetStretchBltMode(param[0].hdc, HALFTONE);
	teSetBool(pVarResult, StretchBlt(param[0].hdc, param[1].intVal, param[2].intVal, param[3].intVal, param[4].intVal,
		param[5].hdc, param[6].intVal, param[7].intVal, param[8].intVal, param[9].intVal, param[10].dword));
}

VOID teApiTransparentBlt(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	SetStretchBltMode(param[0].hdc, HALFTONE);
	teSetBool(pVarResult, TransparentBlt(param[0].hdc, param[1].intVal, param[2].intVal, param[3].intVal, param[4].intVal,
		param[5].hdc, param[6].intVal, param[7].intVal, param[8].intVal, param[9].intVal, param[10].uintVal));
}

VOID teApiFormatMessage(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	LPWSTR lpBuf;
	if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY,
		param[0].lpwstr, 0, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&lpBuf, MAX_STATUS, (va_list *)&param[1])) {
		teSetSZ(pVarResult, lpBuf);
		LocalFree(lpBuf);
	}
}

VOID teApiGetGUIThreadInfo(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, GetGUIThreadInfo(param[0].dword, param[1].pgui));
}

VOID teApiCreateSolidBrush(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, CreateSolidBrush(param[0].colorref));
}

VOID teApiPlgBlt(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	SetStretchBltMode(param[0].hdc, HALFTONE);
	teSetBool(pVarResult, PlgBlt(param[0].hdc, param[1].lppoint, param[2].hdc, param[3].intVal, param[4].intVal,
		param[5].intVal, param[6].intVal, param[7].hbitmap, param[8].intVal, param[9].intVal));
}

VOID teApiRoundRect(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, RoundRect(param[0].hdc, param[1].intVal, param[2].intVal, param[3].intVal, param[4].intVal, param[5].intVal, param[6].intVal)); 
}

VOID teApiCreateProcess(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	HANDLE hRead, hWrite;
 	SECURITY_ATTRIBUTES	sa;
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi = { 0 };
	DWORD dwLen;

	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor	= 0;
	sa.bInheritHandle = TRUE;

	if(!CreatePipe(&hRead, &hWrite, &sa, 0)) {
		return;
	}

	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdOutput = hWrite;
	si.hStdError = hWrite;
	DWORD dwms =  param[3].dword;
	if (dwms == 0) {
		dwms = INFINITE;
	}
	if (CreateProcess(NULL, param[0].lpwstr, NULL, NULL, TRUE, nArg >= 4 ? param[4].dword : CREATE_NO_WINDOW, NULL, param[1].lpwstr, &si, &pi)) {
		if (WaitForInputIdle(pi.hProcess, dwms)) {
			if (WaitForSingleObject(pi.hProcess, dwms) != WAIT_TIMEOUT) {
				if (PeekNamedPipe(hRead, NULL, 0, NULL, &dwLen, NULL)) {
					BSTR bs = ::SysAllocStringByteLen(NULL, dwLen);
					if (dwLen) {
						ReadFile(hRead, bs, dwLen, &dwLen, NULL);
						if (param[2].intVal == 2 || (param[2].intVal != 1 && IsTextUnicode(bs, dwLen, NULL))) {
							teSetBSTR(pVarResult, &bs, dwLen);
						} else {
							BSTR bs2 = teMultiByteToWideChar(CP_ACP, (LPCSTR)bs, dwLen);
							teSetBSTR(pVarResult, &bs2, -1);
							teSysFreeString(&bs);
						}
					}
				}
			}
		}
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	CloseHandle(hRead);
	CloseHandle(hWrite);
}

VOID teApiDeleteFile(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, DeleteFile(param[0].lpcwstr));
}

VOID teApiCreateObject(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	LPWSTR lpwstr = param[0].lpwstr;
	if (lstrcmpi(lpwstr, L"Array") == 0) {
		IDispatch *pdisp = NULL;
		GetNewArray(&pdisp);
		teSetObjectRelease(pVarResult, pdisp);
		return;
	}
	if (lstrcmpi(lpwstr, L"CommonDialog") == 0) {
		teSetObjectRelease(pVarResult, new CteCommonDialog());
		return;
	}
	if (lstrcmpi(lpwstr, L"FolderItems") == 0) {
		IDataObject *pDataObj = NULL;
		if (nArg >= 1) {
			GetDataObjFromVariant(&pDataObj, &pDispParams->rgvarg[nArg - 1]);
		}
		teSetObjectRelease(pVarResult, new CteFolderItems(pDataObj, NULL));
		SafeRelease(&pDataObj);
		return;
	}
	if (lstrcmpi(lpwstr, L"Object") == 0) {
		IDispatch *pdisp = NULL;
		GetNewObject(&pdisp);
		teSetObjectRelease(pVarResult, pdisp);
		return;
	}
	if (lstrcmpi(lpwstr, L"ProgressDialog") == 0) {
		teSetObjectRelease(pVarResult, new CteProgressDialog(NULL));
		return;
	}
	if (lstrcmpi(lpwstr, L"WICBitmap") == 0) {
		teSetObjectRelease(pVarResult, new CteWICBitmap());
		return;
	}
	if (lstrcmpi(lpwstr, L"api") == 0) {
		teSetObjectRelease(pVarResult, new CteWindowsAPI(NULL));
		return;
	}
	CLSID clsid;
	IUnknown *punk;
	if SUCCEEDED(teCLSIDFromProgID(lpwstr, &clsid)) {
		if SUCCEEDED(teCreateInstance(clsid, NULL, NULL, IID_PPV_ARGS(&punk))) {
			teSetObjectRelease(pVarResult, punk);
		}
	}
}

VOID teApiCreateEvent(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, CreateEvent(param[0].lpSecurityAttributes, param[1].boolVal, param[2].boolVal, param[3].lpcwstr));
}

VOID teApiSetEvent(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, SetEvent(param[0].handle));
}

VOID teApiPlaySound(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, PlaySound(param[0].lpcwstr, param[1].hmodule, param[2].dword));
}

VOID teApiSHCreateStreamOnFileEx(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IUnknown *punk = NULL;
	IStream *pStream0 = NULL;
	if (nArg >= 4 && FindUnknown(&pDispParams->rgvarg[nArg - 4], &punk)) {
		punk->QueryInterface(IID_PPV_ARGS(&pStream0));
	}
	IStream *pStream = NULL;
	if (nArg >= 5) {
		teSetLong(pVarResult, SHCreateStreamOnFileEx(param[0].lpcwstr, param[1].dword, param[2].dword, param[3].boolVal, pStream0, &pStream));
		if (pStream) {
			if (FindUnknown(&pDispParams->rgvarg[nArg - 5], &punk)) {
				VARIANT v;
				v.vt = VT_DISPATCH;
				v.pdispVal = new CteObject(pStream);
				tePutPropertyAt(punk, 0, &v);
				VariantClear(&v);
			}
			pStream->Release();
		}
		return;
	}
	SHCreateStreamOnFileEx(param[0].lpcwstr, param[1].dword, param[2].dword, param[3].boolVal, pStream0, &pStream);
	teSetObjectRelease(pVarResult, new CteObject(pStream));
	SafeRelease(&pStream);
}

VOID teApiHasThumbnail(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	LPITEMIDLIST pidl;
	if (teGetIDListFromVariant(&pidl, &pDispParams->rgvarg[nArg])) {
		IShellFolder *pSF;
		LPCITEMIDLIST pidlPart;
		if SUCCEEDED(SHBindToParent(pidl, IID_PPV_ARGS(&pSF), &pidlPart)) {
			IExtractImage *pEI;
			IThumbnailProvider *pTP;
			if (pSF->GetUIObjectOf(NULL, 1, &pidlPart, IID_IThumbnailProvider, NULL, (void **)&pTP) == S_OK) {
				teSetLong(pVarResult, 1);
				pTP->Release();
			} else if (pSF->GetUIObjectOf(NULL, 1, &pidlPart, IID_IExtractImage, NULL, (void **)&pEI) == S_OK) {
				teSetLong(pVarResult, 2);
				pEI->Release();
			} else {
				teSetLong(pVarResult, 0);
			}
			pSF->Release();
		}
	}
}

VOID teApiGetDiskFreeSpaceEx(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	ULARGE_INTEGER FreeBytesOfAvailable;
	ULARGE_INTEGER TotalNumberOfBytes;
	ULARGE_INTEGER TotalNumberOfFreeBytes;
	if (pVarResult && GetDiskFreeSpaceEx(param[0].lpcwstr, &FreeBytesOfAvailable, &TotalNumberOfBytes, &TotalNumberOfFreeBytes)) {
		pVarResult->vt = VT_DISPATCH;
		pVarResult->pdispVal = NULL;
		GetNewObject(&pVarResult->pdispVal);
		VARIANT v;
		teSetLL(&v, FreeBytesOfAvailable.QuadPart);
		tePutProperty(pVarResult->pdispVal, L"FreeBytesOfAvailable", &v);
		VariantClear(&v);
		teSetLL(&v, TotalNumberOfBytes.QuadPart);
		tePutProperty(pVarResult->pdispVal, L"TotalNumberOfBytes", &v);
		VariantClear(&v);
		teSetLL(&v, TotalNumberOfFreeBytes.QuadPart);
		tePutProperty(pVarResult->pdispVal, L"TotalNumberOfFreeBytes", &v);
		VariantClear(&v);
	}
}

VOID teApiSetSysColors(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, SetSysColors(param[0].intVal, (INT *)GetpcFromVariant(&pDispParams->rgvarg[nArg - 1], NULL), (COLORREF *)GetpcFromVariant(&pDispParams->rgvarg[nArg - 2], NULL)));
}

VOID teApiIsClipboardFormatAvailable(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, IsClipboardFormatAvailable(param[0].uintVal));
}

VOID teApiCreateFile(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetPtr(pVarResult, ::CreateFile(param[0].lpcwstr, param[1].dword, param[2].dword, param[3].lpSecurityAttributes, param[4].dword, param[5].dword, param[6].handle));
}

VOID teApiGetPixel(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetLong(pVarResult, ::GetPixel(param[0].hdc, param[1].intVal, param[2].intVal));
}

VOID teApiShouldAppsUseDarkMode(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	teSetBool(pVarResult, g_bDarkMode);
}

/*
VOID teApi(int nArg, teParam *param, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
}

*/

//
TEDispatchApi dispAPI[] = {
	{ 1,  0, -1, -1, "CreateObject", teApiCreateObject },
	{ 1, -1, -1, -1, "Memory", teApiMemory },
	{ 1,  5, -1, -1, "ContextMenu", teApiContextMenu },
	{ 1, -1, -1, -1, "DropTarget", teApiDropTarget },
	{ 0, -1, -1, -1, "DataObject", teApiDataObject },
	{ 5,  1, -1, -1, "OleCmdExec", teApiOleCmdExec },
	{ 1, -1, -1, -1, "sizeof", teApisizeof },
	{ 1, -1, -1, -1, "LowPart", teApiLowPart },
	{ 1, -1, -1, -1, "ULowPart", teApiULowPart },
	{ 1, -1, -1, -1, "HighPart", teApiHighPart },
	{ 1, -1, -1, -1, "QuadPart", teApiQuadPart },
	{ 1, -1, -1, -1, "UQuadPart", teApiUQuadPart },
	{ 1, -1, -1, -1, "pvData", teApipvData },
	{ 3,  1, -1, -1, "ExecMethod", teApiExecMethod },
	{ 4,  2,  3, -1, "Extract", teApiExtract },
	{ 2, -1, -1, -1, "Add", teApiQuadAdd },
	{ 2, -1, -1, -1, "UQuadAdd", teApiUQuadAdd },
	{ 2, -1, -1, -1, "UQuadSub", teApiUQuadSub },
	{ 2, -1, -1, -1, "UQuadCmp", teApiUQuadCmp },
	{ 2,  0,  1, -1, "FindWindow", teApiFindWindow },
	{ 4,  2,  3, -1, "FindWindowEx", teApiFindWindowEx },
	{ 0, -1, -1, -1, "OleGetClipboard", teApiOleGetClipboard },
	{ 1, -1, -1, -1, "OleSetClipboard", teApiOleSetClipboard },
	{ 2,  0,  1, -1, "PathMatchSpec", teApiPathMatchSpec },
	{ 1,  0, -1, -1, "CommandLineToArgv", teApiCommandLineToArgv },
	{ 2, -1, -1, -1, "ILIsEqual", teApiILIsEqual },
	{ 1, -1, -1, -1, "ILClone", teApiILClone },
	{ 3, -1, -1, -1, "ILIsParent", teApiILIsParent },
	{ 1, -1, -1, -1, "ILRemoveLastID", teApiILRemoveLastID },
	{ 1, -1, -1, -1, "ILFindLastID", teApiILFindLastID },
	{ 1, -1, -1, -1, "ILIsEmpty", teApiILIsEmpty },
	{ 1, -1, -1, -1, "ILGetParent", teApiILGetParent },
	{ 2, 10, -1, -1, "FindFirstFile", teApiFindFirstFile },
	{ 1, -1, -1, -1, "WindowFromPoint", teApiWindowFromPoint },
	{ 0, -1, -1, -1, "GetThreadCount", teApiGetThreadCount },
	{ 0, -1, -1, -1, "DoEvents", teApiDoEvents },
	{ 2,  0,  1, -1, "sscanf", teApisscanf },
	{ 4, -1, -1, -1, "SetFileTime", teApiSetFileTime },
	{ 1, -1, -1, -1, "ILCreateFromPath", teApiILCreateFromPath },
	{ 2,  0, -1, -1, "GetProcObject", teApiGetProcObject },
	{ 1,  0, -1, -1, "SetDllDirectory", teApiSetDllDirectory },
	{ 1,  0, -1, -1, "PathIsNetworkPath", teApiPathIsNetworkPath },
	{ 1,  0, -1, -1, "RegisterWindowMessage", teApiRegisterWindowMessage },
	{ 2,  0,  1, -1, "StrCmpI", teApiStrCmpI },
	{ 1,  0, -1, -1, "StrLen", teApiStrLen },
	{ 3,  0,  1, -1, "StrCmpNI", teApiStrCmpNI },
	{ 2,  0,  1, -1, "StrCmpLogical", teApiStrCmpLogical },
	{ 1,  0, -1, -1, "PathQuoteSpaces", teApiPathQuoteSpaces },
	{ 1,  0, -1, -1, "PathUnquoteSpaces", teApiPathUnquoteSpaces },
	{ 1,  0, -1, -1, "GetShortPathName", teApiGetShortPathName },
	{ 1,  0, -1, -1, "PathCreateFromUrl", teApiPathCreateFromUrl },
	{ 1,  0, -1, -1, "UrlCreateFromPath", teApiUrlCreateFromPath },
	{ 1,  0, -1, -1, "PathSearchAndQualify", teApiPathSearchAndQualify },
	{ 3,  0, -1, -1, "PSFormatForDisplay", teApiPSFormatForDisplay },
	{ 1,  0, -1, -1, "PSGetDisplayName", teApiPSGetDisplayName },
	{ 1, -1, -1, -1, "GetCursorPos", teApiGetCursorPos },
	{ 1, -1, -1, -1, "GetKeyboardState", teApiGetKeyboardState },
	{ 1, -1, -1, -1, "SetKeyboardState", teApiSetKeyboardState },
	{ 1, -1, -1, -1, "GetVersionEx", teApiGetVersionEx },
	{ 1, -1, -1, -1, "ChooseFont", teApiChooseFont },
	{ 1, -1, -1, -1, "ChooseColor", teApiChooseColor },
	{ 1, -1, -1, -1, "TranslateMessage", teApiTranslateMessage },
	{ 1, -1, -1, -1, "ShellExecuteEx", teApiShellExecuteEx },
	{ 1, -1, -1, -1, "CreateFontIndirect", teApiCreateFontIndirect },
	{ 1, -1, -1, -1, "CreateIconIndirect", teApiCreateIconIndirect },
	{ 1, -1, -1, -1, "FindText", teApiFindText },
	{ 1, -1, -1, -1, "FindReplace", teApiFindReplace },
	{ 1, -1, -1, -1, "DispatchMessage", teApiDispatchMessage },
	{ 1, -1, -1, -1, "PostMessage", teApiPostMessage },
	{ 1, -1, -1, -1, "SHFreeNameMappings", teApiSHFreeNameMappings },
	{ 1, -1, -1, -1, "CoTaskMemFree", teApiCoTaskMemFree },
	{ 1, -1, -1, -1, "Sleep", teApiSleep },
	{ 6,  2,  3,  4, "ShRunDialog", teApiShRunDialog },
	{ 2, -1, -1, -1, "DragAcceptFiles", teApiDragAcceptFiles },
	{ 1, -1, -1, -1, "DragFinish", teApiDragFinish },
	{ 2, -1, -1, -1, "mouse_event", teApimouse_event },
	{ 4, -1, -1, -1, "keybd_event", teApikeybd_event },
	{ 4, -1, -1, -1, "SHChangeNotify", teApiSHChangeNotify },
	{ 4, -1, -1, -1, "DrawIcon", teApiDrawIcon },
	{ 1, -1, -1, -1, "DestroyMenu", teApiDestroyMenu },
	{ 1, -1, -1, -1, "FindClose", teApiFindClose },
	{ 1, -1, -1, -1, "FreeLibrary", teApiFreeLibrary },
	{ 1, -1, -1, -1, "ImageList_Destroy", teApiImageList_Destroy },
	{ 1, -1, -1, -1, "DeleteObject", teApiDeleteObject },
	{ 1, -1, -1, -1, "DestroyIcon", teApiDestroyIcon },
	{ 1, -1, -1, -1, "IsWindow", teApiIsWindow },
	{ 1, -1, -1, -1, "IsWindowVisible", teApiIsWindowVisible },
	{ 1, -1, -1, -1, "IsZoomed", teApiIsZoomed },
	{ 1, -1, -1, -1, "IsIconic", teApiIsIconic },
	{ 1, -1, -1, -1, "OpenIcon", teApiOpenIcon },
	{ 1, -1, -1, -1, "SetForegroundWindow", teApiSetForegroundWindow },
	{ 1, -1, -1, -1, "BringWindowToTop", teApiBringWindowToTop },
	{ 1, -1, -1, -1, "DeleteDC", teApiDeleteDC },
	{ 1, -1, -1, -1, "CloseHandle", teApiCloseHandle },
	{ 1, -1, -1, -1, "IsMenu", teApiIsMenu },
	{ 6, -1, -1, -1, "MoveWindow", teApiMoveWindow },
	{ 5, -1, -1, -1, "SetMenuItemBitmaps", teApiSetMenuItemBitmaps },
	{ 2, -1, -1, -1, "ShowWindow", teApiShowWindow },
	{ 3, -1, -1, -1, "DeleteMenu", teApiDeleteMenu },
	{ 3, -1, -1, -1, "RemoveMenu", teApiRemoveMenu },
	{ 9, -1, -1, -1, "DrawIconEx", teApiDrawIconEx },
	{ 3, -1, -1, -1, "EnableMenuItem", teApiEnableMenuItem },
	{ 2, -1, -1, -1, "ImageList_Remove", teApiImageList_Remove },
	{ 3, -1, -1, -1, "ImageList_SetIconSize", teApiImageList_SetIconSize },
	{ 6, -1, -1, -1, "ImageList_Draw", teApiImageList_Draw },
	{10, -1, -1, -1, "ImageList_DrawEx", teApiImageList_DrawEx },
	{ 2, -1, -1, -1, "ImageList_SetImageCount", teApiImageList_SetImageCount },
	{ 3, -1, -1, -1, "ImageList_SetOverlayImage", teApiImageList_SetOverlayImage },
	{ 5, -1, -1, -1, "ImageList_Copy", teApiImageList_Copy },
	{ 1, -1, -1, -1, "DestroyWindow", teApiDestroyWindow },
	{ 3, -1, -1, -1, "LineTo", teApiLineTo },
	{ 0, -1, -1, -1, "ReleaseCapture", teApiReleaseCapture },
	{ 2, -1, -1, -1, "SetCursorPos", teApiSetCursorPos },
	{ 2, -1, -1, -1, "DestroyCursor", teApiDestroyCursor },
	{ 2, -1, -1, -1, "SHFreeShared", teApiSHFreeShared },
	{ 0, -1, -1, -1, "EndMenu", teApiEndMenu },
	{ 1, -1, -1, -1, "SHChangeNotifyDeregister", teApiSHChangeNotifyDeregister },
	{ 1, -1, -1, -1, "SHChangeNotification_Unlock", teApiSHChangeNotification_Unlock },
	{ 1, -1, -1, -1, "IsWow64Process", teApiIsWow64Process },
	{ 9, -1, -1, -1, "BitBlt", teApiBitBlt },
	{ 2, -1, -1, -1, "ImmSetOpenStatus", teApiImmSetOpenStatus },
	{ 2, -1, -1, -1, "ImmReleaseContext", teApiImmReleaseContext },
	{ 2, -1, -1, -1, "IsChild", teApiIsChild },
	{ 2, -1, -1, -1, "KillTimer", teApiKillTimer },
	{ 1, -1, -1, -1, "AllowSetForegroundWindow", teApiAllowSetForegroundWindow },
	{ 7, -1, -1, -1, "SetWindowPos", teApiSetWindowPos },
	{ 5,  4, -1, -1, "InsertMenu", teApiInsertMenu },
	{ 2, -1,  1, -1, "SetWindowText", teApiSetWindowText },
	{ 4, -1, -1, -1, "RedrawWindow", teApiRedrawWindow },
	{ 4, -1, -1, -1, "MoveToEx", teApiMoveToEx },
	{ 3, -1, -1, -1, "InvalidateRect", teApiInvalidateRect },
	{ 4, -1, -1, -1, "SendNotifyMessage", teApiSendNotifyMessage },
	{ 5, -1, -1, -1, "PeekMessage", teApiPeekMessage },
	{ 1, -1, -1, -1, "SHFileOperation", teApiSHFileOperation },
	{ 4, -1, -1, -1, "GetMessage", teApiGetMessage },
	{ 2, -1, -1, -1, "GetWindowRect", teApiGetWindowRect },
	{ 2, -1, -1, -1, "GetWindowThreadProcessId", teApiGetWindowThreadProcessId },
	{ 2, -1, -1, -1, "GetClientRect", teApiGetClientRect },
	{ 3, -1, -1, -1, "SendInput", teApiSendInput },
	{ 2, -1, -1, -1, "ScreenToClient", teApiScreenToClient },
	{ 5, -1, -1, -1, "MsgWaitForMultipleObjectsEx", teApiMsgWaitForMultipleObjectsEx },
	{ 2, -1, -1, -1, "ClientToScreen", teApiClientToScreen },
	{ 2, -1, -1, -1, "GetIconInfo", teApiGetIconInfo },
	{ 2, -1, -1, -1, "FindNextFile", teApiFindNextFile },
	{ 3, -1, -1, -1, "FillRect", teApiFillRect },
	{ 2, -1, -1, -1, "Shell_NotifyIcon", teApiShell_NotifyIcon },
	{ 2, -1, -1, -1, "EndPaint", teApiEndPaint },
	{ 2, -1, -1, -1, "ImageList_GetIconSize", teApiImageList_GetIconSize },
	{ 2, -1, -1, -1, "GetMenuInfo", teApiGetMenuInfo },
	{ 2, -1, -1, -1, "SetMenuInfo", teApiSetMenuInfo },
	{ 4, -1, -1, -1, "SystemParametersInfo", teApiSystemParametersInfo },
	{ 3,  1, -1, -1, "GetTextExtentPoint32", teApiGetTextExtentPoint32 },
	{ 4, -1, -1, -1, "SHGetDataFromIDList", teApiSHGetDataFromIDList },
	{ 4, -1, -1, -1, "InsertMenuItem", teApiInsertMenuItem },
	{ 4, -1, -1, -1, "GetMenuItemInfo", teApiGetMenuItemInfo },
	{ 4, -1, -1, -1, "SetMenuItemInfo", teApiSetMenuItemInfo },
	{ 4, -1, -1, -1, "ChangeWindowMessageFilterEx", teApiChangeWindowMessageFilterEx },
	{ 2, -1, -1, -1, "ImageList_SetBkColor", teApiImageList_SetBkColor },
	{ 2, -1, -1, -1, "ImageList_AddIcon", teApiImageList_AddIcon },
	{ 3, -1, -1, -1, "ImageList_Add", teApiImageList_Add },
	{ 3, -1, -1, -1, "ImageList_AddMasked", teApiImageList_AddMasked },
	{ 1, -1, -1, -1, "GetKeyState", teApiGetKeyState },
	{ 1, -1, -1, -1, "GetSystemMetrics", teApiGetSystemMetrics },
	{ 1, -1, -1, -1, "GetSysColor", teApiGetSysColor },
	{ 1, -1, -1, -1, "GetMenuItemCount", teApiGetMenuItemCount },
	{ 1, -1, -1, -1, "ImageList_GetImageCount", teApiImageList_GetImageCount },
	{ 2, -1, -1, -1, "ReleaseDC", teApiReleaseDC },
	{ 2, -1, -1, -1, "GetMenuItemID", teApiGetMenuItemID },
	{ 4, -1, -1, -1, "ImageList_Replace", teApiImageList_Replace },
	{ 3, -1, -1, -1, "ImageList_ReplaceIcon", teApiImageList_ReplaceIcon },
	{ 2, -1, -1, -1, "SetBkMode", teApiSetBkMode },
	{ 2, -1, -1, -1, "SetBkColor", teApiSetBkColor },
	{ 2, -1, -1, -1, "SetTextColor", teApiSetTextColor },
	{ 2, -1, -1, -1, "MapVirtualKey", teApiMapVirtualKey },
	{ 2, -1, -1, -1, "WaitForInputIdle", teApiWaitForInputIdle },
	{ 2, -1, -1, -1, "WaitForSingleObject", teApiWaitForSingleObject },
	{ 3, -1, -1, -1, "GetMenuDefaultItem", teApiGetMenuDefaultItem },
	{ 3, -1, -1, -1, "SetMenuDefaultItem", teApiSetMenuDefaultItem },
	{ 1, -1, -1, -1, "CRC32", teApiCRC32 },
	{ 3,  1, -1, -1, "SHEmptyRecycleBin", teApiSHEmptyRecycleBin },
	{ 0, -1, -1, -1, "GetMessagePos", teApiGetMessagePos },
	{ 2, -1, -1, -1, "ImageList_GetOverlayImage", teApiImageList_GetOverlayImage },
	{ 1, -1, -1, -1, "ImageList_GetBkColor", teApiImageList_GetBkColor },
	{ 3,  1,  2, -1, "SetWindowTheme", teApiSetWindowTheme },
	{ 1, -1, -1, -1, "ImmGetVirtualKey", teApiImmGetVirtualKey },
	{ 1, -1, -1, -1, "GetAsyncKeyState", teApiGetAsyncKeyState },
	{ 6, -1, -1, -1, "TrackPopupMenuEx", teApiTrackPopupMenuEx },
	{ 5,  0, -1, -1, "ExtractIconEx", teApiExtractIconEx },
	{ 3, -1, -1, -1, "GetObject", teApiGetObject },
	{ 6, -1, -1, -1, "MultiByteToWideChar", teApiMultiByteToWideChar },
	{ 8,  2, -1, -1, "WideCharToMultiByte", teApiWideCharToMultiByte },
	{ 2, -1, -1, -1, "GetAttributesOf", teApiGetAttributesOf },
	{ 2, -1, -1, -1, "DoDragDrop", teApiDoDragDrop },//Deprecated
	{ 4, -1, -1, -1, "SHDoDragDrop", teApiSHDoDragDrop },
	{ 3, -1, -1, -1, "CompareIDs", teApiCompareIDs },
	{ 2,  0,  1, -1, "ExecScript", teApiExecScript },
	{ 2,  0,  1, -1, "GetScriptDispatch", teApiGetScriptDispatch },
	{ 2,  1, -1, -1, "GetDispatch", teApiGetDispatch },
	{ 6, -1, -1, -1, "SHChangeNotifyRegister", teApiSHChangeNotifyRegister },
	{ 4,  1,  2, -1, "MessageBox", teApiMessageBox },
	{ 3, -1, -1, -1, "ImageList_GetIcon", teApiImageList_GetIcon },
	{ 5, -1, -1, -1, "ImageList_Create", teApiImageList_Create },
	{ 2, -1, -1, -1, "GetWindowLongPtr", teApiGetWindowLongPtr },
	{ 2, -1, -1, -1, "GetClassLongPtr", teApiGetClassLongPtr },
	{ 2, -1, -1, -1, "GetSubMenu", teApiGetSubMenu },
	{ 2, -1, -1, -1, "SelectObject", teApiSelectObject },
	{ 1, -1, -1, -1, "GetStockObject", teApiGetStockObject },
	{ 1, -1, -1, -1, "GetSysColorBrush", teApiGetSysColorBrush },
	{ 1, -1, -1, -1, "SetFocus", teApiSetFocus },
	{ 1, -1, -1, -1, "GetDC", teApiGetDC },
	{ 1, -1, -1, -1, "CreateCompatibleDC", teApiCreateCompatibleDC },
	{ 0, -1, -1, -1, "CreatePopupMenu", teApiCreatePopupMenu },
	{ 0, -1, -1, -1, "CreateMenu", teApiCreateMenu },
	{ 3, -1, -1, -1, "CreateCompatibleBitmap", teApiCreateCompatibleBitmap },
	{ 3, -1, -1, -1, "SetWindowLongPtr", teApiSetWindowLongPtr },
	{ 3, -1, -1, -1, "SetClassLongPtr", teApiSetClassLongPtr },
	{ 1, -1, -1, -1, "ImageList_Duplicate", teApiImageList_Duplicate },
	{ 2, -1, -1, -1, "SendMessage", teApiSendMessage },
	{ 2, -1, -1, -1, "GetSystemMenu", teApiGetSystemMenu },
	{ 1, -1, -1, -1, "GetWindowDC", teApiGetWindowDC },
	{ 3, -1, -1, -1, "CreatePen", teApiCreatePen },
	{ 1, -1, -1, -1, "SetCapture", teApiSetCapture },
	{ 1, -1, -1, -1, "SetCursor", teApiSetCursor },
	{ 5, -1, -1, -1, "CallWindowProc", teApiCallWindowProc },
	{ 1, -1, -1, -1, "GetWindow", teApiGetWindow },
	{ 1, -1, -1, -1, "GetTopWindow", teApiGetTopWindow },
	{ 3, -1, -1, -1, "OpenProcess", teApiOpenProcess },
	{ 1, -1, -1, -1, "GetParent", teApiGetParent },
	{ 0, -1, -1, -1, "GetCapture", teApiGetCapture },
	{ 1, -1,  0, -1, "GetModuleHandle", teApiGetModuleHandle },
	{ 1, -1, -1, -1, "SHGetImageList", teApiSHGetImageList },
	{ 5, -1, -1, -1, "CopyImage", teApiCopyImage },
	{ 0, -1, -1, -1, "GetCurrentProcess", teApiGetCurrentProcess },
	{ 1, -1, -1, -1, "ImmGetContext", teApiImmGetContext },
	{ 0, -1, -1, -1, "GetFocus", teApiGetFocus },
	{ 0, -1, -1, -1, "GetForegroundWindow", teApiGetForegroundWindow },
	{ 3, -1, -1, -1, "SetTimer", teApiSetTimer },
	{ 2, -1, -1, -1, "LoadMenu", teApiLoadMenu },
	{ 2, -1, -1, -1, "LoadIcon", teApiLoadIcon },
	{ 3, -1, -1, -1, "LoadLibraryEx", teApiLoadLibraryEx },
	{ 6, -1, -1, -1, "LoadImage", teApiLoadImage },
	{ 7, -1, -1, -1, "ImageList_LoadImage", teApiImageList_LoadImage },
	{ 5, 10, -1, -1, "SHGetFileInfo", teApiSHGetFileInfo },
	{12, -1,  1,  2, "CreateWindowEx", teApiCreateWindowEx },
	{ 6, -1,  3,  4, "ShellExecute", teApiShellExecute },
	{ 2, -1, -1, -1, "BeginPaint", teApiBeginPaint },
	{ 2, -1, -1, -1, "LoadCursor", teApiLoadCursor },
	{ 2, -1, -1, -1, "LoadCursorFromFile", teApiLoadCursorFromFile },
	{ 3, -1, -1, -1, "SHParseDisplayName", teApiSHParseDisplayName },
	{ 3, -1, -1, -1, "SHChangeNotification_Lock", teApiSHChangeNotification_Lock },
	{ 1, -1, -1, -1, "GetWindowText", teApiGetWindowText },
	{ 1, -1, -1, -1, "GetClassName", teApiGetClassName },
	{ 1, -1, -1, -1, "GetModuleFileName", teApiGetModuleFileName },
	{ 0, -1, -1, -1, "GetCommandLine", teApiGetCommandLine },
	{ 0, -1, -1, -1, "GetCurrentDirectory", teApiGetCurrentDirectory },//use wsh.CurrentDirectory
	{ 3, -1, -1, -1, "GetMenuString", teApiGetMenuString },
	{ 2, -1, -1, -1, "GetDisplayNameOf", teApiGetDisplayNameOf },
	{ 1, -1, -1, -1, "GetKeyNameText", teApiGetKeyNameText },
	{ 2, -1, -1, -1, "DragQueryFile", teApiDragQueryFile },
	{ 1, -1, -1, -1, "SysAllocString", teApiSysAllocString },
	{ 2, -1, -1, -1, "SysAllocStringLen", teApiSysAllocStringLen },
	{ 2, -1, -1, -1, "SysAllocStringByteLen", teApiSysAllocStringByteLen },
	{ 3, -1,  1, -1, "sprintf", teApisprintf },
	{ 1, -1, -1, -1, "base64_encode", teApibase64_encode },
	{ 2, -1, -1, -1, "LoadString", teApiLoadString },
	{ 4,  2,  3, -1, "AssocQueryString", teApiAssocQueryString },
	{ 1, -1, -1, -1, "StrFormatByteSize", teApiStrFormatByteSize },
	{ 4,  3, -1, -1, "GetDateFormat", teApiGetDateFormat },
	{ 4,  3, -1, -1, "GetTimeFormat", teApiGetTimeFormat },
	{ 2, -1, -1, -1, "GetLocaleInfo", teApiGetLocaleInfo },
	{ 2, -1, -1, -1, "MonitorFromPoint", teApiMonitorFromPoint },
	{ 2, -1, -1, -1, "MonitorFromRect", teApiMonitorFromRect },
	{ 2, -1, -1, -1, "GetMonitorInfo", teApiGetMonitorInfo },
	{ 1,  0, -1, -1, "GlobalAddAtom", teApiGlobalAddAtom },
	{ 1, -1, -1, -1, "GlobalGetAtomName", teApiGlobalGetAtomName },
	{ 1,  0, -1, -1, "GlobalFindAtom", teApiGlobalFindAtom },
	{ 1, -1, -1, -1, "GlobalDeleteAtom", teApiGlobalDeleteAtom },
	{ 4, -1, -1, -1, "RegisterHotKey", teApiRegisterHotKey },
	{ 2, -1, -1, -1, "UnregisterHotKey", teApiUnregisterHotKey },
	{ 1, -1, -1, -1, "ILGetCount", teApiILGetCount },
	{ 2, -1, -1, -1, "SHTestTokenMembership", teApiSHTestTokenMembership },
	{ 2,  1, -1, -1, "ObjGetI", teApiObjGetI },
	{ 3,  1, -1, -1, "ObjPutI", teApiObjPutI },
	{ 5,  1, -1, -1, "DrawText", teApiDrawText },
	{ 5, -1, -1, -1, "Rectangle", teApiRectangle },
	{ 1, 10, -1, -1, "PathIsDirectory", teApiPathIsDirectory },
	{ 2,  0,  1, -1, "PathIsSameRoot", teApiPathIsSameRoot },
	{ 2,  1, -1, -1, "ObjDispId", teApiObjDispId },
	{ 1,  0, -1, -1, "SHSimpleIDListFromPath", teApiSHSimpleIDListFromPath },
	{ 1,  0, -1, -1, "OutputDebugString", teApiOutputDebugString },
	{ 2, -1, -1, -1, "DllGetClassObject", teApiDllGetClassObject },
	{ 0, -1, -1, -1, "IsAppThemed", teApiIsAppThemed },
	{ 6,  0, -1, -1, "SHDefExtractIcon", teApiSHDefExtractIcon },
	{ 3,  1,  2, -1, "URLDownloadToFile", teApiURLDownloadToFile },
	{ 3,  0,  1, -1, "MoveFileEx", teApiMoveFileEx },
	{ 0, -1, -1, -1, "Object", teApiObject },
	{ 0, -1, -1, -1, "Array", teApiArray },
	{ 1,  0, -1, -1, "PathIsRoot", teApiPathIsRoot },
	{ 2, -1, -1, -1, "EnableWindow", teApiEnableWindow },
	{ 1, -1, -1, -1, "IsWindowEnabled", teApiIsWindowEnabled },
	{ 2,  0,  1, -1, "CryptProtectData", teApiCryptProtectData },
	{ 2, -1,  1, -1, "CryptUnprotectData", teApiCryptUnprotectData },
	{ 1,  0, -1, -1, "base64_decode", teApibase64_decode },
	{11, -1, -1, -1, "AlphaBlend", teApiAlphaBlend },
	{11, -1, -1, -1, "StretchBlt", teApiStretchBlt },
	{11, -1, -1, -1, "TransparentBlt", teApiTransparentBlt },
	{ 2,  0,  1,  2, "FormatMessage", teApiFormatMessage },
	{ 2, -1, -1, -1, "GetGUIThreadInfo", teApiGetGUIThreadInfo },
	{ 1, -1, -1, -1, "CreateSolidBrush", teApiCreateSolidBrush },
	{10, -1, -1, -1, "PlgBlt", teApiPlgBlt },
	{ 7, -1, -1, -1, "RoundRect", teApiRoundRect },
	{ 1,  0,  1, -1, "CreateProcess", teApiCreateProcess },
	{ 1,  0, -1, -1, "DeleteFile", teApiDeleteFile },
	{ 4,  3, -1, -1, "CreateEvent", teApiCreateEvent },
	{ 1, -1, -1, -1, "SetEvent", teApiSetEvent },
	{ 3,  0, -1, -1, "PlaySound", teApiPlaySound },
	{ 4,  0, -1, -1, "SHCreateStreamOnFileEx", teApiSHCreateStreamOnFileEx },	
	{ 1, -1, -1, -1, "HasThumbnail", teApiHasThumbnail },
	{ 1,  0, -1, -1, "GetDiskFreeSpaceEx", teApiGetDiskFreeSpaceEx },
	{ 3, -1, -1, -1, "SetSysColors", teApiSetSysColors },
	{ 1, -1, -1, -1, "IsClipboardFormatAvailable", teApiIsClipboardFormatAvailable },
	{ 7,  0, -1, -1, "CreateFile", teApiCreateFile },
	{ 3, -1, -1, -1, "GetPixel", teApiGetPixel },
	{ 0, -1, -1, -1, "ShouldAppsUseDarkMode", teApiShouldAppsUseDarkMode },
//	{ 0, -1, -1, -1, "", teApi },
//	{ 0, -1, -1, -1, "Test", teApiTest },
};

VOID Initlize()
{
	g_maps[MAP_TE] = SortTEMethod(methodTE, _countof(methodTE));
	g_maps[MAP_SB] = SortTEMethod(methodSB, _countof(methodSB));
	g_maps[MAP_TC] = SortTEMethod(methodTC, _countof(methodTC));
	g_maps[MAP_TV] = SortTEMethod(methodTV, _countof(methodTV));
	g_maps[MAP_Mem] = SortTEMethod(methodMem, _countof(methodMem));
	g_maps[MAP_GB] = SortTEMethod(methodGB, _countof(methodGB));
	g_maps[MAP_FIs] = SortTEMethod(methodFIs, _countof(methodFIs));
	g_maps[MAP_CD] = SortTEMethod(methodCD, _countof(methodCD));
	g_maps[MAP_SS] = SortTEStruct(pTEStructs, _countof(pTEStructs));
/*/// For order check
	int *map = g_maps[MAP_SS];
	for (int i = 0; i < _countof(pTEStructs); i++) {
		int j = map[i];
		if (i != j) {
			LPSTR lpi = pTEStructs[i].name;
			LPSTR lpj = pTEStructs[j].name;
			Sleep(0);
		}
	}
*///
#ifdef _USE_BSEARCHAPI
	g_maps[MAP_API] = teSortDispatchApi(dispAPI, _countof(dispAPI));
#endif
	IExplorerBrowser *pEB;
	if (SUCCEEDED(teCreateInstance(CLSID_ExplorerBrowser, NULL, NULL, IID_PPV_ARGS(&pEB)))) {
		IFolderViewOptions *pOptions;
		if SUCCEEDED(pEB->QueryInterface(IID_PPV_ARGS(&pOptions))) {
			g_bCanLayout = TRUE;
			pOptions->Release();
		}
		pEB->Release();
	}
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//	  This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance, LPWSTR szClassName, WNDPROC lpfnWndProc)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= lpfnWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TE));
	wcex.hIconSm		= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_BTNFACE + 1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= szClassName;

	return RegisterClassEx(&wcex);
}

BOOL teFindType(LPWSTR pszText, LPWSTR pszFind)
{
	LPWSTR pszExt1 = pszText;
	LPWSTR pszExt2;
	if (pszFind[0]) {
		while (pszExt2 = StrChr(pszExt1, ',')) {
			if (StrCmpNI(pszFind, pszExt1, (int)(pszExt2 - pszExt1)) == 0) {
				return TRUE;
			}
			pszExt1 = pszExt2 + 1;
		}
		return lstrcmpi(pszText, pszFind) == 0;
	}
	return FALSE;
}

BOOL GetEncoderClsid(LPWSTR pszName, CLSID* pClsid, LPWSTR pszMimeType)
{
	IEnumUnknown *pEnum;
	WCHAR pszType[MAX_PATH];
	WCHAR pszExt[MAX_PATH];
	LPWSTR pszExt0 = PathFindExtension(pszName);
	HRESULT hr = g_pWICFactory->CreateComponentEnumerator(WICEncoder, WICComponentEnumerateDefault, &pEnum);
	if SUCCEEDED(hr) {
		ULONG cbActual = 0;
		IUnknown *punk;
		hr = E_FAIL;
		while (pEnum->Next(1, &punk, &cbActual) == S_OK && hr != S_OK) {
			IWICBitmapCodecInfo *pCodecInfo;
			if SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pCodecInfo))) {
				UINT uActual = 0;
				if (pszMimeType) {
					pszType[0] = NULL;
					pCodecInfo->GetMimeTypes(MAX_PATH, pszType, &uActual);
					if (teFindType(pszType, pszName)) {
						hr = pCodecInfo->GetContainerFormat(pClsid);
					}
				}
				pszExt[0] = NULL;
				pCodecInfo->GetFileExtensions(MAX_PATH, pszExt, &uActual);
				if (teFindType(pszExt, pszExt0)) {
					hr = pCodecInfo->GetContainerFormat(pClsid);
				}
				pCodecInfo->Release();
			}
			punk->Release();
		}
		pEnum->Release();
	}
	if SUCCEEDED(hr) {
		if (pszMimeType) {
			LPWSTR pszExt2 = StrChr(pszType, ',');
			int i = (int)(pszExt2 - pszType + 1);
			if (i <= 0 || i > 15) {
				i = 15;
			}
			lstrcpyn(pszMimeType, pszType, i);
		}
		return TRUE;
	}
	return FALSE;
}

void teCalcClientRect(int *param, LPRECT rc, LPRECT rcClient)
{
	if (param[TE_Left] & 0x3fff) {
		rc->left = (param[TE_Left] * (rcClient->right - rcClient->left)) / 10000 + rcClient->left;
	} else {
		rc->left = param[TE_Left] / 0x4000 + (param[TE_Left] >= 0 ? rcClient->left : rcClient->right);
	}
	if (param[TE_Top] & 0x3fff) {
		rc->top = (param[TE_Top] * (rcClient->bottom - rcClient->top)) / 10000 + rcClient->top;
	} else {
		rc->top = param[TE_Top] / 0x4000 + (param[TE_Top] >= 0 ? rcClient->top : rcClient->bottom);
	}
	if (param[TE_Width] & 0x3fff) {
		rc->right = param[TE_Width] * (rcClient->right - rcClient->left) / 10000 + rc->left;
	} else {
		rc->right = param[TE_Width] / 0x4000 + (param[TE_Width] >= 0 ? rc->left : rcClient->right);
	}

	if (rc->right > rcClient->right) {
		rc->right = rcClient->right;
	}
	if (param[TE_Height] & 0x3fff) {
		rc->bottom = param[TE_Height] * (rcClient->bottom - rcClient->top) / 10000 + rc->top;
	} else {
		rc->bottom = param[TE_Height] / 0x4000 + (param[TE_Height] >= 0 ? rc->top : rcClient->bottom);
	}
	if (rc->bottom > rcClient->bottom) {
		rc->bottom = rcClient->bottom;
	}
}

BOOL teIsHighContrast()
{
	HIGHCONTRAST highContrast = { sizeof(HIGHCONTRAST) };
	return SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(highContrast), &highContrast, FALSE) && (highContrast.dwFlags & HCF_HIGHCONTRASTON);
}

VOID teGetDarkMode()
{
	if (lpfnShouldAppsUseDarkMode && lpfnAllowDarkModeForWindow && lpfnAllowDarkModeForApp) {
		g_bDarkMode = lpfnShouldAppsUseDarkMode() && IsAppThemed() && !teIsHighContrast();
		lpfnAllowDarkModeForApp(g_bDarkMode);
		teSetDarkMode(g_hwndMain);
	}
}

BOOL CanClose(PVOID pObj)
{
	return DoFunc(TE_OnClose, pObj, S_OK) != S_FALSE;
}

VOID CALLBACK teTimerProcFocus(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	try {
		KillTimer(hwnd, idEvent);
		CteShellBrowser *pSB = SBfromhwnd(hwnd);
		if (pSB) {
			pSB->FocusItem(TRUE);
		}
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"teTimerProcFocus";
#endif
	}
}

VOID CALLBACK teTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	RECT rc, rcTab, rcClient;
	KillTimer(hwnd, idEvent);
	try {
		switch (idEvent) {
			case TET_Create:
				if (g_pOnFunc[TE_OnCreate]) {
					DoFunc(TE_OnCreate, g_pTE, E_NOTIMPL);
					DoFunc(TE_OnCreate, g_pWebBrowser, E_NOTIMPL);
					break;
				}
				g_bsDocumentWrite = ::SysAllocStringLen(NULL, MAX_STATUS);
				MultiByteToWideChar(CP_UTF8, 0, lstrcmpi(g_pWebBrowser->m_bstrPath, L"TE32.exe") ? PathFileExists(g_pWebBrowser->m_bstrPath) ?
					"<h1>500 Internal Script Error</h1>" : "<h1>404 File Not Found</h1>" : "<h1>303 Exec Other</h1>",
					-1, g_bsDocumentWrite, MAX_STATUS);
				lstrcat(g_bsDocumentWrite, g_pWebBrowser->m_bstrPath);
				g_nSize = 0;
				g_nLockUpdate = 0;
				::SysReAllocString(&g_pWebBrowser->m_bstrPath, PATH_BLANK);
				g_pWebBrowser->m_pWebBrowser->Navigate(g_pWebBrowser->m_bstrPath, NULL, NULL, NULL, NULL);
				break;
			case TET_Reload:
				if (g_nReload) {
					teRevoke();
#ifndef _DEBUG
					BSTR bsPath, bsQuoted;
					int i = teGetModuleFileName(NULL, &bsPath);
					bsQuoted = teSysAllocStringLen(bsPath, i + 2);
					SysFreeString(bsPath);
					PathQuoteSpaces(bsQuoted);
					ShellExecute(hwnd, NULL, bsQuoted, NULL, NULL, SW_SHOWNOACTIVATE);
					SysFreeString(bsQuoted);
#endif
					PostMessage(hwnd, WM_CLOSE, 0, 0);
				}
				break;
			case TET_Refresh:
				if (g_pWebBrowser) {
					g_nLockUpdate = 0;
					if (g_pWebBrowser->IsBusy()) {
						SetTimer(g_hwndMain, TET_Refresh, 100, teTimerProc);
						return;
					}
					ClearEvents();
					g_pWebBrowser->m_pWebBrowser->Refresh();
				}
				break;
			case TET_Size:
				GetClientRect(hwnd, &rcClient);
				rcClient.left += g_param[TE_Left];
				rcClient.top += g_param[TE_Top];
				rcClient.right -= g_param[TE_Width];
				rcClient.bottom -= g_param[TE_Height];
				CteShellBrowser *pSB;
				BOOL bRedraw;
				bRedraw = FALSE;
				g_nSize--;
				for (UINT i = 0; i < g_ppTC.size(); i++) {
					CteTabCtrl *pTC = g_ppTC[i];
					if (pTC->m_bVisible) {
						pTC->LockUpdate(TRUE);
						try {
							teCalcClientRect(pTC->m_param, &rc, &rcClient);
							if (g_pOnFunc[TE_OnArrange]) {
								VARIANTARG *pv;
								pv = GetNewVARIANT(2);
								teSetObject(&pv[1], pTC);
								teSetObjectRelease(&pv[0], new CteMemory(sizeof(RECT), &rc, 1, L"RECT"));
								Invoke4(g_pOnFunc[TE_OnArrange], NULL, 2, pv);
							}
							if (!pTC->m_bEmpty && pTC->m_bVisible) {
								int nAlign = g_param[TE_Tab] ? pTC->m_param[TC_Align] : 0;
								if (rc.left) {
									rc.left++;
								}
								if (teSetRect(pTC->m_hwndStatic, rc.left, rc.top, rc.right, rc.bottom)) {
									bRedraw = TRUE;
								}
								int left = rc.left;
								int top = rc.top;

								OffsetRect(&rc, -left, -top);
								CopyRect(&rcTab, &rc);
								int nCount = TabCtrl_GetItemCount(pTC->m_hwnd);
								int h = rc.bottom;
								int nBottom = rcTab.bottom;
								if (nCount) {
									DWORD dwStyle = pTC->GetStyle();
									if (nAlign == 4 || nAlign == 5) {
										rc.right = pTC->m_param[TC_TabWidth];
									}
									if ((dwStyle & (TCS_BOTTOM | TCS_BUTTONS)) == (TCS_BOTTOM | TCS_BUTTONS)) {
										SetWindowLong(pTC->m_hwnd, GWL_STYLE, dwStyle & ~TCS_BOTTOM);
										TabCtrl_AdjustRect(pTC->m_hwnd, FALSE, &rc);
										SetWindowLong(pTC->m_hwnd, GWL_STYLE, dwStyle);
										int i = rcTab.bottom - rc.bottom;
										rc.bottom -= rc.top - i;
										rc.top = i;
									} else {
										TabCtrl_AdjustRect(pTC->m_hwnd, FALSE, &rc);
									}
									h = rcTab.bottom - (rc.bottom - rc.top) - 4;
									switch (nAlign) {
										case 0:						//none
											CopyRect(&rc, &rcTab);
											rcTab.top = rcTab.bottom;
											rcTab.left = rcTab.right;
											nBottom = rcTab.bottom;
											break;
										case 2:						//top
											CopyRect(&rc, &rcTab);
											rcTab.bottom = h;
											rc.top += h;
											nBottom = h;
											break;
										case 3:						//bottom
											CopyRect(&rc, &rcTab);
											rcTab.top = rcTab.bottom - h;
											rc.bottom -= h;
											nBottom = rcTab.bottom;
											break;
										case 4:						//left
											SetRect(&rc, pTC->m_param[TC_TabWidth], 0, rcTab.right, rcTab.bottom);
											rcTab.right = rc.left;
											nBottom = h;
											break;
										case 5:						//Right
											SetRect(&rc, 0, 0, rcTab.right - pTC->m_param[TC_TabWidth], rcTab.bottom);
											rcTab.left = rc.right;
											nBottom = h;
											break;
									}
									while (--nCount >= 0) {
										if (nCount != pTC->m_nIndex) {
											pSB = pTC->GetShellBrowser(nCount);
											if (pSB && pSB->m_bVisible) {
												pSB->Show(FALSE, 1);
											}
										}
									}
								}
								MoveWindow(pTC->m_hwndButton, rcTab.left, rcTab.top,
									rcTab.right - rcTab.left, rcTab.bottom - rcTab.top, FALSE);
								pTC->m_nScrollWidth = 0;
								if (nAlign == 4 || nAlign == 5) {
									int h2 = h - rcTab.bottom;
									if (h2 <= 0) {
										h2 = 0;
									} else {
										pTC->m_nScrollWidth = GetSystemMetrics(SM_CXHSCROLL);
									}
									if (h2 < pTC->m_si.nPos) {
										pTC->m_si.nPos = h2;
									}
									ShowScrollBar(pTC->m_hwndButton, SB_VERT, h2 ? TRUE : FALSE);
									pTC->m_si.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
									pTC->m_si.nMax = h;
									pTC->m_si.nPage = rcTab.bottom;
									SetScrollInfo(pTC->m_hwndButton, SB_VERT, &pTC->m_si, TRUE);
									if (pTC->m_param[TE_Flags] & TCS_FIXEDWIDTH) {
										pTC->SetItemSize();
									}
									rcTab.right -= pTC->m_nScrollWidth;
								} else {
									ShowScrollBar(pTC->m_hwndButton, SB_VERT, FALSE);
									if (pTC->m_si.nTrackPos && pTC->m_param[TE_Flags] & TCS_FIXEDWIDTH) {
										pTC->SetItemSize();
									}
									pTC->m_si.nTrackPos = 0;
									pTC->m_si.nPos = 0;
								}
								if (teSetRect(pTC->m_hwnd, 0, -pTC->m_si.nPos, rcTab.right - rcTab.left, (nBottom - rcTab.top) + pTC->m_si.nPos)) {
									ArrangeWindow();
									if (!(pTC->m_param[TE_Flags] & TCS_MULTILINE)) {
										int i = TabCtrl_GetCurSel(pTC->m_hwnd);
										TabCtrl_SetCurSel(pTC->m_hwnd, 0);
										TabCtrl_SetCurSel(pTC->m_hwnd, i);
									}
								}
								pSB = pTC->GetShellBrowser(pTC->m_nIndex);
								if (!pSB) {
									pSB = GetNewShellBrowser(pTC);
								}
								if (pSB) {
									if (pSB->m_param[SB_TreeAlign] & 2) {
										if (pSB->m_pTV->m_bSetRoot && (pSB->m_pTV->m_pNameSpaceTreeControl
#ifdef _2000XP
											|| pSB->m_pTV->m_pShellNameSpace
#endif
										)) {
											pSB->m_pTV->SetRoot();
										}
										RECT rcTree = rc;
										rcTree.right = rc.left + pSB->m_param[SB_TreeWidth] - 2;
										MoveWindow(pSB->m_pTV->m_hwnd, rcTree.left, rcTree.top, rcTree.right - rcTree.left, rcTree.bottom - rcTree.top, TRUE);
#ifdef _2000XP
										pSB->m_pTV->SetObjectRect();
#endif
										rc.left += pSB->m_param[SB_TreeWidth];
									}
									if (pSB->m_bVisible) {
										teSetParent(pSB->m_pTV->m_hwnd, pSB->m_pTC->m_hwndStatic);
										ShowWindow(pSB->m_pTV->m_hwnd, (pSB->m_param[SB_TreeAlign] & 2) ? SW_SHOWNA : SW_HIDE);
									} else {
										pSB->Show(TRUE, 0);
									}
									pSB->SetFolderFlags(FALSE);
									MoveWindow(pSB->m_hwnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, FALSE);
									if (pSB->m_hwndAlt) {
										MoveWindow(pSB->m_hwndAlt, 0, 0, rc.right - rc.left, rc.bottom - rc.top, FALSE);
									}
								}
							}
						} catch (...) {
							g_nException = 0;
#ifdef _DEBUG
							g_strException = L"TimerProc/LockUpdate";
#endif
						}
						pTC->UnlockUpdate();
					}
				}
				if (g_pWebBrowser) {
					IOleInPlaceObject *pOleInPlaceObject;
					g_pWebBrowser->m_pWebBrowser->QueryInterface(IID_PPV_ARGS(&pOleInPlaceObject));
					GetClientRect(hwnd, &rc);
					pOleInPlaceObject->SetObjectRects(&rc, &rc);
					pOleInPlaceObject->Release();
					if (bRedraw) {
						RedrawWindow(g_pWebBrowser->m_hwndBrowser, NULL, 0, RDW_NOERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
					}
				}
				break;
			case TET_Redraw:
				if (g_nLockUpdate) {
					SetTimer(g_hwndMain, TET_Redraw, 500, teTimerProc);
					break;
				}
				for (UINT i = 0; i < g_ppTC.size(); i++) {
					g_ppTC[i]->RedrawUpdate();
				}
				break;
			case TET_Unload:
				g_bUnload = FALSE;
				if (g_pUnload) {
					GetNewArray(&g_pUnload);
					teExecMethod(g_pJS, L"CollectGarbage", NULL, 0, NULL);
				}
				break;
			case TET_FreeLibrary:
				teFreeLibraries();
				break;
			case TET_Status:
				pSB = g_pTC->GetShellBrowser(g_pTC->m_nIndex);
				if (pSB) {
					if (pSB->m_bBeforeNavigate) {
						SetTimer(g_hwndMain, TET_Status, 1000, teTimerProc);
						break;
					}
					if (pSB->m_bRedraw) {
						pSB->m_bRedraw = FALSE;
						pSB->SetRedraw(TRUE);
						RedrawWindow(pSB->m_hwnd, NULL, 0, RDW_NOERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
					}
					g_szStatus[0] = NULL;
					IFolderView *pFV;
					if (pSB->m_pShellView && SUCCEEDED(pSB->m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV)))) {
						int nCount;
						if SUCCEEDED(pFV->ItemCount(SVGIO_SELECTION, &nCount)) {
							UINT uID = 0;
							if (nCount) {
								uID = nCount > 1 ? 38194 : 38195;
							} else if SUCCEEDED(pFV->ItemCount(SVGIO_ALLVIEW, &nCount)) {
								uID = nCount > 1 ? 38192 : 38193;
							}
							if (uID) {
								BSTR bsCount = ::SysAllocStringLen(NULL, 16);
								WCHAR pszNum[16];
								swprintf_s(pszNum, 12, L"%d", nCount);
								teCommaSize(pszNum, bsCount, 16, 0);

								WCHAR psz[MAX_STATUS];
								if (LoadString(g_hShell32, uID, psz, _countof(psz)) > 2 && tePathMatchSpec(psz, L"*%s*")) {
									swprintf_s(g_szStatus, _countof(g_szStatus), psz, bsCount);
								} else {
									uID = uID < 38194 ? 6466 : 6477;
									if (LoadString(g_hShell32, uID, psz, MAX_STATUS) > 6) {
										FormatMessage(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY,
											psz, 0, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), g_szStatus, _countof(g_szStatus), (va_list *)&bsCount);
									}
								}
								::SysFreeString(bsCount);
							}
						}
						pFV->Release();
					}
					DoStatusText(pSB, g_szStatus, 0);
				}
				break;
			case TET_Title:
				SetWindowText(hwnd, g_szTitle);
				break;
			case TET_EndThread:
				DoFunc(TE_OnEndThread, g_pTE, S_OK);
				break;
		}//end_switch
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"teTimerProc";
#endif
	}
}

VOID CALLBACK teTimerProcSW(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	try {
		KillTimer(hwnd, idEvent);
		CteWebBrowser *pWB = (CteWebBrowser *)idEvent;
		if (GetFocus() == pWB->m_hwndParent) {
			IHTMLWindow2 *pwin;
			if (teGetHTMLWindow(pWB->m_pWebBrowser, IID_PPV_ARGS(&pwin))) {
				pwin->focus();
				pwin->Release();
			}
		}
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"teTimerProcSW";
#endif
	}
}

VOID CALLBACK teTimerProcGroupBy(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	KillTimer(hwnd, idEvent);
	TEGroupBy *pGB = (TEGroupBy *)idEvent;
	try {
		CteShellBrowser *pSB = SBfromhwnd(hwnd);
		if (pSB && pSB->m_dwSessionId == pGB->dwSessionId) {
			pSB->SetGroupBy(pGB->bs);
		}
		::SysFreeString(pGB->bs);
		delete [] pGB;
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"teTimerProcGroupBy";
#endif
	}
}

LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	LPCWPSTRUCT pcwp;
	CteShellBrowser *pSB;
	try {
		if (nCode >= 0) {
			if (nCode == HC_ACTION) {
				if (wParam == NULL) {
					pcwp = (LPCWPSTRUCT)lParam;
					if (pcwp->message == WM_ACTIVATE) {
						SetTimer(g_hwndMain, TET_Redraw, 500, teTimerProc);
					}
					switch (pcwp->message)
					{
						case WM_KILLFOCUS:
							if (!IsChild(g_pWebBrowser->get_HWND(), pcwp->hwnd)) {
								break;
							}
						case WM_SHOWWINDOW:
						case WM_ACTIVATE:
						case WM_ACTIVATEAPP:
						case WM_INITDIALOG:
							if (g_pOnFunc[TE_OnSystemMessage]) {
								IDispatch *pdisp;
								if (ControlFromhwnd(&pdisp, pcwp->hwnd) == S_OK) {
									MSG msg1;
									msg1.hwnd = pcwp->hwnd;
									msg1.message = pcwp->message;
									msg1.wParam = pcwp->wParam;
									msg1.lParam = pcwp->lParam;
									MessageSub(TE_OnSystemMessage, pdisp, &msg1, NULL);
								}
							}
							break;
						case WM_SETFOCUS:
							CheckChangeTabSB(pcwp->hwnd);
							break;
						case WM_COMMAND:
							if (pcwp->message == WM_COMMAND && g_hDialog == pcwp->hwnd) {
								if (LOWORD(pcwp->wParam) == IDOK) {
									g_bDialogOk = TRUE;
								}
							}
							break;
						case LVM_SETTEXTCOLOR:
							pSB = SBfromhwnd(pcwp->hwnd);
							if (pSB) {
								if (!pSB->m_bRefreshing) {
									pSB->m_clrText = (COLORREF)pcwp->lParam;
								}
							}
							break;
						case LVM_SETBKCOLOR:
							pSB = SBfromhwnd(pcwp->hwnd);
							if (pSB) {
								if (!pSB->m_bRefreshing) {
									pSB->m_clrBk = (COLORREF)pcwp->lParam;
								}
							}
							break;
						case LVM_SETTEXTBKCOLOR:
							pSB = SBfromhwnd(pcwp->hwnd);
							if (pSB) {
								if (!pSB->m_bRefreshing) {
									pSB->m_clrTextBk = (COLORREF)pcwp->lParam;
								}
							}
							break;
						case LVM_SETCOLUMNWIDTH:
							pSB = SBfromhwnd(pcwp->hwnd);
							if (pSB && pSB->m_param[SB_ViewMode] == FVM_LIST) {
								if (pcwp->lParam < 0) {
									pSB->m_bSetListColumnWidth = TRUE;
									pSB->SetListColumnWidth();
									return 0;
								}
								pSB->m_bSetListColumnWidth = FALSE;
							}
							break;
						case WM_DRAWITEM:
							if (lpfnIsDarkModeAllowedForWindow) {
								LPDRAWITEMSTRUCT pDrawItem = (LPDRAWITEMSTRUCT)pcwp->lParam;
								if (lpfnIsDarkModeAllowedForWindow(pDrawItem->hwndItem)) {
									CHAR szClassA[MAX_CLASS_NAME];
									GetClassNameA(pDrawItem->hwndItem, szClassA, MAX_CLASS_NAME);
									if (lstrcmpA(szClassA, WC_HEADERA) == 0) {
										SetTextColor(pDrawItem->hDC, 0xffffff);
									}
								}
							}
							break;
					}//end_switch
				}
			}
		}
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"HookProc";
#endif
	}
	return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

UINT_PTR CALLBACK OFNHookProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LPOFNOTIFY pNotify;
	try {
		switch(msg){
			case WM_INITDIALOG:
				g_hDialog = GetParent(hwnd);
				return TRUE;
			case WM_NOTIFY:
				pNotify = (LPOFNOTIFY)lParam;
				if (pNotify->hdr.code == CDN_FOLDERCHANGE) {
					if (g_bDialogOk) {
						HWND hDlg = GetParent(hwnd);
						LRESULT nLen = SendMessage(hDlg, CDM_GETFOLDERIDLIST, 0, NULL);
						if (nLen) {
							LPITEMIDLIST pidl = (LPITEMIDLIST)::CoTaskMemAlloc(nLen);
							SendMessage(hDlg, CDM_GETFOLDERIDLIST, nLen, (LPARAM)pidl);
							BSTR bs;
							teGetDisplayNameFromIDList(&bs, pidl, SHGDN_FORPARSING | SHGDN_FORADDRESSBAR);
							teCoTaskMemFree(pidl);
							lstrcpyn(pNotify->lpOFN->lpstrFile, bs, pNotify->lpOFN->nMaxFile);
							::SysFreeString(bs);
							PostMessage(GetParent(hwnd), WM_CLOSE, 0, 0);
							return TRUE;
						}
					}
				}
				break;
		}
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"OFNHookProc";
#endif
	}
    return FALSE;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	BSTR bsPath, bsLib;
	BSTR bsIndex = NULL;
	BSTR bsScript = NULL;
	hInst = hInstance;

	//Eliminates the vulnerable to a DLL pre-loading attack.
	//Late Binding
	HINSTANCE hDll = GetModuleHandleA("kernel32.dll");
	if (hDll) {
		lpfnSetDefaultDllDirectories = (LPFNSetDefaultDllDirectories)GetProcAddress(hDll, "SetDefaultDllDirectories");
		if (lpfnSetDefaultDllDirectories) {
			lpfnSetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_SYSTEM32 | LOAD_LIBRARY_SEARCH_USER_DIRS);
		}
#ifdef _2000XP
		lpfnSetDllDirectoryW = (LPFNSetDllDirectoryW)GetProcAddress(hDll, "SetDllDirectoryW");
		if (lpfnSetDllDirectoryW) {
			lpfnSetDllDirectoryW(L"");
		}
		lpfnIsWow64Process = (LPFNIsWow64Process)GetProcAddress(hDll, "IsWow64Process");
#else
		SetDllDirectory(L"");
#endif
	}
	::OleInitialize(NULL);
	for (int i = MAX_CSIDL2; i--;) {
		g_pidls[i] = NULL;
		g_bsPidls[i] = NULL;
		if (i < MAX_CSIDL) {
			SHGetFolderLocation(NULL, i, NULL, NULL, &g_pidls[i]);
			if (g_pidls[i]) {
				IShellFolder *pSF;
				LPCITEMIDLIST pidlPart;
				if SUCCEEDED(SHBindToParent(g_pidls[i], IID_PPV_ARGS(&pSF), &pidlPart)) {
					teGetDisplayNameBSTR(pSF, pidlPart, SHGDN_FORADDRESSBAR | SHGDN_FORPARSING, &g_bsPidls[i]);
					pSF->Release();
				}
			}
		}
	}
	g_bUpper10 = teVerifyVersion(10, 0, 0);
#ifdef _2000XP
	g_bUpperVista = teVerifyVersion(6, 0, 0);
#else
	g_pidls[CSIDL_RESULTSFOLDER] = ILCreateFromPathA("shell:::{2965E715-EB66-4719-B53F-1672673BBEFA}");
#endif
#ifdef _W2000
	g_bIsXP = teVerifyVersion(5, 1, 0);
	g_bIs2000 = !g_bUpperVista && !g_bIsXP;
#endif
#ifdef _2000XP
	if (g_bUpperVista) {
		g_pidls[CSIDL_RESULTSFOLDER] = ILCreateFromPathA("shell:::{2965E715-EB66-4719-B53F-1672673BBEFA}");
	} else {
		g_pidls[CSIDL_RESULTSFOLDER] = ILCreateFromPathA("::{E17D4FC0-5564-11D1-83F2-00A0C90DC849}");
		g_bCharWidth = TRUE;
	}
#endif
	g_pidls[CSIDL_LIBRARY] = ILCreateFromPathA("shell:libraries");
	g_pidls[CSIDL_USER] = ILCreateFromPathA("shell:UsersFilesFolder");

	teGetDisplayNameFromIDList(&bsPath, g_pidls[CSIDL_SYSTEM], SHGDN_FORPARSING);

	tePathAppend(&bsLib, bsPath, L"shell32.dll");
	g_hShell32 = GetModuleHandle(bsLib);
	SysFreeString(bsLib);
	if (g_hShell32) {
#ifdef _2000XP
		lpfnSHCreateItemFromIDList = (LPFNSHCreateItemFromIDList)GetProcAddress(g_hShell32, "SHCreateItemFromIDList");
		lpfnSHGetIDListFromObject = (LPFNSHGetIDListFromObject)GetProcAddress(g_hShell32, "SHGetIDListFromObject");
#endif
		lpfnSHRunDialog = (LPFNSHRunDialog)GetProcAddress(g_hShell32, MAKEINTRESOURCEA(61));
		lpfnRegenerateUserEnvironment = (LPFNRegenerateUserEnvironment)GetProcAddress(g_hShell32, "RegenerateUserEnvironment");
	}
#ifdef _2000XP
	if (!lpfnSHGetIDListFromObject) {
		lpfnSHGetIDListFromObject = teGetIDListFromObjectXP;
	}
#endif
#ifdef _W2000
	if (!lpfnSHGetImageList) {
		lpfnSHGetImageList = teSHGetImageList2000;
	}
#endif
#ifdef _2000XP
	tePathAppend(&bsLib, bsPath, L"propsys.dll");
	hDll = GetModuleHandle(bsLib);
	if (hDll) {
		lpfnPSPropertyKeyFromString = (LPFNPSPropertyKeyFromString)GetProcAddress(hDll, "PSPropertyKeyFromString");
		lpfnPSGetPropertyKeyFromName = (LPFNPSGetPropertyKeyFromName)GetProcAddress(hDll, "PSGetPropertyKeyFromName");
		lpfnPSGetPropertyDescription = (LPFNPSGetPropertyDescription)GetProcAddress(hDll, "PSGetPropertyDescription");
		lpfnPSStringFromPropertyKey = (LPFNPSStringFromPropertyKey)GetProcAddress(hDll, "PSStringFromPropertyKey");
		lpfnPropVariantToVariant = (LPFNPropVariantToVariant)GetProcAddress(hDll, "PropVariantToVariant");
		lpfnVariantToPropVariant = (LPFNVariantToPropVariant)GetProcAddress(hDll, "VariantToPropVariant");
		lpfnPSPropertyKeyFromStringEx = tePSPropertyKeyFromStringEx;
	}
	else {
		lpfnPSPropertyKeyFromStringEx = tePSPropertyKeyFromStringXP;
	}
	SysFreeString(bsLib);
#endif
	tePathAppend(&bsLib, bsPath, L"user32.dll");
	hDll = GetModuleHandle(bsLib);
	if (hDll) {
		lpfnChangeWindowMessageFilterEx = (LPFNChangeWindowMessageFilterEx)GetProcAddress(hDll, "ChangeWindowMessageFilterEx");
#ifdef _2000XP
		lpfnChangeWindowMessageFilter = (LPFNChangeWindowMessageFilter)GetProcAddress(hDll, "ChangeWindowMessageFilter");
		lpfnRemoveClipboardFormatListener = (LPFNRemoveClipboardFormatListener)GetProcAddress(hDll, "RemoveClipboardFormatListener");
		if (lpfnRemoveClipboardFormatListener) {
			lpfnAddClipboardFormatListener = (LPFNAddClipboardFormatListener)GetProcAddress(hDll, "AddClipboardFormatListener");
		}
#endif
	}
	SysFreeString(bsLib);

	tePathAppend(&bsLib, bsPath, L"ntdll.dll");
	hDll = GetModuleHandle(bsLib);
	if (hDll) {
		lpfnRtlGetVersion = (LPFNRtlGetVersion)GetProcAddress(hDll, "RtlGetVersion");
	}
	SysFreeString(bsLib);

	if (teVerifyVersion(10, 0, 17763)) {
		tePathAppend(&bsLib, bsPath, L"uxtheme.dll");
		hDll = GetModuleHandle(bsLib);
		if (hDll) {
			lpfnShouldAppsUseDarkMode = (LPFNShouldAppsUseDarkMode)GetProcAddress(hDll, MAKEINTRESOURCEA(132));
			lpfnAllowDarkModeForWindow = (LPFNAllowDarkModeForWindow)GetProcAddress(hDll, MAKEINTRESOURCEA(133));
			lpfnAllowDarkModeForApp = (LPFNAllowDarkModeForApp)GetProcAddress(hDll, MAKEINTRESOURCEA(135));
			lpfnIsDarkModeAllowedForWindow = (LPFNIsDarkModeAllowedForWindow)GetProcAddress(hDll, MAKEINTRESOURCEA(137));
			teGetDarkMode();
		}
		SysFreeString(bsLib);
	}
	tePathAppend(&bsLib, bsPath, L"dwmapi.dll");
	g_hDwmapi = LoadLibrary(bsLib);
	if (g_hDwmapi) {
		lpfnDwmSetWindowAttribute = (LPFNDwmSetWindowAttribute)GetProcAddress(g_hDwmapi, "DwmSetWindowAttribute");
	}
	SysFreeString(bsLib);

// API Hook test
#ifdef _USE_APIHOOK
	tePathAppend(&bsLib, bsPath, L"advapi32.dll");
	hDll = GetModuleHandle(bsLib);
	if (hDll) {
		lpfnRegQueryValueExW = (LPFNRegQueryValueExW)GetProcAddress(hDll, "RegQueryValueExW");
	}
	SysFreeString(bsLib);

	tePathAppend(&bsLib, bsPath, L"comctl32.dll");
	hDll = GetModuleHandle(NULL);
	if (hDll) {
		DWORD_PTR dwBase = (DWORD_PTR)hDll;
		DWORD dwIdataSize, dwDummy;
		PIMAGE_IMPORT_DESCRIPTOR pImgDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData((HMODULE)dwBase,
			TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &dwIdataSize);
		while(pImgDesc->Name) {
			char *lpModule = (char*)(dwBase + pImgDesc->Name);
			if (!_strcmpi(lpModule, "advapi32.dll")) {
				PIMAGE_THUNK_DATA pIAT = (PIMAGE_THUNK_DATA)(dwBase + pImgDesc->FirstThunk);
				while (pIAT->u1.Function) {
					if ((LPFNRegQueryValueExW)pIAT->u1.Function == lpfnRegQueryValueExW) {
						VirtualProtect(&pIAT->u1.Function, sizeof(pIAT->u1.Function), PAGE_EXECUTE_READWRITE, &dwDummy);
						LPVOID NewProc = &teRegQueryValueExW;
						WriteProcessMemory(GetCurrentProcess(), &pIAT->u1.Function, &NewProc, sizeof(pIAT->u1.Function), &dwDummy);
						break;
					}
					pIAT++;
				}
				break;
			}
			pImgDesc++;
		}
	}
	SysFreeString(bsLib);
#endif
	SysFreeString(bsPath);
#ifdef _2000XP
	if (g_bUpperVista) {
#endif
		teChangeWindowMessageFilterEx(NULL, WM_COPYDATA, MSGFLT_ALLOW, NULL);
#ifdef _2000XP
	}
#endif
	InitCommonControls();

	for (UINT i = 0; i < 256; i++) {
		UINT c = i;
		for (int j = 8; j--;) {
			c = (c & 1) ? (0xedb88320 ^ (c >> 1)) : (c >> 1);
		}
		g_uCrcTable[i] = c;
	}
	g_dwMainThreadId = GetCurrentThreadId();
	teGetModuleFileName(NULL, &bsPath);//Executable Path
	for (int i = 0; bsPath[i]; i++) {
		if (bsPath[i] == '\\') {
			bsPath[i] = '/';
		}
		bsPath[i] = towupper(bsPath[i]);
	}
	int nCrc32 = CalcCrc32((BYTE *)bsPath, lstrlen(bsPath) * sizeof(WCHAR), 0);
	//Command Line
	BOOL bVisible = !teStartsText(L"/run ", lpCmdLine);
	BOOL bNewProcess = teStartsText(L"/open ", lpCmdLine);
	LPWSTR szClass = WINDOW_CLASS2;
	if (bVisible && !bNewProcess) {
		//Multiple Launch
		szClass = WINDOW_CLASS;
		HWND hwndTE = NULL;
		while (hwndTE = FindWindowEx(NULL, hwndTE, szClass, NULL)) {
			if (GetWindowLongPtr(hwndTE, GWLP_USERDATA) == nCrc32) {
				BSTR bs = teGetCommandLine();
				COPYDATASTRUCT cd;
				cd.dwData = 0;
				cd.lpData = bs;
				cd.cbData = ::SysStringByteLen(bs) + sizeof(WCHAR);
				DWORD_PTR dwResult;
				LRESULT lResult = SendMessageTimeout(hwndTE, WM_COPYDATA, nCmdShow, LPARAM(&cd), SMTO_ABORTIFHUNG, 1000 * 30, &dwResult);
				teSysFreeString(&bs);
				if (lResult && dwResult == S_OK) {
					SysFreeString(bsPath);
					Finalize();
					teSetForegroundWindow(hwndTE);
					return FALSE;
				}
			}
		}
	}
	SysFreeString(bsPath);
	Initlize();
	MSG msg;

	//Initialize FolderView & TreeView settings
	g_paramFV[SB_Type] = 1;
	g_paramFV[SB_ViewMode] = FVM_DETAILS;
	g_paramFV[SB_FolderFlags] = FWF_SHOWSELALWAYS;
	g_paramFV[SB_IconSize] = 0;
	g_paramFV[SB_Options] = EBO_ALWAYSNAVIGATE;
	g_paramFV[SB_ViewFlags] = 0;

	g_paramFV[SB_TreeAlign] = 1;
	g_paramFV[SB_TreeWidth] = 200;
	g_paramFV[SB_TreeFlags] = NSTCS_HASEXPANDOS | NSTCS_SHOWSELECTIONALWAYS | NSTCS_BORDER | NSTCS_HASLINES | NSTCS_NOINFOTIP | NSTCS_HORIZONTALSCROLL;
	g_paramFV[SB_EnumFlags] = SHCONTF_FOLDERS;
	g_paramFV[SB_RootStyle] = NSTCRS_VISIBLE | NSTCRS_EXPANDED;

	// Windows Imaging Component
	if FAILED(teCreateInstance(CLSID_WICImagingFactory, NULL, NULL, IID_PPV_ARGS(&g_pWICFactory))) {
		g_pWICFactory = NULL;
	}
	MyRegisterClass(hInstance, szClass, WndProc);

	g_pSortColumnNull[0].propkey = PKEY_Search_Rank;
	g_pSortColumnNull[0].direction = SORT_DESCENDING;
	g_pSortColumnNull[1].propkey = PKEY_DateModified;
	g_pSortColumnNull[1].direction = SORT_DESCENDING;
	g_pSortColumnNull[2].propkey = PKEY_ItemNameDisplay;
	g_pSortColumnNull[2].direction = SORT_ASCENDING;
	// Title & Version
	lstrcpy(g_szTE, _T(PRODUCTNAME) L" " _T(STRING(VER_Y)) L"." _T(STRING(VER_M)) L"." _T(STRING(VER_D)) L" Gaku");
	lstrcpy(g_szTitle, g_szTE);
	if (bVisible) {
		g_bInit = !bNewProcess;
		g_hwndMain = CreateWindow(szClass, g_szTE, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
		if (!bNewProcess) {
			SetWindowLongPtr(g_hwndMain, GWLP_USERDATA, nCrc32);
		}
	} else {
		g_hwndMain = CreateWindowEx(WS_EX_TOOLWINDOW, szClass, g_szTE, WS_POPUP,
		  CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, NULL, NULL, hInstance, NULL);
	}
	if (!g_hwndMain)
	{
		Finalize();
		return FALSE;
	}
	teGetDarkMode();
	// ClipboardFormat
	IDLISTFormat.cfFormat = (CLIPFORMAT)RegisterClipboardFormat(CFSTR_SHELLIDLIST);
	DROPEFFECTFormat.cfFormat = (CLIPFORMAT)RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
	// Hook
	g_hHook = SetWindowsHookEx(WH_CALLWNDPROC, (HOOKPROC)HookProc, hInst, g_dwMainThreadId);
	g_hMouseHook = SetWindowsHookEx(WH_MOUSE, (HOOKPROC)MouseProc, hInst, g_dwMainThreadId);
	// Create own class
	CoCreateGuid(&g_ClsIdSB);
	CoCreateGuid(&g_ClsIdTC);
	CoCreateGuid(&g_ClsIdStruct);
	CoCreateGuid(&g_ClsIdFI);
	// IUIAutomation
	if FAILED(teCreateInstance(CLSID_CUIAutomation, NULL, NULL, IID_PPV_ARGS(&g_pAutomation))) {
		g_pAutomation = NULL;
	}
	IUIAutomationRegistrar *pUIAR;
	if SUCCEEDED(teCreateInstance(CLSID_CUIAutomationRegistrar, NULL, NULL, IID_PPV_ARGS(&pUIAR))) {
		UIAutomationPropertyInfo info = {ItemIndex_Property_GUID, L"ItemIndex", UIAutomationType_Int};
		pUIAR->RegisterProperty(&info, &g_PID_ItemIndex);
		pUIAR->Release();
	}
#ifdef _2000XP
	if (g_bUpperVista) {
#endif
		teCreateInstance(CLSID_DragDropHelper, NULL, NULL, IID_PPV_ARGS(&g_pDropTargetHelper));
#ifdef _2000XP
	}
	if (lpfnAddClipboardFormatListener) {
		lpfnAddClipboardFormatListener(g_hwndMain);
	} else {
		g_hwndNextClip = SetClipboardViewer(g_hwndMain);
	}
#else
	AddClipboardFormatListener(g_hwndMain);
#endif
	//Get JScript Object
	bsScript = teMultiByteToWideChar(CP_UTF8, "\
function _o() {\
	return {};\
}\
function _a() {\
	return [];\
}\
function _t(o) {\
	return {window: {external: o}};\
}\
function _c(s) {\
	return s.replace(/\\0$/, '').split(/\\0/);\
}\
", -1);
	if (ParseScript(bsScript, L"JScript", NULL, &g_pJS, NULL) != S_OK) {
		teSysFreeString(&bsScript);
		PostMessage(g_hwndMain, WM_CLOSE, 0, 0);
		Finalize();
		MessageBoxA(NULL, "503 Script Engine Unavalable", NULL, MB_OK | MB_ICONERROR);
		return FALSE;
	}
	teSysFreeString(&bsScript);
	CoMarshalInterThreadInterfaceInStream(IID_IDispatch, g_pJS, &g_pStrmJS);
	GetNewArray(&g_pUnload);
	InitializeCriticalSection(&g_csFolderSize);
	//WindowsAPI
#ifdef _USE_OBJECTAPI
	GetNewObject(&g_pAPI);
	VARIANT v;
	for (int i = _countof(dispAPI); i--;) {
		teSetObjectRelease(&v, new CteAPI(&dispAPI[i]));
		BSTR bs = teMultiByteToWideChar(CP_UTF8, dispAPI[i].name, -1);
		tePutProperty(g_pAPI, bs, &v);
		::SysFreeString(bs);
	}
	teSetSZ(&v, L"ADODB.Stream");
	tePutProperty(g_pAPI, L"ADBSTRM", &v);
	VariantClear(&v);
#endif
	// CTE
	g_pTE = new CTE(nCmdShow);

	IDispatch *pJS = NULL;
#ifdef _USE_HTMLDOC
	IHTMLDocument2	*pHtmlDoc = NULL;
#else
	VARIANT vWindow;
	VariantInit(&vWindow);
#endif
	teGetModuleFileName(NULL, &bsPath);//Executable Path
	PathRemoveFileSpec(bsPath);
	if (bVisible) {
		LPTSTR *lplpszArgs = NULL;
		LPWSTR lpFile = L"script\\index.html";
		if (bNewProcess && lpCmdLine && lpCmdLine[0]) {
			int nCount = 0;
			lplpszArgs = CommandLineToArgvW(lpCmdLine, &nCount);
			if (nCount > 1) {
				lpFile = lplpszArgs[1];
			}
		}
		if (teIsFileSystem(lpFile) || teStartsText(L"file://", lpFile)) {
			bsIndex = ::SysAllocString(lpFile);
		} else {
			tePathAppend(&bsIndex, bsPath, lpFile);
		}
		if (lplpszArgs) {
			LocalFree(lplpszArgs);
		}
		CoInternetSetFeatureEnabled(FEATURE_DISABLE_NAVIGATION_SOUNDS, SET_FEATURE_ON_PROCESS, TRUE);
		g_pWebBrowser = new CteWebBrowser(g_hwndMain, bsIndex, NULL);
		SetTimer(g_hwndMain, TET_Create, 10000, teTimerProc);
	} else {
		tePathAppend(&bsIndex, bsPath, L"script\\background.js");
		bsScript = teLoadFromFile(bsIndex);
		VARIANT vResult;
		VariantInit(&vResult);
#ifdef _USE_HTMLDOC
		// CLSID_HTMLDocument
		if SUCCEEDED(teCreateInstance(CLSID_HTMLDocument, NULL, NULL, IID_PPV_ARGS(&pHtmlDoc))) {
			ICustomDoc *pCD = NULL;
			if SUCCEEDED(pHtmlDoc->QueryInterface(IID_PPV_ARGS(&pCD))) {
				CteDocHostUIHandler *pDHUIH = new CteDocHostUIHandler();
				pCD->SetUIHandler(pDHUIH);
				pDHUIH->Release();
				pCD->Release();
			}
			SAFEARRAY *psa = SafeArrayCreateVector(VT_VARIANT, 0, 1);
			if (psa) {
				VARIANT *pv;
				SafeArrayAccessData(psa, (LPVOID *)&pv);
				pv->vt = VT_BSTR;
				LPOLESTR lpHtml = L"<html><body><script type='text/javascript'>%s external.Data=_s;</script></body></html>";
				int nSize = lstrlen(lp) + lstrlen(lpHtml) - 1;
				pv->bstrVal = ::SysAllocStringLen(NULL, nSize);
				swprintf_s(pv->bstrVal, nSize, lpHtml, lp);
				SafeArrayUnaccessData(psa);
				pHtmlDoc->write(psa);
				VariantClear(pv);
				pHtmlDoc->close();
				SafeArrayDestroy(psa);
				IDispatch *pdisp;
				if (GetDispatch(&g_pTE->m_Data, &pdisp)) {
					Invoke5(pdisp, DISPID_VALUE, DISPATCH_METHOD, &vResult, 0, NULL);
					pdisp->Release();
				}
			}
		}
#else
		// IActiveScript
		VARIANTARG *pv = GetNewVARIANT(1);
		teSetObject(&pv[0], g_pTE);
		teExecMethod(g_pJS, L"_t", &vWindow, 1, pv);
		ParseScript(bsScript, L"JScript", &vWindow, &pJS, NULL);
		teExecMethod(pJS, L"_s", &vResult, 0, NULL);
		teSysFreeString(&bsScript);
#endif
		bVisible = (vResult.vt == VT_BSTR) && (lstrcmpi(vResult.bstrVal, L"wait") == 0);
		VariantClear(&vResult);
	}
	GetNewObject(&g_pSubWindows);
	if (!bVisible) {
		PostMessage(g_hwndMain, WM_CLOSE, 0, 0);
	}
	teSysFreeString(&bsIndex);
	teSysFreeString(&bsPath);

	// Init ShellWindows
	teRegister(TRUE);
	// Main message loop:
	while (g_bMessageLoop) {
		try {
			if (!GetMessage(&msg, NULL, 0, 0)) {
				break;
			}
			if (MessageProc(&msg) == S_OK) {
				continue;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} catch (...) {
			g_nException = 0;
#ifdef _DEBUG
			g_strException = L"MainMessageLoop";
#endif
		}
	}
	//At the end of processing
	try {
		g_bMessageLoop = FALSE;
		if (g_hbrBackground) {
			DeleteObject(g_hbrBackground);
		}
		RevokeDragDrop(g_hwndMain);
#ifdef _USE_HTMLDOC
		SafeRelease(&pHtmlDoc);
#else
		SafeRelease(&pJS);
		VariantClear(&vWindow);
#endif
		teFreeLibraries();
		SafeRelease(&g_pSubWindows);
		SafeRelease(&g_pUnload);
#ifdef _USE_OBJECTAPI
		SafeRelease(&g_pAPI);
#endif
		SafeRelease(&g_pStrmJS);
		SafeRelease(&g_pJS);
		SafeRelease(&g_pAutomation);
		SafeRelease(&g_pWICFactory);
		SafeRelease(&g_pDropTargetHelper);
		DeleteCriticalSection(&g_csFolderSize);
		UnhookWindowsHookEx(g_hMouseHook);
		UnhookWindowsHookEx(g_hHook);
		if (g_hMenu) {
			DestroyMenu(g_hMenu);
		}
#ifdef	_2000XP
		if (lpfnAddClipboardFormatListener) {
			lpfnRemoveClipboardFormatListener(g_hwndMain);
		} else {
			ChangeClipboardChain(g_hwndMain, g_hwndNextClip);
		}
#else
		RemoveClipboardFormatListener(g_hwndMain);
#endif
	} catch (...) {}
	Finalize();
	SafeRelease(&g_pTE);
	return (int) msg.wParam;
}

VOID ArrangeWindow()
{
	if (g_nSize <= 0) {
		g_nSize++;
		SetTimer(g_hwndMain, TET_Size, 1, teTimerProc);
	}
}

VOID CALLBACK teTimerProcRelease(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	try {
		KillTimer(hwnd, idEvent);
		((IUnknown *)idEvent)->Release();
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"teTimerProcRelease";
#endif
	}
}

VOID teDelayRelease(PVOID ppObj)
{
	try {
		IUnknown **ppunk = static_cast<IUnknown **>(ppObj);
		if (*ppunk) {
			if (*ppunk != g_pRelease) {
				g_pRelease = *ppunk;
				SetTimer(g_hwndMain, (UINT_PTR)*ppunk, 100, teTimerProcRelease);
			} else {
				(*ppunk)->Release();
			}
			*ppunk = NULL;
		}
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"teDelayRelease";
#endif
	}
}

BOOL teReleaseParse(TEInvoke *pInvoke)
{
	BOOL bResult = ::InterlockedDecrement(&pInvoke->cRef) == 0;
	if (bResult) {
		teDelayRelease(&pInvoke->pdisp);
		delete [] pInvoke;
	}
	return bResult;
}

VOID CALLBACK teTimerProcParse(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	KillTimer(hwnd, idEvent);
	TEInvoke *pInvoke = (TEInvoke *)idEvent;
	try {
		if (pInvoke->pidl || pInvoke->wErrorHandling == 3) {
			VariantClear(&pInvoke->pv[pInvoke->cArgs - 1]);
			if (pInvoke->wMode) {
				teSetLong(&pInvoke->pv[pInvoke->cArgs - 1], pInvoke->hr);
			} else if (pInvoke->pidl) {
				CteFolderItem *pid1 = new CteFolderItem(NULL);
				if SUCCEEDED(pid1->Initialize(pInvoke->pidl)) {
					teSetSZ(&pid1->m_v, pInvoke->pv[pInvoke->cArgs - 1].bstrVal);
				}
				teSetObjectRelease(&pInvoke->pv[pInvoke->cArgs - 1], pid1);
			}
			teILFreeClear(&pInvoke->pidl);
			Invoke5(pInvoke->pdisp, pInvoke->dispid, DISPATCH_METHOD, NULL, pInvoke->cArgs, pInvoke->pv);
		} else if (pInvoke->wErrorHandling == 1) {
			VARIANT *pv = &pInvoke->pv[pInvoke->cArgs - 1];
			CteFolderItem *pid = new CteFolderItem(pv);
			pid->MakeUnavailable();
			VariantClear(pv);
			teSetObjectRelease(pv, pid);
			Invoke5(pInvoke->pdisp, pInvoke->dispid, DISPATCH_METHOD, NULL, pInvoke->cArgs, pInvoke->pv);
		} else if (pInvoke->wErrorHandling == 2) {
			if (g_bShowParseError) {
				CteShellBrowser *pSB = NULL;
				pInvoke->pdisp->QueryInterface(g_ClsIdSB, (LPVOID *)&pSB);
				if (pSB == g_pTC->GetShellBrowser(g_pTC->m_nIndex)) {
					g_bShowParseError = FALSE;
					LPWSTR lpBuf;
					WCHAR pszFormat[MAX_STATUS];
					if (LoadString(g_hShell32, 6456, pszFormat, MAX_STATUS)) {
						if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY,
							pszFormat, 0, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&lpBuf, MAX_STATUS, (va_list *)&pInvoke->pv[pInvoke->cArgs - 1].bstrVal)) {
							int r = MessageBox(g_hwndMain, lpBuf, _T(PRODUCTNAME), MB_ABORTRETRYIGNORE);
							LocalFree(lpBuf);
							if (r == IDRETRY) {
								::InterlockedIncrement(&pInvoke->cRef);
								_beginthread(&threadParseDisplayName, 0, pInvoke);
								g_bShowParseError = TRUE;
								return;
							}
							g_bShowParseError = (r != IDIGNORE);
						}
					}
				}
				SafeRelease(&pSB);
			}
			teClearVariantArgs(pInvoke->cArgs, pInvoke->pv);
		}
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"teTimerProcParse";
#endif
	}
	teReleaseParse(pInvoke);
}

VOID CALLBACK teTimerProcSetRoot(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	KillTimer(hwnd, idEvent);
	CteTreeView *pTV = (CteTreeView *)idEvent;
	try {
		if (pTV->m_param[SB_TreeAlign] & 2) {
			pTV->Show();
		}
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"teTimerProcSetRoot";
#endif
	}
}

static void threadParseDisplayName(void *args)
{
	::OleInitialize(NULL);
	TEInvoke *pInvoke = (TEInvoke *)args;
	try {
		if (!pInvoke->pidl) {
			pInvoke->pidl = teILCreateFromPath1(pInvoke->pv[pInvoke->cArgs - 1].bstrVal);
		}
		if (pInvoke->wMode) {
			pInvoke->hr = E_PATH_NOT_FOUND;
			if (pInvoke->pidl) {
				pInvoke->hr = teILFolderExists(pInvoke->pidl);
			}
		}
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"threadParseDisplayName";
#endif
	}
	if (!teReleaseParse(pInvoke)) {
		SetTimer(g_hwndMain, (UINT_PTR)args, 100, teTimerProcParse);
	}
	::OleUninitialize();
	::_endthread();
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MSG msg1;

	try {
		switch (message)
		{
			case WM_CLOSE:
				g_nLockUpdate += 1000;
				try {
					if (CanClose(g_pTE)) {
						teRevoke();
						try {
							DestroyWindow(hWnd);
						} catch (...) {
							g_bMessageLoop = FALSE;
						}
					}
				} catch (...) {
					DestroyWindow(hWnd);
				}
				g_nLockUpdate -= 1000;
				return 0;
			case WM_SIZE:
				ArrangeWindow();
				break;
			case WM_DESTROY:
				try {
					if (g_pOnFunc[TE_OnSystemMessage]) {
						msg1.hwnd = hWnd;
						msg1.message = message;
						msg1.wParam = wParam;
						msg1.lParam = lParam;
						MessageSub(TE_OnSystemMessage, g_pTE, &msg1, NULL);
					}
					//Close Tab
					while (!g_ppTC.empty()) {
						CteTabCtrl *pTC = g_ppTC[g_ppTC.size() - 1];
						pTC->Close(TRUE);
						SafeRelease(&pTC);
						g_ppTC.pop_back();
					}
					//Close Tree
					while (!g_ppTV.empty()) {
						CteTreeView *pTV = g_ppTV[g_ppTV.size() - 1];
						SafeRelease(&pTV);
						g_ppTV.pop_back();
					}
					//Close Browser
					if (g_pWebBrowser) {
						g_pWebBrowser->Close();
						SafeRelease(&g_pWebBrowser);
					}
				} catch (...) {
#ifdef _DEBUG
					g_strException = L"WM_DESTROY";
#endif
				}
				PostQuitMessage(0);
				return 0;
			case WM_CONTEXTMENU:	//System Menu
				MSG msg1;
				msg1.hwnd = hWnd;
				msg1.message = message;
				msg1.wParam = wParam;
				msg1.pt.x = GET_X_LPARAM(lParam);
				msg1.pt.y = GET_Y_LPARAM(lParam);
				MessageSubPt(TE_OnShowContextMenu, g_pTE, &msg1);
				break;
			//Menu
			case WM_MENUSELECT:
			case WM_HELP:
			case WM_INITMENU:
			case WM_MEASUREITEM:
			case WM_DRAWITEM:
			case WM_ENTERMENULOOP:
			case WM_INITMENUPOPUP:
			case WM_MENUCHAR:
			case WM_EXITMENULOOP:
			case WM_MENURBUTTONUP:
			case WM_MENUDRAG:
			case WM_MENUGETOBJECT:
			case WM_UNINITMENUPOPUP:
			case WM_MENUCOMMAND:
				msg1.hwnd = hWnd;
				msg1.message = message;
				msg1.wParam = wParam;
				msg1.lParam = lParam;
				BOOL bDone;
				bDone = TRUE;

				if (g_pOnFunc[TE_OnMenuMessage]) {
					HRESULT hr = S_FALSE;
					if (MessageSub(TE_OnMenuMessage, g_pCM, &msg1, &hr) && hr == S_OK) {
						bDone = FALSE;
					}
					if (message == WM_MENUCHAR && hr & 0xffff0000) {
						return hr;
					}
				}
				if (bDone && g_pCM) {
					LRESULT lResult = 0;
					BOOL bRet = FALSE;
					IContextMenu3 *pCM3;
					IContextMenu2 *pCM2;
					DISPID dispid;
					VARIANT v;
					VariantInit(&v);
					IUnknown *punk;
					HRESULT hr = g_pCM->GetNextDispID(fdexEnumAll, DISPID_STARTENUM, &dispid);
					while (hr == S_OK) {
						if (Invoke5(g_pCM, dispid, DISPATCH_PROPERTYGET, &v, 0, NULL) == S_OK) {
							if (FindUnknown(&v, &punk)) {
								if SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pCM3))) {
									bRet = SUCCEEDED(pCM3->HandleMenuMsg2(message, wParam, lParam, &lResult));
									pCM3->Release();
								} else if SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pCM2))) {
									pCM2->HandleMenuMsg(message, wParam, lParam);
									pCM2->Release();
								}
							}
							VariantClear(&v);
						}
						hr = g_pCM->GetNextDispID(fdexEnumAll, dispid, &dispid);
					}
					if (bRet) {
						return lResult;
					}
				}
				break;
			//System
			case WM_SETTINGCHANGE:
				if (message == WM_SETTINGCHANGE) {
					teRegister(FALSE);
					teGetDarkMode();
					if (lpfnRegenerateUserEnvironment) {
						try {
							if (lstrcmpi((LPCWSTR)lParam, L"Environment") == 0) {
								LPVOID lpEnvironment;
								lpfnRegenerateUserEnvironment(&lpEnvironment, TRUE);
								//Not permitted to free lpEnvironment!
								//FreeEnvironmentStrings((LPTSTR)lpEnvironment);
							}
						} catch (...) {
							g_nException = 0;
#ifdef _DEBUG
							g_strException = L"RegenerateUserEnvironment";
#endif
						}
					}
				}
			case WM_COPYDATA:
				if (g_nReload) {
					return E_FAIL;
				}
			case WM_POWERBROADCAST:
			case WM_TIMER:
			case WM_DROPFILES:
			case WM_DEVICECHANGE:
			case WM_FONTCHANGE:
			case WM_ENABLE:
			case WM_NOTIFY:
			case WM_SETCURSOR:
			case WM_SYSCOLORCHANGE:
			case WM_SYSCOMMAND:
			case WM_THEMECHANGED:
			case WM_USERCHANGED:
			case WM_QUERYENDSESSION:
			case WM_HOTKEY:
			case WM_DPICHANGED:
			case WM_NCCALCSIZE:
			case TWM_CLIPBOARDUPDATE:
				if (g_pOnFunc[TE_OnSystemMessage]) {
					msg1.hwnd = hWnd;
					msg1.message = message;
					msg1.wParam = wParam;
					msg1.lParam = lParam;
					HRESULT hr;
					if (MessageSub(TE_OnSystemMessage, g_pTE, &msg1, &hr)) {
						return hr;
					}
				}
				break;
			case WM_COMMAND:
				if (teDoCommand(g_pTE, hWnd, message, wParam, lParam) == S_OK) {
					return 1;
				}
				break;
#ifdef	_2000XP
			case WM_CHANGECBCHAIN:
				if ((HWND)wParam == g_hwndNextClip) {
					g_hwndNextClip = (HWND)lParam;
				} else if (g_hwndNextClip) {
					SendMessage(g_hwndNextClip, message, wParam, lParam);
				}
				return 0;
			case WM_DRAWCLIPBOARD:
				if (g_hwndNextClip) {
					SendMessage(g_hwndNextClip, message, wParam, lParam);
				}
#endif
			case WM_CLIPBOARDUPDATE:
				PostMessage(g_hwndMain, TWM_CLIPBOARDUPDATE, 0, 0);
				break;
			default:
				if ((message > WM_APP && message <= MAXWORD)) {
					if (g_pOnFunc[TE_OnAppMessage]) {
						msg1.hwnd = hWnd;
						msg1.message = message;
						msg1.wParam = wParam;
						msg1.lParam = lParam;
						HRESULT hr;
						if (MessageSub(TE_OnAppMessage, g_pTE, &msg1, &hr) && hr == S_OK) {
							return hr;
						}
					}
				}
				return DefWindowProc(hWnd, message, wParam, lParam);
			case WM_CTLCOLORSTATIC:
				if (g_hbrBackground) {
					return (LRESULT)g_hbrBackground;
				}
				break;
		}//end_switch
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"WndProc";
#endif
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

// Sub window

BOOL teGetSubWindow(HWND hwnd, CteWebBrowser **ppWB)
{
	VARIANT v;
	VariantInit(&v);
	BOOL bResult = SUCCEEDED(teGetPropertyAtLLX(g_pSubWindows, (LONGLONG)hwnd, &v));
	*ppWB = (CteWebBrowser *)v.pdispVal;
	VariantClear(&v);
	return bResult;
}
LRESULT CALLBACK WndProc2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	try {
		CteWebBrowser *pWB;
		switch (message)
		{
			case WM_CLOSE:
				try {
					if (teGetSubWindow(hWnd, &pWB)) {
						if (pWB->m_bstrPath) {
							::SysReAllocString(&pWB->m_bstrPath, PATH_BLANK);
							pWB->m_pWebBrowser->Navigate(pWB->m_bstrPath, NULL, NULL, NULL, NULL);
							teSysFreeString(&pWB->m_bstrPath);
							return 0;
						}
						if (GetForegroundWindow() == pWB->m_hwndParent) {
							teSetForegroundWindow((HWND)GetWindowLongPtr(pWB->m_hwndParent, GWLP_HWNDPARENT));
						}
						pWB->Close();
					}
				} catch(...) {
					g_nException = 0;
#ifdef _DEBUG
					g_strException = L"WM_CLOSE";
#endif
				}
				DestroyWindow(hWnd);
				return 0;
			case WM_SIZE:
				if (teGetSubWindow(hWnd, &pWB)) {
					RECT rc;
					GetClientRect(hWnd, &rc);
					IOleInPlaceObject *pOleInPlaceObject;
					pWB->m_pWebBrowser->QueryInterface(IID_PPV_ARGS(&pOleInPlaceObject));
					pOleInPlaceObject->SetObjectRects(&rc, &rc);
					pOleInPlaceObject->Release();
				}
				break;
			case WM_ACTIVATE:
				if (LOWORD(wParam)) {
					if (teGetSubWindow(hWnd, &pWB)) {
						SetTimer(pWB->m_hwndParent, (UINT_PTR)pWB, 100, teTimerProcSW);
					}
				}
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
		}
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"WndProc2";
#endif
	}
	return 0;
}

// CteShellBrowser

CteShellBrowser::CteShellBrowser(CteTabCtrl *pTC)
{
	m_cRef = 1;
	m_bEmpty = TRUE;
	m_bInit = FALSE;
	m_bsFilter = NULL;
	m_bsNextFilter = NULL;
	m_bsAltSortColumn = NULL;
	m_pTV = new CteTreeView();
	m_dwCookie = 0;
	VariantInit(&m_vData);
	m_nForceViewMode = FVM_AUTO;
	Init(pTC, TRUE);
}

void CteShellBrowser::Init(CteTabCtrl *pTC, BOOL bNew)
{
	m_hwnd = NULL;
	m_hwndLV = NULL;
	m_hwndDV = NULL;
	m_hwndDT = NULL;
	m_hwndAlt = NULL;
	m_pShellView = NULL;
	m_pExplorerBrowser = NULL;
	m_dwEventCookie = 0;
	m_pidl = NULL;
	m_pFolderItem = NULL;
	m_pFolderItem1 = NULL;
	m_nUnload = 0;
	m_DefProc = NULL;
	m_pDropTarget2 = NULL;
	m_pdisp = NULL;
	m_pSF2 = NULL;
	m_bRefreshing = FALSE;
	m_pDTColumns = NULL;
	m_ppColumns = NULL;
#ifdef _2000XP
	m_pSFVCB = NULL;
	m_nColumns = 0;
#endif
	m_bCheckLayout = FALSE;
	m_bRefreshLator = FALSE;
	m_nCreate = 0;
	m_nDefultColumns = 0;
	m_pDefultColumns = NULL;
	m_bNavigateComplete = FALSE;
	m_dwUnavailable = 0;
	VariantClear(&m_vData);

	for (int i = SB_Count; i--;) {
		m_param[i] = g_paramFV[i];
	}

	m_pTC = pTC;
	if (bNew) {
		m_ppLog = new FolderItem*[MAX_HISTORY];
	}
	for (int i = Count_SBFunc; i--;) {
		m_ppDispatch[i] = NULL;
	}
	m_bVisible = FALSE;
	m_nLogCount = 0;
	m_nLogIndex = -1;
	m_nFolderSizeIndex = MAXINT;
	m_nLabelIndex = MAXINT;
	m_nSizeIndex = MAXINT;
	m_nLinkTargetIndex = MAXINT;

	m_pTV->Init(this, m_pTC->m_hwndStatic);
	DoFunc(TE_OnCreate, this, E_NOTIMPL);
}

CteShellBrowser::~CteShellBrowser()
{
	Clear();
}

void CteShellBrowser::Clear()
{
	for (int i = _countof(m_ppDispatch); i-- > 0;) {
		SafeRelease(&m_ppDispatch[i]);
	}
	try {
		DestroyView(0);
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"DestroyView";
#endif
	}
	try {
		while (--m_nLogCount >= 0) {
			SafeRelease(&m_ppLog[m_nLogCount]);
		}
		teSysFreeString(&m_bsFilter);
		teSysFreeString(&m_bsNextFilter);
		teSysFreeString(&m_bsAltSortColumn);
		teUnadviseAndRelease(m_pdisp, DIID_DShellFolderViewEvents, &m_dwCookie);
		m_pdisp = NULL;
#ifdef _2000XP
		SafeRelease(&m_pSFVCB);
		if (m_nColumns) {
			delete [] m_pColumns;
			m_nColumns = 0;
		}
#endif
		SafeRelease(&m_pSF2);
		if (m_pDefultColumns) {
			delete [] m_pDefultColumns;
			m_pDefultColumns = NULL;
		}
		teILFreeClear(&m_pidl);
		SafeRelease(&m_pFolderItem);
		SafeRelease(&m_pFolderItem1);
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"ShellBrowser::Clear";
#endif
	}
	VariantClear(&m_vData);
	if (m_pDTColumns) {
		delete [] m_pDTColumns;
		m_pDTColumns = NULL;
	}
	if (m_ppColumns) {
		while (m_iColumns > 0) {
			SafeRelease(&m_ppColumns[--m_iColumns]);
		}
		delete [] m_ppColumns;
		m_ppColumns = NULL;
	}
	m_dwSessionId = 0;
}

STDMETHODIMP CteShellBrowser::QueryInterface(REFIID riid, void **ppvObject)
{
	static const QITAB qit[] =
	{
		QITABENT(CteShellBrowser, IShellBrowser),
		QITABENT(CteShellBrowser, ICommDlgBrowser),
		QITABENT(CteShellBrowser, ICommDlgBrowser2),
//		QITABENT(CteShellBrowser, ICommDlgBrowser3),
//		QITABENT(CteShellBrowser, IFolderFilter),
		QITABENT(CteShellBrowser, IOleWindow),
		QITABENT(CteShellBrowser, IDispatch),
		QITABENT(CteShellBrowser, IShellFolderViewDual),
		QITABENT(CteShellBrowser, IExplorerBrowserEvents),
		QITABENT(CteShellBrowser, IExplorerPaneVisibility),
		QITABENT(CteShellBrowser, IPersist),
		QITABENT(CteShellBrowser, IPersistFolder),
		QITABENT(CteShellBrowser, IPersistFolder2),
#ifdef _2000XP
		QITABENT(CteShellBrowser, IShellFolderViewCB),
#endif
		{ 0 },
	};
	HRESULT hr = QISearch(this, qit, riid, ppvObject);
	if SUCCEEDED(hr) {
		return hr;
	}
	*ppvObject = NULL;
	if (IsEqualIID(riid, g_ClsIdSB)) {
		*ppvObject = this;
	} else if (IsEqualIID(riid, IID_IShellFolder) || IsEqualIID(riid, IID_IShellFolder2)) {
		if (m_pSF2) {
#ifdef _2000XP
			*ppvObject = static_cast<IShellFolder2 *>(this);
#else
			return m_pSF2->QueryInterface(riid, ppvObject);
#endif
		} else {
			if (m_pShellView) {
				IFolderView *pFV;
				if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV))) {
					hr = pFV->GetFolder(riid, ppvObject);
					pFV->Release();
				}
			}
			return hr;
		}
	} else if (IsEqualIID(riid, IID_IServiceProvider)) {
		*ppvObject = static_cast<IServiceProvider *>(new CteServiceProvider(static_cast<IShellBrowser *>(this), m_pShellView));
		return S_OK;
	} else if (m_pShellView && IsEqualIID(riid, IID_IShellView)) {
		return m_pShellView->QueryInterface(riid, ppvObject);
	} else if (m_pFolderItem && (IsEqualIID(riid, IID_FolderItem) || IsEqualIID(riid, g_ClsIdFI))) {
		return m_pFolderItem->QueryInterface(riid, ppvObject);
	} else {
		if (m_pSF2) {
			return m_pSF2->QueryInterface(riid, ppvObject);
		}
		return E_NOINTERFACE;
	}
	AddRef();
	return S_OK;
}

STDMETHODIMP_(ULONG) CteShellBrowser::AddRef()
{
	return ::InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CteShellBrowser::Release()
{
	if (::InterlockedDecrement(&m_cRef) == 0) {
		return 0;
	}

	return m_cRef;
}

//IOleWindow
STDMETHODIMP CteShellBrowser::GetWindow(HWND *phwnd)
{
	*phwnd = m_pTC->m_hwndStatic;
	return S_OK;
}

STDMETHODIMP CteShellBrowser::ContextSensitiveHelp(BOOL fEnterMode)
{
	return E_NOTIMPL;
}

//IShellBrowser
/*//
void CteShellBrowser::InitializeMenuItem(HMENU hmenu, LPTSTR lpszItemName, int nId, HMENU hmenuSub)
{
	MENUITEMINFO mii = { sizeof(MENUITEMINFO), MIIM_ID | MIIM_STRING | MIIM_SUBMENU, 0, 0, nId, hmenuSub, NULL, NULL, 0, lpszItemName };
	InsertMenuItem(hmenu, nId, FALSE, &mii);
}
*///
STDMETHODIMP CteShellBrowser::InsertMenusSB(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths)
{
/*//
	if (!g_hMenu) {
		g_hMenu = LoadMenu(g_hShell32, MAKEINTRESOURCE(216));
		if (!g_hMenu) {
			TEmethod pull_downs[] = {
				{ FCIDM_MENU_FILE, L"&File" },
				{ FCIDM_MENU_EDIT, L"&Edit" },
				{ FCIDM_MENU_VIEW, L"&View" },
				{ FCIDM_MENU_TOOLS, L"&Tools" },
				{ FCIDM_MENU_HELP, L"&Help" },
			};
			for (size_t i = 0; i < 5; i++) {
				InitializeMenuItem(hmenuShared, pull_downs[i].name, pull_downs[i].id, CreatePopupMenu());
				lpMenuWidths->width[i] = 0;
			}
			lpMenuWidths->width[0] = 2; // FILE: File, Edit
			lpMenuWidths->width[2] = 2; // VIEW: View, Tools
			lpMenuWidths->width[4] = 1; // WINDOW: Help
		}
	}
	return S_OK;
*///
	return E_NOTIMPL;
}

STDMETHODIMP CteShellBrowser::SetMenuSB(HMENU hmenuShared, HOLEMENU holemenuRes, HWND hwndActiveObject)
{
/*//
	if (!g_hMenu) {
		if (hwndActiveObject == m_hwnd) {
			g_hMenu = CreatePopupMenu();
			teCopyMenu(g_hMenu, hmenuShared, MF_ENABLED);
			return S_OK;
		}
	}
*///
	return E_NOTIMPL;
}

STDMETHODIMP CteShellBrowser::RemoveMenusSB(HMENU hmenuShared)
{
/*//
	for (int nCount = GetMenuItemCount(hmenuShared); nCount-- > 0;) {
		DeleteMenu(hmenuShared, nCount, MF_BYPOSITION);
	}
	return S_OK;
*///
	return E_NOTIMPL;
}

STDMETHODIMP CteShellBrowser::SetStatusTextSB(LPCWSTR lpszStatusText)
{
	g_szStatus[0] = 0x20;
	SetTimer(g_hwndMain, TET_Status, g_dwTickKey && (GetTickCount() - g_dwTickKey < 3000) ? 500 : 100, teTimerProc);
	return S_OK;
}

STDMETHODIMP CteShellBrowser::EnableModelessSB(BOOL fEnable)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteShellBrowser::TranslateAcceleratorSB(LPMSG lpmsg, WORD wID)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteShellBrowser::BrowseObject(PCUIDLIST_RELATIVE pidl, UINT wFlags)
{
	if (ILIsEqual(pidl, g_pidls[CSIDL_RESULTSFOLDER])) {
		return ILIsEqual(m_pidl, pidl) ? S_FALSE : E_FAIL;
	}
	FolderItem *pid = NULL;
	if (wFlags & SBSP_REDIRECT) {
		if (m_pidl) {
			if (m_nLogIndex >= 0 && m_nLogIndex < m_nLogCount && m_ppLog[m_nLogIndex]) {
				m_ppLog[m_nLogIndex]->QueryInterface(IID_PPV_ARGS(&pid));
			} else {
				VARIANT v;
				if (!GetVarPathFromFolderItem(m_pFolderItem, &v)) {
					GetVarPathFromIDList(&v, m_pidl, SHGDN_FORADDRESSBAR | SHGDN_FORPARSING | SHGDN_FORPARSINGEX | SHGDN_ORIGINAL);
				}
				pid = new CteFolderItem(&v);
				VariantClear(&v);
			}
			teILFreeClear(&m_pidl);
			wFlags &= ~(SBSP_RELATIVE | SBSP_REDIRECT);
		} else if (m_pFolderItem) {
			CteFolderItem *pid1 = NULL;
			if SUCCEEDED(m_pFolderItem->QueryInterface(g_ClsIdFI, (LPVOID *)&pid1)) {
				if (pid1->m_v.vt == VT_BSTR) {
					m_pFolderItem->QueryInterface(IID_PPV_ARGS(&pid));
					wFlags &= ~(SBSP_RELATIVE | SBSP_REDIRECT);
				} else if (pid1->m_dwUnavailable || m_dwUnavailable) {
					if (pid1->m_v.vt == VT_EMPTY && pid1->m_pidl) {
						GetVarPathFromFolderItem(m_pFolderItem, &pid1->m_v);
						teILFreeClear(&pid1->m_pidl);
						pid1->QueryInterface(IID_PPV_ARGS(&pid));
						wFlags &= ~(SBSP_RELATIVE | SBSP_REDIRECT);
					}
				}
				pid1->Release();
			}
		}
	}
	if (!pid) {
		GetFolderItemFromIDList(&pid, const_cast<LPITEMIDLIST>(pidl));
	}
	return BrowseObject2(pid, wFlags);
}

HRESULT CteShellBrowser::BrowseObject2(FolderItem *pid, UINT wFlags)
{
	CteShellBrowser *pSB = NULL;
	DWORD param[SB_Count];
	for (int i = SB_Count; i--;) {
		param[i] = m_param[i];
	}
	param[SB_DoFunc] = 1;
	HRESULT hr = Navigate3(pid, wFlags, param, &pSB, NULL);
	SafeRelease(&pSB);
	return hr;
}

HRESULT CteShellBrowser::Navigate3(FolderItem *pFolderItem, UINT wFlags, DWORD *param, CteShellBrowser **ppSB, FolderItems *pFolderItems)
{
	HRESULT hr = E_FAIL;
	try {
		BSTR bs = NULL;
		if (m_pFolderItem && (wFlags & SBSP_NEWBROWSER) && SUCCEEDED(m_pFolderItem->get_Path(&bs))) { 
			if (teStrSameIFree(bs, PATH_BLANK)) {
				wFlags &= ~SBSP_NEWBROWSER;
			}
		}
		if (!(wFlags & SBSP_NEWBROWSER || m_bEmpty)) {
			hr = Navigate2(pFolderItem, wFlags, param, pFolderItems, m_pFolderItem, this);
			if (hr == E_ACCESSDENIED) {
				wFlags = (wFlags | SBSP_NEWBROWSER) & (~SBSP_ACTIVATE_NOFOCUS);
			}
		}
		if (hr != E_ABORT && (wFlags & SBSP_NEWBROWSER || m_bEmpty)) {
			CteShellBrowser *pSB = this;
			BOOL bNew = !m_bEmpty && (wFlags & SBSP_NEWBROWSER);
			if (bNew) {
				pSB = GetNewShellBrowser(m_pTC);
				*ppSB = pSB;
				VariantCopy(&pSB->m_pTV->m_vRoot, &m_pTV->m_vRoot);
			}
			for (int i = SB_Count; i--;) {
				pSB->m_param[i] = param[i];
			}
			pSB->m_bEmpty = FALSE;
			hr = pSB->Navigate2(pFolderItem, wFlags, param, pFolderItems, m_pFolderItem, this);
			if SUCCEEDED(hr) {
				if (bNew && (wFlags & SBSP_ACTIVATE_NOFOCUS) == 0) {
					TabCtrl_SetCurSel(m_pTC->m_hwnd, m_pTC->m_nIndex + 1);
					m_pTC->m_nIndex = TabCtrl_GetCurSel(m_pTC->m_hwnd);
					pSB->Show(TRUE, 0);
				} else {
					m_pTC->TabChanged(TRUE);
				}
			} else {
				pSB->Close(TRUE);
			}
		}
	} catch (...) {
		hr = E_FAIL;
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"Navigate3";
#endif
	}
	SafeRelease(&pFolderItem);
	return hr;
}

BOOL CteShellBrowser::Navigate1(FolderItem *pFolderItem, UINT wFlags, FolderItems *pFolderItems, FolderItem *pPrevious, int nErrorHandling)
{
	BOOL bResult = FALSE;
	CteFolderItem *pid = NULL;
	if (pFolderItem) {
		if SUCCEEDED(pFolderItem->QueryInterface(g_ClsIdFI, (LPVOID *)&pid)) {
			if (!pid->m_pidl && !pid->m_pidlAlt) {
				if (pid->m_v.vt == VT_BSTR) {
					if ((tePathIsNetworkPath(pid->m_v.bstrVal) && tePathIsDirectory(pid->m_v.bstrVal, 100, 3) != S_OK) || (m_pShellView && pid->m_dwUnavailable)) {
						if (m_nUnload || !g_nLockUpdate) {
							Navigate1Ex(pid->m_v.bstrVal, pFolderItems, wFlags, pPrevious, nErrorHandling);
						} else {
							m_nUnload = 9;
						}
						bResult = TRUE;
					}
				}
			}
			pid->Release();
		}
	}
	return bResult;
}

VOID CteShellBrowser::Navigate1Ex(LPOLESTR pstr, FolderItems *pFolderItems, UINT wFlags, FolderItem *pPrevious, int nErrorHandling)
{
	TEInvoke *pInvoke = new TEInvoke[1];
	QueryInterface(IID_PPV_ARGS(&pInvoke->pdisp));
	pInvoke->cRef = 2;
	pInvoke->dispid = 0x20000007;//Navigate2Ex
	pInvoke->cArgs = 4;
	pInvoke->pv = GetNewVARIANT(4);
	pInvoke->wErrorHandling = g_nLockUpdate || m_nUnload == 2 || !g_bShowParseError ? 1 : nErrorHandling;
	pInvoke->wMode = 0;
	pInvoke->pidl = NULL;
	teSetSZ(&pInvoke->pv[3], pstr);
	teSetLong(&pInvoke->pv[2], wFlags);
	teSetObject(&pInvoke->pv[1], pFolderItems);
	teSetObject(&pInvoke->pv[0], pPrevious);
	_beginthread(&threadParseDisplayName, 0, pInvoke);
}

HRESULT CteShellBrowser::Navigate2(FolderItem *pFolderItem, UINT wFlags, DWORD *param, FolderItems *pFolderItems, FolderItem *pPrevious, CteShellBrowser *pHistSB)
{
	RECT rc;
	LPITEMIDLIST pidl = NULL;
	HRESULT hr;

	SafeRelease(&m_pFolderItem1);
	if (!(wFlags & SBSP_RELATIVE)) {
		m_dwUnavailable = 0;
	}
	m_bRefreshLator = FALSE;
	m_nFolderSizeIndex = MAXINT;
	m_nLabelIndex = MAXINT;
	m_nLinkTargetIndex = MAXINT;
	m_bRegenerateItems = FALSE;
	GetNewObject(&m_ppDispatch[SB_TotalFileSize]);
	SafeRelease(&m_ppDispatch[SB_AltSelectedItems]);
	CteFolderItem *pid1 = NULL;
	if (m_hwnd) {
		KillTimer(m_hwnd, (UINT_PTR)this);
	}
	if (m_bInit) {
		return E_FAIL;
	}
	for (int i = SB_Count; i-- > 0;) {
		m_param[i] = param[i];
		g_paramFV[i] = param[i];
	}
	m_nPrevLogIndex = m_nLogIndex;
	SaveFocusedItemToHistory();
	if ((wFlags & (SBSP_PARENT | SBSP_NAVIGATEBACK | SBSP_NAVIGATEFORWARD | SBSP_RELATIVE)) == 0) {
		if (g_pOnFunc[TE_OnReplacePath]) {
			CteFolderItem *pid1;
			if SUCCEEDED(pFolderItem->QueryInterface(g_ClsIdFI, (LPVOID *)&pid1)) {
				if (pid1->m_pidl && pid1->m_v.vt == VT_EMPTY) {
					VARIANT_BOOL bFolder = VARIANT_FALSE;
					pid1->get_IsFolder(&bFolder);
					if (!bFolder) {
						if (::ILIsEqual(pid1->m_pidl, g_pidls[CSIDL_INTERNET])) {
							::ILRemoveLastID(pid1->m_pidl);
						} else if SUCCEEDED(pid1->get_Path(&pid1->m_v.bstrVal)) {
							pid1->m_v.vt = VT_BSTR;
							teILFreeClear(&pid1->m_pidl);
						}
					}
				}
				if (pid1->m_v.vt == VT_BSTR && !m_bsNextFilter) {
					VARIANTARG *pv = GetNewVARIANT(2);
					teSetObject(&pv[1], this);
					teSetSZ(&pv[0], pid1->m_v.bstrVal);
					m_bsNextFilter = ::SysAllocString(L"");
					VARIANT vResult;
					VariantInit(&vResult);
					Invoke4(g_pOnFunc[TE_OnReplacePath], &vResult, 2, pv);
					if (vResult.vt != VT_EMPTY) {
						VariantClear(&pid1->m_v);
						VariantCopy(&pid1->m_v, &vResult);
						VariantClear(&vResult);
					}
				}
				pid1->Release();
			}
		}
	}
	hr = GetAbsPath(pFolderItem, wFlags, pFolderItems, pPrevious, pHistSB);
	if (hr == S_OK) {
		if (!teGetIDListFromObjectForFV(m_pFolderItem1, &pidl)) {
			hr = E_FAIL;
		}
		if SUCCEEDED(m_pFolderItem1->QueryInterface(g_ClsIdFI, (LPVOID *)&pid1)) {
			m_dwUnavailable = pid1->m_dwUnavailable;
			pid1->Release();
		}
	}
	if (!m_dwUnavailable) {
		if SUCCEEDED(pFolderItem->QueryInterface(g_ClsIdFI, (LPVOID *)&pid1)) {
			m_dwUnavailable = pid1->m_dwUnavailable;
			pid1->Release();
		}
	}
	if (m_dwUnavailable) {
		if (m_pShellView) {
			hr = S_FALSE;
		} else if (ILIsEmpty(pidl)) {
			teILCloneReplace(&pidl, g_pidls[CSIDL_RESULTSFOLDER]);
		}
	}
	if (hr != S_OK) {
		if (hr == S_FALSE) {
			if (!m_pFolderItem) {
				if SUCCEEDED(pFolderItem->QueryInterface(IID_PPV_ARGS(&m_pFolderItem))) {
					CteFolderItem *pid1;
					teQueryFolderItem(&m_pFolderItem, &pid1);
					pid1->MakeUnavailable();
					pid1->Release();
				}
			}
		}
		teCoTaskMemFree(pidl);
		return m_pidl ? S_OK : hr;
	}
	hr = S_FALSE;
	if (!pidl) {
		return S_OK;
	}
	IShellFolder2 *pSF2;
	LPCITEMIDLIST pidlPart;
	if SUCCEEDED(SHBindToParent(pidl, IID_PPV_ARGS(&pSF2), &pidlPart)) {
		SFGAOF sfAttr = SFGAO_LINK | SFGAO_FOLDER;
		if SUCCEEDED(pSF2->GetAttributesOf(1, &pidlPart, &sfAttr)) {
			if ((sfAttr & (SFGAO_LINK | SFGAO_FOLDER)) == SFGAO_LINK) {
				VARIANT v;
				VariantInit(&v);
				if (SUCCEEDED(pSF2->GetDetailsEx(pidlPart, &PKEY_Link_TargetParsingPath, &v))) {
					if (v.vt == VT_BSTR) {
						LPITEMIDLIST pidl2 = teILCreateFromPath(v.bstrVal);
						if (pidl2) {
							teILFreeClear(&pidl);
							pidl = pidl2;
							if (m_pFolderItem1) {
								m_pFolderItem1->Release();
								GetFolderItemFromIDList(&m_pFolderItem1, pidl);
							}
						}
					}
					VariantClear(&v);
				}
			}
		}
		pSF2->Release();
	}
	DWORD dwFrame = 0;
	UINT uViewMode = m_param[SB_ViewMode];
#ifdef _2000XP
	if (g_bUpperVista) {
#endif
		if (m_param[SB_Type] == CTRL_EB) {
			dwFrame = EBO_SHOWFRAMES;
		}
		//ExplorerBrowser
		if (m_pExplorerBrowser && !pFolderItems) {
			m_pExplorerBrowser->GetOptions((EXPLORER_BROWSER_OPTIONS *)&m_param[SB_Options]);
			if (!ILIsEqual(pidl, g_pidls[CSIDL_RESULTSFOLDER]) || !ILIsEqual(m_pidl, g_pidls[CSIDL_RESULTSFOLDER])) {
				if (GetShellFolder2(&pidl) == S_OK) {
					IFolderViewOptions *pOptions;
					if SUCCEEDED(m_pExplorerBrowser->QueryInterface(IID_PPV_ARGS(&pOptions))) {
						pOptions->SetFolderViewOptions(FVO_VISTALAYOUT, teGetFolderViewOptions(pidl, uViewMode));
						pOptions->Release();
					}
					m_pExplorerBrowser->SetOptions(static_cast<EXPLORER_BROWSER_OPTIONS>((m_param[SB_Options] & ~(EBO_SHOWFRAMES | EBO_NAVIGATEONCE | EBO_ALWAYSNAVIGATE)) | dwFrame | EBO_NOTRAVELLOG));
					m_pTC->LockUpdate(TRUE);
					try {
						m_pTC->m_nRedraw = TEREDRAW_NAVIGATE | TEREDRAW_NORMAL;
						BrowseToObject();
					} catch (...) {
						g_nException = 0;
#ifdef _DEBUG
						g_strException = L"BrowseToObject";
#endif
					}
					m_pTC->UnlockUpdate();
					teCoTaskMemFree(pidl);
					return S_OK;
				}
			} else {
				m_bBeforeNavigate = TRUE;
				hr = OnNavigationPending2(pidl);
				if SUCCEEDED(hr) {
					RemoveAll();
					SetTabName();
					OnViewCreated(NULL);
					NavigateComplete(TRUE);
					return hr;
				}
			}
		}
#ifdef _2000XP
	}
#endif
	m_clrText = GetSysColor(COLOR_WINDOWTEXT);
	m_clrBk = GetSysColor(COLOR_WINDOW);
	m_clrTextBk = m_clrBk;
	m_bSetListColumnWidth = FALSE;
	teCoTaskMemFree(m_pidl);
	m_pidl = pidl;
	m_pFolderItem = m_pFolderItem1;
	m_pFolderItem1 = NULL;
	if (g_nLockUpdate || !m_pTC->m_bVisible) {
		m_nUnload = 1;
		//History / Management
		SetHistory(pFolderItems, wFlags);
		return S_OK;
	}
	//Previous display
	IShellView *pPreviousView = m_pShellView;
	EXPLORER_BROWSER_OPTIONS dwflag;
	dwflag = static_cast<EXPLORER_BROWSER_OPTIONS>(m_param[SB_Options]);

	m_bIconSize = FALSE;

	if (param[SB_DoFunc]) {
		hr = OnBeforeNavigate(pPrevious, wFlags);
		if (hr == E_ABORT) {
			if (m_pFolderItem == pFolderItem) {
				m_pFolderItem = NULL;
			}
			if (Close(FALSE)) {
				return hr;
			}
		}
		if (hr == E_ACCESSDENIED && teILIsEqual(m_pFolderItem, pPrevious)) {
			hr = S_OK;
		}
		if (!m_pShellView && FAILED(hr)) {
			teILCloneReplace(&m_pidl, g_pidls[CSIDL_RESULTSFOLDER]);
			m_dwUnavailable = GetTickCount();
		}
		if (hr != S_OK && m_nUnload != 2) {
			teILFreeClear(&m_pidl);
			if (pPrevious) {
				teGetIDListFromObject(pPrevious, &m_pidl);
				pPrevious->AddRef();
				SafeRelease(&m_pFolderItem);
				m_pFolderItem = pPrevious;
			}
			m_nLogIndex = m_nPrevLogIndex;
			return hr;
		}
	}
	if (m_bsNextFilter || !teILIsEqual(m_pFolderItem, pPrevious)) {
		InitFilter();
	}
	SetRectEmpty(&rc);
	m_nSorting = ILIsEqual(m_pidl, g_pidls[CSIDL_RESULTSFOLDER]) ? TESORTING : 0;
	//History / Management
	SetHistory(pFolderItems, wFlags);
	m_bBeforeNavigate = TRUE;
	m_pTC->LockUpdate(TRUE);
	try {
		DestroyView(m_param[SB_Type]);
		Show(FALSE, 2);
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"DestroyView1";
#endif
	}
	try {
		m_pTC->m_nRedraw = TEREDRAW_NAVIGATE | TEREDRAW_NORMAL;
#ifdef _2000XP
		hr = g_bUpperVista ? NavigateEB(dwFrame) : NavigateSB(pPreviousView, pPrevious);
#else
		hr =  NavigateEB(dwFrame);
#endif
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"NavigateSB/EB";
#endif
	}
	m_pTC->UnlockUpdate();

	if (m_pTC && GetTabIndex() == m_pTC->m_nIndex) {
		m_nUnload = 0;
		Show(TRUE, 0);
	}
	if (hr != S_OK) {
		SafeRelease(&m_pShellView);
		m_pShellView = pPreviousView;

		teILFreeClear(&m_pidl);
		teGetIDListFromObject(pPrevious, &m_pidl);
		if (pPrevious) {
			pPrevious->AddRef();
			SafeRelease(&m_pFolderItem);
			m_pFolderItem = pPrevious;
		}
		m_nLogIndex = m_nPrevLogIndex;
		return hr;
	}

	if (pPreviousView) {
		pPreviousView->DestroyViewWindow();
		SafeRelease(&pPreviousView);
	}

	m_nOpenedType = m_param[SB_Type];
	if (!m_pExplorerBrowser) {
		OnViewCreated(NULL);
	}
	return S_OK;
}

HRESULT CteShellBrowser::NavigateEB(DWORD dwFrame)
{
	HRESULT hr = E_FAIL;
	RECT rc;
	try {
		if (::InterlockedIncrement(&m_nCreate) <= 1) {
			if (m_pExplorerBrowser) {
				DestroyView(CTRL_SB);
			}
			if (SUCCEEDED(teCreateInstance(CLSID_ExplorerBrowser, NULL, NULL, IID_PPV_ARGS(&m_pExplorerBrowser)))) {
				if (m_nForceViewMode != FVM_AUTO) {
					m_param[SB_ViewMode] = m_nForceViewMode;
					m_bAutoVM = FALSE;
					m_nForceViewMode = FVM_AUTO;
				}
				FOLDERSETTINGS fs = { !m_bAutoVM || ILIsEqual(m_pidl, g_pidls[CSIDL_RESULTSFOLDER]) ? m_param[SB_ViewMode] : FVM_AUTO, (m_param[SB_FolderFlags] | FWF_USESEARCHFOLDER) & ~FWF_NOENUMREFRESH };
				if (SUCCEEDED(m_pExplorerBrowser->Initialize(m_pTC->m_hwndStatic, &rc, &fs))) {
					if (GetShellFolder2(&m_pidl) == S_OK) {
						m_pExplorerBrowser->Advise(static_cast<IExplorerBrowserEvents *>(this), &m_dwEventCookie);
						IFolderViewOptions *pOptions;
						if SUCCEEDED(m_pExplorerBrowser->QueryInterface(IID_PPV_ARGS(&pOptions))) {
							pOptions->SetFolderViewOptions(FVO_VISTALAYOUT, teGetFolderViewOptions(m_pidl, m_param[SB_ViewMode]));
							pOptions->Release();
						}
						m_pExplorerBrowser->SetOptions(static_cast<EXPLORER_BROWSER_OPTIONS>((m_param[SB_Options] & ~(EBO_SHOWFRAMES | EBO_NAVIGATEONCE | EBO_ALWAYSNAVIGATE)) | dwFrame | EBO_NOTRAVELLOG));
						IServiceProvider *pSP = new CteServiceProvider(static_cast<IShellBrowser *>(this), NULL);
						IUnknown_SetSite(m_pExplorerBrowser, pSP);
						pSP->Release();
/*///
						IFolderFilterSite *pFolderFilterSite;
						if SUCCEEDED(m_pExplorerBrowser->QueryInterface(IID_PPV_ARGS(&pFolderFilterSite))) {
							pFolderFilterSite->SetFilter(static_cast<IFolderFilter *>(this));
						}
*///
						hr = BrowseToObject();
					}
					if (hr == S_OK) {
						if (m_pShellView) {
							GetDefaultColumns();
						}
					}
				}
			}
		}
	} catch (...) {
		hr = E_FAIL;
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"NavigateEB";
#endif
	}
	InterlockedDecrement(&m_nCreate);
	return hr;
}

HRESULT CteShellBrowser::OnBeforeNavigate(FolderItem *pPrevious, UINT wFlags)
{
	HRESULT hr = S_OK;
	if (m_pShellView) {
		GetViewModeAndIconSize(TRUE);
		if (m_pExplorerBrowser) {
			m_pExplorerBrowser->GetOptions((EXPLORER_BROWSER_OPTIONS *)&m_param[SB_Options]);
		}
	}
#ifdef _2000XP
	m_bAutoVM = g_bUpperVista && (g_param[TE_Layout] & TE_AutoViewMode);
#else
	m_bAutoVM = (g_param[TE_Layout] & TE_AutoViewMode);
#endif
	if (g_pOnFunc[TE_OnBeforeNavigate]) {
		VARIANT vResult;
		VariantInit(&vResult);

		VARIANTARG *pv;
		pv = GetNewVARIANT(4);
		teSetObject(&pv[3], this);
		teSetObjectRelease(&pv[2], new CteMemory(4 * sizeof(int), &m_param[SB_ViewMode], 1, L"FOLDERSETTINGS"));
		teSetLong(&pv[1], wFlags);
		if (!teILIsEqual(m_pFolderItem, pPrevious)) {
			teSetObject(&pv[0], pPrevious);
		}
		DWORD nViewMode = m_param[SB_ViewMode];
		m_param[SB_ViewMode] = FVM_AUTO;
		m_bInit = TRUE;
		try {
			Invoke4(g_pOnFunc[TE_OnBeforeNavigate], &vResult, 4, pv);
		} catch (...) {
			g_nException = 0;
#ifdef _DEBUG
			g_strException = L"OnBeforeNavigate";
#endif
		}
		m_bInit = FALSE;
		hr = GetIntFromVariantClear(&vResult);
		if (m_param[SB_ViewMode] == FVM_AUTO) {
			m_param[SB_ViewMode] = nViewMode;
		} else {
			m_bAutoVM = FALSE;
		}
	}
	return hr;
}

VOID CteShellBrowser::SaveFocusedItemToHistory()
{
	if (m_nLogIndex >= 0 && m_nLogIndex < m_nLogCount && m_pShellView && m_ppLog[m_nLogIndex]) {
		IFolderView *pFV;
		if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV))) {
			int i;
			if SUCCEEDED(pFV->GetFocusedItem(&i)) {
				CteFolderItem *pid1 = NULL;
				teQueryFolderItem(&m_ppLog[m_nLogIndex], &pid1);
				teILFreeClear(&pid1->m_pidlFocused);
				pFV->Item(i, &pid1->m_pidlFocused);
				pFV->ItemCount(SVGIO_SELECTION, &pid1->m_nSelected);
				pid1->Release();
			}
			pFV->Release();
		}
	}
}

VOID CteShellBrowser::FocusItem(BOOL bFree)
{
	CteFolderItem *pid;
	if (m_pFolderItem && !m_dwUnavailable && SUCCEEDED(m_pFolderItem->QueryInterface(g_ClsIdFI, (LPVOID *)&pid))) {
		if (pid->m_pidlFocused) {
			SelectItemEx(&pid->m_pidlFocused, pid->m_nSelected == 1 ?
				SVSI_FOCUSED | SVSI_ENSUREVISIBLE | SVSI_NOTAKEFOCUS | SVSI_DESELECTOTHERS | SVSI_SELECTIONMARK | SVSI_SELECT :
				SVSI_FOCUSED | SVSI_ENSUREVISIBLE | SVSI_NOTAKEFOCUS | SVSI_DESELECTOTHERS | SVSI_SELECTIONMARK, bFree);
		}
		m_dwUnavailable = pid->m_dwUnavailable;
		pid->Release();
	}
	if (!bFree) {
		int nTop = -1;
		IFolderView *pFV;
		if (m_pShellView && SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV)))) {
			if (SUCCEEDED(pFV->GetFocusedItem(&nTop)) && nTop < 0) {
				if (SUCCEEDED(pFV->ItemCount(SVGIO_ALLVIEW, &nTop)) && nTop) {
					pFV->SelectItem(0, SVSI_FOCUSED | SVSI_ENSUREVISIBLE | SVSI_NOTAKEFOCUS);
				}
			}
			pFV->Release();
		}
		SetTimer(m_hwnd, (UINT_PTR)&m_nFocusItem, 64, teTimerProcFocus);
		m_nFocusItem = 0;
	}
}

HRESULT CteShellBrowser::GetAbsPath(FolderItem *pid, UINT wFlags, FolderItems *pFolderItems, FolderItem *pPrevious, CteShellBrowser *pHistSB)
{
	if (wFlags & SBSP_PARENT) {
		if (pPrevious) {
			if (m_bsFilter || m_ppDispatch[SB_OnIncludeObject]) {
				teSysFreeString(&m_bsFilter);
				SafeRelease(&m_ppDispatch[SB_OnIncludeObject]);
				Refresh(TRUE);
				return E_FAIL;
			}
			if (teILGetParent(pPrevious, &m_pFolderItem1)) {
				if (Navigate1(m_pFolderItem1, wFlags & ~SBSP_PARENT, pFolderItems, pPrevious, 0)) {
					return S_FALSE;
				}
				CteFolderItem *pid1;
				teQueryFolderItem(&m_pFolderItem1, &pid1);
				teGetIDListFromObject(pPrevious, &pid1->m_pidlFocused);
				pid1->m_nSelected = 1;
				SafeRelease(&pid1);
			}
		} else {
			GetFolderItemFromIDList(&m_pFolderItem1, g_pidls[CSIDL_DESKTOP]);
		}
		if (teILIsEqual(pPrevious, m_pFolderItem1)) {
			return E_FAIL;
		}
		return S_OK;
	}
	if (wFlags & SBSP_NAVIGATEBACK) {
		int nLogIndex = pHistSB->m_nLogIndex;
		if (nLogIndex > 0) {
			if (m_pFolderItem) {
				m_ppLog[nLogIndex]->Release();
				m_pFolderItem->QueryInterface(IID_PPV_ARGS(&m_ppLog[nLogIndex]));
				SaveFocusedItemToHistory();
			}
			if SUCCEEDED(pHistSB->m_ppLog[--nLogIndex]->QueryInterface(IID_PPV_ARGS(&m_pFolderItem1))) {
				if (this == pHistSB) {
					m_nLogIndex = nLogIndex;
				}
				return S_OK;
			}
		}
		return E_FAIL;
	}
	if (wFlags & SBSP_NAVIGATEFORWARD) {
		int nLogIndex = pHistSB->m_nLogIndex;
		if (nLogIndex < pHistSB->m_nLogCount - 1) {
			if (m_pFolderItem) {
				m_ppLog[nLogIndex]->Release();
				m_pFolderItem->QueryInterface(IID_PPV_ARGS(&m_ppLog[nLogIndex]));
				SaveFocusedItemToHistory();
			}
			if SUCCEEDED(pHistSB->m_ppLog[++nLogIndex]->QueryInterface(IID_PPV_ARGS(&m_pFolderItem1))) {
				if (this == pHistSB) {
					m_nLogIndex = nLogIndex;
				}
				return S_OK;
			}
		}
		return E_FAIL;
	}
	LPITEMIDLIST pidl = NULL;
	if (Navigate1(pid, wFlags, pFolderItems, pPrevious, 2)) {
		return S_FALSE;
	}
	if (wFlags & SBSP_RELATIVE) {
		LPITEMIDLIST pidlPrevius = NULL;
		teGetIDListFromObjectForFV(pPrevious, &pidlPrevius);
		if (pidlPrevius && !ILIsEqual(pidlPrevius, g_pidls[CSIDL_RESULTSFOLDER]) && !ILIsEmpty(pidl)) {
			LPITEMIDLIST pidlFull = NULL;
			pidlFull = ILCombine(pidlPrevius, pidl);
			GetFolderItemFromIDList(&m_pFolderItem1, pidlFull);
			teCoTaskMemFree(pidlFull);
		} else if (pPrevious) {
			pPrevious->QueryInterface(IID_PPV_ARGS(&m_pFolderItem1));
		} else if (pidlPrevius) {
			GetFolderItemFromIDList(&m_pFolderItem1, pidlPrevius);
		} else {
			VARIANT v;
			teSetSZ(&v, PATH_BLANK);
			CteFolderItem *pid0 = new CteFolderItem(&v);
			VariantClear(&v);
			pid0->MakeUnavailable();
			m_pFolderItem1 = pid0;
		}
		teILFreeClear(&pidlPrevius);
		teILFreeClear(&pidl);
	} else if (pid) {
		pid->QueryInterface(IID_PPV_ARGS(&m_pFolderItem1));
	}
	return m_pFolderItem1 ? S_OK : E_FAIL;
}

VOID CteShellBrowser::Refresh(BOOL bCheck)
{
	m_bRefreshLator = FALSE;
	if (!m_dwUnavailable) {
		teDoCommand(this, m_hwnd, WM_NULL, 0, 0);//Save folder setings
	}
	GetNewObject(&m_ppDispatch[SB_TotalFileSize]);
	SafeRelease(&m_ppDispatch[SB_AltSelectedItems]);
	if (bCheck) {
		VARIANT v;
		VariantInit(&v);
		if SUCCEEDED(m_pFolderItem->QueryInterface(IID_PPV_ARGS(&v.pdispVal))) {
			VARIANT vResult;
			v.vt = VT_DISPATCH;
			VariantInit(&vResult);
			teGetDisplayNameOf(&v, SHGDN_FORADDRESSBAR | SHGDN_FORPARSING, &vResult);
			VariantClear(&v);
			if (vResult.vt == VT_BSTR) {
				if (!m_pShellView || tePathIsNetworkPath(vResult.bstrVal) || m_dwUnavailable) {
					TEInvoke *pInvoke = new TEInvoke[1];
					QueryInterface(IID_PPV_ARGS(&pInvoke->pdisp));
					pInvoke->dispid = 0x20000206;//Refresh2Ex
					pInvoke->cRef = 2;
					pInvoke->cArgs = 1;
					pInvoke->pv = GetNewVARIANT(1);
					pInvoke->wErrorHandling = 1;
					pInvoke->wMode = 0;
					pInvoke->pidl = NULL;
					pInvoke->pv[0].vt = VT_BSTR;
					pInvoke->pv[0].bstrVal = ::SysAllocString(vResult.bstrVal);
					VariantClear(&vResult);
					_beginthread(&threadParseDisplayName, 0, pInvoke);
					return;
				}
			}
			VariantClear(&vResult);
		}
	}
	if (m_pShellView) {
		if (m_bVisible) {
			if (m_nUnload == 4) {
				m_nUnload = 0;
			}
			if (m_dwUnavailable || (g_pidls[CSIDL_LIBRARY] && ILIsParent(g_pidls[CSIDL_LIBRARY], m_pidl, FALSE))) {
				BrowseObject(NULL, SBSP_RELATIVE | SBSP_SAMEBROWSER);
				return;
			}
			IFolderView2 *pFV2;
			if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV2))) {
				pFV2->SetText(FVST_EMPTYTEXT, m_bsFilter || m_ppDispatch[SB_OnIncludeObject] ? L"" : NULL);
				pFV2->Release();
			}
			m_bRefreshing = TRUE;
			if (ILIsEqual(m_pidl, g_pidls[CSIDL_RESULTSFOLDER])) {
				m_bBeforeNavigate = TRUE;
				RemoveAll();
				SetTabName();
				OnBeforeNavigate(m_pFolderItem, SBSP_SAMEBROWSER | SBSP_ABSOLUTE);
				NavigateComplete(TRUE);
			} else {
				m_pShellView->Refresh();
			}
		} else if (m_nUnload == 0) {
			m_nUnload = 4;
		}
	}
}

BOOL CteShellBrowser::SetActive(BOOL bForce)
{
	if (!m_bVisible) {
		return FALSE;
	}
	if (m_hwndAlt) {
		BringWindowToTop(m_hwndAlt);
		SetFocus(m_hwndAlt);
		return TRUE;
	}
	HWND hwnd = GetFocus();
	if (!bForce) {
		if (hwnd) {
			if (IsChild(g_pWebBrowser->m_hwndBrowser, hwnd) || (m_pTV && IsChild(m_pTV->m_hwnd, hwnd))) {
				return FALSE;
			}
			CHAR szClassA[MAX_CLASS_NAME];
			GetClassNameA(hwnd, szClassA, MAX_CLASS_NAME);
			if (lstrcmpA(szClassA, WC_TREEVIEWA) == 0) {
				return FALSE;
			}
		}
	}
	if (m_pShellView) {
		BYTE lpKeyState[256];
		GetKeyboardState(lpKeyState);
		BYTE ksShift = lpKeyState[VK_SHIFT];
		if (ksShift & 0x80) {
			lpKeyState[VK_SHIFT] &= ~0x80;
			SetKeyboardState(lpKeyState);
		}
		if (IsChild(m_hwnd, hwnd)) {
			SetFocus(m_hwnd);
		}
		try {
			m_pShellView->UIActivate(SVUIA_ACTIVATE_FOCUS);
		} catch (...) {
			m_pShellView = NULL;
			Suspend(0);
			g_nException = 0;
#ifdef _DEBUG
		g_strException = L"SetActive";
#endif
		}
		if (ksShift & 0x80 && GetAsyncKeyState(VK_SHIFT) < 0) {
			GetKeyboardState(lpKeyState);
			lpKeyState[VK_SHIFT] = ksShift;
			SetKeyboardState(lpKeyState);
		}
	}
	return TRUE;
}

VOID CteShellBrowser::SetTitle(BSTR szName, int nIndex)
{
	TC_ITEM tcItem;
	BSTR bsText = ::SysAllocStringLen(NULL, MAX_PATH);
	BSTR bsOldText = ::SysAllocStringLen(NULL, MAX_PATH);
	bsOldText[0] = NULL;
	tcItem.pszText = bsOldText;
	tcItem.mask = TCIF_TEXT;
	tcItem.cchTextMax = MAX_PATH;
	int nCount = ::SysStringLen(szName);
	if (nCount >= MAX_PATH) {
		nCount = MAX_PATH - 1;
	}
	int j = 0;
	for (int i = 0; i < nCount; i++) {
		bsText[j++] = szName[i];
		if (szName[i] == '&') {
			bsText[j++] = '&';
		}
	}
	bsText[j] = NULL;
	TabCtrl_GetItem(m_pTC->m_hwnd, nIndex, &tcItem);
	if (lstrcmpi(bsText, bsOldText)) {
		tcItem.pszText = bsText;
		TabCtrl_SetItem(m_pTC->m_hwnd, nIndex, &tcItem);
		ArrangeWindow();
	}
	SysFreeString(bsOldText);
	SysFreeString(bsText);
}

VOID CteShellBrowser::GetShellFolderView()
{
	teUnadviseAndRelease(m_pdisp, DIID_DShellFolderViewEvents, &m_dwCookie);
	if SUCCEEDED(m_pShellView->GetItemObject(SVGIO_BACKGROUND, IID_PPV_ARGS(&m_pdisp))) {
		teAdvise(m_pdisp, DIID_DShellFolderViewEvents, static_cast<IDispatch *>(this), &m_dwCookie);
	} else {
		m_pdisp = NULL;
	}
}

VOID CteShellBrowser::GetFocusedIndex(int *piItem)
{
#ifdef _W2000
	if (g_bIs2000) {
		//Windows 2000
		*piItem = ListView_GetNextItem(m_hwndLV, -1, LVNI_ALL | LVNI_FOCUSED);
		return;
	}
#endif
	*piItem = -1;
	IFolderView *pFV;
	if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV))) {
		pFV->GetFocusedItem(piItem);
		pFV->Release();
	}
}

VOID CteShellBrowser::SetFolderFlags(BOOL bGetIconSize)
{
	if (m_pShellView) {
		IFolderView2 *pFV2;
		if (SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV2)))) {
			DWORD dwFolderFlags = m_param[SB_FolderFlags];
			if (!m_bVisible) {
				dwFolderFlags &= ~FWF_AUTOARRANGE;
			}
			pFV2->SetCurrentFolderFlags(~(FWF_NOENUMREFRESH | FWF_USESEARCHFOLDER), dwFolderFlags);
			pFV2->Release();
#ifdef _2000XP
		} else {
			ListView_SetExtendedListViewStyleEx(m_hwndLV, LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES, HIWORD(m_param[SB_FolderFlags]));
#endif
		}
		GetViewModeAndIconSize(bGetIconSize);
		SetLVSettings();
	}
}

VOID CteShellBrowser::InitFolderSize()
{
	m_nFolderSizeIndex = MAXINT;
	m_nLabelIndex = MAXINT;
	m_nSizeIndex = MAXINT;
	m_nLinkTargetIndex = MAXINT;
	if (m_pDTColumns) {
		delete [] m_pDTColumns;
		m_pDTColumns = NULL;
	}
	if (m_ppColumns) {
		while (m_iColumns > 0) {
			SafeRelease(&m_ppColumns[--m_iColumns]);
		}
		delete [] m_ppColumns;
		m_ppColumns = NULL;
	}
	if (m_hwndLV) {
		HWND hHeader = ListView_GetHeader(m_hwndLV);
		if (hHeader) {
			UINT nHeader = Header_GetItemCount(hHeader);
			teSetDarkTheme(hHeader, L"itemsview");
			m_iColumns = nHeader;
			BSTR bsTotalFileSize = tePSGetNameFromPropertyKeyEx(PKEY_TotalFileSize, 0, m_pShellView);
			BSTR bsLabel = g_pOnFunc[TE_Labels] ? tePSGetNameFromPropertyKeyEx(PKEY_Contact_Label, 0, m_pShellView) : NULL;
			BSTR bsSize = tePSGetNameFromPropertyKeyEx(PKEY_Size, 0, m_pShellView);
			BSTR bsLinkTarget = tePSGetNameFromPropertyKeyEx(PKEY_Link_TargetParsingPath, 0, m_pShellView);
			WCHAR szText[MAX_COLUMN_NAME_LEN];
			HD_ITEM hdi = { HDI_TEXT | HDI_FORMAT, 0, szText, NULL, MAX_COLUMN_NAME_LEN };
			m_pDTColumns = new WORD[nHeader];
			m_ppColumns = (IDispatch **)new PVOID[nHeader];
			TEmethodW *pColumns = new TEmethodW[nHeader + 1];
			VARIANT v;
			VariantInit(&v);
			for (int i = nHeader; i-- > 0;) {
				m_pDTColumns[i] = 0;
				m_ppColumns[i] = NULL;
				Header_GetItem(hHeader, i, &hdi);
				if (g_bDarkMode) {
					hdi.fmt |= HDF_OWNERDRAW;
					Header_SetItem(hHeader, i, &hdi);
				}
				if (szText[0]) {
					pColumns[i].name = ::SysAllocString(szText);
					if (m_ppDispatch[SB_ColumnsReplace]) {
						teGetProperty(m_ppDispatch[SB_ColumnsReplace], szText, &v);
					}
					if (v.vt == VT_EMPTY && g_pOnFunc[TE_ColumnsReplace]) {
						teGetProperty(g_pOnFunc[TE_ColumnsReplace], szText, &v);
					}
					GetDispatch(&v, &m_ppColumns[i]);
					VariantClear(&v);
					if (m_ppColumns[i]) {
						teGetProperty(m_ppColumns[i], L"fmt", &v);
						if (v.vt != VT_EMPTY) {
							int fmt = GetIntFromVariantClear(&v);
							if ((fmt ^ hdi.fmt) & HDF_JUSTIFYMASK) {
								hdi.fmt = (hdi.fmt & ~HDF_JUSTIFYMASK) | fmt;
								Header_SetItem(hHeader, i, &hdi);
								InvalidateRect(m_hwndLV, NULL, FALSE);
							}
						}
						continue;
					}
#ifdef _2000XP
					if (g_bUpperVista) {
#endif
						if (lstrcmpi(szText, bsTotalFileSize) == 0) {
							m_nFolderSizeIndex = i;
							hdi.fmt |= HDF_RIGHT;
							Header_SetItem(hHeader, i, &hdi);
							InvalidateRect(m_hwndLV, NULL, FALSE);
							continue;
						}
						if (lstrcmpi(szText, bsLabel) == 0) {
							m_nLabelIndex = i;
							continue;
						}
						if (lstrcmpi(szText, bsLinkTarget) == 0) {
							m_nLinkTargetIndex = i;
							continue;
						}
#ifdef _2000XP
					}
#endif
					if (lstrcmpi(szText, bsSize) == 0) {
						m_nSizeIndex = i;
					}
				}
			}

			if (g_bsDateTimeFormat && m_pSF2) {
				int *piColumns = SortTEMethodW(pColumns, nHeader);
				int nIndex;
				SHELLDETAILS sd;
				for (UINT i = 0; m_pSF2->GetDetailsOf(NULL, i, &sd) == S_OK && i < MAX_COLUMNS; i++) {
					BSTR bs;
					if SUCCEEDED(StrRetToBSTR(&sd.str, NULL, &bs)) {
						nIndex = teBSearchW(pColumns, nHeader, piColumns, bs);
						if (nIndex >= 0) {
							m_pDTColumns[nIndex] = i;
						}
						::SysFreeString(bs);
					}
				}
				delete [] piColumns;
			}
			for (int i = nHeader; i-- > 0;) {
				SysFreeString(pColumns[i].name);
			}

			teSysFreeString(&bsLinkTarget);
			teSysFreeString(&bsSize);
			teSysFreeString(&bsLabel);
			::SysFreeString(bsTotalFileSize);
			InvalidateRect(m_hwndLV, NULL, FALSE);
		}
	}
}

VOID CteShellBrowser::SetSize(LPCITEMIDLIST pidl, LPWSTR szText, int cch)
{
	if (m_pSF2) {
		VARIANT v;
		VariantInit(&v);
		if (SUCCEEDED(m_pSF2->GetDetailsEx(pidl, &PKEY_Size, &v))) {
			teStrFormatSize(g_param[TE_SizeFormat], GetLLFromVariant(&v), szText, cch);
			VariantClear(&v);
		}
	}
}

static void threadFolderSize(void *args)
{
	::CoInitialize(NULL);
	try {
		TEFS *pFS;
		while (PopFolderSizeList(&pFS)) {
			IDispatch *pTotalFileSize;
			CoGetInterfaceAndReleaseStream(pFS->pStrmTotalFileSize, IID_PPV_ARGS(&pTotalFileSize));
			if (g_bMessageLoop) {
				VARIANT v;
				teSetLL(&v, teGetFolderSize(pFS->bsPath, MAXINT, pFS->pdwSessionId, pFS->dwSessionId));
				if (*pFS->pdwSessionId == pFS->dwSessionId) {
					tePutProperty(pTotalFileSize, pFS->bsPath, &v);
					if (pFS->hwnd) {
						InvalidateRect(pFS->hwnd, NULL, FALSE);
					}
					SetTimer(g_hwndMain, TET_Status, 100, teTimerProc);
				}
				VariantClear(&v);
			}
			SafeRelease(&pTotalFileSize);
			::SysFreeString(pFS->bsPath);
			delete [] pFS;
		}
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"threadFolderSize";
#endif
	}
	g_nCountOfThreadFolderSize--;
	::CoUninitialize();
	::_endthread();
}

VOID CteShellBrowser::SetFolderSize(IShellFolder2 *pSF2, LPCITEMIDLIST pidl, LPWSTR szText, int cch)
{
	if (pSF2) {
		VARIANT v;
		VariantInit(&v);
		WIN32_FIND_DATA wfd;
		teSHGetDataFromIDList(pSF2, pidl, SHGDFIL_FINDDATA, &wfd, sizeof(WIN32_FIND_DATA));
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			BSTR bsPath;
			teGetDisplayNameBSTR(pSF2, pidl, SHGDN_FORPARSING, &bsPath);
			teGetProperty(m_ppDispatch[SB_TotalFileSize], bsPath, &v);
			if (v.vt == VT_BSTR) {
				if (v.bstrVal) {
					lstrcpyn(szText, v.bstrVal, cch);
				}
			} else if (v.vt != VT_EMPTY) {
				teStrFormatSize(g_param[TE_SizeFormat], GetLLFromVariant(&v), szText, cch);
			} else if (m_ppDispatch[SB_TotalFileSize]) {
				v.vt = VT_BSTR;
				v.bstrVal = NULL;
				tePutProperty(m_ppDispatch[SB_TotalFileSize], bsPath, &v);
				TEFS *pFS = new TEFS[1];
				pFS->bsPath = ::SysAllocString(bsPath);
				pFS->pdwSessionId = &m_dwSessionId;
				pFS->dwSessionId = m_dwSessionId;
				pFS->hwnd = m_hwndLV;
				CoMarshalInterThreadInterfaceInStream(IID_IDispatch, m_ppDispatch[SB_TotalFileSize], &pFS->pStrmTotalFileSize);
				PushFolderSizeList(pFS);
				if (g_nCountOfThreadFolderSize < 8) {
					g_nCountOfThreadFolderSize++;
					_beginthread(&threadFolderSize, 0, NULL);
				}
			}
			teSysFreeString(&bsPath);
		} else if (SUCCEEDED(pSF2->GetDetailsEx(pidl, &PKEY_Size, &v))) {
			teStrFormatSize(g_param[TE_SizeFormat], GetLLFromVariant(&v), szText, cch);
		}
		VariantClear(&v);
	}
}

VOID CteShellBrowser::SetLabel(LPCITEMIDLIST pidl, LPWSTR szText, int cch)
{
	IShellFolder2 *pSF2;
	BSTR bs;
	VARIANT v;
	VariantInit(&v);
	if SUCCEEDED(QueryInterface(IID_PPV_ARGS(&pSF2))) {
		if SUCCEEDED(teGetDisplayNameBSTR(pSF2, pidl, SHGDN_FORADDRESSBAR | SHGDN_FORPARSING, &bs)) {
			if (::SysStringLen(bs)) {
				if (g_bLabelsMode) {
					teGetProperty(g_pOnFunc[TE_Labels], bs, &v);
				} else {
					VARIANTARG *pv = GetNewVARIANT(1);
					teSetBSTR(&pv[0], &bs, -1);
					Invoke4(g_pOnFunc[TE_Labels], &v, 1, pv);
				}
				if (v.vt == VT_BSTR) {
					lstrcpyn(szText, v.bstrVal, cch);
				}
				::VariantClear(&v);
			}
			teSysFreeString(&bs);
		}
		pSF2->Release();
	}
}

BOOL CteShellBrowser::ReplaceColumns(IDispatch *pdisp, NMLVDISPINFO *lpDispInfo)
{
	BOOL bResult = FALSE;
	IFolderView *pFV;
	LPITEMIDLIST pidl;
	if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV))) {
		if SUCCEEDED(pFV->Item(lpDispInfo->item.iItem, &pidl)) {
			VARIANTARG *pv = GetNewVARIANT(3);
			teSetObject(&pv[2], this);
			LPITEMIDLIST pidlFull = ILCombine(m_pidl, pidl);
			teSetIDListRelease(&pv[1], &pidlFull);
			teILFreeClear(&pidl);
			teSetSZ(&pv[0], lpDispInfo->item.pszText);
			VARIANT v;
			VariantInit(&v);
			Invoke4(pdisp, &v, 3, pv);
			if (v.vt == VT_BSTR && ::SysStringLen(v.bstrVal)) {
				lstrcpyn(lpDispInfo->item.pszText, v.bstrVal, lpDispInfo->item.cchTextMax);
				bResult = TRUE;
			}
			VariantClear(&v);
		}
		pFV->Release();
	}
	return bResult;
}

VOID CteShellBrowser::GetViewModeAndIconSize(BOOL bGetIconSize)
{
	if (!m_pShellView || (m_dwUnavailable && !m_bCheckLayout)) {
		return;
	}
	FOLDERSETTINGS fs;
	UINT uViewMode = m_param[SB_ViewMode];
	int iImageSize = m_param[SB_IconSize];
	if SUCCEEDED(m_pShellView->GetCurrentInfo(&fs)) {
		uViewMode = fs.ViewMode;
	}
	if (bGetIconSize || uViewMode != m_param[SB_ViewMode]) {
		IFolderView2 *pFV2;
		if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV2))) {
			pFV2->GetViewModeAndIconSize((FOLDERVIEWMODE *)&uViewMode, &iImageSize);
			pFV2->Release();
#ifdef _2000XP
		} else {
			if (uViewMode == FVM_ICON) {
				iImageSize = 32;
			} else if (uViewMode == FVM_TILE) {
				iImageSize = 48;
			} else if (uViewMode == FVM_THUMBNAIL) {
				iImageSize = 96;
				HKEY hKey;
				if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
					DWORD dwSize = sizeof(iImageSize);
					RegQueryValueExA(hKey, "ThumbnailSize", NULL, NULL, (LPBYTE)&iImageSize, &dwSize);
					RegCloseKey(hKey);
				}
			} else {
				iImageSize = 16;
			}
#endif
		}
		m_param[SB_IconSize] = iImageSize;
		m_param[SB_ViewMode] = uViewMode;
		if (m_bCheckLayout) {
			FOLDERVIEWOPTIONS fvo = teGetFolderViewOptions(m_pidl, uViewMode);
			if (m_pExplorerBrowser) {
				IFolderViewOptions *pOptions;
				if SUCCEEDED(m_pExplorerBrowser->QueryInterface(IID_PPV_ARGS(&pOptions))) {
					FOLDERVIEWOPTIONS fvo0 = fvo;
					pOptions->GetFolderViewOptions(&fvo0);
					m_pExplorerBrowser->GetOptions((EXPLORER_BROWSER_OPTIONS *)&m_param[SB_Options]);
					if ((fvo ^ fvo0 & FVO_VISTALAYOUT) || (m_param[SB_Options] & EBO_SHOWFRAMES) != (m_param[SB_Type] == CTRL_EB ? EBO_SHOWFRAMES : 0)) {
						m_bCheckLayout = FALSE;
					}
					pOptions->Release();
				}
			} else if (m_param[SB_Type] == CTRL_EB || fvo == FVO_DEFAULT) {
				m_bCheckLayout = FALSE;
			}
			if (!m_bCheckLayout) {
				m_nForceViewMode = uViewMode;
				SetRedraw(FALSE);
				Suspend(4);
			}
		}
	}
}

VOID CteShellBrowser::GetVariantPath(FolderItem **ppFolderItem, FolderItems **ppFolderItems, VARIANT *pv)
{
	IUnknown *punk;
	if (FindUnknown(pv, &punk)) {
		if SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(ppFolderItems))) {
			VARIANT v;
			GetFolderItemFromFolderItems(ppFolderItem, *ppFolderItems, (Invoke5(*ppFolderItems, DISPID_TE_INDEX, DISPATCH_PROPERTYGET, &v, 0, NULL) == S_OK) ? GetIntFromVariantClear(&v) : 0);
			return;
		}
		if SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(ppFolderItem))) {
			return;
		}
	}
	*ppFolderItem = new CteFolderItem(pv);
}

VOID CteShellBrowser::SetHistory(FolderItems *pFolderItems, UINT wFlags)
{
	if (pFolderItems) {
		int nLogIndex = 0;
		VARIANT v;
		if (Invoke5(pFolderItems, DISPID_TE_INDEX, DISPATCH_PROPERTYGET, &v, 0, NULL) == S_OK) {
			nLogIndex = GetIntFromVariantClear(&v);
		}

		while (--m_nLogCount >= 0) {
			m_ppLog[m_nLogCount]->Release();
		}
		m_nLogCount = 0;
		long nCount = 0;
		pFolderItems->get_Count(&nCount);
		if (nCount > MAX_HISTORY) {
			nCount = MAX_HISTORY;
		}
		v.vt = VT_I4;
		while (--nCount >= 0) {
			v.lVal = nCount;
			pFolderItems->Item(v, &m_ppLog[m_nLogCount++]);
		}
		m_nLogIndex = m_nLogCount - nLogIndex - 1;
		SafeRelease(&pFolderItems);
	} else if ((wFlags & (SBSP_NAVIGATEBACK | SBSP_NAVIGATEFORWARD | SBSP_WRITENOHISTORY)) == 0) {
		if (ILIsEqual(m_pidl, g_pidls[CSIDL_RESULTSFOLDER])) {
			BOOL bInvalid = TRUE;
			CteFolderItem *pid1 = NULL;
			if SUCCEEDED(m_pFolderItem->QueryInterface(g_ClsIdFI, (LPVOID *)&pid1)) {
				if (pid1->m_v.vt == VT_BSTR) {
					bInvalid = FALSE;
				}
				pid1->Release();
			}
			if (bInvalid) {
				return;
			}
		}
		if (m_nLogCount > 0 && teILIsEqual(m_pFolderItem, m_ppLog[m_nLogIndex])) {
			m_ppLog[m_nLogIndex]->Release();
			m_pFolderItem->QueryInterface(IID_PPV_ARGS(&m_ppLog[m_nLogIndex]));
			return;
		}
		while (m_nLogIndex < m_nLogCount - 1 && m_nLogCount > 0) {
			m_ppLog[--m_nLogCount]->Release();
		}
		if (m_nLogCount >= MAX_HISTORY) {
			m_nLogCount = MAX_HISTORY - 1;
			m_ppLog[0]->Release();
			for (int i = 0; i < MAX_HISTORY - 1; i++) {
				m_ppLog[i] = m_ppLog[i + 1];
			}
		}
		m_nLogIndex = m_nLogCount;
		if (!m_pFolderItem && m_pidl) {
			GetFolderItemFromIDList(&m_pFolderItem, m_pidl);
		}
		if (m_pFolderItem) {
			m_pFolderItem->QueryInterface(IID_PPV_ARGS(&m_ppLog[m_nLogCount++]));
		}
	}
}

STDMETHODIMP CteShellBrowser::GetViewStateStream(DWORD grfMode, IStream **ppStrm)
{
//	return SHCreateStreamOnFileEx(L"", STGM_READWRITE | STGM_SHARE_DENY_WRITE, FILE_ATTRIBUTE_NORMAL, TRUE, NULL, ppStrm);
	return E_NOTIMPL;
}

STDMETHODIMP CteShellBrowser::GetControlWindow(UINT id, HWND *lphwnd)
{
	if (lphwnd) {
		*lphwnd = (id == FCW_STATUS) ? m_hwnd : NULL;
		return *lphwnd ? S_OK : E_FAIL;
	}
	return E_POINTER;
}

STDMETHODIMP CteShellBrowser::SendControlMsg(UINT id, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *pret)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteShellBrowser::QueryActiveShellView(IShellView **ppshv)
{
	return m_pShellView ? m_pShellView->QueryInterface(IID_PPV_ARGS(ppshv)) : E_FAIL;
}

STDMETHODIMP CteShellBrowser::OnViewWindowActive(IShellView *ppshv)
{
	if (m_pTC) {
		CheckChangeTabTC(m_pTC->m_hwnd, false);
	}
/*/// For check
	BSTR bsPath;
	if SUCCEEDED(teGetDisplayNameFromIDList(&bsPath, m_pidl, SHGDN_FORPARSING)) {
		SetCurrentDirectory(bsPath);
		::SysFreeString(bsPath);
	}
//*/
	return S_OK;
}

STDMETHODIMP CteShellBrowser::SetToolbarItems(LPTBBUTTONSB lpButtons, UINT nButtons, UINT uFlags)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteShellBrowser::OnDefaultCommand(IShellView *ppshv)
{
	return DoFunc(TE_OnDefaultCommand, this, S_FALSE);
}

STDMETHODIMP CteShellBrowser::OnStateChange(IShellView *ppshv, ULONG uChange)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteShellBrowser::IncludeObject(IShellView *ppshv, LPCITEMIDLIST pidl)
{
	HRESULT hr = S_OK;
	if (!(m_param[SB_ViewFlags] & CDB2GVF_SHOWALLFILES)) {
		WIN32_FIND_DATA wfd;
		if SUCCEEDED(SHGetDataFromIDList(m_pSF2, pidl, SHGDFIL_FINDDATA, &wfd, sizeof(WIN32_FIND_DATA))) {
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) {
				return S_FALSE;
			}
		}
	}
	if (m_ppDispatch[SB_OnIncludeObject]) {
		VARIANT vResult;
		VariantInit(&vResult);
		VARIANTARG *pv = GetNewVARIANT(4);
		teSetObject(&pv[3], this);
		if SUCCEEDED(teGetDisplayNameBSTR(m_pSF2, pidl, SHGDN_NORMAL, &pv[2].bstrVal)) {
			pv[2].vt = VT_BSTR;
		}
		if SUCCEEDED(teGetDisplayNameBSTR(m_pSF2, pidl, SHGDN_INFOLDER | SHGDN_FORPARSING, &pv[1].bstrVal)) {
			pv[1].vt = VT_BSTR;
		}
		LPITEMIDLIST pidlFull = ILCombine(m_pidl, pidl);
		if (pidlFull) {
			teSetIDListRelease(&pv[0], &pidlFull);
		}
		Invoke4(m_ppDispatch[SB_OnIncludeObject], &vResult, 4, pv);
		hr = GetIntFromVariantClear(&vResult);
	} else if (m_bsFilter) {
		if (m_pSF2) {
			BSTR bs;
			if SUCCEEDED(teGetDisplayNameBSTR(m_pSF2, pidl, SHGDN_NORMAL, &bs)) {
				if (!tePathMatchSpec(bs, m_bsFilter)) {
					hr = S_FALSE;
					BSTR bs2;
					if SUCCEEDED(teGetDisplayNameBSTR(m_pSF2, pidl, SHGDN_INFOLDER | SHGDN_FORPARSING, &bs2)) {
						if (lstrcmpi(bs, bs2) && tePathMatchSpec(bs2, m_bsFilter)) {
							hr = S_OK;
						}
						::SysFreeString(bs2);
					}
				}
				::SysFreeString(bs);
			}
		}
	}
	if (hr == S_OK) {
		SetTimer(g_hwndMain, TET_Status, 500, teTimerProc);
	}
	return hr;
}
//ICommDlgBrowser2
STDMETHODIMP CteShellBrowser::Notify(IShellView *ppshv, DWORD dwNotifyType)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteShellBrowser::GetDefaultMenuText(IShellView *ppshv, LPWSTR pszText, int cchMax)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteShellBrowser::GetViewFlags(DWORD *pdwFlags)
{
//	*pdwFlags = (m_param[SB_ViewFlags] & (~(CDB2GVF_NOINCLUDEITEM | CDB2GVF_NOSELECTVERB))) | (((m_bsFilter || m_ppDispatch[SB_OnIncludeObject]) && !ILIsEqual(m_pidl, g_pidls[CSIDL_RESULTSFOLDER])) ? 0 : CDB2GVF_NOINCLUDEITEM);
	*pdwFlags = (m_param[SB_ViewFlags] & (~(CDB2GVF_NOINCLUDEITEM | CDB2GVF_NOSELECTVERB)));
	return S_OK;
}
/*/// ICommDlgBrowser3
STDMETHODIMP CteShellBrowser::OnColumnClicked(IShellView *ppshv, int iColumn)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteShellBrowser::GetCurrentFilter(LPWSTR pszFileSpec, int cchFileSpec)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteShellBrowser::OnPreViewCreated(IShellView *ppshv)
{
	return S_OK;
}
//*/
/*/// IFolderFilter
STDMETHODIMP CteShellBrowser::ShouldShow(IShellFolder *psf, PCIDLIST_ABSOLUTE pidlFolder, PCUITEMID_CHILD pidlItem)
{
	return IncludeObject(NULL, pidlItem);
}

STDMETHODIMP CteShellBrowser::GetEnumFlags(IShellFolder *psf, PCIDLIST_ABSOLUTE pidlFolder, HWND *phwnd, DWORD *pgrfFlags)
{
	return S_OK;
}
//*/

STDMETHODIMP CteShellBrowser::GetTypeInfoCount(UINT *pctinfo)
{
	*pctinfo = 0;
	return S_OK;
}

STDMETHODIMP CteShellBrowser::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteShellBrowser::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	HRESULT hr = teGetDispId(methodSB, _countof(methodSB), g_maps[MAP_SB], *rgszNames, rgDispId, TRUE);
	if (hr != S_OK && m_pdisp) {
		return m_pdisp->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgDispId);
	}
	return hr;
}

VOID CteShellBrowser::SetColumnsStr(BSTR bsColumns)
{
	if (m_dwUnavailable) {
		return;
	}
	int nAllWidth = 0;
	int nArgs = 0;
	LPTSTR *lplpszArgs = NULL;
	if (bsColumns && bsColumns[0]) {
		lplpszArgs = CommandLineToArgvW(bsColumns, &nArgs);
		nArgs /= 2;
	}
	int nAlloc = nArgs >= (int)m_nDefultColumns ? nArgs + 1 : m_nDefultColumns;
	PROPERTYKEY *pPropKey0 = new PROPERTYKEY[nAlloc];	
	PROPERTYKEY *pPropKey = &pPropKey0[1];
	TEmethodW *methodArgs0 = new TEmethodW[nAlloc];
	TEmethodW *methodArgs = &methodArgs0[1];
#ifdef _2000XP
	for (int i = nArgs; i-- > 0;) {
		methodArgs[i].name = NULL;
	}
#endif
	BOOL bNoName = TRUE;
	int nCount = 0;
	for (int i = 0; i < nArgs; i++) {
		if SUCCEEDED(PropertyKeyFromName(lplpszArgs[i * 2], &pPropKey[nCount])) {
			BOOL bExist = FALSE;
			for (int j = nCount; j-- > 0;) {
				if (IsEqualPropertyKey(pPropKey[nCount], pPropKey[j])) {
					bExist = TRUE;
					break;
				}
			}
			if (bExist) {
				continue;
			}
#ifdef _2000XP
			if (!g_bUpperVista) {
				methodArgs[i].name = tePSGetNameFromPropertyKeyEx(pPropKey[nCount], 0, m_pShellView);
			}
#endif
			if (IsEqualPropertyKey(pPropKey[nCount], PKEY_ItemNameDisplay)) {
				bNoName = FALSE;
			}
			int n;
			if (StrToIntEx(lplpszArgs[i * 2 + 1], STIF_DEFAULT, &n) && n < 32768) {
				nAllWidth += n;
			} else {
				n = -1;
			}
			if (n < 0) {
				nAllWidth += 100;
			}
			methodArgs[nCount++].id = n;
		}
	}
	if (bNoName) {
		pPropKey = pPropKey0;
		methodArgs = methodArgs0;
		if (nCount) {
			pPropKey[0] = PKEY_ItemNameDisplay;
#ifdef _2000XP
			if (!g_bUpperVista) {
				methodArgs[0].name = tePSGetNameFromPropertyKeyEx(PKEY_ItemNameDisplay, 0, m_pShellView);
			}
#endif
			methodArgs[0].id = -1;
			nCount++;
			nAllWidth += 100;
		}
	}
	if (nCount == 0 || nAllWidth > nCount + 4) {
		IColumnManager *pColumnManager;
		if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pColumnManager))) {
			//Default Columns
			if (nCount == 0) {
				pPropKey = pPropKey0;
				methodArgs = methodArgs0;
				nCount = m_nDefultColumns;
				for (int i = nCount; i--;) {
					pPropKey[i] = m_pDefultColumns[i];
					methodArgs[i].id = -1;
				}
			}
			if SUCCEEDED(pColumnManager->SetColumns(pPropKey, nCount)) {
				CM_COLUMNINFO cmci = { sizeof(CM_COLUMNINFO), CM_MASK_WIDTH };
				for (int i = nCount; i--;) {
					cmci.uWidth = methodArgs[i].id;
					pColumnManager->SetColumnInfo(pPropKey[i], &cmci);
				}
			}
			pColumnManager->Release();
#ifdef _2000XP
		} else {
			UINT uCount = m_nColumns;
			SHELLDETAILS sd;
			int nWidth;
			if (m_pSF2) {
				for (int i = uCount; i-- > 0;) {
					m_pSF2->GetDefaultColumnState(i, &m_pColumns[i].csFlags);
					m_pColumns[i].csFlags &= ~SHCOLSTATE_ONBYDEFAULT;
					m_pColumns[i].nWidth = -3;
				}
				m_pColumns[uCount - 1].csFlags = SHCOLSTATE_TYPE_STR;
				m_pColumns[uCount - 2].csFlags = SHCOLSTATE_TYPE_STR;
				m_pColumns[0].csFlags |= SHCOLSTATE_ONBYDEFAULT;
				int *piArgs = SortTEMethodW(methodArgs, nCount);
				BSTR bs = GetColumnsStr(FALSE);
				if (bs && bs[0]) {
					int nCur;
					LPTSTR *lplpszColumns = CommandLineToArgvW(bs, &nCur);
					::SysFreeString(bs);
					nCur /= 2;
					BOOL bDiff = nCount != nCur || nCur == 0;
					if (!bDiff) {
						TEmethodW *methodColumns = new TEmethodW[nCur];
						for (int i = nCur; i-- > 0;) {
							methodColumns[i].name = lplpszColumns[i * 2];
						}
						int *piCur = SortTEMethodW(methodColumns, nCur);
						for (int i = nCur; i-- > 0;) {
							if (lstrcmpi(methodArgs[piArgs[i]].name, methodColumns[piCur[i]].name)) {
								bDiff = TRUE;
								break;
							}
						}
						delete [] piCur;
						delete [] methodColumns;
					}
					LocalFree(lplpszColumns);

					if (ILIsEqual(m_pidl, g_pidls[CSIDL_RESULTSFOLDER])) {
						IFolderView *pFV;
						if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV))) {
							int i = 0;
							pFV->ItemCount(SVGIO_ALLVIEW, &i);
							if (i) {
								bDiff = FALSE;
							}
						}
					}
					if (bDiff) {
						int nIndex;
						BOOL bDefault = TRUE;
						if (nCount) {
							for (UINT i = 0; i < m_nColumns && GetDetailsOf(NULL, i, &sd) == S_OK; i++) {
								BSTR bs;
								if SUCCEEDED(StrRetToBSTR(&sd.str, NULL, &bs)) {
									nIndex = teBSearchW(methodArgs, nCount, piArgs, bs);
									if (nIndex >= 0) {
										m_pColumns[i].csFlags |= SHCOLSTATE_ONBYDEFAULT;
										m_pColumns[i].nWidth = methodArgs[nIndex].id;
										bDefault = false;
									}
									::SysFreeString(bs);
								}
							}
						}
						if (bDefault) {
							for (int i = uCount; i-- > 0;) {
								m_pSF2->GetDefaultColumnState(i, &m_pColumns[i].csFlags);
								m_pColumns[i].nWidth = -3;
							}
						}
						IShellView *pPreviousView = m_pShellView;
						FOLDERSETTINGS fs;
						pPreviousView->GetCurrentInfo(&fs);
						m_param[SB_ViewMode] = fs.ViewMode;
						Show(FALSE, 0);
						ResetPropEx();
						if (CreateViewWindowEx(pPreviousView) == S_OK) {
							pPreviousView->DestroyViewWindow();
							teDelayRelease(&pPreviousView);
							GetShellFolderView();
						}
						Show(TRUE, 0);
						SetPropEx();
					}
					//Columns Order and Width;
					if (m_hwndLV && ListView_GetView(m_hwndLV) == LV_VIEW_DETAILS) {
						HWND hHeader = ListView_GetHeader(m_hwndLV);
						if (hHeader) {
							UINT nHeader = Header_GetItemCount(hHeader);
							if (nHeader == nCount) {
								SetRedraw(false);
								int *piOrderArray = new int[nHeader];
								try {
									for (int i = nHeader; i-- > 0;) {
										piOrderArray[i] = i;
									}
									WCHAR szText[MAX_COLUMN_NAME_LEN];
									HD_ITEM hdi = { HDI_TEXT | HDI_WIDTH, 0, szText, NULL, MAX_COLUMN_NAME_LEN };
									int nIndex;
									for (int i = nHeader; i-- > 0;) {
										Header_GetItem(hHeader, i, &hdi);
										nIndex = teBSearchW(methodArgs, nCount, piArgs, szText);
										if (nIndex >= 0) {
											nWidth = methodArgs[nIndex].id;
											piOrderArray[nIndex] = i;
											if (nWidth != hdi.cxy) {
												if (nWidth == -2) {
													nWidth = LVSCW_AUTOSIZE;
												} else if (nWidth < 0) {
													int cxChar;
													int i = PSGetColumnIndexXP(szText, &cxChar);
													if (i >= 0) {
														nWidth = cxChar * g_nCharWidth;
													}
												}
												ListView_SetColumnWidth(m_hwndLV, i, nWidth);
											}
										}
									}
									ListView_SetColumnOrderArray(m_hwndLV, nHeader, piOrderArray);
								} catch (...) {
									g_nException = 0;
#ifdef _DEBUG
									g_strException = L"SetColumnsStr";
#endif
								}
								delete [] piOrderArray;
								SetRedraw(TRUE);
							}
						}
					}
				}
				delete [] piArgs;
				InitFolderSize();
			}
#endif
		}
	}
	if (lplpszArgs) {
		LocalFree(lplpszArgs);
	}
#ifdef _2000XP
	if (!g_bUpperVista) {
		for (int i = nCount; i-- > 0;) {
			teSysFreeString(&methodArgs[i].name);
		}
	}
#endif
	delete [] methodArgs0;
	delete [] pPropKey0;
}

VOID AddColumnData(LPWSTR pszColumns, LPWSTR pszName, int nWidth)
{
	swprintf_s(&pszColumns[lstrlen(pszColumns)], MAX_COLUMN_NAME_LEN + 20, L" \"%s\" %d", pszName, nWidth);
}

VOID AddColumnDataEx(LPWSTR pszColumns, BSTR bsName, int nWidth)
{
	AddColumnData(pszColumns, bsName, nWidth);
	teSysFreeString(&bsName);
}

BSTR CteShellBrowser::GetColumnsStr(int nFormat)
{
	if (m_dwUnavailable || m_nDefultColumns == 0) {
		return NULL;
	}
	WCHAR szColumns[SIZE_BUFF];
	szColumns[0] = 0;
	szColumns[1] = 0;

	IColumnManager *pColumnManager;
	if (m_pShellView && SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pColumnManager)))) {
		UINT uCount;
		if SUCCEEDED(pColumnManager->GetColumnCount(CM_ENUM_VISIBLE, &uCount)) {
			if (uCount) {
				PROPERTYKEY *pPropKey = new PROPERTYKEY[uCount];
				if SUCCEEDED(pColumnManager->GetColumns(CM_ENUM_VISIBLE, pPropKey, uCount)) {
					CM_COLUMNINFO cmci = { sizeof(CM_COLUMNINFO), CM_MASK_WIDTH | CM_MASK_NAME };
					for (UINT i = 0; i < uCount; i++) {
						if SUCCEEDED(pColumnManager->GetColumnInfo(pPropKey[i], &cmci)) {
							if (nFormat) {
								AddColumnDataEx(szColumns, tePSGetNameFromPropertyKeyEx(pPropKey[i], nFormat, NULL), cmci.uWidth);
							} else {
								AddColumnData(szColumns, cmci.wszName, cmci.uWidth);
							}
						}
					}
				}
				delete [] pPropKey;
			}
		}
		pColumnManager->Release();
#ifdef _2000XP
	} else {
		HWND hHeader = ListView_GetHeader(m_hwndLV);
		if (hHeader) {
			UINT nHeader = Header_GetItemCount(hHeader);
			if (nHeader) {
				int *piOrderArray = new int[nHeader];
				ListView_GetColumnOrderArray(m_hwndLV, nHeader, piOrderArray);
				WCHAR szText[MAX_COLUMN_NAME_LEN];
				HD_ITEM hdi = { HDI_TEXT | HDI_WIDTH, 0, szText, NULL, MAX_COLUMN_NAME_LEN };
				for (UINT i = 0; i < nHeader; i++) {
					Header_GetItem(hHeader, piOrderArray[i], &hdi);
					AddColumnDataXP(szColumns, szText, hdi.cxy, nFormat);
				}
				delete [] piOrderArray;
			}
		} else {
			SHCOLSTATEF csFlags;
			SHELLDETAILS sd;
			BSTR bs;
			for (UINT i = 0; i < m_nColumns && GetDefaultColumnState(i, &csFlags) == S_OK; i++) {
				if (csFlags & SHCOLSTATE_ONBYDEFAULT) {
					if (GetDetailsOf(NULL, i, &sd) == S_OK) {
						if SUCCEEDED(StrRetToBSTR(&sd.str, NULL, &bs)) {
							int nWidth = m_pColumns[i].nWidth;
							if (nWidth < 0 || nWidth >= 32768) {
								nWidth = sd.cxChar * g_nCharWidth;
							}
							AddColumnDataXP(szColumns, bs, nWidth, nFormat);
							::SysFreeString(bs);
						}
					}
				}
			}
		}
#endif
	}
	return SysAllocString(&szColumns[1]);
}

VOID CteShellBrowser::GetDefaultColumns()
{
	if (m_pDefultColumns) {
		delete [] m_pDefultColumns;
		m_pDefultColumns = NULL;
	}
	m_nDefultColumns = 0;
	IColumnManager *pColumnManager;
	if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pColumnManager))) {
		if SUCCEEDED(pColumnManager->GetColumnCount(CM_ENUM_VISIBLE, &m_nDefultColumns)) {
			m_pDefultColumns = new PROPERTYKEY[m_nDefultColumns];
			pColumnManager->GetColumns(CM_ENUM_VISIBLE, m_pDefultColumns, m_nDefultColumns);
		}
		pColumnManager->Release();
#ifdef _2000XP
	} else {
		m_nDefultColumns = -1;
#endif
	}
}

STDMETHODIMP CteShellBrowser::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	try {
		int nIndex;
		int nFormat = 0;
		TC_ITEM tcItem;
		int nArg = pDispParams ? pDispParams->cArgs - 1 : -1;
		if (pVarResult) {
			VariantInit(pVarResult);
		}
		if ((m_bEmpty || m_nUnload == 1 || (m_nUnload & 8)) && dispIdMember >= 0x10000100 && dispIdMember) {
			return S_OK;
		}
		if (!m_pFolderItem && dispIdMember >= DISPID_SELECTIONCHANGED && dispIdMember <= DISPID_INITIALENUMERATIONDONE) {
			return  S_OK;
		}
		switch(dispIdMember) {
			//Data
			case 0x10000001:
				if (nArg >= 0) {
					VariantClear(&m_vData);
					VariantCopy(&m_vData, &pDispParams->rgvarg[nArg]);
				}
				if (pVarResult) {
					VariantCopy(pVarResult, &m_vData);
				}
				return S_OK;
			//hwnd
			case 0x10000002:
				teSetPtr(pVarResult, m_hwnd);
				return S_OK;
			//Type
			case 0x10000003:
				if (nArg >= 0) {
					DWORD dwType = GetIntFromVariant(&pDispParams->rgvarg[nArg]);
					if (dwType == CTRL_SB || dwType == CTRL_EB) {
						if (m_param[SB_Type] != dwType) {
							m_param[SB_Type] = dwType;
							if (!m_bEmpty) {
								m_bCheckLayout = TRUE;
								GetViewModeAndIconSize(TRUE);
							}
						}
					}
				}
				teSetLong(pVarResult, m_param[SB_Type]);
				return S_OK;

			//Navigate
			case 0x10000004:
			//History
			case 0x1000000B:
				if (nArg >= 0) {
					UINT wFlags = 0;
					FolderItem *pFolderItem = NULL;
					FolderItems *pFolderItems = NULL;
					GetVariantPath(&pFolderItem, &pFolderItems, &pDispParams->rgvarg[nArg]);
					if (nArg >= 1) {
						wFlags = GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]);
					}
					CteShellBrowser *pSB = NULL;

					DWORD param[SB_Count];
					for (int i = SB_Count; i--;) {
						param[i] = m_param[i];
					}
					param[SB_DoFunc] = 1;
					if (pFolderItem || wFlags & (SBSP_PARENT | SBSP_NAVIGATEBACK | SBSP_NAVIGATEFORWARD | SBSP_RELATIVE)) {
						Navigate3(pFolderItem, wFlags, param, &pSB, pFolderItems);
					}

					if (pVarResult) {
						//Navigate
						if (dispIdMember == 0x10000004) {
							teSetObject(pVarResult, pSB ? pSB : this);
						} else if (pSB) {
							pSB->Release();
						}
					}
				}
				//History
				if (dispIdMember == 0x1000000B) {
					if (pVarResult) {
						CteFolderItems *pFolderItems = new CteFolderItems(NULL, NULL);
						VARIANT v;
						for (int i = 1; i <= m_nLogCount; i++) {
							try {
								teSetObject(&v, m_ppLog[m_nLogCount - i]);
								pFolderItems->ItemEx(-1, NULL, &v);
								VariantClear(&v);
							} catch (...) {
								g_nException = 0;
#ifdef _DEBUG
								g_strException = L"History";
#endif
							}
						}
						pFolderItems->m_nIndex = m_nLogCount - 1 - m_nLogIndex;
						teSetObjectRelease(pVarResult, pFolderItems);
					}
				}
				return S_OK;
			//Navigate2Ex
			case 0x20000007:
				if (nArg >= 3) {
					FolderItem *pFolderItem = NULL;
					IUnknown *punk;
					if (FindUnknown(&pDispParams->rgvarg[nArg], &punk)) {
						punk->QueryInterface(IID_PPV_ARGS(&pFolderItem));
					}
					if (!pFolderItem && !m_pShellView) {
						GetFolderItemFromIDList(&pFolderItem, g_pidls[CSIDL_DESKTOP]);
					}
					if (pFolderItem) {
						DWORD param[SB_Count];
						for (int i = SB_Count; i--;) {
							param[i] = m_param[i];
						}
						FolderItems *pFolderItems = NULL;
						if (FindUnknown(&pDispParams->rgvarg[nArg - 2], &punk)) {
							punk->QueryInterface(IID_PPV_ARGS(&pFolderItems));
						}
						LPITEMIDLIST pidl = NULL;
						if (FindUnknown(&pDispParams->rgvarg[nArg - 3], &punk)) {
							teGetIDListFromObjectForFV(punk, &pidl);
						}
						if (SUCCEEDED(Navigate2(pFolderItem, GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]), param, pFolderItems, m_pFolderItem, this)) && m_bVisible) {
							Show(TRUE, 0);
						}
					}
				}
				return S_OK;
			//Navigate2
			case 0x10000007:
				if (nArg >= 0) {
					DWORD param[SB_Count];
					for (int i = SB_Count; i--;) {
						m_param[i] = g_paramFV[i];
						param[i] = g_paramFV[i];
					}
#ifdef _2000XP
					if (!g_bUpperVista && m_param[SB_IconSize] >= 96 && m_param[SB_ViewMode] == FVM_ICON) {
						m_param[SB_ViewMode] = FVM_THUMBNAIL;
					}
#endif
					FolderItem *pFolderItem = NULL;
					FolderItems *pFolderItems = NULL;
					GetVariantPath(&pFolderItem, &pFolderItems, &pDispParams->rgvarg[nArg]);
					if (nArg >= 1) {
						wFlags = static_cast<WORD>(GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]));
					}
					for (int i = 0; i <= nArg - 2 && i < SB_DoFunc; i++) {
						int n = GetIntFromVariant(&pDispParams->rgvarg[nArg - i - 2]);
						if (i == SB_TreeAlign && n == 0) {
							break;
						}
						param[i] = n;
					}
					if (param[SB_Type] != CTRL_SB && param[SB_Type] != CTRL_EB) {
						param[SB_Type] = CTRL_SB;
					}
					param[SB_DoFunc] = 0;
					CteShellBrowser *pSB = NULL;
					Navigate3(pFolderItem, wFlags, param, &pSB, pFolderItems);
					if (!pSB) {
						pSB = this;
					}
					if (nArg >= SB_Root + 2) {
						pSB->m_pTV->SetRootV(&pDispParams->rgvarg[nArg - 2 - SB_Root]);
					}
					teSetObject(pVarResult, pSB);
				}
				return S_OK;
			//Index
			case 0x10000008:
				if (nArg >= 0) {
					m_pTC->Move(GetTabIndex(), GetIntFromVariant(&pDispParams->rgvarg[nArg]), m_pTC);
				}
				if (pVarResult) {
					teSetLong(pVarResult, GetTabIndex());
				}
				return S_OK;
			//FolderFlags
			case 0x10000009:
				if (nArg >= 0) {
					m_param[SB_FolderFlags] = GetIntFromVariant(&pDispParams->rgvarg[nArg]);
					SetFolderFlags(FALSE);
				}
				teSetLong(pVarResult, m_param[SB_FolderFlags]);
				return S_OK;
			//CurrentViewMode
			case 0x10000010:
				if (nArg >= 0) {
					m_param[SB_ViewMode] = GetIntFromVariant(&pDispParams->rgvarg[nArg]);
					if (nArg >= 1) {
						m_param[SB_IconSize] = GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]);
					}
					SetViewModeAndIconSize(nArg >= 1);
				}
				SetFolderFlags(FALSE);
				teSetLong(pVarResult, m_param[SB_ViewMode]);
				return S_OK;
			//IconSize
			case 0x10000011:
				if (m_pShellView && m_bIconSize) {
					GetViewModeAndIconSize(TRUE);
					if (nArg >= 0) {
						int iImageSize = GetIntFromVariant(&pDispParams->rgvarg[nArg]);
						IFolderView2 *pFV2;
						if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV2))) {
							if (iImageSize != m_param[SB_IconSize]) {
								pFV2->SetViewModeAndIconSize(static_cast<FOLDERVIEWMODE>(m_param[SB_ViewMode]), iImageSize);
							}
							pFV2->Release();
							GetViewModeAndIconSize(TRUE);
#ifdef _2000XP
						} else {
							m_param[SB_IconSize] = iImageSize;
#endif
						}
					}
				}
				teSetLong(pVarResult, m_param[SB_IconSize]);
				return S_OK;
			//Options
			case 0x10000012:
				if (nArg >= 0) {
					m_param[SB_Options] = GetIntFromVariant(&pDispParams->rgvarg[nArg]);
					if (m_pExplorerBrowser) {
						m_pExplorerBrowser->SetOptions(static_cast<EXPLORER_BROWSER_OPTIONS>((m_param[SB_Options] & ~(EBO_NAVIGATEONCE | EBO_ALWAYSNAVIGATE)) | EBO_NOTRAVELLOG));
						GetViewModeAndIconSize(FALSE);
					}
				}
				if (pVarResult) {
					if (m_pExplorerBrowser) {
						m_pExplorerBrowser->GetOptions((EXPLORER_BROWSER_OPTIONS *)&m_param[SB_Options]);
					}
					teSetLong(pVarResult, m_param[SB_Options]);
				}
				return S_OK;
			//SizeFormat //Deprecated
			case 0x10000013:
				return S_OK;
			//NameFormat //Deprecated
			case 0x10000014:
				return S_OK;
			//ViewFlags
			case 0x10000016:
				if (nArg >= 0) {
					m_param[SB_ViewFlags] = GetIntFromVariant(&pDispParams->rgvarg[nArg]);
				}
				teSetLong(pVarResult, m_param[SB_ViewFlags]);
				return S_OK;
			//Id
			case 0x10000017:
				teSetLong(pVarResult, m_nSB);
				return S_OK;
			//FilterView
			case 0x10000018:
				if (nArg >= 0) {
					VARIANT v;
					teVariantChangeType(&v, &pDispParams->rgvarg[nArg], VT_BSTR);
					if (wFlags & DISPATCH_METHOD) {
						if (m_pdisp) {
							IShellFolderViewDual3 *pSFVD3;
							if SUCCEEDED(m_pdisp->QueryInterface(IID_PPV_ARGS(&pSFVD3))) {
								pSFVD3->FilterView(v.bstrVal);
								pSFVD3->Release();
#ifdef _2000XP
							} else {
								teSysFreeString(&m_bsFilter);
								if (lstrlen(v.bstrVal)) {
									if (StrChr(v.bstrVal, '*')) {
										m_bsFilter = ::SysAllocString(v.bstrVal);
									} else {
										int nLen = ::SysStringLen(v.bstrVal);
										m_bsFilter = ::SysAllocStringLen(NULL, nLen + 2);
										lstrcpy(&m_bsFilter[1], v.bstrVal);
										m_bsFilter[0] = m_bsFilter[nLen + 1] = '*';
										m_bsFilter[nLen + 2] = NULL;
									}
								}
								Refresh(TRUE);
#endif
							}
						}
					} else if (m_bsNextFilter && lstrlen(v.bstrVal)) {
						teSysFreeString(&m_bsNextFilter);
						m_bsNextFilter = ::SysAllocString(v.bstrVal);
					} else if (lstrcmpi(m_bsFilter, v.bstrVal)) {
						teSysFreeString(&m_bsFilter);
						if (lstrlen(v.bstrVal)) {
							m_bsFilter = ::SysAllocString(v.bstrVal);
						}
						SafeRelease(&m_ppDispatch[SB_OnIncludeObject]);
						DoFunc(TE_OnFilterChanged, this, S_OK);
					}
					VariantClear(&v);
				}
				teSetSZ(pVarResult, m_bsFilter ? m_bsFilter : m_bsNextFilter);
				return S_OK;
			//FolderItem
			case 0x10000020:
				teSetObject(pVarResult, m_pFolderItem);
				return S_OK;
			//TreeView
			case 0x10000021:
				teSetObject(pVarResult, m_pTV);
				return S_OK;
			//Parent
			case 0x10000024:
				IDispatch *pdisp;
				if SUCCEEDED(get_Parent(&pdisp)) {
					teSetObjectRelease(pVarResult, pdisp);
				}
				return S_OK;
			//Close
			case 0x10000031:
				teSetBool(pVarResult, Close(FALSE));
				return S_OK;
			//Title
			case 0x10000032:
				nIndex = GetTabIndex();
				if (nIndex >= 0) {
					if (nArg >= 0) {
						VARIANT vText;
						teVariantChangeType(&vText, &pDispParams->rgvarg[nArg], VT_BSTR);
						SetTitle(vText.bstrVal, nIndex);
						VariantClear(&vText);
					}
					if (pVarResult) {
						BSTR bsText = ::SysAllocStringLen(NULL, MAX_PATH);
						bsText[0] = NULL;
						tcItem.pszText = bsText;
						tcItem.mask = TCIF_TEXT;
						tcItem.cchTextMax = MAX_PATH;
						TabCtrl_GetItem(m_pTC->m_hwnd, nIndex, &tcItem);
						int nCount = lstrlen(tcItem.pszText);
						int j = 0;
						WCHAR c = NULL;
						for (int i = 0; i < nCount; i++) {
							bsText[j] = bsText[i];
							if (c != '&' || bsText[i] != '&') {
								j++;
								c = bsText[i];
							} else {
								c = NULL;
							}
						}
						bsText[j] = NULL;
						teSetBSTR(pVarResult, &bsText, -1);
					}
				}
				return S_OK;
			//Suspend
			case 0x10000033:
				Suspend(nArg >= 0 ? GetIntFromVariant(&pDispParams->rgvarg[nArg]) : 0);
				return S_OK;
			//Items
			case 0x10000040:
				FolderItems *pFolderItems;
				if SUCCEEDED(Items(nArg >= 0 ? GetIntFromVariant(&pDispParams->rgvarg[nArg]) : SVGIO_ALLVIEW | SVGIO_FLAG_VIEWORDER, &pFolderItems)) {
					teSetObjectRelease(pVarResult, pFolderItems);
				}
				return S_OK;
			//SelectedItems
			case 0x10000041:
				if SUCCEEDED(Items(SVGIO_SELECTION, &pFolderItems)) {
					teSetObjectRelease(pVarResult, pFolderItems);
				}
				return S_OK;
			//ShellFolderView
			case 0x10000050:
				teSetObject(pVarResult, m_pdisp);
				return S_OK;
			//DropTarget
			case 0x10000058:
				IDropTarget *pDT;
				pDT = NULL;
				if (!m_pDropTarget2 || FAILED(m_pDropTarget2->QueryInterface(IID_PPV_ARGS(&pDT)))) {
					LPITEMIDLIST pidl;
					if (teGetIDListFromObject(m_pFolderItem, &pidl)) {
						IShellFolder *pSF;
						LPCITEMIDLIST pidlPart;
						if SUCCEEDED(SHBindToParent(pidl, IID_PPV_ARGS(&pSF), &pidlPart)) {
							pSF->GetUIObjectOf(g_hwndMain, 1, &pidlPart, IID_IDropTarget, NULL, (LPVOID*)&pDT);
							pSF->Release();
						}
						teCoTaskMemFree(pidl);
					}
				}
				if (pDT) {
					teSetObjectRelease(pVarResult, new CteDropTarget(pDT, NULL));
					pDT->Release();
				}
				return S_OK;
			//Columns
			case 0x10000059:
				if (m_hwndDV) {
					if (nArg >= 0) {
						if (pDispParams->rgvarg[nArg].vt == VT_BSTR) {
							SetColumnsStr(pDispParams->rgvarg[nArg].bstrVal);
						}
						nFormat = GetIntFromVariant(&pDispParams->rgvarg[nArg]);
					}
					if (pVarResult) {
						pVarResult->vt = VT_BSTR;
						pVarResult->bstrVal = GetColumnsStr(nFormat);
					}
				}
				return S_OK;
/*			//Searches //test
			case 0x1000005A:
				IDispatch *pArray;
				GetNewArray(&pArray);
				IEnumExtraSearch *pees;
				if SUCCEEDED(m_pSF2->EnumSearches(&pees)) {
					EXTRASEARCH es;
					ULONG ulFetched;
					VARIANT v;
					VariantInit(&v);
					WCHAR pszBuff[SIZE_BUFF];
					while (pees->Next(1, &es, &ulFetched) == S_OK) {
						StringFromGUID2(es.guidSearch, pszBuff, 39);
						swprintf_s(&pszBuff[40], SIZE_BUFF, L"\t\s\t\s", es.wszFriendlyName, es.wszUrl);
						teSetSZ(&v, pszBuff);
						teExecMethod(pArray, L"push", NULL, -1, &v);
						VariantClear(&v);
					}
					pees->Release();
				}
				teSetObjectRelease(pVarResult, pArray);
				return S_OK;*/
			//MapColumnToSCID
			case 0x1000005B:
				if (nArg >= 1 && pVarResult) {
					SHCOLUMNID scid;
#ifdef _2000XP
					if SUCCEEDED(MapColumnToSCID((UINT)GetIntFromVariant(&pDispParams->rgvarg[nArg]), &scid)) {
#else
					if SUCCEEDED(m_pSF2->MapColumnToSCID((UINT)GetIntFromVariant(&pDispParams->rgvarg[nArg]), &scid)) {
#endif
						pVarResult->bstrVal = tePSGetNameFromPropertyKeyEx(scid, GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]), m_pShellView);
						pVarResult->vt = VT_BSTR;
					}
				}
				return S_OK;
			//hwndList
			case 0x10000102:
				teSetPtr(pVarResult, m_hwndLV);
				return S_OK;
			//hwndView
			case 0x10000103:
				teSetPtr(pVarResult, m_hwndDV);
				return S_OK;
			//SortColumn
			case 0x10000104:
				if (nArg >= 0) {
					if (wFlags & DISPATCH_METHOD) {
						nFormat = GetIntFromVariant(&pDispParams->rgvarg[nArg]);
					} else if (pDispParams->rgvarg[nArg].vt == VT_BSTR) {
						SetSort(pDispParams->rgvarg[nArg].bstrVal);
					}
				}
				if (pVarResult) {
					pVarResult->vt = VT_BSTR;
					GetSort(&pVarResult->bstrVal, nFormat);
				}
				return S_OK;
			//GroupBy
			case 0x10000105:
				if (nArg >= 0) {
					if (pDispParams->rgvarg[nArg].vt == VT_BSTR) {
						SetGroupBy(pDispParams->rgvarg[nArg].bstrVal);
					}
				}
				if (pVarResult) {
					pVarResult->vt = VT_BSTR;
					GetGroupBy(&pVarResult->bstrVal);
				}
				return S_OK;
			//Focus
			case 0x10000106:
				SetActive(TRUE);
				return S_OK;
			//HitTest (Screen coordinates)
			case 0x10000107:
				if (nArg >= 0 && pVarResult) {
					LVHITTESTINFO info;
					GetPointFormVariant(&info.pt, &pDispParams->rgvarg[nArg]);
					UINT flags = nArg >= 1 ? GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]) : LVHT_ONITEM;
					teSetLong(pVarResult, (int)DoHitTest(this, info.pt, flags));
					if (pVarResult->lVal < 0) {
						if (m_hwndLV) {
							ScreenToClient(m_hwndLV, &info.pt);
							info.flags = flags;
							ListView_HitTest(m_hwndLV, &info);
							if (info.flags & flags) {
								pVarResult->lVal = info.iItem;
								if (info.flags == LVHT_ONITEMLABEL && m_param[SB_ViewMode] == FVM_DETAILS) {
									RECT rc;
									ListView_GetItemRect(m_hwndLV, info.iItem, &rc, LVIR_ICON);
									if (info.pt.x < rc.left) {
										pVarResult->lVal = -1;
									}
								}
							}
						} else if (g_pAutomation) {
							IUIAutomationElement *pElement, *pElement2 = NULL;
							if SUCCEEDED(g_pAutomation->ElementFromPoint(info.pt, &pElement)) {
								if SUCCEEDED(pElement->GetCurrentPropertyValue(g_PID_ItemIndex, pVarResult)) {
									pVarResult->lVal--;
								}
								if (pVarResult->lVal < 0) {
									IUIAutomationTreeWalker *pWalker = NULL;
									if SUCCEEDED(g_pAutomation->get_ControlViewWalker(&pWalker)) {
										pWalker->GetParentElement(pElement, &pElement2);
										if (pElement2) {
											if SUCCEEDED(pElement2->GetCurrentPropertyValue(g_PID_ItemIndex, pVarResult)) {
												pVarResult->lVal--;
											}
											pElement2->Release();
										}
										pWalker->Release();
									}
								}
								pElement->Release();
							}
						}
					}
					if (nArg == 0) {
						int i = GetIntFromVariantClear(pVarResult);
						IFolderView *pFV;
						if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV))) {
							LPITEMIDLIST pidl;
							if SUCCEEDED(pFV->Item(i, &pidl)) {
								LPITEMIDLIST pidlFull = ILCombine(m_pidl, pidl);
								teSetIDListRelease(pVarResult, &pidlFull);
								teCoTaskMemFree(pidl);
							}
							pFV->Release();
						}
					}
				}
				return S_OK;
			//hwndAlt
			case 0x10000108:
				if (nArg >= 0) {
					m_hwndAlt = (HWND)GetPtrFromVariant(&pDispParams->rgvarg[nArg]);
					ArrangeWindow();
				}
				teSetPtr(pVarResult, m_hwndAlt);
				return S_OK;
			//ItemCount
			case 0x10000110:
				if (pVarResult && m_pShellView) {
					UINT uItem = (nArg >= 0) ? GetIntFromVariant(&pDispParams->rgvarg[nArg]) : SVGIO_ALLVIEW;
					if (m_ppDispatch[SB_AltSelectedItems] && uItem == SVGIO_SELECTION) {
						FolderItems *pid;
						if SUCCEEDED(m_ppDispatch[SB_AltSelectedItems]->QueryInterface(IID_PPV_ARGS(&pid))) {
							if SUCCEEDED(pid->get_Count(&pVarResult->lVal)) {
								pVarResult->vt = VT_I4;
							}
							pid->Release();
						}
						return S_OK;
					}
					IFolderView *pFV;
					if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV))) {
						if SUCCEEDED(pFV->ItemCount(uItem, &pVarResult->intVal)) {
							pVarResult->vt = VT_I4;
						}
						pFV->Release();
					}
				}
				return S_OK;
			//Item
			case 0x10000111:
				if (pVarResult && m_pShellView && nArg >= 0) {
					IFolderView *pFV;
					if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV))) {
						LPITEMIDLIST pidl;
						if SUCCEEDED(pFV->Item(GetIntFromVariant(&pDispParams->rgvarg[nArg]), &pidl)) {
							LPITEMIDLIST pidlFull = ILCombine(m_pidl, pidl);
							teSetIDListRelease(pVarResult, &pidlFull);
							teCoTaskMemFree(pidl);
						}
						pFV->Release();
					}
				}
				return S_OK;
			//Refresh
			case 0x10000206:
				if (!m_bVisible && nArg >= 0 && GetIntFromVariant(&pDispParams->rgvarg[nArg])) {
					m_bRefreshLator = TRUE;
					return S_OK;
				}
				Refresh(TRUE);
				return S_OK;
			//Refresh2Ex
			case 0x20000206:
				if (nArg >= 0) {
					FolderItem *pid = NULL;
					GetFolderItemFromVariant(&pid, &pDispParams->rgvarg[nArg]);
					if (pid && m_pFolderItem) {
						m_pFolderItem->Release();
						m_pFolderItem = pid;
						if (ILIsEqual(m_pidl, g_pidls[CSIDL_RESULTSFOLDER])) {
							teILFreeClear(&m_pidl);
							teGetIDListFromObject(m_pFolderItem, &m_pidl);
						}
					}
					Refresh(FALSE);
				}
				return S_OK;
			//ViewMenu
			case 0x10000207:
				IContextMenu *pCM;
				CteContextMenu *pdispCM;
				pCM = NULL;
				pdispCM = NULL;
				try {
					if SUCCEEDED(m_pShellView->GetItemObject(nArg >= 0 ? GetIntFromVariant(&pDispParams->rgvarg[nArg]) : SVGIO_BACKGROUND, IID_PPV_ARGS(&pCM))) {
						CteContextMenu *pdispCM = new CteContextMenu(pCM, NULL, static_cast<IShellBrowser *>(this));
						teSetObject(pVarResult, pdispCM);
					}
				} catch(...) {
					g_nException = 0;
#ifdef _DEBUG
					g_strException = L"ViewMenu";
#endif
				}
				SafeRelease(&pdispCM);
				SafeRelease(&pCM);
				return S_OK;
			//TranslateAccelerator
			case 0x10000208:
				HRESULT hr;
				hr = E_NOTIMPL;
				if (m_pShellView) {
					if (nArg >= 3) {
						MSG msg;
						msg.hwnd = (HWND)GetPtrFromVariant(&pDispParams->rgvarg[nArg]);
						msg.message = GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]);
						msg.wParam = (WPARAM)GetPtrFromVariant(&pDispParams->rgvarg[nArg - 2]);
						msg.lParam = (LPARAM)GetPtrFromVariant(&pDispParams->rgvarg[nArg - 3]);
						hr = m_pShellView->TranslateAcceleratorW(&msg);
					}
				}
				teSetLong(pVarResult, hr);
				return S_OK;
			case 0x10000209:// GetItemPosition
				hr = E_NOTIMPL;
				if (m_pShellView) {
					if (nArg >= 1) {
						VARIANT vMem;
						VariantInit(&vMem);
						LPPOINT ppt = (LPPOINT)GetpcFromVariant(&pDispParams->rgvarg[nArg - 1], &vMem);
						if (ppt) {
							IFolderView *pFV;
							if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV))) {
								LPITEMIDLIST pidl;
								if (teGetIDListFromVariant(&pidl, &pDispParams->rgvarg[nArg])) {
									hr = pFV->GetItemPosition(ILFindLastID(pidl), ppt);
									teCoTaskMemFree(pidl);
								}
								pFV->Release();
							}
						}
						teWriteBack(&pDispParams->rgvarg[nArg - 1], &vMem);
					}
				}
				teSetLong(pVarResult, hr);
				return S_OK;
			//SelectAndPositionItem
			case 0x1000020A:
				hr = E_NOTIMPL;
				if (m_pShellView) {
					if (nArg >= 2) {
						VARIANT vMem;
						VariantInit(&vMem);
						LPPOINT ppt = (LPPOINT)GetpcFromVariant(&pDispParams->rgvarg[nArg - 2], &vMem);
						if (ppt) {
							IFolderView *pFV;
							if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV))) {
								LPITEMIDLIST pidl = NULL;
								if (teGetIDListFromVariant(&pidl, &pDispParams->rgvarg[nArg])) {
									LPCITEMIDLIST pidlLast = ILFindLastID(pidl);
									hr = pFV->SelectAndPositionItems(1, &pidlLast, ppt, GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]));
									teCoTaskMemFree(pidl);
								}
								pFV->Release();
							}
						}
						VariantClear(&vMem);
					}
				}
				teSetLong(pVarResult, hr);
				return S_OK;
			//SelectItem
			case 0x10000280:
				if (nArg >= 1) {
					SelectItem(&pDispParams->rgvarg[nArg], GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]));
				}
				return S_OK;
			//FocusedItem
			case 0x10000281:
				FolderItem *pFolderItem;
				if (get_FocusedItem(&pFolderItem) == S_OK) {
					teSetObjectRelease(pVarResult, pFolderItem);
				}
				return S_OK;
			//GetFocusedItem
			case 0x10000282:
				if (pVarResult) {
					GetFocusedIndex(&pVarResult->intVal);
					pVarResult->vt = VT_I4;
				}
				return S_OK;
			//GetItemRect
			case 0x10000283:
				hr = E_NOTIMPL;
				if (m_pShellView && m_hwndLV) {
					if (nArg >= 1) {
						VARIANT vMem;
						VariantInit(&vMem);
						LPRECT prc = (LPRECT)GetpcFromVariant(&pDispParams->rgvarg[nArg - 1], &vMem);
						if (prc) {
							IFolderView *pFV;
							if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV))) {
								LPITEMIDLIST pidl;
								if (teGetIDListFromVariant(&pidl, &pDispParams->rgvarg[nArg])) {
									LPITEMIDLIST pidlChild = ILFindLastID(pidl);
									int i = 0;
									pFV->ItemCount(SVGIO_ALLVIEW, &i);
									while (i-- > 0) {
										LPITEMIDLIST pidlPart;
										if SUCCEEDED(pFV->Item(i, &pidlPart)) {
											if (ILIsEqual(pidlChild, pidlPart)) {
												hr = ListView_GetItemRect(m_hwndLV, i, prc, nArg >= 2 ? GetIntFromVariant(&pDispParams->rgvarg[nArg - 2]) : LVIR_BOUNDS) ? S_OK : E_FAIL;
												i = 0;
											}
											teCoTaskMemFree(pidlPart);
										}
									}
									teCoTaskMemFree(pidl);
								}
								pFV->Release();
							}
						}
						teWriteBack(&pDispParams->rgvarg[nArg - 1], &vMem);
					}
				}
				teSetLong(pVarResult, hr);
				return S_OK;
			//Notify
			case 0x10000300:
				if (nArg >= 2) {
					long lEvent = GetIntFromVariant(&pDispParams->rgvarg[nArg]);
					BOOL bTFS = (nArg >= 3 && GetIntFromVariant(&pDispParams->rgvarg[nArg - 3]));
					for (int i = 1; i < (lEvent & (SHCNE_RENAMEITEM | SHCNE_RENAMEFOLDER) ? 3 : 2); i++) {
						LPITEMIDLIST pidl;
						if (teGetIDListFromVariant(&pidl, &pDispParams->rgvarg[nArg - i])) {
							if (!ILIsEmpty(pidl)) {
								IShellFolder2 *pSF2;
								LPCITEMIDLIST pidlLast;
								if SUCCEEDED(SHBindToParent(pidl, IID_PPV_ARGS(&pSF2), &pidlLast)) {
									if (bTFS) {
										WCHAR szBuf[MAX_COLUMN_NAME_LEN];
										SetFolderSize(pSF2, pidlLast, szBuf, _countof(szBuf));
									} else {
										BSTR bsPath;
										if SUCCEEDED(teGetDisplayNameBSTR(pSF2, pidl, SHGDN_FORPARSING, &bsPath)) {
											if SUCCEEDED(teDelProperty(m_ppDispatch[SB_TotalFileSize], bsPath)) {
												if (m_hwndLV) {
													InvalidateRect(m_hwndLV, NULL, FALSE);
												}
											}
											teSysFreeString(&bsPath);
										}
									}
									pSF2->Release();
								}
							}
							teCoTaskMemFree(pidl);
						}
					}
				}
				return S_OK;
			//NavigateComplete
			case 0x10000400:
				m_bBeforeNavigate = FALSE;
				return S_OK;
			//NavigationComplete
			case 0x10000500:
				NavigateComplete(FALSE);
				return S_OK;
			//AddItem
			case 0x10000501:
				if (nArg >= 0) {
					if (nArg >= 1) {
						DWORD dwSessionId = GetIntFromVariant(&pDispParams->rgvarg[0]);
						if (dwSessionId != m_dwSessionId) {
							return E_ACCESSDENIED;
						}
					}
					LPITEMIDLIST pidl;
					if (teGetIDListFromVariant(&pidl, &pDispParams->rgvarg[nArg])) {
						AddItem(pidl);
					}
				}
				return S_OK;
			//RemoveItem
			case 0x10000502:
				hr = E_NOTIMPL;
				if (nArg >= 0) {
					LPITEMIDLIST pidl;
					if (teGetIDListFromVariant(&pidl, &pDispParams->rgvarg[nArg])) {
						IFolderView *pFV = NULL;
						IResultsFolder *pRF;
						if (SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV))) && SUCCEEDED(pFV->GetFolder(IID_PPV_ARGS(&pRF)))) {
							hr = pRF->RemoveIDList(pidl);
							pRF->Release();
#ifdef _2000XP
						} else if (ILIsEqual(m_pidl, g_pidls[CSIDL_RESULTSFOLDER])) {
							IShellFolderView *pSFV;
							if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pSFV))) {
								BSTR bs;
								if SUCCEEDED(teGetDisplayNameFromIDList(&bs, pidl, SHGDN_FORPARSING)) {
									teILCloneReplace(&pidl, teILCreateFromPath(bs));
									LPITEMIDLIST pidlChild = teILCreateResultsXP(pidl);
									UINT ui;
									hr = pSFV->RemoveObject(pidlChild, &ui);
									teCoTaskMemFree(pidlChild);
									::SysFreeString(bs);
								}
								pSFV->Release();
							}
#endif
						}
						SafeRelease(&pFV);
						teCoTaskMemFree(pidl);
					}
				}
				teSetLong(pVarResult, hr);
				if (hr == S_OK) {
					SetTimer(g_hwndMain, TET_Status, 500, teTimerProc);
				}
				return S_OK;
			//AddItems
			case 0x10000503:
				if (nArg >= 0) {
					if (GetDispatch(&pDispParams->rgvarg[nArg], &pdisp) || teGetDispatchFromString(&pDispParams->rgvarg[nArg], &pdisp)) {
						if (!LoadString(g_hShell32, 13585, g_szStatus, _countof(g_szStatus))) {
							LoadString(g_hShell32, 6478, g_szStatus, _countof(g_szStatus));
						}
						DoStatusText(this, g_szStatus, 0);
#ifdef _USE_THREADADDITEMS
						TEAddItems *pAddItems = new TEAddItems[1];
						CoMarshalInterThreadInterfaceInStream(IID_IDispatch, static_cast<IDispatch *>(this), &pAddItems->pStrmSB);
						CoMarshalInterThreadInterfaceInStream(IID_IDispatch, pdisp, &pAddItems->pStrmArray);
						pdisp->Release();
						pAddItems->pv = GetNewVARIANT(2);
						teSetLong(&pAddItems->pv[0], m_dwSessionId);
						pAddItems->bDeleted = nArg >= 1 && GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]);
						pAddItems->bNavigateComplete = FALSE;
						pAddItems->pStrmOnCompleted = NULL;
						if (nArg >= 2) {
							IUnknown *punk;
							if (FindUnknown(&pDispParams->rgvarg[nArg - 2], &punk)) {
								CoMarshalInterThreadInterfaceInStream(IID_IDispatch, punk, &pAddItems->pStrmOnCompleted);
							} else {
								pAddItems->bNavigateComplete = GetIntFromVariant(&pDispParams->rgvarg[nArg - 2]);
							}
						}
						_beginthread(&threadAddItems, 0, pAddItems);
#else
						IDispatch *pOnCompleted = NULL;
						BOOL bNavigateComplete = FALSE;
						if (nArg >= 2) {
							if (!GetDispatch(&pDispParams->rgvarg[nArg - 2], &pOnCompleted)) {
								bNavigateComplete = GetIntFromVariant(&pDispParams->rgvarg[nArg - 2]);
							}
						}
						AddItems(pdisp, nArg >= 1 && GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]), pOnCompleted, bNavigateComplete);
#endif
					}
				}
				return S_OK;
			//RemoveAll
			case 0x10000504:
				teSetLong(pVarResult, RemoveAll());
				return S_OK;
			//SessionId
			case 0x10000505:
				teSetLong(pVarResult, m_dwSessionId);
				return S_OK;
			//
			case DISPID_VALUE:
				teSetObject(pVarResult, this);
				return S_OK;
			//DIID_DShellFolderViewEvents
/*///
			case DISPID_EXPLORERWINDOWREADY:
				return E_NOTIMPL;
//*///
			case DISPID_SELECTIONCHANGED://XP+
				SetStatusTextSB(NULL);
				return DoFunc(TE_OnSelectionChanged, this, S_OK);
			case DISPID_FILELISTENUMDONE://XP+
				m_pTC->m_nRedraw &= ~TEREDRAW_NAVIGATE;
				if (!m_bNavigateComplete && ILIsEqual(m_pidl, g_pidls[CSIDL_RESULTSFOLDER])) {
					return S_OK;
				}
				IFolderView2 *pFV2;
				if (m_bRefreshing) {
					m_bRefreshing = FALSE;
					if (!m_bNavigateComplete && m_bsAltSortColumn) {
						BSTR bs = ::SysAllocString(m_bsAltSortColumn);
						SetSort(bs);
						::SysFreeString(bs);
					}
				}
				OnNavigationComplete2();
				SetListColumnWidth();
				if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV2))) {
					WCHAR pszFormat[MAX_STATUS];
					pszFormat[0] = NULL;
					LPWSTR lpBuf = pszFormat;
					if (m_dwUnavailable) {
						LoadString(g_hShell32, 4157, pszFormat, MAX_STATUS);
					} else if (!m_bsFilter && !m_ppDispatch[SB_OnIncludeObject]) {
						lpBuf = NULL;
					}
					pFV2->SetText(FVST_EMPTYTEXT, lpBuf);
					pFV2->Release();
				}
				if (!m_hwndLV && m_pExplorerBrowser) {
					SetRedraw(TRUE);
					if (teGetWindow(m_pExplorerBrowser, &m_hwnd) == S_OK && m_hwnd) {
						RedrawWindow(m_hwnd, NULL, 0, RDW_NOERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
					}
				}
				SetTimer(g_hwndMain, TET_Status, 500, teTimerProc);
				return S_OK;
			case DISPID_VIEWMODECHANGED://XP+
				m_bCheckLayout = TRUE;
				SetFolderFlags(TRUE);
				SetListColumnWidth();
				return DoFunc(TE_OnViewModeChanged, this, S_OK);
			case DISPID_BEGINDRAG://XP+
				DoFunc1(TE_OnBeginDrag, this, pVarResult);
				if ((m_bRegenerateItems || ILIsEqual(m_pidl, g_pidls[CSIDL_RESULTSFOLDER])) && pVarResult && (pVarResult->vt != VT_BOOL || pVarResult->boolVal)) {
					FolderItems *pid;
					if SUCCEEDED(SelectedItems(&pid)) {
						IDataObject *pDataObj = NULL;
						if SUCCEEDED(pid->QueryInterface(IID_PPV_ARGS(&pDataObj))) {
							DWORD dwEffect = DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_LINK;
							g_nDropState = 1;
							try {
								SHDoDragDrop(NULL, pDataObj, static_cast<IDropSource *>(g_pTE), dwEffect, &dwEffect);
							} catch(...) {
								g_nException = 0;
#ifdef _DEBUG
								g_strException = L"BeginDrag";
#endif
							}
							pDataObj->Release();
							teSetBool(pVarResult, FALSE);
						}
						pid->Release();
					}
				}
				return S_OK;
			case DISPID_COLUMNSCHANGED://XP-
				InitFolderSize();
				return DoFunc(TE_OnColumnsChanged, this, S_OK);
			case DISPID_ICONSIZECHANGED://XP-
				return DoFunc(TE_OnIconSizeChanged, this, S_OK);
			case DISPID_SORTDONE://XP-
				m_nSorting = 0;
				m_pTC->RedrawUpdate();
				teSysFreeString(&m_bsAltSortColumn);
				if (m_nFocusItem < 0) {
					FocusItem(FALSE);
				}
				return DoFunc(TE_OnSort, this, S_OK);
			case DISPID_INITIALENUMERATIONDONE://XP-
				if (ILIsEqual(m_pidl, g_pidls[CSIDL_RESULTSFOLDER])) {
					return S_OK;
				}
				if (m_bViewCreated) {
					return OnViewCreated(NULL);
				}
				SetFolderFlags(FALSE);
				return S_OK;
/*			case DISPID_CONTENTSCHANGED://XP-
				return S_OK;*/
			default:
				if (dispIdMember >= START_OnFunc && dispIdMember < START_OnFunc + Count_SBFunc) {
					teInvokeObject(&m_ppDispatch[dispIdMember - START_OnFunc], wFlags, pVarResult, nArg, pDispParams->rgvarg);
					return S_OK;
				}
		}
		if (m_pdisp) {
			return m_pdisp->Invoke(dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
		}
	} catch (...) {
		return teException(pExcepInfo, "FolderView", methodSB, dispIdMember);
	}
	return DISP_E_MEMBERNOTFOUND;
}

STDMETHODIMP CteShellBrowser::get_Application(IDispatch **ppid)
{
	HRESULT hr = E_NOTIMPL;
	IShellFolderViewDual *pSFVD;
	if SUCCEEDED(m_pdisp->QueryInterface(IID_PPV_ARGS(&pSFVD))) {
		hr = pSFVD->get_Application(ppid);
		pSFVD->Release();
	}
	return hr;
}

STDMETHODIMP CteShellBrowser::get_Parent(IDispatch **ppid)
{
	return m_pTC->QueryInterface(IID_PPV_ARGS(ppid));
}

STDMETHODIMP CteShellBrowser::get_Folder(Folder **ppid)
{
	HRESULT hr = E_NOTIMPL;
	IShellFolderViewDual *pSFVD;
	if SUCCEEDED(m_pdisp->QueryInterface(IID_PPV_ARGS(&pSFVD))) {
		hr = pSFVD->get_Folder(ppid);
		pSFVD->Release();
	}
	return SUCCEEDED(hr) ? hr : GetFolderObjFromIDList(m_pidl, ppid);
}

STDMETHODIMP CteShellBrowser::SelectedItems(FolderItems **ppid)
{
	return Items(SVGIO_SELECTION, ppid);
}

HRESULT CteShellBrowser::Items(UINT uItem, FolderItems **ppid)
{
	if (m_ppDispatch[SB_AltSelectedItems] && uItem == SVGIO_SELECTION) {
		return m_ppDispatch[SB_AltSelectedItems]->QueryInterface(IID_PPV_ARGS(ppid));
	}
	FolderItems *pItems = NULL;
	IDataObject *pDataObj = NULL;
	if (m_pShellView) {
#ifdef _2000XP
		if (!g_bUpperVista && ListView_GetSelectedCount(m_hwndLV) > 1) {
			BOOL bResultsFolder = ILIsEqual(m_pidl, g_pidls[CSIDL_RESULTSFOLDER]);
			if (bResultsFolder || (uItem & (SVGIO_SELECTION | SVGIO_FLAG_VIEWORDER)) == (SVGIO_SELECTION | SVGIO_FLAG_VIEWORDER)) {
				CteFolderItems *pFolderItems = new CteFolderItems(NULL, NULL);
				IShellFolderView *pSFV;
				if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pSFV))) {
					if (uItem & SVGIO_SELECTION) {
						int nIndex = -1, nFocus = -1;
						if ((uItem & SVGIO_FLAG_VIEWORDER) == 0) {
							nFocus = ListView_GetNextItem(m_hwndLV, -1, LVNI_FOCUSED | LVNI_ALL);
							AddPathXP(pFolderItems, pSFV, nFocus, bResultsFolder);
						}
						while ((nIndex = ListView_GetNextItem(m_hwndLV, nIndex, LVNI_SELECTED | LVNI_ALL)) >= 0) {
							if (nIndex != nFocus) {
								AddPathXP(pFolderItems, pSFV, nIndex, bResultsFolder);
							}
						}
					} else {
						UINT uCount = 0;
						pSFV->GetObjectCount(&uCount);
						for (UINT u = 0; u < uCount; u++) {
							AddPathXP(pFolderItems, pSFV, u, bResultsFolder);
						}
					}
					pSFV->Release();
				}
				*ppid = pFolderItems;
				return S_OK;
			}
		}
#endif
		m_pShellView->GetItemObject(uItem, IID_PPV_ARGS(&pDataObj));
	}
	CteFolderItems *pid = new CteFolderItems(pDataObj, pItems);
	if (m_bRegenerateItems) {
		pid->Regenerate(TRUE);
	}
	*ppid = pid;
	SafeRelease(&pDataObj);
	return S_OK;
}


#ifdef _2000XP
VOID CteShellBrowser::AddPathXP(CteFolderItems *pFolderItems, IShellFolderView *pSFV, int nIndex, BOOL bResultsFolder)
{
	try {
		LPCITEMIDLIST pidl;
		if SUCCEEDED(pSFV->GetObjectW(const_cast<LPITEMIDLIST *>(&pidl), nIndex)) {
			VARIANT v;
			VariantInit(&v);
			if (bResultsFolder) {
				if SUCCEEDED(teGetDisplayNameBSTR(m_pSF2, pidl, SHGDN_FORADDRESSBAR | SHGDN_FORPARSING, &v.bstrVal)) {
					v.vt = VT_BSTR;
				}
			} else {
				FolderItem *pid;
				LPITEMIDLIST pidlFull = ILCombine(m_pidl, pidl);
				if (GetFolderItemFromIDList(&pid, pidlFull)) {
					teSetObject(&v, pid);
					pid->Release();
				}
				teCoTaskMemFree(pidlFull);
			}
			if (v.vt != VT_EMPTY) {
				pFolderItems->ItemEx(-1, NULL, &v);
				VariantClear(&v);
			}
		}
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"AddPathXP";
#endif
	}
}

int CteShellBrowser::PSGetColumnIndexXP(LPWSTR pszName, int *pcxChar)
{
	SHELLDETAILS sd;
	BSTR bs;
	for (UINT i = 0; i < m_nColumns && GetDetailsOf(NULL, i, &sd) == S_OK; i++) {
		if SUCCEEDED(StrRetToBSTR(&sd.str, NULL, &bs)) {
			if (teStrSameIFree(bs, pszName)) {
				if (pcxChar) {
					*pcxChar = sd.cxChar;
				}
				return i;
			}
		}
	}
	return -1;
}

BSTR CteShellBrowser::PSGetNameXP(LPWSTR pszName, int nFormat)
{
	if (nFormat == 0) {
		return ::SysAllocString(pszName);
	}
	int i = PSGetColumnIndexXP(pszName, NULL);
	if (i >= 0) {
		SHCOLUMNID scid;
		if SUCCEEDED(MapColumnToSCID(i, &scid)) {
			return tePSGetNameFromPropertyKeyEx(scid, nFormat, NULL);
		}
	}
	return NULL;
}

VOID CteShellBrowser::AddColumnDataXP(LPWSTR pszColumns, LPWSTR pszName, int nWidth, int nFormat)
{
	if (nFormat) {
		AddColumnDataEx(pszColumns, PSGetNameXP(pszName, nFormat), nWidth);
		return;
	}
	AddColumnData(pszColumns, pszName, nWidth);
}

HRESULT CteShellBrowser::NavigateSB(IShellView *pPreviousView, FolderItem *pPrevious)
{
	HRESULT hr = E_FAIL;
	int nCreate = 4;
	while ((nCreate -= 2) > 0) {
		if (GetShellFolder2(&m_pidl) == S_OK) {
#ifdef _2000XP
			if (!g_bUpperVista) {
				m_nFolderName = MAXINT;
				if (m_nColumns) {
					delete [] m_pColumns;
					m_nColumns = 0;
				}
				SHCOLUMNID scid;
				while (m_pSF2->MapColumnToSCID(m_nColumns, &scid) == S_OK && m_nColumns < MAX_COLUMNS) {
					m_nColumns++;
				}
				m_nColumns += 2;
				m_pColumns = new TEColumn[m_nColumns];
				BOOL bIsResultsFolder = ILIsEqual(m_pidl, g_pidls[CSIDL_RESULTSFOLDER]);
				for (int i = m_nColumns; i--;) {
					if FAILED(m_pSF2->GetDefaultColumnState(i, &m_pColumns[i].csFlags)) {
						m_pColumns[i].csFlags = SHCOLSTATE_TYPE_STR | SHCOLSTATE_ONBYDEFAULT;
					}
					m_pColumns[i].nWidth = -1;
					if (bIsResultsFolder && m_pSF2->MapColumnToSCID(i, &scid) == S_OK && IsEqualPropertyKey(scid, PKEY_ItemFolderNameDisplay)) {
						m_nFolderName = i;
					}
				}
				m_pColumns[m_nColumns - 2].csFlags = SHCOLSTATE_TYPE_STR | SHCOLSTATE_SECONDARYUI | SHCOLSTATE_PREFER_VARCMP;
				m_pColumns[m_nColumns - 1].csFlags = SHCOLSTATE_TYPE_STR | SHCOLSTATE_SECONDARYUI | SHCOLSTATE_PREFER_VARCMP;
			}
#endif
			try {
				hr = CreateViewWindowEx(pPreviousView);
			} catch (...) {
				hr = E_FAIL;
				g_nException = 0;
#ifdef _DEBUG
				g_strException = L"NavigateSB";
#endif
			}
		}
		if (hr != S_OK) {
			teILCloneReplace(&m_pidl, g_pidls[CSIDL_RESULTSFOLDER]);
			m_dwUnavailable = GetTickCount();
			nCreate++;
			hr = S_FALSE;
		}
	}
	return hr;
}
#endif

HRESULT CteShellBrowser::CreateViewWindowEx(IShellView *pPreviousView)
{
	HRESULT hr = E_FAIL;
	m_pShellView = NULL;
	RECT rc;
	if (m_pSF2) {
#ifdef _2000XP
		if SUCCEEDED(CreateViewObject(m_pTC->m_hwndStatic, IID_PPV_ARGS(&m_pShellView)) && m_pShellView) {
			if (!g_bUpperVista && m_param[SB_ViewMode] == FVM_ICON && m_param[SB_IconSize] >= 96) {
				m_param[SB_ViewMode] = FVM_THUMBNAIL;
			}
#else
		if SUCCEEDED(m_pSF2->CreateViewObject(m_pTC->m_hwndStatic, IID_PPV_ARGS(&m_pShellView)) && m_pShellView) {
#endif
			try {
				if (::InterlockedIncrement(&m_nCreate) <= 1) {
					m_hwnd = NULL;
					IEnumIDList *peidl = NULL;
					BOOL bResultsFolder = ILIsEqual(m_pidl, g_pidls[CSIDL_RESULTSFOLDER]);
					hr =  bResultsFolder ? S_OK : m_pSF2->EnumObjects(g_hwndMain, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS | SHCONTF_INCLUDEHIDDEN | SHCONTF_INCLUDESUPERHIDDEN | SHCONTF_NAVIGATION_ENUM, &peidl);
					if (hr == S_OK) {
						if (m_nForceViewMode != FVM_AUTO) {
							m_param[SB_ViewMode] = m_nForceViewMode;
							m_bAutoVM = FALSE;
							m_nForceViewMode = FVM_AUTO;
						}
						FOLDERSETTINGS fs = { m_bAutoVM && !bResultsFolder ? FVM_AUTO : m_param[SB_ViewMode], (m_param[SB_FolderFlags] | FWF_USESEARCHFOLDER) & ~FWF_NOENUMREFRESH };
						hr = m_pShellView->CreateViewWindow(pPreviousView, &fs, static_cast<IShellBrowser *>(this), &rc, &m_hwnd);
					} else {
						hr = E_FAIL;
					}
					SafeRelease(&peidl);
				} else {
					hr = OLE_E_BLANK;
				}
			} catch (...) {
				hr = E_FAIL;
				g_nException = 0;
#ifdef _DEBUG
				g_strException = L"CreateViewWindowEx";
#endif
			}
			InterlockedDecrement(&m_nCreate);
			if (hr == S_FALSE) {
				IFolderView *pFV;
				if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV))) {
					IResultsFolder *pRF;
					if SUCCEEDED(pFV->GetFolder(IID_PPV_ARGS(&pRF))) {
						hr = S_OK;
						pRF->Release();
					}
					pFV->Release();
				}
			}
			if (hr == S_OK) {
				GetDefaultColumns();
			}
		}
	}
	return hr;
}

STDMETHODIMP CteShellBrowser::get_FocusedItem(FolderItem **ppid)
{
	HRESULT hr = E_NOTIMPL;
	*ppid = NULL;
	if (m_ppDispatch[SB_AltSelectedItems]) {
		FolderItems *pItems;
		hr = m_ppDispatch[SB_AltSelectedItems]->QueryInterface(IID_PPV_ARGS(&pItems));
		if SUCCEEDED(hr) {
			VARIANT v;
			VariantInit(&v);
			teExecMethod(m_ppDispatch[SB_AltSelectedItems], L"Index", &v, 0, NULL);
			hr = pItems->Item(v, ppid);
			pItems->Release();
		}
	} else if (m_pdisp) {
		IShellFolderViewDual *pSFVD;
		if SUCCEEDED(m_pdisp->QueryInterface(IID_PPV_ARGS(&pSFVD))) {
			hr = pSFVD->get_FocusedItem(ppid);
			if (hr == S_OK) {
				CteFolderItem *pid1;
				teQueryFolderItem(ppid, &pid1);
				pid1->Release();
			}
			pSFVD->Release();
		}
	}
	return hr;
}

STDMETHODIMP CteShellBrowser::SelectItem(VARIANT *pvfi, int dwFlags)
{
	LPITEMIDLIST pidl;
	if (pvfi->vt == VT_I4) {
		HRESULT hr = E_FAIL;
		IFolderView *pFV;
		if (m_pShellView && SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV)))) {
			int nCount = 0;
			pFV->ItemCount(SVGIO_ALLVIEW, &nCount);
			if (nCount) {
				if (m_hwndLV && (dwFlags & SVSI_FOCUSED)) {// bug fix
#ifdef _2000XP
					if (g_bUpperVista) {
#endif
						int nFocused = ListView_GetNextItem(m_hwndLV, -1, LVNI_ALL | LVNI_FOCUSED);
						if (nFocused >= 0) {
							ListView_SetItemState(m_hwndLV, nFocused, 0, LVIS_FOCUSED);
						}
#ifdef _2000XP
					}
#endif
				}
				hr = pFV->SelectItem(pvfi->lVal, dwFlags);
			}
			pFV->Release();
			if (hr == S_OK) {
				return hr;
			}
		}
	}
/*	IDataObject *pDataObj;
	if (m_pShellView && GetDataObjFromVariant(&pDataObj, pvfi)) {
		if (m_hwndLV && (dwFlags & SVSI_FOCUSED)) {// bug fix
#ifdef _2000XP
			if (g_bUpperVista) {
#endif
				int nFocused = ListView_GetNextItem(m_hwndLV, -1, LVNI_ALL | LVNI_FOCUSED);
				if (nFocused >= 0) {
					ListView_SetItemState(m_hwndLV, nFocused, 0, LVIS_FOCUSED);
				}
#ifdef _2000XP
			}
#endif
		}
		long nCount;
		LPITEMIDLIST *ppidllist = IDListFormDataObj(pDataObj, &nCount);
		if (ppidllist) {
			if (nCount >= 1 &&  ppidllist[0]) {
				for (int i = nCount; i-- > 0;) {
					m_pShellView->SelectItem(ppidllist[i + 1], dwFlags);
					teCoTaskMemFree(ppidllist[i + 1]);
				}
				teILFreeClear(&ppidllist[0]);
			}
			delete [] ppidllist;
		}
		SafeRelease(&pDataObj);
		return S_OK;
	}*/
	teGetIDListFromVariant(&pidl, pvfi);
	return SelectItemEx(&pidl, dwFlags, TRUE);
}

HRESULT CteShellBrowser::SelectItemEx(LPITEMIDLIST *ppidl, int dwFlags, BOOL bFree)
{
	HRESULT hr = E_FAIL;
	if (m_pShellView) {
		if (m_hwndLV && (dwFlags & SVSI_FOCUSED)) {// bug fix
#ifdef _2000XP
			if (g_bUpperVista) {
#endif
				int nFocused = ListView_GetNextItem(m_hwndLV, -1, LVNI_ALL | LVNI_FOCUSED);
				if (nFocused >= 0) {
					ListView_SetItemState(m_hwndLV, nFocused, 0, LVIS_FOCUSED);
				}
#ifdef _2000XP
			}
#endif
		}
		hr = m_pShellView->SelectItem(ILFindLastID(*ppidl), dwFlags);
		if ((dwFlags & (SVSI_SELECTIONMARK | SVSI_SELECT)) == (SVSI_SELECTIONMARK | SVSI_SELECT)) {
			hr = m_pShellView->SelectItem(ILFindLastID(*ppidl), dwFlags & (SVSI_NOTAKEFOCUS | SVSI_SELECT));
		}
	}
	if (bFree) {
		teILFreeClear(ppidl);
		CteFolderItem *pid1 = NULL;
		if SUCCEEDED(m_pFolderItem->QueryInterface(g_ClsIdFI, (LPVOID *)&pid1)) {
			teILFreeClear(&pid1->m_pidlFocused);
			pid1->Release();
		}
	}
	return hr;
}

STDMETHODIMP CteShellBrowser::PopupItemMenu(FolderItem *pfi, VARIANT vx, VARIANT vy, BSTR *pbs)
{
	HRESULT hr = E_NOTIMPL;
	IShellFolderViewDual *pSFVD;
	if SUCCEEDED(m_pdisp->QueryInterface(IID_PPV_ARGS(&pSFVD))) {
		hr = pSFVD->PopupItemMenu(pfi, vx, vy, pbs);
		pSFVD->Release();
	}
	return hr;
}

STDMETHODIMP CteShellBrowser::get_Script(IDispatch **ppDisp)
{
	HRESULT hr = E_NOTIMPL;
	IShellFolderViewDual *pSFVD;
	if SUCCEEDED(m_pdisp->QueryInterface(IID_PPV_ARGS(&pSFVD))) {
		hr = pSFVD->get_Script(ppDisp);
		pSFVD->Release();
	}
	return hr;
}

STDMETHODIMP CteShellBrowser::get_ViewOptions(long *plViewOptions)
{
	HRESULT hr = E_NOTIMPL;
	IShellFolderViewDual *pSFVD;
	if SUCCEEDED(m_pdisp->QueryInterface(IID_PPV_ARGS(&pSFVD))) {
		hr = pSFVD->get_ViewOptions(plViewOptions);
		pSFVD->Release();
	}
	return hr;
}

int CteShellBrowser::GetTabIndex()
{
	if (m_pTC) {
		int i;
		TC_ITEM tcItem;

		for (i = TabCtrl_GetItemCount(m_pTC->m_hwnd); i-- > 0;) {
			tcItem.mask = TCIF_PARAM;
			TabCtrl_GetItem(m_pTC->m_hwnd, i, &tcItem);
			if (tcItem.lParam == m_nSB) {
				return i;
			}
		}
	}
	return -1;
}

STDMETHODIMP CteShellBrowser::OnNavigationPending(PCIDLIST_ABSOLUTE pidlFolder)
{
	if (ILIsEqual(m_pidl, pidlFolder)) {
		if (m_bsNextFilter) {
			InitFilter();
			Refresh(FALSE);
		}
		return S_OK;
	}
	if (!m_pFolderItem1 && !m_dwUnavailable && ILIsEqual(pidlFolder, g_pidls[CSIDL_RESULTSFOLDER])) {
		m_nSuspendMode = 4;
		return E_FAIL;
	}
	return OnNavigationPending2((LPITEMIDLIST)pidlFolder);
}

HRESULT CteShellBrowser::OnNavigationPending2(LPITEMIDLIST pidlFolder)
{
	LPITEMIDLIST pidlPrevius = m_pidl;
	m_pidl = ::ILClone(pidlFolder);
	FolderItem *pPrevious = m_pFolderItem;
	if (m_pFolderItem1) {
		m_pFolderItem = m_pFolderItem1;
		m_pFolderItem1 = NULL;
	} else if (!m_dwUnavailable) {
		GetFolderItemFromIDList(&m_pFolderItem, m_pidl);
	}

	HRESULT hr = OnBeforeNavigate(pPrevious, SBSP_SAMEBROWSER | SBSP_ABSOLUTE);
	if FAILED(hr) {
		m_nLogIndex = m_nPrevLogIndex;
		if (hr == E_ABORT && Close(FALSE)) {
			return hr;
		}
		teCoTaskMemFree(m_pidl);
		m_pidl = pidlPrevius;
//		pidlPrevius = NULL;
		FolderItem *pid;
		m_pFolderItem->QueryInterface(IID_PPV_ARGS(&pid));
		m_pFolderItem = pPrevious;
//		pPrevious = NULL;
		if (hr == E_ACCESSDENIED) {
			BrowseObject2(pid, SBSP_NEWBROWSER | SBSP_ABSOLUTE);
		}
		pid->Release();
		m_nSuspendMode = 0;
		return hr;
	}
	if (m_bsNextFilter || !ILIsEqual(m_pidl, pidlPrevius)) {
		InitFilter();
	}
	teILFreeClear(&pidlPrevius);
	ResetPropEx();
	//History / Management
	SetHistory(NULL, SBSP_SAMEBROWSER | SBSP_ABSOLUTE);
	SafeRelease(&pPrevious);
	if (!m_bAutoVM) {
		SetViewModeAndIconSize(TRUE);
	}
	m_bViewCreated = TRUE;
	return hr;
}

STDMETHODIMP CteShellBrowser::OnViewCreated(IShellView *psv)
{
	m_bViewCreated = FALSE;
	if (psv) {
		psv->QueryInterface(IID_PPV_ARGS(&m_pShellView));
	}
	GetShellFolderView();
	if (m_pExplorerBrowser) {
		teGetWindow(m_pExplorerBrowser, &m_hwnd);
	}
	m_bNavigateComplete = TRUE;
	m_dwSessionId = MAKELONG(GetTickCount(), rand());
	SetPropEx();
#ifdef _2000XP
	if (g_bCharWidth) {
		if (m_hwndLV) {
			HWND hHeader = ListView_GetHeader(m_hwndLV);
			if (hHeader) {
				WCHAR szText[MAX_COLUMN_NAME_LEN];
				HD_ITEM hdi = { HDI_TEXT | HDI_WIDTH, 0, szText, NULL, MAX_COLUMN_NAME_LEN };
				Header_GetItem(hHeader, 0, &hdi);
				int cxChar = 0;
				PSGetColumnIndexXP(hdi.pszText, &cxChar);
				if (cxChar) {
					g_nCharWidth = hdi.cxy / cxChar;
				}
				g_bCharWidth = FALSE;
			}
		}
	}
#endif
	SetTabName();
	return S_OK;
}

VOID CteShellBrowser::SetTabName()
{
	//View Tab Name
	int i = GetTabIndex();
	if (i >= 0) {
		SafeRelease(&m_ppDispatch[SB_ColumnsReplace]);
		if (DoFunc(TE_OnViewCreated, this, E_NOTIMPL) != S_OK) {
			BSTR bstr;
			if (m_pFolderItem && SUCCEEDED(m_pFolderItem->get_Name(&bstr))) {
				SetTitle(bstr, i);
				::SysFreeString(bstr);
			}
		}
		InitFolderSize();
	}
	SetStatusTextSB(NULL);
}

STDMETHODIMP CteShellBrowser::OnNavigationComplete(PCIDLIST_ABSOLUTE pidlFolder)
{
	if (!teILIsFileSystemEx(pidlFolder)) {
		OnNavigationComplete2();
	}
	return S_OK;
}

VOID CteShellBrowser::OnNavigationComplete2()
{
	if (!m_bNavigateComplete) {
		return;
	}
	m_bNavigateComplete = FALSE;
	SetViewModeAndIconSize(TRUE);
	m_bIconSize = TRUE;
	if (m_pTV->m_bSetRoot) {
		if (m_pTV->Create(TRUE)) {
			m_pTV->SetRoot();
		}
	}
	SetRedraw(TRUE);
	SetActive(FALSE);
	m_nFocusItem = 1;
	NavigateComplete(TRUE);
	if (m_nFocusItem > 0) {
		FocusItem(FALSE);
	}
	m_bCheckLayout = TRUE;
	SetFolderFlags(FALSE);
	InitFolderSize();
	if (m_nSorting == 0) {
		if (m_pExplorerBrowser) {
			m_pTC->RedrawUpdate();
		} else if (m_pTC->m_nRedraw) {
			SetTimer(g_hwndMain, TET_Redraw, 1, teTimerProc);
		}
	}
}

HRESULT CteShellBrowser::BrowseToObject()
{
	m_nSuspendMode = 2;
	HRESULT hr = m_pExplorerBrowser->BrowseToObject(m_pSF2, SBSP_ABSOLUTE);
	if SUCCEEDED(hr) {
		teGetWindow(m_pExplorerBrowser, &m_hwnd);
		if (!m_pShellView) {
			m_pExplorerBrowser->GetCurrentView(IID_PPV_ARGS(&m_pShellView));
		}
	}
	return hr;
}

HRESULT CteShellBrowser::GetShellFolder2(LPITEMIDLIST *ppidl)
{
	HRESULT hr = E_FAIL;
	IShellFolder *pSF = NULL;
	if (g_pidls[CSIDL_LIBRARY]) {
		BOOL bLibrary1 = ILIsParent(g_pidls[CSIDL_LIBRARY], *ppidl, TRUE);
		if (bLibrary1 || (m_bsFilter || m_ppDispatch[SB_OnIncludeObject]) && ILIsParent(g_pidls[CSIDL_LIBRARY], *ppidl, FALSE)) {
			BSTR bs;
			teGetDisplayNameFromIDList(&bs, *ppidl, SHGDN_FORPARSING | SHGDN_ORIGINAL);
			if (bLibrary1 || teIsFileSystem(bs)) {
				LPITEMIDLIST pidl2 = teILCreateFromPath(bs);
				if (pidl2) {
					if (GetShellFolder(&pSF, pidl2)) {
						teILCloneReplace(ppidl, pidl2);
					}
					teILFreeClear(&pidl2);
				}
			}
			::SysFreeString(bs);
		}
	}
	if (!pSF) {
		if (!GetShellFolder(&pSF, *ppidl)) {
			GetShellFolder(&pSF, g_pidls[CSIDL_RESULTSFOLDER]);
			teILCloneReplace(ppidl, g_pidls[CSIDL_RESULTSFOLDER]);
			m_dwUnavailable = GetTickCount();
		}
	}
	if (pSF) {
		SafeRelease(&m_pSF2);
		hr = pSF->QueryInterface(IID_PPV_ARGS(&m_pSF2));
		pSF->Release();
	}
	return hr;
}

VOID CteShellBrowser::SetLVSettings()
{
	if (m_hwndLV) {
		if (m_param[SB_ViewFlags] & CDB2GVF_NOSELECTVERB) {
			ListView_SetSelectedColumn(m_hwndLV, -1);
		}
		ListView_SetBkColor(m_hwndLV, m_clrBk);
		ListView_SetTextBkColor(m_hwndLV, m_clrTextBk);
		ListView_SetTextColor(m_hwndLV, m_clrText);
		teSetDarkTheme(m_hwndLV, L"explorer");
	}
	if (m_pTV && m_pTV->m_hwndTV) {
		teSetDarkTheme(m_pTV->m_hwndTV, L"explorer");
	}
}

HRESULT CteShellBrowser::RemoveAll()
{
	HRESULT hr = E_NOTIMPL;
	IFolderView2 *pFV2 = NULL;
	IResultsFolder *pRF;
	m_nSorting = TESORTING;
	m_dwSessionId = MAKELONG(GetTickCount(), rand());
	if (SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV2))) && SUCCEEDED(pFV2->GetFolder(IID_PPV_ARGS(&pRF)))) {
		hr = pRF->RemoveAll();
		pRF->Release();
		pFV2->SetGroupBy(PKEY_Null, TRUE);
		pFV2->SetSortColumns(g_pSortColumnNull, _countof(g_pSortColumnNull));
#ifdef _2000XP
	} else {
		IShellFolderView *pSFV;
		if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pSFV))) {
			UINT ui;
			hr = pSFV->RemoveObject(NULL, &ui);//the removal of all objects
			pSFV->Release();
		}
#endif
	}
	SafeRelease(&pFV2);
	return hr;
}

VOID CteShellBrowser::SetViewModeAndIconSize(BOOL bSetIconSize)
{
	IFolderView2 *pFV2;
	if (m_pShellView) {
		if (bSetIconSize && SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV2)))) {
			FOLDERVIEWMODE uViewMode;
			int iImageSize;
			if (FAILED(pFV2->GetViewModeAndIconSize(&uViewMode, &iImageSize)) || uViewMode != m_param[SB_ViewMode] || iImageSize != m_param[SB_IconSize]) {
				pFV2->SetViewModeAndIconSize(static_cast<FOLDERVIEWMODE>(m_param[SB_ViewMode]), m_param[SB_IconSize]);
			}
			pFV2->Release();
		} else {
			IFolderView *pFV;
			if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV))) {
				pFV->SetCurrentViewMode(m_param[SB_ViewMode]);
				pFV->Release();
			}
		}
	}
}

VOID CteShellBrowser::SetListColumnWidth()
{
	if (m_bSetListColumnWidth) {
		if (m_pSF2 && m_hwndLV && m_pShellView && m_param[SB_ViewMode] == FVM_LIST) {
			m_bSetListColumnWidth = FALSE;
			int nOrgWidth = ListView_GetColumnWidth(m_hwndLV, 0);
			IFolderView *pFV;
			if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV))) {
				int n;
				int nWidth = nOrgWidth - 28;
				if SUCCEEDED(pFV->ItemCount(SVGIO_ALLVIEW, &n)) {
					while (--n >= 0) {
						LPITEMIDLIST pidl;
						if SUCCEEDED(pFV->Item(n, &pidl)) {
							VARIANT v;
							VariantInit(&v);
							if SUCCEEDED(m_pSF2->GetDetailsEx(pidl, &PKEY_ItemName, &v)) {
								if (v.vt == VT_BSTR) {
									int nWidth1 = ListView_GetStringWidth(m_hwndLV, v.bstrVal);
									if (nWidth1 > nWidth) {
										nWidth = nWidth1;
									}
								}
								VariantClear(&v);
							}
							teCoTaskMemFree(pidl);
						}
					}
				}
				nWidth += 28;
				if (nWidth > nOrgWidth) {
					ListView_SetColumnWidth(m_hwndLV, 0, 1);
					ListView_SetColumnWidth(m_hwndLV, 0, nWidth);
					ListView_EnsureVisible(m_hwndLV, ListView_GetNextItem(m_hwndLV, -1, LVNI_ALL | LVNI_FOCUSED), FALSE);
				}
				pFV->Release();
			}
		}
	}
}

VOID CteShellBrowser::AddItem(LPITEMIDLIST pidl)
{
	IFolderView *pFV = NULL;
	LPITEMIDLIST pidlChild = NULL;
    if (!ILIsEmpty(pidl)) {
        HRESULT hr = S_FALSE;
        BSTR bs;
        if SUCCEEDED(teGetDisplayNameFromIDList(&bs, pidl, SHGDN_NORMAL)) {
            if (bs[0]) {
                if (m_ppDispatch[SB_OnIncludeObject]) {
                    VARIANT vResult;
                    VariantInit(&vResult);
                    VARIANTARG *pv = GetNewVARIANT(4);
                    teSetObject(&pv[3], this);
                    teSetSZ(&pv[2], bs);
                    if (teGetDisplayNameFromIDList(&pv[1].bstrVal, pidl, SHGDN_INFOLDER | SHGDN_FORPARSING) == S_OK) {
                        pv[1].vt = VT_BSTR;
                    }
                    teSetIDList(&pv[0], pidl);
                    Invoke4(m_ppDispatch[SB_OnIncludeObject], &vResult, 4, pv);
                    hr = GetIntFromVariantClear(&vResult);
                } else if (m_bsFilter) {
                    if (tePathMatchSpec(bs, m_bsFilter)) {
                        hr = S_OK;
                    } else {
                        BSTR bs2;
                        if (teGetDisplayNameFromIDList(&bs2, pidl, SHGDN_INFOLDER | SHGDN_FORPARSING) == S_OK) {
                            if (tePathMatchSpec(bs2, m_bsFilter)) {
                                hr = S_OK;
                            }
                            ::SysFreeString(bs2);
                        }
                    }
                } else {
                    hr = S_OK;
                }
            }
            teSysFreeString(&bs);
        }
        if (hr == S_OK) {
			m_bRedraw = TRUE;
			SetRedraw(FALSE);
			SetTimer(g_hwndMain, TET_Status, 500, teTimerProc);
            try {
                IResultsFolder *pRF;
                if (SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV))) && SUCCEEDED(pFV->GetFolder(IID_PPV_ARGS(&pRF)))) {
                    BSTR bs2 = NULL;
                    IShellFolder *pSF;
                    LPCITEMIDLIST pidlPart;
                    if SUCCEEDED(SHBindToParent(pidl, IID_PPV_ARGS(&pSF), &pidlPart)) {
                        if SUCCEEDED(teGetDisplayNameBSTR(pSF, pidlPart, SHGDN_FORPARSING, &bs2)) {
                            if (teIsFileSystem(bs2)) {
                                SFGAOF sfAttr = SFGAO_FILESYSTEM | SFGAO_FOLDER;
                                if FAILED(pSF->GetAttributesOf(1, &pidlPart, &sfAttr)) {
                                    sfAttr = 0;
                                }
                                if (!(sfAttr & SFGAO_FILESYSTEM)) {
                                    WIN32_FIND_DATA wfd;
                                    teSHGetDataFromIDList(pSF, pidlPart, SHGDFIL_FINDDATA, &wfd, sizeof(WIN32_FIND_DATA));
                                    teILFreeClear(&pidl);
                                    pidl = teSHSimpleIDListFromPathEx(bs2, sfAttr & SFGAO_FOLDER, (WORD)wfd.dwFileAttributes, wfd.nFileSizeLow, wfd.nFileSizeHigh, wfd.ftLastWriteTime);
                                    m_bRegenerateItems = TRUE;
                                }
                            }
                            teSysFreeString(&bs2);
                        }
                        pSF->Release();
                    }
					pRF->RemoveIDList(pidl);
                    pRF->AddIDList(pidl, &pidlChild);
                    if (FAILED(teGetDisplayNameBSTR(m_pSF2, pidlChild, SHGDN_NORMAL, &bs2)) || ::SysStringLen(bs2) == 0) {
                        pRF->RemoveIDList(pidl);
                        hr = E_FAIL;
                    }
                    teSysFreeString(&bs2);
                    pRF->Release();
#ifdef _2000XP
                } else if (ILIsEqual(m_pidl, g_pidls[CSIDL_RESULTSFOLDER])) {
                    IShellFolderView *pSFV;
                    if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pSFV))) {
                        pidlChild = teILCreateResultsXP(pidl);
                        UINT ui;
						pSFV->RemoveObject(pidlChild, &ui);
                        pSFV->AddObject(pidlChild, &ui);
                        pSFV->Release();
                    }
#endif
                }
            } catch (...) {
                g_nException = 0;
#ifdef _DEBUG
                g_strException = L"AddItem";
#endif
            }
        }
        SafeRelease(&pFV);
		teCoTaskMemFree(pidlChild);
    }
	teCoTaskMemFree(pidl);
}

#ifndef _USE_THREADADDITEMS
VOID CteShellBrowser::AddItems(IDispatch *pdisp, BOOL bDeleted, IDispatch *pOnCompleted, BOOL bNavigateComplete)
{
	WCHAR pszMsg[MAX_PATH];
	LPITEMIDLIST pidl;
	IProgressDialog *ppd = NULL;

	teCreateInstance(CLSID_ProgressDialog, NULL, NULL, IID_PPV_ARGS(&ppd));
	if (ppd) {
		try {
			ppd->StartProgressDialog(g_hwndMain, NULL, PROGDLG_NORMAL | PROGDLG_AUTOTIME, NULL);
#ifdef _2000XP
			ppd->SetAnimation(g_hShell32, 150);
#endif
			if (!LoadString(g_hShell32, 13585, pszMsg, MAX_PATH)) {
				LoadString(g_hShell32, 6478, pszMsg, MAX_PATH);
			}
			ppd->SetLine(1, pszMsg, TRUE, NULL);

			FolderItems *pItems;
			VARIANT v;
			VariantInit(&v);
			if SUCCEEDED(pdisp->QueryInterface(IID_PPV_ARGS(&pItems))) {
				long lCount = 0;
				pItems->get_Count(&lCount);
				if (lCount) {
					v.vt = VT_I4;
					for (v.lVal = 0; v.lVal < lCount && !ppd->HasUserCancelled(); v.lVal++) {
						teSetProgress(ppd, v.lVal, lCount);
						FolderItem *pItem;
						if SUCCEEDED(pItems->Item(v, &pItem)) {
							BSTR bs;
							if SUCCEEDED(pItem->get_Name(&bs)) {
								ppd->SetLine(2, bs, TRUE, NULL);
								teSysFreeString(&bs);
							}
							BOOL bAdd = TRUE;
							if (!bDeleted) {
								CteFolderItem *pid;
								if SUCCEEDED(pItem->QueryInterface(g_ClsIdFI, (LPVOID *)&pid)) {
									bAdd = !pid->m_dwUnavailable;
									pid->Release();
								}
							}
							if (bAdd && teGetIDListFromObject(pItem, &pidl)) {
								AddItem(pidl);
							}
							SafeRelease(&pItem);
						}
					}
				}
				pItems->Release();
			} else {
				int nCount = teGetObjectLength(pdisp);
				for (int nCurrent = 0; nCurrent < nCount && !ppd->HasUserCancelled(); nCurrent++) {
					teSetProgress(ppd, nCurrent, nCount);
					if (SUCCEEDED(teGetPropertyAt(pdisp, nCurrent, &v)) && v.vt != VT_EMPTY) {
						if (!teGetIDListFromVariant(&pidl, &v) && bDeleted && v.vt == VT_BSTR) {
							pidl = teSHSimpleIDListFromPath(v.bstrVal, FALSE, FILE_ATTRIBUTE_HIDDEN);
						}
						if (pidl) {
							if (SHGetPathFromIDList(pidl, pszMsg)) {
								ppd->SetLine(2, PathFindFileName(pszMsg), TRUE, NULL);
							}
							VariantClear(&v);
							AddItem(pidl);
						}
					}
				}
			}
			teSetProgress(ppd, 100, 100);
			ppd->SetLine(2, L"", TRUE, NULL);

			if (bNavigateComplete) {
				Invoke5(this, 0x10000500, DISPATCH_METHOD, NULL, 0, NULL);
			}
			if (pOnCompleted) {
				VARIANT *pv = GetNewVARIANT(3);
				teSetObject(&pv[2], this);
				teSetObject(&pv[1], pdisp);
				teSetObjectRelease(&pv[0], new CteProgressDialog(ppd));
				Invoke4(pOnCompleted, NULL, 3, pv);
			}
			SetTimer(g_hwndMain, TET_Status, 500, teTimerProc);
		} catch (...) {
			g_nException = 0;
#ifdef _DEBUG
			g_strException = L"threadAddItems";
#endif
		}
	}
	SafeRelease(&pdisp);
	if (ppd) {
		ppd->SetLine(2, L"", TRUE, NULL);
		ppd->StopProgressDialog();
		SafeRelease(&ppd);
	}
}
#endif

VOID CteShellBrowser::NavigateComplete(BOOL bBeginNavigate)
{
	if (!bBeginNavigate || DoFunc(TE_OnBeginNavigate, this, S_OK) != S_FALSE) {
		DoFunc(TE_OnNavigateComplete, this, E_NOTIMPL);
	}
}

VOID CteShellBrowser::InitFilter()
{
	teSysFreeString(&m_bsFilter);
	SafeRelease(&m_ppDispatch[SB_OnIncludeObject]);
	if (m_bsNextFilter) {
		m_bsFilter = m_bsNextFilter;
		m_bsNextFilter = NULL;
		if (::SysStringLen(m_bsFilter)) {
			DoFunc(TE_OnFilterChanged, this, S_OK);
		} else {
			teSysFreeString(&m_bsFilter);
		}
	}
}

STDMETHODIMP CteShellBrowser::OnNavigationFailed(PCIDLIST_ABSOLUTE pidlFolder)
{
	if (m_nSuspendMode) {
		Suspend(m_nSuspendMode);
	}
	return S_OK;
}

//IExplorerPaneVisibility
STDMETHODIMP CteShellBrowser::GetPaneState(REFEXPLORERPANE ep, EXPLORERPANESTATE *peps)
{
	if (g_pOnFunc[TE_OnGetPaneState]) {
		VARIANTARG *pv = GetNewVARIANT(3);
		teSetObject(&pv[2], this);
		pv[1].vt = VT_BSTR;
		pv[1].bstrVal = ::SysAllocStringLen(NULL, 38);
		StringFromGUID2(ep, pv[1].bstrVal, 39);
		teSetObjectRelease(&pv[0], new CteMemory(sizeof(int), peps, 1, L"DWORD"));
		VARIANT vResult;
		VariantInit(&vResult);
		Invoke4(g_pOnFunc[TE_OnGetPaneState], &vResult, 3, pv);
		if (vResult.vt != VT_EMPTY) {
			return GetIntFromVariantClear(&vResult);
		}
	}
	return E_NOTIMPL;
}

#ifdef _2000XP
//IShellFolder
STDMETHODIMP CteShellBrowser::ParseDisplayName(HWND hwnd, IBindCtx *pbc, LPWSTR pszDisplayName, ULONG *pchEaten, PIDLIST_RELATIVE *ppidl, ULONG *pdwAttributes)
{
	return m_pSF2->ParseDisplayName(hwnd, pbc, pszDisplayName, pchEaten, ppidl, pdwAttributes);
}

STDMETHODIMP CteShellBrowser::EnumObjects(HWND hwndOwner, SHCONTF grfFlags, IEnumIDList **ppenumIDList)
{
	return m_pSF2->EnumObjects(hwndOwner, grfFlags, ppenumIDList);
}

STDMETHODIMP CteShellBrowser::BindToObject(PCUIDLIST_RELATIVE pidl, IBindCtx *pbc, REFIID riid, void **ppvOut)
{
	return m_pSF2->BindToObject(pidl, pbc, riid, ppvOut);
}

STDMETHODIMP CteShellBrowser::BindToStorage(PCUIDLIST_RELATIVE pidl, IBindCtx *pbc, REFIID riid, void **ppvOut)
{
	return m_pSF2->BindToStorage(pidl, pbc, riid, ppvOut);
}

STDMETHODIMP CteShellBrowser::CompareIDs(LPARAM lParam, PCUIDLIST_RELATIVE pidl1, PCUIDLIST_RELATIVE pidl2)
{
	return m_pSF2->CompareIDs(lParam, pidl1, pidl2);
}

STDMETHODIMP CteShellBrowser::CreateViewObject(HWND hwndOwner, REFIID riid, void **ppv)
{
	if (!g_bUpperVista && IsEqualIID(riid, IID_IShellView)) {
		//only XP
		SafeRelease(&m_pSFVCB);
		IShellView *pSV;
		SFV_CREATE sfvc;
		sfvc.cbSize = sizeof(SFV_CREATE);
		sfvc.pshf = static_cast<IShellFolder *>(this);
		sfvc.psfvcb = static_cast<IShellFolderViewCB *>(this);
		sfvc.psvOuter = NULL;
		if SUCCEEDED(m_pSF2->CreateViewObject(hwndOwner, IID_PPV_ARGS(&pSV))) {
			IShellFolderView *pSFV;
			if (pSV->QueryInterface(IID_PPV_ARGS(&pSFV)) == S_OK) {
				pSFV->SetCallback(NULL, &m_pSFVCB);
				pSFV->Release();
			}
			if (m_pSFVCB) {
				pSV->Release();
			} else {
				*ppv = pSV;
				return S_OK;
			}
		}
		return SHCreateShellFolderView(&sfvc, (IShellView **)ppv);
	}
	return m_pSF2->CreateViewObject(hwndOwner, riid, ppv);
}

STDMETHODIMP CteShellBrowser::GetAttributesOf(UINT cidl, PCUITEMID_CHILD_ARRAY apidl, SFGAOF *rgfInOut)
{
	return m_pSF2->GetAttributesOf(cidl, apidl, rgfInOut);
}

STDMETHODIMP CteShellBrowser::GetUIObjectOf(HWND hwndOwner, UINT cidl, PCUITEMID_CHILD_ARRAY apidl, REFIID riid, UINT *rgfReserved, void **ppv)
{
	return m_pSF2->GetUIObjectOf(hwndOwner, cidl, apidl, riid, rgfReserved, ppv);
}

STDMETHODIMP CteShellBrowser::GetDisplayNameOf(PCUITEMID_CHILD pidl, SHGDNF uFlags, STRRET *pName)
{
	if (m_pSF2) {
		return m_pSF2->GetDisplayNameOf(pidl, uFlags, pName);
	}
	return E_ACCESSDENIED;
}

STDMETHODIMP CteShellBrowser::SetNameOf(HWND hwndOwner, PCUITEMID_CHILD pidl, LPCWSTR pszName, SHGDNF uFlags, PITEMID_CHILD *ppidlOut)
{
	return m_pSF2->SetNameOf(hwndOwner, pidl, pszName, uFlags, ppidlOut);
}

//IShellFolder2
STDMETHODIMP CteShellBrowser::GetDefaultSearchGUID(GUID *pguid)
{
	return m_pSF2->GetDefaultSearchGUID(pguid);
}

STDMETHODIMP CteShellBrowser::EnumSearches(IEnumExtraSearch **ppEnum)
{
	return m_pSF2->EnumSearches(ppEnum);
}

STDMETHODIMP CteShellBrowser::GetDefaultColumn(DWORD dwReserved, ULONG *pSort, ULONG *pDisplay)
{
	return m_pSF2->GetDefaultColumn(dwReserved, pSort, pDisplay);
}

STDMETHODIMP CteShellBrowser::GetDefaultColumnState(UINT iColumn, SHCOLSTATEF *pcsFlags)
{
	if (m_nColumns) {
		if (iColumn < m_nColumns) {
			*pcsFlags = m_pColumns[iColumn].csFlags;
			return S_OK;
		}
	}
	return m_pSF2->GetDefaultColumnState(iColumn, pcsFlags);
}

STDMETHODIMP CteShellBrowser::GetDetailsEx(PCUITEMID_CHILD pidl, const SHCOLUMNID *pscid, VARIANT *pv)
{
	int iColumn = -1;
	if (m_nColumns > 1 && IsEqualPropertyKey(*pscid, PKEY_TotalFileSize)) {
		iColumn = m_nColumns - 1;
	} else if (m_nFolderName != MAXINT && IsEqualPropertyKey(*pscid, PKEY_ItemFolderNameDisplay)) {
		iColumn = m_nFolderName;
	}
	if (iColumn < 0) {
		return m_pSF2->GetDetailsEx(pidl, pscid, pv);
	}
	SHELLDETAILS sd;
	HRESULT hr = GetDetailsOf(pidl, iColumn, &sd);
	if SUCCEEDED(hr) {
		hr = StrRetToBSTR(&sd.str, pidl, &pv->bstrVal);
		if SUCCEEDED(hr) {
			pv->vt = VT_BSTR;
		}
	}
	return hr;
}

STDMETHODIMP CteShellBrowser::GetDetailsOf(PCUITEMID_CHILD pidl, UINT iColumn, SHELLDETAILS *psd)
{
	HRESULT hr;
	if (iColumn < m_nColumns - 2 || iColumn >= m_nColumns) {
		if (pidl && iColumn == m_nFolderName && SUCCEEDED(m_pSF2->GetDisplayNameOf(pidl, SHGDN_FORADDRESSBAR | SHGDN_FORPARSING, &psd->str))) {
			PathRemoveFileSpec(psd->str.pOleStr);
			hr = S_OK;
		} else {
			hr = m_pSF2->GetDetailsOf(pidl, iColumn, psd);
		}
	} else {
		WCHAR szBuf[MAX_COLUMN_NAME_LEN];
		szBuf[0] = NULL;
		if (pidl) {
			if (iColumn == m_nColumns - 1) {
				m_nFolderSizeIndex = MAXINT - 1;
				SetFolderSize(m_pSF2, pidl, szBuf, _countof(szBuf));
			} else {
				SetLabel(pidl, szBuf, MAX_COLUMN_NAME_LEN);
			}
		} else {
			if (iColumn == m_nColumns - 1) {
				lstrcpy(szBuf, g_szTotalFileSizeXP);
				psd->fmt = LVCFMT_RIGHT;
				psd->cxChar = 16;
			} else {
				lstrcpy(szBuf, g_szLabelXP);
				psd->fmt = LVCFMT_LEFT;
				psd->cxChar = 20;
			}
		}
		psd->str.uType = STRRET_WSTR;
		psd->str.pOleStr = (LPOLESTR)::CoTaskMemAlloc((lstrlen(szBuf) + 1) * sizeof(WCHAR));
		lstrcpy(psd->str.pOleStr, szBuf);
		hr = S_OK;
	}
	if (m_nColumns) {
		if (iColumn < m_nColumns) {
			if (m_pColumns[iColumn].nWidth >= 0 && m_pColumns[iColumn].nWidth < 32768) {
				psd->cxChar = m_pColumns[iColumn].nWidth / g_nCharWidth;
			}
		}
	}
	return hr;
}

STDMETHODIMP CteShellBrowser::MapColumnToSCID(UINT iColumn, SHCOLUMNID *pscid)
{
	if (iColumn == m_nColumns - 1) {
		*pscid = PKEY_TotalFileSize;
		return S_OK;
	}
	if (iColumn == m_nColumns - 2) {
		*pscid = PKEY_Contact_Label;
		return S_OK;
	}
	return m_pSF2->MapColumnToSCID(iColumn, pscid);
}

//IShellFolderViewCB
STDMETHODIMP CteShellBrowser::MessageSFVCB(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg != 54 && uMsg != 78 && m_pSFVCB) {
		return m_pSFVCB->MessageSFVCB(uMsg, wParam, lParam);
	}
	return E_NOTIMPL;
}
#endif

//IPersist
STDMETHODIMP CteShellBrowser::GetClassID(CLSID *pClassID)
{
	IPersist *pPersist;
	HRESULT hr = m_pSF2->QueryInterface(IID_PPV_ARGS(&pPersist));
	if SUCCEEDED(hr) {
		hr = pPersist->GetClassID(pClassID);
		pPersist->Release();
	}
	return hr;
}

//IPersistFolder
STDMETHODIMP CteShellBrowser::Initialize(PCIDLIST_ABSOLUTE pidl)
{
	return BrowseObject(pidl, SBSP_ABSOLUTE | SBSP_SAMEBROWSER);
}

//IPersistFolder2
STDMETHODIMP CteShellBrowser::GetCurFolder(PIDLIST_ABSOLUTE *ppidl)
{
	return teGetIDListFromObject(m_pFolderItem, ppidl) ? S_OK : E_FAIL;
}

//

VOID CteShellBrowser::Suspend(int nMode)
{
	BOOL bVisible = m_bVisible;
	if (nMode == 2) {
		if (m_dwUnavailable || ILIsEqual(m_pidl, g_pidls[CSIDL_RESULTSFOLDER])) {
			return;
		}
		Show(FALSE, 0);
		m_dwUnavailable = GetTickCount();
		teILCloneReplace(&m_pidl, g_pidls[CSIDL_RESULTSFOLDER]);
	}
	if (!m_dwUnavailable) {
		SaveFocusedItemToHistory();
		teDoCommand(this, m_hwnd, WM_NULL, 0, 0);//Save folder setings
	}
	teSysFreeString(&m_bsNextFilter);
	m_bsNextFilter = m_bsFilter;
	m_bsFilter = NULL;
	DestroyView(0);
	if (nMode == 4) {
		m_nUnload = 8;
	} else {
		m_nUnload = 9;
		if (nMode == 1 && m_pTV) {
			m_pTV->Close();
			m_pTV->m_bSetRoot = TRUE;
		}
		if (nMode != 2) {
			CteFolderItem *pid1 = NULL;
			if SUCCEEDED(m_pFolderItem->QueryInterface(g_ClsIdFI, (LPVOID *)&pid1)) {
				if (pid1->m_v.vt == VT_BSTR) {
					teILFreeClear(&m_pidl);
					pid1->Clear();
					pid1->Release();
				}
			}
		}
		if (m_pidl) {
			VARIANT v;
			if (GetVarPathFromFolderItem(m_pFolderItem, &v)) {
				CteFolderItem *pid1;
				teQueryFolderItem(&m_pFolderItem, &pid1);
				if (nMode == 2) {
					pid1->MakeUnavailable();
					m_nUnload = 8;
				} else {
					pid1->Clear();
					VariantClear(&pid1->m_v);
					VariantCopy(&pid1->m_v, &v);
				}
				pid1->Release();
				teILFreeClear(&m_pidl);
				VariantClear(&v);
			}
		}
	}
	Show(bVisible, 0);
}

VOID CteShellBrowser::SetPropEx()
{
	if (m_pShellView->GetWindow(&m_hwndDV) == S_OK) {
		if (!m_DefProc) {
			SetWindowLongPtr(m_hwndDV, GWLP_USERDATA, (LONG_PTR)this);
			m_DefProc = SetWindowLongPtr(m_hwndDV, GWLP_WNDPROC, (LONG_PTR)TELVProc);
			for (int i = WM_USER + 173; i <= WM_USER + 175; i++) {
				teChangeWindowMessageFilterEx(m_hwndDV, i, MSGFLT_ALLOW, NULL);
			}
		}
		m_hwndLV = FindWindowExA(m_hwndDV, 0, WC_LISTVIEWA, NULL);
		m_hwndDT = m_hwndLV;
		if (m_pExplorerBrowser) {
			if (m_param[SB_Type] != CTRL_EB || m_param[SB_FolderFlags] & FWF_NOCLIENTEDGE) {
				SetWindowLong(m_hwnd, GWL_EXSTYLE, GetWindowLong(m_hwnd, GWL_EXSTYLE) & ~WS_EX_CLIENTEDGE);
			}
			if (m_param[SB_Type] <= CTRL_SB) {
				if (m_param[SB_FolderFlags] & FWF_NOCLIENTEDGE) {
					SetWindowLong(m_hwnd, GWL_STYLE, GetWindowLong(m_hwnd, GWL_STYLE) & ~WS_BORDER);
				} else {
					SetWindowLong(m_hwnd, GWL_STYLE, GetWindowLong(m_hwnd, GWL_STYLE) | WS_BORDER);
				}
			}
		}
		if (m_hwndLV) {
			SetWindowLong(m_hwndLV, GWL_STYLE, GetWindowLong(m_hwndLV, GWL_STYLE) & ~WS_BORDER);
			if (m_param[SB_Type] <= CTRL_SB || m_param[SB_FolderFlags] & FWF_NOCLIENTEDGE) {
				SetWindowLong(m_hwndLV, GWL_EXSTYLE, GetWindowLong(m_hwndLV, GWL_EXSTYLE) & ~WS_EX_CLIENTEDGE);
			} else {
				SetWindowLong(m_hwndLV, GWL_EXSTYLE, GetWindowLong(m_hwndLV, GWL_EXSTYLE) | WS_EX_CLIENTEDGE);
			}
			SendMessage(m_hwndLV, WM_CHANGEUISTATE, MAKELONG(UIS_SET, UISF_HIDEFOCUS), 0);
		} else {
			m_hwndDT = FindWindowExA(m_hwndDV, NULL, "DirectUIHWND", NULL);
		}
		if (!m_pDropTarget2) {
			m_pDropTarget2 = new CteDropTarget2(NULL, static_cast<IDispatch *>(this));
			teRegisterDragDrop(m_hwndDT, m_pDropTarget2, &m_pDropTarget2->m_pDropTarget);
		}
		m_hwndAlt = NULL;
	}
}

VOID CteShellBrowser::ResetPropEx()
{
	if (m_DefProc) {
		SetWindowLongPtr(m_hwndDV, GWLP_WNDPROC, (LONG_PTR)m_DefProc);
		m_DefProc = NULL;
	}
	if (m_pDropTarget2) {
//		SetProp(m_hwndDT, L"OleDropTargetInterface", (HANDLE)m_pDropTarget2->m_pDropTarget);
		RevokeDragDrop(m_hwndDT);
		RegisterDragDrop(m_hwndDT, m_pDropTarget2->m_pDropTarget);
		SafeRelease(&m_pDropTarget2);
	}
}

void CteShellBrowser::Show(BOOL bShow, DWORD dwOptions)
{
	bShow &= 1;
	if (bShow ^ m_bVisible) {
		if (bShow) {
			if (!g_nLockUpdate && !m_nCreate) {
				if (m_nUnload & 8) {
					if (m_nUnload & 1) {
						CteFolderItem *pid;
						if SUCCEEDED(m_pFolderItem->QueryInterface(g_ClsIdFI, (LPVOID *)&pid)) {
							if (pid->m_v.vt == VT_BSTR) {
								pid->Clear();
							}
							pid->Release();
						}
					}
					m_nUnload = 2;
					if SUCCEEDED(BrowseObject(NULL, SBSP_RELATIVE | SBSP_WRITENOHISTORY | SBSP_REDIRECT)) {
						m_nUnload = 0;
					}
				} else if (m_nUnload == 1 || !m_pShellView) {
					m_nUnload = 2;
					if SUCCEEDED(BrowseObject(NULL, SBSP_RELATIVE | SBSP_SAMEBROWSER)) {
						m_nUnload = 0;
					}
				} else if (m_bRefreshLator) {
					Refresh(TRUE);
				}
			}
		}
		if (m_pShellView) {
			m_bVisible = bShow;
			if (bShow) {
				ShowWindow(m_hwnd, SW_SHOWNA);
				if (m_hwndAlt) {
					ShowWindow(m_hwndAlt, SW_SHOWNA);
				} else if (m_hwndLV) {
					ShowWindow(m_hwndLV, SW_SHOWNA);
				}
				SetRedraw(TRUE);
				if (m_param[SB_TreeAlign] & 2) {
					m_pTV->Show();
					ShowWindow(m_pTV->m_hwnd, SW_SHOWNA);
					BringWindowToTop(m_pTV->m_hwnd);
				}
				if (!SetActive(FALSE)) {
					m_pShellView->UIActivate(SVUIA_ACTIVATE_NOFOCUS);
				}
				BringWindowToTop(m_hwnd);
				ArrangeWindow();
				if (m_nUnload == 4) {
					Refresh(FALSE);
				}
			} else if ((dwOptions & 2) == 0) {
				SetRedraw(FALSE);
				m_pShellView->UIActivate(SVUIA_DEACTIVATE);
				MoveWindow(m_hwnd, -1, -1, 0, 0, FALSE);
				ShowWindow(m_hwnd, SW_HIDE);
				if (m_hwndAlt) {
					ShowWindow(m_hwndAlt, SW_HIDE);
				}
				if (m_hwndLV) {
					ShowWindow(m_hwndLV, SW_HIDE);
				}
				if (m_pTV->m_hwnd) {
					MoveWindow(m_pTV->m_hwnd, -1, -1, 0, 0, FALSE);
					ShowWindow(m_pTV->m_hwnd, SW_HIDE);
				}
				if ((dwOptions & 1) && m_dwUnavailable && !m_nCreate && !ILIsEqual(m_pidl, g_pidls[CSIDL_RESULTSFOLDER])) {
					Suspend(0);
				}
			}
		} else {
			m_bVisible = FALSE;
		}
	}
}

BOOL CteShellBrowser::Close(BOOL bForce)
{
	if (!m_bEmpty && (CanClose(this) || bForce)) {
		int i = GetTabIndex();
		m_bEmpty = TRUE;
		if (i >= 0)	{
			SendMessage(m_pTC->m_hwnd, TCM_DELETEITEM, i, 0);
		}
		ShowWindow(m_pTV->m_hwnd, SW_HIDE);
		Clear();
		return TRUE;
	}
	return FALSE;
}

VOID CteShellBrowser::DestroyView(int nFlags)
{
	KillTimer(m_hwnd, (UINT_PTR)this);
	ResetPropEx();
	BOOL bCloseSB = TRUE;
	if ((nFlags & 2) == 0 && m_pExplorerBrowser) {
		try {
			bCloseSB = FALSE;
			if (m_dwEventCookie) {
			  m_pExplorerBrowser->Unadvise(m_dwEventCookie);
			}
			IUnknown_SetSite(m_pExplorerBrowser, NULL);
			Show(FALSE, 0);
			m_pExplorerBrowser->Destroy();
			m_pExplorerBrowser->Release();
		} catch (...) {
			g_nException = 0;
#ifdef _DEBUG
			g_strException = L"DestroyView";
#endif
		}
		m_pExplorerBrowser = NULL;
	}
	if (m_pShellView) {
		try {
			if (bCloseSB && (nFlags & 1) == 0) {
				Show(FALSE, 0);
				m_pShellView->DestroyViewWindow();
			}
			if (nFlags == 0) {
				SafeRelease(&m_pShellView);
			}
		} catch (...) {
			g_nException = 0;
#ifdef _DEBUG
			g_strException = L"DestroyView/2";
#endif
		}
	}
}

VOID CteShellBrowser::GetSort(BSTR* pbs, int nFormat)
{
	*pbs = NULL;
	if (!m_pShellView || m_dwUnavailable) {
		return;
	}
	if (m_bsAltSortColumn) {
		*pbs = ::SysAllocString(m_bsAltSortColumn);
		return;
	}
	IFolderView2 *pFV2;
	if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV2))) {
		if (teIsSameSort(pFV2, g_pSortColumnNull, _countof(g_pSortColumnNull))) {
			*pbs = ::SysAllocString(L"System.Null");
		} else {
			SORTCOLUMN pCol[1];
			if SUCCEEDED(pFV2->GetSortColumns(pCol, _countof(pCol))) {
				*pbs = tePSGetNameFromPropertyKeyEx(pCol[0].propkey, nFormat, m_pShellView);
				if (pCol[0].direction < 0) {
					ToMinus(pbs);
				}
			}
		}
		pFV2->Release();
		return;
	}
#ifdef _2000XP
	LPARAM Sort = 0;
	IShellFolderView *pSFV;
	if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pSFV))) {
		if SUCCEEDED(pSFV->GetArrangeParam(&Sort)) {//Sort column
			Sort = LOWORD(Sort);
		}
		pSFV->Release();
	}
	SHCOLUMNID scid;
	if SUCCEEDED(MapColumnToSCID((UINT)Sort, &scid)) {
		*pbs = tePSGetNameFromPropertyKeyEx(scid, nFormat, NULL);
		HWND hHeader = ListView_GetHeader(m_hwndLV);
		if (hHeader) {
			int nCount = Header_GetItemCount(hHeader);
			for (int i = 0; i < nCount; i++) {
#ifdef _W2000
				HD_ITEM hdi = { HDI_FORMAT | HDI_BITMAP };
#else
				HD_ITEM hdi = { HDI_FORMAT };
#endif
				Header_GetItem(hHeader, i, &hdi);
				if (hdi.fmt & (HDF_SORTUP | HDF_SORTDOWN | HDF_BITMAP)) {
					if (hdi.fmt & HDF_SORTDOWN) {
						ToMinus(pbs);
					}
#ifdef _W2000
					if (hdi.fmt & HDF_BITMAP) {	//Windows 2000
						if (!(hdi.fmt & (HDF_SORTUP | HDF_SORTDOWN))) {
							HDC hdc = GetDC(hHeader);
							HDC hCompatDC = CreateCompatibleDC(hdc);
							ReleaseDC(hHeader, hdc);
							HGDIOBJ hDefault = SelectObject(hCompatDC, hdi.hbm);
							if (GetPixel(hCompatDC, 3, 4) != GetSysColor(COLOR_BTNFACE)) {
								ToMinus(pbs);
							}
							SelectObject(hCompatDC, hDefault);
							DeleteDC(hCompatDC);
							ReleaseDC(hHeader, hdc);
						}
						DeleteObject(hdi.hbm);
					}
#endif
					break;
				}
			}
		}
	}
#endif
}

VOID CteShellBrowser::GetGroupBy(BSTR* pbs)
{
	*pbs = NULL;
	if (!m_pShellView || m_dwUnavailable) {
		return;
	}
	IFolderView2 *pFV2;
	if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV2))) {
		PROPERTYKEY propKey;
		BOOL fAscending;
		if SUCCEEDED(pFV2->GetGroupBy(&propKey, &fAscending)) {
			*pbs = tePSGetNameFromPropertyKeyEx(propKey, 1, NULL);
			if (!fAscending) {
				ToMinus(pbs);
			}
		}
		pFV2->Release();
		return;
	}
#ifdef _2000XP
	if (ListView_IsGroupViewEnabled(m_hwndLV)) {
		IContextMenu *pCM;
		if SUCCEEDED(m_pShellView->GetItemObject(SVGIO_BACKGROUND, IID_PPV_ARGS(&pCM))) {
			HMENU hMenu = CreatePopupMenu();
			pCM->QueryContextMenu(hMenu, 0, 1, 0x7fff, CMF_DEFAULTONLY);
			WCHAR szName[MAX_COLUMN_NAME_LEN];
			MENUITEMINFO mii = { sizeof(MENUITEMINFO), MIIM_SUBMENU };
			GetMenuItemInfo(hMenu, 2, TRUE, &mii);
			int wID = mii.wID;
			HMENU hSubMenu = mii.hSubMenu;
			int nCount = GetMenuItemCount(hSubMenu);
			mii.fMask = MIIM_STATE | MIIM_STRING;
			mii.dwTypeData = szName;
			mii.cch = MAX_COLUMN_NAME_LEN;
			for (int i = 0; i < nCount; i++) {
				GetMenuItemInfo(hSubMenu, i, TRUE, &mii);
				if (mii.fState & MFS_CHECKED) {
					teStripAmp(szName);
					*pbs = ::SysAllocString(szName);
					break;
				}
			}
			DestroyMenu(hMenu);
			pCM->Release();
		}
		return;
	}
	*pbs = ::SysAllocString(L"System.Null");
#endif
}

HRESULT CteShellBrowser::PropertyKeyFromName(BSTR bs, PROPERTYKEY *pkey)
{
	HRESULT hr = lpfnPSPropertyKeyFromStringEx(bs, pkey);
	if (m_pSF2) {
		SHELLDETAILS sd;
		for (UINT i = 0; hr != S_OK && i < MAX_COLUMNS && 
#ifndef _2000XP
				m_pSF2->
#endif
				GetDetailsOf(NULL, i, &sd) == S_OK; i++) {
			BSTR bs1;
			if SUCCEEDED(StrRetToBSTR(&sd.str, NULL, &bs1)) {
				if (teStrSameIFree(bs1, bs)) {
					hr =
#ifndef _2000XP
						m_pSF2->
#endif
						MapColumnToSCID(i, pkey);
				}
			}
		}
	}
	return hr;
}

FOLDERVIEWOPTIONS CteShellBrowser::teGetFolderViewOptions(LPITEMIDLIST pidl, UINT uViewMode)
{
	if (g_bCanLayout && !(m_param[SB_FolderFlags] & FWF_CHECKSELECT)) {
		UINT uFlag = 1;
		while (--uViewMode > 0) {
			uFlag *= 2;
		}
		if (uFlag & g_param[TE_Layout]) {
			return FVO_DEFAULT;
		}
	}
	return FVO_VISTALAYOUT;
}

VOID CteShellBrowser::SetSort(BSTR bs)
{
	if (!m_pShellView || m_dwUnavailable || lstrlen(bs) == 0) {
		return;
	}
	teSysFreeString(&m_bsAltSortColumn);
	if (g_pOnFunc[TE_OnSorting]) {
		VARIANT v;
		VariantInit(&v);
		VARIANTARG *pv = GetNewVARIANT(2);
		teSetObject(&pv[1], this);
		teSetSZ(&pv[0], bs);
		Invoke4(g_pOnFunc[TE_OnSorting], &v, 2, pv);
		if (GetIntFromVariantClear(&v)) {
			m_bsAltSortColumn = ::SysAllocString(bs);
			return;
		}
	}
	IFolderView2 *pFV2;
	SORTCOLUMN pCol1[1];
	SORTCOLUMN *pCol = pCol1;
	int nSortColumns = _countof(pCol1);
	int dir = (bs[0] == '-') ? 1 : 0;
	LPWSTR szNew = &bs[dir];
	if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV2))) {
		if SUCCEEDED(PropertyKeyFromName(szNew, &pCol1[0].propkey)) {
			pCol1[0].direction = dir ? SORT_DESCENDING : SORT_ASCENDING;
			if (IsEqualPropertyKey(pCol1[0].propkey, PKEY_Null)) {
				pCol = g_pSortColumnNull;
				nSortColumns = _countof(g_pSortColumnNull);
			}
			if (!teIsSameSort(pFV2, pCol, nSortColumns)) {
				m_nFocusItem = -1;
				m_nSorting = TESORTING * 10;
				pFV2->SetSortColumns(pCol, nSortColumns);
			}
		}
		pFV2->Release();
		return;
	}
#ifdef _2000XP
	BOOL bGroup = ListView_IsGroupViewEnabled(m_hwndLV);
	if (bGroup) {
		SendMessage(m_hwndDV, WM_COMMAND, CommandID_GROUP, 0);
	}
	BSTR bsName = NULL;
	PROPERTYKEY propKey;
	if SUCCEEDED(lpfnPSPropertyKeyFromStringEx(szNew, &propKey)) {
		bsName = tePSGetNameFromPropertyKeyEx(propKey, 0, m_pShellView);
		szNew = bsName;
	}
	BSTR bs1;
	GetSort(&bs1, 0);
	int dir1 = 0;
	LPWSTR szOld = NULL;
	if (bs1) {
		if (bs1[0] == '-') {
			dir1 = 1;
		}
		szOld = &bs1[dir1];
	}
	if (dir != dir1 || lstrcmpi(szNew, szOld)) {
		IShellFolderView *pSFV;
		if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pSFV))) {
			int i = PSGetColumnIndexXP(szNew, NULL);
			if (i >= 0) {
				pSFV->Rearrange(i);
				if (dir && lstrcmpi(szNew, szOld)) {
					pSFV->Rearrange(i);
				}
			}
			pSFV->Release();
		}
		::SysFreeString(bs1);
		teSysFreeString(&bsName);
	}
	if (bGroup) {
		SendMessage(m_hwndDV, WM_COMMAND, CommandID_GROUP, 0);
	}
#endif
}

VOID CteShellBrowser::SetGroupBy(BSTR bs)
{
	if (!m_pShellView || m_dwUnavailable || lstrlen(bs) == 0) {
		return;
	}
	BSTR bs0;
	GetGroupBy(&bs0);
	if (teStrSameIFree(bs0, bs)) {
		return;
	}
	IFolderView2 *pFV2;
	PROPERTYKEY propKey;
	int dir = (bs[0] == '-') ? 1 : 0;
	LPWSTR szNew = &bs[dir];
	if SUCCEEDED(m_pShellView->QueryInterface(IID_PPV_ARGS(&pFV2))) {
		if SUCCEEDED(PropertyKeyFromName(szNew, &propKey)) {
			if (m_nSorting) {
				m_nSorting--;
				TEGroupBy *pGB = new TEGroupBy[1];
				pGB->bs = ::SysAllocString(bs);
				pGB->dwSessionId = m_dwSessionId;
				SetTimer(m_hwnd, (UINT_PTR)pGB, 100, teTimerProcGroupBy);
			} else {
				pFV2->SetGroupBy(propKey, !dir);
			}
		}
		pFV2->Release();
		return;
	}
#ifdef _2000XP
	if (szNew[0] && lstrcmpi(szNew, L"System.Null")) {
		if (!ListView_IsGroupViewEnabled(m_hwndLV)) {
			SendMessage(m_hwndDV, WM_COMMAND, CommandID_GROUP, 0);
		}
		IContextMenu *pCM;
		if SUCCEEDED(m_pShellView->GetItemObject(SVGIO_BACKGROUND, IID_PPV_ARGS(&pCM))) {
			HMENU hMenu = CreatePopupMenu();
			pCM->QueryContextMenu(hMenu, 0, 1, 0x7fff, CMF_DEFAULTONLY);
			MENUITEMINFO mii = { sizeof(MENUITEMINFO), MIIM_SUBMENU | MIIM_ID };
			GetMenuItemInfo(hMenu, 2, TRUE, &mii);
			int wID = mii.wID;
			HMENU hSubMenu = mii.hSubMenu;
			int nCount = GetMenuItemCount(hSubMenu);
			for (int i = 0; i < nCount; i++) {
				WCHAR szName[MAX_COLUMN_NAME_LEN];
				mii.fMask = MIIM_ID | MIIM_STRING;
				mii.cch = MAX_COLUMN_NAME_LEN;
				mii.dwTypeData = szName;
				GetMenuItemInfo(hSubMenu, i, TRUE, &mii);
				teStripAmp(szName);
				if (lstrcmpi(szName, szNew) == 0) {
					CMINVOKECOMMANDINFO cmi = { sizeof(CMINVOKECOMMANDINFO), 0, NULL, (LPCSTR)(mii.wID - 1) };
					pCM->InvokeCommand(&cmi);
					break;
				}
			}
			DestroyMenu(hMenu);
			pCM->Release();
		}
	} else if (ListView_IsGroupViewEnabled(m_hwndLV)) {
		SendMessage(m_hwndDV, WM_COMMAND, CommandID_GROUP, 0);
	}
#endif
}

VOID CteShellBrowser::SetRedraw(BOOL bRedraw)
{
	SendMessage(m_hwnd, WM_SETREDRAW, bRedraw, 0);
	if (m_hwndAlt) {
		BringWindowToTop(m_hwndAlt);
	}
}

//CTE

CTE::CTE(int nCmdShow)
{
	m_cRef = 1;
	::ZeroMemory(g_param, sizeof(g_param));
	g_param[TE_Type] = CTRL_TE;
	g_param[TE_CmdShow] = nCmdShow;
	g_param[TE_Layout] = 0x80;
	VariantInit(&m_vData);
	m_pDropTarget2 = new CteDropTarget2(NULL, static_cast<IDispatch *>(this));
	RegisterDragDrop(g_hwndMain, m_pDropTarget2);
}

CTE::~CTE()
{
	VariantClear(&m_vData);
	SafeRelease(&m_pDropTarget2);
}

STDMETHODIMP CTE::QueryInterface(REFIID riid, void **ppvObject)
{
	static const QITAB qit[] =
	{
		QITABENT(CTE, IDispatch),
		QITABENT(CTE, IDropSource),
		{ 0 },
	};
	return QISearch(this, qit, riid, ppvObject);
}

STDMETHODIMP_(ULONG) CTE::AddRef()
{
	return ::InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CTE::Release()
{
	if (::InterlockedDecrement(&m_cRef) == 0) {
		delete this;
		return 0;
	}

	return m_cRef;
}

STDMETHODIMP CTE::GetTypeInfoCount(UINT *pctinfo)
{
	*pctinfo = 0;
	return S_OK;
}

STDMETHODIMP CTE::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CTE::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	return teGetDispId(methodTE, _countof(methodTE), g_maps[MAP_TE], *rgszNames, rgDispId, TRUE);
}

STDMETHODIMP CTE::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	try {
		IUnknown *punk = NULL;
		IDispatch *pdisp = NULL;

		int nArg = pDispParams ? pDispParams->cArgs - 1 : -1;
		if (pVarResult) {
			VariantInit(pVarResult);
		}
		if (dispIdMember >= TE_METHOD && wFlags == DISPATCH_PROPERTYGET) {
			CteDispatch *pClone = new CteDispatch(this, 0);
			pClone->m_dispIdMember = dispIdMember;
			teSetObjectRelease(pVarResult, pClone);
			return S_OK;
		}
		switch(dispIdMember) {
			//Data
			case 1001:
				if (nArg >= 0) {
					VariantClear(&m_vData);
					VariantCopy(&m_vData, &pDispParams->rgvarg[nArg]);
				}
				if (pVarResult) {
					VariantCopy(pVarResult, &m_vData);
				}
				return S_OK;
			//hwnd
			case 1002:
				teSetPtr(pVarResult, g_hwndMain);
				return S_OK;
			//About
			case 1004:
				teSetSZ(pVarResult, g_szTE);
				return S_OK;
			//Ctrl
			case TE_METHOD + 1005:
				if (pVarResult) {
					if (nArg >= 0) {
						int nCtrl = GetIntFromVariant(&pDispParams->rgvarg[nArg]);
						switch (nCtrl) {
							case CTRL_FV:
							case CTRL_SB:
							case CTRL_EB:
							case CTRL_TV:
								CteShellBrowser *pSB;
								pSB = NULL;
								if (nArg >= 1) {
									UINT nId = GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]);
									if (nId && nId < g_ppSB.size()) {
										pSB = g_ppSB[nId - 1];
									}
								}
								if (!pSB && g_pTC) {
									pSB = g_pTC->GetShellBrowser(g_pTC->m_nIndex);
									if (!pSB) {
										pSB = GetNewShellBrowser(g_pTC);
									}
								}
								if (pSB) {
									if (nCtrl == CTRL_TV) {
										teSetObject(pVarResult, pSB->m_pTV);
									} else {
										teSetObject(pVarResult, pSB);
									}
								}
								break;
							case CTRL_TC:
								CteTabCtrl *pTC;
								pTC = g_pTC;
								if (nArg >= 1) {
									int nId = GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]);
									if (nId) {
										pTC = g_ppTC[nId - 1];
									}
								}
								if (pTC) {
									teSetObject(pVarResult, pTC);
								}
								break;
							case CTRL_WB:
								teSetObject(pVarResult, g_pWebBrowser);
								break;
						}//end_switch
					}
				}
				return S_OK;
			//Ctrls
			case TE_METHOD + 1006:
				if (nArg >= 0) {
					int nCtrl = GetIntFromVariant(&pDispParams->rgvarg[nArg]);
					BOOL bAll = nArg >= 1 ? !GetIntFromVariant(&pDispParams->rgvarg[nArg -  1]) : TRUE;
					IDispatch *pArray = NULL;
					GetNewArray(&pArray);

					switch (nCtrl) {
						case CTRL_FV:
						case CTRL_SB:
						case CTRL_EB:
						case CTRL_TV:
							for (UINT i = 0; i < g_ppSB.size(); i++) {
								CteShellBrowser *pSB = g_ppSB[i];
								if (!pSB->m_bEmpty && (bAll || pSB->m_pTC->m_bVisible)) {
									if (nCtrl == CTRL_TV) {
										if (pSB->m_pTV->m_param[SB_TreeAlign] & 2) {
											teArrayPush(pArray, pSB->m_pTV);
										}
									} else {
										teArrayPush(pArray, pSB);
									}
								}
							}
							if (nCtrl == CTRL_TV) {
								for (UINT i = 0; i < g_ppTV.size(); i++) {
									CteTreeView *pTV = g_ppTV[i];
									if (!pTV->m_bEmpty && (bAll || pTV->m_param[SB_TreeAlign] & 2)) {
										teArrayPush(pArray, pTV);
									}
								}
							}
							break;
						case CTRL_TC:
							for (UINT i = 0; i < g_ppTC.size(); i++) {
								CteTabCtrl *pTC = g_ppTC[i];
								if (!pTC->m_bEmpty && (bAll || pTC->m_bVisible)) {
									teArrayPush(pArray, pTC);
								}
							}
							break;
						case CTRL_WB:
							if (g_pWebBrowser) {
								teArrayPush(pArray, g_pWebBrowser);
							}
							break;
					}//end_switch
					teSetObjectRelease(pVarResult, new CteDispatchEx(pArray));
					pArray->Release();
				}
				return S_OK;
			//Version
			case 1007:
				teSetLong(pVarResult, 20000000 + VER_Y * 10000 + VER_M * 100 + VER_D);
				return S_OK;
			//ClearEvents
			case TE_METHOD + 1008:
				ClearEvents();
				g_nReload = 0;
				return S_OK;
			//Reload
			case TE_METHOD + 1009:
				g_nReload = 1;
				if (g_nException-- <= 0 || (nArg >= 0 && GetIntFromVariant(&pDispParams->rgvarg[nArg]))) {
#ifdef _DEBUG
					if (g_strException) {
						MessageBox(NULL, g_strException, NULL, MB_OK);
					}
#endif
					SetTimer(g_hwndMain, TET_Reload, 100, teTimerProc);
					return S_OK;
				}
				SetTimer(g_hwndMain, TET_Refresh, 100, teTimerProc);
				return S_OK;
			//DIID_DShellWindowsEvents
			case DISPID_WINDOWREGISTERED:
				DoFunc(TE_OnWindowRegistered, g_pTE, S_OK);
				return S_OK;
			//GetObject
			case TE_METHOD + 1020:
			//CreateObject
			case TE_METHOD + 1010:
				if (nArg >= 0) {
					CLSID clsid;
					VARIANT vClass;
					teVariantChangeType(&vClass, &pDispParams->rgvarg[nArg], VT_BSTR);
					HRESULT hr = E_FAIL;
					IUnknown *punk;
					if SUCCEEDED(teCLSIDFromProgID(vClass.bstrVal, &clsid)) {
						if (dispIdMember == TE_METHOD + 1020) {
							hr = GetActiveObject(clsid, NULL, &punk);
						}
						if FAILED(hr) {
							hr = teCreateInstance(clsid, NULL, NULL, IID_PPV_ARGS(&punk));
						}
					} else if (dispIdMember == TE_METHOD + 1020) {
						hr = CoGetObject(vClass.bstrVal, NULL, IID_PPV_ARGS(&punk));
					}
					if SUCCEEDED(hr) {
						teSetObjectRelease(pVarResult, punk);
					}
					VariantInit(&vClass);
				}
				return S_OK;
			//WindowsAPI
			case 1030:
#ifdef _USE_OBJECTAPI
				teSetObject(pVarResult, g_pAPI);
#else
				teSetObjectRelease(pVarResult, new CteWindowsAPI(NULL));
#endif
				return S_OK;
			//WindowsAPI0
			case 1031:
				IDispatch *pAPI;
				pAPI = NULL;
				GetNewObject(&pAPI);
				VARIANT v;
				teSetObjectRelease(&v, new CteWindowsAPI(&dispAPI[0]));
				BSTR bs;
				bs = teMultiByteToWideChar(CP_UTF8, dispAPI[0].name, -1);
				tePutProperty(pAPI, bs, &v);
				VariantClear(&v);
				::SysFreeString(bs);
				teSetObject(pVarResult, pAPI);
				return S_OK;
			//CommonDialog
			case 1131:
				teSetObjectRelease(pVarResult, new CteCommonDialog());
				return S_OK;
			//WICBitmap(GdiplusBitmap)
			case 1132:
				teSetObjectRelease(pVarResult, new CteWICBitmap());
				return S_OK;
			//FolderItems
			case TE_METHOD + 1133:
				IDataObject *pDataObj;
				pDataObj = NULL;
				if (nArg >= 0) {
					GetDataObjFromVariant(&pDataObj, &pDispParams->rgvarg[nArg]);
				}
				teSetObjectRelease(pVarResult, new CteFolderItems(pDataObj, NULL));
				SafeRelease(&pDataObj);
				return S_OK;
			//Object
			case TE_METHOD + 1134:
				GetNewObject(&pdisp);
				teSetObjectRelease(pVarResult, pdisp);
				return S_OK;
			//Array
			case TE_METHOD + 1135:
				GetNewArray(&pdisp);
				teSetObjectRelease(pVarResult, pdisp);
				return S_OK;
			//Collection
			case TE_METHOD + 1136:
				if (nArg >= 0 && GetDispatch(&pDispParams->rgvarg[nArg], &pdisp)) {
					teSetObjectRelease(pVarResult, new CteDispatch(pdisp, 1));
				}
				return S_OK;
			//ProgressDialog
			case 1137:
				teSetObjectRelease(pVarResult, new CteProgressDialog(NULL));
				return S_OK;
			//DateTimeFormat
			case 1138:
				if (nArg >= 0) {
					teSysFreeString(&g_bsDateTimeFormat);
					if (pDispParams->rgvarg[nArg].vt == VT_BSTR && ::SysStringLen(pDispParams->rgvarg[nArg].bstrVal)) {
						g_bsDateTimeFormat = ::SysAllocString(pDispParams->rgvarg[nArg].bstrVal);
					}
				}
				teSetSZ(pVarResult, g_bsDateTimeFormat);
				return S_OK;
			//Background
			case 1140:
				if (nArg >= 0) {
					if (g_hbrBackground) {
						DeleteObject(g_hbrBackground);
					}
					g_hbrBackground = (HBRUSH)GetLLFromVariant(&pDispParams->rgvarg[nArg]);
				}
				teSetPtr(pVarResult, g_hbrBackground);
				return S_OK;
#ifdef _USE_TESTOBJECT
			//TestObj
			case 1200:
				teSetObjectRelease(pVarResult, new CteTest());
				return S_OK;
#endif
			//CtrlFromPoint
			case TE_METHOD + 1040:
				if (nArg >= 0) {
					POINT pt;
					GetPointFormVariant(&pt, &pDispParams->rgvarg[nArg]);
					IDispatch *pdisp;
					if SUCCEEDED(ControlFromhwnd(&pdisp, WindowFromPoint(pt))) {
						teSetObjectRelease(pVarResult, pdisp);
					}
				}
				return S_OK;
			//CreateCtrl
			case TE_METHOD + 1050:
				if (nArg >= 0) {
					switch (GetIntFromVariant(&pDispParams->rgvarg[nArg])) {
						case CTRL_TC:
							if (nArg >= 4) {
								CteTabCtrl *pTC;
								pTC = GetNewTC();
								pTC->Init();
								int i = _countof(pTC->m_param) - 1;
								for (i = nArg < i ? nArg : i; i >= 0; i--) {
									pTC->m_param[i] = GetIntFromVariantPP(&pDispParams->rgvarg[nArg - i], i);
								}
								if (pTC->Create()) {
									teSetObject(pVarResult, pTC);
									pTC->Show(TRUE);
								} else {
									pTC->Release();
								}
							}
							break;
						case CTRL_SW:
							if (nArg >= 5) {
								VARIANT v;
								teVariantChangeType(&v, &pDispParams->rgvarg[nArg - 1], VT_BSTR);
								RECT rc, rcWindow;
								HWND hwnd1 = g_hwndMain;
								if (FindUnknown(&pDispParams->rgvarg[nArg - 3], &punk)) {
									IUnknown_GetWindow(punk, &hwnd1);
								}
								GetWindowRect(hwnd1, &rc);
								HWND hwndParent = hwnd1;
								while (hwnd1 = GetParent(hwndParent)) {
									hwndParent = hwnd1;
								}
								GetWindowRect(hwndParent, &rcWindow);
								int a = rcWindow.right - rc.right + rc.left - rcWindow.left;
								int b = rcWindow.bottom - rc.bottom + rc.top - rcWindow.top;
								int w = GetIntFromVariant(&pDispParams->rgvarg[nArg - 4]) + a;
								int h = GetIntFromVariant(&pDispParams->rgvarg[nArg - 5]) + b;
								int x = nArg >= 6 ? GetIntFromVariant(&pDispParams->rgvarg[nArg - 6]) : 0;
								int y = nArg >= 7 ? GetIntFromVariant(&pDispParams->rgvarg[nArg - 7]) : 0;
								if (!x) {
									x = rc.left + (rc.right - rc.left - w) / 2;
									if (w == rcWindow.right - rcWindow.left) {
										x += a;
									}
								}
								if (!y) {
									y = rc.top + (rc.bottom - rc.top - h) / 2;
									if (h == rcWindow.bottom - rcWindow.top) {
										y += b / 2;
									}
								}
								HMONITOR hMonitor = MonitorFromRect(&rcWindow, MONITOR_DEFAULTTONEAREST);
								MONITORINFO mi;
								mi.cbSize = sizeof(mi);
								if (GetMonitorInfo(hMonitor, &mi)) {
									if (x + w > mi.rcWork.right) {
										x = mi.rcWork.right - w;
									}
									if (x < mi.rcWork.left) {
										x = mi.rcWork.left;
									}
									if (y + h > mi.rcWork.bottom) {
										y = mi.rcWork.bottom - h;
									}
									if (y < mi.rcWork.top) {
										y = mi.rcWork.top;
									}
								}
								MyRegisterClass(hInst, WINDOW_CLASS2, WndProc2);
								HWND hwnd = CreateWindow(WINDOW_CLASS2, g_szTE, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
									x, y, w, h, hwndParent, NULL, hInst, NULL);
								teSetDarkMode(hwnd);
								CteWebBrowser *pWB = new CteWebBrowser(hwnd, v.bstrVal, &pDispParams->rgvarg[nArg - 2]);
								VariantClear(&v);
								teSetObject(&v, pWB);
								tePutPropertyAtLLX(g_pSubWindows, (LONGLONG)hwnd, &v);
								teSetObjectRelease(pVarResult, pWB);
							}
							break;
						case CTRL_TV:
							CteTreeView *pTV;
							pTV = GetNewTV();
							pTV->Init(NULL, g_hwndMain);
							teSetObject(pVarResult, pTV);
							break;
					}//end_switch
				}
				return S_OK;

			//MainMenu
			case TE_METHOD + 1060:
				if (pVarResult) {
					if (nArg >= 0) {
						if (!g_hMenu) {
							g_hMenu = LoadMenu(g_hShell32, MAKEINTRESOURCE(216));
/*//
							if (!g_hMenu) {
								CteShellBrowser *pSB = GetNewShellBrowser(NULL);
								IShellFolder *pDesktopFolder;
								SHGetDesktopFolder(&pDesktopFolder);
								if (pDesktopFolder->CreateViewObject(g_hwndMain, IID_PPV_ARGS(&pSB->m_pShellView)) == S_OK) {
									FOLDERSETTINGS fs = { FWF_NOWEBVIEW, FVM_LIST };
									RECT rc;
									SetRectEmpty(&rc);
									if SUCCEEDED(pSB->m_pShellView->CreateViewWindow(NULL, &fs, pSB, &rc, &pSB->m_hwnd)) {
										pSB->m_pShellView->UIActivate(SVUIA_ACTIVATE_NOFOCUS);
									}
								}
								pDesktopFolder->Release();
								pSB->Close(TRUE);
							}
*///
						}
						MENUITEMINFO mii = { sizeof(MENUITEMINFO), MIIM_SUBMENU };
						GetMenuItemInfo(g_hMenu, GetIntFromVariant(&pDispParams->rgvarg[nArg]), FALSE, &mii);

						HMENU hMenu = CreatePopupMenu();
						UINT fState = MFS_DISABLED;
						if (g_pTC) {
							CteShellBrowser *pSB = g_pTC->GetShellBrowser(g_pTC->m_nIndex);
							if (pSB) {
								fState = MFS_ENABLED;
							}
						}
						teCopyMenu(hMenu, mii.hSubMenu, fState);
						teSetPtr(pVarResult, hMenu);
					}
				}
				return S_OK;
			//CtrlFromWindow
			case TE_METHOD + 1070:
				if (nArg >= 0) {
					IDispatch *pdisp;
					if SUCCEEDED(ControlFromhwnd(&pdisp, (HWND)GetPtrFromVariant(&pDispParams->rgvarg[nArg]))) {
						teSetObjectRelease(pVarResult, pdisp);
					}
				}
				return S_OK;
			//LockUpdate
			case TE_METHOD + 1080:
				if (::InterlockedIncrement(&g_nLockUpdate) == 1) {
					teSetRedraw(FALSE);
				}
				return S_OK;
			//UnlockUpdate
			case TE_METHOD + 1090:
				if (::InterlockedDecrement(&g_nLockUpdate) <= 0) {
					g_nLockUpdate = 0;
					teSetRedraw(TRUE);
					for (UINT i = 0; i < g_ppTC.size(); i++) {
						CteTabCtrl *pTC = g_ppTC[i];
						if (pTC->m_bVisible) {
							CteShellBrowser *pSB = pTC->GetShellBrowser(pTC->m_nIndex);
							if (pSB && !pSB->m_bEmpty) {
								if (pSB->m_nUnload & 5) {
									pSB->Show(TRUE, 0);
								}
								if (pTC->m_nRedraw) {
									pTC->RedrawUpdate();
								}
							}
						}
					}
					ArrangeWindow();
				}
				return S_OK;
			//HookDragDrop//Deprecated
			case TE_METHOD + 1100:
				return S_OK;
			//Value
			case DISPID_VALUE:
				teSetObject(pVarResult, this);
				return S_OK;
			default:
				if (dispIdMember >= START_OnFunc && dispIdMember < START_OnFunc + Count_OnFunc) {
					teInvokeObject(&g_pOnFunc[dispIdMember - START_OnFunc], wFlags, pVarResult, nArg, pDispParams->rgvarg);
					if (nArg >= 0 && dispIdMember == TE_Labels + START_OnFunc && g_pOnFunc[TE_Labels]) {
						g_bLabelsMode = !teHasProperty(g_pOnFunc[TE_Labels], L"call");
					}
					return S_OK;
				}
				//Type, offsetLeft etc.
				else if (dispIdMember >= TE_OFFSET && dispIdMember < TE_OFFSET + Count_TE_params) {
					if (nArg >= 0 && dispIdMember != TE_OFFSET) {
						int i = GetIntFromVariant(&pDispParams->rgvarg[nArg]);
						if (i != g_param[dispIdMember - TE_OFFSET]) {
							g_param[dispIdMember - TE_OFFSET] = i;
							if (dispIdMember >= TE_OFFSET + TE_Left && dispIdMember <= TE_Bottom + TE_OFFSET) {
								ArrangeWindow();
							}
							if (dispIdMember == TE_OFFSET + TE_Layout) {
								for (UINT i = 0; i < g_ppSB.size(); i++) {
									CteShellBrowser *pSB = g_ppSB[i];
									pSB->m_bCheckLayout = TRUE;
									pSB->GetViewModeAndIconSize(TRUE);
								}
							}
						}
					}
					teSetLong(pVarResult, g_param[dispIdMember - TE_OFFSET]);
				}
				return S_OK;
		}
	} catch (...) {
		return teException(pExcepInfo, "external", methodTE, dispIdMember);
	}
	return DISP_E_MEMBERNOTFOUND;
}

//IDropSource
STDMETHODIMP CTE::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
{
	if (fEscapePressed || !g_nDropState || (grfKeyState & (MK_LBUTTON | MK_RBUTTON)) == (MK_LBUTTON | MK_RBUTTON)) {
		g_nDropState = 0;
		return DRAGDROP_S_CANCEL;
	}
	if ((grfKeyState & (MK_LBUTTON | MK_RBUTTON)) == 0) {
		VARIANT v;
		VariantInit(&v);
		if (g_nDropState == 2 && g_pOnFunc[TE_OnBeforeGetData]) {
			g_nDropState = 0;
			if (g_pDropTargetHelper) {
				g_pDropTargetHelper->DragLeave();
			}
			VARIANTARG *pv = GetNewVARIANT(3);
			teSetObject(&pv[2], g_pDraggingCtrl);
			teSetObjectRelease(&pv[1], new CteFolderItems(g_pDraggingItems, NULL));
			teSetLong(&pv[0], 4);
			Invoke4(g_pOnFunc[TE_OnBeforeGetData], &v, 3, pv);
		}
		g_nDropState = 0;
		return GetIntFromVariantClear(&v) ? DRAGDROP_S_CANCEL : DRAGDROP_S_DROP;
	}
	return S_OK;
}

STDMETHODIMP CTE::GiveFeedback(DWORD dwEffect)
{
	return DRAGDROP_S_USEDEFAULTCURSORS;
}

// CteInternetSecurityManager
CteInternetSecurityManager::CteInternetSecurityManager()
{
	m_cRef = 1;
}

CteInternetSecurityManager::~CteInternetSecurityManager()
{
}

STDMETHODIMP CteInternetSecurityManager::QueryInterface(REFIID riid, void **ppvObject)
{
	static const QITAB qit[] =
	{
		QITABENT(CteInternetSecurityManager, IInternetSecurityManager),
		{ 0 },
	};
	return QISearch(this, qit, riid, ppvObject);
}

STDMETHODIMP_(ULONG) CteInternetSecurityManager::AddRef()
{
	return ::InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CteInternetSecurityManager::Release()
{
	if (::InterlockedDecrement(&m_cRef) == 0) {
		delete this;
		return 0;
	}

	return m_cRef;
}

//IInternetSecurityManager
STDMETHODIMP CteInternetSecurityManager::SetSecuritySite(IInternetSecurityMgrSite *pSite)
{
	return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CteInternetSecurityManager::GetSecuritySite(IInternetSecurityMgrSite **ppSite)
{
	return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CteInternetSecurityManager::MapUrlToZone(LPCWSTR pwszUrl, DWORD *pdwZone, DWORD dwFlags)
{
	return INET_E_DEFAULT_ACTION;
//	*pdwZone = URLZONE_LOCAL_MACHINE;
//	return S_OK;
}

STDMETHODIMP CteInternetSecurityManager::GetSecurityId(LPCWSTR pwszUrl, BYTE *pbSecurityId, DWORD *pcbSecurityId, DWORD_PTR dwReserved)
{
	return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CteInternetSecurityManager::ProcessUrlAction(LPCWSTR pwszUrl, DWORD dwAction, BYTE *pPolicy, DWORD cbPolicy, BYTE *pContext, DWORD cbContext, DWORD dwFlags, DWORD dwReserved)
{
	*pPolicy = teStartsText(L"http:", pwszUrl) ? URLPOLICY_DISALLOW : URLPOLICY_ALLOW;
	return S_OK;
}

STDMETHODIMP CteInternetSecurityManager::QueryCustomPolicy(LPCWSTR pwszUrl, REFGUID guidKey, BYTE **ppPolicy, DWORD *pcbPolicy, BYTE *pContext, DWORD cbContext, DWORD dwReserved)
{
	return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CteInternetSecurityManager::SetZoneMapping(DWORD dwZone, LPCWSTR lpszPattern, DWORD dwFlags)
{
	return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CteInternetSecurityManager::GetZoneMappings(DWORD dwZone, IEnumString **ppenumString, DWORD dwFlags)
{
	return INET_E_DEFAULT_ACTION;
}

// CteNewWindowManager
CteNewWindowManager::CteNewWindowManager()
{
	m_cRef = 1;
}

CteNewWindowManager::~CteNewWindowManager()
{
}

STDMETHODIMP CteNewWindowManager::QueryInterface(REFIID riid, void **ppvObject)
{
	static const QITAB qit[] =
	{
		QITABENT(CteNewWindowManager, INewWindowManager),
		{ 0 },
	};
	return QISearch(this, qit, riid, ppvObject);
}

STDMETHODIMP_(ULONG) CteNewWindowManager::AddRef()
{
	return ::InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CteNewWindowManager::Release()
{
	if (::InterlockedDecrement(&m_cRef) == 0) {
		delete this;
		return 0;
	}

	return m_cRef;
}

//INewWindowManager
STDMETHODIMP CteNewWindowManager::EvaluateNewWindow(LPCWSTR pszUrl, LPCWSTR pszName, LPCWSTR pszUrlContext, LPCWSTR pszFeatures, BOOL fReplace, DWORD dwFlags, DWORD dwUserActionTime)
{
	return teStartsText(L"http", pszUrl) ? S_FALSE : S_OK;
}

// CteWebBrowser

CteWebBrowser::CteWebBrowser(HWND hwndParent, WCHAR *szPath, VARIANT *pvArg)
{
	m_cRef = 1;
	m_hwndParent = hwndParent;
	m_dwCookie = 0;
	m_pDragItems = NULL;
	m_pDropTarget = NULL;
	m_pExternal = NULL;
	VariantInit(&m_vData);
	if (pvArg) {
		VariantCopy(&m_vData, pvArg);
		VARIANT v;
		VariantInit(&v);
		GetNewObject(&m_pExternal);
		teSetObject(&v, g_pTE);
		tePutProperty(m_pExternal, L"TE", &v);
		teSetObject(&v, this);
		tePutProperty(m_pExternal, L"WB", &v);
		teSetLong(&v, CTRL_AR);
		tePutProperty(m_pExternal, L"Type", &v);
	} else {
		g_pTE->QueryInterface(IID_PPV_ARGS(&m_pExternal));
	}
	MSG        msg;
	RECT       rc;
	IOleObject *pOleObject;
	if (SUCCEEDED(teCreateInstance(CLSID_WebBrowser, NULL, NULL, IID_PPV_ARGS(&m_pWebBrowser))) &&
		SUCCEEDED(m_pWebBrowser->QueryInterface(IID_PPV_ARGS(&pOleObject)))) {
		pOleObject->SetClientSite(static_cast<IOleClientSite *>(this));
		SetRectEmpty(&rc);
		pOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, &msg, static_cast<IOleClientSite *>(this), 0, hwndParent, &rc);
		pOleObject->Release();
		teAdvise(m_pWebBrowser, DIID_DWebBrowserEvents2, static_cast<IDispatch *>(this), &m_dwCookie);
		m_pWebBrowser->put_Offline(VARIANT_TRUE);
		m_bstrPath = SysAllocString(szPath);
		m_pWebBrowser->Navigate(m_bstrPath, NULL, NULL, NULL, NULL);
		m_pWebBrowser->put_Visible(VARIANT_TRUE);
	}
}

CteWebBrowser::~CteWebBrowser()
{
	teSysFreeString(&m_bstrPath);
	Close();
	VariantClear(&m_vData);
	SafeRelease(&m_pExternal);
}

STDMETHODIMP CteWebBrowser::QueryInterface(REFIID riid, void **ppvObject)
{
	static const QITAB qit[] =
	{
		QITABENT(CteWebBrowser, IOleClientSite),
		QITABENT(CteWebBrowser, IOleWindow),
		QITABENT(CteWebBrowser, IOleInPlaceSite),
		QITABENT(CteWebBrowser, IDispatch),
		QITABENT(CteWebBrowser, IDocHostUIHandler),
		QITABENT(CteWebBrowser, IDropTarget),
		QITABENT(CteWebBrowser, IDocHostShowUI),
//		QITABENT(CteWebBrowser, IOleCommandTarget),
		{ 0 },
	};
	*ppvObject = NULL;
	if (IsEqualIID(riid, IID_IServiceProvider)) {
		*ppvObject = static_cast<IServiceProvider *>(new CteServiceProvider(static_cast<IDispatch *>(this), m_pWebBrowser));
		return S_OK;
	} else if (IsEqualIID(riid, IID_IWebBrowser2)) {
		return m_pWebBrowser->QueryInterface(riid, (LPVOID *)ppvObject);
	} else {
		return QISearch(this, qit, riid, ppvObject);
	}
	AddRef();
	return S_OK;
}

STDMETHODIMP_(ULONG) CteWebBrowser::AddRef()
{
	return ::InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CteWebBrowser::Release()
{
	if (::InterlockedDecrement(&m_cRef) == 0) {
		delete this;
		return 0;
	}

	return m_cRef;
}

STDMETHODIMP CteWebBrowser::GetTypeInfoCount(UINT *pctinfo)
{
	*pctinfo = 0;
	return S_OK;
}

STDMETHODIMP CteWebBrowser::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteWebBrowser::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	return teGetDispId(methodWB, 0, NULL, *rgszNames, rgDispId, TRUE);
}

STDMETHODIMP CteWebBrowser::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	try {
		int nArg = pDispParams ? pDispParams->cArgs - 1 : -1;
		if (pVarResult) {
			VariantInit(pVarResult);
		}
		switch(dispIdMember) {
			//Data
			case 0x10000001:
				if (nArg >= 0) {
					VariantClear(&m_vData);
					VariantCopy(&m_vData, &pDispParams->rgvarg[nArg]);
				}
				if (pVarResult) {
					VariantCopy(pVarResult, &m_vData);
				}
				return S_OK;
			//hwnd
			case 0x10000002:
				teSetPtr(pVarResult, get_HWND());
				return S_OK;
			//Type
			case 0x10000003:
				teSetLong(pVarResult, m_hwndParent == g_hwndMain ? CTRL_WB : CTRL_SW);
				return S_OK;
			//TranslateAccelerator
			case 0x10000004:
				HRESULT hr;
				hr = E_NOTIMPL;
				if (nArg >= 3) {
					IOleInPlaceActiveObject *pActiveObject = NULL;
					if SUCCEEDED(QueryInterface(IID_PPV_ARGS(&pActiveObject))) {
						MSG msg;
						msg.hwnd = (HWND)GetPtrFromVariant(&pDispParams->rgvarg[nArg]);
						msg.message = GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]);
						msg.wParam = (WPARAM)GetPtrFromVariant(&pDispParams->rgvarg[nArg - 2]);
						msg.lParam = (LPARAM)GetPtrFromVariant(&pDispParams->rgvarg[nArg - 3]);
						hr = pActiveObject->TranslateAcceleratorW(&msg);
						pActiveObject->Release();
					}
				}
				teSetLong(pVarResult, hr);
				return S_OK;
			//Application
			case 0x10000005:
				IDispatch *pdisp;
				if SUCCEEDED(m_pWebBrowser->get_Application(&pdisp)) {
					teSetObjectRelease(pVarResult, pdisp);
				}
				return S_OK;
			//Document
			case 0x10000006:
				if SUCCEEDED(m_pWebBrowser->get_Document(&pdisp)) {
					teSetObjectRelease(pVarResult, pdisp);
				}
				return S_OK;
			//Window
			case 0x10000007:
				if (teGetHTMLWindow(m_pWebBrowser, IID_PPV_ARGS(&pdisp))) {
					teSetObjectRelease(pVarResult, pdisp);
				}
				return S_OK;
			//Focus
			case 0x10000008:
				teSetForegroundWindow(m_hwndParent);
				return S_OK;
			//Close
/*			case 0x10000009:
				PostMessage(m_hwndParent, WM_CLOSE, 0, 0);
				return S_OK;*/
			//DIID_DWebBrowserEvents2
			case DISPID_DOWNLOADCOMPLETE:
				if (g_nReload == 1 && m_hwndParent == g_hwndMain) {
					g_nReload = 2;
					SetTimer(g_hwndMain, TET_Reload, 2000, teTimerProc);
				}
				return S_OK;
			case DISPID_BEFORENAVIGATE2:
				if (nArg >= 6 && m_hwndParent == g_hwndMain && pDispParams->rgvarg[nArg].pdispVal == m_pWebBrowser) {
					if (pDispParams->rgvarg[nArg - 6].vt == (VT_BYREF | VT_BOOL)) {
						VARIANT vURL;
						teVariantChangeType(&vURL, &pDispParams->rgvarg[nArg - 1], VT_BSTR);
						VARIANT_BOOL bSB = VARIANT_FALSE;
						FolderItem *pid = new CteFolderItem(&vURL);
						pid->get_IsFolder(&bSB);
						if (bSB || StrChr(vURL.bstrVal, '/')) {
							*pDispParams->rgvarg[nArg - 6].pboolVal = VARIANT_TRUE;
							if (bSB) {
								CteShellBrowser *pSB = NULL;
								if (g_pTC) {
									pSB = g_pTC->GetShellBrowser(g_pTC->m_nIndex);
									if (pSB) {
										pSB->BrowseObject2(pid, SBSP_SAMEBROWSER);
									}
								}
							}
						}
						pid->Release();
						VariantClear(&vURL);
					}
				}
				break;
			case DISPID_DOCUMENTCOMPLETE:
				if (m_bstrPath) {
					teGetWindow(m_pWebBrowser, &m_hwndBrowser);
					if (g_bsDocumentWrite) {
						g_pWebBrowser->write(g_bsDocumentWrite);
						teSysFreeString(&g_bsDocumentWrite);
						teShowWindow(m_hwndParent, SW_SHOWNORMAL);
						return S_OK;
					}
					if (g_bInit) {
						g_bInit = FALSE;
						SetTimer(g_hwndMain, TET_Create, 100, teTimerProc);
					}
					if (g_hwndMain != m_hwndParent) {
						teShowWindow(m_hwndParent, SW_SHOWNORMAL);
					}
					return S_OK;
				}
				PostMessage(m_hwndParent, WM_CLOSE, 0, 0);
				return S_OK;
/*/// XP(IE8) does not work.
			case DISPID_NAVIGATECOMPLETE2:
				if (g_hwndMain != m_hwndParent) {
					HRESULT hr = E_NOTIMPL;
					IDispatch *pdisp;
					if (m_pWebBrowser->get_Document(&pdisp) == S_OK) {
						IHTMLDocument2 *pdoc;
						if SUCCEEDED(pdisp->QueryInterface(IID_PPV_ARGS(&pdoc))) {
							IHTMLWindow2 *pwin;
							if SUCCEEDED(pdoc->get_parentWindow(&pwin)) {
								hr = tePutProperty(pwin, L"dialogArguments", &m_vData);
								pwin->Release();
							}
							pdoc->Release();
						}
						pdisp->Release();
					}
				}
				return S_OK;
//*///
			case DISPID_SECURITYDOMAIN:
				return S_OK;
			case DISPID_AMBIENT_DLCONTROL:
				teSetLong(pVarResult, DLCTL_DLIMAGES | DLCTL_VIDEOS | DLCTL_BGSOUNDS);
				return S_OK;
/*///
			case DISPID_AMBIENT_USERAGENT:
				teSetSZ(pVarResult, g_szTE);
				return S_OK;
//*///
			case DISPID_NEWWINDOW3:
				if (g_pOnFunc[TE_OnNewWindow]) {
					VARIANT vResult;
					VariantInit(&vResult);
					VARIANTARG *pv = GetNewVARIANT(4);
					teSetObject(&pv[3], g_pWebBrowser);
					for (int i = 3; i--;) {
						VariantCopy(&pv[2 - i], &pDispParams->rgvarg[nArg - i - 2]);
					}
					Invoke4(g_pOnFunc[TE_OnNewWindow], &vResult, 4, pv);
					if (GetIntFromVariantClear(&vResult) == S_OK) {
						*pDispParams->rgvarg[nArg - 1].pboolVal = VARIANT_TRUE;
					}
				}
				return S_OK;
/*			case DISPID_QUIT:
				return S_OK;
			case DISPID_NAVIGATEERROR:
				return S_OK;*/
			case DISPID_WINDOWCLOSING:
				if (nArg >= 1 && pDispParams->rgvarg[nArg - 1].vt == (VT_BYREF | VT_BOOL)) {
					pDispParams->rgvarg[nArg - 1].pboolVal[0] = VARIANT_TRUE;
				}
				PostMessage(m_hwndParent, WM_CLOSE, 0, 0);
				return S_FALSE;
			case DISPID_FILEDOWNLOAD:
				if (nArg >= 1 && pDispParams->rgvarg[nArg - 1].vt == (VT_BYREF | VT_BOOL)) {
					pDispParams->rgvarg[nArg - 1].pboolVal[0] = VARIANT_TRUE;
				}
				return S_OK;
			case DISPID_TITLECHANGE:
				if (m_hwndParent != g_hwndMain && m_bstrPath && nArg >= 0 && pDispParams->rgvarg[nArg].vt == VT_BSTR) {
					SetWindowText(m_hwndParent, pDispParams->rgvarg[nArg].bstrVal);
				}
				return S_OK;
			case DISPID_VALUE:
				teSetObject(pVarResult, this);
				return S_OK;
		}
	} catch (...) {
		return teException(pExcepInfo, "WebBrowser", methodWB, dispIdMember);
	}
	return DISP_E_MEMBERNOTFOUND;
}

//IOleClientSite
STDMETHODIMP CteWebBrowser::SaveObject()
{
	return E_NOTIMPL;
}

STDMETHODIMP CteWebBrowser::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteWebBrowser::GetContainer(IOleContainer **ppContainer)
{
	*ppContainer = NULL;

	return E_NOINTERFACE;
}

STDMETHODIMP CteWebBrowser::ShowObject()
{
	return S_OK;
}

STDMETHODIMP CteWebBrowser::OnShowWindow(BOOL fShow)
{
	return S_OK;
}

STDMETHODIMP CteWebBrowser::RequestNewObjectLayout()
{
	return E_NOTIMPL;
}

STDMETHODIMP CteWebBrowser::GetWindow(HWND *phwnd)
{
	*phwnd = m_hwndParent;
	return S_OK;
}

STDMETHODIMP CteWebBrowser::ContextSensitiveHelp(BOOL fEnterMode)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteWebBrowser::CanInPlaceActivate()
{
	return S_OK;
}

STDMETHODIMP CteWebBrowser::OnInPlaceActivate()
{
	return S_OK;
}

STDMETHODIMP CteWebBrowser::OnUIActivate()
{
	return S_OK;
}

STDMETHODIMP CteWebBrowser::GetWindowContext(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	*ppFrame = NULL;
	*ppDoc = NULL;

	GetClientRect(m_hwndParent, lprcPosRect);
	CopyRect(lprcClipRect, lprcPosRect);

	return S_OK;
}

STDMETHODIMP CteWebBrowser::Scroll(SIZE scrollExtant)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteWebBrowser::OnUIDeactivate(BOOL fUndoable)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteWebBrowser::OnInPlaceDeactivate()
{
	return E_NOTIMPL;
}

STDMETHODIMP CteWebBrowser::DiscardUndoState()
{
	return E_NOTIMPL;
}

STDMETHODIMP CteWebBrowser::DeactivateAndUndo()
{
	return E_NOTIMPL;
}

STDMETHODIMP CteWebBrowser::OnPosRectChange(LPCRECT lprcPosRect)
{
	return S_OK;
}

/*///
//IOleCommandTarget
STDMETHODIMP CteWebBrowser::QueryStatus(const GUID *pguidCmdGroup, ULONG cCmds, OLECMD *prgCmds, OLECMDTEXT *pCmdText)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteWebBrowser::Exec(const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT *pvaIn, VARIANT *pvaOut)
{
	if (nCmdID == OLECMDID_SHOWMESSAGE) {
		return E_NOTIMPL;
	}
	if (nCmdID == OLECMDID_SHOWSCRIPTERROR) {
		Sleep(0);
	}
	return E_NOTIMPL;
}
//*/
//IDocHostUIHandler
STDMETHODIMP CteWebBrowser::ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved)
{
	if (g_hwndMain == m_hwndParent && g_pOnFunc[TE_OnShowContextMenu]) {
		MSG msg1;
		msg1.hwnd = m_hwndParent;
		msg1.message = WM_CONTEXTMENU;
		msg1.wParam = dwID;
		msg1.pt.x = ppt->x;
		msg1.pt.y = ppt->y;
		return MessageSubPt(TE_OnShowContextMenu, this, &msg1);
	}
	return S_FALSE;
}

STDMETHODIMP CteWebBrowser::GetHostInfo(DOCHOSTUIINFO *pInfo)
{
	pInfo->cbSize        = sizeof(DOCHOSTUIINFO);
	pInfo->dwFlags       = DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_SCROLL_NO | DOCHOSTUIFLAG_IME_ENABLE_RECONVERSION;// DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE
	pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;
	pInfo->pchHostCss    = NULL;
	pInfo->pchHostNS     = NULL;
	return S_OK;
}

STDMETHODIMP CteWebBrowser::ShowUI(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc)
{
//On resize window
/*/// For check
	IOleInPlaceUIWindow *pUIWindow;
	if SUCCEEDED(pCommandTarget->QueryInterface(IID_PPV_ARGS(&pUIWindow))) {
	}
//*/
	return S_FALSE;
}

STDMETHODIMP CteWebBrowser::HideUI(VOID)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteWebBrowser::UpdateUI(VOID)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteWebBrowser::EnableModeless(BOOL fEnable)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteWebBrowser::OnDocWindowActivate(BOOL fActivate)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteWebBrowser::OnFrameWindowActivate(BOOL fActivate)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteWebBrowser::ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fFrameWindow)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteWebBrowser::TranslateAccelerator(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID)
{
	return S_FALSE;
}

STDMETHODIMP CteWebBrowser::GetOptionKeyPath(LPOLESTR *pchKey, DWORD dw)
{
	return E_NOTIMPL;
/*/// There is a problem
	WCHAR* szKey = L"Software\\tablacus\\explorer";
    if (pchKey) {
		*pchKey = (LPOLESTR)::CoTaskMemAlloc((lstrlen(szKey) + 1) * sizeof(WCHAR));
		if (*pchKey) {
			lstrcpy(*pchKey, szKey);
			return S_OK;
		}
    }
	return E_INVALIDARG;
*///
}

STDMETHODIMP CteWebBrowser::GetDropTarget(IDropTarget *pDropTarget, IDropTarget **ppDropTarget)
{
	SafeRelease(&m_pDropTarget);
	if (pDropTarget) {
		pDropTarget->QueryInterface(IID_PPV_ARGS(&m_pDropTarget));
	}
	return QueryInterface(IID_PPV_ARGS(ppDropTarget));
}

STDMETHODIMP CteWebBrowser::GetExternal(IDispatch **ppDispatch)
{
	return m_pExternal->QueryInterface(IID_PPV_ARGS(ppDispatch));
}

STDMETHODIMP CteWebBrowser::TranslateUrl(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut)
{
	return S_FALSE;
}

STDMETHODIMP CteWebBrowser::FilterDataObject(IDataObject *pDO, IDataObject **ppDORet)
{
	return E_NOTIMPL;
}

// IDocHostShowUI
STDMETHODIMP CteWebBrowser::ShowMessage(HWND hwnd, LPOLESTR lpstrText, LPOLESTR lpstrCaption, DWORD dwType, LPOLESTR lpstrHelpFile, DWORD dwHelpContext, LRESULT *plResult)
{
	if (dwType == (MB_ICONEXCLAMATION | MB_YESNO)) {// Stop running this script?
		*plResult = IDNO;
		return S_OK;
	}
	*plResult = MessageBox(hwnd, lpstrText, _T(PRODUCTNAME), dwType);
	return S_OK;
}

STDMETHODIMP CteWebBrowser::ShowHelp(HWND hwnd, LPOLESTR pszHelpFile, UINT uCommand, DWORD dwData, POINT ptMouse, IDispatch *pDispatchObjectHit)
{
	return E_NOTIMPL;
}

//IDropTarget
STDMETHODIMP CteWebBrowser::DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	g_bDragging = TRUE;
	m_DragLeave = E_NOT_SET;
	GetDragItems(&m_pDragItems, pDataObj);
	DWORD dwEffect = *pdwEffect;
	HRESULT	hr = DragSub(TE_OnDragEnter, this, m_pDragItems, &grfKeyState, pt, pdwEffect);
	if (m_pDropTarget) {
		*pdwEffect = dwEffect;
		if (m_pDropTarget->DragEnter(pDataObj, grfKeyState, pt, pdwEffect) == S_OK) {
			hr = S_OK;
		}
	}
	return hr;
}

STDMETHODIMP CteWebBrowser::DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	m_dwEffect = *pdwEffect;
	HRESULT hr = DragSub(TE_OnDragOver, this, m_pDragItems, &grfKeyState, pt, pdwEffect);
	if (hr != S_OK) {
		*pdwEffect = DROPEFFECT_NONE;
	}
	m_dwEffectTE = *pdwEffect;
	if (m_pDropTarget) {
		if (m_pDropTarget->DragOver(grfKeyState, pt, &m_dwEffect) == S_OK) {
			hr = S_OK;
		} else {
			m_dwEffect = DROPEFFECT_NONE;
		}
	}
	*pdwEffect |= m_dwEffect;
	m_grfKeyState = grfKeyState;

	return hr;
}

STDMETHODIMP CteWebBrowser::DragLeave()
{
	g_bDragging = FALSE;
	if (m_DragLeave == E_NOT_SET) {
		m_DragLeave = DragLeaveSub(this, &m_pDragItems);
		if (m_pDropTarget) {
			HRESULT hr = m_pDropTarget->DragLeave();
			if (m_DragLeave != S_OK) {
				m_DragLeave = hr;
			}
		}
	}
	return m_DragLeave;
}

STDMETHODIMP CteWebBrowser::Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	HRESULT hr = E_NOTIMPL;
	DWORD dwEffect = *pdwEffect;
	if (g_pDropTargetHelper) {
		g_pDropTargetHelper->DragLeave();
	}
	if (m_dwEffectTE) {
		GetDragItems(&m_pDragItems, pDataObj);
		hr = DragSub(TE_OnDrop, this, m_pDragItems, &m_grfKeyState, pt, pdwEffect);
	}
	if (m_pDropTarget && m_dwEffect) {
		*pdwEffect = dwEffect;
		if (m_pDropTarget->Drop(pDataObj, m_grfKeyState, pt, pdwEffect) == S_OK) {
			hr = S_OK;
		}
	}
	DragLeave();
	return hr;
}

HWND CteWebBrowser::get_HWND()
{
	HWND hwnd = 0;
	teGetWindow(m_pWebBrowser, &hwnd);
	return hwnd;
}

BOOL CteWebBrowser::IsBusy()
{
	VARIANT_BOOL vb;
	m_pWebBrowser->get_Busy(&vb);
	if (vb) {
		return vb;
	}
	READYSTATE rs;
	m_pWebBrowser->get_ReadyState(&rs);
	return rs < READYSTATE_INTERACTIVE;
}

VOID CteWebBrowser::write(LPWSTR pszPath)
{
	IDispatch *pdisp = NULL;
	IHTMLDocument2 *pDoc = NULL;
	ClearEvents();
	do {
		Sleep(1000);
		if (g_pWebBrowser->m_pWebBrowser->get_Document(&pdisp) == S_OK && pdisp) {
			pdisp->QueryInterface(IID_PPV_ARGS(&pDoc));
			pdisp->Release();
		}
	} while (!pDoc);
	SAFEARRAY *psa = SafeArrayCreateVector(VT_VARIANT, 0, 1);
	if (psa) {
		VARIANT *pv;
		if (::SafeArrayAccessData(psa, (LPVOID *)&pv) == S_OK) {
			teSetSZ(pv, pszPath);
			::SafeArrayUnaccessData(psa);
			pDoc->write(psa);
			VariantClear(pv);
		}
		::SafeArrayDestroy(psa);
	}
}

void CteWebBrowser::Close()
{
	try {
		if (m_pWebBrowser) {
			if (m_hwndParent != g_hwndMain) {
				teDelPropertyAtLLX(g_pSubWindows, (LONGLONG)m_hwndParent);
			}
			m_pWebBrowser->Quit();
			IOleObject *pOleObject;
			if SUCCEEDED(m_pWebBrowser->QueryInterface(IID_PPV_ARGS(&pOleObject))) {
				RECT rc;
				SetRectEmpty(&rc);
				pOleObject->SetClientSite(NULL);
				pOleObject->DoVerb(OLEIVERB_HIDE, NULL, NULL, 0, m_hwndParent, &rc);
				pOleObject->Close(OLECLOSE_NOSAVE);
				pOleObject->Release();
			}
			teUnadviseAndRelease(m_pWebBrowser, DIID_DWebBrowserEvents2, &m_dwCookie);
			m_pWebBrowser = NULL;
		}
		SafeRelease(&m_pDropTarget);
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"WebBrowser::Close";
#endif
	}
}

// CteTabCtrl

CteTabCtrl::CteTabCtrl()
{
	m_cRef = 1;
	m_nLockUpdate = 0;
	m_bVisible = FALSE;
	::ZeroMemory(&m_si, sizeof(m_si));
	m_si.cbSize = sizeof(m_si);
}

void CteTabCtrl::Init()
{
	m_nIndex = -1;
	m_bEmpty = FALSE;
	VariantInit(&m_vData);
	m_param[TE_Left] = 0;
	m_param[TE_Top] = 0;
	m_param[TE_Width] = 100;
	m_param[TE_Height] = 100;
	m_param[TE_Flags] = TCS_FOCUSNEVER | TCS_HOTTRACK | TCS_MULTILINE | TCS_RAGGEDRIGHT | TCS_SCROLLOPPOSITE | TCS_TOOLTIPS;
	m_param[TC_Align] = 0;
	m_param[TC_TabWidth] = 0;
	m_param[TC_TabHeight] = 0;
	m_nScrollWidth = 0;
	m_DefTCProc = NULL;
	m_DefBTProc = NULL;
	m_DefSTProc = NULL;
	m_nRedraw = 0;
}

VOID CteTabCtrl::GetItem(int i, VARIANT *pVarResult)
{
	CteShellBrowser *pSB = GetShellBrowser(i);
	if (pSB) {
		teSetObject(pVarResult, pSB);
	}
}

VOID CteTabCtrl::Show(BOOL bVisible)
{
	bVisible &= 1;
	if (bVisible) {
		if (!g_pTC || !g_pTC->m_bVisible) {
			g_pTC = this;
		}
		ArrangeWindow();
	}
	if (bVisible ^ m_bVisible) {
		if (m_bVisible) {
			MoveWindow(m_hwndStatic, -1, -1, 0, 0, FALSE);
			for (int i = TabCtrl_GetItemCount(m_hwnd); i--;) {
				CteShellBrowser *pSB = GetShellBrowser(i);
				if (pSB) {
					pSB->Show(FALSE, 1);
				}
			}
		}
		m_bVisible = bVisible;
		DoFunc(TE_OnVisibleChanged, this, E_NOTIMPL);
	}
}

BOOL CteTabCtrl::Close(BOOL bForce)
{
	if (CanClose(this) || bForce) {
		int nCount = TabCtrl_GetItemCount(m_hwnd);
		while (nCount--) {
			CteShellBrowser *pSB = GetShellBrowser(0);
			if (pSB) {
				pSB->Close(TRUE);
			}
		}
		Show(FALSE);
		if (m_nRedraw) {
			KillTimer(m_hwnd, (UINT_PTR)this);
		}
		RevokeDragDrop(m_hwnd);
		SetWindowLongPtr(m_hwnd, GWLP_WNDPROC, (LONG_PTR)m_DefTCProc);
		DestroyWindow(m_hwnd);
		m_hwnd = NULL;

		RevokeDragDrop(m_hwndButton);
		SetWindowLongPtr(m_hwndButton, GWLP_WNDPROC, (LONG_PTR)m_DefBTProc);
		DestroyWindow(m_hwndButton);
		m_hwndButton = NULL;

		SetWindowLongPtr(m_hwndStatic, GWLP_WNDPROC, (LONG_PTR)m_DefSTProc);
		DestroyWindow(m_hwndStatic);
		m_hwndStatic = NULL;
		m_bEmpty = TRUE;
		if (this == g_pTC) {
			for (UINT i = 0; i < g_ppTC.size(); i++) {
				CteTabCtrl *pTC = g_ppTC[i];
				if (!pTC->m_bEmpty && pTC->m_bVisible) {
					g_pTC =  pTC;
					break;
				}
			}
		}
		return TRUE;
	}
	return FALSE;
}

VOID CteTabCtrl::SetItemSize()
{
	DWORD dwSize = MAKELPARAM(m_param[TC_TabWidth] - m_nScrollWidth, m_param[TC_TabHeight]);
	if (dwSize != m_dwSize) {
		SendMessage(m_hwnd, TCM_SETITEMSIZE, 0, dwSize);
		m_dwSize = dwSize;
	}
}

DWORD CteTabCtrl::GetStyle()
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TCS_FOCUSNEVER | m_param[TE_Flags];
	if ((dwStyle & (TCS_SCROLLOPPOSITE | TCS_BUTTONS)) == TCS_SCROLLOPPOSITE) {
		if (m_param[TC_Align] == 4 || m_param[TC_Align] == 5) {
			dwStyle |= TCS_BUTTONS;
		}
	}
	if (dwStyle & TCS_BUTTONS) {
		if (dwStyle & TCS_BOTTOM && m_param[TC_Align] > 1) {
			dwStyle &= ~TCS_BOTTOM;
		}
	}
	if (dwStyle & TCS_SCROLLOPPOSITE) {
		if ((dwStyle & TCS_BUTTONS) || !(dwStyle & TCS_MULTILINE)) {
			dwStyle &= ~TCS_SCROLLOPPOSITE;
		}
	}
	return dwStyle;
}

VOID CteTabCtrl::CreateTC()
{
	m_hwnd = CreateWindowEx(
		WS_EX_CONTROLPARENT | WS_EX_COMPOSITED, //Extended style
		WC_TABCONTROL, // Class Name
		NULL, // Window Name
		GetStyle(), // Window Style
		CW_USEDEFAULT,    // X coordinate
		CW_USEDEFAULT,    // Y coordinate
		CW_USEDEFAULT,    // Width
		CW_USEDEFAULT,    // Height
		m_hwndButton, // Parent window handle
		(HMENU)0, // Child window identifier
		hInst, // Instance handle
		NULL); // WM_CREATE Parameter
	SetWindowLong(m_hwnd, GWL_STYLE, GetStyle());
	ArrangeWindow();
	SetItemSize();
	SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)this);
	m_DefTCProc = (WNDPROC)SetWindowLongPtr(m_hwnd, GWLP_WNDPROC, (LONG_PTR)TETCProc);
	RegisterDragDrop(m_hwnd, m_pDropTarget2);
	BringWindowToTop(m_hwnd);
}

BOOL CteTabCtrl::Create()
{
	m_hwndStatic = CreateWindowEx(
		0, WC_STATIC, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | SS_NOTIFY,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		g_hwndMain, (HMENU)0, hInst, NULL);
	SetWindowLongPtr(m_hwndStatic, GWLP_USERDATA, (LONG_PTR)this);
	m_DefSTProc = (WNDPROC)SetWindowLongPtr(m_hwndStatic, GWLP_WNDPROC, (LONG_PTR)TESTProc);
	BringWindowToTop(m_hwndStatic);

	m_hwndButton = CreateWindowEx(
		0, WC_BUTTON, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | BS_OWNERDRAW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		m_hwndStatic, (HMENU)0, hInst, NULL);

	SetWindowLongPtr(m_hwndButton, GWLP_USERDATA, (LONG_PTR)this);
	m_DefBTProc = (WNDPROC)SetWindowLongPtr(m_hwndButton, GWLP_WNDPROC, (LONG_PTR)TEBTProc);
	m_pDropTarget2 = new CteDropTarget2(m_hwndButton, static_cast<IDispatch *>(this));
	RegisterDragDrop(m_hwndButton, m_pDropTarget2);
	BringWindowToTop(m_hwndButton);

	CreateTC();
	DoFunc(TE_OnCreate, this, E_NOTIMPL);
	DoFunc(TE_OnViewCreated, this, E_NOTIMPL);
	ArrangeWindow();
	return TRUE;
}

CteTabCtrl::~CteTabCtrl()
{
	Close(TRUE);
	VariantClear(&m_vData);
	SafeRelease(&m_pDropTarget2);
}

STDMETHODIMP CteTabCtrl::QueryInterface(REFIID riid, void **ppvObject)
{
	static const QITAB qit[] =
	{
		QITABENT(CteTabCtrl, IDispatch),
		QITABENT(CteTabCtrl, IDispatchEx),
		{ 0 },
	};
	if (IsEqualIID(riid, g_ClsIdTC)) {
		*ppvObject = this;
		AddRef();
		return S_OK;
	}
	return QISearch(this, qit, riid, ppvObject);
}

STDMETHODIMP_(ULONG) CteTabCtrl::AddRef()
{
	return ::InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CteTabCtrl::Release()
{
	if (::InterlockedDecrement(&m_cRef) == 0) {
		delete this;
		return 0;
	}

	return m_cRef;
}

STDMETHODIMP CteTabCtrl::GetTypeInfoCount(UINT *pctinfo)
{
	*pctinfo = 0;
	return S_OK;
}

STDMETHODIMP CteTabCtrl::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteTabCtrl::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	return teGetDispId(methodTC, _countof(methodTC), g_maps[MAP_TC], *rgszNames, rgDispId, TRUE);
}

STDMETHODIMP CteTabCtrl::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	try {
		IUnknown *punk = NULL;

		int nArg = pDispParams ? pDispParams->cArgs - 1 : -1;
		if (pVarResult) {
			VariantInit(pVarResult);
		}
		switch(dispIdMember) {
			//Data
			case 1:
				if (nArg >= 0) {
					VariantClear(&m_vData);
					VariantCopy(&m_vData, &pDispParams->rgvarg[nArg]);
				}
				if (pVarResult) {
					VariantCopy(pVarResult, &m_vData);
				}
				return S_OK;
			//hwnd
			case 2:
				teSetPtr(pVarResult, m_hwnd);
				return S_OK;
			//Type
			case 3:
				teSetLong(pVarResult, CTRL_TC);
				return S_OK;
			//HitTest (Screen coordinates)
			case 6:
				if (nArg >= 0 && pVarResult) {
					TCHITTESTINFO info;
					GetPointFormVariant(&info.pt, &pDispParams->rgvarg[nArg]);
					UINT flags = nArg >= 1 ? GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]) : TCHT_ONITEM;
					teSetLong(pVarResult, static_cast<int>(DoHitTest(this, info.pt, flags)));
					if (g_param[TE_Tab] && pVarResult->lVal < 0) {
						ScreenToClient(m_hwnd, &info.pt);
						info.flags = flags;
						pVarResult->lVal = TabCtrl_HitTest(m_hwnd, &info);
						if (!(info.flags & flags)) {
							pVarResult->lVal = -1;
						}
					}
					if (nArg == 0) {
						GetItem(GetIntFromVariantClear(pVarResult), pVarResult);
					}
				}
				return S_OK;
			//Move
			case 7:
				if (nArg >= 1 && !g_nLockUpdate) {
					int nSrc, nDest;
					nDest = GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]);
					CteShellBrowser *pSB;
					if (FindUnknown(&pDispParams->rgvarg[nArg], &punk)) {
						if SUCCEEDED(punk->QueryInterface(g_ClsIdSB, (LPVOID *)&pSB)) {
							pSB->m_pTC->Move(pSB->GetTabIndex(), nDest, this);
							pSB->Release();
							return S_OK;
						}
					}
					nSrc = GetIntFromVariant(&pDispParams->rgvarg[nArg]);
					if (nArg >= 2) {
						if (FindUnknown(&pDispParams->rgvarg[nArg - 2], &punk)) {
							CteTabCtrl *pTC;
							if SUCCEEDED(punk->QueryInterface(g_ClsIdTC, (LPVOID *)&pTC)) {
								Move(nSrc, nDest, pTC);
								pTC->Release();
								return S_OK;
							}
						}
					}
					Move(nSrc, nDest, this);
				}
				return S_OK;
			//Selected
			case 8:
				if (nArg >= 0) {
					int nIndex = -1;
					if (FindUnknown(&pDispParams->rgvarg[nArg], &punk)) {
						CteShellBrowser *pSB;
						if SUCCEEDED(punk->QueryInterface(g_ClsIdSB, (LPVOID *)&pSB)) {
							nIndex = pSB->GetTabIndex();
							pSB->Release();
						}
					}
					if (nIndex >= 0) {
						TabCtrl_SetCurSel(m_hwnd, nIndex);
					}
				}
				if (pVarResult) {
					CteShellBrowser *pSB = GetShellBrowser(m_nIndex);
					if (!pSB) {
						pSB = GetNewShellBrowser(this);
					}
					teSetObject(pVarResult, pSB);
				}
				return S_OK;
			//Close
			case 9:
				Close(FALSE);
				return S_OK;
			//SelectedIndex
			case 10:
				if (nArg >= 0) {
					TabCtrl_SetCurSel(m_hwnd, GetIntFromVariant(&pDispParams->rgvarg[nArg]));
				}
				if (pVarResult) {
					teSetLong(pVarResult, TabCtrl_GetCurSel(m_hwnd));
				}
				return S_OK;
			//Visible
			case 11:
				if (nArg >= 0) {
					Show(GetIntFromVariant(&pDispParams->rgvarg[nArg]));
				}
				teSetBool(pVarResult, m_bVisible);
				return S_OK;
			//Id
			case 12:
				teSetLong(pVarResult, m_nTC);
				return S_OK;
			//LockUpdate
			case 13:
				LockUpdate(nArg >= 0 ? GetIntFromVariant(&pDispParams->rgvarg[nArg]) : TRUE);
				return S_OK;
			//UnlockUpdate
			case 14:
				UnlockUpdate();
				return S_OK;
			//Item
			case DISPID_TE_ITEM:
				if (nArg >= 0) {
					GetItem(GetIntFromVariant(&pDispParams->rgvarg[nArg]), pVarResult);
				}
				return S_OK;
			//Count
			case DISPID_TE_COUNT:
				if (pVarResult) {
					int nCount = TabCtrl_GetItemCount(m_hwnd);
					if (nCount == 1) {
						CteShellBrowser *pSB = GetShellBrowser(0);
						if (!pSB || pSB->m_bEmpty) {
							nCount = 0;
						}
					}
					teSetLong(pVarResult, nCount);
				}
				return S_OK;
			//_NewEnum
			case DISPID_NEWENUM:
				teSetObjectRelease(pVarResult, new CteDispatch(this, 0));
				return S_OK;
			//this
			case DISPID_VALUE:
				teSetObject(pVarResult, this);
				return S_OK;
		}
		if (dispIdMember >= TE_OFFSET && dispIdMember <= TE_OFFSET + TC_TabHeight) {
			if (nArg >= 0 && dispIdMember != TE_OFFSET) {
				m_param[dispIdMember - TE_OFFSET] = GetIntFromVariantPP(&pDispParams->rgvarg[nArg], dispIdMember - TE_OFFSET);
				if (m_hwnd) {
					if (dispIdMember == TE_OFFSET + TE_Flags) {
						DWORD dwStyle = GetWindowLong(m_hwnd, GWL_STYLE);
						DWORD dwPrev = GetStyle();
						if ((dwStyle ^ dwPrev) & 0x7fff) {
							if ((dwStyle ^ dwPrev) & (TCS_SCROLLOPPOSITE | TCS_BUTTONS | TCS_TOOLTIPS)) {
								//Remake
								int nTabs = TabCtrl_GetItemCount(m_hwnd);
								int nSel = TabCtrl_GetCurFocus(m_hwnd);
								TC_ITEM *tabs = new TC_ITEM[nTabs];
								LPWSTR *ppszText = new LPWSTR[nTabs];
								for (int i = nTabs; i-- > 0;) {
									ppszText[i] = new WCHAR[MAX_PATH];
									tabs[i].cchTextMax = MAX_PATH;
									tabs[i].pszText = ppszText[i];
									tabs[i].mask = TCIF_IMAGE | TCIF_PARAM | TCIF_TEXT;
									TabCtrl_GetItem(m_hwnd, i, &tabs[i]);
								}
								HIMAGELIST hImage = TabCtrl_GetImageList(m_hwnd);
								HFONT hFont = (HFONT)SendMessage(m_hwnd, WM_GETFONT, 0, 0);
								RevokeDragDrop(m_hwnd);
								SetWindowLongPtr(m_hwnd, GWLP_WNDPROC, (LONG_PTR)m_DefTCProc);
								DestroyWindow(m_hwnd);

								CreateTC();
								SendMessage(m_hwnd, WM_SETFONT, (WPARAM)hFont, 0);
								TabCtrl_SetImageList(m_hwnd, hImage);
								for (int i = 0; i < nTabs; i++) {
									TabCtrl_InsertItem(m_hwnd, i, &tabs[i]);
									delete [] ppszText[i];
								}
								delete [] ppszText;
								delete [] tabs;
								TabCtrl_SetCurSel(m_hwnd, nSel);
								DoFunc(TE_OnViewCreated, this, E_NOTIMPL);
							}
						}
					} else if (dispIdMember == TE_OFFSET + TC_TabWidth || dispIdMember == TE_OFFSET + TC_TabHeight) {
						SetItemSize();
					}
					SetWindowLong(m_hwnd, GWL_STYLE, GetStyle());
					ArrangeWindow();
				}
			}
			if (pVarResult) {
				int i = m_param[dispIdMember - TE_OFFSET];
				if (dispIdMember >= TE_OFFSET + TE_Left && dispIdMember <= TE_OFFSET + TE_Height) {
					if (i & 0x3fff) {
						wchar_t pszSize[20];
						swprintf_s(pszSize, 20, L"%g%%", ((float)(m_param[dispIdMember - TE_OFFSET])) / 100);
						teSetSZ(pVarResult, pszSize);
						return S_OK;
					}
					teSetLong(pVarResult, i / 0x4000);
					return S_OK;
				}
				teSetLong(pVarResult, i);
			}
			return S_OK;
		}
		if (dispIdMember >= DISPID_COLLECTION_MIN && dispIdMember <= DISPID_TE_MAX) {
			GetItem(dispIdMember - DISPID_COLLECTION_MIN, pVarResult);
			return S_OK;
		}
	} catch (...) {
		return teException(pExcepInfo, "TabControl", methodTC, dispIdMember);
	}
	return DISP_E_MEMBERNOTFOUND;
}

//IDispatchEx
STDMETHODIMP CteTabCtrl::GetDispID(BSTR bstrName, DWORD grfdex, DISPID *pid)
{
	return teGetDispId(methodTC, _countof(methodTC), g_maps[MAP_TC], bstrName, pid, TRUE);
}

STDMETHODIMP CteTabCtrl::InvokeEx(DISPID id, LCID lcid, WORD wFlags, DISPPARAMS *pdp, VARIANT *pvarRes, EXCEPINFO *pei, IServiceProvider *pspCaller)
{
	return Invoke(id, IID_NULL, lcid, wFlags, pdp, pvarRes, pei, NULL);
}

STDMETHODIMP CteTabCtrl::DeleteMemberByName(BSTR bstrName, DWORD grfdex)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteTabCtrl::DeleteMemberByDispID(DISPID id)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteTabCtrl::GetMemberProperties(DISPID id, DWORD grfdexFetch, DWORD *pgrfdex)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteTabCtrl::GetMemberName(DISPID id, BSTR *pbstrName)
{
	return teGetMemberName(id, pbstrName);
}

STDMETHODIMP CteTabCtrl::GetNextDispID(DWORD grfdex, DISPID id, DISPID *pid)
{
	*pid = (id < DISPID_COLLECTION_MIN) ? DISPID_COLLECTION_MIN : id + 1;
	return *pid < TabCtrl_GetItemCount(m_hwnd) + DISPID_COLLECTION_MIN ? S_OK : S_FALSE;
}

STDMETHODIMP CteTabCtrl::GetNameSpaceParent(IUnknown **ppunk)
{
	return E_NOTIMPL;
}

VOID CteTabCtrl::Move(int nSrc, int nDest, CteTabCtrl *pDestTab)
{
	if (nDest < 0) {
		nDest = TabCtrl_GetItemCount(m_hwnd) - 1;
		if (nDest < 0) {
			nDest = 0;
		}
	}
	BOOL bOtherTab = m_hwnd != pDestTab->m_hwnd;
	if (nSrc != nDest || bOtherTab) {
		int nIndex = TabCtrl_GetCurSel(m_hwnd);
		WCHAR szText[MAX_PATH];
		TC_ITEM tcItem = { TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM, 0, 0, szText, MAX_PATH };
		TabCtrl_GetItem(m_hwnd, nSrc, &tcItem);
		SendMessage(m_hwnd, TCM_DELETEITEM, nSrc, 1/* Don't delete SB flag */);
		UINT ui = static_cast<UINT>(tcItem.lParam) - 1;
		if (ui < g_ppSB.size()) {
			CteShellBrowser *pSB = g_ppSB[ui];
			teSetParent(pSB->m_hwnd, pDestTab->m_hwndStatic);
			pSB->m_pTC = pDestTab;
			TabCtrl_InsertItem(pDestTab->m_hwnd, nDest, &tcItem);
			if (nSrc == nIndex) {
				m_nIndex = -1;
				if (bOtherTab) {
					if (nSrc > 0) {
						nSrc--;
					}
					TabCtrl_SetCurSel(m_hwnd, nSrc);
				} else {
					TabCtrl_SetCurSel(m_hwnd, nDest);
				}
			}
		}
		if (bOtherTab) {
			pDestTab->m_nIndex = nDest;
			TabCtrl_SetCurSel(pDestTab->m_hwnd, nDest);
			ArrangeWindow();
		}
		TabChanged(TRUE);
	}
}

VOID CteTabCtrl::LockUpdate(BOOL bTE)
{
	if (InterlockedIncrement(&m_nLockUpdate) == 1 && !g_bDragging) {
		SendMessage(m_hwndStatic, WM_SETREDRAW, FALSE, 0);
		SendMessage(m_hwnd, WM_SETREDRAW, FALSE, 0);
		if (bTE) {
			teSetRedraw(FALSE);
		}
	}
	if (!bTE) {
		SendMessage(g_pWebBrowser->m_hwndBrowser, WM_SETREDRAW, TRUE, 0);
	}
}

VOID CteTabCtrl::UnlockUpdate()
{
	if (::InterlockedDecrement(&m_nLockUpdate) > 0) {
		return;
	}
	m_nLockUpdate = 0;
	if (m_nRedraw & TEREDRAW_NAVIGATE) {
		SetTimer(g_hwndMain, TET_Redraw, 500, teTimerProc);
		return;
	}
	m_nRedraw |= TEREDRAW_NORMAL;
	RedrawUpdate();
}

VOID CteTabCtrl::RedrawUpdate()
{
	if (m_nRedraw) {
		if (g_nLockUpdate) {
			SetTimer(g_hwndMain, TET_Redraw, 500, teTimerProc);
			return;
		}
		m_nRedraw = 0;
		SendMessage(m_hwndStatic, WM_SETREDRAW, TRUE, 0);
		SendMessage(m_hwnd, WM_SETREDRAW, TRUE, 0);
		CteShellBrowser *pSB = GetShellBrowser(m_nIndex);
		if (pSB) {
			pSB->SetRedraw(TRUE);
		}
		RedrawWindow(m_hwndStatic, NULL, 0, RDW_NOERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
		teSetRedraw(TRUE);
	}
}

VOID CteTabCtrl::TabChanged(BOOL bSameTC)
{
	if (bSameTC) {
		m_nIndex = TabCtrl_GetCurSel(m_hwnd);
	}
	CteShellBrowser *pSB = GetShellBrowser(m_nIndex);
	if (pSB) {
		pSB->SetStatusTextSB(NULL);
		DoFunc(TE_OnSelectionChanged, pSB->m_pTC, E_NOTIMPL);
		ArrangeWindow();
	}
}

CteShellBrowser* CteTabCtrl::GetShellBrowser(int nPage)
{
	if (nPage < 0 || !m_hwnd) {
		return NULL;
	}
	TC_ITEM tcItem;
	tcItem.mask = TCIF_PARAM;
	TabCtrl_GetItem(m_hwnd, nPage, &tcItem);
	UINT ui = static_cast<UINT>(tcItem.lParam) - 1;
	if (ui < g_ppSB.size()) {
		return g_ppSB[ui];
	}
	return NULL;
}

// CteFolderItems

CteFolderItems::CteFolderItems(IDataObject *pDataObj, FolderItems *pFolderItems)
{
	m_cRef = 1;
	m_pDataObj = pDataObj;
	m_oFolderItems = NULL;
	if (m_pDataObj) {
		m_pDataObj->AddRef();
	} else {
		GetNewArray(&m_oFolderItems);
	}
	m_pidllist = NULL;
	m_nCount = -1;
	m_bsText = NULL;
	m_pFolderItems = pFolderItems;
	m_nIndex = 0;
	m_dwEffect = (DWORD)-1;
	m_bUseILF = TRUE;
	VariantInit(&m_vData);
}

CteFolderItems::~CteFolderItems()
{
	Clear();
}

VOID CteFolderItems::Clear()
{
	if (m_pidllist) {
		for (int i = m_nCount; i >= 0; i--) {
			teCoTaskMemFree(m_pidllist[i]);
		}
		delete [] m_pidllist;
		m_pidllist = NULL;
	}
	SafeRelease(&m_oFolderItems);
	SafeRelease(&m_pDataObj);
	SafeRelease(&m_pFolderItems);
	teSysFreeString(&m_bsText);
	VariantClear(&m_vData);
}

VOID CteFolderItems::Regenerate(BOOL bFull)
{
	if (!m_oFolderItems) {
		long nCount;
		if SUCCEEDED(get_Count(&nCount)) {
			IDispatch *oFolderItems = NULL;
			GetNewArray(&oFolderItems);
			VARIANT v;
			v.vt = VT_I4;
			FolderItem *pid;
			for (v.lVal = 0; v.lVal < nCount; v.lVal++) {
				if (Item(v, &pid) == S_OK) {
					if (bFull) {
						CteFolderItem *pid1;
						teQueryFolderItem(&pid, &pid1);
						if (pid1->m_pidl) {
							if (pid1->m_v.vt != VT_EMPTY || GetVarPathFromFolderItem(pid, &pid1->m_v)) {
								if (pid1->m_v.vt == VT_BSTR) {
									if (teIsFileSystem(pid1->m_v.bstrVal)) {
										LPITEMIDLIST pidl;
										if (teGetIDListFromVariant(&pidl, &pid1->m_v)) {
											if (!ILIsEqual(pidl, pid1->m_pidl)) {
												teILCloneReplace(&pid1->m_pidl, pidl);
											}
											teILFreeClear(&pidl);
										}
									}
								}
							}
						}
						pid1->Release();
					}
					teArrayPush(oFolderItems, pid);
					pid->Release();
				}
			}
			Clear();
			m_oFolderItems = oFolderItems;
		}
	}
}

VOID CteFolderItems::ItemEx(long nIndex, VARIANT *pVarResult, VARIANT *pVarNew)
{
	VARIANT v;
	teSetLong(&v, nIndex);
	if (pVarNew) {
		teSysFreeString(&m_bsText);
		Regenerate(FALSE);
		if (m_oFolderItems) {
			IUnknown *punk;
			CteFolderItem *pid = NULL;
			if (FindUnknown(pVarNew, &punk)) {
				punk->QueryInterface(g_ClsIdFI, (LPVOID *)&pid);
			}
			if (pid == NULL) {
				pid = new CteFolderItem(pVarNew);
			}
			VARIANT vNew;
			teSetObjectRelease(&vNew, pid);
			if (nIndex >= 0) {
				tePutPropertyAt(m_oFolderItems, nIndex, &vNew);
			} else {
				teExecMethod(m_oFolderItems, L"push", NULL, -1, &vNew);
				VariantClear(&vNew);
			}
		}
	}
	if (pVarResult) {
		FolderItem *pid = NULL;
		if SUCCEEDED(Item(v, &pid)) {
			teSetObjectRelease(pVarResult, pid);
		}
	}
}

VOID CteFolderItems::AdjustIDListEx()
{
	m_bUseILF = TRUE;
	if (!m_pidllist && m_oFolderItems) {
		get_Count(&m_nCount);
		m_pidllist = new LPITEMIDLIST[m_nCount + 1];
		m_pidllist[0] = NULL;
		VARIANT v;
		teSetLong(&v, m_nCount);
		FolderItem *pid = NULL;
		while (--v.lVal >= 0) {
			if (Item(v, &pid) == S_OK) {
				if (!teGetIDListFromObject(pid, &m_pidllist[v.lVal + 1])) {
					m_pidllist[v.lVal + 1] = ::ILClone(g_pidls[CSIDL_DESKTOP]);
					m_bUseILF = FALSE;
				}
				pid->Release();
			}
		}
	}
	m_bUseILF &= AdjustIDList(m_pidllist, m_nCount);
}

STDMETHODIMP CteFolderItems::QueryInterface(REFIID riid, void **ppvObject)
{
	static const QITAB qit[] =
	{
		QITABENTMULTI(CteFolderItems, IDispatch, IDispatchEx),
		QITABENT(CteFolderItems, IDispatchEx),
		QITABENT(CteFolderItems, FolderItems),
		{ 0 },
	};
	*ppvObject = NULL;
	if (IsEqualIID(riid, IID_IDataObject)) {
		if (!m_pDataObj && m_pidllist && m_nCount) {
			m_bUseILF = AdjustIDList(m_pidllist, m_nCount);
			IShellFolder *pSF;
			if (GetShellFolder(&pSF, m_pidllist[0])) {
				pSF->GetUIObjectOf(g_hwndMain, m_nCount, const_cast<LPCITEMIDLIST *>(&m_pidllist[1]), IID_IDataObject, NULL, (LPVOID *)&m_pDataObj);
				pSF->Release();
			}
		}
		if (m_pDataObj) {
			if (m_pDataObj->QueryGetData(&UNICODEFormat) == S_OK || m_pDataObj->QueryGetData(&TEXTFormat) == S_OK) {
				return m_pDataObj->QueryInterface(riid, ppvObject);
			}
		}
		*ppvObject = static_cast<IDataObject *>(this);
		AddRef();
		return S_OK;
	}
	return QISearch(this, qit, riid, ppvObject);
}

STDMETHODIMP_(ULONG) CteFolderItems::AddRef()
{
	return ::InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CteFolderItems::Release()
{
	if (::InterlockedDecrement(&m_cRef) == 0) {
		delete this;
		return 0;
	}
	return m_cRef;
}

STDMETHODIMP CteFolderItems::GetTypeInfoCount(UINT *pctinfo)
{
	*pctinfo = 0;
	return S_OK;
}

STDMETHODIMP CteFolderItems::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	if (m_pFolderItems) {
		return m_pFolderItems->GetTypeInfo(iTInfo, lcid, ppTInfo);
	}
	return E_NOTIMPL;
}

STDMETHODIMP CteFolderItems::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	if (m_pFolderItems) {
		return m_pFolderItems->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgDispId);
	}
	return teGetDispId(methodFIs, _countof(methodFIs), g_maps[MAP_FIs], *rgszNames, rgDispId, TRUE);
}

STDMETHODIMP CteFolderItems::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	try {
		int nArg = pDispParams ? pDispParams->cArgs - 1 : -1;
		if (pVarResult) {
			VariantInit(pVarResult);
		}
		if (dispIdMember >= DISPID_COLLECTION_MIN && dispIdMember <= DISPID_TE_MAX) {
			ItemEx(dispIdMember - DISPID_COLLECTION_MIN, pVarResult, nArg >= 0 ? &pDispParams->rgvarg[nArg] : NULL);
			return S_OK;
		}
		if (m_pFolderItems) {
			return m_pFolderItems->Invoke(dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
		}
		switch(dispIdMember) {
			//Application
			case 2:
				IDispatch *pdisp;
				if SUCCEEDED(get_Application(&pdisp)) {
					teSetObject(pVarResult, pdisp);
					pdisp->Release();
				}
				return S_OK;
			//Parent
			case 3:
				return S_OK;
			//Item
			case DISPID_TE_ITEM:
				if (nArg >= 0) {
					ItemEx(GetIntFromVariant(&pDispParams->rgvarg[nArg]), pVarResult, nArg >= 1 ? &pDispParams->rgvarg[nArg - 1] : NULL);
				}
				return S_OK;
			//Count
			case DISPID_TE_COUNT:
				if (pVarResult) {
					pVarResult->vt = VT_I4;
					get_Count(&pVarResult->lVal);
				}
				return S_OK;
			//_NewEnum
			case DISPID_NEWENUM:
				IUnknown *punk;
				if SUCCEEDED(_NewEnum(&punk)) {
					teSetObject(pVarResult, punk);
					punk->Release();
				}
				return S_OK;
			//AddItem
			case 8:
				if (nArg >= 0) {
					ItemEx(-1, NULL, &pDispParams->rgvarg[nArg]);
				}
				return S_OK;
			//hDrop
			case 9:
				if (pVarResult) {
					int pi[3] = { 0 };
					for (int i = nArg; i >= 0; i--) {
						if (i < 3) {
							pi[i] = GetIntFromVariant(&pDispParams->rgvarg[nArg - i]);
						}
					}
					teSetPtr(pVarResult, GethDrop(pi[0], pi[1], pi[2]));
				}
				return S_OK;
			//GetData
			case 10:
				if (pVarResult) {
					IDataObject *pDataObj;
					if SUCCEEDED(QueryInterface(IID_PPV_ARGS(&pDataObj))) {
						STGMEDIUM Medium;
						if (pDataObj->GetData(&UNICODEFormat, &Medium) == S_OK) {
							teSetSZ(pVarResult, static_cast<LPCWSTR>(GlobalLock(Medium.hGlobal)));
							GlobalUnlock(Medium.hGlobal);
							ReleaseStgMedium(&Medium);
						} else if (pDataObj->GetData(&TEXTFormat, &Medium) == S_OK) {
							pVarResult->bstrVal = teMultiByteToWideChar(CP_ACP, static_cast<LPCSTR>(GlobalLock(Medium.hGlobal)), -1);
							pVarResult->vt = VT_BSTR;
							GlobalUnlock(Medium.hGlobal);
							ReleaseStgMedium(&Medium);
						}
						pDataObj->Release();
					}
				}
				return S_OK;
			//SetData
			case 11:
				if (nArg >= 0) {
					teSysFreeString(&m_bsText);
					VARIANT v;
					teVariantChangeType(&v, &pDispParams->rgvarg[0], VT_BSTR);
					m_bsText = v.bstrVal;
					v.bstrVal = NULL;
				}
				return S_OK;
			//Index
			case DISPID_TE_INDEX:
				if (nArg >= 0) {
					m_nIndex = GetIntFromVariant(&pDispParams->rgvarg[nArg]);
				}
				teSetLong(pVarResult, m_nIndex);
				return S_OK;
			//dwEffect
			case 0x10000001:
				if (nArg >= 0) {
					m_dwEffect = GetIntFromVariant(&pDispParams->rgvarg[nArg]);
				}
				teSetLong(pVarResult, m_dwEffect);
				return S_OK;
			//pdwEffect
			case 0x10000002:
				punk = new CteMemory(sizeof(int), &m_dwEffect, 1, L"DWORD");
				teSetObject(pVarResult, punk);
				punk->Release();
				return S_OK;
			//Data
			case 0x10000003:
				if (nArg >= 0) {
					VariantClear(&m_vData);
					VariantCopy(&m_vData, &pDispParams->rgvarg[nArg]);
				}
				if (pVarResult) {
					VariantCopy(pVarResult, &m_vData);
				}
				return S_OK;
			//this
			case DISPID_VALUE:
				teSetObject(pVarResult, this);
				return S_OK;
		}
	} catch (...) {
		return teException(pExcepInfo, "FolderItems", methodFIs, dispIdMember);
	}
	return DISP_E_MEMBERNOTFOUND;
}

STDMETHODIMP CteFolderItems::get_Count(long *plCount)
{
	if (m_pFolderItems) {
		return m_pFolderItems->get_Count(plCount);
	}
	if (m_oFolderItems) {
		*plCount = teGetObjectLength(m_oFolderItems);
		return S_OK;
	}
	if (m_nCount < 0) {
		if (m_pDataObj) {
			STGMEDIUM Medium;
			if (m_pDataObj->GetData(&IDLISTFormat, &Medium) == S_OK) {
				CIDA *pIDA = (CIDA *)GlobalLock(Medium.hGlobal);
				m_nCount = pIDA ? pIDA->cidl : 0;
				GlobalUnlock(Medium.hGlobal);
				ReleaseStgMedium(&Medium);
			} else if (m_pDataObj->GetData(&HDROPFormat, &Medium) == S_OK) {
				m_nCount = DragQueryFile((HDROP)Medium.hGlobal, (UINT)-1, NULL, 0);
				ReleaseStgMedium(&Medium);
			}
		} else {
			m_nCount = 0;
		}
	}
	*plCount = m_nCount > 0 ? m_nCount : 0;
	return S_OK;
}

STDMETHODIMP CteFolderItems::get_Application(IDispatch **ppid)
{
	if (m_pFolderItems) {
		return m_pFolderItems->get_Application(ppid);
	}
	if (g_pWebBrowser) {
		return g_pWebBrowser->m_pWebBrowser->get_Application(ppid);
	}
	return E_NOTIMPL;
}

STDMETHODIMP CteFolderItems::get_Parent(IDispatch **ppid)
{
	if (m_pFolderItems) {
		return m_pFolderItems->get_Parent(ppid);
	}
	return E_NOTIMPL;
}

STDMETHODIMP CteFolderItems::Item(VARIANT index, FolderItem **ppid)
{
	*ppid = NULL;
	int i = GetIntFromVariantClear(&index);
	if (i >= 0) {
		if (m_pFolderItems) {
			return m_pFolderItems->Item(index, ppid);
		}
		if (m_oFolderItems) {
			VARIANT v;
			VariantInit(&v);
			teGetPropertyAt(m_oFolderItems, i, &v);
			IUnknown *punk;
			if (FindUnknown(&v, &punk)) {
				HRESULT hr = punk->QueryInterface(IID_PPV_ARGS(ppid));
				VariantClear(&v);
				return hr;
			}
		}
	}
	if (!m_pidllist) {
		m_pidllist = IDListFormDataObj(m_pDataObj, &m_nCount);
		if (!m_pidllist) {
			*ppid = NULL;
			return E_FAIL;
		}
	}
	if (i >= 0 && i < m_nCount) {
		if (m_pidllist[0]) {
			LPITEMIDLIST pidl = ILCombine(m_pidllist[0], m_pidllist[i + 1]);
			GetFolderItemFromIDList(ppid, pidl);
			teCoTaskMemFree(pidl);
		} else {
			GetFolderItemFromIDList(ppid, m_pidllist[i + 1]);
		}
	} else if (i == -1) {
		if (m_pidllist[0] == NULL) {
			AdjustIDListEx();
		}
		if (m_pidllist[0]) {
			GetFolderItemFromIDList(ppid, m_pidllist[0]);
		}
	}
	return (*ppid != NULL) ? S_OK : E_FAIL;
}

STDMETHODIMP CteFolderItems::_NewEnum(IUnknown **ppunk)
{
	if (m_pFolderItems) {
		return m_pFolderItems->_NewEnum(ppunk);
	}
	*ppunk = static_cast<IDispatch *>(new CteDispatch(static_cast<FolderItems *>(this), 0));
	return S_OK;
}

//IDispatchEx
STDMETHODIMP CteFolderItems::GetDispID(BSTR bstrName, DWORD grfdex, DISPID *pid)
{
	return teGetDispId(methodFIs, _countof(methodFIs), g_maps[MAP_FIs], bstrName, pid, TRUE);
}

STDMETHODIMP CteFolderItems::InvokeEx(DISPID id, LCID lcid, WORD wFlags, DISPPARAMS *pdp, VARIANT *pvarRes, EXCEPINFO *pei, IServiceProvider *pspCaller)
{
	return Invoke(id, IID_NULL, lcid, wFlags, pdp, pvarRes, pei, NULL);
}

STDMETHODIMP CteFolderItems::DeleteMemberByName(BSTR bstrName, DWORD grfdex)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteFolderItems::DeleteMemberByDispID(DISPID id)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteFolderItems::GetMemberProperties(DISPID id, DWORD grfdexFetch, DWORD *pgrfdex)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteFolderItems::GetMemberName(DISPID id, BSTR *pbstrName)
{
	return teGetMemberName(id, pbstrName);
}

STDMETHODIMP CteFolderItems::GetNextDispID(DWORD grfdex, DISPID id, DISPID *pid)
{
	*pid = (id < DISPID_COLLECTION_MIN) ? DISPID_COLLECTION_MIN : id + 1;
	get_Count(&m_nCount);
	return *pid < m_nCount + DISPID_COLLECTION_MIN ? S_OK : S_FALSE;
}

STDMETHODIMP CteFolderItems::GetNameSpaceParent(IUnknown **ppunk)
{
	return E_NOTIMPL;
}

HDROP CteFolderItems::GethDrop(int x, int y, BOOL fNC)
{
	if (m_pDataObj) {
		STGMEDIUM Medium;
		if (m_pDataObj->GetData(&HDROPFormat, &Medium) == S_OK) {
			HDROP hDrop = (HDROP)Medium.hGlobal;
			if (fNC) {
				LPDROPFILES lpDropFiles = (LPDROPFILES)GlobalLock(hDrop);
				try {
					lpDropFiles->pt.x = x;
					lpDropFiles->pt.y = y;
					lpDropFiles->fNC = fNC;
				} catch (...) {
					g_nException = 0;
#ifdef _DEBUG
					g_strException = L"lpDropFiles";
#endif
				}
				GlobalUnlock(hDrop);
			}
			return hDrop;
		}
	}
	if (!m_pidllist) {
		m_pidllist = IDListFormDataObj(m_pDataObj, &m_nCount);
		if (!m_pidllist) {
			AdjustIDListEx();
			if (!m_pidllist) {
				return NULL;
			}
		}
	}
	BSTR *pbslist = new BSTR[m_nCount];
	UINT uSize = sizeof(WCHAR);
	VARIANT v;
	v.vt = VT_I4;
	for (int i = m_nCount; i-- > 0;) {
		pbslist[i] = NULL;
		v.lVal = i;
		FolderItem *pid;
		if SUCCEEDED(Item(v, &pid)) {
			BSTR bsPath;
			if SUCCEEDED(pid->get_Path(&bsPath)) {
				int nLen = ::SysStringByteLen(bsPath);
				if (nLen) {
					pbslist[i] = bsPath;
					uSize += nLen + sizeof(WCHAR);
				} else {
					::SysFreeString(bsPath);
				}
			}
			pid->Release();
		}
	}
	HDROP hDrop = (HDROP)GlobalAlloc(GHND, sizeof(DROPFILES) + uSize);
	LPDROPFILES lpDropFiles = (LPDROPFILES)GlobalLock(hDrop);
	try {
		lpDropFiles->pFiles = sizeof(DROPFILES);
		lpDropFiles->pt.x = x;
		lpDropFiles->pt.y = y;
		lpDropFiles->fNC = fNC;
		lpDropFiles->fWide = TRUE;

		LPWSTR lp = (LPWSTR)&lpDropFiles[1];
		for (int i = 0; i< m_nCount; i++) {
			if (pbslist[i]) {
				lstrcpy(lp, pbslist[i]);
				lp += (SysStringByteLen(pbslist[i]) / 2) + 1;
				::SysFreeString(pbslist[i]);
			}
		}
		*lp = 0;
		delete [] pbslist;
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"GethDrop";
#endif
	}
	GlobalUnlock(hDrop);
	return hDrop;
}

//IDataObject
STDMETHODIMP CteFolderItems::GetData(FORMATETC *pformatetcIn, STGMEDIUM *pmedium)
{
	if (m_dwEffect != (DWORD)-1 && pformatetcIn->cfFormat == DROPEFFECTFormat.cfFormat) {
		HGLOBAL hGlobal = GlobalAlloc(GHND | GMEM_SHARE, sizeof(DWORD));
		DWORD *pdwEffect = (DWORD *)GlobalLock(hGlobal);
		try {
			if (pdwEffect) {
				*pdwEffect = m_dwEffect;
			}
		} catch (...) {
			g_nException = 0;
#ifdef _DEBUG
			g_strException = L"pdwEffect";
#endif
		}
		GlobalUnlock(hGlobal);

		pmedium->tymed = TYMED_HGLOBAL;
		pmedium->hGlobal = hGlobal;
		pmedium->pUnkForRelease = NULL;
		return S_OK;
	}
	if (m_pDataObj) {
		HRESULT hr = m_pDataObj->GetData(pformatetcIn, pmedium);
		if (hr == S_OK) {
			if (g_bsClipRoot) {
				if (pformatetcIn->cfFormat == IDLISTFormat.cfFormat || pformatetcIn->cfFormat == CF_HDROP) {
					BSTR bs = NULL;
					teGetRootFromDataObj(&bs, this);
					if (teStrSameIFree(bs, g_bsClipRoot)) {
						if (g_pOnFunc[TE_OnBeforeGetData]) {
							VARIANTARG *pv = GetNewVARIANT(3);
							teSetObject(&pv[2], g_pTE);
							teSetObjectRelease(&pv[1], this);
							teSetLong(&pv[0], 5);
							Invoke4(g_pOnFunc[TE_OnBeforeGetData], NULL, 3, pv);
						}
					}
					teSysFreeString(&g_bsClipRoot);
				}
			}
			return hr;
		}
	}
	if (pformatetcIn->cfFormat == IDLISTFormat.cfFormat) {
		if (!m_pidllist) {
			m_pidllist = IDListFormDataObj(m_pDataObj, &m_nCount);
			if (!m_pidllist) {
				return DATA_E_FORMATETC;
			}
		}
		AdjustIDListEx();
		UINT uIndex = sizeof(UINT) * (m_nCount + 2);
		UINT uSize = uIndex;
		for (int i = 0; i <= m_nCount; i++) {
			uSize += ILGetSize(m_pidllist[i]);
		}
		HGLOBAL hGlobal = GlobalAlloc(GHND, uSize);
		CIDA *pIDA = (CIDA *)GlobalLock(hGlobal);
		try {
			if (pIDA) {
				pIDA->cidl = m_nCount;
				for (int i = 0; i <= m_nCount; i++) {
					pIDA->aoffset[i] = uIndex;
					UINT u = ILGetSize(m_pidllist[i]);
					char *pc = (char *)pIDA + uIndex;
					::CopyMemory(pc, m_pidllist[i], u);
					uIndex += u;
				}
			}
		} catch (...) {
			g_nException = 0;
#ifdef _DEBUG
			g_strException = L"CIDA";
#endif
		}
		GlobalUnlock(hGlobal);
		pmedium->tymed = TYMED_HGLOBAL;
		pmedium->hGlobal = hGlobal;
		pmedium->pUnkForRelease = NULL;
		return S_OK;
	}
	if (pformatetcIn->cfFormat == CF_HDROP) {
		pmedium->tymed = TYMED_HGLOBAL;
		pmedium->hGlobal = (HGLOBAL)GethDrop(0, 0, FALSE);
		pmedium->pUnkForRelease = NULL;
		BSTR bs = NULL;
		teGetRootFromDataObj(&bs, this);
		if (teStrSameIFree(bs, g_bsClipRoot)) {
			if (g_pOnFunc[TE_OnBeforeGetData]) {
				VARIANTARG *pv = GetNewVARIANT(3);
				teSetObject(&pv[2], g_pTE);
				teSetObjectRelease(&pv[1], this);
				teSetLong(&pv[0], 5);
				Invoke4(g_pOnFunc[TE_OnBeforeGetData], NULL, 3, pv);
			}
		}
		teSysFreeString(&g_bsClipRoot);
		return S_OK;
	}

	if (pformatetcIn->cfFormat == CF_UNICODETEXT || pformatetcIn->cfFormat == CF_TEXT) {
		if (!m_bsText) {
			try {
				VARIANT vResult, vStr;
				VariantInit(&vResult);
				if SUCCEEDED(DoFunc1(TE_OnClipboardText, this, &vResult)) {
					teVariantChangeType(&vStr, &vResult, VT_BSTR);
					VariantClear(&vResult);
					m_bsText = vStr.bstrVal;
					vStr.bstrVal = NULL;
				}
			} catch (...) {
				g_nException = 0;
#ifdef _DEBUG
				g_strException = L"OnClipboardText";
#endif
			}
		}
		HGLOBAL hMem;
		int nLen = ::SysStringLen(m_bsText) + 1;
		int nLenA = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)m_bsText, nLen, NULL, 0, NULL, NULL);
		if (pformatetcIn->cfFormat == CF_UNICODETEXT) {
/*/// For paste problem at file open dialog.
			if (m_nCount) {
				LPSTR lpA = new char[nLenA + 1];
				WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)m_bsText, nLen, lpA, nLenA, NULL, NULL);
				int nLenW = MultiByteToWideChar(CP_ACP, 0, lpA, nLenA, NULL, NULL);
				LPWSTR lpW = new WCHAR[nLenW];
				MultiByteToWideChar(CP_ACP, 0, lpA, nLenA, lpW, nLenW);
				if (lstrcmpi(m_bsText, lpW) == 0) {
					delete [] lpA;
					delete [] lpW;
					return DATA_E_FORMATETC;
				}
				delete [] lpA;
				delete [] lpW;
			}
//*/
			nLen = ::SysStringByteLen(m_bsText) + sizeof(WCHAR);
			hMem = GlobalAlloc(GHND, nLen);
			try {
				LPWSTR lp = static_cast<LPWSTR>(GlobalLock(hMem));
				if (lp) {
					::CopyMemory(lp, m_bsText, nLen);
				}
			} catch (...) {
				g_nException = 0;
#ifdef _DEBUG
				g_strException = L"CF_UNICODETEXT";
#endif
			}
		} else {
			hMem = GlobalAlloc(GHND, nLenA);
			try {
				LPSTR lpA = static_cast<LPSTR>(GlobalLock(hMem));
				if (lpA) {
					WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)m_bsText, nLen, lpA, nLenA, NULL, NULL);
				}
			} catch (...) {
				g_nException = 0;
#ifdef _DEBUG
				g_strException = L"CF_TEXT";
#endif
			}
		}

		GlobalUnlock(hMem);
		pmedium->tymed = TYMED_HGLOBAL;
		pmedium->hGlobal = hMem;
		pmedium->pUnkForRelease = NULL;
		return S_OK;
	}
	return DATA_E_FORMATETC;
}

STDMETHODIMP CteFolderItems::GetDataHere(FORMATETC *pformatetc, STGMEDIUM *pmedium)
{
	if (m_pDataObj) {
		return m_pDataObj->GetDataHere(pformatetc, pmedium);
	}
	return E_NOTIMPL;
}

STDMETHODIMP CteFolderItems::QueryGetData(FORMATETC *pformatetc)
{
	if (m_dwEffect != (DWORD)-1 && pformatetc->cfFormat == DROPEFFECTFormat.cfFormat) {
		return S_OK;
	}
	if (QueryGetData2(pformatetc) == S_OK) {
		return S_OK;
	}
	if (m_pDataObj) {
		return m_pDataObj->QueryGetData(pformatetc);
	}
	return DATA_E_FORMATETC;
}

HRESULT CteFolderItems::QueryGetData2(FORMATETC *pformatetc)
{
	if (pformatetc->cfFormat == CF_UNICODETEXT) {
		return S_OK;
	}
	if (pformatetc->cfFormat == CF_TEXT) {
		return S_OK;
	}
	if (m_nCount > 0) {
		if (pformatetc->cfFormat == CF_HDROP) {
			return S_OK;
		}
		if (pformatetc->cfFormat == IDLISTFormat.cfFormat && m_bUseILF) {
			return S_OK;
		}
	}
	return DATA_E_FORMATETC;
}

STDMETHODIMP CteFolderItems::GetCanonicalFormatEtc(FORMATETC *pformatectIn, FORMATETC *pformatetcOut)
{
	if (m_pDataObj) {
		return m_pDataObj->GetCanonicalFormatEtc(pformatectIn, pformatetcOut);
	}
	return E_NOTIMPL;
}

STDMETHODIMP CteFolderItems::SetData(FORMATETC *pformatetc, STGMEDIUM *pmedium, BOOL fRelease)
{
	if (m_pDataObj) {
		return m_pDataObj->SetData(pformatetc, pmedium, fRelease);
	}
	return E_NOTIMPL;
}

STDMETHODIMP CteFolderItems::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppenumFormatEtc)
{
	if (dwDirection == DATADIR_GET) {
		FORMATETC formats[MAX_FORMATS];
		FORMATETC teformats[] = { IDLISTFormat, HDROPFormat, UNICODEFormat, TEXTFormat, DROPEFFECTFormat };
		UINT nFormat = 0;

		if (m_pDataObj) {
			IEnumFORMATETC *penumFormatEtc;
			if SUCCEEDED(m_pDataObj->EnumFormatEtc(DATADIR_GET, &penumFormatEtc)) {
				while (nFormat < MAX_FORMATS - _countof(teformats) && penumFormatEtc->Next(1, &formats[nFormat], NULL) == S_OK) {
					if (QueryGetData2(&formats[nFormat]) != S_OK) {
						nFormat++;
					}
				}
				penumFormatEtc->Release();
			}
		}
		AdjustIDListEx();
		int nMax = _countof(teformats);
		if (m_dwEffect == (DWORD)-1) {
			nMax--;
		}
		for (int i = m_nCount ? (m_bUseILF ? 0 : 1) : 2 ; i < nMax; i++) {
			formats[nFormat++] = teformats[i];
		}
		return CreateFormatEnumerator(nFormat, formats, ppenumFormatEtc);
	}
	if (m_pDataObj) {
		return m_pDataObj->EnumFormatEtc(dwDirection, ppenumFormatEtc);
	}
	return E_NOTIMPL;
}

STDMETHODIMP CteFolderItems::DAdvise(FORMATETC *pformatetc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection)
{
	if (m_pDataObj) {
		return m_pDataObj->DAdvise(pformatetc, advf, pAdvSink, pdwConnection);
	}
	return E_NOTIMPL;
}

STDMETHODIMP CteFolderItems::DUnadvise(DWORD dwConnection)
{
	if (m_pDataObj) {
		return m_pDataObj->DUnadvise(dwConnection);
	}
	return E_NOTIMPL;
}

STDMETHODIMP CteFolderItems::EnumDAdvise(IEnumSTATDATA **ppenumAdvise)
{
	if (m_pDataObj) {
		return m_pDataObj->EnumDAdvise(ppenumAdvise);
	}
	return E_NOTIMPL;
}

//CteServiceProvider
STDMETHODIMP CteServiceProvider::QueryInterface(REFIID riid, void **ppvObject)
{
	static const QITAB qit[] =
	{
		QITABENT(CteServiceProvider, IServiceProvider),
		{ 0 },
	};
	HRESULT hr = QISearch(this, qit, riid, ppvObject);
	return SUCCEEDED(hr) ? hr : m_pUnk->QueryInterface(riid, ppvObject);
}

STDMETHODIMP_(ULONG) CteServiceProvider::AddRef()
{
	return ::InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CteServiceProvider::Release()
{
	if (::InterlockedDecrement(&m_cRef) == 0) {
		delete this;
		return 0;
	}

	return m_cRef;
}

STDMETHODIMP CteServiceProvider::QueryService(REFGUID guidService, REFIID riid, void **ppv)
{
	if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IServiceProvider)) {
		return E_NOINTERFACE;
	}
	if (IsEqualIID(riid, IID_IShellBrowser) || IsEqualIID(riid, IID_ICommDlgBrowser) || IsEqualIID(riid, IID_ICommDlgBrowser2) || //IsEqualIID(riid, IID_ICommDlgBrowser3)||
		IsEqualIID(riid, IID_IExplorerPaneVisibility)) {
		return m_pUnk->QueryInterface(riid, ppv);
	}
	if (IsEqualIID(riid, IID_IInternetSecurityManager)) {
		*ppv = static_cast<IInternetSecurityManager *>(new CteInternetSecurityManager());
		return S_OK;
	}
	if (IsEqualIID(riid, IID_INewWindowManager)) {
		*ppv = static_cast<INewWindowManager *>(new CteNewWindowManager());
		return S_OK;
	}
	if (m_pUnk2) {
		return m_pUnk2->QueryInterface(riid, ppv);
	}
	return E_NOINTERFACE;
}

CteServiceProvider::CteServiceProvider(IUnknown *punk, IUnknown *punk2)
{
	m_cRef = 1;
	m_pUnk = punk;
	m_pUnk2 = punk2;
}

CteServiceProvider::~CteServiceProvider()
{
}

//CteMemory

CteMemory::CteMemory(int nSize, void *pc, int nCount, LPWSTR lpStruct)
{
	BOOL bSafeArray = FALSE;
	m_cRef = 1;
	m_pc = (char *)pc;
	m_bsStruct = NULL;
	m_nStructIndex = -1;
	if (lstrcmpi(lpStruct, L"SAFEARRAY") == 0) {
		lpStruct = NULL;
		bSafeArray = TRUE;
	}
	if (lpStruct) {
		m_nStructIndex = teBSearchStruct(pTEStructs, _countof(pTEStructs), g_maps[MAP_SS], lpStruct);
		m_bsStruct = ::SysAllocString(lpStruct);
	}
	m_nCount = nCount;
	m_bsAlloc = NULL;
	m_nSize = 0;
	if (nSize > 0) {
		m_nSize = nSize;
		if (pc == NULL || bSafeArray) {
			m_bsAlloc = ::SysAllocStringByteLen(NULL, nSize);
			m_pc = (char *)m_bsAlloc;
			if (m_pc) {
				PVOID pvData;
				if (bSafeArray && pc && ::SafeArrayAccessData((SAFEARRAY *)pc, &pvData) == S_OK) {
					::CopyMemory(m_pc, pvData, nSize);
					::SafeArrayUnaccessData((SAFEARRAY *)pc);
				} else {
					::ZeroMemory(m_pc, nSize);
				}
			}
		}
	}
	m_nbs = 0;
	m_ppbs = NULL;
}

CteMemory::~CteMemory()
{
	Free(TRUE);
}

void CteMemory::Free(BOOL bpbs)
{
	teSysFreeString(&m_bsAlloc);
	teSysFreeString(&m_bsStruct);
	if (bpbs && m_ppbs) {
		while (--m_nbs >= 0) {
			teSysFreeString(&m_ppbs[m_nbs]);
		}
		delete [] m_ppbs;
		m_ppbs = NULL;
	}
	m_pc = NULL;
}

STDMETHODIMP CteMemory::QueryInterface(REFIID riid, void **ppvObject)
{
	static const QITAB qit[] =
	{
		QITABENT(CteMemory, IDispatch),
		QITABENT(CteMemory, IDispatchEx),
		{ 0 },
	};
	if (IsEqualIID(riid, g_ClsIdStruct)) {
		*ppvObject = this;
		AddRef();
		return S_OK;
	}
	return QISearch(this, qit, riid, ppvObject);
}

STDMETHODIMP_(ULONG) CteMemory::AddRef()
{
	return ::InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CteMemory::Release()
{
	if (::InterlockedDecrement(&m_cRef) == 0) {
		delete this;
		return 0;
	}

	return m_cRef;
}

STDMETHODIMP CteMemory::GetTypeInfoCount(UINT *pctinfo)
{
	*pctinfo = 0;
	return S_OK;
}

STDMETHODIMP CteMemory::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteMemory::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	return GetDispID(*rgszNames, 0, rgDispId);
}

STDMETHODIMP CteMemory::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	try {
		int nArg = pDispParams ? pDispParams->cArgs - 1 : -1;
		if (pVarResult) {
			VariantInit(pVarResult);
		}
		switch(dispIdMember) {
			//P
			case 1:
				teSetPtr(pVarResult, m_pc);
				return S_OK;
			//Item
			case DISPID_TE_ITEM:
				if (nArg >= 0) {
					ItemEx(GetIntFromVariant(&pDispParams->rgvarg[nArg]), pVarResult, nArg >= 1 ? &pDispParams->rgvarg[nArg - 1] : NULL);
				}
				return S_OK;
			//Count
			case DISPID_TE_COUNT:
				teSetLong(pVarResult, m_nCount);
				return S_OK;
			//_NewEnum
			case DISPID_NEWENUM:
				teSetObjectRelease(pVarResult, new CteDispatch(this, 0));
				return S_OK;
			//Read
			case 4:
				if (pVarResult) {
					if (nArg >= 1) {
						pVarResult->vt = GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]);
						int nLen = -1;
						if (nArg >= 2) {
							nLen = GetIntFromVariant(&pDispParams->rgvarg[nArg] - 2);
						}
						Read(GetIntFromVariant(&pDispParams->rgvarg[nArg]), nLen, pVarResult);
					}
				}
				return S_OK;
			//Write
			case 5:
				if (nArg >= 2) {
					int nLen = -1;
					if (nArg >= 3) {
						nLen = GetIntFromVariant(&pDispParams->rgvarg[nArg] - 3);
					}
					Write(GetIntFromVariant(&pDispParams->rgvarg[nArg]), nLen, GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]), &pDispParams->rgvarg[nArg - 2]);
				}
				return S_OK;
			//Size
			case 6:
				teSetLong(pVarResult, m_nSize);
				return S_OK;
			//Free
			case 7:
				Free(TRUE);
				return S_OK;
			//Clone
			case 8:
				if (m_nSize) {
					CteMemory *pMem = new CteMemory(m_nSize, NULL, m_nCount, m_bsStruct);
					::CopyMemory(pMem->m_pc, m_pc, m_nSize);
					teSetObjectRelease(pVarResult, pMem);
				}
				return S_OK;
			//_BLOB
			case 9:
				if (m_bsAlloc) {
					if (nArg >= 0 && pDispParams->rgvarg[nArg].vt == VT_BSTR) {
						int nSize = ::SysStringByteLen(pDispParams->rgvarg[nArg].bstrVal);
						if (nSize != m_nSize) {
							m_bsAlloc = ::SysAllocStringByteLen(NULL, nSize);
							m_pc = (char *)m_bsAlloc;
							m_nSize = nSize;
						}
						::CopyMemory(m_pc, pDispParams->rgvarg[nArg].bstrVal, m_nSize);
					}
					if (pVarResult) {
						pVarResult->vt = VT_BSTR;
						pVarResult->bstrVal = ::SysAllocStringByteLen(m_pc, m_nSize);
					}
				}
				return S_OK;
			//Value
			case DISPID_VALUE:
				teSetObject(pVarResult, this);
				return S_OK;
			default:
				if (dispIdMember >= DISPID_COLLECTION_MIN && dispIdMember <= DISPID_TE_MAX) {
					ItemEx(dispIdMember - DISPID_COLLECTION_MIN, pVarResult, nArg >= 0 ? &pDispParams->rgvarg[nArg] : NULL);
					return S_OK;
				}
				if (m_pc) {
					VARTYPE vt = dispIdMember >> TE_VT;
					if (vt) {
						if (wFlags & DISPATCH_METHOD) {
							teSetLong(pVarResult, dispIdMember & TE_VI);
							return S_OK;
						}
						if (nArg >= 0) {
							if (vt == VT_BSTR && pDispParams->rgvarg[nArg].vt == VT_BSTR) {
								VARIANT v;
								teSetPtr(&v, AddBSTR(pDispParams->rgvarg[nArg].bstrVal));
								Write(dispIdMember & TE_VI, -1, vt, &v);
								VariantClear(&v);
							} else {
								Write(dispIdMember & TE_VI, -1, vt, &pDispParams->rgvarg[nArg]);
							}
						}
						if (pVarResult) {
							pVarResult->vt = vt;
							Read(dispIdMember & TE_VI, -1, pVarResult);
						}
					}
					return S_OK;
				}
		}
	} catch (...) {
		return teException(pExcepInfo, "Memory", methodMem, dispIdMember);
	}
	return DISP_E_MEMBERNOTFOUND;
}

//IDispatchEx
STDMETHODIMP CteMemory::GetDispID(BSTR bstrName, DWORD grfdex, DISPID *pid)
{
	if (m_nStructIndex >= 0 && teGetDispId(pTEStructs[m_nStructIndex].pMethod, 0, NULL, bstrName, pid, false) == S_OK) {
		return S_OK;
	}
	if (teGetDispId(methodMem, _countof(methodMem), g_maps[MAP_Mem], bstrName, pid, TRUE) == S_OK) {
		return S_OK;
	}
	return teGetDispId(methodMem2, 0, NULL, m_bsStruct, pid, false);
}

STDMETHODIMP CteMemory::InvokeEx(DISPID id, LCID lcid, WORD wFlags, DISPPARAMS *pdp, VARIANT *pvarRes, EXCEPINFO *pei, IServiceProvider *pspCaller)
{
	return Invoke(id, IID_NULL, lcid, wFlags, pdp, pvarRes, pei, NULL);
}

STDMETHODIMP CteMemory::DeleteMemberByName(BSTR bstrName, DWORD grfdex)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteMemory::DeleteMemberByDispID(DISPID id)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteMemory::GetMemberProperties(DISPID id, DWORD grfdexFetch, DWORD *pgrfdex)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteMemory::GetMemberName(DISPID id, BSTR *pbstrName)
{
	return teGetMemberName(id, pbstrName);
}

STDMETHODIMP CteMemory::GetNextDispID(DWORD grfdex, DISPID id, DISPID *pid)
{
	*pid = (id < DISPID_COLLECTION_MIN) ? DISPID_COLLECTION_MIN : id + 1;
	return *pid < m_nCount + DISPID_COLLECTION_MIN ? S_OK : S_FALSE;
}

STDMETHODIMP CteMemory::GetNameSpaceParent(IUnknown **ppunk)
{
	return E_NOTIMPL;
}

VOID CteMemory::ItemEx(int i, VARIANT *pVarResult, VARIANT *pVarNew)
{
	if (i >= 0 && i < m_nCount) {
		if (pVarNew) {
			if (m_nCount) {
				int nSize = m_nSize / m_nCount;
				if (nSize >= 1 && nSize <= 8) {
					LARGE_INTEGER li;
					li.QuadPart = GetLLFromVariant(pVarNew);
					if (li.QuadPart == 0 && pVarNew->vt == VT_BSTR && pVarNew->bstrVal &&
					(lstrcmpi(m_bsStruct, L"BSTR") == 0 || lstrcmpi(m_bsStruct, L"LPWSTR") == 0)) {
						li.QuadPart = (LONGLONG)AddBSTR(pVarNew->bstrVal);
					}
					if (nSize == 1) {//BYTE
						m_pc[i] = LOBYTE(li.LowPart);
					} else if (nSize == 2) {//WORD
						WORD *p = (WORD *)m_pc;
						p[i] = LOWORD(li.LowPart);
					} else if (nSize == 4) {//DWORD
						int *p = (int *)m_pc;
						p[i] = li.LowPart;
					} else if (nSize == 8) {//LONGLONG
						LONGLONG *p = (LONGLONG *)m_pc;
						p[i] = li.QuadPart;
					}
				}
			}
		}
		if (pVarResult) {
			if (m_nCount) {
				int nSize = m_nSize / m_nCount;
				if (nSize) {
					if (nSize == 1) {//BYTE/char
						teSetLong(pVarResult, m_pc[i]);
					} else if (nSize == 2) {//WORD/WCHAR
						WORD *p = (WORD *)m_pc;
						teSetLong(pVarResult, p[i]);
					} else if (nSize == 4) {//DWORD/int
						int *p = (int *)m_pc;
						teSetLong(pVarResult, p[i]);
					} else if (nSize == 8 && lstrcmpi(m_bsStruct, L"POINT")) {//LONGLONG
						LONGLONG *p = (LONGLONG *)m_pc;
						teSetLL(pVarResult, p[i]);
					} else {
						teSetObjectRelease(pVarResult, new CteMemory(nSize, m_pc + (nSize * i), 1, m_bsStruct));
					}
				}
			}
		}
	}
}

BSTR CteMemory::AddBSTR(BSTR bs)
{
	BSTR *p = new BSTR[m_nbs + 1];
	if (m_ppbs) {
		::CopyMemory(p, m_ppbs, m_nbs * sizeof(BSTR));
		delete [] m_ppbs;
	}
	m_ppbs = p;
	return m_ppbs[m_nbs++] = ::SysAllocStringByteLen((char *)bs, ::SysStringByteLen(bs));
}

VOID CteMemory::Read(int nIndex, int nLen, VARIANT *pVarResult)
{
	CteMemory *pMem;
	try {
		if (pVarResult->vt & VT_ARRAY) {
			VARTYPE vt = LOBYTE(pVarResult->vt);
			int nSize = SizeOfvt(vt);
			if (nSize) {
				if (nLen < 0) {
					nLen = (m_nSize - nIndex) / nSize;
				}
				if (nLen > 0) {
					SAFEARRAY *psa = SafeArrayCreateVector(vt, 0, nLen);
					if (psa) {
						PVOID pvData;
						if (::SafeArrayAccessData(psa, &pvData) == S_OK) {
							::CopyMemory(pvData, &m_pc[nIndex], nLen * nSize);
							::SafeArrayUnaccessData(psa);
							pVarResult->parray = psa;
						}
					}
				}
			}
			return;
		}
		teParam *pFrom = (teParam *)&m_pc[nIndex];
		switch (pVarResult->vt) {
			case VT_BOOL:
				pVarResult->boolVal = pFrom->boolVal;
				break;
			case VT_I1:
			case VT_UI1:
				pVarResult->bVal = pFrom->bVal;
				break;
			case VT_I2:
			case VT_UI2:
				pVarResult->iVal = pFrom->iVal;
				break;
			case VT_I4:
			case VT_UI4:
				pVarResult->lVal = pFrom->lVal;
				break;
			case VT_I8:
			case VT_UI8:
				teSetLL(pVarResult, pFrom->llVal);
				break;
			case VT_R4:
				pVarResult->fltVal = pFrom->fltVal;
				break;
			case VT_R8:
				pVarResult->dblVal = pFrom->dblVal;
				break;
			case VT_PTR:
			case VT_BSTR:
				teSetPtr(pVarResult, pFrom->uint_ptr);
				break;
			case VT_LPWSTR:
				if (nLen >= 0) {
					if (nLen * (int)sizeof(WCHAR) > m_nSize) {
						nLen = m_nSize / sizeof(WCHAR);
					}
					pVarResult->bstrVal = teSysAllocStringLen((WCHAR *)pFrom, nLen);
					pVarResult->vt = VT_BSTR;
				} else {
					teSetSZ(pVarResult, (WCHAR *)pFrom);
				}
				break;
			case VT_LPSTR:
			case VT_USERDEFINED:
				if (nLen > m_nSize) {
					nLen = m_nSize;
				}
				pVarResult->bstrVal = teMultiByteToWideChar(pVarResult->vt != VT_USERDEFINED ? CP_ACP : CP_UTF8, (LPCSTR)pFrom, nLen);
				pVarResult->vt = VT_BSTR;
				break;
			case VT_FILETIME:
				teFileTimeToVariantTime((LPFILETIME)pFrom, &pVarResult->date);
				pVarResult->vt = VT_DATE;
				break;
			case VT_CY:
				POINT *ppt;
				ppt = (POINT *)pFrom;
				pMem = new CteMemory(sizeof(POINT), NULL, 1, L"POINT");
				pMem->SetPoint(ppt->x, ppt->y);
				teSetObjectRelease(pVarResult, pMem);
				break;
			case VT_CARRAY:
				pMem = new CteMemory(sizeof(RECT), NULL, 1, L"RECT");
				::CopyMemory(pMem->m_pc, pFrom, sizeof(RECT));
				teSetObjectRelease(pVarResult, pMem);
				break;
			case VT_CLSID:
				LPOLESTR lpsz;
				StringFromCLSID(pFrom->iid, &lpsz);
				teSetSZ(pVarResult, lpsz);
				teCoTaskMemFree(lpsz);
				break;
			case VT_VARIANT:
				VariantCopy(pVarResult, (VARIANT *)pFrom);
				break;
/*			case VT_RECORD:
				teSetIDList(pVarResult, *(LPITEMIDLIST *)&m_pc[nIndex]);
				break;*/
			default:
				pVarResult->vt = VT_EMPTY;
				break;
		}//end_switch
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"CteMemory::Read";
#endif
	}
}

//Write
VOID CteMemory::Write(int nIndex, int nLen, VARTYPE vt, VARIANT *pv)
{
	try {
		if (pv->vt & VT_ARRAY) {
			int nSize = SizeOfvt(pv->vt);
			if (nSize) {
				if (nLen < 0) {
					nLen = (m_nSize - nIndex) / nSize;
				}
				if (nLen > 0) {
					PVOID pvData;
					if (::SafeArrayAccessData(pv->parray, &pvData) == S_OK) {
						::CopyMemory(&m_pc[nIndex], pvData, nLen * nSize);
						::SafeArrayUnaccessData(pv->parray);
					}
				}
			}
			return;
		}
		teParam d;
		d.llVal = GetLLFromVariant(pv);
		teParam *pDest = (teParam *)&m_pc[nIndex];
		switch (vt) {
			case VT_I1:
			case VT_UI1:
				pDest->bVal = d.bVal;
				break;
			case VT_I2:
			case VT_UI2:
				pDest->iVal = d.iVal;
				break;
			case VT_BOOL:
			case VT_I4:
			case VT_UI4:
				pDest->lVal = d.lVal;
				break;
			case VT_I8:
			case VT_UI8:
				pDest->llVal = d.llVal;
				break;
			case VT_PTR:
			case VT_BSTR:
				pDest->uint_ptr = d.uint_ptr;
				break;
			case VT_LPWSTR:
				VARIANT v;
				teVariantChangeType(&v, pv, VT_BSTR);
				if (v.bstrVal) {
					if (nLen < 0) {
						nLen = (::SysStringLen(v.bstrVal) + 1);
					}
					nLen *= sizeof(WCHAR);
					if (nLen > m_nSize) {
						nLen = m_nSize;
					}
					::CopyMemory(pDest, v.bstrVal, nLen);
				} else {
					*(WCHAR *)pDest = NULL;
				}
				VariantClear(&v);
				break;
			case VT_LPSTR:
			case VT_USERDEFINED:
				teVariantChangeType(&v, pv, VT_BSTR);
				int nLenA;
				nLenA = 0;
				if (v.bstrVal) {
					UINT cp = vt != VT_USERDEFINED ? CP_ACP : CP_UTF8;
					nLenA = WideCharToMultiByte(cp, 0, (LPCWSTR)v.bstrVal, nLen, NULL, 0, NULL, NULL) + 1;
					if (nLenA > m_nSize) {
						nLenA = m_nSize;
					}
					WideCharToMultiByte(cp, 0, (LPCWSTR)v.bstrVal, nLen, (LPSTR)pDest, nLenA - 1, NULL, NULL);
				}
				((LPSTR)pDest)[nLenA] = NULL;
				VariantClear(&v);
				break;
			case VT_FILETIME:
				teVariantTimeToFileTime(pv->date, (LPFILETIME)pDest);
				break;
			case VT_CY:
				GetPointFormVariant((LPPOINT)pDest, pv);
				break;
			case VT_CARRAY:
				char *pc;
				VARIANT vMem;
				VariantInit(&vMem);
				pc = GetpcFromVariant(pv, &vMem);
				if (pc) {
					::CopyMemory(pDest, pc, sizeof(RECT));
				} else {
					::ZeroMemory(pDest, sizeof(RECT));
				}
				VariantClear(&vMem);
				break;
			case VT_CLSID:
				if (pv->vt == VT_BSTR) {
					teCLSIDFromString(pv->bstrVal, (LPCLSID)pDest);
				}
				break;
			case VT_VARIANT:
				VariantCopy((VARIANT *)pDest, pv);
				break;
/*			case VT_RECORD:
				LPITEMIDLIST *ppidl;
				ppidl = (LPITEMIDLIST *)pDest;
				teCoTaskMemFree(*ppidl);
				teGetIDListFromVariant(ppidl, pv);
				break;*/
		}//end_switch
	} catch (...) {
		g_nException = 0;
#ifdef _DEBUG
		g_strException = L"CteMemory::Write";
#endif
	}
}

void CteMemory::SetPoint(int x, int y)
{
	POINT *ppt = (POINT *)m_pc;
	ppt->x = x;
	ppt->y = y;
}

//CteContextMenu

CteContextMenu::CteContextMenu(IUnknown *punk, IDataObject *pDataObj, IUnknown *punkSB)
{
	m_cRef = 1;
	m_pContextMenu = NULL;
	m_pDataObj = pDataObj;
	m_pDll = NULL;
	if (punk) {
		punk->QueryInterface(IID_PPV_ARGS(&m_pContextMenu));
		if (punkSB) {
			IUnknown_SetSite(punk, punkSB);
		}
	}
}

CteContextMenu::~CteContextMenu()
{
	IUnknown_SetSite(m_pContextMenu, NULL);
	SafeRelease(&m_pContextMenu);
	SafeRelease(&m_pDataObj);
	SafeRelease(&m_pDll);
}

STDMETHODIMP CteContextMenu::QueryInterface(REFIID riid, void **ppvObject)
{
	static const QITAB qit[] =
	{
		QITABENT(CteContextMenu, IDispatch),
		{ 0 },
	};
	if (IsEqualIID(riid, IID_IContextMenu) || IsEqualIID(riid, IID_IContextMenu2) || IsEqualIID(riid, IID_IContextMenu3)) {
		return m_pContextMenu->QueryInterface(riid, ppvObject);
	}
	return QISearch(this, qit, riid, ppvObject);
}

STDMETHODIMP_(ULONG) CteContextMenu::AddRef()
{
	return ::InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CteContextMenu::Release()
{
	if (::InterlockedDecrement(&m_cRef) == 0) {
		delete this;
		return 0;
	}
	return m_cRef;
}

STDMETHODIMP CteContextMenu::GetTypeInfoCount(UINT *pctinfo)
{
	*pctinfo = 0;
	return S_OK;
}

STDMETHODIMP CteContextMenu::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteContextMenu::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	return teGetDispId(methodCM, 0, NULL, *rgszNames, rgDispId, TRUE);
}

STDMETHODIMP CteContextMenu::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	try {
		HRESULT hr = NULL;

		int nArg = pDispParams ? pDispParams->cArgs - 1 : -1;
		if (pVarResult) {
			VariantInit(pVarResult);
		}
		switch(dispIdMember) {
			//QueryContextMenu
			case 1:
				if (nArg >= 4) {
					for (int i = 5; i--;) {
						m_param[i].llVal = GetLLFromVariant(&pDispParams->rgvarg[nArg - i]);
					}
					teSetLong(pVarResult, m_pContextMenu->QueryContextMenu(m_param[0].hmenu, m_param[1].uintVal, m_param[2].uintVal, m_param[3].uintVal, m_param[4].uintVal));
				}
				return S_OK;
			//InvokeCommand
			case 2:
				if (nArg >= 7) {
					if (g_pOnFunc[TE_OnInvokeCommand]) {
						VARIANT vResult;
						VariantInit(&vResult);
						VARIANTARG *pv = GetNewVARIANT(nArg + 2);
						teSetObject(&pv[nArg + 1], this);
						for (int i = nArg; i >= 0; i--) {
							VariantCopy(&pv[i], &pDispParams->rgvarg[i]);
						}
						Invoke4(g_pOnFunc[TE_OnInvokeCommand], &vResult, nArg + 2, pv);
						if (GetIntFromVariantClear(&vResult) == S_OK) {
							return S_OK;
						}
					}
					CMINVOKECOMMANDINFOEX cmi = { sizeof(cmi) };
					VARIANTARG *pv = GetNewVARIANT(3);
					WCHAR **ppwc = new WCHAR*[3];
					char **ppc = new char*[3];
					BOOL bExec = TRUE;
					for (int i = 0; i <= 2; i++) {
						if (pDispParams->rgvarg[nArg - i - 2].vt == VT_I4) {
							UINT_PTR nVerb = (UINT_PTR)pDispParams->rgvarg[nArg - i - 2].lVal;
							ppwc[i] = (WCHAR *)nVerb;
							ppc[i] = (char *)nVerb;
							if (nVerb > MAXWORD) {
								bExec = FALSE;
							}
						} else {
							teVariantChangeType(&pv[i], &pDispParams->rgvarg[nArg - i - 2], VT_BSTR);
							ppwc[i] = pv[i].bstrVal;
							if (i == 2) {
								if (!ppwc[i]) {
									IShellBrowser *pSB;
									if (GetFolderVew(&pSB)) {
										LPITEMIDLIST pidl;
										if (teGetIDListFromObject(pSB, &pidl)) {
											teGetDisplayNameFromIDList(&ppwc[i], pidl, SHGDN_FORPARSING);
											VariantClear(&pv[i]);
											pv[i].bstrVal = ppwc[i];
											pv[i].vt = VT_BSTR;
											teCoTaskMemFree(pidl);
										}
										pSB->Release();
									}
								}
								if (!teIsFileSystem(ppwc[i]) || tePathIsDirectory(ppwc[i], 0, TRUE) != S_OK) {
									ppwc[i] = NULL;
								}
							}
							if ((ULONG_PTR)ppwc[i] > MAXWORD) {
								int nLenW = ::SysStringLen(ppwc[i]) + 1;
								int nLenA = WideCharToMultiByte(CP_ACP, 0, ppwc[i], nLenW, NULL, 0, NULL, NULL);
								ppc[i] = new char[nLenA];
								WideCharToMultiByte(CP_ACP, 0, ppwc[i], nLenW, ppc[i], nLenA, NULL, NULL);
								BSTR bs = ::SysAllocStringLen(NULL, nLenW);
								MultiByteToWideChar(CP_ACP, 0, ppc[i], nLenA, bs, nLenW);
								if (!teStrSameIFree(bs, ppwc[i])) {
									cmi.fMask = CMIC_MASK_UNICODE;
								}
							} else {
								ppc[i] = (char *)ppwc[i];
							}
						}
					}
					if (bExec) {
						cmi.fMask |= (DWORD)GetIntFromVariant(&pDispParams->rgvarg[nArg]);
						cmi.hwnd  = (HWND)GetPtrFromVariant(&pDispParams->rgvarg[nArg - 1]);
						cmi.lpVerbW = ppwc[0];
						cmi.lpVerb = ppc[0];
						cmi.lpParametersW = ppwc[1];
						cmi.lpParameters = ppc[1];
						cmi.lpDirectoryW = ppwc[2];
						cmi.lpDirectory = ppc[2];
						cmi.nShow = GetIntFromVariant(&pDispParams->rgvarg[nArg - 5]);
						cmi.dwHotKey = (DWORD)GetIntFromVariant(&pDispParams->rgvarg[nArg - 6]);
						cmi.hIcon =(HANDLE)GetPtrFromVariant(&pDispParams->rgvarg[nArg - 7]);
						try {
							hr = m_pContextMenu->InvokeCommand((LPCMINVOKECOMMANDINFO)&cmi);
						} catch (...) {
							hr = E_UNEXPECTED;
						}
					}
					teSetLong(pVarResult, hr);
					for (int i = 3; i--;) {
						VariantClear(&pv[i]);
						if ((ULONG_PTR)ppc[i] >= MAXWORD) {
							delete [] ppc[i];
						}
					}
					delete [] pv;
				}
				return S_OK;
			//Items
			case 3:
				teSetObjectRelease(pVarResult, new CteFolderItems(m_pDataObj, NULL));
				return S_OK;
			//GetCommandString
			case 4:
				if (nArg >= 1) {
					WCHAR szName[MAX_PATH];
					szName[0] = NULL;
					UINT idCmd = GetIntFromVariant(&pDispParams->rgvarg[nArg]);
					if (idCmd <= MAXWORD) {
						m_pContextMenu->GetCommandString(idCmd,
							GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]),
							NULL, (LPSTR)szName, MAX_PATH);
					}
					teSetSZ(pVarResult, szName);
				} else {
					teSetBool(pVarResult, TRUE);
				}
				return S_OK;
			//FolderView
			case 5:
				IShellBrowser *pSB;
				if (GetFolderVew(&pSB)) {
					teSetObjectRelease(pVarResult, pSB);
				}
				return S_OK;
			//HandleMenuMsg
			case 6:
				LRESULT lResult;
				IContextMenu3 *pCM3;
				IContextMenu2 *pCM2;
				lResult = 0;
				if (nArg >= 2) {
					if SUCCEEDED(QueryInterface(IID_PPV_ARGS(&pCM3))) {
						pCM3->HandleMenuMsg2(
							(UINT)GetIntFromVariant(&pDispParams->rgvarg[nArg]),
							(WPARAM)GetPtrFromVariant(&pDispParams->rgvarg[nArg - 1]),
							(LPARAM)GetPtrFromVariant(&pDispParams->rgvarg[nArg - 2]), &lResult
						);
						pCM3->Release();
					} else if SUCCEEDED(QueryInterface(IID_PPV_ARGS(&pCM2))) {
						pCM2->HandleMenuMsg(
							(UINT)GetIntFromVariant(&pDispParams->rgvarg[nArg]),
							(WPARAM)GetPtrFromVariant(&pDispParams->rgvarg[nArg - 1]),
							(LPARAM)GetPtrFromVariant(&pDispParams->rgvarg[nArg - 2])
						);
						pCM2->Release();
					}
				}
				teSetPtr(pVarResult, lResult);
				return S_OK;

			case DISPID_VALUE:
				teSetObject(pVarResult, this);
				return S_OK;
			default:
				if (dispIdMember >= 10 && dispIdMember <= 14) {
					teSetPtr(pVarResult, m_param[dispIdMember - 10].long_ptr);
					return S_OK;
				}
				break;
		}
	} catch (...) {
		return teException(pExcepInfo, "ContextMenu", methodCM, dispIdMember);
	}
	return DISP_E_MEMBERNOTFOUND;
}

BOOL CteContextMenu::GetFolderVew(IShellBrowser **ppSB)
{
	BOOL bResult = FALSE;
	IServiceProvider *pSP;
	if SUCCEEDED(IUnknown_GetSite(m_pContextMenu, IID_PPV_ARGS(&pSP))) {
		bResult = SUCCEEDED(pSP->QueryService(SID_SShellBrowser, IID_PPV_ARGS(ppSB)));
		pSP->Release();
	}
	return bResult;
}

//CteDropTarget

CteDropTarget::CteDropTarget(IDropTarget *pDropTarget, FolderItem *pFolderItem)
{
	m_cRef = 1;
	m_pDropTarget = NULL;
	if (pDropTarget) {
		pDropTarget->QueryInterface(IID_PPV_ARGS(&m_pDropTarget));
	}
	m_pFolderItem = NULL;
	if (pFolderItem) {
		pFolderItem->QueryInterface(IID_PPV_ARGS(&m_pFolderItem));
	}
}

CteDropTarget::~CteDropTarget()
{
	SafeRelease(&m_pFolderItem);
	SafeRelease(&m_pDropTarget);
}

STDMETHODIMP CteDropTarget::QueryInterface(REFIID riid, void **ppvObject)
{
	static const QITAB qit[] =
	{
		QITABENT(CteDropTarget, IDispatch),
		{ 0 },
	};

	if (m_pFolderItem && (IsEqualIID(riid, IID_FolderItem) || IsEqualIID(riid, g_ClsIdFI))) {
		return m_pFolderItem->QueryInterface(riid, ppvObject);
	}
	return QISearch(this, qit, riid, ppvObject);
}

STDMETHODIMP_(ULONG) CteDropTarget::AddRef()
{
	return ::InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CteDropTarget::Release()
{
	if (::InterlockedDecrement(&m_cRef) == 0) {
		delete this;
		return 0;
	}
	return m_cRef;
}

STDMETHODIMP CteDropTarget::GetTypeInfoCount(UINT *pctinfo)
{
	*pctinfo = 0;
	return S_OK;
}

STDMETHODIMP CteDropTarget::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteDropTarget::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	return teGetDispId(methodDT, 0, NULL, *rgszNames, rgDispId, TRUE);
}

STDMETHODIMP CteDropTarget::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	try {
		HRESULT hr;

		int nArg = pDispParams ? pDispParams->cArgs - 1 : -1;
		if (pVarResult) {
			VariantInit(pVarResult);
		}
		switch(dispIdMember) {
			//DragEnter
			case 1:
			//DragOver
			case 2:
			//Drop
			case 3:
				hr = E_INVALIDARG;
				if (nArg >= 2) {
					BOOL bSingle = FALSE;
					IUnknown *pObj = this;
					IDataObject *pDataObj;
					if (GetDataObjFromVariant(&pDataObj, &pDispParams->rgvarg[nArg])) {
						try {
							DWORD grfKeyState = (DWORD)GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]);
							POINTL pt;
							GetPointFormVariant((POINT *)&pt, &pDispParams->rgvarg[nArg - 2]);
							DWORD dwEffect1 = DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_LINK;
							VARIANT v;
							VariantInit(&v);
							IDispatch *pEffect = NULL;
							if (nArg >= 3) {
								if (GetDispatch(&pDispParams->rgvarg[nArg - 3], &pEffect)) {
									teGetPropertyAt(pEffect, 0, &v);
									dwEffect1 = GetIntFromVariantClear(&v);
									if (nArg >= 4) {
										bSingle = GetIntFromVariant(&pDispParams->rgvarg[nArg - 4]);
										if (nArg >= 5) {
											IUnknown *punk;
											if (FindUnknown(&pDispParams->rgvarg[nArg - 5], &punk)) {
												pObj = punk;
											}
										}
									}
								} else {
									dwEffect1 = GetIntFromVariantClear(&pDispParams->rgvarg[nArg - 3]);
								}
							}
							DWORD dwEffect = dwEffect1;
							CteFolderItems *pDragItems = new CteFolderItems(pDataObj, NULL);
							try {
								POINTL pt0 = {0, 0};
								if (!bSingle || dispIdMember == 1) {//DragEnter
									if (m_pDropTarget) {
										try {
											hr = m_pDropTarget->DragEnter(pDataObj, grfKeyState, pt, &dwEffect1);
										} catch (...) {
											hr = E_UNEXPECTED;
										}
									} else {
										dwEffect1 = 0;
									}
									if (m_pFolderItem) {
										DWORD dwEffect2 = dwEffect;
										if (DragSub(TE_OnDragEnter, pObj, pDragItems, &grfKeyState, pt0, &dwEffect2) == S_OK) {
											hr = S_OK;
											dwEffect1 = dwEffect2;
										}
									}
								} else {
									hr = S_OK;
								}
								if (hr == S_OK && dispIdMember >= 2) {
									if (!bSingle || dispIdMember == 2) {//DragOver
										hr = S_FALSE;
										if (m_pFolderItem) {
											dwEffect1 = dwEffect;
											hr = DragSub(TE_OnDragOver, pObj, pDragItems, &grfKeyState, pt0, &dwEffect1);
										}
										if (hr != S_OK && m_pDropTarget) {
											dwEffect1 = dwEffect;
											try {
												hr = m_pDropTarget->DragOver(grfKeyState, pt, &dwEffect1);
											} catch (...) {
												hr = E_UNEXPECTED;
											}
										}
									}
									if (hr == S_OK && dispIdMember >= 3) {
										if (!bSingle || dispIdMember == 3) {//Drop
											hr = S_FALSE;
											if (m_pFolderItem) {
												dwEffect1 = dwEffect;
												hr = DragSub(TE_OnDrop, pObj, pDragItems, &grfKeyState, pt0, &dwEffect1);
											}
											if (hr != S_OK) {
												dwEffect1 = dwEffect;
												if (m_pDropTarget) {
													try {
														hr = m_pDropTarget->Drop(pDataObj, grfKeyState, pt, &dwEffect1);
													} catch (...) {
														hr = E_UNEXPECTED;
													}
												}
											}
										}
									}
								}
							} catch (...) {
								g_nException = 0;
#ifdef _DEBUG
								g_strException = L"DropTarget";
#endif
							}
							pDragItems->Release();
							if (!bSingle) {
								if (m_pDropTarget) {
									m_pDropTarget->DragLeave();
								}
								if (dispIdMember >= 3) {
									DragLeaveSub(this, NULL);
								}
							}
							if (pEffect) {
								teSetLong(&v, dwEffect1);
								tePutPropertyAt(pEffect, 0, &v);
								pEffect->Release();
							}
						} catch (...) {
							g_nException = 0;
#ifdef _DEBUG
							g_strException = L"DropTarget2";
#endif
						}
						pDataObj->Release();
					}
				}
				teSetLong(pVarResult, hr);
				return S_OK;
			//DragLeave
			case 4:
				if (m_pDropTarget) {
					hr = m_pDropTarget->DragLeave();
				}
				DragLeaveSub(this, NULL);
				teSetLong(pVarResult, hr);
				return S_OK;
			//Type
			case 5:
				teSetLong(pVarResult, CTRL_DT);
				return S_OK;
			//FolderItem
			case 6:
				teSetObject(pVarResult, m_pFolderItem);
				return S_OK;

			case DISPID_VALUE:
				teSetObject(pVarResult, this);
				return S_OK;
		}
	} catch (...) {
		return teException(pExcepInfo, "DropTarget", methodDT, dispIdMember);
	}
	return DISP_E_MEMBERNOTFOUND;
}

//CteTreeView

CteTreeView::CteTreeView()
{
	m_cRef = 1;
	m_DefProc = NULL;
	m_DefProc2 = NULL;
	m_param = NULL;
	m_bMain = TRUE;
	m_pDropTarget2 = NULL;
	VariantInit(&m_vData);
	VariantInit(&m_vRoot);
#ifdef _W2000
	VariantInit(&m_vSelected);
#endif
}

CteTreeView::~CteTreeView()
{
	Close();
	VariantClear(&m_vRoot);
	if (!m_pFV && m_param) {
		delete [] m_param;
		m_param = NULL;
	}
}

VOID CteTreeView::Init(CteShellBrowser *pFV, HWND hwnd)
{
	m_pFV = pFV;
	m_hwndParent = hwnd;
	if (m_pFV) {
		m_param = pFV->m_param;
	} else {
		m_param = new DWORD[SB_Count];
		for (int i = SB_Count; i--;) {
			m_param[i] = g_paramFV[i];
		}
	}
	m_hwnd = NULL;
	m_hwndTV = NULL;
	m_pNameSpaceTreeControl = NULL;
	m_bSetRoot = TRUE;
	m_bEmpty = FALSE;
	teSetLong(&m_vRoot, 0);
#ifdef _2000XP
	m_pShellNameSpace = NULL;
#endif
	m_dwCookie = 0;
	m_bEmpty = FALSE;
}

VOID CteTreeView::Close()
{
	if (m_DefProc) {
#ifdef _2000XP
		if (!g_bUpperVista) {
			SetWindowLongPtr(m_hwnd, GWLP_WNDPROC, (LONG_PTR)m_DefProc);
		}
#endif
		m_DefProc = NULL;
	}
	if (m_DefProc2) {
		SetWindowLongPtr(m_hwndTV, GWLP_WNDPROC, (LONG_PTR)m_DefProc2);
		m_DefProc2 = NULL;
	}
	if (m_pDropTarget2) {
//		SetProp(m_hwndTV, L"OleDropTargetInterface", (HANDLE)m_pDropTarget2->m_pDropTarget);
		RevokeDragDrop(m_hwndTV);
		RegisterDragDrop(m_hwndTV, m_pDropTarget2->m_pDropTarget);
		SafeRelease(&m_pDropTarget2);
	}
	if (m_pNameSpaceTreeControl) {
		m_pNameSpaceTreeControl->RemoveAllRoots();
		m_pNameSpaceTreeControl->TreeUnadvise(m_dwCookie);
		m_dwCookie = 0;

		HWND hwnd;
		if (teGetWindow(m_pNameSpaceTreeControl, &hwnd) == S_OK) {
			PostMessage(hwnd, WM_CLOSE, 0, 0);
		}
		SafeRelease(&m_pNameSpaceTreeControl);
	}
#ifdef _2000XP
	if (m_pShellNameSpace) {
		IOleObject *pOleObject;
		if SUCCEEDED(m_pShellNameSpace->QueryInterface(IID_PPV_ARGS(&pOleObject))) {
			RECT rc;
			SetRectEmpty(&rc);
			pOleObject->SetClientSite(NULL);
			pOleObject->DoVerb(OLEIVERB_HIDE, NULL, NULL, 0, g_hwndMain, &rc);
			pOleObject->Close(OLECLOSE_NOSAVE);
			pOleObject->Release();
			DestroyWindow(m_hwnd);
		}
		SafeRelease(&m_pShellNameSpace);
	}
#endif
	VariantClear(&m_vData);
#ifdef _W2000
	VariantClear(&m_vSelected);
#endif
}

BOOL CteTreeView::Create(BOOL bIfVisible)
{
	if (bIfVisible) {
		if ((m_param[SB_TreeAlign] & 2) == 0 || (m_pFV && !m_pFV->m_bVisible)) {
			return FALSE;
		}
	}
	if (_Emulate_XP_ SUCCEEDED(teCreateInstance(CLSID_NamespaceTreeControl, NULL, NULL, IID_PPV_ARGS(&m_pNameSpaceTreeControl)))) {
		RECT rc;
		SetRectEmpty(&rc);
		if SUCCEEDED(m_pNameSpaceTreeControl->Initialize(m_hwndParent, &rc, m_param[SB_TreeFlags])) {
			m_pNameSpaceTreeControl->TreeAdvise(static_cast<INameSpaceTreeControlEvents *>(this), &m_dwCookie);
			if (teGetWindow(m_pNameSpaceTreeControl, &m_hwnd) == S_OK) {
				m_hwndTV = FindTreeWindow(m_hwnd);
				if (m_hwndTV) {
					SetWindowLongPtr(m_hwndTV, GWLP_USERDATA, (LONG_PTR)this);
					m_DefProc = (WNDPROC)GetWindowLongPtr(GetParent(m_hwndTV), GWLP_WNDPROC);
					m_DefProc2 = (WNDPROC)SetWindowLongPtr(m_hwndTV, GWLP_WNDPROC, (LONG_PTR)TETVProc2);
					if (!m_pDropTarget2) {
						m_pDropTarget2 = new CteDropTarget2(NULL, static_cast<IDispatch *>(this));
						teRegisterDragDrop(m_hwndTV, m_pDropTarget2, &m_pDropTarget2->m_pDropTarget);
					}
					if (m_param[SB_TreeFlags] & NSTCS_NOEDITLABELS) {
						SetWindowLongPtr(m_hwndTV, GWL_STYLE, GetWindowLongPtr(m_hwndTV, GWL_STYLE) & ~TVS_EDITLABELS);
					}
					TreeView_SetTextColor(m_hwndTV, GetSysColor(COLOR_WINDOWTEXT));
				}
				BringWindowToTop(m_hwnd);
				ArrangeWindow();
			}
/*// Too heavy.
			INameSpaceTreeControl2 *pNSTC2;
			if SUCCEEDED(m_pNameSpaceTreeControl->QueryInterface(IID_PPV_ARGS(&pNSTC2))) {
				pNSTC2->SetControlStyle2(NSTCS2_INTERRUPTNOTIFICATIONS, NSTCS2_INTERRUPTNOTIFICATIONS);
				pNSTC2->Release();
			}
*///
			DoFunc(TE_OnCreate, this, E_NOTIMPL);
			return TRUE;
		}
	}
#ifdef _2000XP
	m_hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE, WC_EDIT, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | ES_READONLY,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		m_hwndParent, (HMENU)0, hInst, NULL);
	if FAILED(teCreateInstance(CLSID_ShellShellNameSpace, NULL, NULL, IID_PPV_ARGS(&m_pShellNameSpace))) {
		if FAILED(teCreateInstance(CLSID_ShellNameSpace, NULL, NULL, IID_PPV_ARGS(&m_pShellNameSpace))) {
			return FALSE;
		}
	}
	IQuickActivate *pQuickActivate;
	if (FAILED(m_pShellNameSpace->QueryInterface(IID_PPV_ARGS(&pQuickActivate)))) {
		return FALSE;
	}
	QACONTAINER qaContainer = { sizeof(QACONTAINER), static_cast<IOleClientSite *>(this), NULL, NULL, static_cast<IDispatch *>(this) };
	QACONTROL qaControl = { sizeof(QACONTROL) };
	pQuickActivate->QuickActivate(&qaContainer, &qaControl);
	pQuickActivate->Release();

	IPersistStreamInit  *pPersistStreamInit;
//	IPersistStorage     *pPersistStorage;
//	IPersistMemory      *pPersistMemory;
//	IPersistPropertyBag *pPersistPropertyBag;

	if (m_pShellNameSpace->QueryInterface(IID_PPV_ARGS(&pPersistStreamInit)) == S_OK) {
		pPersistStreamInit->InitNew();
		pPersistStreamInit->Release();
	}
/*/// Not use in IShellNameSpace
	else if (m_pShellNameSpace->QueryInterface(IID_PPV_ARGS(&pPersistStorage)) == S_OK) {
		pPersistStorage->InitNew(NULL);
		pPersistStorage->Release();
	} else if (m_pShellNameSpace->QueryInterface(IID_PPV_ARGS(&pPersistMemory)) == S_OK) {
		pPersistMemory->InitNew();
		pPersistMemory->Release();
	} else if (m_pShellNameSpace->QueryInterface(IID_PPV_ARGS(&pPersistPropertyBag)) == S_OK) {
		pPersistPropertyBag->InitNew();
		pPersistPropertyBag->Release();
	}
//*/
	else {
		return FALSE;
	}
/*/// Not use in IShellNameSpace
	IRunnableObject *pRunnableObject;
	if (qaControl.dwMiscStatus & OLEMISC_ALWAYSRUN) {
		m_pShellNameSpace->QueryInterface(IID_PPV_ARGS(&pRunnableObject));
		if (pRunnableObject) {
			pRunnableObject->Run(NULL);
			pRunnableObject->Release();
		}
	}
//*/
	RECT rc;
	SetRectEmpty(&rc);

	IOleObject *pOleObject;
	if FAILED(m_pShellNameSpace->QueryInterface(IID_PPV_ARGS(&pOleObject))) {
		return FALSE;
	}
	pOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, static_cast<IOleClientSite *>(this), 0, m_hwndParent, &rc);
	pOleObject->Release();

	HWND hwnd;
	if (teGetWindow(m_pShellNameSpace, &hwnd) == S_OK) {
		m_pShellNameSpace->put_Flags(m_param[SB_TreeFlags] & NSTCS_FAVORITESMODE);
		m_hwndTV = FindTreeWindow(hwnd);
		if (m_hwndTV) {
			HWND hwndParent = GetParent(m_hwndTV);
			SetWindowLongPtr(hwndParent, GWLP_USERDATA, (LONG_PTR)this);
			m_DefProc = (WNDPROC)SetWindowLongPtr(hwndParent, GWLP_WNDPROC, (LONG_PTR)TETVProc);
			SetWindowLongPtr(m_hwndTV, GWLP_USERDATA, (LONG_PTR)this);
			m_DefProc2 = (WNDPROC)SetWindowLongPtr(m_hwndTV, GWLP_WNDPROC, (LONG_PTR)TETVProc2);
			if (!m_pDropTarget2) {
				m_pDropTarget2 = new CteDropTarget2(NULL, static_cast<IDispatch *>(this));
				teRegisterDragDrop(m_hwndTV, m_pDropTarget2, &m_pDropTarget2->m_pDropTarget);
			}

			DWORD dwStyle = GetWindowLong(m_hwndTV, GWL_STYLE);
			struct
			{
				DWORD nstc;
				DWORD nsc;
			} style[] = {
				{ NSTCS_HASEXPANDOS, TVS_HASBUTTONS },
				{ NSTCS_HASLINES, TVS_HASLINES },
				{ NSTCS_SINGLECLICKEXPAND, TVS_TRACKSELECT },
				{ NSTCS_FULLROWSELECT, TVS_FULLROWSELECT },
				{ NSTCS_SPRINGEXPAND, TVS_SINGLEEXPAND },
				{ NSTCS_HORIZONTALSCROLL, TVS_NOHSCROLL },//Opposite
				//NSTCS_ROOTHASEXPANDO
				{ NSTCS_SHOWSELECTIONALWAYS, TVS_SHOWSELALWAYS },
				{ NSTCS_NOINFOTIP, TVS_INFOTIP },//Opposite
				{ NSTCS_EVENHEIGHT, TVS_NONEVENHEIGHT },//Opposite
				//NSTCS_NOREPLACEOPEN
				{ NSTCS_DISABLEDRAGDROP, TVS_DISABLEDRAGDROP },
				//NSTCS_NOORDERSTREAM
				{ NSTCS_BORDER, WS_BORDER },
				{ NSTCS_NOEDITLABELS, TVS_EDITLABELS },//Opposite
				//NSTCS_FADEINOUTEXPANDOS
				//NSTCS_EMPTYTEXT
				{ NSTCS_CHECKBOXES, TVS_CHECKBOXES },
				//NSTCS_NOINDENTCHECKS
				//NSTCS_ALLOWJUNCTIONS
				//NSTCS_SHOWTABSBUTTON
				//NSTCS_SHOWDELETEBUTTON
				//NSTCS_SHOWREFRESHBUTTON
			};
			for (int i = _countof(style); i--;) {
				if (m_param[SB_TreeFlags] & style[i].nstc) {
					dwStyle |= style[i].nsc;
				} else {
					dwStyle &= ~style[i].nsc;
				}
			}
			dwStyle ^= (TVS_NOHSCROLL | TVS_INFOTIP | TVS_NONEVENHEIGHT | TVS_EDITLABELS);//Opposite
			SetWindowLongPtr(m_hwndTV, GWL_STYLE, dwStyle & ~WS_BORDER);
			DWORD dwExStyle = GetWindowLong(m_hwnd, GWL_EXSTYLE);
			if (dwStyle & WS_BORDER) {
				dwExStyle |= WS_EX_CLIENTEDGE;
			} else {
				dwExStyle &= ~WS_EX_CLIENTEDGE;
			}
			SetWindowLong(m_hwnd, GWL_EXSTYLE, dwExStyle);
		}
		BringWindowToTop(m_hwnd);
		ArrangeWindow();
	}
	DoFunc(TE_OnCreate, this, E_NOTIMPL);
	return TRUE;
#else
	return FALSE;
#endif
}

STDMETHODIMP CteTreeView::QueryInterface(REFIID riid, void **ppvObject)
{
	static const QITAB qit[] =
	{
		QITABENT(CteTreeView, IDispatch),
		QITABENT(CteTreeView, INameSpaceTreeControlEvents),
		QITABENT(CteTreeView, INameSpaceTreeControlCustomDraw),
//		QITABENT(CteTreeView, IContextMenu),
#ifdef _2000XP
		&DIID_DShellNameSpaceEvents, OFFSETOFCLASS(IDispatch, CteTreeView),
		QITABENT(CteTreeView, IOleClientSite),
		QITABENT(CteTreeView, IOleWindow),
		QITABENT(CteTreeView, IOleInPlaceSite),
//		QITABENT(CteTreeView, IOleControlSite),
#endif
		{ 0 },
	};

	if (m_pNameSpaceTreeControl && IsEqualIID(riid, IID_INameSpaceTreeControl)) {
		return m_pNameSpaceTreeControl->QueryInterface(riid, ppvObject);
	}
#ifdef _2000XP
	if (m_pShellNameSpace) {
		if (IsEqualIID(riid, IID_IOleObject)) {
			return m_pShellNameSpace->QueryInterface(riid, ppvObject);
		}
		if (IsEqualIID(riid, IID_IShellNameSpace)) {
			return m_pShellNameSpace->QueryInterface(riid, ppvObject);
		}
	}
#endif
	return QISearch(this, qit, riid, ppvObject);
}

STDMETHODIMP_(ULONG) CteTreeView::AddRef()
{
	return ::InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CteTreeView::Release()
{
	if (::InterlockedDecrement(&m_cRef) == 0) {
		delete this;
		return 0;
	}

	return m_cRef;
}

STDMETHODIMP CteTreeView::GetTypeInfoCount(UINT *pctinfo)
{
	*pctinfo = 0;
	return S_OK;
}

STDMETHODIMP CteTreeView::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteTreeView::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	return teGetDispId(methodTV, _countof(methodTV), g_maps[MAP_TV], *rgszNames, rgDispId, TRUE);
}

STDMETHODIMP CteTreeView::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	try {
		int nArg = pDispParams ? pDispParams->cArgs - 1 : -1;
		if (pVarResult) {
			VariantInit(pVarResult);
		}

		if (dispIdMember >= 0x10000101 && dispIdMember < TE_OFFSET) {
			if (m_param[SB_TreeAlign] & 2) {
				if (!m_pNameSpaceTreeControl
#ifdef _2000XP
					&& !m_pShellNameSpace
#endif
				) {
					Create(TRUE);
				}
			} else if (dispIdMember != 0x20000002 && dispIdMember != 0x20000003) {
				return S_FALSE;
			}
		}

		switch(dispIdMember) {
			//Data
			case 0x10000001:
				if (nArg >= 0) {
					VariantClear(&m_vData);
					VariantCopy(&m_vData, &pDispParams->rgvarg[nArg]);
				}
				if (pVarResult) {
					VariantCopy(pVarResult, &m_vData);
				}
				return S_OK;
			//Type
			case 0x10000002:
				teSetLong(pVarResult, CTRL_TV);
				return S_OK;
			//hwnd
			case 0x10000003:
				teSetPtr(pVarResult, m_hwnd);
				return S_OK;
			//Close
			case 0x10000004:
				if (m_param) {
					m_param[SB_TreeAlign] = 0;
					ArrangeWindow();
					m_bEmpty = TRUE;
					Close();
				}
				return S_OK;
			//hwnd
			case 0x10000005:
				teSetPtr(pVarResult, m_hwndTV);
				return S_OK;
			//FolderView
			case 0x10000007:
				if (m_pFV) {
					teSetObject(pVarResult, m_pFV);
				} else if (g_pTC) {
					teSetObject(pVarResult, g_pTC->GetShellBrowser(g_pTC->m_nIndex));
				}
				return S_OK;
			//Align
			case 0x10000008:
				if (nArg >= 0) {
					m_param[SB_TreeAlign] = GetIntFromVariant(&pDispParams->rgvarg[nArg]);
					ShowWindow(m_hwnd, m_param[SB_TreeAlign] & 2 ? SW_SHOWNA : SW_HIDE);
					ArrangeWindow();
					if (m_bSetRoot && m_param[SB_TreeAlign] & 2) {
						SetTimer(g_hwndMain, (UINT_PTR)this, 500, teTimerProcSetRoot);
					}
				}
				teSetLong(pVarResult, m_param[SB_TreeAlign]);
				return S_OK;
			//Visible
			case 0x10000009:
				if (nArg >= 0) {
					m_param[SB_TreeAlign] = GetIntFromVariant(&pDispParams->rgvarg[nArg]) ? 3 : 1;
					ShowWindow(m_hwnd, m_param[SB_TreeAlign] & 2 ? SW_SHOWNA : SW_HIDE);
					ArrangeWindow();
					if (m_bSetRoot && m_param[SB_TreeAlign] & 2) {
						SetTimer(g_hwndMain, (UINT_PTR)this, 500, teTimerProcSetRoot);
					}
				}
				teSetBool(pVarResult, m_param[SB_TreeAlign] & 2);
				return S_OK;
			//Focus
			case 0x10000106:
				SetFocus(m_hwndTV);
				return S_OK;
			//HitTest (Screen coordinates)
			case 0x10000107:
				if (nArg >= 0 && pVarResult) {
					TVHITTESTINFO info;
					HTREEITEM hItem;
					GetPointFormVariant(&info.pt, &pDispParams->rgvarg[nArg]);
					UINT flags = nArg >= 1 ? GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]) : TVHT_ONITEM;
					LONG_PTR r = DoHitTest(this, info.pt, flags);
					if (r != -1) {
						hItem = (HTREEITEM)r;
					} else {
						ScreenToClient(m_hwndTV, &info.pt);
						info.flags = flags;
						hItem = TreeView_HitTest(m_hwndTV, &info);
						if (!(info.flags & flags)) {
							hItem = 0;
						}
						if (nArg == 0 && hItem) {
							if (m_pNameSpaceTreeControl) {
								IShellItem *pSI;
								if (m_pNameSpaceTreeControl->HitTest(&info.pt, &pSI) == S_OK) {
									FolderItem *pFI;
									if SUCCEEDED(GetFolderItemFromObject(&pFI, pSI)) {
										teSetObjectRelease(pVarResult, pFI);
									}
									pSI->Release();
								}
								return S_OK;
							}
#ifdef _2000XP
							if (hItem == TreeView_GetSelection(m_hwndTV)) {
								IDispatch *pdisp;
								if (getSelected(&pdisp) == S_OK) {
									teSetObjectRelease(pVarResult, pdisp);
									return S_OK;
								}
							}
#endif
						}
					}
					teSetPtr(pVarResult, hItem);
				}
				return S_OK;
			//GetItemRect
			case 0x10000283:
				if (nArg >= 1) {
					VARIANT vMem;
					VariantInit(&vMem);
					LPRECT prc = (LPRECT)GetpcFromVariant(&pDispParams->rgvarg[nArg - 1], &vMem);
					if (prc) {
						LPITEMIDLIST pidl;
						if (teGetIDListFromVariant(&pidl, &pDispParams->rgvarg[nArg])) {
							if (m_pNameSpaceTreeControl
#ifdef _2000XP
								&& lpfnSHCreateItemFromIDList
#endif
							) {
								IShellItem *pShellItem;
								if SUCCEEDED(lpfnSHCreateItemFromIDList(pidl, IID_PPV_ARGS(&pShellItem))) {
									teSetLong(pVarResult, m_pNameSpaceTreeControl->GetItemRect(pShellItem, prc));
									pShellItem->Release();
								}
							}
							teCoTaskMemFree(pidl);
						}
					}
					teWriteBack(&pDispParams->rgvarg[nArg - 1], &vMem);
				}
				return S_OK;
			//Notify
			case 0x10000300:
				if (nArg >= 2 && m_hwndTV && m_pNameSpaceTreeControl) {
					long lEvent = GetIntFromVariant(&pDispParams->rgvarg[nArg]);
					if (lEvent & (SHCNE_MKDIR | SHCNE_MEDIAINSERTED | SHCNE_DRIVEADD | SHCNE_NETSHARE)) {
						LPITEMIDLIST pidl;
						if (teGetIDListFromVariant(&pidl, &pDispParams->rgvarg[nArg - 1])) {
							IShellItem *psi, *psiParent;
							if SUCCEEDED(lpfnSHCreateItemFromIDList(pidl, IID_PPV_ARGS(&psi))) {
								DWORD dwState;
								if FAILED(m_pNameSpaceTreeControl->GetItemState(psi, NSTCIS_EXPANDED, &dwState)) {
									if SUCCEEDED(psi->GetParent(&psiParent)) {
										if SUCCEEDED(m_pNameSpaceTreeControl->GetItemState(psiParent, NSTCIS_EXPANDED, &dwState)) {
											m_pNameSpaceTreeControl->SetItemState(psi, NSTCIS_EXPANDED, NSTCIS_EXPANDED);
										}
										psiParent->Release();
									}
								}
								psi->Release();
							}
							teCoTaskMemFree(pidl);
						}
					}
					if (nArg >= 4 && m_DefProc && lEvent & (SHCNE_DRIVEREMOVED | SHCNE_MEDIAREMOVED | SHCNE_NETUNSHARE | SHCNE_RENAMEFOLDER | SHCNE_RMDIR | SHCNE_SERVERDISCONNECT | SHCNE_UPDATEDIR)) {
						try {
							CallWindowProc(m_DefProc, GetParent(m_hwndTV), WM_USER + 1, GetPtrFromVariant(&pDispParams->rgvarg[nArg - 3]), GetPtrFromVariant(&pDispParams->rgvarg[nArg - 4]));
						} catch (...) {
							g_nException = 0;
#ifdef _DEBUG
							g_strException = L"Notify_CallWindowProc";
#endif
						}
					}
				}
				return S_OK;
			//SelectedItem
			case 0x20000000:
				IDispatch *pdisp;
				if (getSelected(&pdisp) == S_OK) {
					teSetObjectRelease(pVarResult, pdisp);
					return S_OK;
				}
				break;
			//SelectedItems
			case 0x20000001:
				if (getSelected(&pdisp) == S_OK) {
					CteFolderItems *pFolderItems = new CteFolderItems(NULL, NULL);
					VARIANT v;
					VariantInit(&v);
					teSetObjectRelease(&v, pdisp);
					pFolderItems->ItemEx(-1, NULL, &v);
					VariantClear(&v);
					teSetObjectRelease(pVarResult, pFolderItems);
					return S_OK;
				}
				break;
			//Root
			case 0x20000002:
				if (pVarResult) {
					VariantCopy(pVarResult, &m_vRoot);
					if (pVarResult->vt == VT_NULL || pVarResult->vt == VT_EMPTY) {
						teSetLong(pVarResult, 0);
					}
					if (nArg < 0) {
						return S_OK;
					}
				}
			//SetRoot
			case 0x20000003:
				if (nArg >= 0) {
					m_param[SB_EnumFlags] = g_paramFV[SB_EnumFlags];
					m_param[SB_RootStyle] = g_paramFV[SB_RootStyle];
					if (nArg >= 1) {
						m_param[SB_EnumFlags] = GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]);
						if (nArg >= 2) {
							m_param[SB_RootStyle] = GetIntFromVariant(&pDispParams->rgvarg[nArg - 2]);
						}
					}
					SetRootV(&pDispParams->rgvarg[nArg]);
					return S_OK;
				}
				break;
			//Expand
			case 0x20000004:
				if (nArg >= 1) {
					if (g_nLockUpdate || (m_pFV && m_pFV->m_pTC->m_nLockUpdate)) {
						teSetLong(pVarResult, E_PENDING);
						return S_OK;
					}
					LPITEMIDLIST pidl;
					teGetIDListFromVariant(&pidl, &pDispParams->rgvarg[nArg]);
					if (::ILIsEqual(pidl, g_pidls[CSIDL_RESULTSFOLDER])) {
						::ILRemoveLastID(pidl);
					}
					if (m_pNameSpaceTreeControl
#ifdef _2000XP
						&& lpfnSHCreateItemFromIDList
#endif
					) {
						IShellItem *pShellItem;
						DWORD dwState = NSTCIS_SELECTED;
						if (GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]) != 0) {
							dwState |= NSTCIS_EXPANDED;
						}
						if SUCCEEDED(lpfnSHCreateItemFromIDList(pidl, IID_PPV_ARGS(&pShellItem))) {
							m_pNameSpaceTreeControl->SetItemState(pShellItem, dwState, dwState);
							pShellItem->Release();
							HTREEITEM hItem = TreeView_GetNextSelected(m_hwndTV, NULL);
							RECT rc;
							int w = TreeView_GetItemHeight(m_hwndTV);
							for (int i = 9; i--;) {
								TreeView_GetItemRect(m_hwndTV, hItem, &rc, TRUE);
								if (rc.left >= w) {
									break;
								}
								SendMessage(m_hwndTV, WM_HSCROLL, SB_LINELEFT, 0);
							}
						}
						teCoTaskMemFree(pidl);
						return S_OK;
					}
					teCoTaskMemFree(pidl);
				}
				break;
			//DIID_DShellNameSpaceEvents
#ifdef _W2000
			case DISPID_FAVSELECTIONCHANGE://(for Windows 2000)
				//SelectedItem is empty (Windows 2000)
				//Be sure to set a route in SetRoot (Windows 2000)
				if (nArg >= 3) {
					::VariantCopy(&m_vSelected, &pDispParams->rgvarg[nArg - 3]);
				}
				return E_NOTIMPL;
#endif
#ifdef _2000XP
			case DISPID_SELECTIONCHANGE:
				return DoFunc(TE_OnSelectionChanged, this, E_NOTIMPL);
			case DISPID_DOUBLECLICK:
				return S_OK;
			case DISPID_READYSTATECHANGE:
				return S_OK;
/*			case DISPID_AMBIENT_BACKCOLOR:
				return S_OK;*/
/*			case DISPID_INITIALIZED:
				return E_NOTIMPL;*/
			case DISPID_AMBIENT_LOCALEID:
				teSetLong(pVarResult, (SHORT)GetThreadLocale());
				return S_OK;
			case DISPID_AMBIENT_USERMODE:
				teSetBool(pVarResult, TRUE);
				return S_OK;
			case DISPID_AMBIENT_DISPLAYASDEFAULT:
				teSetBool(pVarResult, FALSE);
				return S_OK;
#endif
			case DISPID_VALUE:
				teSetObject(pVarResult, this);
				return S_OK;
			default:
				if (dispIdMember >= TE_OFFSET && dispIdMember <= TE_OFFSET + SB_RootStyle) {
					if (nArg >= 0) {
						DWORD dw = GetIntFromVariantPP(&pDispParams->rgvarg[nArg], dispIdMember - TE_OFFSET);
						if (dw != m_param[dispIdMember - TE_OFFSET]) {
							m_param[dispIdMember - TE_OFFSET] = dw;
							g_paramFV[dispIdMember - TE_OFFSET] = dw;
							if (dispIdMember == TE_OFFSET + SB_TreeFlags) {
								Close();
								m_bSetRoot = TRUE;
							}
							ArrangeWindow();
						}
					}
					teSetLong(pVarResult, m_param[dispIdMember - TE_OFFSET]);
					return S_OK;
				}
				break;
		}
		if (dispIdMember >= 0x20000000 && dispIdMember < 0x20000000 + TVVERBS) {
#ifdef _2000XP
			if (m_pNameSpaceTreeControl) {
				return S_OK;
			}
			if (m_pShellNameSpace) {
				for (int i = _countof(methodTV); i--;) {
					if (methodTV[i].id == dispIdMember) {
						if (m_pShellNameSpace) {
							IDispatch *pDispatch;
							if SUCCEEDED(m_pShellNameSpace->QueryInterface(IID_PPV_ARGS(&pDispatch))) {
								DISPID dispid;
								BSTR bs = teMultiByteToWideChar(CP_UTF8, methodTV[i].name, -1);
								if SUCCEEDED(pDispatch->GetIDsOfNames(IID_NULL, &bs, 1, LOCALE_USER_DEFAULT, &dispid)) {
									pDispatch->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, wFlags, pDispParams, pVarResult, NULL, NULL);
								}
								::SysFreeString(bs);
								pDispatch->Release();
							}
						}
						break;
					}
				}
			}
#endif
			return S_OK;
		}
	} catch (...) {
		return teException(pExcepInfo, "TreeView", methodTV, dispIdMember);
	}
	return DISP_E_MEMBERNOTFOUND;
}

//INameSpaceTreeControlEvents
STDMETHODIMP CteTreeView::OnItemClick(IShellItem *psi, NSTCEHITTEST nstceHitTest, NSTCSTYLE nsctsFlags)
{
	if (g_pOnFunc[TE_OnItemClick]) {
		VARIANTARG *pv = GetNewVARIANT(4);
		teSetObject(&pv[3], this);

		if (m_pNameSpaceTreeControl) {
			FolderItem *pid;
			if SUCCEEDED(GetFolderItemFromObject(&pid, psi)) {
				teSetObjectRelease(&pv[2], pid);
			}

		} else {
			IDispatch *pdisp;
			if (getSelected(&pdisp) == S_OK) {
				teSetObjectRelease(&pv[2], pdisp);
			}
		}
		teSetLong(&pv[1], nstceHitTest);
		teSetLong(&pv[0], nsctsFlags);
		VARIANT vResult;
		VariantInit(&vResult);
		Invoke4(g_pOnFunc[TE_OnItemClick], &vResult, 4, pv);
		if (vResult.vt != VT_EMPTY) {
			return GetIntFromVariantClear(&vResult);
		}
	}
	return S_FALSE;
}

STDMETHODIMP CteTreeView::OnPropertyItemCommit(IShellItem *psi)
{
	return S_FALSE;
}

STDMETHODIMP CteTreeView::OnItemStateChanging(IShellItem *psi, NSTCITEMSTATE nstcisMask, NSTCITEMSTATE nstcisState)
{
	return S_OK;
}

STDMETHODIMP CteTreeView::OnItemStateChanged(IShellItem *psi, NSTCITEMSTATE nstcisMask, NSTCITEMSTATE nstcisState)
{
	return S_OK;
}

STDMETHODIMP CteTreeView::OnSelectionChanged(IShellItemArray *psiaSelection)
{
	return DoFunc(TE_OnSelectionChanged, this, S_OK);
}

STDMETHODIMP CteTreeView::OnKeyboardInput(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_FALSE;
}

STDMETHODIMP CteTreeView::OnBeforeExpand(IShellItem *psi)
{
	return S_OK;
}

STDMETHODIMP CteTreeView::OnAfterExpand(IShellItem *psi)
{
	return S_OK;
}

STDMETHODIMP CteTreeView::OnBeginLabelEdit(IShellItem *psi)
{
	return S_OK;
}

STDMETHODIMP CteTreeView::OnEndLabelEdit(IShellItem *psi)
{
	return S_OK;
}

STDMETHODIMP CteTreeView::OnGetToolTip(IShellItem *psi, LPWSTR pszTip, int cchTip)
{
	if (g_pOnFunc[TE_OnToolTip]) {
		VARIANTARG *pv = GetNewVARIANT(2);
		teSetObject(&pv[1], this);
		FolderItem *pid;
		if SUCCEEDED(GetFolderItemFromObject(&pid, psi)) {
			teSetObjectRelease(&pv[0], pid);
		}
		VARIANT vResult;
		VariantInit(&vResult);
		Invoke4(g_pOnFunc[TE_OnToolTip], &vResult, 2, pv);
		if (vResult.vt == VT_BSTR) {
			lstrcpyn(pszTip, vResult.bstrVal, cchTip);
			VariantClear(&vResult);
			return S_OK;
		}
	}
	return E_NOTIMPL;
}

STDMETHODIMP CteTreeView::OnBeforeItemDelete(IShellItem *psi)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteTreeView::OnItemAdded(IShellItem *psi, BOOL fIsRoot)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteTreeView::OnItemDeleted(IShellItem *psi, BOOL fIsRoot)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteTreeView::OnBeforeContextMenu(IShellItem *psi, REFIID riid, void **ppv)
{
	*ppv = NULL;
	return E_NOTIMPL;
}

STDMETHODIMP CteTreeView::OnAfterContextMenu(IShellItem *psi, IContextMenu *pcmIn, REFIID riid, void **ppv)
{
	MSG msg1;
	msg1.hwnd = m_hwnd;
	msg1.message = WM_CONTEXTMENU;
	msg1.wParam = 0;
	RECT rc;
	m_pNameSpaceTreeControl->GetItemRect(psi, &rc);
	GetCursorPos(&msg1.pt);
	if (!PtInRect(&rc, msg1.pt)) {
		msg1.pt.x = (rc.left + rc.right) / 2;
		msg1.pt.y = (rc.top + rc.bottom) / 2;
	}
	if (g_pOnFunc[TE_OnShowContextMenu]) {
		if (MessageSubPt(TE_OnShowContextMenu, this, &msg1) == S_OK) {
			return QueryInterface(IID_IContextMenu, ppv);
		}
	}
/*/// For check
	if (pcmIn) {
		HMENU hMenu = CreatePopupMenu();
		if SUCCEEDED(pcmIn->QueryContextMenu(hMenu, 0, 1, 0x7FFF, CMF_EXPLORE)) {
			int nVerb = TrackPopupMenuEx(hMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD, msg1.pt.x, msg1.pt.y, g_hwndMain, NULL);

		}
		DestroyMenu(hMenu);
		return QueryInterface(IID_IContextMenu, ppv);
	}
*///
	*ppv = NULL;
	return E_NOTIMPL;
}

STDMETHODIMP CteTreeView::OnBeforeStateImageChange(IShellItem *psi)
{
	return S_OK;
}

STDMETHODIMP CteTreeView::OnGetDefaultIconIndex(IShellItem *psi, int *piDefaultIcon, int *piOpenIcon)
{
	return E_NOTIMPL;
}

//INameSpaceTreeControlCustomDraw
STDMETHODIMP CteTreeView::PrePaint(HDC hdc, RECT *prc, LRESULT *plres)
{
	*plres = CDRF_NOTIFYITEMDRAW;
	return S_OK;
}

STDMETHODIMP CteTreeView::PostPaint(HDC hdc, RECT *prc)
{
	return S_OK;
}

STDMETHODIMP CteTreeView::ItemPrePaint(HDC hdc, RECT *prc, NSTCCUSTOMDRAW *pnstccdItem, COLORREF *pclrText, COLORREF *pclrTextBk, LRESULT *plres)
{
	NMTVCUSTOMDRAW tvcd = { { { 0, 0, NM_CUSTOMDRAW }, CDDS_ITEMPREPAINT, hdc, *prc, 0, pnstccdItem->uItemState }, *pclrText, *pclrTextBk, pnstccdItem->iLevel };
	teCustomDraw(TE_OnItemPrePaint, NULL, this, pnstccdItem->psi, &tvcd.nmcd, &tvcd, plres);
	*pclrText = tvcd.clrText;
	*pclrTextBk = tvcd.clrTextBk;
	return S_OK;
}

STDMETHODIMP CteTreeView::ItemPostPaint(HDC hdc, RECT *prc, NSTCCUSTOMDRAW *pnstccdItem)
{
	NMTVCUSTOMDRAW tvcd = { { { 0, 0, NM_CUSTOMDRAW }, CDDS_ITEMPOSTPAINT, hdc, *prc, 0, pnstccdItem->uItemState }, 0, 0, pnstccdItem->iLevel };
	LRESULT lres = CDRF_DODEFAULT;
	teCustomDraw(TE_OnItemPostPaint, NULL, this, pnstccdItem->psi, &tvcd.nmcd, &tvcd, &lres);
	return S_OK;
}
//*/
#ifdef _2000XP
//IOleClientSite
STDMETHODIMP CteTreeView::SaveObject()
{
	return E_NOTIMPL;
}

STDMETHODIMP CteTreeView::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteTreeView::GetContainer(IOleContainer **ppContainer)
{
	*ppContainer = NULL;
	return E_NOINTERFACE;
}

STDMETHODIMP CteTreeView::ShowObject()
{
	return S_OK;
}

STDMETHODIMP CteTreeView::OnShowWindow(BOOL fShow)
{
	return S_OK;
}

STDMETHODIMP CteTreeView::RequestNewObjectLayout()
{
	return E_NOTIMPL;
}

//IOleInPlaceSite
STDMETHODIMP CteTreeView::GetWindow(HWND *phwnd)
{
	*phwnd = m_hwnd;
	return S_OK;
}

STDMETHODIMP CteTreeView::ContextSensitiveHelp(BOOL fEnterMode)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteTreeView::CanInPlaceActivate()
{
	return S_OK;
}

STDMETHODIMP CteTreeView::OnInPlaceActivate()
{
	return S_OK;
}

STDMETHODIMP CteTreeView::OnUIActivate()
{
	return S_OK;
}

STDMETHODIMP CteTreeView::GetWindowContext(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	*ppFrame = NULL;
	*ppDoc = NULL;
	GetClientRect(m_hwnd, lprcClipRect);
	CopyRect(lprcPosRect, lprcClipRect);
	return S_OK;
}

STDMETHODIMP CteTreeView::Scroll(SIZE scrollExtant)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteTreeView::OnUIDeactivate(BOOL fUndoable)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteTreeView::OnInPlaceDeactivate()
{
	return E_NOTIMPL;
}

STDMETHODIMP CteTreeView::DiscardUndoState()
{
	return E_NOTIMPL;
}

STDMETHODIMP CteTreeView::DeactivateAndUndo()
{
	return E_NOTIMPL;
}

STDMETHODIMP CteTreeView::OnPosRectChange(LPCRECT lprcPosRect)
{
	return S_OK;
}

/*//IOleControlSite
STDMETHODIMP CteTreeView::OnControlInfoChanged()
{
	return E_NOTIMPL;
}

STDMETHODIMP CteTreeView::LockInPlaceActive(BOOL fLock)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteTreeView::GetExtendedControl(IDispatch **ppDisp)
{
	*ppDisp = NULL;
		return E_NOINTERFACE;
}

STDMETHODIMP CteTreeView::TransformCoords(POINTL *pPtlHimetric, POINTF *pPtfContainer, DWORD dwFlags)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteTreeView::TranslateAccelerator(MSG *pMsg, DWORD grfModifiers)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteTreeView::OnFocus(BOOL fGotFocus)
{
	return S_OK;
}

STDMETHODIMP CteTreeView::ShowPropertyFrame()
{
	return E_NOTIMPL;
}
//*/
#endif

/*/// IContextMenu
STDMETHODIMP CteTreeView::QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
{
	return S_OK;
}

STDMETHODIMP CteTreeView::GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT *pwReserved, LPSTR pszName, UINT cchMax)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteTreeView::InvokeCommand(LPCMINVOKECOMMANDINFO pici)
{
	return S_OK;
}
//*/

VOID CteTreeView::SetRootV(VARIANT *pv)
{
	LPITEMIDLIST pidl1, pidl2;
	teGetIDListFromVariant(&pidl1, &m_vRoot);
	teGetIDListFromVariant(&pidl2, pv);
	if (!ILIsEqual(pidl1, pidl2)) {
		Close();
		VariantClear(&m_vRoot);
		VariantCopy(&m_vRoot, pv);
		m_bSetRoot = TRUE;
		if (m_param[SB_TreeAlign] & 2) {
			SetTimer(g_hwndMain, (UINT_PTR)this, 500, teTimerProcSetRoot);
		}
	}
	teCoTaskMemFree(pidl2);
	teCoTaskMemFree(pidl1);
}

VOID CteTreeView::SetRoot()
{
	HRESULT hr = E_FAIL;
	if (m_pNameSpaceTreeControl) {
		IShellItem *pShellItem;
#ifdef _2000XP
		if (lpfnSHCreateItemFromIDList) {
#endif
			LPITEMIDLIST pidl;
			if (!teGetIDListFromVariant(&pidl, &m_vRoot)) {
				pidl = ::ILClone(g_pidls[CSIDL_DESKTOP]);
			}
			if SUCCEEDED(lpfnSHCreateItemFromIDList(pidl, IID_PPV_ARGS(&pShellItem))) {
				hr = m_pNameSpaceTreeControl->AppendRoot(pShellItem, m_param[SB_EnumFlags], m_param[SB_RootStyle], NULL);
				pShellItem->Release();
			}
			teCoTaskMemFree(pidl);
#ifdef _2000XP
		}
#endif
	}
#ifdef _2000XP
	if (hr != S_OK && m_pShellNameSpace) {
		DWORD dwStyle = GetWindowLong(m_hwndTV, GWL_STYLE);
		if (m_param[SB_RootStyle] & NSTCRS_HIDDEN) {
			dwStyle &= ~TVS_LINESATROOT;
		} else {
			dwStyle |= TVS_LINESATROOT;
		}
		SetWindowLong(m_hwndTV, GWL_STYLE, dwStyle);

		//running to call DISPID_FAVSELECTIONCHANGE SetRoot(Windows 2000)
		m_pShellNameSpace->put_EnumOptions(m_param[SB_EnumFlags]);

		LPITEMIDLIST pidl;
		if (teGetIDListFromVariant(&pidl, &m_vRoot)) {
			BSTR bs;
			if SUCCEEDED(teGetDisplayNameFromIDList(&bs, pidl, SHGDN_FORPARSING)) {
				if (::SysStringLen(bs) && bs[0] == ':') {
					hr = m_pShellNameSpace->SetRoot(bs);
				}
				::SysFreeString(bs);
			}
			teCoTaskMemFree(pidl);
		}
		if (hr != S_OK) {
			if (m_vRoot.vt == VT_BSTR) {
				hr = m_pShellNameSpace->SetRoot(m_vRoot.bstrVal);
				if (hr != S_OK) {
					VARIANT v;
					teVariantChangeType(&v, &m_vRoot, VT_I4);
					hr = m_pShellNameSpace->put_Root(v);
					m_pShellNameSpace->SetRoot(NULL);
				}
			} else {
				hr = m_pShellNameSpace->put_Root(m_vRoot);
				m_pShellNameSpace->SetRoot(NULL);
			}
		}
	}
#endif
	if (hr == S_OK) {
		m_bSetRoot = FALSE;
		DoFunc(TE_OnViewCreated, this, E_NOTIMPL);
	}
}

VOID CteTreeView::Show()
{
	if (!m_pNameSpaceTreeControl
#ifdef _2000XP
		&& !m_pShellNameSpace
#endif
	) {
		Create(FALSE);
	}
	if (m_bSetRoot) {
		SetRoot();
	}
}

#ifdef _2000XP
VOID CteTreeView::SetObjectRect()
{
	if (m_pShellNameSpace) {
		RECT rc;
		GetClientRect(m_hwnd, &rc);
		IOleInPlaceObject *pOleInPlaceObject;
		m_pShellNameSpace->QueryInterface(IID_PPV_ARGS(&pOleInPlaceObject));
		pOleInPlaceObject->SetObjectRects(&rc, &rc);
		pOleInPlaceObject->Release();
	}
}
#endif

HRESULT CteTreeView::getSelected(IDispatch **ppid)
{
	*ppid = NULL;
	HRESULT hr = E_FAIL;
	if (m_pNameSpaceTreeControl) {
		IShellItemArray *psia;
		if SUCCEEDED(m_pNameSpaceTreeControl->GetSelectedItems(&psia)) {
			IShellItem *psi;
			if SUCCEEDED(psia->GetItemAt(0, &psi)) {
				FolderItem *pFI;
				if SUCCEEDED(GetFolderItemFromObject(&pFI, psi)) {
					hr = pFI->QueryInterface(IID_PPV_ARGS(ppid));
					pFI->Release();
				}
				psi->Release();
			}
			psia->Release();
		}
		return hr;
	}
#ifdef _2000XP
	if (m_pShellNameSpace) {
		hr = m_pShellNameSpace->get_SelectedItem(ppid);
#endif
#ifdef _W2000
		if (!*ppid && m_vSelected.vt != VT_EMPTY) {
			*ppid = new CteFolderItem(&m_vSelected);
			hr = S_OK;
		}
#endif
#ifdef _2000XP
	}
#endif
	if (!*ppid) {
		hr = E_FAIL;
	}
	return hr;
}

//IPersist
STDMETHODIMP CteTreeView::GetClassID(CLSID *pClassID)
{
	*pClassID = CLSID_ShellShellNameSpace;
	return S_OK;
}

//IPersistFolder
STDMETHODIMP CteTreeView::Initialize(PCIDLIST_ABSOLUTE pidl)
{
	return E_NOTIMPL;
}

//IPersistFolder2
STDMETHODIMP CteTreeView::GetCurFolder(PIDLIST_ABSOLUTE *ppidl)
{
	IDispatch *pdisp;
	HRESULT hr = getSelected(&pdisp);
	if SUCCEEDED(hr) {
		teGetIDListFromObject(pdisp, ppidl);
		pdisp->Release();
	}
	return S_OK;
}

// CteFolderItem

CteFolderItem::CteFolderItem(VARIANT *pv)
{
	m_cRef = 1;
	m_pidl = NULL;
	m_pidlAlt = NULL;
	m_pFolderItem = NULL;
	m_bStrict = FALSE;
	m_pidlFocused = NULL;
	m_pEnum = NULL;
	m_nSelected = 0;
	m_dwUnavailable = 0;
	m_dwSessionId = g_dwSessionId;
	VariantInit(&m_v);
	if (pv) {
		CteFolderItem *pid;
		if (pv->vt == VT_DISPATCH && SUCCEEDED(pv->pdispVal->QueryInterface(g_ClsIdFI, (LPVOID *)&pid))) {
			m_pidl = ILClone(pid->m_pidl);
			m_pidlAlt = ILClone(pid->m_pidlAlt);
			m_pidlFocused = ILClone(pid->m_pidlFocused);
			if (pid->m_pFolderItem) {
				pid->QueryInterface(IID_PPV_ARGS(&m_pFolderItem));
			}
			m_bStrict = pid->m_bStrict;
			if (pid->m_pEnum) {
				pid->QueryInterface(IID_PPV_ARGS(&m_pEnum));
			}
			m_nSelected = pid->m_nSelected;
			m_dwUnavailable = pid->m_dwUnavailable;
			m_dwSessionId = pid->m_dwSessionId;
			VariantCopy(&m_v, &pid->m_v);
			pid->Release();
		} else {
			VariantCopy(&m_v, pv);
		}
	}
}

CteFolderItem::~CteFolderItem()
{
	teCoTaskMemFree(m_pidlAlt);
	teCoTaskMemFree(m_pidl);
	SafeRelease(&m_pFolderItem);
	SafeRelease(&m_pEnum);
	::VariantClear(&m_v);
	teCoTaskMemFree(m_pidlFocused);
}

LPITEMIDLIST CteFolderItem::GetPidl()
{
	if (m_v.vt != VT_EMPTY) {
		if (m_pidl == NULL) {
			if (m_v.vt == VT_BSTR) {
				if (tePathMatchSpec(m_v.bstrVal, L"*\\..")) {
					BSTR bs = ::SysAllocString(m_v.bstrVal);
					PathRemoveFileSpec(bs);
					LPITEMIDLIST pidl = teILCreateFromPathEx(bs);
					::SysFreeString(bs);
					if (pidl) {
						teILCloneReplace(&m_pidlFocused, ILFindLastID(pidl));
						m_nSelected = 1;
						teCoTaskMemFree(pidl);
					}
				}
			}
			if (!teGetIDListFromVariant(&m_pidl, &m_v)) {
				if (m_v.vt == VT_BSTR) {
					m_pidl = teSHSimpleIDListFromPath(m_v.bstrVal, FALSE, FILE_ATTRIBUTE_HIDDEN);
				}
				if (!m_pidlAlt) {
					m_dwUnavailable = GetTickCount();
				}
			}
		}
	}
	return m_pidl ? m_pidl : m_pidlAlt ? m_pidlAlt : g_pidls[CSIDL_DESKTOP];
}

LPITEMIDLIST CteFolderItem::GetAlt()
{
	if (m_pEnum && m_dwSessionId != g_dwSessionId) {
		m_pEnum = NULL;
		Clear();
	}
	if (m_pidlAlt == NULL) {
		m_dwSessionId = g_dwSessionId;
		if (m_v.vt == VT_BSTR) {
			if (g_pOnFunc[TE_OnTranslatePath]) {
				try {
					if (InterlockedIncrement(&g_nProcFI) < 5) {
						VARIANTARG *pv = GetNewVARIANT(2);
						teSetObject(&pv[1], this);
						VariantCopy(&pv[0], &m_v);
						VARIANT vPath;
						::VariantInit(&vPath);
						Invoke4(g_pOnFunc[TE_OnTranslatePath], &vPath, 2, pv);
						if (vPath.vt != VT_EMPTY && vPath.vt != VT_NULL) {
							if (teGetIDListFromVariant(&m_pidlAlt, &vPath)) {
								m_dwUnavailable = 0;
							}
						}
						VariantClear(&vPath);
					}
				} catch(...) {
					g_nException = 0;
#ifdef _DEBUG
					g_strException = L"TranslatePath";
#endif
				}
				::InterlockedDecrement(&g_nProcFI);
			}
		}
	}
	if (m_pidlAlt == NULL) {
		m_pidlAlt = ILClone(m_pidl);
	}
	return m_pidlAlt;
}

BOOL CteFolderItem::GetFolderItem()
{
	if (m_pFolderItem) {
		return TRUE;
	}
	return GetFolderItemFromIDList2(&m_pFolderItem, GetPidl());
}

VOID CteFolderItem::Clear()
{
	teILFreeClear(&m_pidl);
	teILFreeClear(&m_pidlAlt);
	m_dwUnavailable = 0;
	m_dispExtendedProperty = -1;
}

BSTR CteFolderItem::GetStrPath()
{
	if (m_v.vt == VT_BSTR) {
		if (m_pidl == NULL || (m_pidlAlt && !ILIsEqual(m_pidl, m_pidlAlt))) {
			return m_v.bstrVal;
		}
	}
	return NULL;
}

VOID CteFolderItem::MakeUnavailable()
{
	teILCloneReplace(&m_pidlAlt, g_pidls[CSIDL_RESULTSFOLDER]);
	m_dwUnavailable = GetTickCount();
}

STDMETHODIMP CteFolderItem::QueryInterface(REFIID riid, void **ppvObject)
{
	static const QITAB qit[] =
	{
		QITABENT(CteFolderItem, IDispatch),
		QITABENT(CteFolderItem, FolderItem),
		QITABENT(CteFolderItem, IPersist),
		QITABENT(CteFolderItem, IPersistFolder),
		QITABENT(CteFolderItem, IPersistFolder2),
		QITABENT(CteFolderItem, IParentAndItem),
		{ 0 },
	};
	if (IsEqualIID(riid, g_ClsIdFI)) {
		*ppvObject = this;
		AddRef();
		return S_OK;
	}
	HRESULT hr = QISearch(this, qit, riid, ppvObject);
	if FAILED(hr) {
		if (IsEqualIID(riid, IID_FolderItem2) || IsEqualIID(riid, CLSID_ShellFolderItem)) {
			GetFolderItem();
		}
		if (m_pFolderItem) {
			hr = m_pFolderItem->QueryInterface(riid, ppvObject);
		}
	}
	return hr;
}

STDMETHODIMP_(ULONG) CteFolderItem::AddRef()
{
	return ::InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CteFolderItem::Release()
{
	if (::InterlockedDecrement(&m_cRef) == 0) {
		delete this;
		return 0;
	}

	return m_cRef;
}

STDMETHODIMP CteFolderItem::GetTypeInfoCount(UINT *pctinfo)
{
	*pctinfo = 0;
	return S_OK;
}

STDMETHODIMP CteFolderItem::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	return E_NOTIMPL;
}

//IPersist
STDMETHODIMP CteFolderItem::GetClassID(CLSID *pClassID)
{
	*pClassID = CLSID_FolderItem;
	return S_OK;
}

//IPersistFolder
STDMETHODIMP CteFolderItem::Initialize(PCIDLIST_ABSOLUTE pidl)
{
	teILCloneReplace(&m_pidl, pidl);
	VariantClear(&m_v);
	teILFreeClear(&m_pidlAlt);
	m_dwUnavailable = 0;
	return S_OK;
}

//IPersistFolder2
STDMETHODIMP CteFolderItem::GetCurFolder(PIDLIST_ABSOLUTE *ppidl)
{
	*ppidl = ::ILClone(GetPidl());
	return S_OK;
}

//IParentAndItem
STDMETHODIMP CteFolderItem::GetParentAndItem(PIDLIST_ABSOLUTE *ppidlParent, IShellFolder **ppsf, PITEMID_CHILD *ppidlChild)
{
	HRESULT hr = E_NOTIMPL;
	LPITEMIDLIST pidl = GetPidl();
	if (pidl) {
		if (ppsf) {
			LPCITEMIDLIST pidlChild;
			SHBindToParent(const_cast<LPCITEMIDLIST>(pidl), IID_PPV_ARGS(ppsf), &pidlChild);
		}
		if (ppidlParent) {
			*ppidlParent = ILClone(pidl);
			ILRemoveLastID(*ppidlParent);
		}
		*ppidlChild = ILClone(ILFindLastID(pidl));
		hr = S_OK;
	}
	return hr;
}

STDMETHODIMP CteFolderItem::SetParentAndItem(PCIDLIST_ABSOLUTE pidlParent, IShellFolder *psf, PCUITEMID_CHILD pidlChild)
{
	return E_NOTIMPL;
}

//IDispatch
STDMETHODIMP CteFolderItem::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	HRESULT hr = teGetDispId(methodFI, 0, NULL, *rgszNames, rgDispId, TRUE);
	if SUCCEEDED(hr) {
		return S_OK;
	}
	if (GetFolderItem()) {
		hr = m_pFolderItem->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgDispId);
		if (SUCCEEDED(hr) && *rgDispId > 0) {
			if (lstrcmpi(*rgszNames, L"ExtendedProperty") == 0) {
				m_dispExtendedProperty = *rgDispId;
			}
			*rgDispId += 10;
		}
	}
#ifdef _DEBUG
	if FAILED(hr) {
		Sleep(0);
	}
#endif
	return hr;
}

STDMETHODIMP CteFolderItem::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	int i;
	try {
		HRESULT hr = E_FAIL;
		int nArg = pDispParams ? pDispParams->cArgs - 1 : -1;
		if (pVarResult) {
			VariantInit(pVarResult);
		}
		switch(dispIdMember) {
			//Name
			case 1:
				if (nArg >= 0) {
					hr = put_Name(pDispParams->rgvarg[nArg].bstrVal);
				}
				if (pVarResult) {
					hr = get_Name(&pVarResult->bstrVal);
					if SUCCEEDED(hr) {
						pVarResult->vt = VT_BSTR;
					}
				}
				return hr;
			//Path
			case 2:
				if (pVarResult) {
					if SUCCEEDED(get_Path(&pVarResult->bstrVal)) {
						pVarResult->vt = VT_BSTR;
					}
				}
				return S_OK;
			//Alt
			case 3:
				if (nArg >= 0) {
					teILFreeClear(&m_pidlAlt);
					if (teGetIDListFromVariant(&m_pidlAlt, &pDispParams->rgvarg[nArg])) {
						m_dwUnavailable = 0;
					}
				}
				if (pVarResult) {
					teSetIDList(pVarResult, GetAlt());
				}
				return S_OK;
			//Unavailable
			case 5:
				i = 0;
				if (nArg >= 0) {
					m_dwUnavailable = GetIntFromVariant(&pDispParams->rgvarg[nArg]);
					if (m_dwUnavailable) {
						m_dwUnavailable += GetTickCount();
					}
				}
				if (m_dwUnavailable) {
					i = GetTickCount() - m_dwUnavailable;
					if (i == 0) {
						i = 1;
					}
					if (i < 0) {
						i = MAXINT;
					}
				}
				teSetLong(pVarResult, i);
				return S_OK;
			//Enum
			case 6:
				if (pVarResult || wFlags & DISPATCH_METHOD) {
					GetAlt();
				}
				teInvokeObject(&m_pEnum, wFlags, pVarResult, nArg, pDispParams->rgvarg);
				return S_OK;
			//_BLOB ** To be necessary
			case 9:
				return S_OK;
/*/// Reserved future
			//FocusedItem
			case 8:
				if (nArg >= 0) {
					teILFreeClear(&m_pidlFocused);
					teGetIDListFromVariant(&m_pidlFocused, &pDispParams->rgvarg[nArg]);
				}
				if (pVarResult) {
					teSetIDList(pVarResult, m_pidlFocused);
				}
				return S_OK;
//*/
			case DISPID_VALUE:
				teSetObject(pVarResult, this);
				return S_OK;
		}
		if (GetFolderItem()) {
			if (dispIdMember > 10) {
				dispIdMember -= 10;
			}
			hr = m_pFolderItem->Invoke(dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
			if (dispIdMember == m_dispExtendedProperty && pVarResult && nArg >= 0 && !pVarResult->bstrVal && pDispParams->rgvarg[nArg].vt == VT_BSTR) {
				PROPERTYKEY propKey;
				if SUCCEEDED(lpfnPSPropertyKeyFromStringEx(pDispParams->rgvarg[nArg].bstrVal, &propKey)) {
					if (IsEqualPropertyKey(propKey, PKEY_Link_TargetParsingPath)) {
						BSTR bs;
						if SUCCEEDED(get_Path(&bs)) {
							teGetJunctionLinkTarget(bs, &pVarResult->bstrVal, -1);
							if (pVarResult->bstrVal) {
								pVarResult->vt = VT_BSTR;
							}
							teSysFreeString(&bs);
						}
					}
				}
			}
			return hr;
		}
	} catch (...) {
		return teException(pExcepInfo, "FolderItem", methodFI, dispIdMember);
	}
	return DISP_E_MEMBERNOTFOUND;
}

STDMETHODIMP CteFolderItem::get_Application(IDispatch **ppid)
{
	return GetFolderItem() ? m_pFolderItem->get_Application(ppid) : E_FAIL;
}

STDMETHODIMP CteFolderItem::get_Parent(IDispatch **ppid)
{
	return GetFolderItem() ? m_pFolderItem->get_Parent(ppid) : E_FAIL;
}

STDMETHODIMP CteFolderItem::get_Name(BSTR *pbs)
{
	BSTR bs = GetStrPath();
	if (bs && lstrlen(bs) > 2) {
		LPWSTR pszName = PathFindFileName(bs);
		if (pszName && pszName[0] != ':') {
			*pbs = ::SysAllocString(pszName);
			return S_OK;
		}
	}
	return teGetDisplayNameFromIDList(pbs, GetPidl(), SHGDN_INFOLDER);
}

STDMETHODIMP CteFolderItem::put_Name(BSTR bs)
{
	HRESULT hr = E_FAIL;
	if (g_pOnFunc[TE_OnSetName]) {
		VARIANT v;
		VariantInit(&v);
		VARIANTARG *pv = GetNewVARIANT(2);
		teSetObject(&pv[1], this);
		teSetSZ(&pv[0], bs);
		Invoke4(g_pOnFunc[TE_OnSetName], &v, 2, pv);
		if (v.vt != VT_EMPTY) {
			return GetIntFromVariantClear(&v);
		}
	}
	if (GetPidl() && m_pidl) {
		IShellFolder *pSF;
		LPCITEMIDLIST pidlPart;
		if SUCCEEDED(SHBindToParent(m_pidl, IID_PPV_ARGS(&pSF), &pidlPart)) {
			BSTR bs2 = bs;
			if (bs[0] == '.' && !StrChr(&bs[1], '.')) {
				int i = ::SysStringLen(bs);
				bs2 = ::SysAllocStringLen(NULL, i);
				lstrcpy(bs2, bs);
				bs2[i] = '.';
				bs2[i + 1] = NULL;
			}
			hr = pSF->SetNameOf(g_hwndMain, pidlPart, bs2, SHGDN_NORMAL, NULL);
			if (bs != bs2) {
				::SysFreeString(bs2);
			}
			pSF->Release();
		}
	}
	return hr;
}

STDMETHODIMP CteFolderItem::get_Path(BSTR *pbs)
{
	BSTR bs = GetStrPath();
	if (bs) {
		*pbs = ::SysAllocString(bs);
		return S_OK;
	}
	return teGetDisplayNameFromIDList(pbs, GetPidl(), SHGDN_FORADDRESSBAR | SHGDN_FORPARSING);
}

STDMETHODIMP CteFolderItem::get_GetLink(IDispatch **ppid)
{
	return GetFolderItem() ? m_pFolderItem->get_GetLink(ppid) : E_FAIL;
}

STDMETHODIMP CteFolderItem::get_GetFolder(IDispatch **ppid)
{
	return GetFolderObjFromIDList(GetPidl(), (Folder **)ppid);
}

STDMETHODIMP CteFolderItem::get_IsLink(VARIANT_BOOL *pb)
{
	return GetFolderItem() ? m_pFolderItem->get_IsLink(pb) : E_FAIL;
}

STDMETHODIMP CteFolderItem::get_IsFolder(VARIANT_BOOL *pb)
{
	return GetFolderItem() ? m_pFolderItem->get_IsFolder(pb) : E_FAIL;
}

STDMETHODIMP CteFolderItem::get_IsFileSystem(VARIANT_BOOL *pb)
{
	return GetFolderItem() ? m_pFolderItem->get_IsFileSystem(pb) : E_FAIL;
}

STDMETHODIMP CteFolderItem::get_IsBrowsable(VARIANT_BOOL *pb)
{
	return GetFolderItem() ? m_pFolderItem->get_IsBrowsable(pb) : E_FAIL;
}

STDMETHODIMP CteFolderItem::get_ModifyDate(DATE *pdt)
{
	return GetFolderItem() ? m_pFolderItem->get_ModifyDate(pdt) : E_FAIL;
}

STDMETHODIMP CteFolderItem::put_ModifyDate(DATE dt)
{
	return GetFolderItem() ? m_pFolderItem->put_ModifyDate(dt) : E_FAIL;
}

STDMETHODIMP CteFolderItem::get_Size(LONG *pul)
{
	return GetFolderItem() ? m_pFolderItem->get_Size(pul) : E_FAIL;
}

STDMETHODIMP CteFolderItem::get_Type(BSTR *pbs)
{
	return GetFolderItem() ? m_pFolderItem->get_Type(pbs) : E_FAIL;
}

STDMETHODIMP CteFolderItem::Verbs(FolderItemVerbs **ppfic)
{
	return GetFolderItem() ? m_pFolderItem->Verbs(ppfic) : E_FAIL;
}

STDMETHODIMP CteFolderItem::InvokeVerb(VARIANT vVerb)
{
	return GetFolderItem() ? m_pFolderItem->InvokeVerb(vVerb) : E_FAIL;
}

//CteCommonDialog

CteCommonDialog::CteCommonDialog()
{
	m_cRef = 1;

	::ZeroMemory(&m_ofn, sizeof(OPENFILENAME));
	m_ofn.lStructSize = sizeof(OPENFILENAME);
	m_ofn.hwndOwner = GetForegroundWindow();
//	m_ofn.hInstance = hInst;
	m_ofn.nMaxFile = MAX_PATH;
}

CteCommonDialog::~CteCommonDialog()
{

	teSysFreeString(&m_ofn.lpstrFile);
	teSysFreeString(const_cast<BSTR *>(&m_ofn.lpstrInitialDir));
	teSysFreeString(const_cast<BSTR *>(&m_ofn.lpstrFilter));
	teSysFreeString(const_cast<BSTR *>(&m_ofn.lpstrDefExt));
	teSysFreeString(const_cast<BSTR *>(&m_ofn.lpstrTitle));
}

STDMETHODIMP CteCommonDialog::QueryInterface(REFIID riid, void **ppvObject)
{
	static const QITAB qit[] =
	{
		QITABENT(CteCommonDialog, IDispatch),
		{ 0 },
	};
	return QISearch(this, qit, riid, ppvObject);
}

STDMETHODIMP_(ULONG) CteCommonDialog::AddRef()
{
	return ::InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CteCommonDialog::Release()
{
	if (::InterlockedDecrement(&m_cRef) == 0) {
		delete this;
		return 0;
	}

	return m_cRef;
}

STDMETHODIMP CteCommonDialog::GetTypeInfoCount(UINT *pctinfo)
{
	*pctinfo = 0;
	return S_OK;
}

STDMETHODIMP CteCommonDialog::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteCommonDialog::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	return teGetDispId(methodCD, _countof(methodCD), g_maps[MAP_CD], *rgszNames, rgDispId, TRUE);
}

STDMETHODIMP CteCommonDialog::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	try {
		LPCWSTR *pbs = NULL;
		DWORD *pdw = NULL;
		int nArg = pDispParams ? pDispParams->cArgs - 1 : -1;
		if (pVarResult) {
			VariantInit(pVarResult);
		}
		switch(dispIdMember) {
			//FileName
			case 10:
				if (nArg >= 0) {
					VARIANT vFile;
					teVariantChangeType(&vFile, &pDispParams->rgvarg[nArg], VT_BSTR);
					if (!m_ofn.lpstrFile) {
						m_ofn.lpstrFile = teSysAllocStringLenEx(vFile.bstrVal, m_ofn.nMaxFile);
					} else {
						lstrcpyn(m_ofn.lpstrFile, vFile.bstrVal, m_ofn.nMaxFile);
					}
					VariantClear(&vFile);
				}
				if (pVarResult) {
					pVarResult->bstrVal = teSysAllocStringLen(m_ofn.lpstrFile, m_ofn.nMaxFile);
					pVarResult->vt = VT_BSTR;
				}
				return S_OK;
			//Filter
			case 13:
				if (nArg >= 0) {
					teSysFreeString(const_cast<BSTR *>(&m_ofn.lpstrFilter));
					if (pDispParams->rgvarg[nArg].vt == VT_BSTR) {
						teSysFreeString(const_cast<BSTR *>(&m_ofn.lpstrFilter));
						int i = ::SysStringLen(pDispParams->rgvarg[nArg].bstrVal);
						BSTR bs = teSysAllocStringLenEx(pDispParams->rgvarg[nArg].bstrVal, i + 1);
						while (i >= 0) {
							if (bs[i] == '|') {
								bs[i] = NULL;
							}
							i--;
						}
						m_ofn.lpstrFilter = bs;
					}
				}
				teSetSZ(pVarResult, m_ofn.lpstrInitialDir);
				return S_OK;
			case DISPID_VALUE:
				teSetObject(pVarResult, this);
				return S_OK;
		}
		if (dispIdMember >= 40) {
			if (!m_ofn.lpstrFile) {
				m_ofn.lpstrFile = ::SysAllocStringLen(NULL, m_ofn.nMaxFile);
				m_ofn.lpstrFile[0] = NULL;
			}
			BOOL bResult = FALSE;
			switch (dispIdMember) {
				//ShowOpen
				case 40:
					if (m_ofn.Flags & OFN_ENABLEHOOK) {
						m_ofn.lpfnHook = OFNHookProc;
					}
					g_bDialogOk = FALSE;
					bResult = GetOpenFileName(&m_ofn) || g_bDialogOk;
					break;
				//ShowSave
				case 41:
					bResult = GetSaveFileName(&m_ofn);
					break;
/*/// IFileOpenDialog
				case 42:
					{
						IFileOpenDialog *pFileOpenDialog;
						if (lpfnSHCreateItemFromIDList && SUCCEEDED(teCreateInstance(CLSID_FileOpenDialog, NULL, NULL, IID_PPV_ARGS(&pFileOpenDialog)))) {
							IShellItem *psi;
							LPITEMIDLIST pidl = teILCreateFromPath(const_cast<LPWSTR>(m_ofn.lpstrInitialDir));
							if (pidl) {
								if SUCCEEDED(lpfnSHCreateItemFromIDList(pidl, IID_PPV_ARGS(&psi))) {
									pFileOpenDialog->SetFolder(psi);
									psi->Release();
								}
								teCoTaskMemFree(pidl);
							}
							pFileOpenDialog->SetFileName(m_ofn.lpstrFile);
							DWORD dwOptions;
							pFileOpenDialog->GetOptions(&dwOptions);
							pFileOpenDialog->SetOptions(dwOptions | FOS_PICKFOLDERS);
							if SUCCEEDED(pFileOpenDialog->Show(g_hwndMain)) {
								if SUCCEEDED(pFileOpenDialog->GetResult(&psi)) {
									if (GetIDListFromObject(psi, &pidl)) {
										BSTR bs;
										teGetDisplayNameFromIDList(&bs, pidl, SHGDN_FORPARSING);
										teCoTaskMemFree(pidl);
										lstrcpyn(m_ofn.lpstrFile, bs, m_ofn.nMaxFile);
										bResult = TRUE;
									}
									psi->Release();
								}
							}
							pFileOpenDialog->Release();
							break;
						}
#ifdef _2000XP
						m_ofn.Flags |= OFN_ENABLEHOOK;
						m_ofn.lpfnHook = OFNHookProc;
						g_bDialogOk = FALSE;
						bResult = GetOpenFileName(&m_ofn) || g_bDialogOk;
						break;
						BROWSEINFO bi;
						bi.hwndOwner = g_hwndMain;
						bi.pidlRoot = NULL;
						bi.pszDisplayName =	m_ofn.lpstrFile;
						bi.lpszTitle = m_ofn.lpstrFileTitle;
						bi.ulFlags = BIF_EDITBOX;
						bi.lpfn = NULL;
						LPITEMIDLIST pidl =	SHBrowseForFolder(&bi);
						if (pidl) {
							BSTR bs;
							teGetDisplayNameFromIDList(&bs, pidl, SHGDN_FORPARSING);
							teCoTaskMemFree(pidl);
							lstrcpyn(m_ofn.lpstrFile, bs, m_ofn.nMaxFile);
							::SysFreeString(bs);
							bResult = TRUE;
						}
#endif
					}
					break;
//*/
			}//end_switch
			teSetBool(pVarResult, bResult);
			return S_OK;
		}
		if (dispIdMember >= 30) {
			switch (dispIdMember) {
				case 30:
					pdw = &m_ofn.nMaxFile;
					break;
				case 31:
					pdw = &m_ofn.Flags;
					break;
				case 32:
					pdw = &m_ofn.nFilterIndex;
					break;
				case 33:
					pdw = &m_ofn.FlagsEx;
					break;
			}//end_switch
			if (nArg >= 0) {
				*pdw = GetIntFromVariant(&pDispParams->rgvarg[nArg]);
			}
			teSetLong(pVarResult, *pdw);
			return S_OK;
		}
		if (dispIdMember >= 20) {
			switch (dispIdMember) {
				case 20:
					pbs = &m_ofn.lpstrInitialDir;
					break;
				case 21:
					pbs = &m_ofn.lpstrDefExt;
					break;
				case 22:
					pbs = &m_ofn.lpstrTitle;
					break;
			}//end_switch
			if (nArg >= 0) {
				if (pDispParams->rgvarg[nArg].vt == VT_BSTR) {
					SysReAllocString(const_cast<BSTR *>(pbs), pDispParams->rgvarg[nArg].bstrVal);
				}
			}
			teSetSZ(pVarResult, *pbs);
			return S_OK;
		}
	} catch (...) {
		return teException(pExcepInfo, "CommonDialog", methodCD, dispIdMember);
	}
	return DISP_E_MEMBERNOTFOUND;
}

//CteWICBitmap

CteWICBitmap::CteWICBitmap()
{
	m_cRef = 1;
	m_uFrame = 0;
	m_uFrameCount = 1;
	m_pImage = NULL;
	m_pStream = NULL;
	m_ppMetadataQueryReader[0] = NULL;
	m_ppMetadataQueryReader[1] = NULL;
}

CteWICBitmap::~CteWICBitmap()
{
	ClearImage(TRUE);
}

VOID CteWICBitmap::ClearImage(BOOL bAll)
{
	if (bAll) {
		m_uFrame = 0;
		m_uFrameCount = 1;
		SafeRelease(&m_pStream);
		SafeRelease(&m_ppMetadataQueryReader[0]);
		SafeRelease(&m_ppMetadataQueryReader[1]);
	}
	SafeRelease(&m_pImage);
}

STDMETHODIMP CteWICBitmap::QueryInterface(REFIID riid, void **ppvObject)
{
	static const QITAB qit[] =
	{
		QITABENT(CteWICBitmap, IDispatch),
		{ 0 },
	};
	return QISearch(this, qit, riid, ppvObject);
}

STDMETHODIMP_(ULONG) CteWICBitmap::AddRef()
{
	return ::InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CteWICBitmap::Release()
{
	if (::InterlockedDecrement(&m_cRef) == 0) {
		delete this;
		return 0;
	}
	return m_cRef;
}

STDMETHODIMP CteWICBitmap::GetTypeInfoCount(UINT *pctinfo)
{
	*pctinfo = 0;
	return S_OK;
}

STDMETHODIMP CteWICBitmap::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteWICBitmap::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	return teGetDispId(methodGB, _countof(methodGB), g_maps[MAP_GB], *rgszNames, rgDispId, TRUE);
}

HBITMAP CteWICBitmap::GetHBITMAP(COLORREF clBk)
{
	BOOL bFixAlpha = FALSE;
	if (clBk != -2) {
		WICPixelFormatGUID guidPF;
		m_pImage->GetPixelFormat(&guidPF);
		if (clBk == -1 || !IsEqualGUID(guidPF, GUID_WICPixelFormat24bppBGR)) {
			bFixAlpha = IsEqualGUID(guidPF, GUID_WICPixelFormat32bppBGRA) || (clBk && Get(GUID_WICPixelFormat32bppBGRA));
			if (clBk == -1) {
				clBk = 0;
			}
		}
	}
	HBITMAP hBM = NULL;
	UINT w = 0, h = 0;
	m_pImage->GetSize(&w, &h);
	if (w && h) {
		IWICBitmapLock *pBitmapLock;
		WICRect rc = {0, 0, w, h};
		if SUCCEEDED(m_pImage->Lock(&rc, WICBitmapLockRead, &pBitmapLock)) {
			UINT cbStride;
			if SUCCEEDED(pBitmapLock->GetStride(&cbStride)) {
				UINT uCount = 0;
				IWICPalette *pPalette = NULL;
				if SUCCEEDED(g_pWICFactory->CreatePalette(&pPalette)) {
					if SUCCEEDED(m_pImage->CopyPalette(pPalette)) {
						pPalette->GetColorCount(&uCount);
					}
				}
				LPBITMAPINFO lpbmi = (LPBITMAPINFO)::SysAllocStringByteLen(NULL, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * uCount);
				if (lpbmi) {
					UINT cbSize = 0;
					RGBQUAD *lpBits, *pbData;
					if SUCCEEDED(pBitmapLock->GetDataPointer(&cbSize, (PBYTE *)&pbData)) {
						::ZeroMemory(&lpbmi->bmiHeader, sizeof(BITMAPINFOHEADER));
						lpbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
						lpbmi->bmiHeader.biWidth = w;
						lpbmi->bmiHeader.biHeight = -(LONG)h;
						lpbmi->bmiHeader.biPlanes = 1;
						lpbmi->bmiHeader.biBitCount = cbStride * 8 / w;
						if (uCount) {
							pPalette->GetColors(uCount, (WICColor *)&lpbmi->bmiColors, (UINT *)&lpbmi->bmiHeader.biClrUsed);
						}
						SafeRelease(&pPalette);
						hBM = CreateDIBSection(NULL, lpbmi, DIB_RGB_COLORS, (void **)&lpBits, NULL, 0);
						if (hBM) {
							try {
								if (SetDIBits(NULL, hBM, 0, h, pbData, lpbmi, DIB_RGB_COLORS) && bFixAlpha && lpBits) {
									int r0 = GetRValue(clBk);
									int g0 = GetGValue(clBk);
									int b0 = GetBValue(clBk);
									for (int y = h; y--;) {
										RGBQUAD *pcl = &lpBits[y * w];
										for (int x = w; x--; pcl++) {
											int a = pcl->rgbReserved;
											pcl->rgbBlue = (pcl->rgbBlue - b0) * a / 0xff + b0;
											pcl->rgbGreen = (pcl->rgbGreen - g0) * a / 0xff + g0;
											pcl->rgbRed = (pcl->rgbRed - r0) * a / 0xff + r0;
											if (clBk) {
												pcl->rgbReserved = 0xff;
											}
										}
									}
								}
							} catch (...) {
								g_nException = 0;
#ifdef _DEBUG
								g_strException = L"GetHBITMAP";
#endif
							}
						}
					}
					::SysFreeString((BSTR)lpbmi);
				}
			}
			SafeRelease(&pBitmapLock);
		}
	}
	return hBM;
}

BOOL CteWICBitmap::Get(WICPixelFormatGUID guidNewPF)
{
	WICPixelFormatGUID guidPF;
	m_pImage->GetPixelFormat(&guidPF);
	if (IsEqualGUID(guidPF, guidNewPF)) {
		return TRUE;
	}
	BOOL b = FALSE;
	IWICFormatConverter *pConverter;
	if SUCCEEDED(g_pWICFactory->CreateFormatConverter(&pConverter)) {
		if SUCCEEDED(pConverter->Initialize(m_pImage, guidNewPF, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeMedianCut)) {
			pConverter->GetPixelFormat(&guidPF);
			if (IsEqualGUID(guidPF, guidNewPF)) {
				ClearImage(FALSE);
				b = SUCCEEDED(g_pWICFactory->CreateBitmapFromSource(pConverter, WICBitmapCacheOnDemand, &m_pImage));
			}
			SafeRelease(&pConverter);
		}
	}
	return b;
}

HRESULT CteWICBitmap::CreateStream(IStream *pStream, ULARGE_INTEGER *puliSize, CLSID encoderClsid, LONG lQuality)
{
	HRESULT hr = E_FAIL;
	if (pStream) {
		IWICBitmapEncoder *pEncoder;
		if SUCCEEDED(g_pWICFactory->CreateEncoder(encoderClsid, NULL, &pEncoder)) {
			if (lQuality == 0 && m_pStream && IsEqualGUID(encoderClsid, m_guidSrc)) {
				teCopyStream(m_pStream, pStream);
				hr = S_OK;
			} else {
				UINT w = 0, h = 0;
				m_pImage->GetSize(&w, &h);
				WICPixelFormatGUID guidPF;
				m_pImage->GetPixelFormat(&guidPF);
				pEncoder->Initialize(pStream, WICBitmapEncoderNoCache);
				IWICBitmapFrameEncode *pFrameEncode;
				IPropertyBag2 *pPropBag;
				hr = pEncoder->CreateNewFrame(&pFrameEncode, &pPropBag);
				if SUCCEEDED(hr) {
					if (lQuality) {
						PROPBAG2 option = { 0 };
						option.pstrName = L"ImageQuality";
						VARIANT v;
						v.vt = VT_R4;
						v.fltVal = FLOAT(lQuality) / 100;
						pPropBag->Write(1, &option, &v);
					}
					hr = pFrameEncode->Initialize(pPropBag);
					if SUCCEEDED(hr) {
						pFrameEncode->SetSize(w, h);
						pFrameEncode->SetPixelFormat(&guidPF);
						IWICPalette *pPalette = NULL;
						if SUCCEEDED(g_pWICFactory->CreatePalette(&pPalette)) {
							if SUCCEEDED(m_pImage->CopyPalette(pPalette)) {
								pFrameEncode->SetPalette(pPalette);
							}
							SafeRelease(&pPalette);
						}
						hr = pFrameEncode->WriteSource(m_pImage, NULL);
						if SUCCEEDED(hr) {
							hr = pFrameEncode->Commit();
						}
						hr = pEncoder->Commit();
					}
					SafeRelease(&pPropBag);
					SafeRelease(&pFrameEncode);
				}
			}
			if SUCCEEDED(hr) {
				LARGE_INTEGER liOffset;
				liOffset.QuadPart = 0;
				pStream->Seek(liOffset, STREAM_SEEK_END, puliSize);
				pStream->Seek(liOffset, STREAM_SEEK_SET, NULL);
			}
			SafeRelease(&pEncoder);
		}
	}
	return hr;
}

/*
HRESULT CteWICBitmap::CreateBMPStream(IStream *pStream, ULARGE_INTEGER *puliSize, LPWSTR szMime)
{
	HRESULT hr = E_FAIL;
	if (pStream) {
		HBITMAP hBM = GetHBITMAP(GetSysColor(COLOR_BTNFACE));
		if (hBM) {
			BITMAPFILEHEADER bf;
			BITMAPINFO bi;
			BITMAP bm;
			GetObject(hBM, sizeof(BITMAP), &bm);
			ZeroMemory(&bi.bmiHeader, sizeof(BITMAPINFOHEADER));
			bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bi.bmiHeader.biWidth = bm.bmWidth;
			bi.bmiHeader.biHeight = bm.bmHeight;
			bi.bmiHeader.biPlanes = 1;
			bi.bmiHeader.biBitCount = bm.bmBitsPixel;
			bi.bmiHeader.biCompression = BI_RGB;
			bi.bmiHeader.biSizeImage = bm.bmWidthBytes * bm.bmHeight;
			bf.bfType = 0x4d42;
			bf.bfReserved1 = 0;
			bf.bfReserved2 = 0;
			DWORD cb = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
			bf.bfOffBits = cb;
			bf.bfSize = cb + bi.bmiHeader.biSizeImage;
			bf.bfType = 0x4d42;
			bf.bfReserved1 = 0;
			bf.bfReserved2 = 0;

			LPVOID pBits = HeapAlloc(GetProcessHeap(), 0, bi.bmiHeader.biSizeImage);
			if (pBits) {
				HDC hdc = GetDC(NULL);
				if (hdc) {
					if (GetDIBits(hdc, hBM, 0, bm.bmHeight, pBits, (BITMAPINFO*)&bi, DIB_RGB_COLORS)) {
						LARGE_INTEGER liOffset;
						liOffset.QuadPart = 0;
						pStream->Seek(liOffset, STREAM_SEEK_SET, NULL);
						hr = pStream->Write(&bf, sizeof(BITMAPFILEHEADER), NULL);
						if SUCCEEDED(hr) {
							hr = pStream->Write(&bi, sizeof(BITMAPINFOHEADER), NULL);
						}
						if SUCCEEDED(hr) {
							hr = pStream->Write(pBits, bi.bmiHeader.biSizeImage, NULL);
						}
						if SUCCEEDED(hr) {
							lstrcpy(szMime, L"image/bmp");
							pStream->Seek(liOffset, STREAM_SEEK_END, puliSize);
							pStream->Seek(liOffset, STREAM_SEEK_SET, NULL);
						}
					}
					ReleaseDC(NULL, hdc);
				}
				HeapFree(GetProcessHeap(), 0, pBits);
			}
			DeleteObject(hBM);
		}
	}
	return hr;
}
*/
BOOL CteWICBitmap::HasImage()
{
	if (m_pImage) {
		UINT w = 0, h = 0;
		m_pImage->GetSize(&w, &h);
		return w != 0;
	}
	return FALSE;
}

CteWICBitmap* CteWICBitmap::GetBitmapObj()
{
	return HasImage() ? this : NULL;
}

VOID CteWICBitmap::GetFrameFromStream(IStream *pStream, UINT uFrame, BOOL bInit)
{
	LARGE_INTEGER liOffset;
	liOffset.QuadPart = 0;
	pStream->Seek(liOffset, SEEK_SET, NULL);
	IWICBitmapDecoder *pDecoder;
	if SUCCEEDED(g_pWICFactory->CreateDecoderFromStream(pStream, NULL, WICDecodeMetadataCacheOnDemand, &pDecoder)) {
		SafeRelease(&m_ppMetadataQueryReader[0]);
		pDecoder->GetMetadataQueryReader(&m_ppMetadataQueryReader[0]);
		IWICBitmapFrameDecode *pFrameDecode;
		pDecoder->GetFrameCount(&m_uFrameCount);
		if (m_uFrameCount) {
			if SUCCEEDED(pDecoder->GetFrame(uFrame, &pFrameDecode)) {
				ClearImage(FALSE);
				g_pWICFactory->CreateBitmapFromSource(pFrameDecode, WICBitmapCacheOnDemand, &m_pImage);
				SafeRelease(&m_ppMetadataQueryReader[1]);
				pFrameDecode->GetMetadataQueryReader(&m_ppMetadataQueryReader[1]);
				SafeRelease(&pFrameDecode);
			}
			if (m_uFrameCount > 1 && bInit) {
				if (m_pStream = SHCreateMemStream(NULL, NULL)) {
					teCopyStream(pStream, m_pStream);
					pDecoder->GetContainerFormat(&m_guidSrc);
				}
			}
		}
		SafeRelease(&pDecoder);
	}
}

VOID CteWICBitmap::FromStreamRelease(IStream *pStream, LPWSTR lpfn, BOOL bExtend, VARIANT *pvCX)
{
	SafeRelease(&m_pStream);
	m_uFrameCount = 1;
	GetFrameFromStream(pStream, 0, TRUE);
	if (bExtend && !HasImage()) {
		if (g_pOnFunc[TE_OnFromStream]) {
			try {
				if (InterlockedIncrement(&g_nProcIMG) < 8) {
					VARIANTARG *pv = GetNewVARIANT(4);
					teSetObject(&pv[3], this);
					CteObject *poStrm = new CteObject(pStream);
					teSetObject(&pv[2], poStrm);
					teSetSZ(&pv[1], lpfn);
					if (pvCX) {
						::VariantCopy(&pv[0], pvCX);
					}
					Invoke4(g_pOnFunc[TE_OnFromStream], NULL, 4, pv);
					poStrm->Clear();
					SafeRelease(&poStrm);
				}
			} catch(...) {
				g_nException = 0;
#ifdef _DEBUG
				g_strException = L"FromStream";
#endif
			}
			::InterlockedDecrement(&g_nProcIMG);
		}
	}
	SafeRelease(&pStream);
}

STDMETHODIMP CteWICBitmap::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	try {
		HRESULT hr = E_FAIL;
		int nArg = pDispParams ? pDispParams->cArgs - 1 : -1;
		if (pVarResult) {
			VariantInit(pVarResult);
		}
		if (!g_pWICFactory) {
			return S_OK;
		}
		if (dispIdMember < 100) {
			ClearImage(TRUE);
		} else if (!m_pImage && dispIdMember < 900) {
			return S_OK;
		}

		switch(dispIdMember) {
			//FromHBITMAP
			case 1:
				if (nArg >= 0) {
					HPALETTE pal = (nArg >= 1) ? (HPALETTE)GetPtrFromVariant(&pDispParams->rgvarg[nArg - 1]) : 0;
					WICBitmapAlphaChannelOption nAlpha = (nArg >= 2) ? (WICBitmapAlphaChannelOption)GetIntFromVariant(&pDispParams->rgvarg[nArg - 2]) : WICBitmapIgnoreAlpha;
					IWICBitmap *pBitmap;
					if SUCCEEDED(g_pWICFactory->CreateBitmapFromHBITMAP((HBITMAP)GetPtrFromVariant(&pDispParams->rgvarg[nArg]), pal, nAlpha, &pBitmap)) {
						SafeRelease(&m_pImage);
						m_pImage = pBitmap;
					}
					teSetObject(pVarResult, GetBitmapObj());
				}
				return S_OK;
			//FromHICON
			case 2:
				if (nArg >= 0) {
					g_pWICFactory->CreateBitmapFromHICON((HICON)GetPtrFromVariant(&pDispParams->rgvarg[nArg]), &m_pImage);
					teSetObject(pVarResult, GetBitmapObj());
				}
				return S_OK;
			//FromResource
			case 3:
				if (nArg >= 1) {
					HBITMAP hBM = (HBITMAP)LoadImage((HINSTANCE)GetPtrFromVariant(&pDispParams->rgvarg[nArg]), GetLPWSTRFromVariant(&pDispParams->rgvarg[nArg - 1]), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
					if (hBM) {
						IWICBitmap *pBitmap;
						if SUCCEEDED(g_pWICFactory->CreateBitmapFromHBITMAP(hBM, 0, WICBitmapIgnoreAlpha, &pBitmap)) {
							SafeRelease(&m_pImage);
							m_pImage = pBitmap;
						}
						DeleteObject(hBM);
					}
					teSetObject(pVarResult, GetBitmapObj());
				}
				return S_OK;
			//FromFile
			case 4:
				if (nArg >= 0) {
					IStream *pStream;
					LPWSTR lpfn = GetLPWSTRFromVariant(&pDispParams->rgvarg[nArg]);
					if (teStartsText(L"data:", lpfn)) {
						LPWSTR lpBase64 = StrChr(lpfn, ',');
						if (lpBase64) {
							DWORD dwData;
							DWORD dwLen = lstrlen(++lpBase64);
							if (CryptStringToBinary(lpBase64, dwLen, CRYPT_STRING_BASE64, NULL, &dwData, NULL, NULL) && dwData > 0) {
								BSTR bs = ::SysAllocStringByteLen(NULL, dwData);
								CryptStringToBinary(lpBase64, dwLen, CRYPT_STRING_BASE64, (BYTE *)bs, &dwData, NULL, NULL);
								IStream *pStream = SHCreateMemStream(NULL, NULL);
								pStream->Write(bs, dwData, NULL);
								::SysFreeString(bs);
								FromStreamRelease(pStream, lpfn, FALSE, nArg >= 1 ? &pDispParams->rgvarg[nArg - 1] : NULL);
							}
						}
					}
					if (!HasImage()) {
						if SUCCEEDED(SHCreateStreamOnFileEx(lpfn, STGM_READ | STGM_SHARE_DENY_NONE, FILE_ATTRIBUTE_NORMAL, FALSE, NULL, &pStream)) {
							FromStreamRelease(pStream, lpfn, FALSE, nArg >= 1 ? &pDispParams->rgvarg[nArg - 1] : NULL);
						}
						if (!HasImage()) {
							if (g_pOnFunc[TE_OnFromFile]) {
								try {
									if (InterlockedIncrement(&g_nProcIMG) < 8) {
										VARIANTARG *pv = GetNewVARIANT(3);
										teSetObject(&pv[2], this);
										VariantCopy(&pv[1], &pDispParams->rgvarg[nArg]);
										if (nArg >= 1) {
											VariantCopy(&pv[0], &pDispParams->rgvarg[nArg - 1]);
										}
										Invoke4(g_pOnFunc[TE_OnFromFile], NULL, 3, pv);
									}
								} catch(...) {
									g_nException = 0;
#ifdef _DEBUG
									g_strException = L"FromFile";
#endif
								}
								::InterlockedDecrement(&g_nProcIMG);
							}
						}
					}
					teSetObject(pVarResult, GetBitmapObj());
				}
				return S_OK;
			//FromStream
			case 5:
				if (nArg >= 0) {
					LPWSTR lpfn = nArg ? GetLPWSTRFromVariant(&pDispParams->rgvarg[nArg - 1]) : NULL;
					IUnknown *punk;
					if (FindUnknown(&pDispParams->rgvarg[nArg], &punk)) {
						IStream *pStream;
						if SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pStream))) {
							FromStreamRelease(pStream, lpfn, TRUE, nArg >= 2 ? &pDispParams->rgvarg[nArg - 2] : NULL);
						}
					}
					teSetObject(pVarResult, GetBitmapObj());
				}
				return S_OK;
			//FromArchive
			case 6:
				if (nArg >= 3) {
					IStorage *pStorage = NULL;
					HMODULE hDll;
					HRESULT hr = teInitStorage(&pDispParams->rgvarg[nArg], &pDispParams->rgvarg[nArg - 1], GetLPWSTRFromVariant(&pDispParams->rgvarg[nArg - 2]), &hDll, &pStorage);
					if SUCCEEDED(hr) {
						VARIANT v;
						teVariantChangeType(&v, &pDispParams->rgvarg[nArg - 3], VT_BSTR);
						LPWSTR lpPath;
						LPWSTR lpPath1 = v.bstrVal;
						while (lpPath = StrChr(lpPath1, '\\')) {
							lpPath[0] = NULL;
							IStorage *pStorageNew;
							pStorage->OpenStorage(lpPath1, NULL, STGM_READ, 0, 0, &pStorageNew);
							if (pStorageNew) {
								pStorage->Release();
								pStorage = pStorageNew;
							} else {
								break;
							}
							lpPath1 = lpPath + 1;
						}
						IStream *pStream;
						hr = pStorage->OpenStream(lpPath1, NULL, STGM_READ, NULL, &pStream);
						if SUCCEEDED(hr) {
							FromStreamRelease(pStream, lpPath1, TRUE, nArg >= 4 ? &pDispParams->rgvarg[nArg - 4] : NULL);
						}
						VariantClear(&v);
					}
					SafeRelease(&pStorage);
					teFreeLibrary(hDll, 0);
					teSetObject(pVarResult, GetBitmapObj());
				}
				return S_OK;
			//FromItem
			case 7:
				if (nArg >= 0) {
					LPITEMIDLIST pidl;
					if (teGetIDListFromVariant(&pidl, &pDispParams->rgvarg[nArg])) {
						SIZE size;
						int cx = nArg >= 1 ? GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]) : 0;
						size.cx = cx ? cx : 0x1000;
						IShellFolder *pSF;
						LPCITEMIDLIST pidlPart;
						if SUCCEEDED(SHBindToParent(pidl, IID_PPV_ARGS(&pSF), &pidlPart)) {
							HBITMAP hBM = NULL;
							WTS_ALPHATYPE alphaType = WTSAT_ARGB;
							IExtractImage *pEI;
							IThumbnailProvider *pTP;
							if (pSF->GetUIObjectOf(NULL, 1, &pidlPart, IID_IThumbnailProvider, NULL, (void **)&pTP) == S_OK) {
								pTP->GetThumbnail(size.cx, &hBM, &alphaType);
								pTP->Release();
							} else if (pSF->GetUIObjectOf(NULL, 1, &pidlPart, IID_IExtractImage, NULL, (void **)&pEI) == S_OK) {
								alphaType = WTSAT_RGB;
								size.cy = size.cx;
								DWORD dwFlags = cx ? IEIFLAG_SCREEN : IEIFLAG_ASPECT | IEIFLAG_ORIGSIZE | IEIFLAG_QUALITY;
								WCHAR pszPath[MAX_PATH];
								HRESULT hr = pEI->GetLocation(pszPath, MAX_PATH, NULL, &size, 24, &dwFlags);
								//Fix for Acrobat Reader DC
								if (FAILED(hr) && size.cx > 512) {
									size.cx = 512;
									size.cy = size.cx;
									pEI->GetLocation(pszPath, MAX_PATH, NULL, &size, 24, &dwFlags);
								}
								pEI->Extract(&hBM);
								pEI->Release();

							}
							if (hBM) {
								SafeRelease(&m_pImage);
								g_pWICFactory->CreateBitmapFromHBITMAP(hBM, 0, alphaType == WTSAT_ARGB ? WICBitmapUseAlpha : WICBitmapIgnoreAlpha, &m_pImage);
								DeleteObject(hBM);
							}
							pSF->Release();
						}
						teCoTaskMemFree(pidl);
					}
					teSetObject(pVarResult, GetBitmapObj());
				}
				return S_OK;
			//FromClipboard
			case 8:
				if (IsClipboardFormatAvailable(CF_BITMAP)) {
					::OpenClipboard(NULL);
					HBITMAP hBM = (HBITMAP)::GetClipboardData(CF_BITMAP);
					SafeRelease(&m_pImage);
					g_pWICFactory->CreateBitmapFromHBITMAP(hBM, 0, WICBitmapIgnoreAlpha, &m_pImage);
					::CloseClipboard();
				}
				teSetObject(pVarResult, GetBitmapObj());
				return S_OK;
			//Free
			case 99:
				return S_OK;
			//Save
			case 100:
				if (nArg >= 0) {
					VARIANT vText;
					teVariantChangeType(&vText, &pDispParams->rgvarg[nArg], VT_BSTR);
					CLSID encoderClsid;
					if (GetEncoderClsid(vText.bstrVal, &encoderClsid, NULL)) {
						LONG lQuality = nArg ? GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]) : 0;
						ULARGE_INTEGER uliSize;
						IStream *pStream;
						hr = SHCreateStreamOnFileEx(vText.bstrVal, STGM_WRITE | STGM_CREATE | STGM_SHARE_DENY_WRITE, FILE_ATTRIBUTE_NORMAL, TRUE, NULL, &pStream);
						if SUCCEEDED(hr) {
							hr = CreateStream(pStream, &uliSize, encoderClsid, lQuality);
							SafeRelease(&pStream);
						}
					}
				}
				teSetLong(pVarResult, hr);
				return S_OK;
			//Base64
			case 101:
			//DataURI
			case 102:
				if (nArg >= 0) {
					VARIANT vText;
					teVariantChangeType(&vText, &pDispParams->rgvarg[nArg], VT_BSTR);
					CLSID encoderClsid;
					WCHAR szMime[16];
					if (GetEncoderClsid(vText.bstrVal, &encoderClsid, szMime)) {
						ULARGE_INTEGER uliSize;
						IStream *pStream = SHCreateMemStream(NULL, NULL);
						hr = CreateStream(pStream, &uliSize, encoderClsid, 0);
/*						if FAILED(hr) {
							hr = CreateBMPStream(pStream, &uliSize, szMime);
						}*/
						if SUCCEEDED(hr) {
							BSTR bsBuff = SysAllocStringByteLen(NULL, uliSize.LowPart);
							if (bsBuff) {
								ULONG ulBytesRead;
								if SUCCEEDED(pStream->Read(bsBuff, uliSize.LowPart, &ulBytesRead)) {
									wchar_t szHead[32];
									szHead[0] = NULL;
									if (dispIdMember == 102) {
										swprintf_s(szHead, 32, L"data:%s;base64,", szMime);
									}
									int nDest = lstrlen(szHead);
									DWORD dwSize;
									CryptBinaryToString((BYTE *)bsBuff, ulBytesRead, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &dwSize);
									if (dwSize > 0) {
										pVarResult->vt = VT_BSTR;
										pVarResult->bstrVal = ::SysAllocStringLen(NULL, nDest + dwSize - 1);
										lstrcpy(pVarResult->bstrVal, szHead);
										CryptBinaryToString((BYTE *)bsBuff, ulBytesRead, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, &pVarResult->bstrVal[nDest], &dwSize);
									}
								}
								::SysFreeString(bsBuff);
							}
						}
						SafeRelease(&pStream);
					}
				}
				return S_OK;
			//GetWidth
			case 110:
				UINT w, h;
				w = 0;
				m_pImage->GetSize(&w, &h);
				teSetLong(pVarResult, w);
				return S_OK;
			//GetHeight
			case 111:
				h = 0;
				m_pImage->GetSize(&w, &h);
				teSetLong(pVarResult, h);
				return S_OK;
			//GetPixel
			case 112:
				if (nArg >= 1 && Get(GUID_WICPixelFormat32bppBGRA)) {
					UINT w = 0, h = 0;
					m_pImage->GetSize(&w, &h);
					IWICBitmapLock *pBitmapLock;
					WICRect rc = {0, 0, w, h};
					if SUCCEEDED(m_pImage->Lock(&rc, WICBitmapLockRead, &pBitmapLock)) {
						UINT cbSize = 0;
						LONG *pbData = NULL;
						if SUCCEEDED(pBitmapLock->GetDataPointer(&cbSize, (PBYTE *)&pbData)) {
							UINT x = GetIntFromVariant(&pDispParams->rgvarg[nArg]);
							if (x < w) {
								UINT y = GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]);
								if (y < h) {
									teSetLong(pVarResult, pbData[x + y * w]);
								}
							}
						}
						pBitmapLock->Release();
					}
				}
				return S_OK;
			//SetPixel
			case 113:
				if (nArg >= 2) {
					if (Get(GUID_WICPixelFormat32bppBGRA)) {
						UINT w = 0, h = 0;
						m_pImage->GetSize(&w, &h);
						IWICBitmapLock *pBitmapLock;
						WICRect rc = {0, 0, w, h};
						if SUCCEEDED(m_pImage->Lock(&rc, WICBitmapLockWrite, &pBitmapLock)) {
							UINT cbSize = 0;
							LONG *pbData = NULL;
							if SUCCEEDED(pBitmapLock->GetDataPointer(&cbSize, (PBYTE *)&pbData)) {
								UINT x = GetIntFromVariant(&pDispParams->rgvarg[nArg]);
								if (x < w) {
									UINT y = GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]);
									if (y < h) {
										pbData[x + y * w] = GetIntFromVariant(&pDispParams->rgvarg[nArg - 2]);
										SafeRelease(&m_pStream);
										m_uFrameCount = 1;
									}
								}
							}
							SafeRelease(&pBitmapLock);
						}
					}
				}
				return S_OK;
			//GetPixelFormat
			case 114:
				WICPixelFormatGUID guidPF;
				m_pImage->GetPixelFormat(&guidPF);
				WCHAR pszBuff[40];
				StringFromGUID2(guidPF, pszBuff, 39);
				teSetSZ(pVarResult, pszBuff);
				return S_OK;
			//GetThumbnailImage
			case 120:
				if (nArg >= 1) {
					//WIC
					UINT w = GetIntFromVariant(&pDispParams->rgvarg[nArg]);
					UINT h = GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]);
					IWICBitmapScaler *pScaler;
					if SUCCEEDED(g_pWICFactory->CreateBitmapScaler(&pScaler)) {
						int mode = nArg >= 2 ? GetIntFromVariant(&pDispParams->rgvarg[nArg - 2]) : 4;
						if (!g_bUpper10 && mode > 3) {
							mode = 3;
						}
						if SUCCEEDED(pScaler->Initialize(m_pImage, w, h, WICBitmapInterpolationMode(mode))) {
							CteWICBitmap *pGB = new CteWICBitmap();
							g_pWICFactory->CreateBitmapFromSource(pScaler, WICBitmapCacheOnDemand, &pGB->m_pImage);
							SafeRelease(&pScaler);
							teSetObjectRelease(pVarResult, pGB);
						}
					}
				}
				return S_OK;
			//RotateFlip
			case 130:
				if (nArg >= 0) {
					IWICBitmapFlipRotator *pFlipRotator;
					if SUCCEEDED(g_pWICFactory->CreateBitmapFlipRotator(&pFlipRotator)) {
						if SUCCEEDED(pFlipRotator->Initialize(m_pImage, static_cast<WICBitmapTransformOptions>(GetIntFromVariant(&pDispParams->rgvarg[nArg])))) {
							ClearImage(TRUE);
							g_pWICFactory->CreateBitmapFromSource(pFlipRotator, WICBitmapCacheOnDemand, &m_pImage);
						}
						SafeRelease(&pFlipRotator);
					}
				}
				return S_OK;
			//GetFrameCount
			case 140:
				teSetLong(pVarResult, HasImage() ? m_uFrameCount : 0);
				return S_OK;
			//Frame
			case 150:
				if (nArg >= 0 && m_pStream) {
					m_uFrame = GetIntFromVariant(&pDispParams->rgvarg[nArg]);
					GetFrameFromStream(m_pStream, m_uFrame, FALSE);
				}
				if (pVarResult && HasImage()) {
					teSetLong(pVarResult, m_uFrame);
				}
				return S_OK;
			//GetMetadata
			case 160:
			//GetFrameMetadata
			case 161:
				if (nArg >= 0 && pVarResult) {
					if (m_ppMetadataQueryReader[dispIdMember - 160]) {
						PROPVARIANT propVar;
						PropVariantInit(&propVar);
						if SUCCEEDED(m_ppMetadataQueryReader[dispIdMember - 160]->GetMetadataByName(GetLPWSTRFromVariant(&pDispParams->rgvarg[nArg]), &propVar)) {
#ifdef _2000XP
							if (lpfnPropVariantToVariant) {
								lpfnPropVariantToVariant(&propVar, pVarResult);
							} else {
								VariantCopy(pVarResult, (VARIANT *)&propVar);
							}
#else
							PropVariantToVariant(&propVar, pVarResult);
#endif
							PropVariantClear(&propVar);
						}
					}
				}
				return S_OK;
			//GetHBITMAP
			case 210:
				if (pVarResult) {
					HBITMAP hBM;
					hBM = GetHBITMAP(nArg >= 0 ? GetIntFromVariant(&pDispParams->rgvarg[nArg]) : 0);
					teSetPtr(pVarResult, hBM);
				}
				return S_OK;
			//GetHICON
			case 211:
			//DrawEx
			case 212:
				HBITMAP hBM;
				if (hBM = GetHBITMAP(-1)) {
					BITMAP bm;
					GetObject(hBM, sizeof(BITMAP), &bm);
					HIMAGELIST himl = ImageList_Create(bm.bmWidth, bm.bmHeight, ILC_COLOR32, 0, 0);
					ImageList_Add(himl, hBM, NULL);
					DeleteObject(hBM);
					if (dispIdMember == 211) {
						teSetPtr(pVarResult, ImageList_GetIcon(himl, 0, nArg >= 0 ? GetIntFromVariant(&pDispParams->rgvarg[nArg]) : ILD_NORMAL));
					} else if (dispIdMember == 212 && nArg >= 7) {
						teSetBool(pVarResult, ImageList_DrawEx(himl, 0, (HDC)GetPtrFromVariant(&pDispParams->rgvarg[nArg]),
							GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]), GetIntFromVariant(&pDispParams->rgvarg[nArg - 2]),
							GetIntFromVariant(&pDispParams->rgvarg[nArg - 3]), GetIntFromVariant(&pDispParams->rgvarg[nArg - 4]),
							GetIntFromVariant(&pDispParams->rgvarg[nArg - 5]), GetIntFromVariant(&pDispParams->rgvarg[nArg - 6]),
							GetIntFromVariant(&pDispParams->rgvarg[nArg - 7])
						));
					}
					ImageList_Destroy(himl);
				}
				return S_OK;
			//GetCodecInfo
			case 900:
				IDispatch *pdisp;
				if (nArg >= 2 && GetDispatch(&pDispParams->rgvarg[nArg - 2], &pdisp)) {
					IEnumUnknown *pEnum;
					HRESULT hr = g_pWICFactory->CreateComponentEnumerator(WICComponentType(GetIntFromVariant(&pDispParams->rgvarg[nArg])), WICComponentEnumerateDefault, &pEnum);
					if SUCCEEDED(hr) {
						int nMode = GetIntFromVariant(&pDispParams->rgvarg[nArg - 1]);
						ULONG cbActual = 0;
						IUnknown *punk;
						hr = E_FAIL;
						while(pEnum->Next(1, &punk, &cbActual) == S_OK && hr != S_OK) {
							IWICBitmapCodecInfo *pCodecInfo;
							if SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pCodecInfo))) {
								UINT uActual = 0;
								teGetCodecInfo(pCodecInfo, nMode, 0, NULL, &uActual);
								if (uActual > 1) {
									VARIANT *pv = GetNewVARIANT(1);
									pv->vt = VT_BSTR;
									pv->bstrVal = ::SysAllocStringLen(NULL, uActual - 1);
									teGetCodecInfo(pCodecInfo, nMode, uActual, pv->bstrVal, &uActual);
									teExecMethod(pdisp, L"push", NULL, 1, pv);
								}
								pCodecInfo->Release();
							}
							punk->Release();
						}
						pEnum->Release();
					}
					pdisp->Release();
				}
				return S_OK;
			case DISPID_VALUE:
				teSetObject(pVarResult, this);
				return S_OK;
		}
	} catch (...) {
		return teException(pExcepInfo, "WICBitmap", methodGB, dispIdMember);
	}
	return DISP_E_MEMBERNOTFOUND;
}

#ifdef _USE_BSEARCHAPI
//CteWindowsAPI
CteWindowsAPI::CteWindowsAPI(TEDispatchApi *pApi)
{
	m_cRef = 1;
	m_pApi = pApi;
}

CteWindowsAPI::~CteWindowsAPI()
{
}

STDMETHODIMP CteWindowsAPI::QueryInterface(REFIID riid, void **ppvObject)
{
	static const QITAB qit[] =
	{
		QITABENT(CteWindowsAPI, IDispatch),
		{ 0 },
	};
	return QISearch(this, qit, riid, ppvObject);
}

STDMETHODIMP_(ULONG) CteWindowsAPI::AddRef()
{
	return ::InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CteWindowsAPI::Release()
{
	if (::InterlockedDecrement(&m_cRef) == 0) {
		delete this;
		return 0;
	}

	return m_cRef;
}

STDMETHODIMP CteWindowsAPI::GetTypeInfoCount(UINT *pctinfo)
{
	*pctinfo = 0;
	return S_OK;
}

STDMETHODIMP CteWindowsAPI::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteWindowsAPI::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	int nIndex = teBSearchApi(dispAPI, _countof(dispAPI), g_maps[MAP_API], *rgszNames);
	if (nIndex >= 0) {
		*rgDispId = nIndex + 0x60010000;
		return S_OK;
	}
	if (lstrcmpi(*rgszNames, L"ADBSTRM") == 0) {
		*rgDispId = 0x40010000;
		return S_OK;
	}
	return DISP_E_UNKNOWNNAME;
}

STDMETHODIMP CteWindowsAPI::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	if (wFlags & DISPATCH_METHOD) {
		TEDispatchApi *pApi = (dispIdMember >= 0x60010000 && dispIdMember < _countof(dispAPI) + 0x60010000) ? &dispAPI[dispIdMember - 0x60010000] : m_pApi;
		if (pApi) {
			try {
				return teInvokeAPI(pApi, pDispParams, pVarResult, pExcepInfo);
			} catch (...) {
				return teExceptionEx(pExcepInfo, "WindowsAPI", pApi->name);
			}
		}
	} else if (dispIdMember == DISPID_VALUE) {
		teSetObject(pVarResult, this);
		return S_OK;
	} else if (dispIdMember >= 0x60010000 && dispIdMember < _countof(dispAPI) + 0x60010000) {
		teSetObjectRelease(pVarResult, new CteWindowsAPI(&dispAPI[dispIdMember - 0x60010000]));
		return S_OK;
	} else if (dispIdMember == 0x40010000) {
		teSetSZ(pVarResult, L"ADODB.Stream");
		return S_OK;
	}
	return DISP_E_MEMBERNOTFOUND;
}
#endif
//CteDispatch

CteDispatch::CteDispatch(IDispatch *pDispatch, int nMode)
{
	m_cRef = 1;
	m_pDispatch = pDispatch;
	m_pDispatch->AddRef();
	m_nMode = nMode;
	m_dispIdMember = DISPID_UNKNOWN;
	m_nIndex = 0;
	m_pActiveScript = NULL;
	m_pDll = NULL;
}

CteDispatch::~CteDispatch()
{
	Clear();
}

VOID CteDispatch::Clear()
{
	SafeRelease(&m_pDispatch);
	if (m_pActiveScript) {
		m_pActiveScript->SetScriptState(SCRIPTSTATE_CLOSED);
		m_pActiveScript->Close();
		SafeRelease(&m_pActiveScript);
	}
	SafeRelease(&m_pDll);
}


STDMETHODIMP CteDispatch::QueryInterface(REFIID riid, void **ppvObject)
{
	static const QITAB qit[] =
	{
		QITABENT(CteDispatch, IDispatch),
		QITABENT(CteDispatch, IEnumVARIANT),
		{ 0 },
	};
	return QISearch(this, qit, riid, ppvObject);
}

STDMETHODIMP_(ULONG) CteDispatch::AddRef()
{
	return ::InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CteDispatch::Release()
{
	if (::InterlockedDecrement(&m_cRef) == 0) {
		delete this;
		return 0;
	}

	return m_cRef;
}

STDMETHODIMP CteDispatch::GetTypeInfoCount(UINT *pctinfo)
{
	*pctinfo = 0;
	return S_OK;
}

STDMETHODIMP CteDispatch::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteDispatch::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	if (m_nMode && m_pDispatch) {
		return m_pDispatch->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgDispId);
	}
	return DISP_E_UNKNOWNNAME;
}

STDMETHODIMP CteDispatch::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	try {
		if (pVarResult) {
			VariantInit(pVarResult);
		}
		if (m_nMode & 3) {
			if (dispIdMember == DISPID_NEWENUM) {
				teSetObject(pVarResult, this);
				return S_OK;
			}
			if (dispIdMember == DISPID_VALUE) {
				int nArg = pDispParams ? pDispParams->cArgs - 1 : -1;
				if (nArg >= 0) {
					VARIANTARG *pv = GetNewVARIANT(1);
					teVariantChangeType(pv, &pDispParams->rgvarg[nArg], VT_BSTR);
					m_pDispatch->GetIDsOfNames(IID_NULL, &pv[0].bstrVal, 1, LOCALE_USER_DEFAULT, &dispIdMember);
					VariantClear(&pv[0]);
					if (nArg >= 1) {
						VariantCopy(&pv[0], &pDispParams->rgvarg[nArg - 1]);
						Invoke5(m_pDispatch, dispIdMember, DISPATCH_PROPERTYPUT, NULL, 1, pv);
					} else {
						delete [] pv;
					}
					if (pVarResult) {
						Invoke5(m_pDispatch, dispIdMember, DISPATCH_PROPERTYGET, pVarResult, 0, NULL);
					}
					return S_OK;
				}
				teSetObject(pVarResult, this);
				return S_OK;
			}
			return m_pDispatch->Invoke(dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
		}
		if (wFlags & DISPATCH_PROPERTYGET && dispIdMember == DISPID_VALUE) {
			teSetObject(pVarResult, this);
			return S_OK;
		}
		if (m_pDispatch) {
			return m_pDispatch->Invoke(m_nMode ? dispIdMember : m_dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
		}
	} catch (...) {
		return teException(pExcepInfo, "Dispatch", NULL, dispIdMember);
	}
	return DISP_E_MEMBERNOTFOUND;
}

STDMETHODIMP CteDispatch::Next(ULONG celt, VARIANT *rgVar, ULONG *pCeltFetched)
{
	if (rgVar) {
		if (m_nMode) {
			IDispatchEx *pdex = NULL;
			if SUCCEEDED(m_pDispatch->QueryInterface(IID_PPV_ARGS(&pdex))) {
				HRESULT hr;
				hr = pdex->GetNextDispID(0, m_dispIdMember, &m_dispIdMember);
				if (hr == S_OK) {
					hr = pdex->GetMemberName(m_dispIdMember, &rgVar->bstrVal);
					rgVar->vt = VT_BSTR;
				}
				pdex->Release();
				return hr;
			}
		}
		int nCount = 0;
		VARIANT v;
		if (Invoke5(m_pDispatch, DISPID_TE_COUNT, DISPATCH_PROPERTYGET, &v, 0, NULL) == S_OK) {
			nCount = GetIntFromVariantClear(&v);
		}
		if (m_nIndex < nCount) {
			if (pCeltFetched) {
				*pCeltFetched = 1;
			}
			VARIANTARG *pv = GetNewVARIANT(1);
			teSetLong(&pv[0], m_nIndex++);
			return Invoke5(m_pDispatch, DISPID_TE_ITEM, DISPATCH_METHOD, rgVar, 1, pv);
		}
	}
	return S_FALSE;
}

STDMETHODIMP CteDispatch::Skip(ULONG celt)
{
	m_nIndex += celt;
	return S_OK;
}

STDMETHODIMP CteDispatch::Reset(void)
{
	m_nIndex = 0;
	return S_OK;
}

STDMETHODIMP CteDispatch::Clone(IEnumVARIANT **ppEnum)
{
	if (ppEnum) {
		CteDispatch *pdisp = new CteDispatch(m_pDispatch, m_nMode);
		pdisp->m_dispIdMember = m_dispIdMember;
		pdisp->m_nMode = m_nMode;
		*ppEnum = static_cast<IEnumVARIANT *>(pdisp);
		return S_OK;
	}
	return E_POINTER;
}

//CteActiveScriptSite
CteActiveScriptSite::CteActiveScriptSite(IUnknown *punk, EXCEPINFO *pExcepInfo, HRESULT *phr)
{
	m_cRef = 1;
	m_phr = phr;
	m_pDispatchEx = NULL;
	if (punk) {
		punk->QueryInterface(IID_PPV_ARGS(&m_pDispatchEx));
	}
	m_pExcepInfo = pExcepInfo;
	if (!pExcepInfo) {
		m_pExcepInfo = &g_ExcepInfo;
	}
}

CteActiveScriptSite::~CteActiveScriptSite()
{
	SafeRelease(&m_pDispatchEx);
}

STDMETHODIMP CteActiveScriptSite::QueryInterface(REFIID riid, void **ppvObject)
{
	static const QITAB qit[] =
	{
		QITABENT(CteActiveScriptSite, IActiveScriptSite),
		QITABENT(CteActiveScriptSite, IActiveScriptSiteWindow),
		{ 0 },
	};
	return QISearch(this, qit, riid, ppvObject);
}

STDMETHODIMP_(ULONG) CteActiveScriptSite::AddRef()
{
	return ::InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CteActiveScriptSite::Release()
{
	if (::InterlockedDecrement(&m_cRef) == 0) {
		delete this;
		return 0;
	}

	return m_cRef;
}

STDMETHODIMP CteActiveScriptSite::GetLCID(LCID *plcid)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteActiveScriptSite::GetItemInfo(LPCOLESTR pstrName,
                   DWORD dwReturnMask,
                   IUnknown **ppiunkItem,
                   ITypeInfo **ppti)
{
	HRESULT hr = TYPE_E_ELEMENTNOTFOUND;
	if (dwReturnMask & SCRIPTINFO_IUNKNOWN) {
		if (m_pDispatchEx) {
			VARIANT v;
			VariantInit(&v);
			teGetProperty(m_pDispatchEx, const_cast<LPOLESTR>(pstrName), &v);
			if (FindUnknown(&v, ppiunkItem)) {
				return S_OK;
			}
			VariantClear(&v);
		}
		if (g_dwMainThreadId == GetCurrentThreadId()) {
			if (g_pWebBrowser && lstrcmpi(pstrName, L"window") == 0) {
				teGetHTMLWindow(g_pWebBrowser->m_pWebBrowser, IID_PPV_ARGS(ppiunkItem));
				return S_OK;
			}
#ifdef _USE_BSEARCHAPI
		} else if (lstrcmpi(pstrName, L"api") == 0) {
			*ppiunkItem = new CteWindowsAPI(NULL);
			return S_OK;
#endif
		}
	}
	return hr;
}

STDMETHODIMP CteActiveScriptSite::GetDocVersionString(BSTR *pbstrVersion)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteActiveScriptSite::OnScriptError(IActiveScriptError *pscripterror)
{
	if (!pscripterror) {
		return E_POINTER;
	}
	if (m_pExcepInfo && (SUCCEEDED(pscripterror->GetExceptionInfo(m_pExcepInfo)))) {
		BSTR bsSrcText;
		if SUCCEEDED(pscripterror->GetSourceLineText(&bsSrcText)) {
			DWORD dw1;
			ULONG ulLine;
			LONG ulColumn;
			pscripterror->GetSourcePosition(&dw1, &ulLine, &ulColumn);
			WCHAR pszLine[32];
			swprintf_s(pszLine, 32, L"\nLine: %d\n", ulLine);
			BSTR bs = ::SysAllocStringLen(NULL, ::SysStringLen(m_pExcepInfo->bstrDescription) + ::SysStringLen(bsSrcText) + lstrlen(pszLine));
			lstrcpy(bs, m_pExcepInfo->bstrDescription);
			lstrcat(bs, pszLine);
			lstrcat(bs, bsSrcText);
			::SysFreeString(bsSrcText);
			teSysFreeString(&m_pExcepInfo->bstrDescription);
			m_pExcepInfo->bstrDescription = bs;
			if (m_pExcepInfo == &g_ExcepInfo) {
				MessageBox(NULL, bs, _T(PRODUCTNAME), MB_OK | MB_ICONERROR);
				teSysFreeString(&m_pExcepInfo->bstrDescription);
			}
		}
		*m_phr = DISP_E_EXCEPTION;
	}
	return S_OK;
}

STDMETHODIMP CteActiveScriptSite::OnStateChange(SCRIPTSTATE ssScriptState)
{
	return S_OK;
}

STDMETHODIMP CteActiveScriptSite::OnScriptTerminate(const VARIANT *pvarResult,const EXCEPINFO *pexcepinfo)
{
	return S_OK;
}

STDMETHODIMP CteActiveScriptSite::OnEnterScript(void)
{
	return S_OK;
}

STDMETHODIMP CteActiveScriptSite::OnLeaveScript(void)
{
	return S_OK;
}

STDMETHODIMP CteActiveScriptSite::GetWindow(HWND *phwnd)
{
	*phwnd = g_hwndMain;
	return S_OK;
}

STDMETHODIMP CteActiveScriptSite::EnableModeless(BOOL fEnable)
{
	return E_NOTIMPL;
}

CteDll::CteDll(HMODULE hDll)
{
	m_hDll = hDll;
	m_cRef = 1;
}

CteDll::~CteDll()
{
	teFreeLibrary(m_hDll, 100);
}

STDMETHODIMP CteDll::QueryInterface(REFIID riid, void **ppvObject)
{
	static const QITAB qit[] =
	{
		QITABENT(CteDll, IUnknown),
		{ 0 },
	};
	return QISearch(this, qit, riid, ppvObject);
}

STDMETHODIMP_(ULONG) CteDll::AddRef()
{
	return ::InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CteDll::Release()
{
	if (::InterlockedDecrement(&m_cRef) == 0) {
		LPFNDllCanUnloadNow lpfnDllCanUnloadNow = (LPFNDllCanUnloadNow)GetProcAddress(m_hDll, "DllCanUnloadNow");
		if (g_pUnload && lpfnDllCanUnloadNow && lpfnDllCanUnloadNow() != S_OK) {
			if (!g_bUnload) {
				g_bUnload = TRUE;
				teArrayPush(g_pUnload, this);
				SetTimer(g_hwndMain, TET_Unload, 15 * 1000, teTimerProc);
			}
			return m_cRef;
		}
		delete this;
		return 0;
	}
	return m_cRef;
}

#ifdef _USE_OBJECTAPI
//CteAPI
CteAPI::CteAPI(TEDispatchApi *pApi)
{
	m_cRef = 1;
	m_pApi = pApi;
}

CteAPI::~CteAPI()
{
}

STDMETHODIMP CteAPI::QueryInterface(REFIID riid, void **ppvObject)
{
	static const QITAB qit[] =
	{
		QITABENT(CteAPI, IDispatch),
		{ 0 },
	};
	return QISearch(this, qit, riid, ppvObject);
}

STDMETHODIMP_(ULONG) CteAPI::AddRef()
{
	return ::InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CteAPI::Release()
{
	if (::InterlockedDecrement(&m_cRef) == 0) {
		delete this;
		return 0;
	}

	return m_cRef;
}

STDMETHODIMP CteAPI::GetTypeInfoCount(UINT *pctinfo)
{
	*pctinfo = 0;
	return S_OK;
}

STDMETHODIMP CteAPI::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteAPI::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteAPI::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	try {
		if (wFlags & DISPATCH_METHOD) {
			return teInvokeAPI(m_pApi, pDispParams, pVarResult, pExcepInfo);
		}
		if (dispIdMember == DISPID_VALUE) {
			teSetObject(pVarResult, this);
			return S_OK;
		}
	} catch (...) {
		return teExceptionEx(pExcepInfo, "WindowsAPI", m_pApi->name);
	}
	return DISP_E_MEMBERNOTFOUND;
}
#endif

///CteDispatchEx

CteDispatchEx::CteDispatchEx(IUnknown *punk)
{
	m_cRef = 1;
	punk->QueryInterface(IID_PPV_ARGS(&m_pdex));
	m_dispItem = 0;
}


CteDispatchEx::~CteDispatchEx()
{
	m_pdex->Release();
}

STDMETHODIMP CteDispatchEx::QueryInterface(REFIID riid, void **ppvObject)
{
	static const QITAB qit[] =
	{
		QITABENT(CteDispatchEx, IDispatch),
		QITABENT(CteDispatchEx, IDispatchEx),
		{ 0 },
	};
	return QISearch(this, qit, riid, ppvObject);
}

STDMETHODIMP_(ULONG) CteDispatchEx::AddRef()
{
	return ::InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CteDispatchEx::Release()
{
	if (::InterlockedDecrement(&m_cRef) == 0) {
		delete this;
		return 0;
	}

	return m_cRef;
}

STDMETHODIMP CteDispatchEx::GetTypeInfoCount(UINT *pctinfo)
{
	return m_pdex->GetTypeInfoCount(pctinfo);
}

STDMETHODIMP CteDispatchEx::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	return m_pdex->GetTypeInfo(iTInfo, lcid, ppTInfo);
}

STDMETHODIMP CteDispatchEx::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	return GetDispID(*rgszNames, fdexNameCaseSensitive, rgDispId);
}

STDMETHODIMP CteDispatchEx::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	return InvokeEx(dispIdMember, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, NULL);
}

//IDispatchEx
STDMETHODIMP CteDispatchEx::GetDispID(BSTR bstrName, DWORD grfdex, DISPID *pid)
{
	HRESULT hr = m_pdex->GetDispID(bstrName, grfdex, pid);
	if FAILED(hr) {
		if (lstrcmpi(bstrName, L"Count") == 0) {
			BSTR bs = ::SysAllocString(L"length");
			hr = m_pdex->GetDispID(bs, fdexNameCaseSensitive, pid);
			::SysFreeString(bs);
		} else if (lstrcmp(bstrName, L"Item") == 0) {
			m_dispItem = DISPID_COLLECT;
			*pid = m_dispItem;
			hr = S_OK;
		} else if (grfdex & fdexNameCaseSensitive) {
			hr = m_pdex->GetDispID(bstrName, (grfdex & fdexNameCaseSensitive) | fdexNameCaseInsensitive, pid);
		}
	}
	return hr;
}

STDMETHODIMP CteDispatchEx::InvokeEx(DISPID id, LCID lcid, WORD wFlags, DISPPARAMS *pdp, VARIANT *pvarRes, EXCEPINFO *pei, IServiceProvider *pspCaller)
{
	if (m_dispItem && id == m_dispItem && (wFlags & DISPATCH_METHOD) && pdp && pdp->cArgs > 0) {
		return teGetPropertyAt(m_pdex, GetIntFromVariant(&pdp->rgvarg[pdp->cArgs - 1]), pvarRes);
	}
	return m_pdex->InvokeEx(id, lcid, wFlags, pdp, pvarRes, pei, pspCaller);
}

STDMETHODIMP CteDispatchEx::DeleteMemberByName(BSTR bstrName, DWORD grfdex)
{
	return m_pdex->DeleteMemberByName(bstrName, grfdex);
}

STDMETHODIMP CteDispatchEx::DeleteMemberByDispID(DISPID id)
{
	return m_pdex->DeleteMemberByDispID(id);
}

STDMETHODIMP CteDispatchEx::GetMemberProperties(DISPID id, DWORD grfdexFetch, DWORD *pgrfdex)
{
	return m_pdex->GetMemberProperties(id, grfdexFetch, pgrfdex);
}

STDMETHODIMP CteDispatchEx::GetMemberName(DISPID id, BSTR *pbstrName)
{
	return m_pdex->GetMemberName(id, pbstrName);
}

STDMETHODIMP CteDispatchEx::GetNextDispID(DWORD grfdex, DISPID id, DISPID *pid)
{
	return m_pdex->GetNextDispID(grfdex, id, pid);
}

STDMETHODIMP CteDispatchEx::GetNameSpaceParent(IUnknown **ppunk)
{
	return m_pdex->GetNameSpaceParent(ppunk);
}
//*///

CteDropTarget2::CteDropTarget2(HWND hwnd, IUnknown *punk)
{
	m_cRef = 1;
	m_pDragItems = NULL;
	m_pDropTarget = NULL;

	m_hwnd = hwnd;
	m_punk = punk;
}

CteDropTarget2::~CteDropTarget2()
{
	SafeRelease(&m_pDropTarget);
}

STDMETHODIMP CteDropTarget2::QueryInterface(REFIID riid, void **ppvObject)
{
	*ppvObject = NULL;

	if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDropTarget)) {
		*ppvObject = static_cast<IDropTarget *>(this);
		AddRef();
		return S_OK;
	}
	if (m_pDropTarget) {
		return m_pDropTarget->QueryInterface(riid, ppvObject);
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CteDropTarget2::AddRef()
{
	return ::InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CteDropTarget2::Release()
{
	if (::InterlockedDecrement(&m_cRef) == 0) {
		delete this;
		return 0;
	}

	return m_cRef;
}

//IDropTarget
STDMETHODIMP CteDropTarget2::DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	g_bDragging = TRUE;
	m_DragLeave = E_NOT_SET;
	GetDragItems(&m_pDragItems, pDataObj);
	DWORD dwEffect = *pdwEffect;
	HRESULT hr = DragSub(TE_OnDragEnter, m_punk, m_pDragItems, &grfKeyState, pt, pdwEffect);
	if (m_pDropTarget) {
		*pdwEffect = dwEffect;
		if (m_pDropTarget->DragEnter(pDataObj, grfKeyState, pt, pdwEffect) == S_OK) {
			hr = S_OK;
		}
	}
	if (g_pDropTargetHelper && m_hwnd) {
		g_pDropTargetHelper->DragEnter(m_hwnd, pDataObj, (LPPOINT)&pt, *pdwEffect);
	}
	return hr;
}

STDMETHODIMP CteDropTarget2::DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	DWORD dwEffect2 = *pdwEffect;
	HRESULT hr2 = E_NOTIMPL;
	HRESULT hr = DragSub(TE_OnDragOver, m_punk, m_pDragItems, &grfKeyState, pt, pdwEffect);
	if (m_pDropTarget) {
		hr2 = m_pDropTarget->DragOver(grfKeyState, pt, &dwEffect2);
	}
	if (hr == S_OK) {
		if (*pdwEffect != dwEffect2) {
			if (g_pDropTargetHelper && m_hwnd) {
				g_pDropTargetHelper->DragOver((LPPOINT)&pt, *pdwEffect);
			}
		}
	} else {
		*pdwEffect = dwEffect2;
		hr = hr2;
	}
	m_grfKeyState = grfKeyState;
	return hr;
}

STDMETHODIMP CteDropTarget2::DragLeave()
{
	g_bDragging = FALSE;
	if (m_DragLeave == E_NOT_SET) {
		if (g_pDropTargetHelper && m_hwnd) {
			g_pDropTargetHelper->DragLeave();
		}
		m_DragLeave = DragLeaveSub(m_punk, &m_pDragItems);
		if (m_pDropTarget) {
			HRESULT hr = m_pDropTarget->DragLeave();
			if (m_DragLeave != S_OK) {
				m_DragLeave = hr;
			}
		}
	}
	return m_DragLeave;
}

STDMETHODIMP CteDropTarget2::Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	GetDragItems(&m_pDragItems, pDataObj);
	DWORD dwEffect = *pdwEffect;
	if (g_pDropTargetHelper) {
		g_pDropTargetHelper->DragLeave();
	}
	HRESULT hr = DragSub(TE_OnDrop, m_punk, m_pDragItems, &m_grfKeyState, pt, pdwEffect);
	if (m_pDropTarget) {
		if (hr != S_OK) {
			*pdwEffect = dwEffect;
			hr = m_pDropTarget->Drop(pDataObj, m_grfKeyState, pt, pdwEffect);
		}
	}
	if (g_pDropTargetHelper && m_hwnd) {
		g_pDropTargetHelper->Drop(pDataObj, (LPPOINT)&pt, *pdwEffect);
	}
	DragLeave();
	return hr;
}

#ifdef _USE_HTMLDOC
// CteDocHostUIHandler
CteDocHostUIHandler::CteDocHostUIHandler()
{
	m_cRef = 1;
}

CteDocHostUIHandler::~CteDocHostUIHandler()
{
}

STDMETHODIMP CteDocHostUIHandler::QueryInterface(REFIID riid, void **ppvObject)
{
	*ppvObject = NULL;

	if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDocHostUIHandler)) {
		*ppvObject = static_cast<IDocHostUIHandler *>(this);
	} else if (IsEqualIID(riid, IID_IServiceProvider)) {
		*ppvObject = static_cast<IServiceProvider *>(new CteServiceProvider(NULL, NULL));
		return S_OK;
	} else {
		return E_NOINTERFACE;
	}
	AddRef();

	return S_OK;
}

STDMETHODIMP_(ULONG) CteDocHostUIHandler::AddRef()
{
	return ::InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CteDocHostUIHandler::Release()
{
	if (::InterlockedDecrement(&m_cRef) == 0) {
		delete this;
		return 0;
	}

	return m_cRef;
}
//for IHTMLDocument2
//IDocHostUIHandler
STDMETHODIMP CteDocHostUIHandler::ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteDocHostUIHandler::GetHostInfo(DOCHOSTUIINFO *pInfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteDocHostUIHandler::ShowUI(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc)
{
	return S_FALSE;
}

STDMETHODIMP CteDocHostUIHandler::HideUI(VOID)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteDocHostUIHandler::UpdateUI(VOID)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteDocHostUIHandler::EnableModeless(BOOL fEnable)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteDocHostUIHandler::OnDocWindowActivate(BOOL fActivate)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteDocHostUIHandler::OnFrameWindowActivate(BOOL fActivate)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteDocHostUIHandler::ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fFrameWindow)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteDocHostUIHandler::TranslateAccelerator(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID)
{
	return S_FALSE;
}

STDMETHODIMP CteDocHostUIHandler::GetOptionKeyPath(LPOLESTR *pchKey, DWORD dw)
{
	return S_FALSE;
}

STDMETHODIMP CteDocHostUIHandler::GetDropTarget(IDropTarget *pDropTarget, IDropTarget **ppDropTarget)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteDocHostUIHandler::GetExternal(IDispatch **ppDispatch)
{
	return g_pTE->QueryInterface(IID_PPV_ARGS(ppDispatch));
}

STDMETHODIMP CteDocHostUIHandler::TranslateUrl(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut)
{
	return S_FALSE;
}

STDMETHODIMP CteDocHostUIHandler::FilterDataObject(IDataObject *pDO, IDataObject **ppDORet)
{
	return E_NOTIMPL;
}
#endif
#ifdef _USE_TESTOBJECT
//CteTest
CteTest::CteTest()
{
	m_cRef = 1;
}

CteTest::~CteTest()
{
	Sleep(1);
}

STDMETHODIMP CteTest::QueryInterface(REFIID riid, void **ppvObject)
{
	*ppvObject = NULL;

	if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDispatch)) {
		*ppvObject = static_cast<IDispatch *>(this);
	} else {
		return E_NOINTERFACE;
	}
	AddRef();

	return S_OK;
}

STDMETHODIMP_(ULONG) CteTest::AddRef()
{
	return ::InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CteTest::Release()
{
	if (::InterlockedDecrement(&m_cRef) == 0) {
		delete this;
		return 0;
	}

	return m_cRef;
}

STDMETHODIMP CteTest::GetTypeInfoCount(UINT *pctinfo)
{
	*pctinfo = 0;
	return S_OK;
}

STDMETHODIMP CteTest::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteTest::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteTest::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	switch (dispIdMember) {
		//this
		case DISPID_VALUE:
			teSetObject(pVarResult, this);
			return S_OK;
	}//end_switch
	return DISP_E_MEMBERNOTFOUND;
}
#endif

//CteProgressDialog

CteProgressDialog::CteProgressDialog(IProgressDialog *ppd)
{
	m_cRef = 1;
	m_ppd = NULL;
	if (ppd) {
		ppd->QueryInterface(IID_PPV_ARGS(&m_ppd));
	}
	if (!m_ppd) {
		teCreateInstance(CLSID_ProgressDialog, NULL, NULL, IID_PPV_ARGS(&m_ppd));
	}
}

CteProgressDialog::~CteProgressDialog()
{
	SafeRelease(&m_ppd);
}

STDMETHODIMP CteProgressDialog::QueryInterface(REFIID riid, void **ppvObject)
{
	static const QITAB qit[] =
	{
		QITABENT(CteProgressDialog, IDispatch),
		{ 0 },
	};
	return QISearch(this, qit, riid, ppvObject);
}

STDMETHODIMP_(ULONG) CteProgressDialog::AddRef()
{
	return ::InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CteProgressDialog::Release()
{
	if (::InterlockedDecrement(&m_cRef) == 0) {
		delete this;
		return 0;
	}

	return m_cRef;
}

STDMETHODIMP CteProgressDialog::GetTypeInfoCount(UINT *pctinfo)
{
	*pctinfo = 0;
	return S_OK;
}

STDMETHODIMP CteProgressDialog::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteProgressDialog::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	return teGetDispId(methodPD, 0, NULL, *rgszNames, rgDispId, TRUE);
}

STDMETHODIMP CteProgressDialog::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	try {
		LPCWSTR *pbs = NULL;
		DWORD *pdw = NULL;
		int nArg = pDispParams ? pDispParams->cArgs - 1 : -1;
		if (pVarResult) {
			VariantInit(pVarResult);
		}
		VARIANT v;
		switch(dispIdMember) {
			//HasUserCancelled
			case 0x60010001:
				teSetBool(pVarResult, m_ppd->HasUserCancelled());
				return S_OK;
			//SetCancelMsg
			case 0x60010002:
				if (nArg >= 0) {
					teVariantChangeType(&v, &pDispParams->rgvarg[nArg], VT_BSTR);
					teSetLong(pVarResult, m_ppd->SetCancelMsg(v.bstrVal, NULL));
					VariantClear(&v);
				}
				return S_OK;
			//SetLine
			case 0x60010003:
				if (nArg >= 2) {
					teVariantChangeType(&v, &pDispParams->rgvarg[nArg - 1], VT_BSTR);
					teSetLong(pVarResult, m_ppd->SetLine(GetIntFromVariant(&pDispParams->rgvarg[nArg]), v.bstrVal, GetIntFromVariant(&pDispParams->rgvarg[nArg - 2]), NULL));
					VariantClear(&v);
				}
				return S_OK;
			//SetProgress
			case 0x60010004:
				if (nArg >= 1) {
					teSetLong(pVarResult, m_ppd->SetProgress64(GetLLFromVariant(&pDispParams->rgvarg[nArg]), GetLLFromVariant(&pDispParams->rgvarg[nArg - 1])));
				}
				return S_OK;
			//SetTitle
			case 0x60010005:
				if (nArg >= 0) {
					teVariantChangeType(&v, &pDispParams->rgvarg[nArg], VT_BSTR);
					teSetLong(pVarResult, m_ppd->SetTitle(v.bstrVal));
					VariantClear(&v);
				}
				return S_OK;
			//StartProgressDialog
			case 0x60010006:
				if (nArg >= 2) {
					IUnknown *punk = NULL;
					FindUnknown(&pDispParams->rgvarg[nArg - 1], &punk);
					teSetLong(pVarResult, m_ppd->StartProgressDialog((HWND)GetPtrFromVariant(&pDispParams->rgvarg[nArg]), punk, GetIntFromVariant(&pDispParams->rgvarg[nArg - 2]), NULL));
				}
				return S_OK;
			//StopProgressDialog
			case 0x60010007:
				m_ppd->SetLine(2, L"", TRUE, NULL);
				teSetLong(pVarResult, m_ppd->StopProgressDialog());
				return S_OK;
			//Timer
			case 0x60010008:
				if (nArg >= 0) {
					teSetLong(pVarResult, m_ppd->Timer(GetIntFromVariant(&pDispParams->rgvarg[nArg]), NULL));
				}
				return S_OK;
			//SetAnimation
			case 0x60010009:
#ifdef _2000XP
				if (nArg >= 1) {
					teSetLong(pVarResult, m_ppd->SetAnimation((HINSTANCE)GetPtrFromVariant(&pDispParams->rgvarg[nArg]), GetIntFromVariant(&pDispParams->rgvarg[nArg - 1])));
				}
#endif
				return S_OK;
			case DISPID_VALUE:
				teSetObject(pVarResult, this);
				return S_OK;
		}//end_switch
	} catch (...) {
		return teException(pExcepInfo, "ProgressDialog", methodPD, dispIdMember);
	}
	return DISP_E_MEMBERNOTFOUND;
}

//CteObject
CteObject::CteObject(PVOID pObj)
{
	m_punk = NULL;
	if (pObj) {
		try {
			IUnknown *punk = static_cast<IUnknown *>(pObj);
			punk->QueryInterface(IID_PPV_ARGS(&m_punk));
		} catch (...) {
			g_nException = 0;
#ifdef _DEBUG
			g_strException = L"CteObject";
#endif
		}
	}
	m_cRef = 1;
}

CteObject::~CteObject()
{
	Clear();
}

VOID CteObject::Clear()
{
	SafeRelease(&m_punk);
}

STDMETHODIMP CteObject::QueryInterface(REFIID riid, void **ppvObject)
{
	*ppvObject = NULL;

	if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDispatch)) {
		*ppvObject = static_cast<IDispatch *>(this);
		AddRef();
		return S_OK;
	}
	if (m_punk) {
		return m_punk->QueryInterface(riid, ppvObject);
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CteObject::AddRef()
{
	return ::InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CteObject::Release()
{
	if (::InterlockedDecrement(&m_cRef) == 0) {
		delete this;
		return 0;
	}

	return m_cRef;
}

STDMETHODIMP CteObject::GetTypeInfoCount(UINT *pctinfo)
{
	*pctinfo = 0;
	return S_OK;
}

STDMETHODIMP CteObject::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CteObject::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	return teGetDispId(methodCO, 0, NULL, *rgszNames, rgDispId, TRUE);
}

STDMETHODIMP CteObject::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	switch (dispIdMember) {
		case 0x60010001:
			Clear();
			return S_OK;
		//this
		case DISPID_VALUE:
			teSetObject(pVarResult, this);
			return S_OK;
	}//end_switch
	return DISP_E_MEMBERNOTFOUND;
}

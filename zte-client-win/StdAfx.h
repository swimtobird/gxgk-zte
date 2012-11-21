// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently

#if !defined(AFX_STDAFX_H__7C0E3A18_9A51_445F_8810_EFC647794D15__INCLUDED_)
#define AFX_STDAFX_H__7C0E3A18_9A51_445F_8810_EFC647794D15__INCLUDED_

#ifndef _WIN32_IE // 
#define _WIN32_IE 0x0500 //
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <windows.h>

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#include <Iphlpapi.h>
#pragma comment(lib, "Iphlpapi.lib")

#include "pcap.h"
#pragma comment(lib, "wpcap.lib")

//#pragma warning (disable: 4800)
#define CURL_STATICLIB
#include "curl/curl.h"
#ifdef CURL_STATICLIB
#ifdef _DEBUG
#pragma comment(lib, "libcurld_static.lib")
#else
#pragma comment(lib, "libcurl_static.lib")
#endif
#else
#pragma comment(lib, "libcurl.lib")
#endif
//#pragma warning (default: 4800)

#include "WebAuth.h"

typedef  unsigned char	u_char;

#endif // _AFX_NO_AFXCMN_SUPPORT

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__7C0E3A18_9A51_445F_8810_EFC647794D15__INCLUDED_)
#if !defined(AFX_SETTINGDLG_H__23034AFF_1612_43AD_B47F_42C19FC2810A__INCLUDED_)
#define AFX_SETTINGDLG_H__23034AFF_1612_43AD_B47F_42C19FC2810A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SettingDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSettingDlg dialog
#include "Config.h"
#include "HyperLink.h"
extern CConfig Config;
class CSettingDlg : public CDialog
{
// Construction
public:
	CSettingDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSettingDlg)
	enum { IDD = IDD_SETTING };	
	CHyperLink	m_url;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSettingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	// Generated message map functions
	//{{AFX_MSG(CSettingDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnChkWebAuth();
	afx_msg void OnChkWebLogout();
	afx_msg void OnChkEnableWebaccount();
	afx_msg void OnChkReauthTime();
	afx_msg void OnKillfocusTxtReauthTime();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETTINGDLG_H__23034AFF_1612_43AD_B47F_42C19FC2810A__INCLUDED_)

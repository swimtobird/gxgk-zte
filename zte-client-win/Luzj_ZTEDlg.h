// Luzj_ZTEDlg.h : header file
//

#if !defined(AFX_LUZJ_ZTEDLG_H__AC99FC7E_1545_435A_8DEE_A24B2C06EF61__INCLUDED_)
#define AFX_LUZJ_ZTEDLG_H__AC99FC7E_1545_435A_8DEE_A24B2C06EF61__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CLuzj_ZTEDlg dialog
#include "PicStatic.h"
#include "SystemTray.h"
#include "SettingDlg.h"
#include "Config.h"
#include "Packet.h"
#include "Define.h"
#define WM_USER_TRAY_NOTIFICATION (WM_USER+0x101)


extern CConfig Config;


class CLuzj_ZTEDlg : public CDialog
{
// Construction
public:
	CLuzj_ZTEDlg(CWnd* pParent = NULL);	// standard constructor

	CSystemTray		m_tray;					//系统托盘图标
	CStatusBarCtrl  m_StatusBar;			//状态栏
	bool			m_bAuth;				//是否已经认证
	time_t			m_startTime;			//认证的开始时间
	HANDLE			m_AuthThread;			//认证的线程句柄,以便控制线程
	RECT			m_rc;					//存放着窗体的矩形区域

    pcap_t			* m_adapterHandle;		//适配器句柄
	u_char			m_MacAdd[6];			//存放MAC地址


	u_char			m_username[50];
	int				m_usernameLen;
	u_char			m_password[50];
	int				m_passwordLen;

	char	m_ip[16];
	char	m_adaptername[256];

	CEdit	*editLog;


	void	SetBubble(char * title,char * content,int timeout=1000);	
	void	Log (const char *fmt, ...);
	char*	DescriptionToName(const char *description);
	char*	HttpAuth(BOOL bForce);
	void	UpdateStatus(BOOL bOnline);
	char *	GetOSVersion();

	static void	get_packet(u_char *args, const struct pcap_pkthdr *pcaket_header, const u_char *packet);
	static DWORD WINAPI eap_thread(void *para);
	DWORD WINAPI GetMacIP(const char *adaptername, char ip[16], unsigned char mac[6]);
	DWORD WINAPI IpconfigRenew();
	int CheckUpdate();
	
// Dialog Data
	//{{AFX_DATA(CLuzj_ZTEDlg)
	enum { IDD = IDD_LUZJ_ZTE_DIALOG };
	CComboBox	m_ccb_username;		
	CComboBox	m_ccbNetCard;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLuzj_ZTEDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CLuzj_ZTEDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnStart();
	afx_msg void OnLogoff();
	afx_msg void OnExit();
	afx_msg void OnTrayShow();
	afx_msg LONG OnTrayNotification(WPARAM wparam, LPARAM lparam);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSetting();
	afx_msg void OnLogshow();
	afx_msg void OnSelchangeUsername();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LUZJ_ZTEDLG_H__AC99FC7E_1545_435A_8DEE_A24B2C06EF61__INCLUDED_)

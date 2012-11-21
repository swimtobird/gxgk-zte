

/**************************************************************************************
			The Luzj's Zte Project
			//////////////////////
			Copyleft ? 2009 Luzj
		Author:Luzj		QQ:86829232
		http://blog.csdn.net/luzjqq
		Email: luzjcn@gmail.com
	///////////////////////////////////
关于Luzj's Zte认证端的声明：

1、本软件所有涉及的中兴认证的功能的实现均是通过黑盒分析得来，并未通过任何不正当方法获得。

2、本软件仅供研究学习之用，不得使用本软件损害中兴公司商业利益。

3、本软件不可用于任何商业和非法用途，否则责任自负。

4、本软件在发布前均通过一般性应用测试，但不保证任何情况下对机器无害，
由于未知的使用环境或不当的使用对计算机造成的损害，责任由使用者全部承担。

5.本软件版权没有，翻印不究，但请协助改进本作品。

6.本软件属开源软件，如果需要修改本软件源码以进行二次发布，也请继续公开源代码。

由于任何不遵守上叙条例引起的纠纷，均与本人无关，如不同意该声明请不要使用该软件，谢谢合作。
**************************************************************************************/
// SettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Luzj_ZTE.h"
#include "Luzj_ZTEDlg.h"
#include "SettingDlg.h"
#include "AutoUpdate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSettingDlg dialog


CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSettingDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingDlg)
	DDX_Control(pDX, IDC_WEBURL, m_url);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingDlg, CDialog)
	//{{AFX_MSG_MAP(CSettingDlg)
	ON_BN_CLICKED(IDC_CHK_WEB_AUTH, OnChkWebAuth)
	ON_BN_CLICKED(IDC_CHK_WEB_LOGOUT, OnChkWebLogout)
	ON_BN_CLICKED(IDC_CHK_ENABLE_WEBACCOUNT, OnChkEnableWebaccount)
	ON_BN_CLICKED(IDC_CHK_REAUTH_TIME, OnChkReauthTime)
	ON_EN_KILLFOCUS(IDC_TXT_REAUTH_TIME, OnKillfocusTxtReauthTime)
	ON_BN_CLICKED(IDC_BTN_AUTO_UPDATE, OnBtnAutoUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettingDlg message handlers

BOOL CSettingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CheckDlgButton(IDC_CHK_AUTOLOGON,Config.m_bAutologon?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_AUTORUN,Config.m_bAutorun?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_BUBBLE,Config.m_bShowBubble?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_WEB_AUTH,Config.m_bWebAuth?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_WEB_LOGOUT,Config.m_bWebLogout?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_ENABLE_WEBACCOUNT,Config.m_bEnableWebAccount?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_REAUTH_TIME,Config.m_bReauth?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_AUTO_UPDATE,Config.m_bAutoUpdate?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_DEBUG,Config.m_bDebug?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_AUTO_FILTER,Config.m_bAutoFilter?BST_CHECKED:BST_UNCHECKED);


	char szTemp[MAX_STRING];
	sprintf(szTemp,"%d",Config.m_iTimeout);
	GetDlgItem(IDC_TIMEOUT)->SetWindowText(szTemp);

	GetDlgItem(IDC_WEB_AUTH_URL)->SetWindowText(Config.m_csWebAuthUrl);
	GetDlgItem(IDC_WEB_LOGOUT_URL)->SetWindowText(Config.m_csWebLogoutUrl);
	GetDlgItem(IDC_WEB_USERNAME)->SetWindowText(Config.m_csWebUsername);
	GetDlgItem(IDC_WEB_PASSWORD)->SetWindowText(Config.m_csWebPassword);
	GetDlgItem(IDC_TXT_REAUTH_TIME)->SetWindowText(Config.m_csReauthTime);
			

	
	m_url.SetURL(STR_WebUrl);
	GetDlgItem(IDC_VERSION)->SetWindowText(STR_AppName"       "STR_Version);

	OnChkWebAuth();
	OnChkWebLogout();
	OnChkEnableWebaccount();
	OnChkReauthTime();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSettingDlg::OnOK()
{
	
#define BIND_BOOL_VAR_CONTROL(v, c) v=(IsDlgButtonChecked(c)==0)?false:true;
	BIND_BOOL_VAR_CONTROL(Config.m_bAutologon, IDC_CHK_AUTOLOGON);
	BIND_BOOL_VAR_CONTROL(Config.m_bWebAuth, IDC_CHK_WEB_AUTH);
	BIND_BOOL_VAR_CONTROL(Config.m_bAutorun, IDC_CHK_AUTORUN);
	BIND_BOOL_VAR_CONTROL(Config.m_bShowBubble, IDC_CHK_BUBBLE);
	BIND_BOOL_VAR_CONTROL(Config.m_bWebLogout, IDC_CHK_WEB_LOGOUT);
	BIND_BOOL_VAR_CONTROL(Config.m_bEnableWebAccount, IDC_CHK_ENABLE_WEBACCOUNT);
	BIND_BOOL_VAR_CONTROL(Config.m_bReauth, IDC_CHK_REAUTH_TIME);
	BIND_BOOL_VAR_CONTROL(Config.m_bAutoUpdate, IDC_CHK_AUTO_UPDATE);
	BIND_BOOL_VAR_CONTROL(Config.m_bDebug, IDC_CHK_DEBUG);
	BIND_BOOL_VAR_CONTROL(Config.m_bAutoFilter, IDC_CHK_AUTO_FILTER);

	char szTemp[MAX_STRING];
	GetDlgItem(IDC_TIMEOUT)->GetWindowText(szTemp,MAX_STRING);
	Config.m_iTimeout=atoi(szTemp);

#define BIND_STRING_VAR_CONTROL(v, c) {CString t; GetDlgItem(c)->GetWindowText(t); v = t;}

	BIND_STRING_VAR_CONTROL(Config.m_csWebAuthUrl, IDC_WEB_AUTH_URL);
	BIND_STRING_VAR_CONTROL(Config.m_csWebLogoutUrl, IDC_WEB_LOGOUT_URL);
	BIND_STRING_VAR_CONTROL(Config.m_csWebUsername, IDC_WEB_USERNAME);
	BIND_STRING_VAR_CONTROL(Config.m_csWebPassword, IDC_WEB_PASSWORD);
	BIND_STRING_VAR_CONTROL(Config.m_csReauthTime, IDC_TXT_REAUTH_TIME);

	Config.SaveConfig();
	CDialog::OnOK();
}

void CSettingDlg::OnChkWebAuth() 
{
	// TODO: Add your control notification handler code here
	if(IsDlgButtonChecked(IDC_CHK_WEB_AUTH))
	{
		GetDlgItem(IDC_WEB_AUTH_URL)->EnableWindow(TRUE);
	}else{
		GetDlgItem(IDC_WEB_AUTH_URL)->EnableWindow(FALSE);
	}
}

void CSettingDlg::OnChkWebLogout() 
{
	// TODO: Add your control notification handler code here
	if(IsDlgButtonChecked(IDC_CHK_WEB_LOGOUT))
	{
		GetDlgItem(IDC_WEB_LOGOUT_URL)->EnableWindow(TRUE);
	}else{
		GetDlgItem(IDC_WEB_LOGOUT_URL)->EnableWindow(FALSE);
	}
}

void CSettingDlg::OnChkEnableWebaccount() 
{
	// TODO: Add your control notification handler code here
	if(IsDlgButtonChecked(IDC_CHK_ENABLE_WEBACCOUNT))
	{
		GetDlgItem(IDC_WEB_USERNAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_WEB_PASSWORD)->EnableWindow(TRUE);
	}else{
		GetDlgItem(IDC_WEB_USERNAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_WEB_PASSWORD)->EnableWindow(FALSE);
	}
}

void CSettingDlg::OnChkReauthTime() 
{
	// TODO: Add your control notification handler code here
	if(IsDlgButtonChecked(IDC_CHK_REAUTH_TIME))
	{
		GetDlgItem(IDC_TXT_REAUTH_TIME)->EnableWindow(TRUE);		
	}else{
		GetDlgItem(IDC_TXT_REAUTH_TIME)->EnableWindow(FALSE);		
	}
}

void CSettingDlg::OnKillfocusTxtReauthTime() 
{
	// TODO: Add your control notification handler code here
	char szTemp[MAX_STRING];
	int hour,min,second;
	CWnd *wndReauthTime = GetDlgItem(IDC_TXT_REAUTH_TIME);
	wndReauthTime->GetWindowText(szTemp,MAX_STRING);
	if(sscanf(szTemp, "%d:%d:%d", &hour, &min, &second) != 3) {
		AfxMessageBox("格式错误！时间修改无效！样例：23:30:00", MB_SYSTEMMODAL);
	}
}

void CSettingDlg::OnBtnAutoUpdate() 
{
	// TODO: Add your control notification handler code here
	CWnd *parent = this->GetParent();
	if(parent != NULL) {
		CLuzj_ZTEDlg *Dlg = (CLuzj_ZTEDlg *)parent;
		if(Dlg->CheckUpdate() == 2) {
			AfxMessageBox("这已经是最新版本的了！");
		}
	}
}

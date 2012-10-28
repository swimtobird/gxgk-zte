
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



// Luzj_ZTEDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Luzj_ZTE.h"
#include "Luzj_ZTEDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CLuzj_ZTEDlg dialog
CLuzj_ZTEDlg::CLuzj_ZTEDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLuzj_ZTEDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLuzj_ZTEDlg)
	m_pass = _T("");
	m_user = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//认证状态为 未认证
	m_bAuth=FALSE;
}

void CLuzj_ZTEDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLuzj_ZTEDlg)
	DDX_Control(pDX, IDC_USERNAME, m_ccb_username);
	DDX_CBString(pDX, IDC_USERNAME, m_user);
	DDX_Text(pDX, IDC_PWD, m_pass);
	DDX_Control(pDX, IDC_LIST_LOG, m_lcLog);
	DDX_Control(pDX, IDC_NETCARD, m_ccbNetCard);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLuzj_ZTEDlg, CDialog)
	//{{AFX_MSG_MAP(CLuzj_ZTEDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_LOGOFF, OnLogoff)
	ON_BN_CLICKED(IDC_TEST, OnTest)
	ON_BN_CLICKED(IDC_EXIT, OnExit)
	ON_BN_CLICKED(MENU_SHOW, OnTrayShow)
	ON_BN_CLICKED(MENU_EXIT, OnExit)
	ON_MESSAGE(WM_USER_TRAY_NOTIFICATION,OnTrayNotification)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SETTING, OnSetting)
	ON_BN_CLICKED(IDC_LOGSHOW, OnLogshow)
	ON_CBN_SELCHANGE(IDC_USERNAME, OnSelchangeUsername)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CLuzj_ZTEDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	SetWindowText(STR_AppName);

	//创建托盘图标
	m_tray.Create(this, WM_USER_TRAY_NOTIFICATION,STR_AppName, m_hIcon, 0);

	//修改列表控件
	m_lcLog.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_FLATSB|LVS_EX_GRIDLINES );
	m_lcLog.InsertColumn(1,"时间",LVCFMT_CENTER,60);
	m_lcLog.InsertColumn(2,"事件",LVCFMT_CENTER,280);


	GetWindowRect(&m_rc);
	m_rc.top=m_rc.bottom-5;    //设置状态栏的矩形区域
	m_StatusBar.Create(WS_CHILD |WS_VISIBLE|CBRS_BOTTOM,m_rc,this,20000);  
	int nParts[2]= {100,-1};      //分割尺寸
	m_StatusBar.SetParts(2, nParts);
	m_StatusBar.SetText("00:00:00",0,0);
	m_StatusBar.SetText("        状态:  未认证",1,0);

	//控制状态栏的显示
	SetTimer(1,1000,NULL);
	//开始的时候先将日志框隐藏
	OnLogshow();

	//////////////////////////////////////////////////////////////////////////
	char szTemp[MAX_STRING];
	int i=0,k=0;
	CString str;

	//读取配置到文件配置对象中去
	Config.LoadConfig();

	CheckDlgButton(IDC_REMEMBER,Config.m_bRememberPWD?BST_CHECKED:BST_UNCHECKED);
	//////////////////////////////////////////////////////////////////////////
	//加载账号信息
	CString user,pass;	
	POSITION p = Config.m_UserInfo.GetStartPosition();	
	while(p != NULL) {
		Config.m_UserInfo.GetNextAssoc(p, user, pass);		
		m_ccb_username.AddString(user);		
	}
	k = m_ccb_username.FindStringExact(-1, Config.m_csLastUser);
	if(k < 0) k = 0;
	if(!Config.m_UserInfo.IsEmpty()) {
		m_ccb_username.SetCurSel(k);
		m_ccb_username.GetWindowText(user);
		GetDlgItem(IDC_PWD)->SetWindowText(Config.m_UserInfo[user]);		
	}
	

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//加载网卡信息
	char m_errorBuffer[ PCAP_ERRBUF_SIZE ];		//错误信息缓冲区
	pcap_if_t		* allAdapters;				//适配器列表
	if(pcap_findalldevs(&allAdapters, m_errorBuffer) == -1 || allAdapters == NULL)
	{
		MessageBox("读取网卡信息失败，请确保你安装了WinPcap!","错误",MB_ICONERROR|MB_OK);
		pcap_freealldevs(allAdapters);
		CDialog::OnCancel();
	}

	pcap_if_t* adapter;//临时存放适配器
    for(adapter = allAdapters; adapter != NULL; adapter = adapter->next)
    {
		m_ccbNetCard.AddString(adapter->description);
    }
	pcap_freealldevs(allAdapters);


	m_ccbNetCard.SetCurSel(0);
	for (i=0;i<m_ccbNetCard.GetCount();i++)
	{
		m_ccbNetCard.GetLBText(i,szTemp);
		if (strcmp(szTemp,Config.m_csNetCard)==0)
		{
			m_ccbNetCard.SetCurSel(i);
			break;
		}
	}
	
	this->AddLog("加载网卡完成");
	//////////////////////////////////////////////////////////////////////////

	//使得开始按钮有效，而断开按钮无效
	this->ChgBtn(TRUE);  

	if (Config.m_bAutologon == TRUE)
	{
		ShowWindow(SW_HIDE);
		this->OnStart();
	}
	SetProcessWorkingSetSize(GetCurrentProcess(),-1,-1);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLuzj_ZTEDlg::ChgBtn(bool bStart,char * szMsg)
{
	if (bStart)
	{
		GetDlgItem(IDC_LOGOFF)->EnableWindow(FALSE);
		GetDlgItem(IDC_START)->EnableWindow(TRUE);
	} 
	else
	{
		GetDlgItem(IDC_LOGOFF)->EnableWindow(TRUE);
		GetDlgItem(IDC_START)->EnableWindow(FALSE);
	}
	if (szMsg!=NULL)
	{
		SetBubble("提示",szMsg);
		MessageBox(szMsg,"错误",MB_ICONERROR|MB_OK);
	}
}


void CLuzj_ZTEDlg::AddLog(CString addStr)
{
	time_t t=time(NULL);
	char szTime[MAX_STRING];
	strftime(szTime,MAX_STRING,"%H:%M:%S",localtime(&t));	
	if (m_lcLog.GetItemCount()>80)
	{
		m_lcLog.DeleteAllItems();
	}
	m_lcLog.InsertItem(0,szTime);	
	m_lcLog.SetItemText(0,1,addStr);
}

void CLuzj_ZTEDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == SC_MINIMIZE || (nID & 0xFFF0) == SC_CLOSE)
	{
		ShowWindow(SW_HIDE);
		SetProcessWorkingSetSize(GetCurrentProcess(),-1,-1);
		SetBubble("提示",STR_AppName" 正在后台运行着……",1);
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}
void CLuzj_ZTEDlg::SetBubble(char * title,char * content,int timeout)
{
	if (Config.m_bShowBubble==TRUE)
	{
		m_tray.SetBubble(title,content,timeout);
	}
}

void CLuzj_ZTEDlg::OnTrayShow()
{   
	if (IsWindowVisible())
	{
		ShowWindow(SW_HIDE);
		SetProcessWorkingSetSize(GetCurrentProcess(),-1,-1);
	} 
	else
	{
		ShowWindow(SW_SHOW);
	}
}


LONG CLuzj_ZTEDlg::OnTrayNotification(WPARAM wparam, LPARAM lparam)
{   
	CMenu	m_menu;
	CPoint	m_point;
	switch (lparam)
	{
	case WM_RBUTTONDOWN:
		m_menu.CreatePopupMenu();
		if (IsWindowVisible())
		{
			m_menu.AppendMenu(MF_ENABLED, MENU_SHOW,_T("隐藏窗口"));
		} 
		else
		{
			m_menu.AppendMenu(MF_ENABLED, MENU_SHOW,_T("显示窗口"));
		}
		m_menu.AppendMenu(MF_ENABLED, MF_SEPARATOR);
		m_menu.AppendMenu(MF_ENABLED, MENU_EXIT,_T("退 出"));
		GetCursorPos(&m_point);
		m_menu.TrackPopupMenu(TPM_LEFTALIGN,m_point.x,m_point.y,this);
		break;
	case WM_LBUTTONDOWN:
		if (IsWindowVisible())
		{
			ShowWindow(SW_HIDE);
			SetProcessWorkingSetSize(GetCurrentProcess(),-1,-1);
		}
		else
		{
			ShowWindow(SW_SHOW);
			BringWindowToTop();
		}	
		break;
	}
	return 0;
}

void CLuzj_ZTEDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLuzj_ZTEDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CLuzj_ZTEDlg::OnStart() 
{
	//////////////////////////////////////////////////////////////////////////
	//		先存放设置参数
	CString str,strUser,strPass,strTemp;

	//取得用户名密码
	this->getUserInfo();
	if (m_usernameLen<1 || m_passwordLen<1)
	{
		this->ChgBtn(TRUE,"用户名或者密码太短!");
		return;	
	}
	this->ChgBtn(FALSE);

	GetDlgItem(IDC_NETCARD)->GetWindowText(strTemp);
	Config.m_csNetCard=strTemp;

	GetDlgItem(IDC_USERNAME)->GetWindowText(strUser);
	if (IsDlgButtonChecked(IDC_REMEMBER))
	{
		GetDlgItem(IDC_PWD)->GetWindowText(strPass);
		Config.m_bRememberPWD=TRUE;
	}
	else
	{
		strPass="";
		Config.m_bRememberPWD=FALSE;
	}

	Config.m_csLastUser=strUser;	
	Config.m_UserInfo[strUser]=strPass;

	Config.SaveConfig();
	//////////////////////////////////////////////////////////////////////////

	SetBubble("提示",STR_AppName" 开始联网认证……");

	DWORD dwPID;
	m_AuthThread=CreateThread(NULL,0,CLuzj_ZTEDlg::StartAuth,this,0,&dwPID);
}

DWORD WINAPI CLuzj_ZTEDlg::GetMacIP(const char *adaptername, char ip[16], unsigned char mac[6])
{

	PIP_ADAPTER_INFO AdapterInfo = NULL;

	DWORD dwBufLen = sizeof(IP_ADAPTER_INFO);
	DWORD dwStatus;
	AdapterInfo = (PIP_ADAPTER_INFO)malloc(dwBufLen);
	if(AdapterInfo == NULL) return -1;
	dwStatus = GetAdaptersInfo(AdapterInfo,&dwBufLen);	
	if(dwStatus == ERROR_BUFFER_OVERFLOW) {
		free(AdapterInfo); 
		AdapterInfo = (PIP_ADAPTER_INFO)malloc(dwBufLen);
		dwStatus = GetAdaptersInfo(AdapterInfo,&dwBufLen);	
		if(AdapterInfo == NULL) return -1;
	}
	if(dwStatus != NO_ERROR) {
		if(AdapterInfo != NULL) free(AdapterInfo);
		return -2;
	}

	pcap_if_t* adapter;							//临时存放适配器
	pcap_if_t* allAdapters;						//适配器列表
	
	if(pcap_findalldevs(&allAdapters, NULL) == -1 || allAdapters == NULL) {		
		free(AdapterInfo); pcap_freealldevs(allAdapters);	return -1;
	}
	
	memset(ip, 0, 16);	memset(mac, 0, 6);

    for(adapter = allAdapters; adapter != NULL; adapter= adapter->next) {
		if (strcmp(adapter->description,adaptername)==0) break;
    }
 	
	if(adapter != NULL) {
		PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;		
		while(pAdapterInfo) {
			if (strstr(adapter->name,pAdapterInfo->AdapterName) >= 0) {
				memcpy(mac, pAdapterInfo->Address, 6);
				strncpy(ip, pAdapterInfo->IpAddressList.IpAddress.String, 16);
				break;
			}
			pAdapterInfo = pAdapterInfo->Next;
		};
	}
	free(AdapterInfo);	pcap_freealldevs(allAdapters);
	return 0;
}

DWORD WINAPI CLuzj_ZTEDlg::IpconfigRenew()
{
	
	SHELLEXECUTEINFO ShExecInfo = {0};
	TCHAR tempBuffer[MAX_STRING];
	CString strSystemDirectory;
	GetSystemDirectory(tempBuffer, MAX_STRING);
	strSystemDirectory.Format(_T("%s\\ipconfig.exe"),tempBuffer);
	ShExecInfo.cbSize	= sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask		= SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.lpFile		= strSystemDirectory;		
	ShExecInfo.lpParameters = _T("/renew");	
	ShExecInfo.nShow	= SW_HIDE;
	ShellExecuteEx(&ShExecInfo);
	
	int iRnt =WaitForSingleObject(ShExecInfo.hProcess,Config.m_iTimeout*1000);
	if (iRnt == WAIT_TIMEOUT || iRnt == WAIT_FAILED) return -1;
	
	return 0;
}

DWORD WINAPI CLuzj_ZTEDlg::ZteAuth(const char *username, const char *password, 
								   const char *adaptername)
{
	char ip[16];
	unsigned char mac[6];
	if(GetMacIP(adaptername, ip, mac) != 0) return 1;

	pcap_t	* adapterhandle;
	if((adapterhandle=pcap_open_live(adaptername,65536,1,1000,NULL)) == NULL) {		
		return 2;
    }

	//包过滤字符串
	char	FilterStr[100];		
	struct bpf_program	mfcode;	
	const u_char		*captured;
	struct pcap_pkthdr	*header;
	snprintf(FilterStr, 100, "(ether proto 0x888e) and (ether dst host %02x:%02x:%02x:%02x:%02x:%02x)",
			mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    if(pcap_compile(adapterhandle, &mfcode, FilterStr, 1, 0xff)==-1
		||pcap_setfilter(adapterhandle, &mfcode)==-1) {
		return 3;
    } 	

	//zte auth
	CPacket packet;
//	packet.send_packet_logoff(adapterhandle,mac);

	if(!packet.send_packet_start(adapterhandle,mac))   return 4;
	if(pcap_next_ex(adapterhandle, &header, &captured) != 1) return 5;
	
	if(!packet.send_packet_response_Identity(adapterhandle,captured,mac,(u_char*)username,strlen(username))) return 6;
	if(pcap_next_ex(adapterhandle, &header, &captured) != 1) return 7;

	if(!packet.send_packet_response_MD5(adapterhandle,captured,mac,
										(u_char*)username,strlen(username),
										(u_char*)password,strlen(password))) return 8;
	if(pcap_next_ex(adapterhandle, &header, &captured) != 1) return 9;
	if(captured[18] != 0x03)	return 10;

	IpconfigRenew(); GetMacIP(adaptername, ip, mac);

	const int trytime = 5;	int i;
	pcap_set_timeout(adapterhandle, Config.m_iTimeout * 1000);
	while(TRUE) {
		if(pcap_next_ex(adapterhandle, &header, &captured) != 1) break;
		if(captured[15]!=0x03 && captured[18]!=0x01) continue;
		for(i = 0 ; i < trytime; i++) {
			if(!packet.send_packet_key2(adapterhandle,captured,mac)) break;
			Sleep(500);
		}
		if(i >= trytime) return 11;
//web auth
		if(Config.m_bWebAuth && Config.m_csWebAuthUrl.GetLength() > 0){
			if(Config.m_bEnableWebAccount) {
				WebAuth(Config.m_csWebUsername, Config.m_csWebPassword, ip, Config.m_csWebAuthUrl);
			} else {
				WebAuth(username, password, ip, Config.m_csWebAuthUrl);
			}
		}
	}
	
	return -100;
}

DWORD WINAPI CLuzj_ZTEDlg::StartAuth(LPVOID pParam)
{
	CLuzj_ZTEDlg* Dlg=(CLuzj_ZTEDlg*)pParam;
	int retcode = 0;
	const int	DefaultTimeout=1000;
	
	/////////////////////////////////////////////////////////////////////////
	//寻找所选的网卡的MAC
	IP_ADAPTER_INFO AdapterInfo[16];
	DWORD dwBufLen = sizeof(AdapterInfo);
	DWORD dwStatus = GetAdaptersInfo(AdapterInfo,&dwBufLen);
 	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;

	pcap_if_t* adapter;							//临时存放适配器
	pcap_if_t* allAdapters;						//适配器列表
	char m_errorBuffer[ PCAP_ERRBUF_SIZE ];		//错误信息缓冲区
	if(pcap_findalldevs(&allAdapters, m_errorBuffer) == -1 || allAdapters == NULL)
	{
		Dlg->ChgBtn(TRUE,"读取网卡信息失败，请确保你安装了WinPcap!");
		pcap_freealldevs(allAdapters);
		return 0;
	}
    for(adapter = allAdapters; adapter != NULL; adapter= adapter->next)
    {
		if (strcmp(adapter->description,Config.m_csNetCard)==0)
		{
			break;
		}
    }

	do{
		if (strstr(adapter->name,pAdapterInfo->AdapterName)>0)
		{
			Dlg->m_MacAdd[0]=pAdapterInfo->Address[0];
			Dlg->m_MacAdd[1]=pAdapterInfo->Address[1];
			Dlg->m_MacAdd[2]=pAdapterInfo->Address[2];
			Dlg->m_MacAdd[3]=pAdapterInfo->Address[3];
			Dlg->m_MacAdd[4]=pAdapterInfo->Address[4];
			Dlg->m_MacAdd[5]=pAdapterInfo->Address[5];

			break;
		}
	}while(pAdapterInfo = pAdapterInfo->Next);

	CString csMac;
	if (!pAdapterInfo)
	{
		Dlg->ChgBtn(TRUE,"神奇的错误，居然没有找到网卡的MAC!");
		return 0;
	} else {
		csMac.Format("MAC地址:%02X:%02X:%02X:%02X:%02X:%02X", 
			Dlg->m_MacAdd[0], Dlg->m_MacAdd[1], Dlg->m_MacAdd[2],
			Dlg->m_MacAdd[3], Dlg->m_MacAdd[4], Dlg->m_MacAdd[5]);
		Dlg->AddLog(csMac);
	}

	if (pAdapterInfo->DhcpEnabled!=1)//适配器是否启用了动态主机配置协议（DHCP）
	{				
		Dlg->ChgBtn(TRUE,"请先去 控制面板--管理--服务 里面，将DHCP服务启动!");
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////
	// 打开指定适配器
    if((Dlg->m_adapterHandle=pcap_open_live(adapter->name,65536,1,DefaultTimeout,m_errorBuffer)) == NULL)
    {
		Dlg->ChgBtn(TRUE,"无法打开网卡!");
		pcap_freealldevs(allAdapters);
		return 0;
    }
	pcap_freealldevs(allAdapters);


	Dlg->AddLog("打开了指定网卡适配器");
	

	char	FilterStr[100];		//包过滤字符串
	struct bpf_program	mfcode;	
	const u_char		*captured;
	struct pcap_pkthdr	*header;

	sprintf(FilterStr, "(ether proto 0x888e) and (ether dst host %02x:%02x:%02x:%02x:%02x:%02x)",
			Dlg->m_MacAdd[0],Dlg->m_MacAdd[1],Dlg->m_MacAdd[2],Dlg->m_MacAdd[3],Dlg->m_MacAdd[4],Dlg->m_MacAdd[5]);

    if(pcap_compile(Dlg->m_adapterHandle, &mfcode, FilterStr, 1, 0xff)==-1
		||pcap_setfilter(Dlg->m_adapterHandle, &mfcode)==-1)
    {
		Dlg->ChgBtn(TRUE,"过滤包设置错误!");
		return 0;
    }	

	CPacket packet;
	time_t t1,t2;

	//////////////////////////////////////////////////////////////////////////
	///开始认证包
    if(!packet.send_packet_start(Dlg->m_adapterHandle,Dlg->m_MacAdd))
    {
        Dlg->ChgBtn(TRUE,"EAPOL-Start包发送失败!");
		Dlg->AddLog("EAPOL-Start包发送失败!");
		return 0;
    }
	Dlg->AddLog("发送 EAPOL-Start 包，请求认证");
	//////////////////////////////////////////////////////////////////////////
	//接收回应包
	t1=time(NULL);
	retcode=pcap_next_ex(Dlg->m_adapterHandle, &header, &captured);
	t2=time(NULL);
	while(retcode!=1 && t2-t1<Config.m_iTimeout)
	{
		Sleep(1000);
		retcode=pcap_next_ex(Dlg->m_adapterHandle, &header, &captured);
		t2=time(NULL);
	}
	if (retcode!=1)
	{
		Dlg->ChgBtn(TRUE,"对于EAPOL-Start包,服务器响应超时!");
		Dlg->AddLog("对于EAPOL-Start包,服务器响应超时!");
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	//发送EAP-Identity包
	if(!packet.send_packet_response_Identity(Dlg->m_adapterHandle,captured,Dlg->m_MacAdd,Dlg->m_username,Dlg->m_usernameLen))
	{
		Dlg->ChgBtn(TRUE,"EAP-Identity包发送失败!");
		return 0;
	}
	Dlg->AddLog("发送 EAP-Identity 包，提交账号");
	//////////////////////////////////////////////////////////////////////////
	//接收回应包
	t1=time(NULL);
	retcode=pcap_next_ex(Dlg->m_adapterHandle, &header, &captured);
	t2=time(NULL);
	while(retcode!=1 && t2-t1<Config.m_iTimeout)
	{
		Sleep(1000);
		retcode=pcap_next_ex(Dlg->m_adapterHandle, &header, &captured);
		t2=time(NULL);
	}
	if (retcode!=1)
	{
		Dlg->ChgBtn(TRUE,"对于EAP-Identity包,服务器响应超时!");
		Dlg->AddLog("对于EAP-Identity包,服务器响应超时!");
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	//发送MD5挑战包
    if(!packet.send_packet_response_MD5(Dlg->m_adapterHandle,captured,Dlg->m_MacAdd,
										Dlg->m_username,Dlg->m_usernameLen,Dlg->m_password,Dlg->m_passwordLen))
    {
        Dlg->ChgBtn(TRUE,"MD5-Challenge包发送失败!");
		return 0;
    }
	Dlg->AddLog("发送 MD5-Challenge 包，提交密码");
	//////////////////////////////////////////////////////////////////////////
	//接收回应包
	t1=time(NULL);
	retcode=pcap_next_ex(Dlg->m_adapterHandle, &header, &captured);
	t2=time(NULL);
	while(retcode!=1 && t2-t1<Config.m_iTimeout)
	{
		Sleep(1000);
		retcode=pcap_next_ex(Dlg->m_adapterHandle, &header, &captured);
		t2=time(NULL);
	}
	if (retcode!=1)
	{
		Dlg->ChgBtn(TRUE,"对于 MD5-Challenge 包,服务器响应超时!");
		Dlg->AddLog("对于 MD5-Challenge 包,服务器响应超时!");
		return 0;
	}
	if (captured[18]==0x03)
	{
		Dlg->AddLog("服务器认证通过");
	} 
	else
	{
		Dlg->ChgBtn(TRUE,"服务器认证失败,请查看日志信息!");		
		Dlg->AddLog(captured+24);
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////

	Dlg->ShowWindow(SW_HIDE);

	SHELLEXECUTEINFO ShExecInfo = {0};
	TCHAR tempBuffer[MAX_STRING];
	CString strSystemDirectory;
	GetSystemDirectory( tempBuffer, MAX_STRING);
	strSystemDirectory.Format(_T("%s\\ipconfig.exe"),tempBuffer);
	ShExecInfo.cbSize	= sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask		= SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.lpFile		= strSystemDirectory;		
	ShExecInfo.lpParameters = _T("/renew");	
	ShExecInfo.nShow	= SW_HIDE;
	ShellExecuteEx(&ShExecInfo);
	
	int iRnt =WaitForSingleObject(ShExecInfo.hProcess,Config.m_iTimeout*1000);
	if (iRnt == WAIT_TIMEOUT) 
	{
		Dlg->SetBubble("提示","  获取IP超时,中断认证!");
		return 0;
	}
	else if (iRnt == WAIT_FAILED)
	{
		Dlg->SetBubble("提示","  获取IP失败,中断认证!");
		return 0;
	}
	else
	{
		Dlg->SetBubble("提示","  认证成功!");
	}

		
	dwBufLen = sizeof(AdapterInfo);
	dwStatus = GetAdaptersInfo(AdapterInfo,&dwBufLen);
 	pAdapterInfo = AdapterInfo;

	if(pcap_findalldevs(&allAdapters, m_errorBuffer) == -1 || allAdapters == NULL)
	{
		Dlg->ChgBtn(TRUE,"读取网卡信息失败，请确保你安装了WinPcap!");
		pcap_freealldevs(allAdapters);
		return 0;
	}
	Dlg->m_ip[0] = 0;
    for(adapter = allAdapters; adapter != NULL; adapter= adapter->next)
    {
		if (strcmp(adapter->description,Config.m_csNetCard)==0)
		{
			pcap_addr *paddr = adapter->addresses;
			sockaddr_in *sin;
			for(;paddr; paddr = paddr->next)
			{
				sin = (struct sockaddr_in *)paddr->addr;
				if(sin->sin_family == AF_INET) {
					strncpy(Dlg->m_ip, inet_ntoa(sin->sin_addr), 16);				
					break;
				}
			}
			break;
		}
    }
	pcap_freealldevs(allAdapters);

	CString csIP;
	if (Dlg->m_ip[0] == 0)
	{
		Dlg->ChgBtn(TRUE,"神奇的错误，居然没有找到网卡的IP地址!");
		return 0;
	} else {
		csIP.Format("IP地址:%s", Dlg->m_ip);
		Dlg->AddLog(csIP);
	}

	//
	// 进行网页认证
	//
	
	if(Config.m_bWebAuth && Config.m_csWebAuthUrl.GetLength() > 0){
		char *msg;
		int i;
		for(i = 0; i < 3; i++) {
			if(Config.m_bEnableWebAccount) {
				msg = WebAuth(Config.m_csWebUsername, Config.m_csWebPassword, Dlg->m_ip, Config.m_csWebAuthUrl);
			} else {
				msg = WebAuth((const char *)Dlg->m_username, (const char *)Dlg->m_password, Dlg->m_ip, Config.m_csWebAuthUrl);
			}
			if(msg == NULL) {			
				Dlg->AddLog("网页认证成功!");
				break;
			} else {				
				Dlg->AddLog(msg);			
			}
			Sleep(3000);
		}
		if(i >= 3) {
			Dlg->ChgBtn(FALSE,"网页认证失败，请稍后手动进行网页认证!");
		}
	}


	Dlg->m_bAuth=TRUE;
	Dlg->m_startTime =time(NULL);
	SetProcessWorkingSetSize(GetCurrentProcess(),-1,-1);
	int good = 0;
	int iTimeOut=Config.m_iTimeout*2;
	while (Dlg->m_bAuth)
	{
		t1=time(NULL);
		retcode=pcap_next_ex(Dlg->m_adapterHandle, &header, &captured);
		t2=time(NULL);
		while(Dlg->m_bAuth && (t2-t1)<iTimeOut
			&& (captured[15]!=0x03 && captured[18]!=0x01 || retcode!=1))
		{
			Sleep(200);
			retcode=pcap_next_ex(Dlg->m_adapterHandle, &header, &captured);
			t2=time(NULL);
		}
		if (t2-t1>=iTimeOut)
		{
			Dlg->ChgBtn(TRUE,"接收EAPOL-Key包超时,估计网络已经中断!");
			Dlg->AddLog("接收EAPOL-Key包超时,估计网络已经中断!");
			return 0;
		}
		/*
		if(!packet.send_packet_key1(Dlg->m_adapterHandle,captured,Dlg->m_MacAdd))
		{
			Dlg->ChgBtn(TRUE,"回应EAPOL-Key1 包错误!");
			Dlg->AddLog("回应EAPOL-Key1 包错误!");
			return 0;
		}
		*/
		if(!packet.send_packet_key2(Dlg->m_adapterHandle,captured,Dlg->m_MacAdd))
		{
			Dlg->ChgBtn(TRUE,"回应EAPOL-Key 包错误!");
			Dlg->AddLog("回应EAPOL-Key 包错误!");
			return 0;
		}
		
		if(good < 3) {
			Dlg->AddLog("发送 EAPOL-Key 包，维持连接");
			good++;
		}
		else if(good == 3) {
			Dlg->AddLog("已正常工作，将维持连接信息将被忽略");
			good = 4;
		}
		
		if(Config.m_bWebAuth && Config.m_csWebAuthUrl.GetLength() > 0) {
			if(Config.m_bEnableWebAccount) {
				WebAuth(Config.m_csWebUsername, Config.m_csWebPassword, Dlg->m_ip, Config.m_csWebAuthUrl);
			} else {
				WebAuth((const char *)Dlg->m_username, (const char *)Dlg->m_password, Dlg->m_ip, Config.m_csWebAuthUrl);
			}
		}
	}

    return 0;
}

//========================OnLogoff======================================
void CLuzj_ZTEDlg::OnLogoff() 
{
	if(Config.m_bWebLogout && strlen(Config.m_csWebLogoutUrl) > 0) {
		char *msg = WebLogout(this->m_ip, Config.m_csWebLogoutUrl);
		if(msg == NULL) {
			this->AddLog("网页认证已下线!");
		} else {
			this->AddLog("网页认证下线失败!");
			this->AddLog(msg);
		}
	}

	CPacket packet;
	packet.send_packet_logoff(m_adapterHandle,m_MacAdd);

	this->ChgBtn(TRUE);

	m_bAuth=FALSE;

	//等待，如果500ms后没有自动退出，则强制结束
	if (::WaitForSingleObject(this->m_AuthThread,500) == WAIT_TIMEOUT)	
	{
		::TerminateThread(this->m_AuthThread ,0);
	}

	m_StatusBar.SetText("        状态: 已经下线!",1,0);
	this->AddLog("已经下线!");
}
//========================OnLogoff======================================


//========================getUserInfo======================================
void CLuzj_ZTEDlg::getUserInfo()
{
	UpdateData();
	m_usernameLen=m_user.GetLength();
	m_passwordLen=m_pass.GetLength();
	strncpy((char*)m_username, m_user, sizeof(m_username));
	strncpy((char*)m_password, m_pass, sizeof(m_password));
}
//========================getUserInfo======================================


//////////////////////////////////////////////////////////////////////////
//响应退出按钮，如果已经认证，就先发送退出认证包
void CLuzj_ZTEDlg::OnExit() 
{
	ShowWindow(SW_HIDE);
	if (m_bAuth)
	{
		m_bAuth=!m_bAuth;
		this->OnLogoff();
	}
	CDialog::OnOK();
}
//////////////////////////////////////////////////////////////////////////


void CLuzj_ZTEDlg::OnTimer(UINT nIDEvent)
{
	time_t t =time(NULL);
	char szTime[MAX_STRING];
	strftime(szTime,MAX_STRING,"%H:%M:%S", localtime(&t));
	m_StatusBar.SetText(szTime,0,0);

	if(m_bAuth && Config.m_bReauth && strcmp(szTime, Config.m_csReauthTime) == 0) {
		OnLogoff();
		OnStart();
	}

	if (m_bAuth)
	{
		t -= m_startTime;
		strftime(szTime,MAX_STRING,"        状态:  在线时间%H:%M:%S:",gmtime(&t));
		m_StatusBar.SetText(szTime,1,0);
	}
	else
	{
		m_StatusBar.SetText("        状态:  未联网",1,0);
	}
		
//	m_AuthThread=CreateThread(NULL,0,CLuzj_ZTEDlg:::ZteAuth,this,0,&dwPID);

	CDialog::OnTimer(nIDEvent);
}

void CLuzj_ZTEDlg::OnLogshow() 
{
	static bShow=TRUE;
	RECT rc1={0},rc2={0};
	if (bShow)
	{
		GetDlgItem(IDC_SPLIT)->GetWindowRect(&rc1);
		GetWindowRect(&m_rc);
		rc2=m_rc;
		rc2.right=rc1.left-2;
		MoveWindow(&rc2,TRUE);
		bShow=!bShow;
		GetDlgItem(IDC_LOGSHOW)->SetWindowText("日志>>");
	}
	else
	{
		GetWindowRect(&rc1);
		
		m_rc.right+=rc1.left-m_rc.left;
		m_rc.bottom+=rc1.top-m_rc.top;
		m_rc.left=rc1.left;
		m_rc.top=rc1.top;

		MoveWindow(&m_rc,TRUE);
		bShow=!bShow;
		GetDlgItem(IDC_LOGSHOW)->SetWindowText("日志<<");
	}
}

void CLuzj_ZTEDlg::OnSelchangeUsername() 
{
	CString str;
	m_ccb_username.GetLBText(m_ccb_username.GetCurSel(), str);	
	GetDlgItem(IDC_PWD)->SetWindowText(Config.m_UserInfo[str]);	
}

void CLuzj_ZTEDlg::OnTest() 
{
//	char szTemp[MAX_STRING];

/*
	MIB_IFTABLE *pIfTable; 
    MIB_IFROW *pIfRow; 
	pIfTable = (MIB_IFTABLE *) malloc(sizeof (MIB_IFTABLE)); 
	DWORD len=sizeof(MIB_IFTABLE);
	if (GetIfTable(pIfTable,&len,TRUE) == ERROR_INSUFFICIENT_BUFFER)
	{
		free(pIfTable); 
		pIfTable = (MIB_IFTABLE *)malloc(len);
	}
	if (GetIfTable(pIfTable,&len,TRUE)==NO_ERROR)
	{
		MessageBox("OK");
		for (int i=0;i<pIfTable->dwNumEntries;i++)
		{
				//if (pIfTable->table[i].dwOperStatus==MIB_IF_OPER_STATUS_CONNECTED)
				//{
					sprintf(szTemp,"%d",pIfTable->table[i].dwOperStatus);
					MessageBox(szTemp);
				//}
		}
	}

	//加载网卡信息
	char m_errorBuffer[ PCAP_ERRBUF_SIZE ];		//错误信息缓冲区
	pcap_if_t		* allAdapters;				//适配器列表
	if(pcap_findalldevs(&allAdapters, m_errorBuffer) == -1 || allAdapters == NULL)
	{
		MessageBox("读取网卡信息失败，请确保你安装了WinPcap!","错误",MB_ICONERROR|MB_OK);
		pcap_freealldevs(allAdapters);
		CDialog::OnCancel();
	}
	
	pcap_if_t* adapter;//临时存放适配器
    for(adapter = allAdapters; adapter != NULL; adapter = adapter->next)
    {
		MessageBox(adapter->description);
    }
	pcap_freealldevs(allAdapters);

*/
}

void CLuzj_ZTEDlg::OnSetting() 
{
	CSettingDlg dlg;
	if (dlg.DoModal())
	{
	}
}

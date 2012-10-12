// Config.h: interface for the CConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIG_H__2EE917D0_3B18_4580_BC7D_C675F4866D9A__INCLUDED_)
#define AFX_CONFIG_H__2EE917D0_3B18_4580_BC7D_C675F4866D9A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include "Define.h"

struct userInfo 
{
//	int id;
	CString user;
	CString pass;
};

class CConfig  
{
public:
	CConfig();
	virtual ~CConfig();
	void	SaveConfig();
	void	LoadConfig();
	void	LoadDefaultConfig();
	void	GetFullPathToFile(char  *pszFullPath,char * pszFilename);

public:
	bool	m_bAutorun;		//自动运行
	bool	m_bAutologon;		//自动登录
	bool	m_bRememberPWD;	//记忆密码
	bool	m_bShowBubble;		//显示气泡

	int		m_iTimeout;		//超时时间

	bool m_bWebAuth;//是否进行网页认证
	CString m_csWebAuthUrl;//网页认证地址

	bool m_bWebLogout;//是否进行网页注销
	CString m_csWebLogoutUrl;//网页注销地址

	bool m_bEnableWebAccount;//是否启用网页认证帐号信息，否则与中兴认证帐号一样
	CString m_csWebUsername;//网页认证用户名
	CString m_csWebPassword;//网页认证密码


	CMap<int,int,userInfo,userInfo>   m_UserInfo;	//存放所有的账号密码信息

	CString m_csLastUser;	//上次所用的用户名

	CString	m_csNetCard;	//上次所选择的网卡名字



/*==================以下配置参数尚未被启用================
	bool m_bDHCP;			//是否启用DHCP

	bool m_bReauth;		//重试认证
	int  m_iReauthTime;	//重试次数
=========================================================*/
};

#endif // !defined(AFX_CONFIG_H__2EE917D0_3B18_4580_BC7D_C675F4866D9A__INCLUDED_)


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


// Config.cpp: implementation of the CConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Luzj_ZTE.h"
#include "Config.h"
#include "aes.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CConfig Config;

CConfig::CConfig()
{
	
}

CConfig::~CConfig()
{

}

//取得路径
void CConfig::GetFullPathToFile(char  *pszFullPath,char * pszFilename)
{
	GetModuleFileName(GetModuleHandle(NULL), pszFullPath, MAX_STRING);
	strcpy(strrchr(pszFullPath, '\\') + 1, pszFilename);
}


void CConfig::SaveConfig()
{
	//保存配置到文件中去
	char pszFullPath[MAX_STRING];
	char pszFilename[MAX_STRING]=CONFIGNAME;
	char szTemp[MAX_STRING];
	GetFullPathToFile(pszFullPath,pszFilename);
	
	//LOAD_CONFIG_INT_NAME_VAR("HeartInterval", m_iHeartInterval, 30);
#define WRITE_CONFIG_INT_NAME_VAR(n, v)  {sprintf(szTemp,"%d",v);WritePrivateProfileString("config", n, szTemp, pszFullPath);}
	WRITE_CONFIG_INT_NAME_VAR("Timeout", m_iTimeout);
	WRITE_CONFIG_INT_NAME_VAR("HeartInterval", m_iHeartInterval);	

#define WRITE_CONFIG_BOOL_NAME_VAR(n, v)  WritePrivateProfileString("config", n, v?"1":"0", pszFullPath);

	WRITE_CONFIG_BOOL_NAME_VAR("RememberPWD", m_bRememberPWD);
	WRITE_CONFIG_BOOL_NAME_VAR("WebAuth", m_bWebAuth);
	WRITE_CONFIG_BOOL_NAME_VAR("WebLogout", m_bWebLogout);
	WRITE_CONFIG_BOOL_NAME_VAR("Autorun", m_bAutorun);
	WRITE_CONFIG_BOOL_NAME_VAR("Autologon", m_bAutologon);
	WRITE_CONFIG_BOOL_NAME_VAR("ShowBubble", m_bShowBubble);
	WRITE_CONFIG_BOOL_NAME_VAR("EnableWebAccount", m_bEnableWebAccount);
	WRITE_CONFIG_BOOL_NAME_VAR("Reauth", m_bReauth);
	WRITE_CONFIG_BOOL_NAME_VAR("AutoUpdate", m_bAutoUpdate);
	WRITE_CONFIG_BOOL_NAME_VAR("AutoFilter", m_bAutoFilter);
	WRITE_CONFIG_BOOL_NAME_VAR("Debug", m_bDebug);
	WRITE_CONFIG_BOOL_NAME_VAR("DHCP", m_bDHCP);
	WRITE_CONFIG_BOOL_NAME_VAR("HttpHeart", m_bHttpHeart);

	HKEY hRun;
	LONG kResult = ::RegOpenKeyEx(	HKEY_CURRENT_USER ,
									"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
									NULL ,KEY_ALL_ACCESS ,&hRun);
	if (m_bAutorun == TRUE)		//设置开机自动启动
	{
		char szTemp[MAX_STRING],pjPath[MAX_STRING];

		DWORD regsz=REG_SZ;
		DWORD iPathLen;

		strcpy(pszFilename,AfxGetApp()->m_pszExeName);
		strcat(pszFilename,".exe");
		GetFullPathToFile(szTemp,pszFilename);
		sprintf(pjPath,"\"%s\"",szTemp);
	
		iPathLen = (strlen(szTemp) +1) *sizeof(char);
		kResult =::RegQueryValueEx(hRun ,"zte",NULL ,&regsz ,(BYTE *)szTemp ,&iPathLen);
		if (kResult!=ERROR_SUCCESS || strcmp(szTemp,pjPath)!=0)
		{
			iPathLen = (strlen(pjPath) +1) *sizeof(char);
			kResult =::RegSetValueEx(hRun ,"zte",NULL ,REG_SZ ,(BYTE *)pjPath ,iPathLen);
		}
	}
	else
	{
		RegDeleteValue(hRun,"zte");
	}
	::RegCloseKey(hRun);

#define WRITE_CONFIG_STRING_NAME_VAR(n, v)  WritePrivateProfileString("config", n, v, pszFullPath);

	WRITE_CONFIG_STRING_NAME_VAR("HeartUrl", m_csHeartUrl);
	WRITE_CONFIG_STRING_NAME_VAR("HeartCookies", m_csHeartCookies);
	WRITE_CONFIG_STRING_NAME_VAR("ReauthTime", m_csReauthTime);
	WRITE_CONFIG_STRING_NAME_VAR("netcard", m_csNetCard);
	WRITE_CONFIG_STRING_NAME_VAR("WebAuthUrl", m_csWebAuthUrl);
	WRITE_CONFIG_STRING_NAME_VAR("WebLogoutUrl", m_csWebLogoutUrl);
	WRITE_CONFIG_STRING_NAME_VAR("LastUser", m_csLastUser);
	WRITE_CONFIG_STRING_NAME_VAR("WebUsername", m_csWebUsername);
	WRITE_CONFIG_STRING_NAME_VAR("WebPassword", aes_encrypt((LPCTSTR)m_csWebPassword));

#define WRITE_USER(u, p)  WritePrivateProfileString("users", u, aes_encrypt((LPCTSTR)p), pszFullPath);
	//读取所有账号密码参数	
	CString user,pass;	
	POSITION p = Config.m_UserInfo.GetStartPosition();
	while(p != NULL) {
		Config.m_UserInfo.GetNextAssoc(p, user, pass);		
		if(user.GetLength() > 0) WRITE_USER(user, pass);			
	}


}
void CConfig::LoadConfig()
{

	//从配置文件中读取设置
	char pszFullPath[MAX_STRING];
	char pszFilename[MAX_STRING]=CONFIGNAME;
	GetFullPathToFile(pszFullPath,pszFilename);
	
#define LOAD_CONFIG_INT_NAME_VAR(n, v, d) v=(GetPrivateProfileInt("config",n,d,pszFullPath));

	LOAD_CONFIG_INT_NAME_VAR("Timeout", m_iTimeout, 10);
	LOAD_CONFIG_INT_NAME_VAR("HeartInterval", m_iHeartInterval, 30);

#define LOAD_CONFIG_BOOL_NAME_VAR(n, v, d) v=(GetPrivateProfileInt("config",n,d,pszFullPath)==1);

	LOAD_CONFIG_BOOL_NAME_VAR("RememberPWD", m_bRememberPWD, 1);
	LOAD_CONFIG_BOOL_NAME_VAR("WebAuth", m_bWebAuth, 1);
	LOAD_CONFIG_BOOL_NAME_VAR("WebLogout", m_bWebLogout, 1);
	LOAD_CONFIG_BOOL_NAME_VAR("Autorun", m_bAutorun, 1);
	LOAD_CONFIG_BOOL_NAME_VAR("Autologon", m_bAutologon, 0);
	LOAD_CONFIG_BOOL_NAME_VAR("ShowBubble", m_bShowBubble, 1);
	LOAD_CONFIG_BOOL_NAME_VAR("EnableWebAccount", m_bEnableWebAccount, 0);
	LOAD_CONFIG_BOOL_NAME_VAR("AutoUpdate", m_bAutoUpdate, 0);
	LOAD_CONFIG_BOOL_NAME_VAR("Debug", m_bDebug, 1);
	LOAD_CONFIG_BOOL_NAME_VAR("AutoFilter", m_bAutoFilter, 1);
	LOAD_CONFIG_BOOL_NAME_VAR("DHCP", m_bDHCP, 1);
	LOAD_CONFIG_BOOL_NAME_VAR("HttpHeart", m_bHttpHeart, 1);
	
	//读取所有账号密码参数
	char szTemp[MAX_STRING];
	Config.m_UserInfo.RemoveAll();
	GetPrivateProfileString("users",NULL, NULL, szTemp, MAX_STRING, pszFullPath);
	char user[MAX_STRING], pass[MAX_STRING];	
	char *p = szTemp;
	if(*p == '\0') p++;
	while(*p) {
		strncpy(user,p, MAX_STRING);
		GetPrivateProfileString("users", user, "", pass, MAX_STRING,pszFullPath);
		char *szPass = aes_decrypt(pass);
		if(szPass == NULL) Config.m_UserInfo[user] = "";
		else Config.m_UserInfo[user] = aes_decrypt(pass);
		while(*p++);
	}

#define LOAD_CONFIG_STRING_NAME_VAR(n, v, d) {char t[MAX_STRING];GetPrivateProfileString("config",n,d,t,MAX_STRING,pszFullPath);v=t;}

	LOAD_CONFIG_STRING_NAME_VAR("HeartUrl", m_csHeartUrl, "http://bbs.gxgk.cc/index.php");
	LOAD_CONFIG_STRING_NAME_VAR("HeartCookies", m_csHeartCookies, "");
	LOAD_CONFIG_STRING_NAME_VAR("LastUser", m_csLastUser, "");
	LOAD_CONFIG_STRING_NAME_VAR("netcard", m_csNetCard, "");
	LOAD_CONFIG_STRING_NAME_VAR("WebAuthUrl", m_csWebAuthUrl, "http://125.88.59.131:10001/login.do?edubas=113.98.13.29");
	LOAD_CONFIG_STRING_NAME_VAR("WebLogoutUrl", m_csWebLogoutUrl, "http://125.88.59.131:10001/Logout.do?edubas=113.98.13.29");
	LOAD_CONFIG_STRING_NAME_VAR("WebUsername", m_csWebUsername, "");
	LOAD_CONFIG_STRING_NAME_VAR("WebPassword", m_csWebPassword, "");m_csWebPassword = CString(aes_decrypt(m_csWebPassword));
	LOAD_CONFIG_STRING_NAME_VAR("ReauthTime", m_csReauthTime, "");
}

void CConfig::LoadDefaultConfig()
{

}

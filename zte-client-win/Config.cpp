
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





CConfig::CConfig():m_UserInfo(16)
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
	
	sprintf(szTemp,"%d",m_iTimeout);
	WritePrivateProfileString("config","Timeout",szTemp,pszFullPath);

	//保存是否自动保存密码
	WritePrivateProfileString("config","RememberPWD",m_bRememberPWD==TRUE?"1":"0",pszFullPath);

	//保存是否网页认证
	WritePrivateProfileString("config","WebAuth",m_bWebAuth==TRUE?"1":"0",pszFullPath);
	
	//保存是否网页注销
	WritePrivateProfileString("config","WebLogout",m_bWebLogout==TRUE?"1":"0",pszFullPath);
	
	//保存是否开机自动运行
	WritePrivateProfileString("config","Autorun",m_bAutorun==TRUE?"1":"0",pszFullPath);
	
	//保存是否自动登录	
	WritePrivateProfileString("config","Autologon",m_bAutologon==TRUE?"1":"0",pszFullPath);
	//气泡提示
	WritePrivateProfileString("config","ShowBubble",m_bShowBubble==TRUE?"1":"0",pszFullPath);
	//启用网页认证帐号
	WritePrivateProfileString("config","EnableWebAccount",m_bEnableWebAccount==TRUE?"1":"0",pszFullPath);
	//启用重新认证时间
	WritePrivateProfileString("config","Reauth",m_bReauth ==TRUE?"1":"0",pszFullPath);
	//启用自动更新
	WritePrivateProfileString("config","AutoUpdate",m_bAutoUpdate==TRUE?"1":"0",pszFullPath);

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


	//重新认证时间
	WritePrivateProfileString("config","ReauthTime",m_csReauthTime,pszFullPath);

	//保存首选的网卡
	WritePrivateProfileString("config","netcard",m_csNetCard,pszFullPath);

	//保存网页认证地址
	WritePrivateProfileString("config","WebAuthUrl",m_csWebAuthUrl,pszFullPath);

	//保存网页注销地址
	WritePrivateProfileString("config","WebLogoutUrl",m_csWebLogoutUrl,pszFullPath);
	
	//读取所有账号密码参数	
	CString user,pass;	
	POSITION p = Config.m_UserInfo.GetStartPosition();
	while(p != NULL) {		
		Config.m_UserInfo.GetNextAssoc(p, user, pass);		
		if(user.GetLength() > 0)
			WritePrivateProfileString("users", user, aes_encrypt((LPCTSTR)pass), pszFullPath);		
	}	

	WritePrivateProfileString("config","LastUser",m_csLastUser,pszFullPath);

	WritePrivateProfileString("config","WebUsername",m_csWebUsername,pszFullPath);
	WritePrivateProfileString("config","WebPassword",aes_encrypt((LPCTSTR)m_csWebPassword),pszFullPath);

}
void CConfig::LoadConfig()
{

	//从配置文件中读取设置
	char pszFullPath[MAX_STRING];
	char pszFilename[MAX_STRING]=CONFIGNAME;
	GetFullPathToFile(pszFullPath,pszFilename);
	

	int retCode;
	m_iTimeout=GetPrivateProfileInt("config","Timeout",180,pszFullPath);

	//读取是否自动保存密码,默认为是
	retCode=GetPrivateProfileInt("config","RememberPWD",1,pszFullPath);
	m_bRememberPWD=(retCode==1?TRUE:FALSE);

	//读取是否进行网页认证,默认为是
	retCode=GetPrivateProfileInt("config","WebAuth",1,pszFullPath);
	m_bWebAuth=(retCode==1?TRUE:FALSE);

	//读取是否进行网页注销,默认为是
	retCode=GetPrivateProfileInt("config","WebLogout",0,pszFullPath);
	m_bWebLogout=(retCode==1?TRUE:FALSE);

	//读取是否开机自动运行,默认为否
	retCode=GetPrivateProfileInt("config","Autorun",0,pszFullPath);
	m_bAutorun=(retCode==1?TRUE:FALSE);

	//读取是否自动登录
	retCode=GetPrivateProfileInt("config","Autologon",0,pszFullPath);
	m_bAutologon=(retCode==1?TRUE:FALSE);


	retCode=GetPrivateProfileInt("config","ShowBubble",1,pszFullPath);
	m_bShowBubble=(retCode==1?TRUE:FALSE);

	retCode=GetPrivateProfileInt("config","EnableWebAccount",0,pszFullPath);
	m_bEnableWebAccount=(retCode==1?TRUE:FALSE);
	
	retCode=GetPrivateProfileInt("config","Reauth",0,pszFullPath);
	m_bReauth=(retCode==1?TRUE:FALSE);

	retCode=GetPrivateProfileInt("config","AutoUpdate",0,pszFullPath);
	m_bAutoUpdate=(retCode==1?TRUE:FALSE);



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
		Config.m_UserInfo[user] = aes_decrypt(pass);
		while(*p++);
	}

	//读取上次所选网卡的名字
	GetPrivateProfileString("config","LastUser","",szTemp,MAX_STRING,pszFullPath);
	m_csLastUser=szTemp;

	//读取上次所选网卡的名字
	GetPrivateProfileString("config","netcard","",szTemp,MAX_STRING,pszFullPath);
	m_csNetCard=szTemp;

	//读取网页认证地址
	GetPrivateProfileString("config","WebAuthUrl","http://125.88.59.131:10001/login.do?edubas=113.98.13.29",szTemp,MAX_STRING,pszFullPath);
	m_csWebAuthUrl=szTemp;

	//读取网页注销地址
	GetPrivateProfileString("config","WebLogoutUrl","http://125.88.59.131:10001/Logout.do?edubas=113.98.13.29",szTemp,MAX_STRING,pszFullPath);
	m_csWebLogoutUrl=szTemp;

	GetPrivateProfileString("config","WebUsername","",szTemp,MAX_STRING,pszFullPath);
	m_csWebUsername=szTemp;

	GetPrivateProfileString("config","WebPassword","",szTemp,MAX_STRING,pszFullPath);
	m_csWebPassword=aes_decrypt(szTemp);

	GetPrivateProfileString("config","ReauthTime","",szTemp,MAX_STRING,pszFullPath);
	m_csReauthTime=szTemp;

}
void CConfig::LoadDefaultConfig()
{

}

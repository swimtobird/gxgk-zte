
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
	m_bRememberPWD==TRUE?WritePrivateProfileString("config","RememberPWD","1",pszFullPath)
						:WritePrivateProfileString("config","RememberPWD","0",pszFullPath);
	//保存是否网页认证
	m_bWebAuth==TRUE?WritePrivateProfileString("config","WebAuth","1",pszFullPath)
						:WritePrivateProfileString("config","WebAuth","0",pszFullPath);
	//保存是否网页注销
	m_bWebLogout==TRUE?WritePrivateProfileString("config","WebLogout","1",pszFullPath)
						:WritePrivateProfileString("config","WebLogout","0",pszFullPath);
	
	//保存是否开机自动运行
	m_bAutorun==TRUE?WritePrivateProfileString("config","Autorun","1",pszFullPath)
					:WritePrivateProfileString("config","Autorun","0",pszFullPath);
	//保存是否自动登录
	m_bAutologon==TRUE?WritePrivateProfileString("config","Autologon","1",pszFullPath)
					  :WritePrivateProfileString("config","Autologon","0",pszFullPath);

	m_bShowBubble==TRUE?WritePrivateProfileString("config","ShowBubble","1",pszFullPath)
					  :WritePrivateProfileString("config","ShowBubble","0",pszFullPath);

	m_bEnableWebAccount ==TRUE?WritePrivateProfileString("config","EnableWebAccount","1",pszFullPath)
					  :WritePrivateProfileString("config","EnableWebAccount","0",pszFullPath);

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


	//保存首选的网卡
	WritePrivateProfileString("config","netcard",m_csNetCard,pszFullPath);

	//保存网页认证地址
	WritePrivateProfileString("config","WebAuthUrl",m_csWebAuthUrl,pszFullPath);

	//保存网页注销地址
	WritePrivateProfileString("config","WebLogoutUrl",m_csWebLogoutUrl,pszFullPath);
	
	//读取所有账号密码参数
	CString str="";
	userInfo user;
	int i=0,k=0;
	i=Config.m_UserInfo.GetCount();
	for (k=0;k<i;k++)
	{
		Config.m_UserInfo.Lookup(k,user);
		WritePrivateProfileString("config",user.user,user.pass,pszFullPath);
		if (k==0)
		{
			str=user.user;
		}
		else
		{
			str=user.user+"|"+str;
		}
	}

	WritePrivateProfileString("config","username",str,pszFullPath);

	WritePrivateProfileString("config","LastUser",m_csLastUser,pszFullPath);

	WritePrivateProfileString("config","WebUsername",m_csWebUsername,pszFullPath);
	WritePrivateProfileString("config","WebPassword",m_csWebPassword,pszFullPath);

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


	//读取所有账号密码参数
	char szTemp[MAX_STRING],szPass[MAX_STRING];
	GetPrivateProfileString("config","LastUser","",szTemp,MAX_STRING,pszFullPath);
	m_csLastUser=szTemp;
	GetPrivateProfileString("config","username","",szTemp,MAX_STRING,pszFullPath);
	char *seps= "|";
	char *token;
	int i=0,k=0;
	userInfo user;
	token = strtok(szTemp, seps);
	while( token != NULL )
	{
		GetPrivateProfileString("config",token,"",szPass,MAX_STRING,pszFullPath);
		user.user=token;
		user.pass=szPass;
		m_UserInfo.SetAt(i,user);
		token = strtok( NULL, seps );	
		i++;
	}


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
	m_csWebPassword=szTemp;

}
void CConfig::LoadDefaultConfig()
{

}

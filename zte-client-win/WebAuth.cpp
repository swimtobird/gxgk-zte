#include "stdafx.h"
#include "AutoUpdate.h"
#include "WebAuth.h"

#define MAX_BUFFER_SIZE 1024

int UTF8ToGBK(unsigned char * lpUTF8Str,unsigned char * lpGBKStr,int nGBKStrLen)
{
	wchar_t * lpUnicodeStr = NULL;
	int nRetLen = 0;

	if(!lpUTF8Str) //如果UTF8字符串为NULL则出错退出
		return 0;

	nRetLen = ::MultiByteToWideChar(CP_UTF8,0,(char *)lpUTF8Str,-1,NULL,NULL); //获取转换到Unicode编码后所需要的字符空间长度
	lpUnicodeStr = new WCHAR[nRetLen + 1]; //为Unicode字符串空间
	nRetLen = ::MultiByteToWideChar(CP_UTF8,0,(char *)lpUTF8Str,-1,lpUnicodeStr,nRetLen); //转换到Unicode编码
	if(!nRetLen) //转换失败则出错退出
		return 0;

	nRetLen = ::WideCharToMultiByte(CP_ACP,0,lpUnicodeStr,-1,NULL,NULL,NULL,NULL); //获取转换到GBK编码后所需要的字符空间长度

	if(!lpGBKStr) //输出缓冲区为空则返回转换后需要的空间大小
	{
		if(lpUnicodeStr)
		delete []lpUnicodeStr;
		return nRetLen;
	}

	if(nGBKStrLen < nRetLen) //如果输出缓冲区长度不够则退出
	{
		if(lpUnicodeStr)
		delete []lpUnicodeStr;
		return 0;
	}

	nRetLen = ::WideCharToMultiByte(CP_ACP,0,lpUnicodeStr,-1,(char *)lpGBKStr,nRetLen,NULL,NULL); //转换到GBK编码

	if(lpUnicodeStr)
		delete []lpUnicodeStr;

	return nRetLen;
}

char * WebAuth(const char * username, const char *password, const char *ip, const char *base_url)
{	
	static char msg[MAX_BUFFER_SIZE];
	char url[MAX_BUFFER_SIZE] = {0};
	
	static char http[MAX_BUFFER_SIZE];
#ifdef WIN32
#define snprintf _snprintf
#endif
	snprintf(url, MAX_BUFFER_SIZE, "%s&eduuser=%s&userName1=%s&password1=%s", base_url, ip, username, password);

	DownLoadFileToBuffer(http, MAX_BUFFER_SIZE, url, ip);
	
#define LOCATE_STR "var alertInfo = \""
	char *p = strstr(http, LOCATE_STR);
	if(p == NULL) return http;
	char *q = strstr(p, "\";");
	if(q == NULL) return http;
	p += strlen(LOCATE_STR); *q ='\0';	
	//utf-8 to ascii
	//wcstombs(msg, (WCHAR*)p, 20);
	UTF8ToGBK((unsigned char*)p, (unsigned char*)msg, MAX_BUFFER_SIZE);
	
	return msg;
}

char * WebLogout(const char *ip, const char *base_url)
{
	static char msg[MAX_BUFFER_SIZE];
	char url[MAX_BUFFER_SIZE] = {0};
	
	CURL *curl;
	CURLcode res;
	long retcode = 0;
	
	curl = curl_easy_init();
	if(curl) {	
		
#ifdef WIN32
#define snprintf _snprintf
#endif
		snprintf(url, MAX_BUFFER_SIZE, "%s&eduuser=%s", base_url, ip);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);//注销之后会自动转跳
		
		res = curl_easy_perform(curl);		
		if(res == CURLE_OK) {			
			res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &retcode); 
			if(res == CURLE_OK && retcode == 200) {				
				char *r;
				res = curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &r);				
				if(res == CURLE_OK && strstr(r, "/logoutsuccess.jsp")) {
					curl_easy_cleanup(curl);
					return NULL;
				} else if(res == CURLE_OK){
					strncpy(msg, "logout is redirected to ", MAX_BUFFER_SIZE);
					strcat(msg, r);					
				}
			}
		}
		if(res != CURLE_OK)		strncpy(msg, curl_easy_strerror(res), MAX_BUFFER_SIZE);
		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}
	
	return msg;
}

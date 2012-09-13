#include "stdafx.h"
#include "WebAuth.h"

#define MAX_BUFFER_SIZE 1024

char * WebAuth(const char * username, const char *password, const char *ip, const char *base_url)
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
		snprintf(url, MAX_BUFFER_SIZE, "%s&eduuser=%s&userName1=%s&password1=%s", base_url, ip, username, password);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);//认证之前会自动转跳
		res = curl_easy_perform(curl);		
		if(res == CURLE_OK) {			
			res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &retcode); 
			if(res == CURLE_OK && retcode == 200) {				
				char *r;
				res = curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &r);				
				if(res == CURLE_OK && strstr(r, "/success.jsp")) {
					curl_easy_cleanup(curl);
					return NULL;
				}
			}
		}
		strncpy(msg, curl_easy_strerror(res), MAX_BUFFER_SIZE);
		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}
	
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

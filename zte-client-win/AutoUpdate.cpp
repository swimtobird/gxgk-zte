#include "stdafx.h"
#include "Define.h"
#include "AutoUpdate.h"

#define FILETIME_LENGTH	64

static size_t write_file_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	int written = fwrite(ptr, size, nmemb, (FILE *)stream);
	return written;
}

static size_t write_buffer_data(void *ptr, size_t size, size_t nmemb, void *buffer)
{
	Buffer *buf = (Buffer*)buffer;
	if(buf->offset + size >= buf->size) return 0;
	memcpy(buf->buffer + buf->offset, ptr, size * nmemb);	
	buf->offset += size * nmemb;
	return size;
}

FileInfo *GetFileModifyTime()
{
	static FileInfo info;
	char filepath[MAX_PATH] = {0};
	info.filesize = 0; info.modifytime = 0;

	if(!GetModuleFileName(NULL, filepath, MAX_PATH)) return NULL;

	HANDLE hFile = CreateFile(filepath, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, 0, NULL);
	if(hFile == NULL) return NULL;

	FILETIME LastModifyTime;

	LARGE_INTEGER largeInt;
	if(GetFileTime(hFile, NULL, NULL, &LastModifyTime)) {		
		largeInt.QuadPart =  ((DWORD64)LastModifyTime.dwHighDateTime << 32) + LastModifyTime.dwLowDateTime;
		info.modifytime = (long)((largeInt.QuadPart - 116444736000000000) /  10000000);
	}
	
	info.filesize = GetFileSize(hFile, NULL);
	
	CloseHandle(hFile);
	
	return &info;
}

int AutoUpdate()
{
	FileInfo *remote_info = GetHttpModifyTime(GOOGLE_CODE_URL_ZTE);
	FileInfo *local_info = GetFileModifyTime();
	if(local_info == NULL || remote_info == NULL) return -1;
	if(local_info->modifytime < remote_info->modifytime) {
		char msg[1024] = "发现新版本，立即进行更新吗？\r\n\r\n";
		char szTime[1024];
		strftime(szTime,1024,"%Y-%m-%d %H:%M:%S",localtime(&remote_info->modifytime));	
		strcat(msg, "新版本信息:\r\n文件时间："); strcat(msg, szTime); strcat(msg, "\r\n文件大小：");
		itoa(remote_info->filesize, szTime, 10); strcat(msg, szTime); strcat(msg, "字节\r\n\r\n");

		strftime(szTime,1024,"%Y-%m-%d %H:%M:%S",localtime(&local_info->modifytime));	
		strcat(msg, "当前版本信息:\r\n文件时间："); strcat(msg, szTime); strcat(msg, "\r\n文件大小：");
		itoa(local_info->filesize, szTime, 10); strcat(msg, szTime); strcat(msg, "字节\r\n");
		
		if(IDYES != AfxMessageBox(msg, MB_YESNO | MB_ICONINFORMATION)) return 1;
		
		char szTempPath[MAX_PATH], szTempName[MAX_PATH];
		GetTempPath(MAX_PATH, szTempPath);
		GetTempFileName(szTempPath, "zte", 0, szTempName);
		strcat(szTempName, ".exe");

		if(DownLoadFile(szTempName, GOOGLE_CODE_URL_ZTE) != 0) return -2;
		if(IDOK != AfxMessageBox("更新认证客户端期间可能短暂无法使用网络，请备份好数据，然后按“确定”开始更新。",
			MB_OK | MB_ICONINFORMATION)) {
			return 2;
		}

		char filepath[MAX_PATH] = {0};
		char parameters[MAX_PATH] = "-update \"";
		if(!GetModuleFileName(NULL, filepath, MAX_PATH)) return -1;
		strcat(parameters, filepath);

		SHELLEXECUTEINFO ShExecInfo = {0};	
		ShExecInfo.cbSize	= sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask		= SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.lpFile		= szTempName;	
		ShExecInfo.lpParameters = _T(parameters);
		ShExecInfo.nShow	= SW_HIDE;
		
		if(!ShellExecuteEx(&ShExecInfo)) {
			AfxMessageBox("启动更新程序失败！");			
			return 3;
		}
		Sleep(1000);
		
		return 0;
	} 
	//UpdateFile(GOOGLE_CODE_URL_ZTE, GetFileModifyTime());
	return 4;
}

int DownLoadFile(const char *savename, const char *url, const char *ip)
{
	FILE *file = NULL;
	file = fopen(savename, "wb");
	if(file == NULL) return 1;

	CURL *curl;
	CURLcode res;
	long retcode = 0;
				
	curl = curl_easy_init();
	if(curl) {	
		curl_easy_setopt(curl, CURLOPT_URL, url);		
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		if(ip) curl_easy_setopt(curl, CURLOPT_INTERFACE, ip);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);//认证之前会自动转跳
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_file_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)file);
		res = curl_easy_perform(curl);
		/*
		if(res == CURLE_OK) {			
			res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &retcode); 
			if(res == CURLE_OK && retcode == 200) {				
				
			}
		}
		*/
		
		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}
	fclose(file);

	return 0;
}

int DownLoadFileToBuffer(char *buffer, int size, const char *url, const char *ip)
{
	CURL *curl;
	CURLcode res;
	long retcode = 0;
	Buffer buf;

	buf.buffer = buffer;
	buf.size = size;
	buf.offset = 0;	
	
	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);	
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);			
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");	
		if(ip) curl_easy_setopt(curl, CURLOPT_INTERFACE, ip);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);//认证之前会自动转跳
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_buffer_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&buf);
		res = curl_easy_perform(curl);		
		/*
		if(res == CURLE_OK) {			
			res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &retcode); 
			if(res == CURLE_OK && retcode == 200) {				
				
			}
		}
		*/
		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}
	return buf.offset;
}

FileInfo *GetHttpModifyTime(const char* url)
{
	static FileInfo fileinfo;
	
	double size;
	CURL *curl;
	CURLcode res;
	long retcode = 0;
	fileinfo.filesize = 0; fileinfo.modifytime = 0;
			
	curl = curl_easy_init();
	if(curl) {	
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
		curl_easy_setopt(curl, CURLOPT_FILETIME, 1);		
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);//认证之前会自动转跳
		res = curl_easy_perform(curl);
		if(res == CURLE_OK) {	
			res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &retcode); 
			if(res == CURLE_OK && retcode == 200) {				
				res = curl_easy_getinfo(curl, CURLINFO_FILETIME, &fileinfo.modifytime);				
				if(res != CURLE_OK) fileinfo.modifytime = 0;
				res = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &size);
				if(res != CURLE_OK) fileinfo.filesize = 0;
				else fileinfo.filesize = (int)size;
				curl_easy_cleanup(curl);
				return &fileinfo;
			}
		}
		
		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}
	
	return NULL;
}


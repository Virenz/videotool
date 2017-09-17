#pragma once

#include "httptool.h"
#include <regex>
#include <string>
#include <queue>
#include <vector>

typedef struct tagVideoInfo
{
	int							totalNum;				//����
	std::wstring				name;					//Ƭ��
	std::vector<std::wstring>	resLinks;				//��Դ����
}VideoInfo;

class MagParse
{
public:
	MagParse();
	~MagParse();

	std::wstring GetHtmlPage(char* url, int url_id);
	std::vector<VideoInfo> GetVideoInfos(char* search_name);

	std::string FormatString(const char * lpcszFormat, ...);
	BOOL UrlEncode(const char* szSrc, char* pBuf, int cbBufLen, BOOL bUpperCase);
	BOOL UrlDecode(const char* szSrc, char* pBuf, int cbBufLen);
	void StringToWstring(std::wstring& szDst, std::string& str);

	std::vector<VideoInfo>& getMags();

private:
	std::vector<VideoInfo> mags;
	const char *search_url[2] = {
		"http://so.tv.sohu.com/mts?box=1&wd=%s",
		"https://v.qq.com/x/search/?q=%s"
	};
};
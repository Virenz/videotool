#ifndef __MAG_PARSE_H__
#define __MAG_PARSE_H__
#include "httptool.h"
#include <regex>
#include <string>
#include <map>

// 定义Key长度排序
struct CmpByKeyLength {
	bool operator()(const std::wstring& k1, const std::wstring& k2) {
		return true;
	}
};

typedef struct tagVideoInfo
{
	int							totalNum;				//集数
	std::string					img;					//图片
	std::wstring				name;					//片名
	std::map<std::wstring, std::wstring, CmpByKeyLength>	resLinks;				//资源名称
}VideoInfo;

class MagParse
{
public:
	MagParse();
	~MagParse();

	std::wstring GetHtmlPage(char* url, int url_id);
	std::wstring GetHtmlPage(const char* url);
	std::vector<VideoInfo> GetVideoInfos(char* search_name);

	std::string FormatString(const char * lpcszFormat, ...);
	BOOL UrlEncode(const char* szSrc, char* pBuf, int cbBufLen, BOOL bUpperCase);
	BOOL UrlDecode(const char* szSrc, char* pBuf, int cbBufLen);
	void StringToWstring(std::wstring& szDst, std::string& str);
	void WstringToString(std::string& szDst, std::wstring& wchar);

	std::vector<VideoInfo> getMags();

private:
	std::vector<VideoInfo> mags;
	const char *search_url[4] = {
		"http://so.tv.sohu.com/mts?box=1&wd=%s",
		"https://v.qq.com/x/search/?q=%s",
		"http://www.soku.com/search_video/q_%s?f=1",
		"http://so.iqiyi.com/so/q_%s"
	};
};

#endif  // MAG_PARSE_H_
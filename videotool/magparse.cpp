#include "magparse.h"

MagParse::MagParse()
{
}

MagParse::~MagParse()
{
}

std::vector<VideoInfo> MagParse::getMags()
{
	// TODO: �ڴ˴����� return ���
	return this->mags;
}

std::wstring MagParse::GetHtmlPage(char* url, int url_id)
{
	std::wstring magunicodeHtml;

	int len = 0;
	char pBuf[256];
	BOOL isSucess = UrlEncode(url, pBuf, 256, true);
	if (!isSucess)
	{
		return magunicodeHtml;
	}
	std::string searcher_url = FormatString(search_url[url_id], pBuf);

	HttpTool* httpTool = new HttpTool();
	//http://so.tv.sohu.com/mts?box=1&wd=%E8%A1%80%E6%97%8F
	//https://v.qq.com/x/search/?q=%E6%97%A0%E5%BF%83%E6%B3%95%E5%B8%882&stag=0
	bool isGet = httpTool->httpGet(searcher_url.c_str());
	if (!isGet)
		return magunicodeHtml;

	StringToWstring(magunicodeHtml, std::string(httpTool->getReponseHTML()));
	delete httpTool;

	return magunicodeHtml;
}

//��ȡ��Ƶ��Ϣ
std::vector<VideoInfo> MagParse::GetVideoInfos(char * search_name)
{
	/*
	GetHtmlPage ���������ؼ��ֺ�url_id(��ʾʹ�õ���Ƶ��վ�ӿ�)
	0: �Ѻ���Ƶ
	1: ��Ѷ��Ƶ
	*/

	BOOL isSearch = FALSE;

	std::wstring html;
	int nPos = 0;
	int n = 0;

	////-----------------------ʹ���Ѻ���Ƶ��������-----------------------------------
	//html = GetHtmlPage(search_name, 0);

	//while ((nPos = html.find(L"data-title", n)) != -1)
	//{
	//	isSearch = TRUE;
	//	// Ϊ��������find����λ��
	//	n = nPos + 1;

	//	VideoInfo vdinfo;

	//	// ��ȡ��Ƶ����
	//	int nStartPos = nPos + 12;
	//	int nEndPos = html.find(L"\"", nStartPos);
	//	std::wstring strname = html.substr(nStartPos, nEndPos - nStartPos);
	//	vdinfo.name = strname;

	//	// ��ȡ��Ƶ����Դ���
	//	nStartPos = html.rfind(L"series cfix", nPos);
	//	if (nStartPos < 0)	// ��ȡ��Ӱ
	//	{
	//		nStartPos = html.find(L"data-url", nPos);
	//		nStartPos = nStartPos + 10;
	//		nEndPos = html.find(L"\"", nStartPos);
	//		std::wstring strreslink = html.substr(nStartPos, nEndPos - nStartPos);
	//		vdinfo.resLinks.push_back(strreslink.insert(0, L"http:"));
	//	}
	//	else //��ȡ���Ӿ�
	//	{
	//		nEndPos = html.find(L"</div>", nStartPos);
	//		std::wstring contentHtml = html.substr(nStartPos + 1, nEndPos - nStartPos);

	//		// �������ȡ����Ƶ����
	//		const std::wregex pattern(L"//tv.sohu.com/[0-9]{8}/\\w{10}.shtml");
	//		std::wsmatch result;

	//		for (std::wsregex_iterator it(contentHtml.begin(), contentHtml.end(), pattern), end;     //end��β���������regex_iterator��regex_iterator��string���͵İ汾
	//			it != end;
	//			++it)
	//		{
	//			vdinfo.resLinks.push_back(it->str().insert(0, L"http:"));
	//		}
	//	}

	//	// ��ȡ��Ƶ����
	//	vdinfo.totalNum = vdinfo.resLinks.size();

	//	mags.push_back(vdinfo);
	//}
	//// ����ʹ�ñ���
	//html.clear();
	//nPos = 0;
	//n = 0;

	//if (isSearch)
	//{
	//	return mags;
	//}

	// -----------------------ʹ����Ѷ��Ƶ��������-----------------------------------
	html = GetHtmlPage(search_name, 1);

	while ((nPos = html.find(L"_playlist", n)) != -1)
	{
		isSearch = TRUE;
		// Ϊ��������find����λ��
		n = nPos + 1;

		VideoInfo vdinfo;

		// ��ȡ�ؼ���������������ݿ�
		int nEndPos = html.find(L"tip_download", nPos);
		std::wstring strreslink = html.substr(n, nEndPos - n);

		// �������δչ���ı�ǩ����Ҫ��������
		if(strreslink.find(L"video:poster_num_unfold") != -1)
		{
		}
		else
		{
			// ��ȡ��Ƶ����
			int nStartPos = strreslink.find(L"title: \'", 0);
			if (nStartPos != -1)
			{
				nStartPos = nStartPos + 8; // 8(title: ')
				nEndPos = strreslink.find(L"\'", nStartPos);
				std::wstring strname = strreslink.substr(nStartPos, nEndPos - nStartPos);
				vdinfo.name = strname;

				// �������ȡ����Ƶ����
				const std::wregex pattern(L"href=\"(((http|ftp|https)://)(([a-zA-Z0-9\._-]+\.[a-zA-Z]{2,6})|([0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}))(:[0-9]{1,4})*(/[a-zA-Z0-9\&%_\./-~-]*)?)([^\.]*)video:poster_(num|play)([^\.]*|[^\u4e00-\u9fa5]*)>([0-9\u4e00-\u9fa5]{1,})<");
				std::wsmatch result;

				for (std::wsregex_iterator it(strreslink.begin(), strreslink.end(), pattern), end;     //end��β���������regex_iterator��regex_iterator��string���͵İ汾
					it != end;
					++it)
				{
					vdinfo.resLinks.insert(std::pair<std::wstring, std::wstring>((*it)[(*it).size() - 1].str(), (*it)[1].str()));
					//insert(std::pair<std::string, std::string>(key,value));
				}

				// ��ȡ��Ƶ����
				vdinfo.totalNum = vdinfo.resLinks.size();

				mags.push_back(vdinfo);
			}
		}
		
	}
	// ����ʹ�ñ���
	html.clear();
	nPos = 0;
	n = 0;

	if (isSearch)
	{
		return mags;
	}

	return mags;
}

std::string MagParse::FormatString(const char * lpcszFormat, ...)
{
	char *pszStr = NULL;
	if (NULL != lpcszFormat)
	{
		va_list marker = NULL;
		va_start(marker, lpcszFormat); //��ʼ����������  
		size_t nLength = _vscprintf(lpcszFormat, marker) + 1; //��ȡ��ʽ���ַ�������  
		pszStr = new char[nLength];
		memset(pszStr, '\0', nLength);
		_vsnprintf_s(pszStr, nLength, nLength, lpcszFormat, marker);
		va_end(marker); //���ñ�������  
	}
	std::string strResult(pszStr);
	delete[]pszStr;
	return strResult;
}

BOOL MagParse::UrlEncode(const char* szSrc, char* pBuf, int cbBufLen, BOOL bUpperCase)
{
	if (szSrc == NULL || pBuf == NULL || cbBufLen <= 0)
		return FALSE;

	size_t len_ascii = strlen(szSrc);
	if (len_ascii == 0)
	{
		pBuf[0] = 0;
		return TRUE;
	}

	//��ת����UTF-8
	char baseChar = bUpperCase ? 'A' : 'a';
	int cchWideChar = MultiByteToWideChar(CP_ACP, 0, szSrc, len_ascii, NULL, 0);
	LPWSTR pUnicode = (LPWSTR)malloc((cchWideChar + 1) * sizeof(WCHAR));
	if (pUnicode == NULL)
		return FALSE;
	MultiByteToWideChar(CP_ACP, 0, szSrc, len_ascii, pUnicode, cchWideChar + 1);

	int cbUTF8 = WideCharToMultiByte(CP_UTF8, 0, pUnicode, cchWideChar, NULL, 0, NULL, NULL);
	LPSTR pUTF8 = (LPSTR)malloc((cbUTF8 + 1) * sizeof(CHAR));
	if (pUTF8 == NULL)
	{
		free(pUnicode);
		return FALSE;
	}
	WideCharToMultiByte(CP_UTF8, 0, pUnicode, cchWideChar, pUTF8, cbUTF8 + 1, NULL, NULL);
	pUTF8[cbUTF8] = '\0';

	unsigned char c;
	int cbDest = 0; //�ۼ�
	unsigned char *pSrc = (unsigned char*)pUTF8;
	unsigned char *pDest = (unsigned char*)pBuf;
	while (*pSrc && cbDest < cbBufLen - 1)
	{
		c = *pSrc;
		if (isalpha(c) || isdigit(c) || c == '-' || c == '.' || c == '~')
		{
			*pDest = c;
			++pDest;
			++cbDest;
		}
		else if (c == ' ')
		{
			*pDest = '+';
			++pDest;
			++cbDest;
		}
		else
		{
			//��黺������С�Ƿ��ã�
			if (cbDest + 3 > cbBufLen - 1)
				break;
			pDest[0] = '%';
			pDest[1] = (c >= 0xA0) ? ((c >> 4) - 10 + baseChar) : ((c >> 4) + '0');
			pDest[2] = ((c & 0xF) >= 0xA) ? ((c & 0xF) - 10 + baseChar) : ((c & 0xF) + '0');
			pDest += 3;
			cbDest += 3;
		}
		++pSrc;
	}
	//null-terminator
	*pDest = '\0';
	free(pUnicode);
	free(pUTF8);
	return TRUE;
}

//�������utf-8����
BOOL MagParse::UrlDecode(const char* szSrc, char* pBuf, int cbBufLen)
{
	if (szSrc == NULL || pBuf == NULL || cbBufLen <= 0)
		return FALSE;

	size_t len_ascii = strlen(szSrc);
	if (len_ascii == 0)
	{
		pBuf[0] = 0;
		return TRUE;
	}

	char *pUTF8 = (char*)malloc(len_ascii + 1);
	if (pUTF8 == NULL)
		return FALSE;

	int cbDest = 0; //�ۼ�
	unsigned char *pSrc = (unsigned char*)szSrc;
	unsigned char *pDest = (unsigned char*)pUTF8;
	while (*pSrc)
	{
		if (*pSrc == '%')
		{
			*pDest = 0;
			//��λ
			if (pSrc[1] >= 'A' && pSrc[1] <= 'F')
				*pDest += (pSrc[1] - 'A' + 10) * 0x10;
			else if (pSrc[1] >= 'a' && pSrc[1] <= 'f')
				*pDest += (pSrc[1] - 'a' + 10) * 0x10;
			else
				*pDest += (pSrc[1] - '0') * 0x10;

			//��λ
			if (pSrc[2] >= 'A' && pSrc[2] <= 'F')
				*pDest += (pSrc[2] - 'A' + 10);
			else if (pSrc[2] >= 'a' && pSrc[2] <= 'f')
				*pDest += (pSrc[2] - 'a' + 10);
			else
				*pDest += (pSrc[2] - '0');

			pSrc += 3;
		}
		else if (*pSrc == '+')
		{
			*pDest = ' ';
			++pSrc;
		}
		else
		{
			*pDest = *pSrc;
			++pSrc;
		}
		++pDest;
		++cbDest;
	}
	//null-terminator
	*pDest = '\0';
	++cbDest;

	int cchWideChar = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pUTF8, cbDest, NULL, 0);
	LPWSTR pUnicode = (LPWSTR)malloc(cchWideChar * sizeof(WCHAR));
	if (pUnicode == NULL)
	{
		free(pUTF8);
		return FALSE;
	}
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pUTF8, cbDest, pUnicode, cchWideChar);
	WideCharToMultiByte(CP_ACP, 0, pUnicode, cchWideChar, pBuf, cbBufLen, NULL, NULL);
	free(pUTF8);
	free(pUnicode);
	return TRUE;
}

void MagParse::StringToWstring(std::wstring& szDst, std::string& str)
{
	std::string temp = str;
	int len = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)temp.c_str(), -1, NULL, 0);
	wchar_t * wszUtf8 = new wchar_t[len + 1];
	memset(wszUtf8, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)temp.c_str(), -1, (LPWSTR)wszUtf8, len);
	szDst = wszUtf8;
	delete[] wszUtf8;
}
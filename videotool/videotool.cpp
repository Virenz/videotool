#include <windows.h>
#include <tchar.h>
#include <thread>
#include <commctrl.h>
#include <gdiplus.h>
#include "resource.h"

#include "include/cef_sandbox_win.h"
#include "simple_app.h"
#include "simple_handler.h"
#include "jiekou.h"
#include "magparse.h"

#pragma comment(lib, "libcef.lib")
#pragma comment(lib, "libcef_dll_wrapper.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void InitComBox(HWND hDlg);
void performActions(HWND hwnd);
int InitTreeControl(HWND hwnd, MagParse *uidatas);
void ClearTreeControl(HWND hwnd);
BOOL VideoLoadImage(HWND hDlg, const char* pszFileName);

ULONG_PTR m_gdiplusToken;
HBITMAP s_hBitmap = NULL;
CefRefPtr<SimpleApp>	app;
BOOL					m_bCEFInitialized;
std::multimap<std::string, std::string> jiexiurls;	// 记录接口
std::vector<void*>						lparams;	// 记录treeview.lparam临时申请空间的指针，便于进行释放

POINT oldRect;

// When generating projects with CMake the CEF_USE_SANDBOX value will be defined
// automatically if using the required compiler version. Pass -DUSE_SANDBOX=OFF
// to the CMake command-line to disable use of the sandbox.
// Uncomment this line to manually enable sandbox support.
//#define CEF_USE_SANDBOX 1

#if defined(CEF_USE_SANDBOX)
// The cef_sandbox.lib static library is currently built with VS2013. It may not
// link successfully with other VS versions.
#pragma comment(lib, "cef_sandbox.lib")
#endif

// Entry point function for all processes.
//WINAPI WinMain(HINSTANCE hThisApp, HINSTANCE hPrevApp, LPSTR lpCmd, int nShow)
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow) {

	// Init GDI+    
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Enable High-DPI support on Windows 7 or newer.
	CefEnableHighDPISupport();

	void* sandbox_info = NULL;

#if defined(CEF_USE_SANDBOX)
	// Manage the life span of the sandbox information object. This is necessary
	// for sandbox support on Windows. See cef_sandbox_win.h for complete details.
	CefScopedSandboxInfo scoped_sandbox;
	sandbox_info = scoped_sandbox.sandbox_info();
#endif

	// Provide CEF with command-line arguments.
	CefMainArgs main_args(hInstance);

	// SimpleApp implements application-level callbacks for the browser process.
	// It will create the first browser instance in OnContextInitialized() after
	// CEF has initialized.
	app = new SimpleApp;

	// CEF applications have multiple sub-processes (render, plugin, GPU, etc)
	// that share the same executable. This function checks the command-line and,
	// if this is a sub-process, executes the appropriate logic.
	int exit_code = CefExecuteProcess(main_args, NULL, sandbox_info);
	if (exit_code >= 0) {
		// The sub-process has completed so return here.
		return exit_code;
	}

	// Specify CEF global settings here.
	CefSettings settings;
	//settings.multi_threaded_message_loop = true;

#if !defined(CEF_USE_SANDBOX)
	settings.no_sandbox = true;
#endif

	HWND hdlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_VIDEO), GetDesktopWindow(), (DLGPROC)DlgProc);
	if (!hdlg)
	{
		return 0;
	}
	ShowWindow(hdlg, SW_SHOW);

	HWND hHTMLGroupBox = GetDlgItem(hdlg, IDC_BROWN);
	RECT htmlRect;
	WINDOWINFO wndInfo;
	GetWindowInfo(hdlg, &wndInfo);

	GetWindowRect(hHTMLGroupBox, &htmlRect);
	htmlRect.left -= wndInfo.rcClient.left;
	htmlRect.right -= wndInfo.rcClient.left;
	htmlRect.top -= wndInfo.rcClient.top;
	htmlRect.bottom -= wndInfo.rcClient.top;

	ShowWindow(hHTMLGroupBox, SW_HIDE);
	app.get()->SetParentWindow(hdlg, htmlRect);

	// Initialize CEF.
	m_bCEFInitialized = CefInitialize(main_args, settings, app.get(), sandbox_info);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message == WM_KEYDOWN && GetKeyState('A') && GetKeyState(VK_CONTROL))
		{
			//Edit_SetSel(GetDlgItem(hdlg, IDC_URL), 0, -1);
			SendMessage(GetDlgItem(hdlg, IDC_URL), EM_SETSEL, 0, -1);
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (m_bCEFInitialized)
		{
			CefDoMessageLoopWork();
		}
	}
	// delete GDI
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
	return 0;
}

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
	{
		HINSTANCE hInst = GetModuleHandle(NULL);
		SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON)));
		InitComBox(hDlg);
		// 获取初始化窗口大小
		RECT rect;
		GetClientRect(hDlg, &rect);
		oldRect.x = rect.right - rect.left;
		oldRect.y = rect.bottom - rect.top;
		break;
	}
	case WM_SIZE:
	{
		if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
		{

			INT nWidth = LOWORD(lParam);
			INT nHeight = HIWORD(lParam);

			float   ratio[2];
			POINT   newDialogSize;
			RECT    newRect;

			WINDOWINFO wndInfo;
			GetWindowInfo(hDlg, &wndInfo);

			//获取新的客户区的大小  
			GetClientRect(hDlg, &newRect);
			newDialogSize.x = newRect.right - newRect.left;
			newDialogSize.y = newRect.bottom - newRect.top;

			//得现在的对话框与以往对话框的大小比例  
			ratio[0] = (float)newDialogSize.x / oldRect.x;
			ratio[1] = (float)newDialogSize.y / oldRect.y;

			RECT rect;

			HWND hStaticSearch = GetDlgItem(hDlg, IDC_STATIC_SEARCH);
			GetWindowRect(hStaticSearch, &rect);
			MoveWindow(hStaticSearch,
				newDialogSize.x - wndInfo.rcClient.left - (oldRect.x - rect.left),
				rect.top - wndInfo.rcClient.top,
				rect.right - rect.left,
				(rect.bottom - rect.top)*ratio[1],
				TRUE);

			HWND hSearch = GetDlgItem(hDlg, IDC_SEARCH);
			GetWindowRect(hSearch, &rect);
			MoveWindow(hSearch,
				newDialogSize.x - wndInfo.rcClient.left - (oldRect.x - rect.left),
				rect.top - wndInfo.rcClient.top,
				rect.right - rect.left,
				rect.bottom - rect.top,
				TRUE);

			HWND hBtnStartSearch = GetDlgItem(hDlg, IDC_START_SEARCH);
			GetWindowRect(hBtnStartSearch, &rect);
			MoveWindow(hBtnStartSearch,
				newDialogSize.x - wndInfo.rcClient.left - (oldRect.x - rect.left),
				rect.top - wndInfo.rcClient.top,
				rect.right - rect.left,
				(rect.bottom - rect.top),
				TRUE);

			HWND hTreeVideoInfos = GetDlgItem(hDlg, IDC_VIDEOINFOS);
			GetWindowRect(hTreeVideoInfos, &rect);
			MoveWindow(hTreeVideoInfos,
				newDialogSize.x - wndInfo.rcClient.left - (oldRect.x - rect.left),
				rect.top - wndInfo.rcClient.top,
				rect.right - rect.left,
				(rect.bottom - rect.top)*ratio[1],
				TRUE);

			HWND hImageVideoImg = GetDlgItem(hDlg, IDC_VIDEOIMG);
			GetWindowRect(hImageVideoImg, &rect);
			MoveWindow(hImageVideoImg,
				newDialogSize.x - wndInfo.rcClient.left - (oldRect.x - rect.left),
				(rect.top - wndInfo.rcClient.top)*ratio[1],
				rect.right - rect.left,
				rect.bottom - rect.top,
				TRUE);

			HWND hWebBrown = GetDlgItem(hDlg, IDC_BROWN);
			HWND hBrowserWnd = app.get()->GetBrowserHostWnd();
			GetWindowRect(hWebBrown, &rect);
			MoveWindow(hWebBrown,
				rect.left - wndInfo.rcClient.left,
				rect.top - wndInfo.rcClient.top,
				(rect.right - rect.left)*ratio[0],
				(rect.bottom - rect.top)*ratio[1],
				TRUE);
			MoveWindow(hBrowserWnd,
				rect.left - wndInfo.rcClient.left,
				rect.top - wndInfo.rcClient.top,
				(rect.right - rect.left)*ratio[0],
				(rect.bottom - rect.top)*ratio[1],
				TRUE);

			oldRect = newDialogSize;
		}
		break;
	}
	case WM_CLOSE:
	{
		ClearTreeControl(hDlg);
		if (app != NULL)
		{
			if (app.get()->GetCefClient() != NULL)
			{
				app.get()->GetCefClient()->CloseAllBrowsers(true);
			}
		}
		if (m_bCEFInitialized)
		{
			// closing stop work loop
			m_bCEFInitialized = FALSE;
			// release CEF app
			app = NULL;
			CefShutdown();
		}
		PostQuitMessage(0);//退出
		//EndDialog(hDlg, IDCANCEL);
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
			case IDOK:
			{
				// 获取解析借口
				HWND combox = GetDlgItem(hDlg, IDC_INTERFACE);
				CHAR szBuff[200];
				ZeroMemory(szBuff, sizeof(szBuff));
				SendMessageA(combox, CB_GETLBTEXT, SendMessage(combox, CB_GETCURSEL, 0, 0), (LPARAM)szBuff);
				std::string jiexiurl = jiexiurls.find(szBuff)->second;

				// 获取需要播放视频的url
				ZeroMemory(szBuff, sizeof(szBuff));
				GetDlgItemTextA(hDlg, IDC_URL, szBuff, 200);

				//启动播放该视频
				std::string url;
				url.append(jiexiurl + szBuff);
				app.get()->PlayByCef(url);
				break;
			}
			case IDC_START_SEARCH:
			{
				// 清理视频信息数据
				ClearTreeControl(hDlg);

				std::thread action(performActions, hDlg);
				action.detach();
			}
			default:
				break;
		}
		break;
	}
	case WM_NOTIFY: {
		LPNMHDR lpnmh = (LPNMHDR)lParam;

		if (NM_DBLCLK == lpnmh->code)
		{
			DWORD dwPos = GetMessagePos();
			POINT pt;
			pt.x = LOWORD(dwPos);
			pt.y = HIWORD(dwPos);
			ScreenToClient(lpnmh->hwndFrom, &pt);
			TVHITTESTINFO ht = { 0 };
			ht.pt = pt;
			ht.flags = TVHT_ONITEM;
			HTREEITEM hItem = TreeView_HitTest(lpnmh->hwndFrom, &ht);
			TreeView_SelectItem(lpnmh->hwndFrom, hItem);
			TVITEM ti = { 0 };
			ti.mask = TVIF_TEXT | TVIF_PARAM | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS;
			TCHAR buf[MAX_PATH] = { 0 };
			int index = 0;
			ti.cchTextMax = MAX_PATH;
			ti.pszText = buf;
			ti.hItem = hItem;
		
			TreeView_GetItem(lpnmh->hwndFrom, &ti);

			if (!TreeView_GetChild(lpnmh->hwndFrom, hItem))
			{
				SetDlgItemText(hDlg, IDC_URL, (TCHAR*)(ti.lParam));
			}
			else
			{
				std::thread action(VideoLoadImage, hDlg, (char*)(ti.lParam));
				action.join();
			}
		}
	}
	default:
		break;
	}
	return (INT_PTR)FALSE;
}

void performActions(HWND hwnd)
{
	// 唤醒执行 按钮
	EnableWindow(GetDlgItem(hwnd, IDC_START_SEARCH), false);

	char search_str[MAX_PATH];
	GetDlgItemTextA(hwnd, IDC_SEARCH, search_str, MAX_PATH);

	MagParse* magParse = new MagParse();
	magParse->GetVideoInfos(search_str);
	InitTreeControl(hwnd, magParse);
	delete magParse;

	// 唤醒执行 按钮
	EnableWindow(GetDlgItem(hwnd, IDC_START_SEARCH), true);
}

void InitComBox(HWND hDlg)
{
	// 清空jiexiurls数据
	jiexiurls.clear();

	HWND combox = GetDlgItem(hDlg, IDC_INTERFACE);

	JieKou* jiekouurls = new JieKou();
	std::multimap<std::string, std::string>::iterator iter = jiekouurls->getJieKou().begin();
	while (iter != jiekouurls->getJieKou().end()) //#1
	{
		jiexiurls.insert(*iter);
		SendMessageA(combox, CB_ADDSTRING, 0, (LPARAM)iter->first.c_str());
		iter++;
	}
	SendMessage(combox, CB_SETCURSEL, 0, 0);
	delete jiekouurls;
}

int InitTreeControl(HWND hdlg, MagParse *uidatas)
{
	auto infos = uidatas->getMags();
	for (auto sp : infos)
	{
		TV_ITEM item;
		item.mask = TVIF_TEXT | TVIF_PARAM | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS;
		item.cchTextMax = MAX_PATH;
		item.pszText = (LPWSTR)sp.name.c_str();
		item.lParam = (LPARAM)_strdup(sp.img.c_str());

		lparams.push_back((void*)item.lParam);

		TV_INSERTSTRUCT insert;
		insert.hParent = TVI_ROOT;
		insert.hInsertAfter = TVI_LAST;
		insert.item = item;

		HWND m_tree = GetDlgItem(hdlg, IDC_VIDEOINFOS);
		HTREEITEM Selected = TreeView_InsertItem(m_tree, &insert);
		//for (int index = 0; index < sp.totalNum; index++) //遍历json成员
		for(std::multimap<std::wstring, std::wstring>::iterator iter = sp.resLinks.begin(); iter != sp.resLinks.end(); ++iter)
		{
			TV_ITEM item1;
			item1.mask = TVIF_TEXT | TVIF_PARAM | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS;
			item1.cchTextMax = MAX_PATH;
			item1.pszText = (LPTSTR)iter->first.c_str();
			item1.lParam = (LPARAM)_wcsdup(iter->second.c_str());

			lparams.push_back((void*)item1.lParam);

			TV_INSERTSTRUCT insert1;
			insert1.hParent = Selected;
			insert1.hInsertAfter = TVI_LAST;
			insert1.item = item1;

			HTREEITEM root2 = TreeView_InsertItem(m_tree, &insert1);
		}
	}

	return 0;
}

void ClearTreeControl(HWND hwnd)
{
	HWND m_tree = GetDlgItem(hwnd, IDC_VIDEOINFOS);
	
	std::vector<void*>::iterator iter = lparams.begin();
	while (iter != lparams.end()) //#1
	{
		//注意要先释放内存，在删除vector元素，顺序不能颠倒。
		//释放内存
		free(*iter);
		*iter = NULL;
		iter++; //#1
	}
	lparams.clear();
	TreeView_DeleteAllItems(m_tree);
}

BOOL VideoLoadImage(HWND hDlg, const char* pszFileName)
{
	HttpTool* httpTool = new HttpTool();

	bool isGet = httpTool->httpGet(pszFileName);
	if (!isGet)
		return FALSE;

	size_t dwSize;
	// 根据文件大小分配HGLOBAL内存  
	dwSize = httpTool->getReponseHTMLSize();

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_NODISCARD, dwSize);
	if (!hGlobal)
	{
		return FALSE;
	};

	char *pData = reinterpret_cast<char*>(GlobalLock(hGlobal));
	if (!pData)
	{
		GlobalFree(hGlobal);
		return FALSE;
	};

	// 将文件内容读到HGLOBAL内存中 
	int erro = 0;
	memcpy(pData, httpTool->getReponseHTML(), dwSize);
	pData[dwSize] = 0;
	/*erro = strcpy_s(pData, dwSize, httpTool->getReponseHTML());
	if (erro != 0)
	{
		GlobalFree(hGlobal);
		delete httpTool;
		return FALSE;
	}*/
	
	GlobalUnlock(hGlobal);
	

	// 利用hGlobal内存中的数据创建stream  
	IStream* istream = NULL;
	if (CreateStreamOnHGlobal(hGlobal, TRUE, &istream) != S_OK)
	{
		return FALSE;
	}

	HWND videoimg = GetDlgItem(hDlg, IDC_VIDEOIMG);
	HDC hdc = GetDC(videoimg);//BeginPaint(hDlg, &ps);

	//加载图像  
	Gdiplus::Image* image = Gdiplus::Image::FromStream(istream);// https://puui.qpic.cn/vcover_vt_pic/0/m8i6uooilmandtf1502788298/260
	if (image->GetLastStatus() != Gdiplus::Ok)
	{
		MessageBox(hDlg, L"加载图片失败!", L"提示", MB_OK);
		return -1;
	}

	RECT videoimgrect;
	GetClientRect(videoimg, &videoimgrect);
	//ShowWindow(videoimg, SW_HIDE);

	//取得宽度和高度  
	Gdiplus::Image* pThumbnail = image->GetThumbnailImage(
		videoimgrect.right - videoimgrect.left,
		videoimgrect.bottom - videoimgrect.top,
		NULL,
		NULL);

	//绘图  
	Gdiplus::Graphics graphics(hdc);
	graphics.DrawImage(image, videoimgrect.left, videoimgrect.top, pThumbnail->GetWidth(), pThumbnail->GetHeight());

	delete pThumbnail;
	delete image;
	istream->Release();
	delete httpTool;
	graphics.ReleaseHDC(hdc);

	//EndPaint(hDlg, &ps);

	return TRUE;
}

#include <windows.h>
#include <tchar.h>
#include <thread>
#include <commctrl.h>
#include "resource.h"

#include "include/cef_sandbox_win.h"
#include "simple_app.h"
#include "simple_handler.h"
#include "jiekou.h"
#include "magparse.h"

#pragma comment(lib, "libcef.lib")
#pragma comment(lib, "libcef_dll_wrapper.lib")

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void InitComBox(HWND hDlg);
void performActions(HWND hwnd);
int InitTreeControl(HWND hwnd, MagParse *uidatas);

CefRefPtr<SimpleApp>	app;
BOOL					m_bCEFInitialized;
std::multimap<std::string, std::string> jiexiurls;

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

	HWND hdlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_VIDEO), NULL, (DLGPROC)DlgProc);
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
		if (!IsDialogMessage(hdlg, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (m_bCEFInitialized)
		{
			CefDoMessageLoopWork();
		}
	}

	return 0;
}

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
	{
		//SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(hgInst, MAKEINTRESOURCE(IDI_ICON1)));
		InitComBox(hDlg);
		break;
	}
	case WM_CLOSE:
	{
		if (app != NULL)
		{
			if (app.get()->GetCefClient() != NULL)
			{
				app.get()->GetCefClient()->CloseAllBrowsers(false);
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
		EndDialog(hDlg, IDCANCEL);
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
				std::string url;
				url.append(jiexiurl + szBuff);
				
				//启动播放该视频
				app.get()->PlayByCef(url);
				break;
			}
			case IDC_START_SEARCH:
			{
				// 清理视频信息数据
				HWND m_tree = GetDlgItem(hDlg, IDC_VIDEOINFOS);
				TreeView_DeleteAllItems(m_tree);

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
			ti.mask = TVIF_HANDLE | TVIF_TEXT | TVS_SHOWSELALWAYS;
			TCHAR buf[MAX_PATH] = { 0 };
			int index = 0;
			ti.cchTextMax = MAX_PATH;
			ti.pszText = buf;
			ti.hItem = hItem;
			
			TreeView_GetItem(lpnmh->hwndFrom, &ti);

			if (!TreeView_GetChild(lpnmh->hwndFrom, hItem))
			{
				SetDlgItemText(hDlg, IDC_URL, buf);
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
		item.mask = TVIF_TEXT;
		item.cchTextMax = 10;
		item.pszText = (LPWSTR)sp.name.c_str();

		TV_INSERTSTRUCT insert;
		insert.hParent = TVI_ROOT;
		insert.hInsertAfter = TVI_LAST;
		insert.item = item;

		HWND m_tree = GetDlgItem(hdlg, IDC_VIDEOINFOS);
		HTREEITEM Selected = TreeView_InsertItem(m_tree, &insert);
		for (int index = 0; index < sp.totalNum; index++) //遍历json成员
		{
			TV_ITEM item1;
			item1.mask = TVIF_TEXT | TVIF_PARAM | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS;
			item1.cchTextMax = 2;
			item1.pszText = (LPTSTR)sp.resLinks[index].c_str();

			TV_INSERTSTRUCT insert1;
			insert1.hParent = Selected;
			insert1.hInsertAfter = TVI_LAST;
			insert1.item = item1;

			HTREEITEM root2 = TreeView_InsertItem(m_tree, &insert1);
		}
	}

	return 0;
}

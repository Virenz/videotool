// videotool.cpp: 定义控制台应用程序的入口点。
//

// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <windows.h>
#include <tchar.h>
#include "resource.h"

#include "include/cef_sandbox_win.h"
#include "simple_app.h"
#include "simple_handler.h"

#pragma comment(lib, "libcef.lib")
#pragma comment(lib, "libcef_dll_wrapper.lib")

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void InitComBox(HWND hDlg);

CefRefPtr<SimpleApp>	app;
BOOL					m_bCEFInitialized;//是否初始化

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
	//UpdateWindow(hdlg);

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
		// 设置对话框的图标 
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
		EndDialog(hDlg, IDCANCEL);
		PostQuitMessage(0);//退出
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
			case IDOK:
			{
				app.get()->PlayByCef("http://www.sjzvip.com/jiexi3.php?url=http://v.youku.com/v_show/id_XMjk0NjEyNDkwOA==.html?tpa=dW5pb25faWQ9MTAyMjEzXzEwMDAwNl8wMV8wMQ&from=360sousuo&refer=360sousuo");
				break;
			}
			default:
				break;
		}
		break;
	}
	default:
		break;
	}
	return (INT_PTR)FALSE;
}

void InitComBox(HWND hDlg)
{
	HWND combox = GetDlgItem(hDlg, IDC_INTERFACE);

	TCHAR achTemp[256] = TEXT("Food");
	TCHAR achTemp_One[256] = TEXT("Dog");
	TCHAR achTemp_Two[256] = TEXT("Fish");
	DWORD dwIndex;

	// Get the handle of the food groups combo box.  
	dwIndex = SendMessage(combox, CB_ADDSTRING, 0, (LPARAM)achTemp);
	dwIndex = SendMessage(combox, CB_ADDSTRING, 0, (LPARAM)achTemp_One);
	dwIndex = SendMessage(combox, CB_ADDSTRING, 0, (LPARAM)achTemp_Two);
	
}
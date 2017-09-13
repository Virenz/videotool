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

#if !defined(CEF_USE_SANDBOX)
	settings.no_sandbox = true;
#endif

	// SimpleApp implements application-level callbacks for the browser process.
	// It will create the first browser instance in OnContextInitialized() after
	// CEF has initialized.
	CefRefPtr<SimpleApp> app(new SimpleApp);

	// Initialize CEF.
	CefInitialize(main_args, settings, app.get(), sandbox_info);

	HWND hdlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_VIDEO), NULL, (DLGPROC)DlgProc);
	if (!hdlg)
	{
		return 0;
	}
	ShowWindow(hdlg, SW_SHOW);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Run the CEF message loop. This will block until CefQuitMessageLoop() is
	// called.
	//CefRunMessageLoop();

	// Shut down CEF.
	//CefShutdown();

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
		
		//CefString url = "http://www.sjzvip.com/jiexi8.php?url=http://v.youku.com/v_show/id_XMjk2MTUyMzMxNg==.html?tpa=dW5pb25faWQ9MTAyMjEzXzEwMDAwNl8wMV8wMQ&from=360sousuo&refer=360sousuo";
		CefString url = "https://www.baidu.com";
		// SimpleHandler implements browser-level callbacks.
		CefRefPtr<SimpleHandler> handler(new SimpleHandler);
		// Specify CEF browser settings here.
		CefBrowserSettings browser_settings;
		CefWindowInfo window_info;
		RECT rt;
		HWND brown = GetDlgItem(hDlg, IDC_BROWNAREA);
		GetClientRect(brown, &rt);
		window_info.SetAsChild(brown, rt);
		CefBrowserHost::CreateBrowser(window_info, handler, url, browser_settings,NULL);

		break;
	}
	case WM_SYSCOMMAND:
	{
		if (wParam == SC_CLOSE)
		{
			//CefQuitMessageLoop();
			CefShutdown();
			PostQuitMessage(0);//退出
		}
		return 0;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
			break;
		}
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			break;
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

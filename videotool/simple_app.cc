// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "simple_app.h"
#include "simple_handler.h"

#include <string>

#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_helpers.h"


namespace {

// When using the Views framework this object provides the delegate
// implementation for the CefWindow that hosts the Views-based browser.
class SimpleWindowDelegate : public CefWindowDelegate {
 public:
  explicit SimpleWindowDelegate(CefRefPtr<CefBrowserView> browser_view)
      : browser_view_(browser_view) {}

  void OnWindowCreated(CefRefPtr<CefWindow> window) OVERRIDE;

  void OnWindowDestroyed(CefRefPtr<CefWindow> window) OVERRIDE {
    browser_view_ = NULL;
  }

  bool CanClose(CefRefPtr<CefWindow> window) OVERRIDE;

 private:
  CefRefPtr<CefBrowserView> browser_view_;

  IMPLEMENT_REFCOUNTING(SimpleWindowDelegate);
  DISALLOW_COPY_AND_ASSIGN(SimpleWindowDelegate);
};

void SimpleWindowDelegate::OnWindowCreated(CefRefPtr<CefWindow> window) 
{
	// Add the browser view and show the window.
	window->AddChildView(browser_view_);
	window->Show();

	// Give keyboard focus to the browser view.
	browser_view_->RequestFocus();
}

bool SimpleWindowDelegate::CanClose(CefRefPtr<CefWindow> window)  
{
	// Allow the window to close if the browser says it's OK.
	CefRefPtr<CefBrowser> browser = browser_view_->GetBrowser();
	if (browser)
		return browser->GetHost()->TryCloseBrowser();
	return true;
}

}  // namespace

SimpleApp::SimpleApp()
{
	m_hParentWindow = NULL;
	memset(&m_ParentRect, 0, sizeof(m_ParentRect));
}

void SimpleApp::SetParentWindow(HWND hParentWindow, RECT parentRect)
{
	m_hParentWindow = hParentWindow;
	m_ParentRect = parentRect;
}

CefRefPtr<SimpleHandler> SimpleApp::GetCefClient()
{
	return m_pCefClient;
}

void SimpleApp::PlayByCef(CefString url)
{
	CefBrowser* pBrowser = m_pCefClient->GetBrowser();

	CefRefPtr<CefFrame> pMainFram = pBrowser->GetMainFrame();
	if (pMainFram)
	{
		pMainFram->LoadURL(url);
	}
}

void SimpleApp::OnBeforeCommandLineProcessing(const CefString & process_type, CefRefPtr<CefCommandLine> command_line)
{	
	// 此参数解决多窗口问题
	command_line->AppendSwitch("process-per-site");
	command_line->AppendSwitch("no-proxy-server");
	//command_line->AppendSwitch("enable-npapi");
	//command_line->AppendSwitchWithValue("register-pepper-plugins", "NPSWF32_11_6_602_180.dll;application/x-shockwave-flash");
	//加载flash插件
	command_line->AppendSwitchWithValue("--ppapi-flash-path", "pepflashplayer.dll");
	//manifest.json中的version
	command_line->AppendSwitchWithValue("--ppapi-flash-version", "27.0.0.130");
}

void SimpleApp::OnContextInitialized() 
{
	CEF_REQUIRE_UI_THREAD();
	m_pCefClient = new SimpleHandler();

	// Information used when creating the native window.
	CefWindowInfo window_info;

	if (m_hParentWindow == NULL)
	{
		window_info.SetAsPopup(NULL, "CefDlgTest");
	}
	else {
		window_info.SetAsChild(m_hParentWindow, m_ParentRect);
	}

	// Specify CEF browser settings here.
	CefBrowserSettings browser_settings;

	CefBrowserHost::CreateBrowser(window_info, m_pCefClient.get(), "", browser_settings, NULL);
}

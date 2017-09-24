// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#include "simple_handler.h"

#include <windows.h>
#include <string>

#include "include/cef_browser.h"

void SimpleHandler::PlatformTitleChange(CefRefPtr<CefBrowser> browser,
                                        const CefString& title) {
  CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
  // 如果为被嵌入到主对话框的主浏览器窗口则更改我们对话框标题  
  //if (GetParent(hwnd) != NULL)
  //{
	 // hwnd = GetParent(hwnd);
	 // SetWindowText(hwnd, std::wstring(title).c_str());
  //}

  //// 否则有内核创建（链接形式打开）的窗口标题设置为自己的窗口标题  
  //else
  //{
	 // ::SetWindowText(hwnd, std::wstring(title).c_str());
  //}
}

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
  // ���Ϊ��Ƕ�뵽���Ի�����������������������ǶԻ������  
  //if (GetParent(hwnd) != NULL)
  //{
	 // hwnd = GetParent(hwnd);
	 // SetWindowText(hwnd, std::wstring(title).c_str());
  //}

  //// �������ں˴�����������ʽ�򿪣��Ĵ��ڱ�������Ϊ�Լ��Ĵ��ڱ���  
  //else
  //{
	 // ::SetWindowText(hwnd, std::wstring(title).c_str());
  //}
}

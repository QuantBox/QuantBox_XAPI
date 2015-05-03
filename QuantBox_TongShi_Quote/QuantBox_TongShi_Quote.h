// QuantBox_TongShi_Quote.h : main header file for the QuantBox_TongShi_Quote DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

// CQuantBox_TongShi_QuoteApp
// See QuantBox_TongShi_Quote.cpp for the implementation of this class
//

class CQuantBox_TongShi_QuoteApp : public CWinApp
{
public:
	CQuantBox_TongShi_QuoteApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

// TradeApi.h : main header file for the TradeApi DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CTradeApiApp
// See TradeApi.cpp for the implementation of this class
//

class CTradeApiApp : public CWinApp
{
public:
	CTradeApiApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};


// TradeGatewayGtja.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTradeGatewayGtjaApp:
// �йش����ʵ�֣������ TradeGatewayGtja.cpp
//

class CTradeGatewayGtjaApp : public CWinApp
{
public:
	CTradeGatewayGtjaApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
	virtual int Run();
};

extern CTradeGatewayGtjaApp theApp;

extern CCriticalSection g_csPackID;
extern long	g_lPackID;
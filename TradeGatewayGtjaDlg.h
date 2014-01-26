
// TradeGatewayGtjaDlg.h : ͷ�ļ�
//
#pragma once

#include "./network/io_service_pool.h"

#include "./network/ssl/ssl_server.h"
#include "./network/ssl/trade_server_ssl.h"


#include "./network/tcp/tcp_server_old.h"
#include "./network/tcp/trade_server_tcp_old.h"

#include "./network/http/http_server.h"
#include "./network/http/trade_server_http.h"


#include "./network/TcpServer.h"
#include "./network/tcp/TcpTradeServer.h"

#include "FileLogManager.h"


// CTradeGatewayGtjaDlg �Ի���
class CTradeGatewayGtjaDlg : public CDialog
{
// ����
public:
	CTradeGatewayGtjaDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TRADEGATEWAYGTJA_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStartup();


	




	afx_msg void OnBnClickedOpenLog();

	CString m_sSslAddr;
	CString m_sSslStatus;
	CString m_sSslWorker;
	CString m_sSslSend;

	CString m_sHttpAddr;
	CString m_sHttpStatus;
	CString m_sHttpWorker;
	CString m_sHttpSend;

	CString m_sTcpAddr;
	CString m_sTcpStatus;
	CString m_sTcpWorker;
	CString m_sTcpSend;

	CString m_sConnPoolMin;
	CString m_sConnPoolIncrease;
	//CString m_sConnPoolMax;
	CString m_sConnTimeout;
	CString m_sConnRetry;
	CString m_sConnIdleTimeout;
	CString m_sBusiRetry;

	CString m_sLogFilePath;
	CString m_sLogDb;
	CString m_sLogDbConnPoolMin;
	CString m_sLogDbConnPoolIncrease;
	CString m_sLogDbConnPoolMax;
	CString m_sLogGuiShowcount;
	CString m_sLogLevel;

	int m_nLogFileEnable;
	int m_nLogDBEnable;
	int m_nLogMQEnable;
	int m_nLogGUIEnable;

	int GetCPUCount();
	afx_msg void OnDestroy();
	afx_msg void OnClose();

/*
	bool verify_certificate(bool preverified,      boost::asio::ssl::verify_context& ctx)
	{
		// The verify callback can be used to check whether the certificate that is
		// being presented is valid for the peer. For example, RFC 2818 describes
		// the steps involved in doing this for HTTPS. Consult the OpenSSL
		// documentation for more details. Note that the callback is called once
		// for each certificate in the certificate chain, starting from the root
		// certificate authority.

		// In this example we will simply print the certificate's subject name.
		char subject_name[256];
		X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
		X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
		OutputDebugString("Verifying " );
		OutputDebugString(subject_name);
		OutputDebugString("\n");

		return preverified;
	}
*/
	void start();
	void stop();

	//io_service_pool iospool;

	io_service_pool * piospool_ssl;
	trade_server_ssl * pTradeServerSSL;
	ssl_server * pSSLServer;

	io_service_pool * piospool_tcpold;
	trade_server_tcp_old * pTradeServerTcpOld;
	tcp_server_old * pTcpServerOld;
	
	io_service_pool * piospool_tcp;
	TcpTradeServer * pTradeServerTcp;
	TcpServer * pTcpServer;
	
	
	io_service_pool * piospool_http;
	trade_server_http * pTradeServerHttp;
	http_server * pHttpServer;

	afx_msg void OnBnClickedExit();
	CString m_sLogFileThreadPool;
	CString m_sLogDbThreadPool;
	CString m_sLogMqThreadPool;
	CString m_sLogShowThreadPool;
	afx_msg void OnBnClickedTestCrash();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedClearCache();
	afx_msg void OnBnClickedButton1();

	
};
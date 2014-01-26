#include "stdafx.h"


#include <string>
#include <boost/format.hpp>

#include "Connect.h"
#include "./config/ConfigManager.h"
#include "common.h"
#include "./output/FileLog.h"

#include "./lib/szkingdom/KDEncodeCli.h"
#include "./lib/szkingdom/kcbpcli.h"

// ����

#include "./lib/apexsoft/fixapi.h"
#include "./lib/apexsoft/fiddef.h"

#include "./business/apexsoft/DingDian.h"


Connect::Connect(int ConnectNo, Counter counter)
{
	init();

	m_nID = ConnectNo;
	m_Counter = counter;

	boost::format fmt("���%1% ��̨��ַ%2%:%3%");
	fmt % m_nID % m_Counter.m_sIP % m_Counter.m_nPort;
	m_sServerInfo = fmt.str();

}

void Connect::init()
{
	// ��֤windows
	handle = NULL;
	
	//  ����t2
	lpConnection = NULL;
	lpConfig = NULL;

	// ����COM
	m_pComm = NULL;

	// ����
	m_hHandle = 0;

	

	
}

std::string Connect::GetConnectInfo()
{
	return m_sServerInfo;
}

Connect::~Connect(void)
{
	CloseConnect();
}



bool Connect::CreateConnectT2()
{
	int nRetry = gConfigManager::instance().m_nConnectRetry;


	int nRet = 0;

	lpConfig = NewConfig();
	lpConfig->AddRef();

	std::string s = m_Counter.m_sIP;
	s += ":";
	s += boost::lexical_cast<std::string>(m_Counter.m_nPort);
	lpConfig->SetString("t2sdk", "servers", s.c_str());

	std::string license_file;
	license_file = gConfigManager::instance().m_sPath + "\\license.dat";
	lpConfig->SetString("t2sdk", "license_file", license_file.c_str());
	lpConfig->SetString("t2sdk", "lang", "1033");
	lpConfig->SetString("t2sdk", "send_queue_size", "100");
	lpConfig->SetString("safe", "safe_level", "none");

	for (int i=0; i<nRetry; i++)
	{
		lpConnection = NewConnection(lpConfig);
		lpConnection->AddRef();

		nRet = lpConnection->Create(NULL);

		
		nRet = lpConnection->Connect(m_Counter.m_nConnectTimeout*1000);

		if (nRet != 0)
		{
			// ��������ʧ��
			std::string sErrMsg = lpConnection->GetErrorMsg(nRet);
			std::string msg = "��������ʧ��, " + m_sServerInfo + sErrMsg;
			gFileLog::instance().Log(msg);

			Sleep(500);

			continue;
		}
		else
		{
			// �������Ӵ���ʱ��
			prev = time(NULL);

			std::string msg = "�������ӳɹ�, " + m_sServerInfo;
			
			gFileLog::instance().Log(msg);

			return true;
		}
	} // end for

	return false;
}

bool Connect::CreateConnectKCBP()
{
	int nRetry = gConfigManager::instance().m_nConnectRetry;


	int nRet = 0;

	for (int i=0; i<nRetry; i++)
	{
		tagKCBPConnectOption stKCBPConnection;
		memset(&stKCBPConnection, 0x00, sizeof(stKCBPConnection));
		strcpy(stKCBPConnection.szServerName, m_Counter.m_sServerName.c_str());
		stKCBPConnection.nProtocal = 0;
		strcpy(stKCBPConnection.szAddress, m_Counter.m_sIP.c_str());
		stKCBPConnection.nPort = m_Counter.m_nPort;
		strcpy(stKCBPConnection.szSendQName, m_Counter.m_sReq.c_str());
		strcpy(stKCBPConnection.szReceiveQName, m_Counter.m_sRes.c_str());

		nRet = KCBPCLI_Init( &handle );
		nRet = KCBPCLI_SetConnectOption( handle, stKCBPConnection );		

		//���ó�ʱ
		nRet = KCBPCLI_SetCliTimeOut( handle, m_Counter.m_nConnectTimeout);

		// �����Ƿ����������Ϣ
		//nRet = KCBPCLI_SetOptions( handle, KCBP_OPTION_TRACE, &gConfigManager::instance().m_nIsTradeServerDebug, sizeof(int));

		nRet = KCBPCLI_SQLConnect( handle, stKCBPConnection.szServerName, (char*)m_Counter.m_sUserName.c_str(), (char*)m_Counter.m_sPassword.c_str());

		if (nRet != 0)
		{
			// ��������ʧ��
			std::string msg = "��������ʧ��, " + m_sServerInfo + ",�����룺" + boost::lexical_cast<std::string>(nRet);
			

			gFileLog::instance().Log(msg);

			Sleep(500);

			continue;
		}
		else
		{
			// �������Ӵ���ʱ��
			prev = time(NULL);

			std::string msg = "�������ӳɹ�, " + m_sServerInfo;
			
			gFileLog::instance().Log(msg);

			return true;
		}
	} // end for

	return false;
}

bool Connect::CreateConnectComm()
{
	::CoInitialize(NULL);

	int nRetry = gConfigManager::instance().m_nConnectRetry;


	int nRet = 0;

	for (int i=0; i<nRetry; i++)
	{
		m_pComm = new CComm();
		m_pComm->CreateDispatch("HsCommX.Comm");
		m_pComm->Create();
		
		//handle->put_SendTimeOut(g_ConfigManager.m_nSendTimeout);
		//handle->put_ReceiveTimeOut(g_ConfigManager.m_nRecvTimeout);

		nRet = m_pComm->ConnectX(1, m_Counter.m_sIP.c_str(), m_Counter.m_nPort, 0, "", 0);
		if (nRet != 0)
		{
			// ��������ʧ��
			std::string msg = "��������ʧ��, " + m_sServerInfo + "\n";
			OutputDebugString(msg.c_str());

			//g_LogManager.SendNormalMsg(Cssweb::CsswebMessage::ERROR_LEVEL, Cssweb::CsswebMessage::GUI_FILE_OUTPUT, "", msg);

			Sleep(500);

			continue;
		}
		else
		{
			// �������Ӵ���ʱ��
			prev = time(NULL);

			std::string msg = "�������ӳɹ�, " + m_sServerInfo + "\n";
			//g_LogManager.SendNormalMsg(Cssweb::CsswebMessage::DEBUG_LEVEL, Cssweb::CsswebMessage::GUI_FILE_OUTPUT, "", msg);
			OutputDebugString(msg.c_str());

			return true;
		}
	} // end for

	return false;
}

bool Connect::CreateConnectDingDian()
{
	int nRetry = gConfigManager::instance().m_nConnectRetry;


	int nRet = 0;

	
	for (int i=0; i<nRetry; i++)
	{
		std::string gtAddr = m_Counter.m_sIP + "@" + boost::lexical_cast<std::string>(m_Counter.m_nPort) + "/tcp";
		m_hHandle = Fix_Connect(gtAddr.c_str(), m_Counter.m_sUserName.c_str(), m_Counter.m_sPassword.c_str(), m_Counter.m_nConnectTimeout); 

		
		if (m_hHandle == 0)
		{
			// ��������ʧ��
			std::string msg = "��������ʧ��, " + m_sServerInfo;
			gFileLog::instance().Log(msg);

			Sleep(500);

			continue;
		}
		else
		{
			// �������Ӵ���ʱ��
			prev = time(NULL);

			std::string msg = "�������ӳɹ�, " + m_sServerInfo;
			
			gFileLog::instance().Log(msg);

			return true;
		}
	} // end for
	

	return false;
}


bool Connect::CreateConnect()
{
	bool bRet = false;

	switch(m_Counter.m_eCounterType)
	{
	case CT_HS_T2:
		bRet = CreateConnectT2();
		break;
	case CT_HS_COM:
		bRet = CreateConnectComm();
		break;
	case CT_JZ_WIN:
		bRet = CreateConnectKCBP();
		break;
	case CT_JZ_LINUX:
		break;
	case CT_DINGDIAN:
		bRet = CreateConnectDingDian();
		break;
	
	default:
		break;
	}

	return bRet;
}

bool Connect::IsTimeout()
{
	// ��ǰʱ��
	time_t now = time(NULL);

	// ��̨apiû���ṩ��ǰ�����Ƿ���Ч�ķ���
	// �޷��õ���̨��socket���޷�����tcp���������ʵ��ά������
	// ����tcp������һ��ʱ����û�������������жϣ�������Ҫ�ж��Ƿ�ʱ
	// prev�������ϴβ���������ʱ��
	if ((now - prev) >= m_Counter.m_nIdleTimeout)
	{
		std::string msg = "���ӳ�ʱ, " + m_sServerInfo;
		gFileLog::instance().Log(msg);
		
		return true;
	}

	//std::string msg = "����û�г�ʱ" + m_sServerInfo;
	return false;
}


bool Connect::ReConnect()
{
	CloseConnect();

	bool bRet = CreateConnect();

	if (bRet)
	{
		std::string msg = "�����ɹ�, " + m_sServerInfo;
		gFileLog::instance().Log(msg);
	}
	else
	{
		std::string msg = "����ʧ��, " + m_sServerInfo;
		gFileLog::instance().Log(msg);
	}

	return bRet;
}



void Connect::CloseConnect()
{
		
	switch(m_Counter.m_eCounterType)
	{
	case CT_HS_T2:
		// ����t2sdk
		if (lpConnection != NULL)
		{
			std::string msg = "�رպ���t2���ӣ�" + m_sServerInfo;
			gFileLog::instance().Log(msg);

			lpConnection->Release();
			//delete lpConnection;
			lpConnection = NULL;

		}

		if (lpConfig != NULL)
		{
			lpConfig->Release();
			//delete lpConfig;
			lpConfig = NULL;
		}
		break;
	case CT_HS_COM:
		// ����COM
		if (m_pComm != NULL)
		{
			std::string msg = "�رպ���COM���ӣ�" + m_sServerInfo;
			gFileLog::instance().Log(msg);

			m_pComm->DisConnect();
			m_pComm->ReleaseDispatch();
			m_pComm->Free();
			delete m_pComm;
			m_pComm = NULL;

		}
		break;
	case CT_JZ_WIN:
		// ��֤windows�汾
		if (handle != NULL)
		{
			std::string msg = "�رս�֤win���ӣ�" + m_sServerInfo;
			gFileLog::instance().Log(msg);

			KCBPCLI_SQLDisconnect(handle);
			KCBPCLI_Exit(handle);
			handle = NULL;
		}
		break;
	case CT_JZ_LINUX:
		break;
	case CT_DINGDIAN:
		// ����
		
		if (m_hHandle != NULL)
		{
			std::string msg = "�رն������ӣ�" + m_sServerInfo;
			gFileLog::instance().Log(msg);

			Fix_Close(m_hHandle);
			m_hHandle = NULL;
		}
		
		break;
	
	default:
		break;
	}
}

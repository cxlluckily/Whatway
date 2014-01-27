#include "stdafx.h"

#include "LogConnectPool.h"
#include "LogConnect.h"

#include "./config/ConfigManager.h"
#include "./output/FileLog.h"


LogConnectPool::LogConnectPool(void)
{
	m_nID = 0;
	m_nConnCount = 0;
	m_sLogFile = "�ֲ�ʽ��־.log";
}


LogConnectPool::~LogConnectPool(void)
{
}

BOOL LogConnectPool::CreateConnectPool()
{
	std::string msg = "��ʼ������־���ӳ�";
	gFileLog::instance().Log(msg, m_sLogFile);

	bool bRet = false;

	
	//m_bCreatePool = true;

	// ��ʼ��������=��̨�������� * m_nConnPoolMin
	// �����������4��, m_nConnPoolMin=2, ������s1, s2, s3, s4, s1,s2,s3,s4

	for (int i=0; i < gConfigManager::instance().m_nLogMqMin; i++)
	{
		for (std::vector<std::string>::iterator pos = gConfigManager::instance().m_vLogMqServer.begin(); pos != gConfigManager::instance().m_vLogMqServer.end(); pos++)
		{
			
			std::string server = *pos;

			LogConnect * pConn = new LogConnect();
			if (pConn->Connect(server))
			{
				m_pool.push(pConn);
				m_nID++;
			}
			else
			{
				delete pConn;
			}

		} // end for ��̨��������
	} // end for ��ʼ����
	

	m_nConnCount = m_pool.queue_.size();

	if (m_nConnCount == 0)
	{
		std::string msg = "������־���ӳ�ʧ��";
		gFileLog::instance().Log(msg, m_sLogFile);
		

		bRet = false;
	}
	else
	{
		std::string msg = "������־���ӳسɹ�";
		gFileLog::instance().Log(msg, m_sLogFile);


		bRet = true;
	}


	return bRet;
}

BOOL LogConnectPool::IncreaseConnPool()
{
	BOOL bRet = FALSE;

	int nOldSize = m_pool.queue_.size();



	// ������=��̨�������� * m_nConnPoolIncrease
	for (int i=0; i < gConfigManager::instance().m_nLogMqIncrease; i++)
	{
		for (std::vector<std::string>::iterator pos = gConfigManager::instance().m_vLogMqServer.begin(); pos != gConfigManager::instance().m_vLogMqServer.end(); pos++)
		{
			std::string server = *pos;

			LogConnect * pConn = new LogConnect();
			if (pConn->Connect(server))
			{
				m_pool.push(pConn);
				m_nID++;
			}
			else
			{
				delete pConn;
			}

		} // end for ��̨��������
	} // end for ��������
		

	m_nConnCount = m_pool.queue_.size();

	if (m_nConnCount == nOldSize)
	{
			
		std::string msg = "������־���ӳ�ʧ��";
		gFileLog::instance().Log(msg, m_sLogFile);

		bRet = FALSE;
	}
	else
	{
		std::string msg = "������־���ӳسɹ�";
		gFileLog::instance().Log(msg, m_sLogFile);


		bRet = TRUE;
	}

	return bRet;
}

LogConnect* LogConnectPool::GetConnect()
{
	std::string msg;

	if (m_pool.queue_.empty())
	{
		if (!IncreaseConnPool())
			return NULL;

	}

	LogConnect *pConn = m_pool.pop();
	if (pConn == NULL)
	{
		msg = "��ȡ���ӣ�ʧ��";
		gFileLog::instance().Log(msg, m_sLogFile);

		return NULL;
	}



	//msg = "��ȡ���ӳɹ�, " + pConn->GetConnectInfo();
	gFileLog::instance().Log(msg, m_sLogFile);

	return pConn;
}

void LogConnectPool::PushConnect(LogConnect * pConn)
{
	if (pConn == NULL)
		return;

	std::string msg = "�ͷ�����, " ;//+ pConn->GetConnectInfo();
	gFileLog::instance().Log(msg, m_sLogFile);

	msg = "�ͷ�����: �黹ǰ��С" + boost::lexical_cast<std::string>(m_pool.queue_.size());
	gFileLog::instance().Log(msg, m_sLogFile);

	m_pool.push(pConn);

	msg = "�ͷ�����: �黹���С" + boost::lexical_cast<std::string>(m_pool.queue_.size());
	gFileLog::instance().Log(msg, m_sLogFile);
}

void LogConnectPool::CloseConnectPool()
{
	std::string msg = "�ر����ӳ�";
	gFileLog::instance().Log(msg, m_sLogFile);

	//m_bCreatePool = false;

	m_pool.stop();

	/*
	����һ
	Connect * pConn = m_pool.pop();
	*/

	// ������
	for (std::deque<LogConnect*>::iterator pos = m_pool.queue_.begin(); pos != m_pool.queue_.end(); pos++)
	{
		LogConnect * pConn = *pos;

		if (pConn != NULL)
		{
			pConn->Close();
			delete pConn;
			pConn = NULL;
		}
	}
}



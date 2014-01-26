#include "ConnectPool.h"
#include "./config/ConfigManager.h"

#include "./output/FileLog.h"



ConnectPool::ConnectPool(std::vector<Counter> vCounter)
{
	m_nID = 0;

	m_vCounter = vCounter;

	m_bCreatePool = false;
}

ConnectPool::~ConnectPool(void)
{
	CloseConnPool();
}

bool ConnectPool::CreateConnPool()
{
		std::string msg = "��ʼ�������ӳ�";
		gFileLog::instance().Log(msg);

	bool bRet = false;

	
	m_bCreatePool = true;

	// ��ʼ��������=��̨�������� * m_nConnPoolMin
	// �����������4��, m_nConnPoolMin=2, ������s1, s2, s3, s4, s1,s2,s3,s4

	for (int i=0; i < gConfigManager::instance().m_nConnectPoolMin; i++)
	{
		for (std::vector<Counter>::iterator pos = m_vCounter.begin(); pos != m_vCounter.end(); pos++)
		{
			
			Connect * pConn = new Connect(m_nID, *pos);
			if (pConn->CreateConnect())
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
		std::string msg = "�������ӳ�ʧ��";
		gFileLog::instance().Log(msg);
		

		bRet = false;
	}
	else
	{
		std::string msg = "�������ӳسɹ�";
		gFileLog::instance().Log(msg);


		bRet = true;
	}


	return bRet;
}


bool ConnectPool::IncreaseConnPool()
{
	bool bRet = false;

	int nOldSize = m_pool.queue_.size();



	// ������=��̨�������� * m_nConnPoolIncrease
	for (int i=0; i < gConfigManager::instance().m_nConnectPoolIncrease; i++)
	{
		for (std::vector<Counter>::iterator pos = m_vCounter.begin(); pos != m_vCounter.end(); pos++)
		{

			Connect * pConn = new Connect(m_nID, *pos);
			if (pConn->CreateConnect())
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
			
		std::string msg = "�������ӳ�ʧ��";
		gFileLog::instance().Log(msg);

		bRet = false;
	}
	else
	{
		std::string msg = "�������ӳسɹ�";
		gFileLog::instance().Log(msg);


		bRet = true;
	}

	return bRet;
}



// ��ȡ���ӵı�������ѭ���������������������
 Connect* ConnectPool::GetConnect()
{
	std::string msg;

	if (m_pool.queue_.empty())
	{
		if (!IncreaseConnPool())
			return NULL;
	}

	Connect *pConn = m_pool.pop();
	if (pConn == NULL)
	{
		msg = "��ȡ���ӣ�ʧ��";
		gFileLog::instance().Log(msg);

		return NULL;
	}


	if (pConn->IsTimeout())
	{
		if (!pConn->ReConnect())
		{
			msg = "��ȡ���ӳ�ʱ����ʧ��" + pConn->GetConnectInfo();
			gFileLog::instance().Log(msg);

			return NULL;
		}
	}

	msg = "��ȡ���ӳɹ�, " + pConn->GetConnectInfo();
	gFileLog::instance().Log(msg);

	return pConn;
}

void ConnectPool::PushConnect(Connect* pConn)
{
	if (pConn == NULL)
		return;

	std::string msg = "�ͷ�����, " + pConn->GetConnectInfo();
	gFileLog::instance().Log(msg);

	msg = "�ͷ�����: �黹ǰ��С" + boost::lexical_cast<std::string>(m_pool.queue_.size());
	gFileLog::instance().Log(msg);

	m_pool.push(pConn);

	msg = "�ͷ�����: �黹���С" + boost::lexical_cast<std::string>(m_pool.queue_.size());
	gFileLog::instance().Log(msg);
}

void ConnectPool::CloseConnPool()
{
	std::string msg = "�ر����ӳ�";
	gFileLog::instance().Log(msg);

	m_bCreatePool = false;

	m_pool.stop();

	/*
	����һ
	Connect * pConn = m_pool.pop();
	*/

	// ������
	for (std::deque<Connect*>::iterator pos = m_pool.queue_.begin(); pos != m_pool.queue_.end(); pos++)
	{
		Connect * pConn = *pos;

		if (pConn != NULL)
		{
			pConn->CloseConnect();
			delete pConn;
			pConn = NULL;
		}
	}
	
}

bool ConnectPool::IsCreatePool()
{
	return m_bCreatePool;
}
#include "stdafx.h"

#include "ConnectManager.h"
#include "./config/ConfigManager.h"
#include "./output/FileLog.h"
//#include ".\dingdian\fixapi.h"




CConnectManager g_ConnectManager;

CConnectManager::CConnectManager(void)
{
}

CConnectManager::~CConnectManager(void)
{
	//CloseConnPool();
}






Connect* CConnectManager::GetConnect(std::string sysNo, BUSI_TYPE busiType, std::string sBranchId)
{
	Connect* pConn = NULL;

	// �Ҳ���ϵͳ��ֱ�ӷ���
	std::map<std::string, BusinessSystem>::iterator itSys;
	itSys = systems.find(sysNo);
	if (itSys == systems.end())
		return NULL;

	// �Ҳ���ҵ�����ͷ���
	BusinessSystem& sys = itSys->second;
	std::map<BUSI_TYPE, BusinessType >::iterator itBusiType;
	itBusiType = sys.busis.find(busiType);
	if (itBusiType == sys.busis.end())
	{
		return NULL;
	}

	// �Ҳ���Ӫҵ������
	BusinessType& bt = itBusiType->second;
	std::map<std::string, ConnectPool*>::iterator itBranch;
	bool bFoundBranch = false;
	for (itBranch = bt.connPool.begin(); itBranch != bt.connPool.end(); itBranch++)
	{
		std::string branchList = itBranch->first;

		if (branchList == "0000")
		{
			bFoundBranch = true;
			break;
		}

		if (branchList.find(sBranchId) != std::string::npos)
		{
			bFoundBranch = true;
			break;
		}
		else
		{
			bFoundBranch = false;
		}
	}

	if (!bFoundBranch)
	{
		return NULL;
	}

	
	ConnectPool * pool = itBranch->second;
	if (pool->IsCreatePool())
	{
		pConn = pool->GetConnect();

	}
	else
	{
		if (!pool->CreateConnPool())
		{

			return NULL;
		}
		else
		{
			pConn = pool->GetConnect();
		}		
	}

	return pConn;
}

void CConnectManager::PushConnect(Connect * pConn, std::string sysNo, BUSI_TYPE busiType, std::string sBranchId)
{
	if (pConn == NULL)
		return;

	if (sysNo.empty())
		return ;


	// �Ҳ���ϵͳ��ֱ�ӷ���
	std::map<std::string, BusinessSystem>::iterator itSys;
	itSys = systems.find(sysNo);
	if (itSys == systems.end())
		return;

	// �Ҳ���ҵ�����ͷ���
	BusinessSystem& sys = itSys->second;
	std::map<BUSI_TYPE, BusinessType >::iterator itBusiType;
	itBusiType = sys.busis.find(busiType);
	if (itBusiType == sys.busis.end())
	{
		return;
	}

	// �Ҳ���Ӫҵ������
	BusinessType& bt = itBusiType->second;
	std::map<std::string, ConnectPool*>::iterator itBranch;
	bool bFoundBranch = false;
	for (itBranch = bt.connPool.begin(); itBranch != bt.connPool.end(); itBranch++)
	{
		std::string branchList = itBranch->first;

		if (branchList == "0000")
		{
			bFoundBranch = true;
			break;
		}

		if (branchList.find(sBranchId) != std::string::npos)
		{
			bFoundBranch = true;
			break;
		}
		else
		{
			bFoundBranch = false;
		}
	}

	if (!bFoundBranch)
	{
		return;
	}

	
	ConnectPool * pool = itBranch->second;
	pool->PushConnect(pConn);

}

void CConnectManager::CloseConnPool()
{
	std::map<std::string, BusinessSystem>::iterator itSys;


	for (itSys = systems.begin(); itSys != systems.end(); itSys++)
	{
		BusinessSystem& bs = itSys->second;

		std::map<BUSI_TYPE, BusinessType>::iterator itBusiType;
		for (itBusiType = bs.busis.begin(); itBusiType != bs.busis.end(); itBusiType++)
		{
			BusinessType& bt = itBusiType->second;

			std::map<std::string, ConnectPool*>::iterator itBranch;
			for (itBranch = bt.connPool.begin(); itBranch != bt.connPool.end(); itBranch++)
			{
				ConnectPool * pConnPool = itBranch->second;
				if (pConnPool != NULL && pConnPool->IsCreatePool())
				{
					pConnPool->CloseConnPool();
					delete pConnPool;
					pConnPool = NULL;
				}
			}
		}
	}
}

Counter* CConnectManager::GetServer(std::string sysNo, BUSI_TYPE busiType, std::string sBranchId)
{
	// �˺�����Ҫ��ͬ��mutex

	// �Ҳ���ϵͳ��ֱ�ӷ���
	std::map<std::string, BusinessSystem>::iterator itSys;
	itSys = systems.find(sysNo);
	if (itSys == systems.end())
		return NULL;

	// �Ҳ���ҵ�����ͷ���
	BusinessSystem& sys = itSys->second;
	std::map<BUSI_TYPE, BusinessType >::iterator itBusiType;
	itBusiType = sys.busis.find(busiType);
	if (itBusiType == sys.busis.end())
	{
		return NULL;
	}

	// �Ҳ���Ӫҵ������
	BusinessType& bt = itBusiType->second;
	std::map<std::string, Branch*>::iterator itBranch;
	bool bFoundBranch = false;
	for (itBranch = bt.branches.begin(); itBranch != bt.branches.end(); itBranch++)
	{
		std::string branchList = itBranch->first;

		if (branchList == "0000")
		{
			bFoundBranch = true;
			break;
		}

		if (branchList.find(sBranchId) != std::string::npos)
		{
			bFoundBranch = true;
			break;
		}
		else
		{
			bFoundBranch = false;
		}
	}

	if (!bFoundBranch)
	{
		return NULL;
	}

	
	Branch* branch = itBranch->second;
	return branch->GetCounter();
	
	
}

int CConnectManager::GetServerCount(std::string sysNo, BUSI_TYPE busiType, std::string sBranchId)
{
		// �˺�����Ҫ��ͬ��mutex

	// �Ҳ���ϵͳ��ֱ�ӷ���
	std::map<std::string, BusinessSystem>::iterator itSys;
	itSys = systems.find(sysNo);
	if (itSys == systems.end())
		return 0;

	// �Ҳ���ҵ�����ͷ���
	BusinessSystem& sys = itSys->second;
	std::map<BUSI_TYPE, BusinessType >::iterator itBusiType;
	itBusiType = sys.busis.find(busiType);
	if (itBusiType == sys.busis.end())
	{
		return 0;
	}

	// �Ҳ���Ӫҵ������
	BusinessType& bt = itBusiType->second;
	std::map<std::string, Branch*>::iterator itBranch;
	bool bFoundBranch = false;
	for (itBranch = bt.branches.begin(); itBranch != bt.branches.end(); itBranch++)
	{
		std::string branchList = itBranch->first;

		if (branchList == "0000")
		{
			bFoundBranch = true;
			break;
		}

		if (branchList.find(sBranchId) != std::string::npos)
		{
			bFoundBranch = true;
			break;
		}
		else
		{
			bFoundBranch = false;
		}
	}

	if (!bFoundBranch)
	{
		return 0;
	}

	
	Branch* branch = itBranch->second;
	return branch->GetServerCount();
}

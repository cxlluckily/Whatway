#ifndef _CONNECT_MANAGER_H_
#define _CONNECT_MANAGER_H_

#include <map>
#include <string>

#include <mutex>

#include "common.h"
#include "BusinessSystem.h"
#include "BusinessType.h"
#include "Counter.h"
#include "connectpool.h"


class CConnectManager
{
public:
	CConnectManager(void);
	~CConnectManager(void);



	//��֤�̰߳�ȫ?
	Connect* GetConnect(std::string sysNo, BUSI_TYPE busiType, std::string sBranchId);
	void PushConnect(Connect * pConn, std::string sysNo, BUSI_TYPE busiType, std::string sBranchId);
	void CloseConnPool();

	Counter* GetServer(std::string system, BUSI_TYPE business, std::string branch);
	int GetServerCount(std::string system, BUSI_TYPE business, std::string branch);
	

	// configmanager����Ҫ����std::map<std::string, BusinessSystem> systems;
	std::map<std::string, BusinessSystem> systems;

	//            ϵͳ���         ҵ������        Ӫҵ���б�   ���ӳ�
	//std::map<std::string, std::map<enum, std::map<std::string, ConnectPool>> >
	// connectpool --> queue --> connect
};

extern CConnectManager g_ConnectManager;

#endif

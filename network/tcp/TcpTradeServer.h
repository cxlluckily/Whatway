#ifndef _TRADE_SERVER_TCP_
#define _TRADE_SERVER_TCP_

#include "network/CustomMessage.h"
#include "ThreadSafeQueue/job_queue.h"
#include "./threadpool/worker.h"

//#include "network/TcpSession.h"

#include "common.h"


class TcpTradeServer
{
public:
	typedef job_queue<CustomMessage*> req_queue_type;
	typedef worker<req_queue_type> req_worker_type;

	typedef job_queue<CustomMessage*> resp_queue_type;
	typedef worker<resp_queue_type> resp_worker_type;

private:
	req_queue_type recvq_;
	resp_queue_type sendq_;

	req_worker_type req_worker_;
	resp_worker_type resp_worker_;


public:
	TcpTradeServer();
	req_queue_type& recv_queue();
	void start();
	void stop();

private:
	// ����������Ϣ
	bool ProcessRequest(CustomMessage* req);

	// ����Ӧ����Ϣ
	bool ProcessResponse(CustomMessage* resp);


	bool GetSysNoAndBusiType(std::string& request, std::string& sysNo, std::string& busiType, std::string& sysVer, std::string& account, std::string& funcId, std::string& clientIp);


	std::string GetCounterType(COUNTER_TYPE counterType);


};
#endif

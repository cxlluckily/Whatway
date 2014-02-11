#ifndef _CONNECT_
#define _CONNECT_

#include "stdafx.h"

#include <string>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "common.h"


// ����T2
#include "business/hundsun_t2/t2sdk_interface.h"

// ����CommX
#include "business/hundsun_com/CComm.h"






#include "Counter.h"


class Connect
{
public:
	Connect(int ConnectNo, Counter counter);
	~Connect(void);

	void init();

	int m_nID;
	Counter m_Counter ;


	std::string m_sServerInfo;
	std::string GetConnectInfo();

	time_t prev; // ������ǰһ��ʹ�õ�ʱ�䣬�����ж��Ƿ���Ϊ��ʱ��û�в�����ɵĳ�ʱ
	bool IsTimeout();

	bool CreateConnect();
	bool ReConnect();
	void CloseConnect();


	// ��֤
	void * handle; // ��̨���Ӿ��
	bool CreateConnectKCBP();

	// ����T2
	CConfigInterface * lpConfig;
	CConnectionInterface * lpConnection;
	bool CreateConnectT2();

	// ����CommX
	CComm * m_pComm;
	bool CreateConnectComm();

	// ����
	long m_hHandle;
	bool CreateConnectDingDian();	

	

	

};
#endif

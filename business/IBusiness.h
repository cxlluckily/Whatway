#ifndef _IBUSINESS_H_
#define _IBUSINESS_H_

// stl
#include <string>
#include <map>
#include <vector>
// boost
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "common.h"
#include "./log/tradelog.pb.h"
#include "./connectpool/Connect.h"
#include "ConnectPool/Counter.h"

class IBusiness
{
public:
	IBusiness(void);
	~IBusiness(void);

	bool m_bConnected;
	virtual bool IsConnected();

	Counter * m_Counter;
	virtual void SetCounterServer(Counter * counter);

	virtual bool CreateConnect() = 0;
	virtual void CloseConnect() = 0;

	virtual bool Send(std::string& request, std::string& response, int& status, std::string& errCode, std::string& errMsg) = 0;


	
	
	

	


	std::string sysNo;

	int busiType;
	std::string bt;

	std::string sysVer;

	std::string branchId;

	std::string funcid;
	long lFuncId;

	std::string route;
	int nRoute;

	std::string SOH;

	std::string account; // �ͻ��ţ�����������־�ļ�

	std::string captcha;

	std::string note;

	std::string hardinfo;

	std::string cssweb_cacheFlag;

	std::string cssweb_flashCallback;
	std::string cssweb_pwdType;

	std::string cssweb_connid;

	std::map<std::string, std::string> reqmap;

	

	std::string sCounterType;

	// flash���ף��޸����빦�ܣ���ͨģʽ����������������Ҫ���ظ�ǰ��
	std::string flash_normal_modifypwd_newpwd;


	// ��־���ñ�������
	boost::posix_time::ptime beginTime; // ��ʼʱ��
	std::string sBeginTime;
	//boost::posix_time::ptime endTime; // ��ʼʱ��
	Trade::TradeLog::LogLevel logLevel; // ��־����
	std::string logEncodeRequest; // ��������, flash����
	std::string logRequest; // ��������
	int64_t runtime; // ����ʱ��
	int status;
	std::string retcode;
	std::string retmsg;
	std::string logResponse; // ��Ӧ����
	std::string logEncodeResponse; // ��Ӧ����, flash����
	std::string gt_ip; // ��̨ip
	std::string gt_port; // ��̨port
	std::string gateway_ip; // ����ip
	std::string gateway_port; // ����port

	void ParseRequest(std::string& request);
	bool FilterRequestField(std::string& key);
	
	void RetErrRes(Trade::TradeLog::LogLevel logLevel, std::string& response, std::string retcode, std::string retmsg);
	void RetNoRecordRes(std::string& response);

	

	int ConvertIntToBusiType(int val);
	
	
	

	bool DecryptPassword(std::string algo, std::string key, std::string cipher, std::string plain);

	
	void GenResponse(int nErrCode, std::string sErrMsg, std::string& response, int& status, std::string& errCode, std::string& errMsg);
};

#endif

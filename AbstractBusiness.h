#ifndef ABSTRACT_BUSINESS_H
#define ABSTRACT_BUSINESS_H

// stl
#include <string>
#include <map>
#include <vector>

// boost
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "./log/tradelog.pb.h"

#include "ConnectPool/Counter.h"

class AbstractBusiness
{
public:
	AbstractBusiness(void);
	~AbstractBusiness(void);

	virtual void SetCounterServer(Counter * counter);

	
	virtual bool IsConnected() = 0;
	virtual bool Connect() = 0;

	virtual bool Send(std::string& request, std::string& response, int& status, std::string& errCode, std::string& errMsg) = 0;


	Counter * counter;



		std::string sysNo;

	BUSI_TYPE busiType;
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
	void GenResponse(std::string& response, std::string& errCode, std::string& errMsg);

	void BeginLog(std::string& request);
	void EndLog(std::string& response, Trade::TradeLog& logMsg);

	BUSI_TYPE ConvertIntToBusiType(int val);
	
	
	void FreeConnect();

	bool DecryptPassword(std::string algo, std::string key, std::string cipher, std::string plain);
	
};

#endif

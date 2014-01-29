#ifndef _CONFIG_MANAGER_H_
#define _CONFIG_MANAGER_H_

#include <map>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/thread/detail/singleton.hpp>


#include "common.h"
#include "./connectpool/counter.h"
#include "./connectpool/businesssystem.h"


class ConfigManager
{
public:
	ConfigManager(void);
	~ConfigManager(void);

	// �����ļ�·��
	std::string m_sPath;
	std::string m_sExeFile;
	bool LoadConfig(std::string sPath="");



	// ͨ�Ų�
	std::string m_sIp;

	int m_nSslPort;
	int m_nSslWorkerThreadPool;
	int m_nSslSendThreadPool;
	int m_nAuth;
	
	int m_nHttpPort;
	int m_nHttpWorkerThreadPool;
	int m_nHttpSendThreadPool;
	std::string m_sFlashUrlContext;
	int m_nFlashTradeURLContext;
	//crossdomain.xml
	std::string m_sFlashCrossDomain;
	void LoadCrossDomain();

	int m_nTcpPort;
	int m_nTcpWorkerThreadPool;
	int m_nTcpSendThreadPool;


	int m_nSslNewPort;
	int m_nSslNewWorkerThreadPool;
	int m_nSslNewSendThreadPool;
	int m_nNewAuth;

	int m_nTcpNewPort;
	int m_nTcpNewWorkerThreadPool;
	int m_nTcpNewSendThreadPool;
	
	// ѹ����С
	int zlib;

	// ����ģʽ
	int nRunMode;

	// ��̨
	int m_nConnectPoolMin;
	int m_nConnectPoolMax;
	int m_nConnectPoolIncrease;


	std::string m_sWtfsWeb;
	std::string m_sWtfsMobile;


	int m_nConnectRetry;
	
	int m_nBusinessRetry; // ҵ��ʧ������



	//int m_nStockEnable;
	//std::map<std::string, std::vector<Counter>> m_Stock;

	//int m_nCreditEnable;
	//std::map<std::string, std::vector<Counter>> m_Credit;

	//std::map<std::string, std::vector<Counter>> m_Account;

	//std::map<std::string, std::vector<Counter>> m_Option;

	//std::map<std::string, std::vector<Counter>> m_Auth;

	
	// ��־
	
	// ��־����
	int m_nLogLevel;
	int m_nResponseLen;

	int m_nFilterFuncId;

	// ��������ֶ�
	int m_nFilterField;

	// �ͻ���
	std::map<std::string, int> m_mCustIds;
	std::string m_sFilterCustId; // custid=123&custid=456

	// �ļ�ϵͳ��־
	int m_nLogFileEnable;
	std::string m_sLogFilePath;
	std::string m_sLogDirSystem; // ?
	int m_nLogFileThreadPool;


	// ���ݿ���־
	int m_nLogDbEnable;
	std::string m_sLogDbServiceName;
	std::string m_sLogDbUserName;
	std::string m_sLogDbPassword;
	int m_nLogDbMin;
	int m_nLogDbMax;
	int m_nLogDbIncrease;
	std::string m_sDBInfo;
	int m_nLogDbThreadPool;


	// ������־
	int m_nLogGuiEnable;
	int m_nLogGuiShowCount;
	int m_nShowDebugLog;
	int m_nShowInfoLog;
	int m_nShowWarnLog;
	int m_nShowErrorLog;
	int m_nLogShowThreadPool;

	// �ֲ�ʽ��־ϵͳ
	int m_nLogMqEnable;
	int m_nLogMqThreadPool;
	int m_nLogMqMin;
	int m_nLogMqMax;
	int m_nLogMqIncrease;
	std::vector<std::string> m_vLogMqServer;


	// ���
	int m_nMonitorTcpPort;

	// ��������
	std::string m_sDumpPath;	


	int captcha_timeout;

	// OCR
	// tesseract��װ·��
	std::string OCRPath;
	std::string OCROutput;

	int m_nNameAlgorithm;
	int m_nNameThreshold;
	
	int m_nAddressAlgorithm;
	int m_nAddressThreshold;

	int m_nIDNoAlgorithm;
	int m_nIDNoThreshold;

	int m_nExpireAlgorithm;
	int m_nExpireThreshold;
	
	//std::map<std::string, BusinessSystem> systems;
	// ����ϵͳ��ţ�ҵ�����ͣ����ض�Ӧ�Ĺ�̨����
	int GetCounterType(std::string SystemNo, std::string busiType);

	int ConvertIntToBusiType(int val);
	int ConvertIntToCounterType(int val);
	

};
typedef boost::detail::thread::singleton<ConfigManager> gConfigManager;

#endif



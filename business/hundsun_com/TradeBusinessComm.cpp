

#include "StdAfx.h"
#include "TradeBusinessComm.h"
//#include "LogManager.h"
#include "./connectpool/ConnectManager.h"



#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string.hpp>

//#include "aes.h"

#include "./config/ConfigManager.h"



//#include "CacheData.h"


// ICU
//#include <unicode/putil.h>
//#include <unicode/ucnv.h>

#include "./encrypt/mybotan.h"


#include "./captcha/captchamanager.h"



#include "./output/FileLog.h"

#include "TradeGatewayGtja.h"


TradeBusinessComm::TradeBusinessComm(std::string type)
{


	//m_pConn = NULL;

	aeskey = "29dlo*%AO+3i16BaweTw.lc!)61K{9^5";

	SOH = '\x01';
	LF = '\x1A';

	this->type = type;
}



TradeBusinessComm::~TradeBusinessComm(void)
{
}


void TradeBusinessComm::Process(std::string& request, std::string& response, Trade::TradeLog& logMsg)
{
	/*
	// ��ʼ����ʱ��
	beginTime =  boost::posix_time::microsec_clock::local_time();

	int nRet = 0;





	logRequest = request;

	

	int rc = 0;

	std::vector<std::string> keyvalues;
	boost::split(keyvalues, request, boost::is_any_of("&")); // ע����Ҫͨ�������ļ�����

	// ȫ��ת��Сд
	//ȡ��ʱ��Ҳȫ���ĳ�Сд

	for (std::vector<std::string>::iterator it = keyvalues.begin(); it != keyvalues.end(); it++)
	{
		std::string keyvalue = *it;
		TRACE("keyvalue = %s\n", keyvalue.c_str());

		if (keyvalue.empty())
			break;

		std::vector<std::string> kv;
		boost::split(kv, keyvalue, boost::is_any_of("="));

		std::string key = "";
		if (!kv[0].empty())
			key = kv[0];

		std::string value = "";
		if (!kv[1].empty())
			value = kv[1];

		if (value!="" &&  value.find("amp;") != std::string::npos)
			boost::algorithm::replace_all(value, "amp;", "&");

		reqmap[key] = value;
	}

	source = reqmap["cssweb_os"];
	if (source == "web" || source == "flash" || source == "ios" || source == "android" || source == "windows")
	{
	}
	else
	{
		if (type == "web" || type == "flash")
		{
			source = type;
		}
		else
		{
			RetErrRes(Trade::TradeLog::ERROR_LEVEL, response, "1001", "cssweb_os������Ч");
			return;
		}
	}

	


	funcid = reqmap["function_id"];
	branchNo = reqmap["op_branch_no"];

//	if (funcid.empty())
//		funcid = reqmap["funcid"];

	

	
	



	long FuncNo = 0;
	long  BranchNo = 0;
	try
	{
		FuncNo = boost::lexical_cast<long>(funcid);

		BranchNo = boost::lexical_cast<long>(branchNo);
	}
	catch(boost::exception& e)
	{
		RetErrRes(Trade::TradeLog::ERROR_LEVEL, response, "1001", "ȱ�ٲ���function_id");
		return;
	}

	OutputDebugString("funcid=");
	OutputDebugString(funcid.c_str());
	OutputDebugString("\n");







	clientIp = "";

	//isSafe = reqmap["isSafe"];

	//flashreqcallback = reqmap["cssweb_type"];
	//TRACE("cssweb_type=%s\n", flashreqcallback.c_str());

	//trdpwd = reqmap["trdpwd"];
	//newpwd = reqmap["newpwd"];


	




	// flash���׽���
	if (funcid == "000000")
	{
		if (trdpwd != newpwd)
		{
			RetErrRes(Trade::TradeLog::ERROR_LEVEL, response,  "1001", "������󣬽���ʧ�ܣ�");
			return;
		}
//		else
//			goto UNLOCK_SUCCESS;
	}

	

	m_pConn = g_ConnectManager.GetConnect(sysNo, busiType, branchId);
	if (m_pConn == NULL)
	{
		RetErrRes(Trade::TradeLog::ERROR_LEVEL, response, "1000", "���ӹ�̨ʧ��");
		return;
	}
	gt_ip = m_pConn->m_Counter.m_sIP;
	gt_port = boost::lexical_cast<std::string>(m_pConn->m_Counter.m_nPort);

	
		int nFieldCount = 0;
	//nFieldCount = reqmap.count(;

	m_pConn->m_pComm->SetHead(BranchNo, FuncNo);
	m_pConn->m_pComm->SetRange(nFieldCount, 1);

	for (std::map<std::string, std::string>::iterator it = reqmap.begin(); it != reqmap.end(); it++)
	{
		std::string key = it->first;

		if (key == "function_id")
			continue;

		if (key == "cssweb_os")
			continue;

		m_pConn->m_pComm->AddField(key.c_str());

		OutputDebugString("key=");
		OutputDebugString(key.c_str());
		OutputDebugString("\n");
	}

	for (std::map<std::string, std::string>::iterator it = reqmap.begin(); it != reqmap.end(); it++)
	{
		std::string key = it->first;
		std::string value = it->second;

		if (key == "function_id")
			continue;

		if (key == "cssweb_os")
			continue;

		m_pConn->m_pComm->AddValue(value.c_str());

		OutputDebugString("value");
		OutputDebugString("=");
		OutputDebugString(value.c_str());
		OutputDebugString("\n");

	} // end for

		long nSenderId = 1;
	ComputePackID(&nSenderId);

	m_pConn->m_pComm->put_SenderId(nSenderId); // �˴��ǲ��ǿ��Ըĳɿͻ���֮��Ψһ����ͻ���ֵ, long���Ϳ��Բ���Ψ һ�Ŀͻ���

	nRet = m_pConn->m_pComm->Send();
	

	if (nRet < 0)
	{
		m_pConn->m_pComm->FreePack();

		long lErrNo = m_pConn->m_pComm->get_ErrorNo();
		std::string sErrMsg = m_pConn->m_pComm->get_ErrorMsg();

		RetErrRes(Trade::TradeLog::ERROR_LEVEL, response, boost::lexical_cast<std::string>(lErrNo), sErrMsg);
		return;
	}



	// ��������
	

	
	
	
	nRet = m_pConn->m_pComm->Receive();
	if (nRet != 0)
	{
		long lErrNo = m_pConn->m_pComm->get_ErrorNo();
		std::string sErrMsg = m_pConn->m_pComm->get_ErrorMsg();

		RetErrRes(Trade::TradeLog::ERROR_LEVEL, response, boost::lexical_cast<std::string>(lErrNo), sErrMsg);

		return;
	}

	long nRecvSenderId = m_pConn->m_pComm->get_SenderId();
	if (nRecvSenderId != nSenderId)
	{
		std::string sErrMsg = "sendid������recvid";
		RetErrRes(Trade::TradeLog::ERROR_LEVEL, response, boost::lexical_cast<std::string>(0), sErrMsg);

		return;
	}

	nRet = m_pConn->m_pComm->get_ErrorNo();
	if (nRet != 0)
	{
		long lErrNo = m_pConn->m_pComm->get_ErrorNo();
		std::string sErrMsg = m_pConn->m_pComm->get_ErrorMsg();

		RetErrRes(Trade::TradeLog::ERROR_LEVEL, response, boost::lexical_cast<std::string>(lErrNo), sErrMsg);

		return;

	}

	int nCols = m_pConn->m_pComm->get_FieldCount();
	for (int c=0; c<nCols; c++)
	{
		std::string sColName = m_pConn->m_pComm->GetFieldName(c);
		response += sColName;	
					
		if (c != (nCols-1))
		{
			response += SOH;
		}
		else
		{
			response += LF;
		}
	} // end for

	while (m_pConn->m_pComm->get_Eof() == 0)
	{
		int nCols = m_pConn->m_pComm->get_FieldCount();
		for (int c=0; c<nCols; c++)
		{
			std::string fieldName = m_pConn->m_pComm->GetFieldName(c);
			std::string fieldValue = m_pConn->m_pComm->FieldByName(fieldName.c_str());
			
			response += fieldValue;
		
			if (c != (nCols-1))
				response += SOH;

		}
			
		response += LF;


		m_pConn->m_pComm->MoveBy(1);
	} // end while
		


	

	if (response.length() == 0)
	{
		response = "errcode";
		response += SOH;
		response += "errmsg";
		response += LF;
		response += "1";
		response += SOH;
		response += "����ִ�гɹ���û�����ݷ��أ�";
		response += LF;
	}
	else
	{
	}		


	// ������־
	// ������־����,log��������cssweb::csswebmessage


	status = 1;

	this->retcode = "0";

	this->retmsg = "";

	if (response.length() > gConfigManager::instance().m_nResponseLen)
		logResponse = response.substr(0, gConfigManager::instance().m_nResponseLen);
	else
		logResponse = response;

	this->logLevel = Trade::TradeLog::INFO_LEVEL;

	if (source == "web")
	{
		gateway_port = boost::lexical_cast<std::string>(gConfigManager::instance().m_nTcpPort);
	}
	else if(source == "ios" || source == "android" || source == "windows")
	{
		gateway_port = boost::lexical_cast<std::string>(gConfigManager::instance().m_nSslPort);
	}
	*/
}


void TradeBusinessComm::RetErrRes(Trade::TradeLog::LogLevel logLevel, std::string& response, std::string retcode, std::string retmsg)
{
	//retmsgҪ����=

	if (type == "flash")
	{
	}
	else
	{
		// ��Ҫ����retmsg
		//boost::algorithm::replace_all(retmsg, "=", ":");

		response = "errcode";
		response += SOH;
		response += "errmsg";
		response += LF;
		response += retcode;
		response += SOH;
		response += retmsg;
		response += LF;
	}





/*
		// convert gbk to utf8
		UErrorCode errcode = U_ZERO_ERROR;
		char dest[1024];
		memset(dest, 0x00, sizeof(dest));
		int ret = ucnv_convert("utf8", "gbk", dest, sizeof(dest), retmsg.c_str(), -1, &errcode);
		row->add_value(dest);
		
		std::string str = "gbk to utf8 ����";
		str += boost::lexical_cast<std::string>(ret);
		str += "\n";

		OutputDebugString(str.c_str());
*/		
	


	// ��־����


	status = 0;

	this->retcode = retcode;

	this->retmsg = retmsg;

	if (response.length() > gConfigManager::instance().m_nResponseLen)
		logResponse = response.substr(0, gConfigManager::instance().m_nResponseLen);
	else
		logResponse = response;

	this->logLevel = logLevel;

	if (source == "web")
	{
		gateway_port = boost::lexical_cast<std::string>(gConfigManager::instance().m_nTcpPort);
	}
	else if(source == "ios" || source == "android" || source == "windows")
	{
		gateway_port = boost::lexical_cast<std::string>(gConfigManager::instance().m_nSslPort);
	}

}

void TradeBusinessComm::ComputePackID(long* lPackID)
{
	/*

	if (g_csPackID.Lock(INFINITE))
	{
	

		if (g_lPackID == (LONG_MAX -1))
			g_lPackID = 1;

		*lPackID = g_lPackID++;

		g_csPackID.Unlock();
	}

	*/
}

#include "stdafx.h"
//#define BOOST_FILESYSTEM_VERSION 2
// mongodb�õĻ����ϰ汾��ֻ֧��2��δ�����Դ�boost1.49

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/functional/factory.hpp>
//ѹ��
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>


#include "TradeServer.h"

#include "common.h"
#include "./config/configmanager.h"

// ��֤
#include "./business/szkingdom_win/tradebusiness.h"
#include "./business/hundsun_com/TradeBusinessComm.h"
// ����
#include "./business/hundsun_t2/tradebusinesst2.h"
// ����
#include "./business/apexsoft/TradeBusinessDingDian.h"
#include "./business/apexsoft/DingDian.h"
// AGC
#include "business/SunGuard/SywgConnect.h"
// ����
#include "business/xinyi/TCPClientSync.h"




#include "./output/FileLog.h"

#include "connectpool/connectmanager.h"




#include "log/FileLogManager.h"
#include "errcode.h"

#include "network/ssl_tcp/TcpSession.h"
#include "network/ssl_tcp/SSLSession.h"

#include "network/http/http_message.h"
#include "network/tcp/tcp_message_old.h"
#include "network/ssl/ssl_message.h"
#include "network/ssl_tcp/custommessage.h"


TradeServer::TradeServer(int msgType)
	:req_worker_(recvq_, boost::bind(&TradeServer::ProcessRequest, this, _1), gConfigManager::instance().m_nTcpWorkerThreadPool)
	,resp_worker_(sendq_, boost::bind(&TradeServer::ProcessResponse, this, _1), gConfigManager::instance().m_nTcpSendThreadPool)
	
{
	m_MsgType = msgType;
}





void TradeServer::start()
{
	req_worker_.start();
	resp_worker_.start();
}

void TradeServer::stop()
{
	req_worker_.stop();
	resp_worker_.stop();
}

TradeServer::req_queue_type& TradeServer::recv_queue()
{
	return recvq_;
}

// ����Ӧ��
bool TradeServer::ProcessResponse(IMessage* resp)
{
	
	resp->GetSession()->write(resp);

	return true;
}

// ���������������̺߳�������Ҫʹ�ù�������
bool TradeServer::ProcessRequest(IMessage* req)
{
	std::string SOH = "\x01";

	Trade::TradeLog::LogLevel logLevel = Trade::TradeLog::INFO_LEVEL;

//	MSG_HEADER binMsgHeader;
//	memcpy(&binMsgHeader, req->GetMsgHeader().data(), req->GetMsgHeaderSize());

	std::string sysNo = "";
	std::string busiType = "";
	int nBusiType;
	std::string sysVer = "";
	std::string funcId = "";
	int nFuncId = -1;
	std::string account = "";
	std::string clientIp = "";

	std::string gatewayIp = "";
	std::string gatewayPort = "";
	std::string gatewayServer = "";

	std::string counterIp = "";
	std::string counterPort = "";
	std::string counterType = "";
	int nCounterType;
	std::string counterServer = "";

	
	boost::posix_time::ptime ptBeginTime;
	std::string beginTime = "";
	int runtime = 0;


	std::string request = req->GetMsgContentString();
	std::string response = "";
	int status = 0;
	std::string errCode = "";
	std::string errMsg = "";

	if (m_MsgType == MSG_TYPE_TCP_OLD)
	{
		gatewayIp = gConfigManager::instance().m_sIp;

		gatewayPort = boost::lexical_cast<std::string>(gConfigManager::instance().m_nTcpPort);
	}
	if (m_MsgType == MSG_TYPE_SSL_PB)
	{
		gatewayIp = gConfigManager::instance().m_sIp;

		gatewayPort = boost::lexical_cast<std::string>(gConfigManager::instance().m_nSslPort);
	}
	if (m_MsgType == MSG_TYPE_TCP_NEW)
	{
		gatewayIp = gConfigManager::instance().m_sIp;

		gatewayPort = boost::lexical_cast<std::string>(gConfigManager::instance().m_nTcpNewPort);
	}
	if (m_MsgType == MSG_TYPE_SSL_NEW)
	{
		gatewayIp = gConfigManager::instance().m_sIp;

		gatewayPort = boost::lexical_cast<std::string>(gConfigManager::instance().m_nSslNewPort);
	}
	gatewayServer = gatewayIp + ":" + gatewayPort;
			
							
			
			
	/*

	// �����Ϣ���Ͳ�����������
	if (binMsgHeader.MsgType != MSG_TYPE_REQUEST)
	{
		logLevel = Trade::TradeLog::ERROR_LEVEL;

		errCode = boost::lexical_cast<std::string>(MSG_HEADER_ERROR);
		errMsg = gError::instance().GetErrMsg(MSG_HEADER_ERROR);

		response = "1" + SOH + "2" + SOH;
		response += "cssweb_code";
		response += SOH;
		response += "cssweb_msg";
		response += SOH;
		response += errCode;
		response += SOH;
		response += errMsg;
		response += SOH;

		
		goto finish;
	}
	*/

	// �ͻ�����������
	if (nFuncId == FUNCTION_HEARTBEAT)
	{
		response = "heartbeat";

		goto finish;
	}

	

	
	if (!GetSysNoAndBusiType(request, sysNo, busiType, sysVer, account, funcId, clientIp))
	{
		logLevel = Trade::TradeLog::ERROR_LEVEL;

		errCode = boost::lexical_cast<std::string>(PARAM_ERROR);
		errMsg = gError::instance().GetErrMsg(PARAM_ERROR);

		response = "1" + SOH + "2" + SOH;
		response += "cssweb_code";
		response += SOH;
		response += "cssweb_msg";
		response += SOH;
		response += errCode;
		response += SOH;
		response += errMsg;
		response += SOH;

		goto finish;
	}

	nBusiType = boost::lexical_cast<int>(busiType);

	// �õ���̨����
	nCounterType = g_ConnectManager.GetCounterType(sysNo, busiType);
	if (nCounterType == COUNTER_TYPE_UNKNOWN)
	{
	}

	/*
	// ��ʼ����̨����
	if (req->GetSession()->GetCounterConnect() == NULL)
	{
		Counter * counter = NULL;
		counter = g_ConnectManager.GetServer(sysNo, gConfigManager::instance().ConvertIntToBusiType(nBusiType), "0000");
		if (counter == NULL)
		{
			logLevel = Trade::TradeLog::ERROR_LEVEL;

			errCode = boost::lexical_cast<std::string>(CONFIG_ERROR);
			errMsg = gError::instance().GetErrMsg(CONFIG_ERROR);

			response = "1" + SOH + "2" + SOH;
			response += "cssweb_code";
			response += SOH;
			response += "cssweb_msg";
			response += SOH;
			response += errCode;
			response += SOH;
			response += errMsg;
			response += SOH;


			goto finish;
		}
				
		

		req->GetSession()->GetCounterConnect()->SetCounterServer(counter);

		counterIp = counter->m_sIP;
		counterPort = boost::lexical_cast<std::string>(counter->m_nPort);
		counterType = GetCounterType(counter->m_eCounterType);
	}
	else
	{
		counterIp = req->GetSession()->GetCounterConnect(nCounterType)->m_Counter->m_sIP;
		counterPort = boost::lexical_cast<std::string>(req->GetSession()->GetCounterConnect(nCounterType)->m_Counter->m_nPort);
		counterType = GetCounterType(req->GetSession()->GetCounterConnect(nCounterType)->m_Counter->m_eCounterType);
	}
	*/
	
	int serverCount = g_ConnectManager.GetServerCount(sysNo, gConfigManager::instance().ConvertIntToBusiType(nBusiType), "0000");
	if (serverCount == 0)
	{
	}



	// ������Ҫ�����ؾ���͹����л�
	// ҵ�����Ի���Ҫ������
	/*
	* ���û�����ӣ��������ӣ���������
	* �����������ʧ�ܣ���ѯÿһ����������������з���������ʧ�ܣ����ش���
	* ��ǰ����ʧЧ��send����ʧ�ܣ�����
	*/


	
	
	// ҵ������
	ptBeginTime = boost::posix_time::microsec_clock::local_time();
	beginTime = boost::gregorian::to_iso_extended_string(ptBeginTime.date()) + " " + boost::posix_time::to_simple_string(ptBeginTime.time_of_day());;

	for (int retry=0; retry<1; retry++)
	{
		bool bConnect = false;
	
		// ��ѯÿһ��������
		for (int i=0; i<serverCount; i++)
		{
			if (req->GetSession()->GetCounterConnect(nCounterType)->IsConnected())
			{
				

				// �ѽ������ӣ�����ѭ��
				bConnect = true;
				break;
			}
			else
			{
				ptBeginTime = boost::posix_time::microsec_clock::local_time();
				beginTime = boost::gregorian::to_iso_extended_string(ptBeginTime.date()) + " " + boost::posix_time::to_simple_string(ptBeginTime.time_of_day());;

				Counter * counter = NULL;
				counter = g_ConnectManager.GetServer(sysNo, gConfigManager::instance().ConvertIntToBusiType(nBusiType), "0000");
				counterIp = counter->m_sIP;
				counterPort = boost::lexical_cast<std::string>(counter->m_nPort);
				counterType = GetCounterType(counter->m_eCounterType);
				
				req->GetSession()->GetCounterConnect(nCounterType)->SetCounterServer(counter);

				if (req->GetSession()->GetCounterConnect(nCounterType)->CreateConnect())
				{
					// �������ӳɹ�������ѭ��
					bConnect = true;
					break;
				}
				else
				{
					// ��������ʧ��
					// ��ѯ�㷨��������һ��������
					bConnect = false;

					//Counter * counter = NULL;
					//counter = g_ConnectManager.GetServer(sysNo, gConfigManager::instance().ConvertIntToBusiType(nBusiType), "0000");
					//req->GetSession()->GetCounterConnect(nCounterType)->SetCounterServer(counter);

					
					
					logLevel = Trade::TradeLog::ERROR_LEVEL;

					errCode = boost::lexical_cast<std::string>(CONNECT_COUNTER_ERROR);
					errMsg = gError::instance().GetErrMsg(CONNECT_COUNTER_ERROR);

					boost::posix_time::ptime ptEndTime = boost::posix_time::microsec_clock::local_time();
					runtime = (ptEndTime - ptBeginTime).total_microseconds();// ΢����

					req->Log(Trade::TradeLog::ERROR_LEVEL, sysNo, sysVer, busiType, funcId, account, clientIp, request, response, status, errCode, errMsg, beginTime, runtime, gatewayIp, gatewayPort, counterIp, counterPort, counterType);
					gFileLogManager::instance().push(req->log);
				} // end if
			} // end if
		} // end for (int i=0; i<serverCount; i++)

		// ���з��������Ӳ���
		if (!bConnect)
		{
			logLevel = Trade::TradeLog::ERROR_LEVEL;

			errCode = boost::lexical_cast<std::string>(CONNECT_ALL_COUNTER_ERROR);
			errMsg = gError::instance().GetErrMsg(CONNECT_ALL_COUNTER_ERROR);

			response = "1" + SOH + "2" + SOH;
			response += "cssweb_code";
			response += SOH;
			response += "cssweb_msg";
			response += SOH;
			response += errCode;
			response += SOH;
			response += errMsg;
			response += SOH;

			
			goto finish;
		}

		// ÿ��������Ҫ��¼��̨��ip��port
				Counter * counter = req->GetSession()->GetCounterConnect(nCounterType)->m_Counter;
				counterIp = counter->m_sIP;
				counterPort = boost::lexical_cast<std::string>(counter->m_nPort);
				counterType = GetCounterType(counter->m_eCounterType);
				counterServer = counterIp + ":"+ counterPort;
		
		// ����ҵ��ҵ��ʧ�ܻ�ɹ�����ɹ��ģ�ֻ��ͨ��ʧ�ܲ���Ҫ����
		if (req->GetSession()->GetCounterConnect(nCounterType)->Send(request, response, status, errCode, errMsg))
		{
			// ҵ��ʧ�ܣ��ع�response
			if (status == 0)
			{
				response = "1";
				response += SOH;
				response += "4";
				response += SOH;

				response += "cssweb_code";
				response += SOH;
				response += "cssweb_msg";
				response += SOH;
				response += "cssweb_gwInfo";
				response += SOH;
				response += "cssweb_counter";
				response += SOH;

				response += errCode;
				response += SOH;
				response += errMsg;
				response += SOH;
				response += gatewayServer;
				response += SOH;
				response += counterServer;
				response += SOH;
			}

			logLevel = Trade::TradeLog::INFO_LEVEL;

			boost::posix_time::ptime ptEndTime = boost::posix_time::microsec_clock::local_time();
			runtime = (ptEndTime - ptBeginTime).total_microseconds();// ΢����



			break;
		}
		else
		{
			continue;
			// ͨ��ʧ��,��ʼ����
			// ����һ�Σ�дһ����־
			// fileLog.push(req->log)
			//req->Log(Trade::TradeLog::, sysNo, sysVer, busiType, funcId, account, clientIp, request, response, status, errCode, errMsg, beginTime, runtime, gatewayIp, gatewayPort, counterIp, counterPort, counterType);
		}// end if
	} // end for retry
	



finish:

	std::vector<char> compressedMsgContent;
	

	

	IMessage * resp = NULL;
	
	//std::vector<char> msgHeader;

	switch(req->msgType)
	{
	case MSG_TYPE_HTTP:
		{
			
		resp = new http_message();
		// ������Ϣ����
		resp->SetMsgContent(response);

		}
		break;
	case MSG_TYPE_TCP_OLD:
		{
		resp = new tcp_message_old();
		int msgHeaderSize = response.size();
		msgHeaderSize = htonl(msgHeaderSize);
		memcpy(&(resp->m_MsgHeader.front()), &msgHeaderSize, 4);
		// ������Ϣ����
		resp->SetMsgContent(response);

		}
		break;
	case MSG_TYPE_SSL_PB:
		{
		resp = new ssl_message();

		quote::PkgHeader pbHeader;

		if (response.size() > gConfigManager::instance().zlib)
		{
			boost::iostreams::filtering_streambuf<boost::iostreams::output> compress_out;
			compress_out.push(boost::iostreams::zlib_compressor());
			compress_out.push(boost::iostreams::back_inserter(compressedMsgContent));
			boost::iostreams::copy(boost::make_iterator_range(response), compress_out);

			pbHeader.set_zip(true);
			int compressedMsgContentSize = compressedMsgContent.size();
			pbHeader.set_bodysize(compressedMsgContentSize);
			// ������Ϣ����
			resp->SetMsgContent(compressedMsgContent);

			
		}
		else
		{
			pbHeader.set_zip(false);
			pbHeader.set_bodysize(response.size());
			// ������Ϣ����
			resp->SetMsgContent(response);

		}
		
		pbHeader.set_ver(1);
		pbHeader.set_enc(false);
		
		pbHeader.set_more(false);
		pbHeader.set_msgtype(quote::PkgHeader::RES_TRADE);
		pbHeader.set_errcode(0);
		
		
		bool bTmp = pbHeader.SerializeToArray(&(resp->m_MsgHeader.front()), pbHeader.ByteSize());
			
		}
		break;
	case MSG_TYPE_TCP_NEW:
		{
		resp = new CustomMessage();

		MSG_HEADER binRespMsgHeader;
		binRespMsgHeader.CRC = 0;
		binRespMsgHeader.FunctionNo = nFuncId;
		
		binRespMsgHeader.MsgType = MSG_TYPE_RESPONSE_END;

		if (response.size() > gConfigManager::instance().zlib)
		{
			boost::iostreams::filtering_streambuf<boost::iostreams::output> compress_out;
			compress_out.push(boost::iostreams::zlib_compressor());
			compress_out.push(boost::iostreams::back_inserter(compressedMsgContent));
			boost::iostreams::copy(boost::make_iterator_range(response), compress_out);

			binRespMsgHeader.zip = 1;
			int compressedMsgContentSize = compressedMsgContent.size();
			binRespMsgHeader.MsgContentSize = compressedMsgContentSize;
			// ������Ϣ����
			resp->SetMsgContent(compressedMsgContent);

		}
		else
		{
			binRespMsgHeader.zip = 0;
			binRespMsgHeader.MsgContentSize = response.size();
			// ������Ϣ����
			resp->SetMsgContent(response);

		}

		//msgHeader.resize(sizeof(MSG_HEADER));

		memcpy(&(resp->m_MsgHeader.front()), &binRespMsgHeader, sizeof(MSG_HEADER));
		}
		break;
	}

	// ������־��Ϣ
	req->Log(logLevel, sysNo, sysVer, busiType, funcId, account, clientIp, request, response, status, errCode, errMsg, beginTime, runtime, gatewayIp, gatewayPort, counterIp, counterPort, counterType);
	resp->log = req->log; 


	// ���ûỰ
	resp->SetSession(req->GetSession());

	
	

	

	
		

	// ������Ϣͷ
	//resp->SetMsgHeader(msgHeader);

	
	

	// �ͷ�����
	req->destroy();

	sendq_.push(resp);

	
	return true;
}

bool TradeServer::GetSysNoAndBusiType(std::string& request, std::string& sysNo, std::string& busiType, std::string& sysVer, std::string& account, std::string& funcId, std::string& clientIp)
{
	
	std::string SOH = "\x01";

	std::vector<std::string> keyvalues;
	boost::split(keyvalues, request, boost::is_any_of(SOH));


	for (std::vector<std::string>::iterator it = keyvalues.begin(); it != keyvalues.end(); it++)
	{
		std::string keyvalue = *it;


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


		if (key == "cssweb_sysNo")
			sysNo = value;

		if (key == "cssweb_busiType")
			busiType = value;

		if (key == "cssweb_sysVer")
		{
			sysVer = value;
		}

		if (key == "cssweb_funcid")
		{
			funcId = value;
		}
		
		if (key == "cssweb_account")
		{
			account = value;
		}

	}

	if (sysNo.empty() || busiType.empty())
		return false;

	return true;
}

std::string TradeServer::GetCounterType(int counterType)
{
	switch (counterType)
	{
	case COUNTER_TYPE_HS_T2:
		return "1";
	case COUNTER_TYPE_HS_COM:
		return "2";
	case COUNTER_TYPE_JZ_WIN:
		return "3";
	case COUNTER_TYPE_JZ_LINUX:
		return "4";
	case COUNTER_TYPE_DINGDIAN:
		return "5";
	case COUNTER_TYPE_JSD:
		return "6";
	case COUNTER_TYPE_XINYI:
		return "7";
	default:
		return "0";
	}
}
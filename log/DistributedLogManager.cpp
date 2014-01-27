#include "DistributedLogManager.h"

#include "./config/ConfigManager.h"

#include "./output/FileLog.h"

// xml
#include "./lib/xml/rapidxml.hpp"
#include "./lib/xml/rapidxml_utils.hpp"

#include "log/mq/LogConnect.h"
#include "log/mq/LogConnectPool.h"


DistributedLogManager::DistributedLogManager(void)
	:kafka_worker_(kafka_q_, boost::bind(&DistributedLogManager::kafka_log, this, _1), gConfigManager::instance().m_nLogMqThreadPool)
{
}


DistributedLogManager::~DistributedLogManager(void)
{
}


void DistributedLogManager::start()
{
	
	kafka_worker_.start();
	
}

void DistributedLogManager::stop()
{
	
	kafka_worker_.stop();
	
}

void DistributedLogManager::push(Trade::TradeLog * log)
{
	
	if (gConfigManager::instance().m_nLogMqEnable)
	{
		
		kafka_q_.push(log);
	}
	
}

bool DistributedLogManager::kafka_log(Trade::TradeLog* log)
{
/*
{
  "logType":"trade",  //��־���͡�'trade':������־
  "message":{
    "logLevel":"warn", //��־����
    "request":"", //��������
    "requestTime":"2012-05-10 13:00:00", //����ʱ���
    "requestRuntime":"", //�����ʱ����λ������
    "status":"0", //����״̬��1���ɹ���0��ʧ��
    "response":"", //����������Ӧ��
    "sourceSysNo":"njzq_jlp", //����������Դϵͳ���
    "sourceSysVer":"iphone", //����������Դϵͳ�汾
    "counterIp":"", //��̨IP
    "counterPort":"", //��̨�˿�
    "gatewayIp":"", //��������IP
    "gatewayPort":"" //�������ض˿�
  }
}
*/

	std::string json;
	json = "{";

	std::string SOH = "\x01";

	

	std::string funcid = log->funcid();
	// ��������¼
	if (funcid == "999999")
	{
		delete log;
		return true;
	}

	std::string countertype = log->countertype();

	// ����t2
	if (countertype == "1")
	{
		// ��������˹��˲�ѯ���ܺ�
		if (gConfigManager::instance().m_nFilterFuncId)
		{
			std::map<std::string, FUNCTION_DESC>::iterator it = m_mT2_FilterFunc.find(funcid);
			if (it != m_mT2_FilterFunc.end())
			{
				// ���˲�ѯ���ܺ�
				if (it->second.isQuery)
				{
					//log->destroy();
					delete log;
					return true;
				}
			}
		}
	}
	
	// ��֤
	if (countertype == "3")
	{
		// ��������˹��˲�ѯ���ܺ�
		if (gConfigManager::instance().m_nFilterFuncId)
		{
			std::map<std::string, FUNCTION_DESC>::iterator it = m_mKingdom_FilterFunc.find(funcid);
			if (it != m_mKingdom_FilterFunc.end())
			{
				// ���˲�ѯ���ܺ�
				if (it->second.isQuery)
				{
					//log->destroy();
					delete log;
					return true;
				}
			}
		}
	}

	// ����
	if (countertype == "5")
	{
		// ��������˹��˲�ѯ���ܺ�
		if (gConfigManager::instance().m_nFilterFuncId)
		{
			std::map<std::string, FUNCTION_DESC>::iterator it = m_mDingDian_FilterFunc.find(funcid);
			if (it != m_mDingDian_FilterFunc.end())
			{
				// ���˲�ѯ���ܺ�
				if (it->second.isQuery)
				{
					//log->destroy();
					delete this;
					return true;
				}
			}
		}
	}

	std::string request = log->request();
	std::map<std::string, std::string> reqmap;

	// �����ֶ�
	if (countertype == "1")
	{
		GetFilterMap(request, m_mT2_FilterField, reqmap);
	}
	else if (countertype == "3")
	{
		GetFilterMap(request, m_mKingdom_FilterField, reqmap);
	}
	else if (countertype == "5")
	{
		GetFilterMap(request, m_mDingDian_FilterField, reqmap);
	}



	std::string sFilterRequest = "";
	for (std::map<std::string, std::string>::iterator it = reqmap.begin(); it != reqmap.end(); it++)
	{
		sFilterRequest += it->first;
		sFilterRequest += "=";
		sFilterRequest += it->second;
		sFilterRequest += SOH;
	}
	json += "\"request\":\"";
	json += sFilterRequest;
	json += "\",";


	std::string sysNo = log->sysno();
	if (sysNo.empty())
		sysNo = "no_sysno";
	json += "\"sourceSysNo\":\"";
	json += sysNo;
	json += "\",";

	std::string sysVer = log->sysver();
	if (sysVer.empty())
		sysVer = "no_sysver";
	json += "\"sourceSysVer\":\"";
	json += sysVer;
	json += "\",";

	std::string busiType = log->busitype();
	if (busiType.empty())
		busiType = "no_busitype";
	json += "\"busiType\":\"";
	json += busiType;
	json += "\",";


	// �˻�����־����
	//std::string account = logMsg.account();
	//if (account.empty())
	//	sLogFileName += "no_account";
	//else
	//	sLogFileName += account;

	json += "\"logLevel\":\"";
	switch (log->level())
	{
	case Trade::TradeLog::DEBUG_LEVEL:
		json += "debug";
		break;
	case Trade::TradeLog::INFO_LEVEL:
		json += "info";
		break;
	case Trade::TradeLog::WARN_LEVEL:
		json += "warn";
		break;
	case Trade::TradeLog::ERROR_LEVEL:
		json += "error";
		break;
	default:
		json += "error";
	}
	json += "\",";


	json += "\"requestTime\":\"";
	json += log->recvtime();
	json += "\",";

		
	json += "\"requestRuntime\":\"";
	json += boost::lexical_cast<std::string>(log->runtime());
	json += "\",";

	json += "\"status\":\"";
	json += boost::lexical_cast<std::string>(log->status());
	json += "\",";
	
	json += "\"response\":\"";
	json += log->response();
	json += "\",";

	json += "\"counterIp\":\"";
	json += log->gtip();
	json += "\",";

	json += "\"counterPort\":\"";
	json += log->gtport();
	json += "\",";

	json += "\"gatewayIp\":\"";
	json += log->gatewayip();
	json += "\",";

	json += "\"gatewayPort\":\"";
	json += log->gatewayport();
	json += "\""; // ע�⣺���û�ж���

	json += "}";

	LogConnect * pConnect = gLogConnectPool::instance().GetConnect();
	if (pConnect)
	{
		std::string response;

		if (!pConnect->Send(json, response))
		{
			std::string file = "�ֲ�ʽ��־.log";
			gFileLog::instance().Log("д��־ʧ�ܣ���������ʧ��", file);
		}

		// �黹����
		gLogConnectPool::instance().PushConnect(pConnect);
	}
	else
	{
		std::string file = "�ֲ�ʽ��־.log";
		gFileLog::instance().Log("д��־ʧ�ܣ�����ʧ��", file);
	}


	// �ͷ�
	//log->destroy();
	delete log;

	return true;
}

void DistributedLogManager::GetFilterMap(std::string& request, std::map<std::string, std::string>& mapFieldFilter, std::map<std::string, std::string>& reqmap)
{
	std::string SOH = "\x01";

	std::vector<std::string> keyvalues;
	boost::split(keyvalues, request, boost::is_any_of(SOH)); // ע����Ҫͨ�������ļ�����

	
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

		// ��������˹��˹ؼ��ֶ�
		if (gConfigManager::instance().m_nFilterField)
		{
			if (mapFieldFilter.find(key) != mapFieldFilter.end())
				continue;
		}



		std::string value = "";
		if (!kv[1].empty())
			value = kv[1];

		reqmap[key] = value;
	}
}


void DistributedLogManager::LoadFieldFilter()
{
	std::string xmlfile = "";
	
	// ����
	xmlfile = gConfigManager::instance().m_sPath + "\\apex_filterfield.xml";
	LoadFieldFilter(xmlfile, m_mDingDian_FilterField);

	// ����T2
	xmlfile = gConfigManager::instance().m_sPath + "\\hundsun_t2_filterfield.xml";
	LoadFieldFilter(xmlfile, m_mT2_FilterField);

	// ��֤
	xmlfile = gConfigManager::instance().m_sPath + "\\kingdom_win_filterfield.xml";
	LoadFieldFilter(xmlfile, m_mKingdom_FilterField);

}


void DistributedLogManager::LoadFuncFilter()
{
	std::string xmlfile = "";
	
	// ����
	xmlfile = gConfigManager::instance().m_sPath + "\\apex_filterfunc.xml";
	LoadFuncFilter(xmlfile, m_mDingDian_FilterFunc);

	// ����T2
	xmlfile = gConfigManager::instance().m_sPath + "\\hundsun_t2_filterfunc.xml";
	LoadFuncFilter(xmlfile, m_mT2_FilterFunc);

	// ��֤
	xmlfile = gConfigManager::instance().m_sPath + "\\kingdom_win_filterfunc.xml";
	LoadFuncFilter(xmlfile, m_mKingdom_FilterFunc);
}

// ���������ֶ�
void DistributedLogManager::LoadFieldFilter(std::string& sFieldFilterXML, std::map<std::string, std::string>& mapFieldFilter)
{
	rapidxml::file<> f(sFieldFilterXML.c_str());

	rapidxml::xml_document<> doc;

	doc.parse<0>(f.data());

	rapidxml::xml_node<> * root = doc.first_node();


	

	rapidxml::xml_node<> * filter = root->first_node();
	while (filter != NULL)
	{
		//TRACE("name1 = %s\n", filter->name());

		rapidxml::xml_node<>* fields = filter->first_node();
		while (fields != NULL)
		{
			//TRACE("\tname2 = %s\n", fields->name());

			rapidxml::xml_node<>* field = fields->first_node();
			while (field != NULL)
			{
				//TRACE("\t\tname3 = %s\n", field->name());

				std::string sFieldName = field->value();
				mapFieldFilter[sFieldName] = ""; // value���������Ժ���չ��

				field = field->next_sibling();
			}

			fields = fields->next_sibling();
		} // end while

		filter = filter->next_sibling();
	} // end while
}

// ���˲�ѯ�๦�ܺ�
void DistributedLogManager::LoadFuncFilter(std::string& sFuncFilterXML, std::map<std::string, FUNCTION_DESC>& mapFuncFilter)
{
	rapidxml::file<> f(sFuncFilterXML.c_str());

	rapidxml::xml_document<> doc;

	doc.parse<0>(f.data());

	rapidxml::xml_node<> * root = doc.first_node();

	
	

	rapidxml::xml_node<> * filter = root->first_node();
	while (filter != NULL)
	{
		//TRACE("name1 = %s\n", filter->name());
		

		rapidxml::xml_node<> * functions = filter->first_node();
		while (functions != NULL)
		{
			//TRACE("\tname2 = %s\n", functions->name());
			if (strcmp(functions->name(), "function") == 0)
			{

			//rapidxml::xml_node<> * function = functions->first_node();
			//while (function != NULL)
			//{
			//	TRACE("\t\tname3 = %s\n", function->name());

				FUNCTION_DESC funcDesc;

				std::string sFuncId = functions->first_attribute("id")->value();
				std::string sIsQuery = functions->first_attribute("isQuery")->value();
				std::string sHasResultSet = functions->first_attribute("hasResultSet")->value();
			
				if (sIsQuery == "true")
					funcDesc.isQuery = true;
				else
					funcDesc.isQuery = false;

				if (sHasResultSet == "true")
					funcDesc.hasResultRet = true;
				else
					funcDesc.hasResultRet = false;

				mapFuncFilter[sFuncId] = funcDesc;
			}

			//	function = function->next_sibling();
			//}

			functions = functions->next_sibling();
		}

		filter = filter->next_sibling();
	} // end while
}


#include "StdAfx.h"
#include "TradeBusinessT2.h"
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




TradeBusinessT2::TradeBusinessT2()
{
	m_pConn = NULL;

	aeskey = "29dlo*%AO+3i16BaweTw.lc!)61K{9^5";

	sCounterType = "1";
}



TradeBusinessT2::~TradeBusinessT2(void)
{
}
/*
std::string TradeBusinessT2::GetTradePWD(std::string isSafe, std::string sEncPwd)
{

	// ��Ϊbase64�п��ܲ���=�������ԭ�ȵĸ�ʽ
	boost::algorithm::replace_all(sEncPwd, "$", "=");

	std::string sPwd = "";

	if (isSafe == "0")
	{
		// ��׼��¼����weblogic��Կ����
		std::string algo = "AES-256/ECB/PKCS7";
		std::string pubKey = "23dpasd23d-23l;df9302hzv/3lvjh*5";
		std::string cipher = sEncPwd;
		
		bool bRet = g_MyBotan.Base64Decoder_AESDecrypt(algo, pubKey, cipher, sPwd);
	}
	else if (isSafe == "1")
	{
		// ��ǿ��¼���ÿؼ���Կ����
		sPwd = GetOtherPWD("1", sEncPwd);
	}
	else
	{
		// windows phone
		return sEncPwd;
	}

	return sPwd;
}

std::string TradeBusinessT2::GetOtherPWD(std::string isSafe, std::string sEncPwd)
{
	// ��Ϊbase64�п��ܲ���=�������ԭ�ȵĸ�ʽ
	boost::algorithm::replace_all(sEncPwd, "$", "=");

	std::string sPwd = "";

	if (isSafe == "0")
	{
		char decoder[50];
		memset(decoder, 0x00, sizeof(decoder));
		int outlen;

		g_MyBotan.Base64Decoder(sEncPwd, decoder, &outlen);
		sPwd = decoder;
	}
	else if (isSafe == "1")
	{
			// ��ǿ��¼���ÿؼ���Կ����
		

		int outlen;
		char out[256];
		memset(out, 0, sizeof(out));
		//int outlen = sEncPwd.length()/2;
		//outlen = HexDataToBinData((unsigned char*)sEncPwd.c_str(), sEncPwd.length(), (unsigned char*)out, sizeof(out));


		std::string algo = "AES-256/ECB/PKCS7";
		std::string pubKey = "29dlo*%AO+3i16BaweTw.lc!)61K{9^5";
		bool bRet = g_MyBotan.AESDecrypt(algo, pubKey, (const unsigned char*)out, outlen, sPwd);


	}
	else
	{
		// windows phone
		return sEncPwd;
	}

	return sPwd;
}
*/
void TradeBusinessT2::Process(std::string& request, std::string& response, Trade::TradeLog& logMsg)
{
	// ��ʼ����ʱ��
	

	int nRet = 0;

	BeginLog(request);
	



	ParseRequest(request);

	if (route.empty())
	{
		nRoute = -1;
	}
	else
	{
		try
		{
			nRoute = boost::lexical_cast<int>(route);
		}
		catch(boost::exception& e)
		{
			RetErrRes(Trade::TradeLog::ERROR_LEVEL, response, "1001", "ȱ�ٲ���cssweb_route");
			goto FINISH;
		}
	}

	
	try
	{
		lFuncId = boost::lexical_cast<long>(funcid);
	}
	catch(boost::exception& e)
	{
		RetErrRes(Trade::TradeLog::ERROR_LEVEL, response, "1001", "ȱ�ٲ���cssweb_funcid");
		goto FINISH;
	}



	if (sysVer == "windows" && funcid == "331100")
	{
		 
		if (!captcha::instance().VerifyCaptcha(captcha))
		{
			RetErrRes(Trade::TradeLog::ERROR_LEVEL, response, "1002", "��֤������ʧЧ");
			goto FINISH;
		}
	}




	


/*
	// ������
	if (funcid == "105001")
	{
		if (type == "web" && g_CacheData.m_sFundCompany_Web != "")
		{
			response = g_CacheData.m_sFundCompany_Web;
			m_bEnableSendLog = false;
			return response;
		}

		if ( (type == "mobile" || type == "flash") && g_CacheData.m_sFundCompany_Json != "" )
		{
			response = g_CacheData.m_sFundCompany_Json;
			m_bEnableSendLog = false;
			return response;
		}

		
	}
	if (funcid == "105002")
	{
		if (type == "web" && g_CacheData.m_sFund_Web != "")
		{
			response = g_CacheData.m_sFund_Web;
			m_bEnableSendLog = false;
			return response;
		}

		if ( (type == "mobile" || type == "flash") && g_CacheData.m_sFund_Json != "" )
		{
			response = g_CacheData.m_sFund_Json;
			m_bEnableSendLog = false;
			return response;
		}
	}
	// ������
*/





	IF2Packer* pack = NewPacker(2);
	pack->AddRef();
	pack->BeginPack();
	for (std::map<std::string, std::string>::iterator it = reqmap.begin(); it != reqmap.end(); it++)
	{
		std::string key = it->first;

		if (FilterRequestField(key))
		{
			continue;
		}		


		//pack->AddField(key.c_str());
		pack->AddField(key.c_str(), 'S', 8000);

	}

	for (std::map<std::string, std::string>::iterator it = reqmap.begin(); it != reqmap.end(); it++)
	{
		std::string key = it->first;
		std::string value = it->second;

		if (FilterRequestField(key))
		{
			continue;
		}		

		if (key.compare("auth_key") == 0)
		{
			boost::algorithm::replace_all(value, "^", "=");
			//std::string temp;
			//temp = "auth_key=";
			//temp += value;
			//gFileLog::instance().Log(temp);
		}

		if (key.compare("usbkey_request_info") == 0)
		{
			boost::algorithm::replace_all(value, "^", "=");
			//std::string temp;
			//temp = "auth_key=";
			//temp += value;
			//gFileLog::instance().Log(temp);
		}

		if (key == "op_station")
		{
			value = note;
		}

		pack->AddStr(value.c_str());

	} // end for
	pack->EndPack();

/*
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
*/

	

	m_pConn = g_ConnectManager.GetConnect(sysNo, busiType, branchId);
	if (m_pConn == NULL)
	{
		RetErrRes(Trade::TradeLog::ERROR_LEVEL, response, "1000", "���ӹ�̨ʧ��");
		goto FINISH;
	}
	gt_ip = m_pConn->m_Counter.m_sIP;
	gt_port = boost::lexical_cast<std::string>(m_pConn->m_Counter.m_nPort);

	
	//nRet = m_pConn->lpConnection->SendBiz(boost::lexical_cast<int>(funcid), pack, 0, boost::lexical_cast<int>(system)); //  ͬ��


	if (nRoute == -1)
		nRet = m_pConn->lpConnection->SendBiz(lFuncId, pack); //  ͬ��
	else
		nRet = m_pConn->lpConnection->SendBiz(lFuncId, pack, 0, nRoute); //  ͬ��

	if (nRet < 0)
	{
		pack->Release();

		std::string sErrMsg = m_pConn->lpConnection->GetErrorMsg(nRet);

		RetErrRes(Trade::TradeLog::ERROR_LEVEL, response, boost::lexical_cast<std::string>(nRet), sErrMsg);
		goto FINISH;
	}


	pack->FreeMem(pack->GetPackBuf());
	pack->Release();

	// ��������
	void *Pointer = NULL;

	//nRet = m_pConn->lpConnection->RecvBiz(nRet, &Pointer, gConfigManager::instance().m_nConnectTimeout, 0);
	
	
	nRet = m_pConn->lpConnection->RecvBiz(nRet, &Pointer, m_pConn->m_Counter.m_nRecvTimeout*1000);


	// ���سɹ�
	if (nRet == 0)
	{
		IF2UnPacker *lpUnPacker = (IF2UnPacker *)Pointer;

		

		int nRows = lpUnPacker->GetRowCount();
		int nCols = lpUnPacker->GetColCount();

		if (nRows == 0)
		{
			RetNoRecordRes(response);
			goto FINISH;
		}
		
		/*
		std::map<std::string, FUNCTION_DESC>::iterator it = sLogManager::instance().m_mT2_FilterFunc.find(funcid);
		if (it != sLogManager::instance().m_mT2_FilterFunc.end())
		{
			// ��Ӧ���н��������
			if (!it->second.hasResultRet)
			{
				gFileLog::instance().Log(funcid + "��Ӧ�÷��ؽ����");

				RetNoRecordRes(response);
				goto FINISH;
			}
		}
		*/

		response = boost::lexical_cast<std::string>(nRows);
		response += SOH;
		response += boost::lexical_cast<std::string>(nCols);
		response += SOH;
	
		for (int i = 0; i < lpUnPacker->GetDatasetCount(); ++i)
		{
			// ���õ�ǰ�����
			lpUnPacker->SetCurrentDatasetByIndex(i);

			for (int c = 0; c < nCols; c++)
			{
				std::string sColName = lpUnPacker->GetColName(c);
					
				response += sColName;
					
				response += SOH;
			} // end all column

			// ��ӡ���м�¼
			for (int r = 0; r < nRows; r++)
			{
				// ��ӡÿ����¼
				for (int c = 0; c < nCols; c++)
				{
					std::string temp = lpUnPacker->GetStrByIndex(c);

					// ��̩����һ��ͨ���⴦��
					if (funcid == "337014")
						boost::algorithm::replace_all(temp, SOH, "");
					
					response += temp;
					
					response += SOH;
				} // end all column

				lpUnPacker->Next();
			} // end for all row

		} // end for GetDatasetCount()

		status = 1;
		retcode = "";
		retmsg = "";
		logLevel = Trade::TradeLog::INFO_LEVEL;

	}
	else if(nRet == 1)
	{
		IF2UnPacker *lpUnPacker = (IF2UnPacker *)Pointer;

		std::string sErrCode = lpUnPacker->GetStr("error_no");
		std::string sErrMsg = "ҵ�����ʧ�ܡ�";
		sErrMsg += lpUnPacker->GetStr("error_info");

		RetErrRes(Trade::TradeLog::ERROR_LEVEL, response, sErrCode, sErrMsg);
		goto FINISH;
	}
	else if(nRet == 2)
	{
		std::string sErrCode =  boost::lexical_cast<std::string>(nRet);
		std::string sErrMsg = (char*)Pointer;

		RetErrRes(Trade::TradeLog::ERROR_LEVEL, response, sErrCode, sErrMsg);
		goto FINISH;
	}
	else if (nRet == 3)
	{
		std::string sErrCode =  boost::lexical_cast<std::string>(nRet);
		RetErrRes(Trade::TradeLog::ERROR_LEVEL, response, sErrCode, "ҵ������ʧ��");
		goto FINISH;
	}
	else
	{
		std::string sErrCode =  boost::lexical_cast<std::string>(nRet);

		std::string sErrMsg = "δ֪����";
		sErrMsg += m_pConn->lpConnection->GetErrorMsg(nRet);
		

		RetErrRes(Trade::TradeLog::ERROR_LEVEL, response, sErrCode, sErrMsg);
		goto FINISH;
	}


FINISH:
	// �ͷ�����
	FreeConnect();

	// ������־
	EndLog(response, logMsg);
}

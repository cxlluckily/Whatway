#include "StdAfx.h"

#include "TradeBusiness.h"
//#include "LogManager.h"
#include "./connectpool/ConnectManager.h"



#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string.hpp>

//#include "aes.h"

#include "./config/ConfigManager.h"
#include "./lib/szkingdom/KDEncodeCli.h"
#include "./lib/szkingdom/kcbpcli.h"



//#include "CacheData.h"
#include "encrypt.h"
//ICU
//#include <unicode/putil.h>
//#include <unicode/ucnv.h>
#include "./encrypt/mybotan.h"

#include "./output/FileLog.h"


TradeBusiness::TradeBusiness()
{
	m_pConn = NULL;

	aeskey = "29dlo*%AO+3i16BaweTw.lc!)61K{9^5";

	sCounterType = "3";
}



TradeBusiness::~TradeBusiness(void)
{
}

std::string TradeBusiness::GetTradePWD(std::string isSafe, std::string sEncPwd)
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

std::string TradeBusiness::GetOtherPWD(std::string isSafe, std::string sEncPwd)
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

bool TradeBusiness::Send(std::string& request, std::string& response, int& status, std::string& errCode, std::string& errMsg)
{
	int nRet = 0;
	char szErrMsg[256];
	std::string pkgType = "2";
	std::string log;
	std::string content = "";
	int nRows = 0;



	BeginLog(request);
	



	ParseRequest(request);


	m_pConn = g_ConnectManager.GetConnect(sysNo, busiType, branchId);
	if (m_pConn == NULL)
	{
		RetErrRes(Trade::TradeLog::ERROR_LEVEL, response, "1000", "û�п��õĹ�̨����");
		goto FINISH;
	}


	nRet = KCBPCLI_BeginWrite(m_pConn->handle);

	KCBPCLI_SetSystemParam(m_pConn->handle, KCBP_PARAM_SERVICENAME, (char*)funcid.c_str());
	KCBPCLI_SetSystemParam(m_pConn->handle, KCBP_PARAM_RESERVED, (char*)reqmap["orgid"].c_str());
	KCBPCLI_SetSystemParam(m_pConn->handle, KCBP_PARAM_PACKETTYPE, (char*)pkgType.c_str());

	for (std::map<std::string, std::string>::iterator it = reqmap.begin(); it != reqmap.end(); it++)
	{
		std::string key = it->first;
		std::string value = it->second;


		if (FilterRequestField(key))
		{
			continue;
		}		
		else if(key == "trdpwd")
		{
				std::string encrypt_pwd = g_Encrypt.EncryptPWD(value);
				KCBPCLI_SetValue(m_pConn->handle, (char*)key.c_str(), (char*)encrypt_pwd.c_str());

				OutputDebugString("�͵���̨�Ľ�������");
				OutputDebugString(encrypt_pwd.c_str());
				OutputDebugString("\n");
/*
			if (type == "flash")
			{
				if (funcid == "410302")
				{
					//�޸�����
					trdpwd = GetOtherPWD(isSafe, value);

					std::string encrypt_pwd = g_Encrypt.EncryptPWD(trdpwd);
					
					KCBPCLI_SetValue(m_pConn->handle, (char*)key.c_str(), (char*)encrypt_pwd.c_str());
				}
				else
				{
					// ��Ҫ�Ƚ���
					trdpwd = GetTradePWD(isSafe, value);

					std::string encrypt_pwd = g_Encrypt.EncryptPWD(trdpwd);
					
					KCBPCLI_SetValue(m_pConn->handle, (char*)key.c_str(), (char*)encrypt_pwd.c_str());


					OutputDebugString("�͵���̨�Ľ�������");
					OutputDebugString(encrypt_pwd.c_str());
					OutputDebugString("\n");
				}
			}
			else
			{

			}

			continue;
*/
/*
			char szEncryptPwd[33];
			memset(szEncryptPwd, 0, sizeof(szEncryptPwd));

			std::string enc_key = reqmap["custid"];


			if (type == "web")
			{
				if (funcid == "410301")
				{
					KDEncode(KDCOMPLEX_ENCODE, (unsigned char*)value.c_str(), value.length(), (unsigned char *)szEncryptPwd, sizeof(szEncryptPwd)-1, "410301", 6);
				}
				else
				{
					KDEncode(KDCOMPLEX_ENCODE, (unsigned char*)value.c_str(), value.length(), (unsigned char *)szEncryptPwd, sizeof(szEncryptPwd)-1, (void*)enc_key.c_str(), enc_key.length());
				}
			}
			else
			{
				std::string tradepwd_plain = GetTradePWD(isSafe, value);

				if (funcid == "410301")
				{
					KDEncode(KDCOMPLEX_ENCODE, (unsigned char*)tradepwd_plain.c_str(), tradepwd_plain.length(), (unsigned char *)szEncryptPwd, sizeof(szEncryptPwd)-1, "410301", 6);
				}
				else
				{
					KDEncode(KDCOMPLEX_ENCODE, (unsigned char*)tradepwd_plain.c_str(), tradepwd_plain.length(), (unsigned char *)szEncryptPwd, sizeof(szEncryptPwd)-1, (void*)enc_key.c_str(), enc_key.length());
				}
			}
			std::string encrypt_pwd = g_Encrypt.EncryptPWD("456456");
			//KCBPCLI_SetValue(m_pConn->handle, (char*)key.c_str(), szEncryptPwd);
			
			//KCBPCLI_SetValue(m_pConn->handle, (char*)key.c_str(), "");
			TRACE("��������...........................%s\n", szEncryptPwd);

			//7Ketq+ncv0rD+iX0Sgq8/A==
*/
		}
		else if (key == "newpwd")
		{
				std::string encrypt_pwd = g_Encrypt.EncryptPWD(value);

				KCBPCLI_SetValue(m_pConn->handle, (char*)key.c_str(), (char*)encrypt_pwd.c_str());	
/*
			if (type == "flash")
			{
				//����ǰ��
				newpwd = GetOtherPWD(isSafe, value);

				//�ù�̨����
				std::string encrypt_pwd = g_Encrypt.EncryptPWD(newpwd);

				KCBPCLI_SetValue(m_pConn->handle, (char*)key.c_str(), (char*)encrypt_pwd.c_str());

				// ��׼��¼���޸�����ɹ�����Ҫ���ܷ��ظ�ǰ�ˣ��´δ�����ܺ��������
				// ��ǿ��¼, �޸�����ɹ���ǰ���Լ�����������
				std::string aeskey = "23dpasd23d-23l;df9302hzv/3lvjh*5";

				char cipher[50];
				memset(cipher, 0x00, sizeof(cipher));
				int outlen;
				bool bRet = g_MyBotan.AESEncrypt("AES-256/ECB/PKCS7", aeskey, newpwd, cipher, &outlen);
				//char * out = aes_encrypt_encode(aeskey.c_str(), newpwd.c_str());
				if (bRet)
				{
					newpwd_enc = cipher;
					boost::algorithm::replace_all(newpwd_enc, "=", "$");
					//free(out);
				}
			}
			else
			{
				newpwd = value;
					
				//�ù�̨����
				std::string encrypt_pwd = g_Encrypt.EncryptPWD(newpwd);

				KCBPCLI_SetValue(m_pConn->handle, (char*)key.c_str(), (char*)encrypt_pwd.c_str());		

			}
			continue;
*/
/*
			char szEncryptPwd[33];
			memset(szEncryptPwd, 0, sizeof(szEncryptPwd));

			std::string enc_key = reqmap["custid"];


			if (type == "web")
			{
				
				KDEncode(KDCOMPLEX_ENCODE, (unsigned char*)value.c_str(), value.length(), (unsigned char *)szEncryptPwd, sizeof(szEncryptPwd)-1, (void*)enc_key.c_str(), enc_key.length());
			}
			else
			{
				std::string tradepwd_plain = GetTradePWD(isSafe, value);

				KDEncode(KDCOMPLEX_ENCODE, (unsigned char*)tradepwd_plain.c_str(), tradepwd_plain.length(), (unsigned char *)szEncryptPwd, sizeof(szEncryptPwd)-1, (void*)enc_key.c_str(), enc_key.length());
			}

			KCBPCLI_SetValue(m_pConn->handle, (char*)key.c_str(), szEncryptPwd);

			continue;	
*/
		}

		else if(key == "fundpwd" || key == "newfundpwd" || key == "oldfundpwd")
		{
			std::string encrypt_pwd = g_Encrypt.EncryptPWD(value);
			KCBPCLI_SetValue(m_pConn->handle, (char*)key.c_str(), (char*)encrypt_pwd.c_str());		

			/*
			if (type == "flash")
			{
				//����ǰ��
				std::string sPwdPlain = GetOtherPWD(isSafe, value);

				//�ù�̨����
				std::string encrypt_pwd = g_Encrypt.EncryptPWD(sPwdPlain);

				KCBPCLI_SetValue(m_pConn->handle, (char*)key.c_str(), (char*)encrypt_pwd.c_str());			
			}
			else
			{
				//�ù�̨����
			}

			continue;
			*/

/*
				int KDEncode(int nEncode_Level, 
			  unsigned char *pSrcData, int nSrcDataLen, 
			  unsigned char *pDestData, int nDestDataBufLen, 
			  void *pKey, int nKeyLen);


			char szEncryptPwd[33];
			memset(szEncryptPwd, 0, sizeof(szEncryptPwd));

			std::string enc_key = reqmap["fundid"];


			if (type == "web")
			{
				
				KDEncode(KDCOMPLEX_ENCODE, (unsigned char*)value.c_str(), value.length(), (unsigned char *)szEncryptPwd, sizeof(szEncryptPwd)-1, (void*)enc_key.c_str(), enc_key.length());
			}
			else
			{
				std::string tradepwd_plain = GetTradePWD(isSafe, value);

				KDEncode(KDCOMPLEX_ENCODE, (unsigned char*)tradepwd_plain.c_str(), tradepwd_plain.length(), (unsigned char *)szEncryptPwd, sizeof(szEncryptPwd)-1, (void*)enc_key.c_str(), enc_key.length());
			}
*/
		}
//		else if(key == "ddasigndata")
//		{
//			if (type == "flash")
//			{
				//flash����ר��
//				boost::algorithm::replace_all(value, "$", "=");
//				KCBPCLI_SetValue(m_pConn->handle, (char*)key.c_str(), (char*)value.c_str());
//			}

//			continue;
//		}
		else
		{
			KCBPCLI_SetValue(m_pConn->handle, (char*)key.c_str(), (char*)value.c_str());
		}
	} // end for

/*
	// flash���׽���
	if (funcid == "000000")
	{
		if (trdpwd != newpwd)
		{
			RetErrRes(Trade::TradeLog::ERROR_LEVEL, response, "1001", "������󣬽���ʧ�ܣ�");
			return;
		}
		else
			goto UNLOCK_SUCCESS;
	}
*/

	nRet  = KCBPCLI_SQLExecute(m_pConn->handle, (char*)funcid.c_str()); // ���ܺ�
	if (nRet != 0)
	{
		//gFileLog::instance().Log(funcid + " KCBPCLI_SQLExecute");

		//if (nRet == 2003 || nRet == 2004 || nRet == 2055 || nRet == 2054)
		//����
		//int nErrCode = 0;
		//char szErrMsg[2048] = {0};
		//KCBPCLI_GetErr(m_pConn->handle, &nErrCode, szErrMsg);

		//if (nErrCode == 0)
		//{
		//	RetNoRecordRes(response);
		//	goto FINISH;
		//}

		
		RetErrRes(Trade::TradeLog::ERROR_LEVEL, response, boost::lexical_cast<std::string>(nRet), "KCBPCLI_SQLExecute error");
		goto FINISH;
	}


	int nErrCode = 0;
	nRet = KCBPCLI_GetErrorCode(m_pConn->handle, &nErrCode);
	if (nErrCode != 0)
	{
		KCBPCLI_GetErrorMsg( m_pConn->handle, szErrMsg );
		RetErrRes(Trade::TradeLog::ERROR_LEVEL, response, boost::lexical_cast<std::string>(nErrCode), szErrMsg);
		goto FINISH;
	}


	nRet = KCBPCLI_RsOpen(m_pConn->handle);
	if (nRet != 0 && nRet != 100)
	{
		RetErrRes(Trade::TradeLog::ERROR_LEVEL, response, boost::lexical_cast<std::string>(nRet), "KCBPCLI_RsOpen error");
		goto FINISH;
	}


	nRet = KCBPCLI_SQLFetch(m_pConn->handle);

	char szTmpbuf[1024];
	memset(szTmpbuf, 0x00, sizeof(szTmpbuf));
	nRet = KCBPCLI_RsGetColByName( m_pConn->handle, "CODE", szTmpbuf );
	if (nRet != 0)
	{
		RetErrRes(Trade::TradeLog::ERROR_LEVEL, response, boost::lexical_cast<std::string>(nRet), "KCBPCLI_RsGetColByName error");
		goto FINISH;
	}

	if( strcmp(szTmpbuf, "0") != 0 )
	{
		retcode = szTmpbuf;

		KCBPCLI_RsGetColByName( m_pConn->handle, "MSG", szTmpbuf );
		retmsg = szTmpbuf;

		RetErrRes(Trade::TradeLog::ERROR_LEVEL, response, retcode, retmsg);
		goto FINISH;
	}

	
	nRet = KCBPCLI_SQLMoreResults(m_pConn->handle);
	if (nRet != 0)
	{
		int nErrCode = 0;
		KCBPCLI_GetErrorCode(m_pConn->handle, &nErrCode);
		KCBPCLI_GetErrorMsg( m_pConn->handle, szErrMsg );

		if (nErrCode == 0)
		{
			RetNoRecordRes(response);
			goto FINISH;
		}


		RetErrRes(Trade::TradeLog::ERROR_LEVEL, response, boost::lexical_cast<std::string>(nErrCode), szErrMsg);
		goto FINISH;

		// �ǲ���ִ�гɹ���û�����ݷ���
	}
	

/*
	if (nRet != 0 )
	{
		RetErrRes(Cssweb::CsswebMessage::ERROR_LEVEL, response, response_pb, boost::lexical_cast<std::string>(nRet), "KCBPCLI_SQLMoreResults error");
		return;
	}
*/



	//nRet = KCBPCLI_SQLNumResultCols(m_pConn->handle, &nCol);
	

	
	nRows = 0;

	int nCols = 0;
	KCBPCLI_RsGetColNum(m_pConn->handle, &nCols);


	bool bGetColName = true;
	
				
	while( !KCBPCLI_RsFetchRow(m_pConn->handle) )
	{
		nRows += 1;

		// ȡ��ͷ
		if (bGetColName)
		{
			for( int i = 1; i <= nCols; i++ )
			{
				char szColName[256];
				memset(szColName, 0x00, sizeof(szColName));

				KCBPCLI_RsGetColName( m_pConn->handle, i, szColName, sizeof(szColName)-1 );

				content += szColName;
				content += SOH;
			}

			bGetColName = false;
		} // end for col

		//����������
		for( int i = 1; i <= nCols; i++ )
		{
			//��ȡ����
			char szColName[256];
			memset(szColName, 0x00, sizeof(szColName));
			KCBPCLI_RsGetColName( m_pConn->handle, i, szColName, sizeof(szColName)-1 );

			//����������ȡ��ֵ
			char szColValue[1024];
			KCBPCLI_RsGetColByName( m_pConn->handle, szColName, szColValue );

			content += szColValue;
			content += SOH;
		} // end for row
	} // end while

	log = funcid;
	log += "��������";
	log += boost::lexical_cast<std::string>(nRows);
	gFileLog::instance().Log(log);

	if (nRows == 0)
	{
		// ִ�гɹ�����̨û�����ݷ���
		RetNoRecordRes(response);
		goto FINISH;
	}

	response = boost::lexical_cast<std::string>(nRows);
	response += SOH;
	response += boost::lexical_cast<std::string>(nCols);
	response += SOH;

	response += content;


	nRet = KCBPCLI_SQLCloseCursor(m_pConn->handle);


	status = 1;
	retcode = "";
	retmsg = "";
	logLevel = Trade::TradeLog::INFO_LEVEL;
	

FINISH:
	// �ͷ�����
	FreeConnect();

	// ������־
	//EndLog(response, log);
	
	

	/*
	if (type == "flash")
	{
		if (hasData)
		{
			response += "{\"cssweb_test\":\"0\"}]}";
		}
		else
		{
			if (funcid  == "000000")
			{
				response = "{";
				response += "\"cssweb_type\":\"" + flashreqcallback + "\",";
				response += "\"cssweb_code\":\"success\",";
				response += "\"cssweb_msg\":\"�����ɹ���\",";
				response += "\"item\":[]}";
			}
			else if (funcid == "410302")
			{

				response = "{";
				response += "\"cssweb_type\":\"" + flashreqcallback + "\",";
				response += "\"cssweb_code\":\"success\",";
				response += "\"cssweb_msg\":\"�޸�����ɹ���\",";
				response += "\"item\":[";
				response += "{\"pwd\":\"";
				response += newpwd_enc;

				response += "\"},{\"cssweb_test\":\"0\"}]}";
			}
			else
			{
				// ��ѯ�ɹ��������ݷ���

				response = "{";
				response += "\"cssweb_type\":\"" + flashreqcallback + "\",";
				response += "\"cssweb_code\":\"success\",";
				response += "\"cssweb_msg\":\"�����ɹ�����̨�����ݷ��أ�\",";
				response += "\"item\":[]}";
			}
		}
	}

	*/		

	return true;
}

bool TradeBusiness::CreateConnect()
{
	int nRet = 0;


		tagKCBPConnectOption stKCBPConnection;
		memset(&stKCBPConnection, 0x00, sizeof(stKCBPConnection));
		strcpy(stKCBPConnection.szServerName, m_Counter->m_sServerName.c_str());
		stKCBPConnection.nProtocal = 0;
		strcpy(stKCBPConnection.szAddress, m_Counter->m_sIP.c_str());
		stKCBPConnection.nPort = m_Counter->m_nPort;
		strcpy(stKCBPConnection.szSendQName, m_Counter->m_sReq.c_str());
		strcpy(stKCBPConnection.szReceiveQName, m_Counter->m_sRes.c_str());

		nRet = KCBPCLI_Init( &handle );
		nRet = KCBPCLI_SetConnectOption( handle, stKCBPConnection );		

		//���ó�ʱ
		nRet = KCBPCLI_SetCliTimeOut( handle, m_Counter->m_nConnectTimeout);

		// �����Ƿ����������Ϣ
		//nRet = KCBPCLI_SetOptions( handle, KCBP_OPTION_TRACE, &gConfigManager::instance().m_nIsTradeServerDebug, sizeof(int));

		nRet = KCBPCLI_SQLConnect( handle, stKCBPConnection.szServerName, (char*)m_Counter->m_sUserName.c_str(), (char*)m_Counter->m_sPassword.c_str());

		if (nRet != 0)
		{
			

			return false;
		}
		else
		{
			

			return true;
		}
	
}

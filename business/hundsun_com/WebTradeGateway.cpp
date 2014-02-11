#include "StdAfx.h"
#include "WebTradeGateway.h"




#include "ConnectPool.h"

#include "tradegatewaygtjadlg.h"
#include "tradegatewaygtja.h"
#include "ConfigManager.h"


CWebTradeGateway::CWebTradeGateway(CConnect* pConn)
{
	m_pConn = pConn;
	m_nReTry = g_ConfigManager.m_nRetry;
}


CWebTradeGateway::~CWebTradeGateway()
{
}


void CWebTradeGateway::SendRequest(CString sRequest, CString& sResponse)
{
	sResponse.Empty();
	
	
	CString strFuncId;
	CString strBranchNo;
	int nRet = 0;	

	int i = 0;
	CString strNameValue;
	CString strName, strValue;

	while (AfxExtractSubString(strNameValue, sRequest, i))
	{
		i++;

	   if (!AfxExtractSubString(strName, strNameValue, 0, '='))
	   {
		  continue;
	   }

	   if (!AfxExtractSubString(strValue, strNameValue, 1, '='))
	   {
		  continue;
	   }

	   if (strName == "function_id")
	   {
			strFuncId = strValue;
	   }

	   if (strName == "op_branch_no")
	   {
		   strBranchNo = strValue;
	   }
	}

	//TRACE("Ӫҵ��=%s\n", strBranchNo);
	//TRACE("���ܺ�=%s\n", strFuncId);
	int lBranchNo = atoi(strBranchNo);
	int lFuncNo = atoi(strFuncId);

	m_pConn->lpConnection->SetHead(lBranchNo, lFuncNo);
	
	i = i - 1;
	//TRACE("������������������%d\n", i);
	m_pConn->lpConnection->SetRange(i, 1);

	i = 0;
	while (AfxExtractSubString(strNameValue, sRequest, i))
	{
	   i++;
//	   CString strName, strValue;

	   if (!AfxExtractSubString(strName, strNameValue, 0, '='))
	   {
		  continue;
	   }

	   if (!AfxExtractSubString(strValue, strNameValue, 1, '='))
	   {
		  continue;
	   }

//		TRACE("AddField=%s\n", strName);
		m_pConn->lpConnection->AddField(strName);
	}

	i = 0;
	while (AfxExtractSubString(strNameValue, sRequest, i))
	{
	   i++;
	   //CString strName, strValue;

	   if (!AfxExtractSubString(strName, strNameValue, 0, '='))
	   {
		  continue;
	   }

	   if (!AfxExtractSubString(strValue, strNameValue, 1, '='))
	   {
		  continue;
	   }

//		TRACE("AddValue=%s\n", strValue);
		m_pConn->lpConnection->AddValue(strValue);
	}

	int nSenderId = 1;
	
	m_pConn->lpConnection->put_SenderId(nSenderId); // �˴��ǲ��ǿ��Ըĳɿͻ���֮��Ψһ����ͻ���ֵ


	nRet = m_pConn->lpConnection->Send();
	if (nRet != 0)
	{
		// ����ʧ��
		sResponse = "cssweb_code=error&cssweb_msg=��������ʧ��\n";
		return;
	}

	m_pConn->lpConnection->FreePack();

	nRet = m_pConn->lpConnection->Receive();
	if (nRet != 0)
	{
		// ����ʧ��
		sResponse = "cssweb_code=error&cssweb_msg=��������ʧ��\n";
		return;
	}

	long nRecvSenderId = m_pConn->lpConnection->get_SenderId();
	if (nRecvSenderId != nSenderId)
	{
		sResponse = "cssweb_code=error&cssweb_msg=���ͺͽ��ղ�ƥ��\n";
		return;
	}

	
	while (m_pConn->lpConnection->get_Eof() == 0)
	{
		int fieldCount = m_pConn->lpConnection->get_FieldCount();
		
		for (int i=0; i<fieldCount; i++)
		{
				CString fieldName = m_pConn->lpConnection->GetFieldName(i);
				CString fieldValue = m_pConn->lpConnection->FieldByName(fieldName);
				
				CString sKeyValue = fieldName + "=" + fieldValue;
				//TRACE("%s\n", sKeyValue);

				if (i != (fieldCount-1))
				{
					//sResponse.Append(sKeyValue + "&");
					sResponse = sResponse + sKeyValue + "&";
				}
				else
				{
					//sResponse.Append(sKeyValue);
					sResponse = sResponse + sKeyValue;
				}
		}

		sResponse = sResponse + "\n";
		m_pConn->lpConnection->MoveBy(1);
	}

	if (sResponse.GetLength() <= 0)
	{
		sResponse = "cssweb_code=success&cssweb_msg=û������\n";
	}
}


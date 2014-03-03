#ifndef SYWG_CONNECT_H
#define SYWG_CONNECT_H




#include <string>

#include <boost/asio.hpp>

#include "business/IBusiness.h"

// ����ͬ��ģʽʵ��
// ����boost::asio��ʵ�ֿ�ƽ̨
// ��Ҫ֧��CRC
// ��Ҫ֧��DES�����㷨, botan֧��
// ���Ӻź�des��Կ�;���ҵ���޹�
// Ҫ����ҵ��ӿ�xml

class CSywgConnect : public IBusiness
{
public:
	CSywgConnect(void);
	~CSywgConnect(void);

public:
	virtual bool CreateConnect();
	virtual void CloseConnect();
	virtual bool Send(std::string& request, std::string& response, int& status, std::string& errCode, std::string& errMsg);
	
	

	



	WORD GetCRC(void* msg, size_t len);
	int ComputeNetPackSize(int nRealSize);
	int ReadMsgHeader(char * buf);

	bool InitConnect();
	bool GetErrorMsg(int nErrCode, std::string& sErrMsg);
	bool Login(std::string& response, int& status, std::string& errCode, std::string& errMsg);
	bool Send(std::string& response, int& status, std::string& errCode, std::string& errMsg);
	
	//BOOL OpenAccount(std::map<std::string, std::string>& reqmap, std::string& response);
	// 705
	//BOOL BankTransfReq(std::map<std::string, std::string>& reqmap, std::string& response);
	

private:
	long cn_id;      	// �������Ӻ�
	BYTE des_key[8];         // DES��Կ(���á�ExpressT����Ϊ��Կdes����)
	std::string branchNo; 

	boost::asio::io_service ios;
	boost::asio::ip::tcp::socket socket;
	std::string m_sIP;
	int m_nPort;

	

public:
	int m_nErrCode;
	std::string m_sErrMsg;
	void SetErrInfo(int nErrCode, std::string sErrMsg="");
	void GetErrInfo(std::string& sErrCode, std::string& sErrMsg);

	
	

private:
	// �����¼��Ϣ�������Զ�����

};

#endif
#ifndef _COUNTER_H_
#define _COUNTER_H_

#include <string>
#include "common.h"


class Counter
{
public:
	Counter(void);
	~Counter(void);

	std::string m_sIP;
	int m_nPort;
	std::string m_sServerName;
	std::string m_sUserName;
	std::string m_sPassword;
	std::string m_sReq;
	std::string m_sRes;

	COUNTER_TYPE m_eCounterType;
	int m_nConnectTimeout; // ��̨���ӳ�ʱʱ��
	int m_nIdleTimeout; // ��̨���Ӻ�û�������Զ��ж�����ʱ��
	int m_nRecvTimeout; // ������ʱʱ��

	std::string m_sGydm; // �����̨����Ա����
	std::string m_sWtfs_mobile;
	std::string m_sWtfs_web;

};
#endif

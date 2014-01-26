#ifndef _COMMON_H_
#define _COMMON_H_

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>

#define EC_SUCCESS 0
#define EC_UNKNOWN 1
#define EC_REQ_INVALID 100
#define EC_NO_ACCESS 101
#define EC_SERVICE_STOP 102
#define EC_PARAM 103
#define EC_SM 104
#define EC_FILE 105



// ��̨����
typedef	enum CounterType {
		CT_UNKNOWN = 0,
		CT_HS_T2 = 1,
		CT_HS_COM = 2,
		CT_JZ_WIN = 3,
		CT_JZ_LINUX = 4,
		CT_DINGDIAN = 5,
		CT_JSD = 6,
		CT_XINYI = 7
	} COUNTER_TYPE;

// ҵ������
//<!-- ҵ�����ͣ�0.ͨ��, 1.֤ȯ����, 2.������ȯ, 3.���Ͽ��� 4.�˻�ϵͳ, 5.ͳһ��֤�� 6.��Ȩ, 7.��ǩ-->
typedef	enum BusiType {
		BUSI_TYPE_ALL = 0,
		BUSI_TYPE_STOCK = 1,
		BUSI_TYPE_CREDIT = 2,
		BUSI_TYPE_REGISTER = 3,
		BUSI_TYPE_ACCOUNT = 4,
		BUSI_TYPE_AUTH = 5,
		BUSI_TYPE_OPTION = 6,
		BUSI_TYPE_VERIFY = 7
	} BUSI_TYPE;

#endif

#ifndef _IMESSAGE_H_
#define _IMESSAGE_H_

#include "log/tradelog.pb.h"

class ISession;

class IMessage
{
public:
	IMessage(void);
	~IMessage(void);

	// ҵ�������ɵ���־
	Trade::TradeLog log;

	ISession* session;

public:
	void SetSession(ISession* session);
	ISession* GetSession();

};

#endif

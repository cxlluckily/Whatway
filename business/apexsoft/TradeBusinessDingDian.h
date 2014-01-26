#include <string>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "./log/tradelog.pb.h"
#include "./connectpool/Connect.h"
#include <map>


//#include "trade.pb.h"



#include "./connectpool/Connect.h"
#include "./business/IBusiness.h"


class TradeBusinessDingDian : public IBusiness
{
public:
	TradeBusinessDingDian();
	~TradeBusinessDingDian(void);




	


	void Process(std::string& request, std::string& response, Trade::TradeLog& logMsg);


	std::string GetTradePWD(std::string isSafe, std::string sEncPwd);
	std::string GetOtherPWD(std::string isSafe, std::string sEncPwd);
	std::string trdpwd;
	std::string newpwd;
	std::string newpwd_enc;
	char* EncryptPwd(const char* plainText, char* szPwd);
};
#ifndef _TRADE_SERVER_SSL_
#define _TRADE_SERVER_SSL_

#include <boost/functional/factory.hpp>

#include "ssl_message.h"
#include "ThreadSafeQueue/job_queue.h"
#include "./threadpool/worker.h"
#include "ssl_session.h"



#include "./captcha/VerificationCode.h"

//opencv
#include "./lib/opencv/opencv_h.h"


class trade_server_ssl
{
public:
	typedef job_queue<ssl_request_ptr> req_queue_type;
	typedef worker<req_queue_type> req_worker_type;

	typedef job_queue<ssl_response_ptr> resp_queue_type;
	typedef worker<resp_queue_type> resp_worker_type;

private:
	req_queue_type recvq_;
	resp_queue_type sendq_;

	req_worker_type req_worker_;
	resp_worker_type resp_worker_;

public:
	trade_server_ssl();
	req_queue_type& recv_queue();
	void start();
	void stop();

private:
	bool process_msg(ssl_request_ptr& req);
	void trade(ssl_request_ptr& req);
	void captcha(ssl_request_ptr& req);
	void OCR(ssl_request_ptr& req);
	void error(ssl_request_ptr& req);

	bool send_msg(ssl_response_ptr resp);


	
	

	bool GetSysNoAndBusiType(std::string& request, std::string& sysNo, std::string& busiType);

	bool ReadOCR(std::string file, std::string& content);
	bool GenImage(std::string sSrcFile, std::string sTargetFile, int nType=1, int nThreshold=128, bool bShow=false, std::string sSrcWndName="", std::string sTargetWndName="");
	void rotate_image_90n(cv::Mat &src, cv::Mat &dst, int angle);

private:
	//CaptchaManager m_captcha;
	VerificationCode m_Captcha;
};
#endif
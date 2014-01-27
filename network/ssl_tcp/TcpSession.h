#ifndef _TCP_SESSION_
#define _TCP_SESSION_




#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/pool/object_pool.hpp>

#include "Network/ssl_tcp/MsgHeader.h"
//#include "network/CustomMessage.h"
#include "ThreadSafeQueue/job_queue.h"




#include "network/ISession.h"

class CustomMessage;

/*
session���ͷſ��Բο�
http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/example/cpp03/ssl/server.cpp

http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/example/cpp03/http/server2/connection.hpp
*/

class TcpSession : public boost::enable_shared_from_this<TcpSession>, public ISession
{
public:
	typedef boost::asio::ip::tcp::socket socket_type;
	typedef boost::asio::io_service ios_type;

	typedef ios_type::strand strand_type;
	typedef job_queue<CustomMessage*> queue_type;

	//�ڴ�أ���ʱ����
	typedef boost::object_pool<CustomMessage*> object_pool_type;

private:
	socket_type socket_;
	strand_type strand_;
	queue_type& queue_;

	static object_pool_type msg_pool_;


public:
	TcpSession( ios_type& ios, queue_type& q);
	~TcpSession();

	socket_type& socket();
	ios_type& io_service();

	void start();
	void close();
	

private:
	CustomMessage* create_request();
	void read(CustomMessage* req);

	virtual void write(CustomMessage* resp);

	virtual void handle_read_head(const boost::system::error_code& error, size_t bytes_transferred, CustomMessage* req);
	virtual void handle_read_msg(const boost::system::error_code& error, size_t bytes_transferred, CustomMessage* req);

	virtual void handle_write_head(const boost::system::error_code& error, size_t bytes_transferred, CustomMessage* resp);
	virtual void handle_write_msg(const boost::system::error_code& error, size_t bytes_transferred, CustomMessage* resp);

public:

	// �Ƿ��¼״̬
	int loginStatus;

	
	// ����
	//CTCPClientSync m_xinyi;

	// ���˴�
	//CSywgConnect * m_pAGC;

	// �����̣߳�����������Ϣ������ҵ�����ӵ������Զ���ʵ�ַ��������ֻ
	
};


typedef boost::shared_ptr<TcpSession> TcpSessionPtr;

#endif // _TCP_SESSION_


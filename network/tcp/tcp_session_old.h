#ifndef _TCP_SESSION_OLD_
#define _TCP_SESSION_OLD_


#include "tcp_message_old.h"
#include "ThreadSafeQueue/job_queue.h"

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/pool/object_pool.hpp>



//class tcp_message_old;
//typedef tcp_message_old tcp_old_request;
//typedef tcp_message_old tcp_old_response;
//typedef tcp_message_old* tcp_old_request_ptr;
//typedef tcp_message_old* tcp_old_response_ptr;

class tcp_session_old : public boost::enable_shared_from_this<tcp_session_old>
	,private boost::noncopyable
{
public:
	typedef boost::asio::ip::tcp::socket socket_type;
	typedef boost::asio::io_service ios_type;

	typedef ios_type::strand strand_type;
	typedef job_queue<tcp_old_request_ptr> queue_type;
	typedef boost::object_pool<tcp_message_old> object_pool_type;

private:
	socket_type socket_;
	strand_type strand_;
	queue_type& queue_;
	static object_pool_type msg_pool_;


public:
	tcp_session_old(ios_type& ios, queue_type& q);
	//~tcp_session_old();

	socket_type& socket();
	ios_type& io_service();

	void start();
	void close();
	void write(tcp_old_response_ptr resp);

private:
	tcp_old_request_ptr create_request();
	void read(tcp_old_request_ptr req);

	void handle_read_head(const boost::system::error_code& error, size_t bytes_transferred, tcp_old_request_ptr req);
	void handle_read_msg(const boost::system::error_code& error, size_t bytes_transferred, tcp_old_request_ptr req);

	void handle_write_head(const boost::system::error_code& error, size_t bytes_transferred, tcp_old_response_ptr resp);
	void handle_write_msg(const boost::system::error_code& error, size_t bytes_transferred, tcp_old_response_ptr resp);
};

typedef boost::shared_ptr<tcp_session_old> tcp_session_old_ptr;

#endif // _TCP_SESSION_

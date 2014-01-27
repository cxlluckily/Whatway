#include "stdafx.h"

#include "http_session.h"
#include <boost/bind.hpp>
#include <boost/functional/factory.hpp>

http_session::object_pool_type http_session::msg_pool_;


http_session::http_session(ios_type& ios, queue_type& q):
	
	socket_(ios), strand_(ios), queue_(q)
{
}

http_session::socket_type& http_session::socket()
{
	return socket_;
}

http_session::ios_type& http_session::io_service()
{
	return socket_.get_io_service();
}

void http_session::close()
{
	TRACE("�ر�����\n");

	boost::system::error_code ignored_ec;
	socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
	socket_.close(ignored_ec);
}


void http_session::start()
{
	http_request_ptr req = create_request();
	read(req);
}

http_request_ptr http_session::create_request()
{
	// �ڴ��
	//return msg_pool_.construct(shared_from_this(), bind(&object_pool_type::destroy, ref(msg_pool_), _1));
		// �ǵ�Ҫ�ͷ�
	http_session_ptr sess = shared_from_this();
	http_request_ptr req = boost::factory<http_request_ptr>()(sess);
	return req;
}

void http_session::read(http_request_ptr req)
{
//	boost::asio::async_read(socket_, boost::asio::buffer(RequestMsg.msg_header, http_message::header_length),
//		bind(&TcpConnection::handle_read_header, shared_from_this(), boost::asio::placeholders::error));

	TRACE("begin read...\n");
/*
  socket_.async_read_some(boost::asio::buffer(buffer_),
      boost::bind(&connection::handle_read, shared_from_this(),
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));

	socket_.async_read_some(boost::asio::buffer(buffer_),
		  boost::bind(&connection::handle_read, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));

  socket_.async_read_some(boost::asio::buffer(buffer_),
      boost::bind(&connection::handle_read, shared_from_this(),
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));

  socket_.async_read_some(boost::asio::buffer(buffer_),
      strand_.wrap(
        boost::bind(&connection::handle_read, shared_from_this(),
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred)));
*/

	//Ӧ�õķ�ʽ
	//1.async_read_util (\r\n\r\n)
	//2.����content-length, ���ս������Ĵ�С

	socket_.async_read_some(
	
		boost::asio::buffer(req->msg_header, http_message::header_length), 
		strand_.wrap(
			boost::bind(
				&http_session::handle_read_msg, 
				this, 
				boost::asio::placeholders::error, 
				boost::asio::placeholders::bytes_transferred, 
				req
			)
		)
	);

	TRACE("read finished\n");
}


void http_session::handle_read_msg(const boost::system::error_code& error, size_t bytes_transferred, http_request_ptr req)
{
	TRACE("handle_read_msg\n");
	//std::cout << "handle_read_msg bytes_transferred" << bytes_transferred << std::endl;
		
	if (error) // req->check_msg_crc(0
	{
		TRACE("handle_read_msg error\n");

		close();
		return;
	}

	// �õ��������ʱ��
	req->genreq();

	//req->decode_header();
	queue_.push(req);

	start();
}


void http_session::write(http_response_ptr resp)
{
	async_write(socket_,
		boost::asio::buffer(resp->msg_body, resp->msg_body.size()),
		strand_.wrap(
			bind(&http_session::handle_write_msg, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, resp)
		)
	);
	

}


void http_session::handle_write_msg(const boost::system::error_code& error, size_t bytes_transferred, http_response_ptr resp)
{
	if (error || bytes_transferred != resp->msg_body.size())
	{
		//std::cout << "handle_write_msg error" << std::endl;
		close();
		// ����û���ͷ�resp
		return;
	}

	// ������־����
	boost::posix_time::ptime ptSendTime = boost::posix_time::microsec_clock::local_time();
	std::string sSendTime = boost::gregorian::to_iso_extended_string(ptSendTime.date()) + " " + boost::posix_time::to_simple_string(ptSendTime.time_of_day());
	resp->logMsg.set_sendtime(sSendTime);

//	log_.push(resp->logMsg);

	// �ͷ���Ӧ��
	resp->destroy();
}


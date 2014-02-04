#include "stdafx.h"

#include "SSLSession.h"

#include <boost/bind.hpp>
#include <boost/functional/factory.hpp>
#include <boost/bind.hpp>
#include <boost/functional/factory.hpp>

#include "./output/FileLog.h"
#include "network/ssl_tcp/custommessage.h" 

#include "log/FileLogManager.h"
#include "output/filelog.h"

#include "network/imessage.h"
#include "network/http/http_message.h"
#include "network/tcp/tcp_message_old.h"
#include "network/ssl/ssl_message.h"


SSLSession::SSLSession(ios_type& ios, queue_type& q, int msgType, boost::asio::ssl::context& context)
	:socket_(ios, context), 
	strand_(ios), 
	queue_(q)
{
	//socket_.set_verify_mode(boost::asio::ssl::verify_fail_if_no_peer_cert);
	//socket_.set_verify_callback(boost::bind(&ssl_session::verify_certificate, this, _1, _2));

	m_msgType = msgType;
}


SSLSession::ios_type& SSLSession::io_service()
{
	return socket_.get_io_service();
}

boost::asio::ssl::stream<boost::asio::ip::tcp::socket>::lowest_layer_type& SSLSession::socket()
{
	return socket_.lowest_layer();
}

void SSLSession::close()
{
	CloseCounterConnect();

	boost::system::error_code ignored_ec;

	socket_.lowest_layer().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);

	socket_.lowest_layer().close(ignored_ec);

	// �ͷ�SSLServer������ָ��
	delete this;
}


void SSLSession::start()
{
	socket_.async_handshake(
		boost::asio::ssl::stream_base::server,
        boost::bind(&SSLSession::handle_handshake, shared_from_this(), boost::asio::placeholders::error)
	);
  
}

void SSLSession::handle_handshake(const boost::system::error_code& error)
{
	if (error)
	{
		close();
		return;
	}	
	
	read();
}

IMessage* SSLSession::create_request()
{
	IMessage* req = NULL;
	
	switch (m_msgType)
	{
	case MSG_TYPE_HTTP:
		req = new http_message();
		break;
	case MSG_TYPE_TCP_OLD:
		req = new tcp_message_old();
		break;
	case MSG_TYPE_SSL_PB:
		req = new ssl_message();
		break;
	case MSG_TYPE_TCP_NEW:
		req = new CustomMessage();
		break;
	}
			
	req->SetSession(this);
	return req;
	/*
	ssl_session_ptr sess = shared_from_this();
	ssl_request_ptr req = boost::factory<ssl_request_ptr>()(sess);
	return req;
	*/
}

// ������
void SSLSession::read()
{
	IMessage* req = create_request();			

	boost::asio::async_read(socket_, 
		boost::asio::buffer(req->GetMsgHeader(), req->GetMsgHeaderSize()), 
		boost::asio::transfer_all(),
		strand_.wrap(
			boost::bind(&SSLSession::handle_read_head, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, req)
		)
	);
}

void SSLSession::handle_read_head(const boost::system::error_code& error, size_t bytes_transferred, IMessage* req)
{
	if (error)
	{
		gFileLog::instance().Log("����ͷʧ�ܣ� �������:" + boost::lexical_cast<std::string>(error.value()) + ", ������Ϣ:" + error.message());

		close();
		return;
	}

	if (bytes_transferred != req->GetMsgHeaderSize())
	{
		gFileLog::instance().Log("����ͷʧ�ܣ���Ҫ��:" + boost::lexical_cast<std::string>(req->GetMsgHeaderSize()) + ", ʵ�ʶ�:" + boost::lexical_cast<std::string>(bytes_transferred) );

		close();
		return;
	}

	
	if (!req->DecoderMsgHeader())
	{
		gFileLog::instance().Log("�����ͷʧ��");

		close();
		return;
	}

	boost::asio::async_read(socket_, 
		boost::asio::buffer(req->GetMsgContent(), req->GetMsgContentSize()),
		boost::asio::transfer_all(),
		strand_.wrap(
			bind(&SSLSession::handle_read_msg, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, req)
		)
	);
}



void SSLSession::handle_read_msg(const boost::system::error_code& error, size_t bytes_transferred, IMessage* req)
{
	
		
	if (error) 
	{
	
		gFileLog::instance().Log("��������ʧ�ܣ� �������:" + boost::lexical_cast<std::string>(error.value()) + ", ������Ϣ:" + error.message());

		close();
		return;
	}

	if (bytes_transferred != req->GetMsgContentSize())
	{
		gFileLog::instance().Log("��������ʧ�� ��Ҫ��:" + boost::lexical_cast<std::string>(req->GetMsgContentSize()) + ", ʵ�ʶ�:" + boost::lexical_cast<std::string>(bytes_transferred) );

		close();
		return;
	}

	
	// ���ý��յ���ʱ��
	req->SetRecvTime();

	queue_.push(req);

	read();
}


// дӦ������
void SSLSession::write(IMessage* resp)
{
	boost::asio::async_write(socket_,
		boost::asio::buffer(resp->GetMsgHeader(), resp->GetMsgHeaderSize()),
		boost::asio::transfer_all(),
		strand_.wrap(
			bind(&SSLSession::handle_write_head, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, resp)
		)
	);
}
void SSLSession::handle_write_head(const boost::system::error_code& error, size_t bytes_transferred, IMessage* resp)
{
	if (error)
	{
		gFileLog::instance().Log("д��ͷʧ�ܣ��������:" + boost::lexical_cast<std::string>(error.value()) + ", ������Ϣ:" + error.message());

		close();
		return;
	}


	if (bytes_transferred != resp->GetMsgHeaderSize())
	{
		gFileLog::instance().Log("д��ͷʧ�� ��Ҫд:" + boost::lexical_cast<std::string>(resp->GetMsgHeaderSize()) + ", ʵ��д:" + boost::lexical_cast<std::string>(bytes_transferred) );

		close();
		return;
	}

	
	boost::asio::async_write(socket_,
		boost::asio::buffer(resp->GetMsgContent(), resp->GetMsgContentSize()),
		boost::asio::transfer_all(),
		strand_.wrap(
			bind(&SSLSession::handle_write_msg, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, resp)
		)
	);
	
}

void SSLSession::handle_write_msg(const boost::system::error_code& error, size_t bytes_transferred, IMessage* resp)
{

	if (error)
	{

		gFileLog::instance().Log("д������ʧ�ܣ� �������:" + boost::lexical_cast<std::string>(error.value()) + ", ������Ϣ:" + error.message());

		close();
		return;
	}

	if (bytes_transferred != resp->GetMsgContentSize())
	{
		gFileLog::instance().Log("д������ʧ�� ��Ҫд:" + boost::lexical_cast<std::string>(resp->GetMsgContentSize()) + ", ʵ��д:" + boost::lexical_cast<std::string>(bytes_transferred) );

		close();
		return;
	}

	// ������־����
	resp->SetSendTime();

	/*
	if (resp->GetMsgHeader()->FunctionNo == 0)
	{
		// �������ܲ�д��־
	}
	else
	{
		gFileLogManager::instance().push(resp->log);
	}
	*/

	// ɾ��Ӧ���
	resp->destroy();
}

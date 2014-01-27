#include "stdafx.h"

#include "TcpSession.h"

#include <boost/bind.hpp>
#include <boost/functional/factory.hpp>

#include "./output/FileLog.h"
#include "network/ssl_tcp/custommessage.h" 
#include "log/FileLogManager.h"




TcpSession::TcpSession( ios_type& ios, queue_type& q):

	socket_(ios), 
	strand_(ios), 
	queue_(q)
{
	counterConnect = NULL;
}

TcpSession::~TcpSession()
{
	if (counterConnect != NULL)
		delete counterConnect;
}


TcpSession::socket_type& TcpSession::socket()
{
	return socket_;
}

TcpSession::ios_type& TcpSession::io_service()
{
	return socket_.get_io_service();
}

void TcpSession::close()
{
	boost::system::error_code ignored_ec;

	socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);

	socket_.close(ignored_ec);
}


void TcpSession::start()
{
	IMessage* req = create_request();
	

	read(req);
}

IMessage* TcpSession::create_request()
{
	IMessage* req = new CustomMessage();
	req->SetSession(this);
	return req;

	// �ڴ��
	//return msg_pool_.construct(shared_from_this(), bind(&object_pool_type::destroy, ref(msg_pool_), _1));

	//tcp_session_old_ptr sess = shared_from_this();
	//tcp_old_request_ptr req = boost::factory<tcp_old_request_ptr>()(shared_from_this());
	//tcp_old_request_ptr req = new tcp_old_request((tcp_session_old_ptr)shared_from_this());
	
}


// ������
void TcpSession::read(IMessage* req)
{
	boost::asio::async_read(socket_, 
		boost::asio::buffer(req->GetMsgHeader(), req->GetMsgHeaderSize()), 
		boost::asio::transfer_all(),
		strand_.wrap(
			boost::bind(&TcpSession::handle_read_head, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, req)
		)
	);
}

void TcpSession::handle_read_head(const boost::system::error_code& error, size_t bytes_transferred, IMessage* req)
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

	async_read(socket_, 
		boost::asio::buffer(req->GetMsgContent(), req->GetMsgContentSize()),
		boost::asio::transfer_all(),
		strand_.wrap(
			bind(&TcpSession::handle_read_msg, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, req)
		)
	);
}



void TcpSession::handle_read_msg(const boost::system::error_code& error, size_t bytes_transferred, IMessage* req)
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

	start();
}

// дӦ������
void TcpSession::write(IMessage* resp)
{
	boost::asio::async_write(socket_,
		boost::asio::buffer(resp->GetMsgHeader(), resp->GetMsgHeaderSize()),
		boost::asio::transfer_all(),
		strand_.wrap(
			bind(&TcpSession::handle_write_head, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, resp)
		)
	);
}

void TcpSession::handle_write_head(const boost::system::error_code& error, size_t bytes_transferred, IMessage* resp)
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
			bind(&TcpSession::handle_write_msg, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, resp)
		)
	);
	
}

void TcpSession::handle_write_msg(const boost::system::error_code& error, size_t bytes_transferred, IMessage* resp)
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
	if (msgHeader.FunctionNo == 0)
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


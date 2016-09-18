#include "chatServer.hpp"
#include <unistd.h>
#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <queue>

boost::mutex global_stream_lock;

void ChatConnection::OnAccept( const std::string & host, uint16_t port )
{
	global_stream_lock.lock();
	std::cout << "[" << __FUNCTION__ << "] "
						<< host << ":" << port << std::endl;
	global_stream_lock.unlock();

	// Start the next receive
	Recv();
}


void ChatConnection::OnConnect( const std::string & host, uint16_t port )
{
	global_stream_lock.lock();
	std::cout << "[" << __FUNCTION__ << "] "
						<< host << ":" << port << std::endl;
	global_stream_lock.unlock();

	// Start the next receive
	Recv();
}

void ChatConnection::OnSend( const std::vector< uint8_t > & buffer )
{
	global_stream_lock.lock();
	std::cout << "[" << __FUNCTION__ << "] "
						<< buffer.size() << " bytes" << std::endl;
	for( size_t x = 0; x < buffer.size(); ++x )
	{
		std::cout << std::hex << std::setfill( '0' ) <<
			std::setw( 2 ) << (int)buffer[ x ] << " ";
		if( ( x + 1 ) % 16 == 0 )
		{
			std::cout << std::endl;
		}
	}
	std::cout << std::endl;
	global_stream_lock.unlock();
}

void ChatConnection::OnRecv( std::vector< uint8_t > & buffer )
{
	global_stream_lock.lock();
	std::cout << "[" << __FUNCTION__ << "] "
						<< buffer.size() << " bytes" << std::endl;
	for( size_t x = 0; x < buffer.size(); ++x )
	{
		std::cout << std::hex << std::setfill( '0' ) <<
			std::setw( 2 ) << (int)buffer[ x ] << " ";
		if( ( x + 1 ) % 16 == 0 )
		{
			std::cout << std::endl;
		}
	}
	std::cout << std::endl;

	global_stream_lock.unlock();
	// Start the next receive
	Recv();

	// Echo the data back
	Send( buffer );
}

void ChatConnection::OnTimer( const boost::posix_time::time_duration & delta )
{
	global_stream_lock.lock();
	std::cout << "[" << __FUNCTION__ << "] " << delta << std::endl;
	global_stream_lock.unlock();
}

void ChatConnection::OnError( const boost::system::error_code & error )
{
	global_stream_lock.lock();
	std::cout << "[" << __FUNCTION__ << "] " << error
						<< ": " << error.message() << std::endl;
	global_stream_lock.unlock();
}

ChatConnection::ChatConnection( boost::shared_ptr< Hive > hive )
	: Connection( hive )
{
}

ChatConnection::~ChatConnection()
{
}

bool ChatAcceptor::OnAccept( boost::shared_ptr< Connection > connection,
	const std::string & host, uint16_t port )
{
	global_stream_lock.lock();
	std::cout << "[" << __FUNCTION__ << "] "
						<< host << ":" << port << std::endl;
	global_stream_lock.unlock();

	return true;
}

void ChatAcceptor::OnTimer( const boost::posix_time::time_duration & delta )
{
	global_stream_lock.lock();
	std::cout << "[" << __FUNCTION__ << "] " << delta << std::endl;
	global_stream_lock.unlock();
}

void ChatAcceptor::OnError( const boost::system::error_code & error )
{
	global_stream_lock.lock();
	std::cout << "[" << __FUNCTION__ << "] " << error << std::endl;
	global_stream_lock.unlock();
}
ChatAcceptor::ChatAcceptor( boost::shared_ptr< Hive > hive )
	: Acceptor( hive )
{
}

ChatAcceptor::~ChatAcceptor()
{
}

int main( int argc, char * argv[] )
{
	boost::shared_ptr< Hive > hive( new Hive() );

	boost::shared_ptr< ChatAcceptor > acceptor( new ChatAcceptor( hive ) );
	acceptor->Listen( "127.0.0.1", 7777 );

	boost::shared_ptr< ChatConnection > connection( new ChatConnection( hive ) );
	acceptor->Accept( connection );

	while(true) {
		hive->Poll();
		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	}

	hive->Stop();

	return 0;
}

#include "networkWrapper.hpp"
#include <unistd.h>
#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

boost::mutex global_stream_lock;

class ChatHive : public Hive
{
}

class ChatConnection : public Connection
{
private:
	void OnAccept( const std::string & host, uint16_t port )
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] "
							<< host << ":" << port << std::endl;
		global_stream_lock.unlock();

		// Start the next receive
		Recv();
	}

	void OnConnect( const std::string & host, uint16_t port )
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] "
							<< host << ":" << port << std::endl;
		global_stream_lock.unlock();

		// Start the next receive
		Recv();
	}

	void OnSend( const std::vector< uint8_t > & buffer )
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

	void OnRecv( std::vector< uint8_t > & buffer )
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

	void OnTimer( const boost::posix_time::time_duration & delta )
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << delta << std::endl;
		global_stream_lock.unlock();
	}

	void OnError( const boost::system::error_code & error )
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << error
							<< ": " << error.message() << std::endl;
		global_stream_lock.unlock();
	}

public:
	ChatConnection( boost::shared_ptr< ChatHive > hive )
		: Connection( hive )
	{
	}

	~ChatConnection()
	{
	}
};

class ChatAcceptor : public Acceptor
{
private:

private:
	bool OnAccept( boost::shared_ptr< Connection > connection,
		const std::string & host, uint16_t port )
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] "
							<< host << ":" << port << std::endl;
		global_stream_lock.unlock();

		return true;
	}

	void OnTimer( const boost::posix_time::time_duration & delta )
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << delta << std::endl;
		global_stream_lock.unlock();
	}

	void OnError( const boost::system::error_code & error )
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << error << std::endl;
		global_stream_lock.unlock();
	}

public:
	ChatAcceptor( boost::shared_ptr< ChatHive > hive )
		: Acceptor( hive )
	{
	}

	~ChatAcceptor()
	{
	}
};

int main( int argc, char * argv[] )
{
	boost::shared_ptr< ChatHive > hive( new ChatHive() );

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

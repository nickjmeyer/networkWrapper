#include "networkWrapper.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

boost::mutex global_stream_lock;

class MyConnection : public Connection
{
private:

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

		std::string str = "GET / HTTP/1.0\r\n\r\n";

		std::vector< uint8_t > request;
		std::copy( str.begin(), str.end(), std::back_inserter( request ) );
		Send( request );
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
	MyConnection( boost::shared_ptr< Hive > hive )
		: Connection( hive )
	{
	}

	~MyConnection()
	{
	}
};

int main( int argc, char * argv[] )
{
	global_stream_lock.lock();
	std::cout << "hive" << std::endl;
	global_stream_lock.unlock();
	boost::shared_ptr< Hive > hive( new Hive() );

	global_stream_lock.lock();
	std::cout << "connection" << std::endl;
	global_stream_lock.unlock();
	boost::shared_ptr< MyConnection > connection( new MyConnection( hive ) );
	global_stream_lock.lock();
	std::cout << "connect" << std::endl;
	global_stream_lock.unlock();
	connection->Connect( "127.0.0.1", 7777 );

	uint8_t tick = 0;
	while (true) {
		std::vector<uint8_t> data;
		data.push_back(tick++);

		connection->Send(data);

		hive->Poll();
		boost::this_thread::sleep(boost::posix_time::milliseconds(5000));
	}

	global_stream_lock.lock();
	std::cout << "stop" << std::endl;
	global_stream_lock.unlock();
	hive->Stop();

	global_stream_lock.lock();
	std::cout << "return" << std::endl;
	global_stream_lock.unlock();
	return 0;
}

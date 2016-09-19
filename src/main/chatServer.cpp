#include "chatServer.hpp"
#include <unistd.h>
#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/lexical_cast.hpp>
#include <queue>

boost::mutex global_stream_lock;

void ChatServer::process(const chat::Letter & letter,
	const boost::shared_ptr<ChatConnection> connection) {

	global_stream_lock.lock();
	std::cout << "[" << __PRETTY_FUNCTION__ << "]" << std::endl;
	global_stream_lock.unlock();

	if(letter.has_type()) {
		if(letter.type() == chat::Letter_Type_SCOUT) {
			chat::Letter response;
			response.set_type(chat::Letter_Type_SCOUT);
			chat::ScoutLetter * scout = response.mutable_scoutletter();

			std::map<const std::string, const boost::shared_ptr<ChatConnection> >
				::const_iterator
				it,end;
			end = hpToConn.end();
			for (it = hpToConn.begin(); it != end; ++it) {
				scout->add_client(it->first);
			}
			connection->SendLetter(response);
		}
	}
}

void ChatServer::addConnection(const std::string & hostPort,
	const boost::shared_ptr<ChatConnection> connection) {
	global_stream_lock.lock();
	std::cout << "Adding connection: " << hostPort << std::endl;
	global_stream_lock.unlock();
	hpToConn.insert(std::pair<const std::string,
		const boost::shared_ptr<ChatConnection> >(hostPort,connection));
}

void ChatServer::delConnection(
	const boost::shared_ptr<ChatConnection> connection) {
	std::map<const std::string, const boost::shared_ptr<ChatConnection> >
		::const_iterator
		it,end;
	end = hpToConn.end();
	for (it = hpToConn.begin(); it != end; ++it) {
    if(it->second == connection) {
			global_stream_lock.lock();
			std::cout << "Deleting connection: " << it->first << std::endl;
			global_stream_lock.unlock();
			hpToConn.erase(it->first);
			break;
		}
	}
}

void ChatConnection::SendLetter( const chat::Letter & letter)
{
	std::string letterStr;
	letter.SerializeToString(&letterStr);

	std::vector<uint8_t> letterArr(letterStr.begin(),letterStr.end());
	Send(letterArr);
}


void ChatConnection::OnAccept( const std::string & host, uint16_t port )
{
	global_stream_lock.lock();
	std::cout << "[" << __PRETTY_FUNCTION__ << "] "
						<< host << ":" << port << std::endl;
	global_stream_lock.unlock();

	// Start the next receive
	Recv();
}


void ChatConnection::OnConnect( const std::string & host, uint16_t port )
{
	global_stream_lock.lock();
	std::cout << "[" << __PRETTY_FUNCTION__ << "] "
						<< host << ":" << port << std::endl;
	global_stream_lock.unlock();

	// Start the next receive
	Recv();
}

void ChatConnection::OnSend( const std::vector< uint8_t > & buffer )
{
	global_stream_lock.lock();
	// std::cout << "[" << __PRETTY_FUNCTION__ << "] "
	// 					<< buffer.size() << " bytes" << std::endl;
	// for( size_t x = 0; x < buffer.size(); ++x )
	// {
	// 	std::cout << std::hex << std::setfill( '0' ) <<
	// 		std::setw( 2 ) << (int)buffer[ x ] << " ";
	// 	if( ( x + 1 ) % 16 == 0 )
	// 	{
	// 		std::cout << std::endl;
	// 	}
	// }
	std::cout << std::endl;
	global_stream_lock.unlock();
}

void ChatConnection::OnRecv( std::vector< uint8_t > & buffer )
{
	global_stream_lock.lock();
	// std::cout << "[" << __PRETTY_FUNCTION__ << "] "
	// 					<< buffer.size() << " bytes" << std::endl;
	// for( size_t x = 0; x < buffer.size(); ++x )
	// {
	// 	std::cout << std::hex << std::setfill( '0' ) <<
	// 		std::setw( 2 ) << (int)buffer[ x ] << " ";
	// 	if( ( x + 1 ) % 16 == 0 )
	// 	{
	// 		std::cout << std::endl;
	// 	}
	// }
	// std::cout << std::endl;

	chat::Letter letter;
	letter.ParseFromString(std::string(buffer.begin(),buffer.end()));
	// std::cout << letter.body() << std::endl;

	global_stream_lock.unlock();
	// Start the next receive
	Recv();

	// Process the letter
	chatSrv->process(letter,
		boost::dynamic_pointer_cast<ChatConnection>(shared_from_this()));
}

void ChatConnection::OnTimer( const boost::posix_time::time_duration & delta )
{
	global_stream_lock.lock();
	// std::cout << "[" << __PRETTY_FUNCTION__ << "] " << delta << std::endl;
	global_stream_lock.unlock();
}

void ChatConnection::OnError( const boost::system::error_code & error )
{
	global_stream_lock.lock();
	std::cout << "[" << __PRETTY_FUNCTION__ << "] " << error
						<< ": " << error.message() << std::endl;
	global_stream_lock.unlock();

	chatSrv->delConnection(
		boost::dynamic_pointer_cast<ChatConnection>(shared_from_this()));
}

ChatConnection::ChatConnection( boost::shared_ptr<ChatServer> chatSrv,
	boost::shared_ptr< Hive > hive )
	: Connection( hive ), chatSrv(chatSrv)
{
}

ChatConnection::~ChatConnection()
{
}

boost::shared_ptr<Connection> ChatConnection::NewConnection(){
	return boost::shared_ptr<ChatConnection>(
		new ChatConnection(this->chatSrv,this->GetHive()));
}

bool ChatAcceptor::OnAccept( boost::shared_ptr< Connection > connection,
	const std::string & host, uint16_t port )
{
	global_stream_lock.lock();
	std::cout << "[" << __PRETTY_FUNCTION__ << "] "
						<< host << ":" << port << std::endl;
	global_stream_lock.unlock();

	chatSrv->addConnection(host+":"+
		boost::lexical_cast<std::string>(port),
		boost::dynamic_pointer_cast<ChatConnection>(connection));
	return true;
}

void ChatAcceptor::OnTimer( const boost::posix_time::time_duration & delta )
{
	global_stream_lock.lock();
	// std::cout << "[" << __PRETTY_FUNCTION__ << "] " << delta << std::endl;
	global_stream_lock.unlock();
}

void ChatAcceptor::OnError( const boost::system::error_code & error )
{
	global_stream_lock.lock();
	std::cout << "[" << __PRETTY_FUNCTION__ << "] " << error << std::endl;
	global_stream_lock.unlock();
}
ChatAcceptor::ChatAcceptor( boost::shared_ptr<ChatServer> chatSrv,
	boost::shared_ptr< Hive > hive )
	: Acceptor( hive ), chatSrv(chatSrv)
{
}

ChatAcceptor::~ChatAcceptor()
{
}

int main( int argc, char * argv[] )
{
	boost::shared_ptr<ChatServer> chatSrv( new ChatServer() );

	boost::shared_ptr< Hive > hive( new Hive() );

	boost::shared_ptr< ChatAcceptor > acceptor(
		new ChatAcceptor( chatSrv, hive ) );
	acceptor->Listen( "127.0.0.1", 7777 );

	boost::shared_ptr< ChatConnection > connection(
		new ChatConnection(chatSrv,hive));
	acceptor->Accept( connection );

	while(true) {
		hive->Poll();
		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	}

	hive->Stop();

	return 0;
}

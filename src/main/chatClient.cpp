#include "networkWrapper.hpp"
#include "chat.pb.h"
#include "chatClient.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <google/protobuf/text_format.h>

boost::mutex global_stream_lock;

void WorkerThread( boost::shared_ptr< Hive > hive)
{
	global_stream_lock.lock();
	std::cout << "thread starting" << std::endl;
	global_stream_lock.unlock();
	hive->Run();

}

bool ChatClient::pollInput() {
	global_stream_lock.lock();

	bool validInput = false;
	bool quit = false;

	std::string input;
	std::cout << "What do you want to do?" << std::endl
						<< "1) see users online" << std::endl
						<< "2) send a message" << std::endl
						<< "3) read mail" << std::endl
						<< "4) change your nick-name" << std::endl
						<< "5) quit chatting" << std::endl
						<< "Enter choice [1-5]: " << std::flush;
	while (!validInput) {
		std::cin.clear();
		std::cin >> input;
		if (!input.compare("1") ||
			!input.compare("2") ||
			!input.compare("3") ||
			!input.compare("4") ||
			!input.compare("5")) {
			validInput = true;
		} else {
			std::cout << "Invalid input.  Enter choice [1-5]: " << std::flush;
		}
	}

	if (!input.compare("1")) {
		std::cout << "Users online: " << std::endl;
		std::vector<std::pair<std::string,
													std::string> >::const_iterator
			it,end;
		end = users.end();
		for (it = users.begin(); it != end; ++it) {
			std::cout << it->second << std::endl;
		}
	} else if (!input.compare("2")) {
		validInput = false;
		int numInput;
		std::cout << "Users online: " << std::endl;
		std::vector<std::pair<std::string,
													std::string> >::const_iterator
			it,end;
		end = users.end();
		int i;
		for (i = 0, it = users.begin(); it != end; ++it,++i) {
			std::cout << i+1 << ")" << it->second << std::endl;
		}
		std::cout << "Enter recipient number [" << 1 << "-" << users.size()
							<< "]: " << std::flush;
		while (!validInput) {
			std::cin.clear();
			std::cin >> input;

			numInput = boost::lexical_cast<int>(input);
			if (numInput >= 1 && numInput <= users.size()) {
				validInput = true;
			} else {
				std::cout << "Invalid input. Enter recipient number ["
									<< 1 << "-" << users.size() << "]: " << std::flush;
			}
		}

		std::cout << "What is your message to "
							<< users.at(numInput-1).second << "?" << std::endl;

		std::cin.clear();
		std::ws(std::cin);
		std::getline(std::cin,input);

		std::cout << "Message sent!" << std::endl;

		chat::Letter letter;
		letter.set_type(chat::Letter_Type_TALKING);
		chat::TalkingLetter * talkingLetter;
		talkingLetter = letter.mutable_talkingletter();
		talkingLetter->set_body(input);
		talkingLetter->set_recipient(users.at(numInput-1).first);

		outgoing.push(letter);
	} else if(!input.compare("3")) {
		if(incoming.empty()) {
			std::cout << "No new messages." << std::endl;
		} else {
			std::cout << "Here are your new messages:" << std::endl;
			while(!incoming.empty()) {
				chat::Letter letter = incoming.front();
				incoming.pop();
				std::cout << "Message: " << letter.talkingletter().body()
									<< std::endl;
			}
		}
	} else if(!input.compare("4")) {
		std::cout << "Enter your new nick-name: " << std::flush;
		std::cin.clear();
		std::cin >> input;

		chat::Letter letter;
		letter.set_type(chat::Letter_Type_NAME);
		chat::NameLetter * nameLetter;
		nameLetter = letter.mutable_nameletter();
		nameLetter->set_name(input);
		outgoing.push(letter);
	} else {
    quit = true;
	}
	global_stream_lock.unlock();

	return !quit;
}

void ChatClient::setUsers(
	std::vector<std::pair<std::string,std::string> > users) {
	this->users = users;
}

bool ChatClient::getOutgoing(chat::Letter * letter) {
	if(!outgoing.empty()) {
		*letter = outgoing.front();
		outgoing.pop();
		return true;
	} else {
    return false;
	}
}

void ChatClient::dropLetter(chat::Letter letter) {
	incoming.push(letter);
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

	std::string str = "GET / HTTP/1.0\r\n\r\n";

	std::vector< uint8_t > request;
	std::copy( str.begin(), str.end(), std::back_inserter( request ) );
	Send( request );
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
	// std::cout << std::endl;
	global_stream_lock.unlock();
}

void ChatConnection::OnRecv( std::vector< uint8_t > & buffer )
{
	// global_stream_lock.lock();
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
	// global_stream_lock.unlock();



	chat::Letter letter;
	letter.ParseFromString(std::string(buffer.begin(),buffer.end()));
	if(letter.type() == chat::Letter_Type_SCOUT) {
		std::vector<std::pair<std::string, std::string> > users;
		chat::ScoutLetter scoutLetter = letter.scoutletter();
		for (int i = 0; i < scoutLetter.client_size(); ++i) {
			users.push_back(std::pair<std::string,std::string>(
					scoutLetter.client(i).id(),
						scoutLetter.client(i).nickname()));
		}
		chatClnt->setUsers(users);
	} else if(letter.type() == chat::Letter_Type_TALKING) {
		chatClnt->dropLetter(letter);
	}

	// Start the next receive
	Recv();
}

void ChatConnection::OnTimer( const boost::posix_time::time_duration & delta )
{
	// global_stream_lock.lock();
	// std::cout << "[" << __PRETTY_FUNCTION__ << "] " << delta << std::endl;
	// global_stream_lock.unlock();
	chat::Letter letter;
	letter.set_type(chat::Letter_Type_SCOUT);
	SendLetter(letter);
	while(chatClnt->getOutgoing(&letter)) {
		SendLetter(letter);
	}
}

void ChatConnection::OnError( const boost::system::error_code & error )
{
	global_stream_lock.lock();
	std::cout << "[" << __PRETTY_FUNCTION__ << "] " << error
						<< ": " << error.message() << std::endl;
	global_stream_lock.unlock();
}

ChatConnection::ChatConnection( boost::shared_ptr<ChatClient> chatClnt,
	boost::shared_ptr< Hive > hive )
	: Connection( hive ), chatClnt(chatClnt)
{
}

ChatConnection::~ChatConnection()
{
}

boost::shared_ptr<Connection> ChatConnection::NewConnection () {
	return boost::shared_ptr<ChatConnection>(new ChatConnection(
			this->chatClnt,this->GetHive()));
}

int main( int argc, char * argv[] )
{
	std::string srv = "localhost";
	std::string name = "Anonymous";
	if(argc > 3)
		std::cout << "too many arguments" << std::endl;
	else if(argc == 3){
		srv = argv[1];
		name = argv[2];
	}
	else if(argc == 2){
		srv = argv[1];
		name = "Anonymous";
	}

	std::cout << "connecting too: " << srv << std::endl;
	std::cout << "name is: " << name << std::endl;

	boost::shared_ptr<ChatClient> chatClnt( new ChatClient() );

	boost::shared_ptr< Hive > hive( new Hive() );

	boost::shared_ptr< ChatConnection > connection(
		new ChatConnection( chatClnt,hive ) );
	connection->Connect( srv, 7777 );

	boost::thread_group worker_threads;
	worker_threads.create_thread(
		boost::bind(&WorkerThread, hive));

	boost::this_thread::sleep(boost::posix_time::milliseconds(2000));

	while (chatClnt->pollInput()) {
		boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
	}

	hive->Stop();

	return 0;
}

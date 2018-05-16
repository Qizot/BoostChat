#pragma once
#include <boost/asio.hpp>
#include "ChatServer.h"
#include "ChatMessage.h"
#include <set>
#include <memory>
#include <deque>
#include <queue>



namespace chat {
	class ChatParticipant;
	class SessionParticipant;
	class ChatRoom;
	class ChatServer;


	using participant_ptr = std::shared_ptr<chat::ChatParticipant>;
	using message_ptr = std::shared_ptr<chat::ChatMessage>;
	using boost::asio::ip::tcp;

	class ChatParticipant
	{
	public:

		virtual ~ChatParticipant() {};
		virtual void deliver(message_ptr) = 0;
		virtual void participate() = 0;
	};

	class ChatRoom
	{
		using participant_ptr = std::shared_ptr<chat::ChatParticipant>;
		using participants_pool = std::set<participant_ptr>;
		using current_messages = std::deque<message_ptr>;
		static constexpr size_t messages_limit = 20;
	public:
		void join(participant_ptr);			  // connect user to the room
		void leave(participant_ptr);		  // diconnect user from the chat
		void deliver(message_ptr); // deliver message to all participants and add it to the current messages

	private:
		participants_pool participants;
		current_messages messages;
	};

	class SessionParticipant : public ChatParticipant, public std::enable_shared_from_this<SessionParticipant>
	{
		using MessageQueue = std::queue<message_ptr>;
	public:
		SessionParticipant(boost::asio::io_service& read_service, 
						   chat::ChatRoom& room) : ios(read_service),
						   sock(read_service), room(room), read_msg(new BaseMessage) {}

		void participate(); // starts receiving messages from remote client
		void deliver(message_ptr);

		void handle_header_read(const boost::system::error_code&);
		void handle_body_read(const boost::system::error_code&);

		void do_write();
		void handle_write(const boost::system::error_code&);

		tcp::socket& socket() { return sock; }


	private:
		boost::asio::io_service& ios;
		tcp::socket sock;
		chat::ChatRoom& room;
		message_ptr read_msg;
		MessageQueue messages;

	};

	class ChatServer
	{
	public:
		ChatServer(boost::asio::io_service& io_service, const tcp::endpoint& endpoint) : ios(io_service), acceptor(io_service, endpoint) {}

		void start_accepting();
		void handle_accept(const boost::system::error_code&, participant_ptr);

	private:
		boost::asio::io_service& ios;
		tcp::acceptor acceptor;
		chat::ChatRoom room;
	};



} // chat namespace


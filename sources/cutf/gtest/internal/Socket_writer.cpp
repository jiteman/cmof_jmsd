#include "Socket_writer.h"


#if GTEST_CAN_STREAM_RESULTS_
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif



namespace jmsd {
namespace cutf {
namespace internal {


#if GTEST_CAN_STREAM_RESULTS_


SocketWriter::SocketWriter(const std::string& host, const std::string& port)
	:
		sockfd_(-1),
		host_name_(host),
		port_num_(port)
{
	this->MakeConnection();
}

SocketWriter::~SocketWriter() {
	if (sockfd_ != -1) {
		CloseConnection();
	}
}

// Sends a string to the socket.
void SocketWriter::Send(const std::string& message) {
	GTEST_CHECK_(sockfd_ != -1) << "Send() can be called only when there is a connection.";

	auto const len = static_cast<size_t>(message.length());

	if ( write(sockfd_, message.c_str(), len) != static_cast<ssize_t>(len) ) {
		GTEST_LOG_(WARNING) << "stream_result_to: failed to stream to " << host_name_ << ":" << port_num_;
	}
}

// Creates a client socket and connects to the server.
void SocketWriter::MakeConnection() {
	GTEST_CHECK_(sockfd_ == -1) << "MakeConnection() can't be called when there is already a connection.";

	addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;    // To allow both IPv4 and IPv6 addresses.
	hints.ai_socktype = SOCK_STREAM;
	addrinfo* servinfo = nullptr;

	// Use the getaddrinfo() to get a linked list of IP addresses for
	// the given host name.
	const int error_num = getaddrinfo( host_name_.c_str(), port_num_.c_str(), &hints, &servinfo );

	if ( error_num != 0 ) {
		GTEST_LOG_(WARNING) << "stream_result_to: getaddrinfo() failed: " << gai_strerror(error_num);
	}

	// Loop through all the results and connect to the first we can.
	for ( addrinfo *cur_addr = servinfo; sockfd_ == -1 && cur_addr != nullptr; cur_addr = cur_addr->ai_next ) {
		sockfd_ = socket( cur_addr->ai_family, cur_addr->ai_socktype, cur_addr->ai_protocol );

		if (sockfd_ != -1) {
			// Connect the client socket to the server socket.
			if (connect(sockfd_, cur_addr->ai_addr, cur_addr->ai_addrlen) == -1) {
				close(sockfd_);
				sockfd_ = -1;
			}
		}
	}

	freeaddrinfo(servinfo);  // all done with this structure

	if ( sockfd_ == -1 ) {
		GTEST_LOG_(WARNING) << "stream_result_to: failed to connect to " << host_name_ << ":" << port_num_;
	}
}

// Closes the socket.
void SocketWriter::CloseConnection() {
	GTEST_CHECK_(sockfd_ != -1) << "CloseConnection() can be called only when there is a connection.";

	close(sockfd_);
	sockfd_ = -1;
}


#endif // #if GTEST_CAN_STREAM_RESULTS_


} // namespace internal
} // namespace cutf
} // namespace jmsd

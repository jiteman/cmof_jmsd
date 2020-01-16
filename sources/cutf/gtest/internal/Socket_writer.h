#pragma once

#include "Socket_writer.hxx"


#include "Abstract_socket_writer.h"

#include "gtest-port.h"


namespace jmsd {
namespace cutf {
namespace internal {


#if GTEST_CAN_STREAM_RESULTS_

// Concrete class for actually writing strings to a socket.
class SocketWriter :
	public AbstractSocketWriter
{
public:
	SocketWriter(const std::string& host, const std::string& port);

	~SocketWriter() override;

	// Sends a string to the socket.
	void Send(const std::string& message) override;

private:
	// Creates a client socket and connects to the server.
	void MakeConnection();

	// Closes the socket.
	void CloseConnection() override;

	int sockfd_;  // socket file descriptor
	const std::string host_name_;
	const std::string port_num_;

	GTEST_DISALLOW_COPY_AND_ASSIGN_(SocketWriter);
};  // class SocketWriter


#endif // #if GTEST_CAN_STREAM_RESULTS_


} // namespace internal
} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing

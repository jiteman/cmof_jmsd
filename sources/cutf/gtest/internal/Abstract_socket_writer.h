#pragma once

#include "Abstract_socket_writer.hxx"


namespace jmsd {
namespace cutf {
namespace internal {


// Abstract base class for writing strings to a socket.
class AbstractSocketWriter {

public:
	virtual ~AbstractSocketWriter() = 0;

	// Sends a string to the socket.
	virtual void Send(const std::string& message) = 0;

	// Closes the socket.
	virtual void CloseConnection();

	// Sends a string and a newline to the socket.
	void SendLn(const std::string& message);

};


} // namespace internal
} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing

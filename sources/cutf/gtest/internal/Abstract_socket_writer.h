#pragma once

#include "Abstract_socket_writer.hxx"


#include <string>


namespace jmsd {
namespace cutf {
namespace internal {


// Abstract base class for writing strings to a socket.
class AbstractSocketWriter {

public:
	virtual ~AbstractSocketWriter() = 0;

	// Sends a string to the socket.
	virtual void Send( ::std::string const &message ) = 0;

	// Closes the socket.
	virtual void CloseConnection();

	// Sends a string and a newline to the socket.
	void SendLn( ::std::string const &message );

};


} // namespace internal
} // namespace cutf
} // namespace jmsd

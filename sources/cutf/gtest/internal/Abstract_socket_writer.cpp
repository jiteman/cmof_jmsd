#include "Abstract_socket_writer.h"


namespace jmsd {
namespace cutf {
namespace internal {


AbstractSocketWriter::~AbstractSocketWriter()
{}

// Closes the socket.
void AbstractSocketWriter::CloseConnection()
{}

// Sends a string and a newline to the socket.
void AbstractSocketWriter::SendLn( const std::string& message) {
	Send(message + "\n");
}


} // namespace internal
} // namespace cutf
} // namespace jmsd

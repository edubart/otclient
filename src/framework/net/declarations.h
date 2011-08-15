#ifndef FRAMEWORK_NET_DECLARATIONS_H
#define FRAMEWORK_NET_DECLARATIONS_H

#include <framework/global.h>
#include <boost/asio.hpp>

namespace asio = boost::asio;

class Connection;
class InputMessage;
class OutputMessage;
class Protocol;

typedef std::shared_ptr<Connection> ConnectionPtr;
typedef std::shared_ptr<Protocol> ProtocolPtr;

#endif

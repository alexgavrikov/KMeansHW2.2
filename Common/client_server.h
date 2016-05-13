/*
 * client_server.h
 *
 *  Created on: 11 мая 2016 г.
 *      Author: user
 */

#ifndef COMMON_CLIENT_SERVER_H_
#define COMMON_CLIENT_SERVER_H_

#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

class ClientServerBase {
protected:
  virtual ~ClientServerBase() {
  }
  virtual bool Send(const std::string& message, int socket) const;
  virtual std::string Recieve(int socket) const;
  static bool ResolveHost(const std::string &host, int &addr);

private:
  static const char kDelimiterBetweenContentLengthAndContent = '\t';
  static const size_t kDelimiterLength = 1;
};

class LocalClient : private ClientServerBase {
public:
  LocalClient(const std::string& host, int port) {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    Connect(host, port);
  }

  virtual bool Send(const std::string& message) const {
    return ClientServerBase::Send(message, server_socket);
  }

  virtual std::string Recieve() const {
    return ClientServerBase::Recieve(server_socket);
  }

private:
  void Connect(const std::string &host, int port);

  int server_socket;
};



#endif /* COMMON_CLIENT_SERVER_H_ */

#ifndef SERVER_H
#define SERVER_H

#include "BasicMessage.h"
#include "Protocol.h"

class Server {
  public:
    Server();
    Server(const Server &) = delete;
    Server &operator=(const Server &) = delete;
    ~Server() = default;

    void handleUserInfo(const std::shared_ptr<BasicMessage> &msg);
    void handleChatInfo(const std::shared_ptr<BasicMessage> &msg);
    void handleGroupInfo(const std::shared_ptr<BasicMessage> &msg);
    void handleLogoutInfo(const std::shared_ptr<BasicMessage> &msg);
    void handleFileInfo(const std::shared_ptr<BasicMessage> &msg);

    int getLocalPort() { return protocol.getLocolPort(); }

  private:
    Protocol protocol;
    std::unordered_map<std::string, UserInfo> userInfos;
    std::unordered_map<std::string, GroupInfo> groupInfos;
    int fileID = 0;
};

#endif  // SERVER_H

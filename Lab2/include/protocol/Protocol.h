#ifndef PROTOCOL_H
#define PROTOCOL_H
// 协议类，负责从UdpConnector中接收和发送消息，并调用对应回调函数
// 需要向类中注册对应的回调函数

#include "BasicMessage.h"
#include "UdpConnector.h"
#include "functional"

class Protocol {
  public:
    // 注册三个回调函数，接收到对应类型消息将调用
    Protocol(
        std::function<void(const std::shared_ptr<BasicMessage> &)> onUserInfo,
        std::function<void(const std::shared_ptr<BasicMessage> &)> onChatInfo,
        std::function<void(const std::shared_ptr<BasicMessage> &)> onGroupInfo,
        std::function<void(const std::shared_ptr<BasicMessage> &)>
            onLogoutInfo);

    void sendUserInfo(UserInfo &userInfo,
                      const std::string &ip = "",
                      const int port = -1);
    void sendChatInfo(ChatInfo &chatInfo,
                      const std::string &ip = "",
                      const int port = -1);
    void sendGroupInfo(GroupInfo &groupInfo,
                       const std::string &ip = "",
                       const int port = -1);
    void sendLogoutInfo(LogoutInfo &logoutInfo,
                        const std::string &ip = "",
                        const int port = -1);
    void sendMsg(const std::shared_ptr<BasicMessage> &msg,
                 const std::string &ip = "",
                 const int port = -1);
    int getLocolPort();
    // 客户端设置服务器地址
    void setRemoteAddr(const std::string &ip, int port);

  private:
    // 接收消息，注册为UdpConnector接收到消息后的回调。其会根据消息类型调用对应的回调函数
    void recvMsg(const std::string &msg, const std::string &ip, int port);

  private:
    UdpConnector &udpConnector;
    std::function<void(const std::shared_ptr<BasicMessage> &)> onUserInfo;
    std::function<void(const std::shared_ptr<BasicMessage> &)> onChatInfo;
    std::function<void(const std::shared_ptr<BasicMessage> &)> onGroupInfo;
    std::function<void(const std::shared_ptr<BasicMessage> &)> onLogoutInfo;
};

#endif  // PROTOCOL_H

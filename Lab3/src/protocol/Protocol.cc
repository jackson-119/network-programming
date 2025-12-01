#include "Logger.h"
#include "Protocol.h"

Protocol::Protocol(
    std::function<void(const std::shared_ptr<BasicMessage> &)> onUserInfo,
    std::function<void(const std::shared_ptr<BasicMessage> &)> onChatInfo,
    std::function<void(const std::shared_ptr<BasicMessage> &)> onGroupInfo,
    std::function<void(const std::shared_ptr<BasicMessage> &)> onLogoutInfo,
    std::function<void(const std::shared_ptr<BasicMessage> &)> onFileInfo)
    : udpConnector(UdpConnector::getInstance()),
      onUserInfo(onUserInfo),
      onChatInfo(onChatInfo),
      onGroupInfo(onGroupInfo),
      onLogoutInfo(onLogoutInfo),
      onFileInfo(onFileInfo) {
    udpConnector.setOnRecvMsg(
        [this](const std::string &msg, const std::string &ip, int port) {
            this->recvMsg(msg, ip, port);
        });
    udpConnector.initialize();
};

void Protocol::sendUserInfo(UserInfo &userInfo,
                            const std::string &ip,
                            const int port) {
    auto msg = BasicMessage::makeUserInfoMsg(userInfo);
    sendMsg(msg, ip, port);
}

void Protocol::sendChatInfo(ChatInfo &chatInfo,
                            const std::string &ip,
                            const int port) {
    auto msg = BasicMessage::makeChatInfoMsg(chatInfo);
    sendMsg(msg, ip, port);
}

void Protocol::sendGroupInfo(GroupInfo &groupInfo,
                             const std::string &ip,
                             const int port) {
    auto msg = BasicMessage::makeGroupInfoMsg(groupInfo);
    sendMsg(msg, ip, port);
}

void Protocol::sendLogoutInfo(LogoutInfo &logoutInfo,
                              const std::string &ip,
                              const int port) {
    auto msg = BasicMessage::makeLogoutInfoMsg(logoutInfo);
    sendMsg(msg, ip, port);
}

void Protocol::sendFileInfo(FileInfo &fileInfo,
                            const std::string &ip,
                            const int port) {
    auto msg = BasicMessage::makeFileInfoMsg(fileInfo);
    sendMsg(msg, ip, port);
    Logger::get_instance()->info(
        "发送文件信息给{}，文件名为{}", fileInfo.receiver, fileInfo.fileName);
}

void Protocol::sendMsg(const std::shared_ptr<BasicMessage> &msg,
                       const std::string &ip,
                       const int port) {
    if (ip.size() == 0) {
        udpConnector.sendMsgAsync(msg->serialize());
    } else {
        udpConnector.sendMsgAsync(msg->serialize(), ip, port);
    }
}

int Protocol::getLocolPort() { return udpConnector.getLocalPort(); }

void Protocol::setRemoteAddr(const std::string &ip, int port) {
    udpConnector.setRemoteAddr(ip, port);
}

void Protocol::recvMsg(const std::string &msg,
                       const std::string &ip,
                       int port) {
    std::istringstream iss(msg);
    cereal::PortableBinaryInputArchive ar(iss);
    std::shared_ptr<BasicMessage> message = std::make_shared<BasicMessage>();
    ar(*message);

    switch (message->getType()) {
        case INFO: {
            auto userInfo = message->getUserInfo();
            if (userInfo.value().ip.size() == 0) {
                message->setUserInfoIpAndPort(ip, port);
            }
            onUserInfo(message);
            break;
        }
        case CHAT: {
            onChatInfo(message);
            break;
        }
        case NEW_GROUP: {
            onGroupInfo(message);
            break;
        }
        case LOGOUT: {
            onLogoutInfo(message);
            break;
        }
        case FILE_SLICE: {
            onFileInfo(message);
            break;
        }
        default:
            break;
    }
}

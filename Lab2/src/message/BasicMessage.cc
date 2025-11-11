#include "BasicMessage.h"
#include <chrono>

BasicMessage::BasicMessage(BasicMessage&& other) noexcept
    : type(other.type),
      userInfo(std::move(other.userInfo)),
      chatInfo(std::move(other.chatInfo)),
      groupInfo(std::move(other.groupInfo)) {}

BasicMessage& BasicMessage::operator=(BasicMessage&& other) noexcept {
    if (this != &other) {
        type = other.type;
        userInfo = std::move(other.userInfo);
        chatInfo = std::move(other.chatInfo);
        groupInfo = std::move(other.groupInfo);
    }
    return *this;
}

std::string BasicMessage::serialize() const {
    std::ostringstream oss;
    {
        cereal::PortableBinaryOutputArchive ar(oss);
        ar(*this);
    }
    return oss.str();
}

std::shared_ptr<BasicMessage> BasicMessage::makeUserInfoMsg(
    const std::string& nickName,
    const std::string& ip,
    const int port,
    const std::string& avatar,
    const std::string& avatarFormat) {
    auto msg = std::make_shared<BasicMessage>();
    msg->type = INFO;
    msg->userInfo = UserInfo{nickName, ip, port, avatar, avatarFormat};
    return msg;
}

std::shared_ptr<BasicMessage> BasicMessage::makeUserInfoMsg(
    const UserInfo& info) {
    auto msg = std::make_shared<BasicMessage>();
    msg->type = INFO;
    msg->userInfo = info;
    return msg;
}

std::shared_ptr<BasicMessage> BasicMessage::makeChatInfoMsg(
    const std::string& sender,
    const std::string& receiver,
    const std::string& content) {
    auto msg = std::make_shared<BasicMessage>();
    msg->type = CHAT;
    msg->chatInfo = ChatInfo{sender, receiver, content};
    msg->chatInfo->timestamp =
        std::chrono::system_clock::now().time_since_epoch().count();
    return msg;
}

std::shared_ptr<BasicMessage> BasicMessage::makeChatInfoMsg(
    const ChatInfo& info) {
    auto msg = std::make_shared<BasicMessage>();
    msg->type = CHAT;
    msg->chatInfo = info;
    return msg;
}

std::shared_ptr<BasicMessage> BasicMessage::makeGroupInfoMsg(
    const std::string& groupName, const std::vector<std::string>& members) {
    auto msg = std::make_shared<BasicMessage>();
    msg->type = NEW_GROUP;
    msg->groupInfo = GroupInfo{groupName, members};
    return msg;
}

std::shared_ptr<BasicMessage> BasicMessage::makeLogoutInfoMsg(
    const std::string& nickName) {
    auto msg = std::make_shared<BasicMessage>();
    msg->type = LOGOUT;
    msg->logoutInfo = LogoutInfo{nickName};
    return msg;
}

std::shared_ptr<BasicMessage> BasicMessage::makeGroupInfoMsg(
    const GroupInfo& info) {
    auto msg = std::make_shared<BasicMessage>();
    msg->type = NEW_GROUP;
    msg->groupInfo = info;
    return msg;
}

std::shared_ptr<BasicMessage> BasicMessage::makeLogoutInfoMsg(
    const LogoutInfo& info) {
    auto msg = std::make_shared<BasicMessage>();
    msg->type = LOGOUT;
    msg->logoutInfo = info;
    return msg;
}

void BasicMessage::setUserInfoIpAndPort(const std::string& ip, const int port) {
    if (userInfo.has_value()) {
        userInfo->ip = ip;
        userInfo->port = port;
    }
}

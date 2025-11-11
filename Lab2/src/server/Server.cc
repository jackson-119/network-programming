#include "Server.h"
#include "Logger.h"

Server::Server()
    : protocol(Protocol(
          [this](const std::shared_ptr<BasicMessage> &msg) {
              this->handleUserInfo(msg);
          },
          [this](const std::shared_ptr<BasicMessage> &msg) {
              this->handleChatInfo(msg);
          },
          [this](const std::shared_ptr<BasicMessage> &msg) {
              this->handleGroupInfo(msg);
          },
          [this](const std::shared_ptr<BasicMessage> &msg) {
              this->handleLogoutInfo(msg);
          })) {
    Logger::get_instance()->info("服务端已启动，监听端口为{}",
                                 protocol.getLocolPort());
}

void Server::handleUserInfo(const std::shared_ptr<BasicMessage> &msg) {
    UserInfo newUserInfo = msg->getUserInfo().value();
    Logger::get_instance()->info("收到来自{}:{}的用户信息，昵称为{}",
                                 newUserInfo.ip,
                                 newUserInfo.port,
                                 newUserInfo.nickName);

    // 将新用户信息发送给其他用户
    for (auto &receiver : userInfos) {
        if (receiver.first != newUserInfo.nickName) {
            protocol.sendUserInfo(
                newUserInfo, receiver.second.ip, receiver.second.port);
        }
    }

    // 将旧用户信息发送给新用户
    for (auto &oldUserInfo : userInfos) {
        if (oldUserInfo.first == newUserInfo.nickName) {
            continue;
        }
        protocol.sendUserInfo(
            oldUserInfo.second, newUserInfo.ip, newUserInfo.port);
    }

    userInfos[newUserInfo.nickName] = newUserInfo;
}

void Server::handleChatInfo(const std::shared_ptr<BasicMessage> &msg) {
    ChatInfo chatInfo = msg->getChatInfo().value();
    Logger::get_instance()->info(
        "收到来自{}的消息，内容为{}", chatInfo.sender, chatInfo.content);
    if (userInfos.find(chatInfo.receiver) != userInfos.end()) {
        protocol.sendChatInfo(chatInfo,
                              userInfos[chatInfo.receiver].ip,
                              userInfos[chatInfo.receiver].port);
    } else if (groupInfos.find(chatInfo.receiver) != groupInfos.end()) {
        // 群消息
        for (auto &member : groupInfos[chatInfo.receiver].members) {
            if (member == chatInfo.sender) {
                // 跳过发送者
                continue;
            }
            protocol.sendChatInfo(
                chatInfo, userInfos[member].ip, userInfos[member].port);
        }
    } else {
        Logger::get_instance()->warn("未找到接收者{}", chatInfo.receiver);
    }
}

void Server::handleGroupInfo(const std::shared_ptr<BasicMessage> &msg) {
    GroupInfo groupInfo = msg->getGroupInfo().value();
    Logger::get_instance()->info("收到来自名为{}的群组信息",
                                 groupInfo.groupName);
    groupInfos[groupInfo.groupName] = groupInfo;
    for (auto &member : groupInfo.members) {
        protocol.sendGroupInfo(
            groupInfo, userInfos[member].ip, userInfos[member].port);
    }
}

void Server::handleLogoutInfo(const std::shared_ptr<BasicMessage> &msg) {
    LogoutInfo logoutInfo = msg->getLogoutInfo().value();
    Logger::get_instance()->info("用户{}已下线", logoutInfo.nickName);
    userInfos.erase(logoutInfo.nickName);
    for (auto &userInfo : userInfos) {
        protocol.sendLogoutInfo(
            logoutInfo, userInfo.second.ip, userInfo.second.port);
    }
}

int main() {
    Server server;
    printf("Listening the port: %d\n", server.getLocalPort());
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}

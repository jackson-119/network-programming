#ifndef BASICMESSAGE_H
#define BASICMESSAGE_H

#include <cstdint>
#include <optional>
#include <string>
#include "cereal/archives/portable_binary.hpp"
#include "cereal/types/optional.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/vector.hpp"

struct UserInfo {
    std::string nickName;
    std::string ip;
    int port;
    std::string avatar;
    std::string avatarFormat;

    template <class Archive>
    void serialize(Archive& ar) {
        ar(CEREAL_NVP(nickName),
           CEREAL_NVP(ip),
           CEREAL_NVP(port),
           CEREAL_NVP(avatar),
           CEREAL_NVP(avatarFormat));
    }
};

struct GroupInfo {
    std::string groupName;
    std::vector<std::string> members;

    template <class Archive>
    void serialize(Archive& ar) {
        ar(CEREAL_NVP(groupName), CEREAL_NVP(members));
    }
};

struct ChatInfo {
    std::string sender;
    std::string receiver;  // 用户或者群组
    std::string content;
    int64_t timestamp = 0;

    template <class Archive>
    void serialize(Archive& ar) {
        ar(CEREAL_NVP(sender),
           CEREAL_NVP(receiver),
           CEREAL_NVP(content),
           CEREAL_NVP(timestamp));
    }
};

struct LogoutInfo {
    std::string nickName;

    template <class Archive>
    void serialize(Archive& ar) {
        ar(CEREAL_NVP(nickName));
    }
};

struct FileInfo {
    std::string receiver;
    std::string sender;
    std::string fileName;
    std::string fileData;
    int totalSize;
    int sliceIndex;
    int sliceSize;
    bool isDir;

    template <class Archive>
    void serialize(Archive& ar) {
        ar(CEREAL_NVP(receiver),
           CEREAL_NVP(sender),
           CEREAL_NVP(fileName),
           CEREAL_NVP(fileData),
           CEREAL_NVP(totalSize),
           CEREAL_NVP(sliceIndex),
           CEREAL_NVP(sliceSize),
           CEREAL_NVP(isDir));
    }
};

enum MessageType {
    INFO,       // 身份信息
    CHAT,       // 聊天消息
    NEW_GROUP,  // 创建新群组
    LOGOUT,     // 退出登录
    FILE_SLICE,       // 文件传输

};

class BasicMessage {
  public:
    BasicMessage() = default;
    BasicMessage(BasicMessage&& other) noexcept;
    BasicMessage& operator=(BasicMessage&& other) noexcept;
    virtual ~BasicMessage() = default;

    std::string serialize() const;
    std::optional<UserInfo> getUserInfo() const { return userInfo; }
    std::optional<ChatInfo> getChatInfo() const { return chatInfo; }
    std::optional<GroupInfo> getGroupInfo() const { return groupInfo; }
    std::optional<LogoutInfo> getLogoutInfo() const { return logoutInfo; }
    std::optional<FileInfo> getFileInfo() const { return fileInfo; }

    MessageType getType() const { return type; }

    static std::shared_ptr<BasicMessage> makeUserInfoMsg(
        const std::string& nickName,
        const std::string& ip,
        const int port,
        const std::string& avatar,
        const std::string& avatarFormat);

    static std::shared_ptr<BasicMessage> makeChatInfoMsg(
        const std::string& sender,
        const std::string& receiver,
        const std::string& content);

    static std::shared_ptr<BasicMessage> makeGroupInfoMsg(
        const std::string& groupName, const std::vector<std::string>& members);

    static std::shared_ptr<BasicMessage> makeLogoutInfoMsg(
        const std::string& nickName);

    static std::shared_ptr<BasicMessage> makeFileInfoMsg(
        const std::string& receiver,
        const std::string& sender,
        const std::string& fileName,
        const std::string& fileData,
        const int totalSize,
        const int sliceIndex,
        const int sliceSize,
        const bool isDir);

    static std::shared_ptr<BasicMessage> makeUserInfoMsg(const UserInfo& info);

    static std::shared_ptr<BasicMessage> makeChatInfoMsg(const ChatInfo& info);

    static std::shared_ptr<BasicMessage> makeGroupInfoMsg(
        const GroupInfo& info);

    static std::shared_ptr<BasicMessage> makeLogoutInfoMsg(
        const LogoutInfo& info);
    
    static std::shared_ptr<BasicMessage> makeFileInfoMsg(
        const FileInfo& info);

    void setUserInfoIpAndPort(const std::string& ip, const int port);

    template <class Archive>
    void serialize(Archive& ar) {
        ar(CEREAL_NVP(type),
           CEREAL_NVP(userInfo),
           CEREAL_NVP(chatInfo),
           CEREAL_NVP(groupInfo),
           CEREAL_NVP(logoutInfo),
           CEREAL_NVP(fileInfo));
    }

  private:
    MessageType type;
    std::optional<UserInfo> userInfo;      // 当且仅当type==INFO
    std::optional<ChatInfo> chatInfo;      // 当且仅当type==CHAT
    std::optional<GroupInfo> groupInfo;    // 当且仅当type==CREATE_GROUP
    std::optional<LogoutInfo> logoutInfo;  // 当且仅当type==LOGOUT
    std::optional<FileInfo> fileInfo;      // 当且仅当type==FILE
};

#endif  // BASICMESSAGE_H

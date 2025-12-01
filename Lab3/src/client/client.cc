#include <qdebug.h>
#include <qurl.h>
#include <QBuffer>
#include <QDateTime>
#include <QFile>
#include <QPainter>
#include <QPainterPath>
#include <filesystem>
#include <fstream>
#include "Client.h"
#include "Logger.h"

#define SLICE_SIZE 4096

Client::Client(QObject *parent)
    : QObject(parent),
      protocol(Protocol(
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
          },
          [this](const std::shared_ptr<BasicMessage> &msg) {
              this->handleFileInfo(msg);
          })) {
    Logger::get_instance()->info("客户已启动，监听端口为{}",
                                 protocol.getLocolPort());
    protocol.setRemoteAddr(defaultServerIp, defaultServerPort);
    qRegisterMetaType<UserInfo>("UserInfo");
    qRegisterMetaType<ChatInfo>("ChatInfo");
    qRegisterMetaType<GroupInfo>("GroupInfo");
}
Client::~Client() { Logger::get_instance()->info("客户端已关闭"); }
void Client::handleUserInfo(const std::shared_ptr<BasicMessage> &msg) {
    UserInfo newUserInfo = msg->getUserInfo().value();
    userInfos[newUserInfo.nickName] = newUserInfo;

    // 创建 QVariantMap 来存储用户信息
    QVariantMap userInfoMap;

    // 转换基本信息
    userInfoMap["nickName"] = QString::fromStdString(newUserInfo.nickName);
    userInfoMap["ip"] = QString::fromStdString(newUserInfo.ip);
    userInfoMap["port"] = newUserInfo.port;

    // 处理头像
    if (!newUserInfo.avatar.empty()) {
        QByteArray avatarData(newUserInfo.avatar.data(),
                              newUserInfo.avatar.size());
        QString avatarBase64 =
            QString("data:image/%1;base64,%2")
                .arg(QString::fromStdString(newUserInfo.avatarFormat))
                .arg(QString(avatarData.toBase64()));
        userInfoMap["avatar"] = avatarBase64;
    } else {
        userInfoMap["avatar"] = "";  // 或者设置一个默认头像
    }

    // 发送修改后的用户信息
    emit userInfoReceived(userInfoMap);
}
void Client::handleChatInfo(const std::shared_ptr<BasicMessage> &msg) {
    ChatInfo chatInfo = msg->getChatInfo().value();
    // 将 C++ 结构转换为 QVariantMap，以便在 QML 中使用
    QVariantMap chatInfoMap;
    chatInfoMap["sender"] = QString::fromStdString(chatInfo.sender);
    chatInfoMap["receiver"] = QString::fromStdString(chatInfo.receiver);
    chatInfoMap["content"] = QString::fromStdString(chatInfo.content);
    chatInfoMap["timestamp"] =
        QDateTime::fromMSecsSinceEpoch(chatInfo.timestamp);

    // 发送信号到 QML
    emit chatInfoReceived(chatInfoMap);
}

void Client::handleGroupInfo(const std::shared_ptr<BasicMessage> &msg) {
    GroupInfo groupInfo = msg->getGroupInfo().value();
    QVariantMap groupInfoMap;
    groupInfoMap["nickName"] = QString::fromStdString(groupInfo.groupName);
    groupInfoMap["ip"] = QString::number(groupInfo.members.size()) + " 人";
    groupInfoMap["port"] = -1;

    // 生成群头像
    QImage groupAvatar = createGroupAvatar(groupInfo, 100);

    // 将群头像转换为 base64 编码的字符串
    QByteArray avatarByteArray;
    QBuffer buffer(&avatarByteArray);
    buffer.open(QIODevice::WriteOnly);
    groupAvatar.save(&buffer, "PNG");
    QString avatarBase64 = QString("data:image/png;base64,%1")
                               .arg(QString(avatarByteArray.toBase64()));

    groupInfoMap["avatar"] = avatarBase64;
    groupInfoMap["avatarFormat"] = "png";

    emit userInfoReceived(groupInfoMap);
}

QImage Client::createGroupAvatar(const GroupInfo &groupInfo, int size) {
    QImage groupAvatar(size, size, QImage::Format_ARGB32);
    groupAvatar.fill(Qt::white);

    QPainter painter(&groupAvatar);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    int avatarCount = 0;
    int singleSize = size / 2;

    for (const auto &member : groupInfo.members) {
        if (avatarCount == 4) break;  // Stop if we have 4 avatars

        QImage avatar;

        if (userInfos.find(member) != userInfos.end() &&
            !userInfos[member].avatar.empty()) {
            // Load avatar from userInfos
            const std::string &avatarData = userInfos[member].avatar;
            const std::string &avatarFormat = userInfos[member].avatarFormat;

            avatar.loadFromData(
                reinterpret_cast<const uchar *>(avatarData.data()),
                avatarData.size(),
                avatarFormat.c_str());

            if (!avatar.isNull()) {
                // Scale the avatar
                avatar = avatar.scaled(singleSize,
                                       singleSize,
                                       Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation);

                int x = (avatarCount % 2) * singleSize;
                int y = (avatarCount / 2) * singleSize;

                QRect targetRect(x, y, singleSize, singleSize);

                // Create circular clipping path
                QPainterPath path;
                path.addEllipse(targetRect);
                painter.setClipPath(path);

                // Draw avatar
                painter.drawImage(targetRect, avatar);

                // Reset clipping path
                painter.setClipPath(QPainterPath());

                // Draw border
                painter.setPen(QPen(Qt::lightGray, 2));
                painter.drawEllipse(targetRect);

                avatarCount++;
            }
        }
        // If no avatar or avatar is null, we simply skip this member
    }

    painter.end();

    return groupAvatar;
}

void Client::handleLogoutInfo(const std::shared_ptr<BasicMessage> &msg) {
    auto logoutInfo = msg->getLogoutInfo().value();
    userInfos.erase(logoutInfo.nickName);
    QVariantMap logoutInfoMap;
    logoutInfoMap["nickName"] = QString::fromStdString(logoutInfo.nickName);
    emit logoutInfoReceived(logoutInfoMap);
}

void Client::handleFileInfo(const std::shared_ptr<BasicMessage> &msg) {
    auto fileInfo = msg->getFileInfo().value();
    Logger::get_instance()->info("Received file info: {}", fileInfo.fileName);
    auto tempDir = std::filesystem::temp_directory_path();
    auto peerChatDir = tempDir / "PeerChat";
    if (!std::filesystem::exists(peerChatDir)) {
        std::filesystem::create_directory(peerChatDir);
    }
    auto saveDir = peerChatDir / (fileInfo.fileName + "_tmp");
    if (!std::filesystem::exists(saveDir)) {
        std::filesystem::create_directory(saveDir);
    }
    auto sliceFilePath = saveDir / (std::to_string(fileInfo.sliceIndex));

    std::ofstream sliceFile(sliceFilePath,
                            std::ios::out | std::ios::trunc | std::ios::binary);
    if (!sliceFile.is_open()) {
        Logger::get_instance()->error("Failed to open file: {}",
                                      sliceFilePath.string());
        return;
    }
    sliceFile.write(fileInfo.fileData.c_str(), fileInfo.fileData.size());
    sliceFile.close();
    recvedSize += fileInfo.sliceSize;

    if (recvedSize == fileInfo.totalSize) {
        recvedSize = 0;

        // 收集所有切片文件
        std::vector<std::filesystem::path> sliceFiles;
        for (const auto &entry : std::filesystem::directory_iterator(saveDir)) {
            if (entry.is_regular_file()) {
                sliceFiles.push_back(entry.path());
            }
        }

        // 根据序号排序切片文件
        std::sort(
            sliceFiles.begin(),
            sliceFiles.end(),
            [](const std::filesystem::path &a, const std::filesystem::path &b) {
                int indexA = std::stoi(a.filename().string());
                int indexB = std::stoi(b.filename().string());
                return indexA < indexB;
            });

        // 拼接所有切片文件
        auto outputFilePath = peerChatDir / fileInfo.fileName;
        std::ofstream outputFile(
            outputFilePath, std::ios::binary | std::ios::out | std::ios::trunc);
        if (!outputFile.is_open()) {
            Logger::get_instance()->error("Failed to open output file: {}",
                                          outputFilePath.string());
            return;
        }

        for (const auto &sliceFilePath : sliceFiles) {
            std::ifstream sliceFile(sliceFilePath,
                                    std::ios::binary | std::ios::in);
            if (!sliceFile.is_open()) {
                Logger::get_instance()->error("Failed to open slice file: {}",
                                              sliceFilePath.string());
                continue;
            }
            outputFile << sliceFile.rdbuf();
            sliceFile.close();
            std::filesystem::remove(sliceFilePath);  // 拼接后删除切片文件
        }

        outputFile.close();
        std::filesystem::remove(saveDir);  // 删除临时保存的目录
    }
    auto outputFilePath = peerChatDir / fileInfo.fileName;

    QVariantMap fileInfoMap;
    fileInfoMap["receiver"] = QString::fromStdString(fileInfo.receiver);
    fileInfoMap["sender"] = QString::fromStdString(fileInfo.sender);
    fileInfoMap["fileName"] = QString::fromStdString(fileInfo.fileName);
    fileInfoMap["totalSize"] = fileInfo.totalSize;
    fileInfoMap["sliceSize"] = fileInfo.sliceSize;
    fileInfoMap["isDir"] = fileInfo.isDir;
    fileInfoMap["localPath"] = QString::fromStdString(outputFilePath.string());
    emit fileInfoReceived(fileInfoMap);
}

void Client::sendUserInfo(UserInfo &userInfo) {
    protocol.sendUserInfo(userInfo, "", -1);
}

void Client::sendUserInfo(const QVariantMap &userInfo) {
    UserInfo newUserInfo;
    newUserInfo.nickName = userInfo.value("nickName").toString().toStdString();
    newUserInfo.avatarFormat =
        userInfo.value("avatarFormat").toString().toStdString();

    auto avatarSource = userInfo.value("avatar").toString();
    QByteArray fileData;

    if (avatarSource.startsWith("qrc:/")) {
        // 处理 qrc 资源, 去除qrc
        QFile file(avatarSource.mid(3));
        if (file.open(QIODevice::ReadOnly)) {
            fileData = file.readAll();
            file.close();
        } else {
            qDebug() << "Failed to open qrc avatar file:" << avatarSource;
            return;
        }
    } else {
        // 处理本地文件路径
        QUrl url(avatarSource);
        if (url.isLocalFile()) {
            avatarSource = url.toLocalFile();
        }

        QFile file(avatarSource);
        if (file.open(QIODevice::ReadOnly)) {
            fileData = file.readAll();
            file.close();
        } else {
            qDebug() << "Failed to open local avatar file:" << avatarSource;
            return;
        }
    }

    if (!fileData.isEmpty()) {
        newUserInfo.avatar = std::string(fileData.constData(), fileData.size());
        sendUserInfo(newUserInfo);
    } else {
        qDebug() << "Avatar file is empty:" << avatarSource;
    }
}

void Client::sendChatInfo(ChatInfo &chatInfo) {
    protocol.sendChatInfo(chatInfo, "", -1);
}

void Client::sendChatInfo(const QVariantMap &userInfo) {
    ChatInfo chatInfo;
    chatInfo.sender = userInfo.value("sender").toString().toStdString();
    chatInfo.receiver = userInfo.value("receiver").toString().toStdString();
    chatInfo.content = userInfo.value("content").toString().toStdString();
    chatInfo.timestamp = userInfo.value("timestamp").toLongLong();
    sendChatInfo(chatInfo);
}

void Client::sendGroupInfo(GroupInfo &groupInfo) {
    protocol.sendGroupInfo(groupInfo, "", -1);
}

void Client::sendGroupInfo(const QVariantMap &groupInfo) {
    GroupInfo newGroupInfo;
    newGroupInfo.groupName =
        groupInfo.value("groupName").toString().toStdString();
    auto members = groupInfo.value("members").toList();
    for (auto member : members) {
        newGroupInfo.members.push_back(member.toString().toStdString());
    }
    sendGroupInfo(newGroupInfo);
}

void Client::sendLogoutInfo(LogoutInfo &logoutInfo) {
    protocol.sendLogoutInfo(logoutInfo, "", -1);
}

void Client::sendLogoutInfo(const QVariantMap &logoutInfo) {
    LogoutInfo newLogoutInfo;
    newLogoutInfo.nickName =
        logoutInfo.value("nickName").toString().toStdString();
    protocol.sendLogoutInfo(newLogoutInfo, "", -1);
}

void Client::sendFileInfo(const QVariantMap &fileInfo) {
    std::string filePath = fileInfo.value("filePath").toString().toStdString();
    std::string receiver = fileInfo.value("receiver").toString().toStdString();
    std::string sender = fileInfo.value("sender").toString().toStdString();
    std::string fileName = fileInfo.value("fileName").toString().toStdString();
    bool isDir = fileInfo.value("isDir").toBool();

    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        Logger::get_instance()->error("Failed to open file: {}", filePath);
        // Consider notifying the user or handling this error
        return;
    }

    file.seekg(0, std::ios::end);
    std::streamsize totalSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(SLICE_SIZE);

    for (int sliceIndex = 0; !file.eof(); ++sliceIndex) {
        file.read(buffer.data(), SLICE_SIZE);
        std::streamsize sliceSize = file.gcount();

        if (file.fail() && !file.eof()) {
            Logger::get_instance()->error("Error reading file: {}", filePath);
            break;
        }

        FileInfo fileInfo{receiver,
                          sender,
                          fileName,
                          std::string(buffer.data(), sliceSize),
                          static_cast<int>(totalSize),
                          sliceIndex,
                          static_cast<int>(sliceSize),
                          isDir};

        protocol.sendFileInfo(fileInfo, "", -1);
    }

    file.close();
}

void Client::sendFileInfo(FileInfo &fileInfo) {
    protocol.sendFileInfo(fileInfo, "", -1);
}

// int main(int argc, char *argv[]) {
//     Client client;
//     UserInfo userInfo;
//     // userInfo.nickName = "test";
//     std::cin >> userInfo.nickName;
//     client.sendUserInfo(userInfo);
//     printf("Listening the port: %d\n", client.getLocalPort());
//     while (true) {
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//     }
//     return 0;
// }
#ifndef CLIENT_H
#define CLIENT_H

#include <QByteArray>
#include <QImage>
#include <QMap>
#include <QMetaType>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QVariantMap>


#include "BasicMessage.h"
#include "Protocol.h"

Q_DECLARE_METATYPE(UserInfo)
Q_DECLARE_METATYPE(ChatInfo)
Q_DECLARE_METATYPE(GroupInfo)

class Client : public QObject {
    Q_OBJECT
  public:
    Client(QObject *parent = nullptr);
    Client(const Client &) = delete;
    Client &operator=(const Client &) = delete;
    ~Client();

    void handleUserInfo(const std::shared_ptr<BasicMessage> &msg);
    void handleChatInfo(const std::shared_ptr<BasicMessage> &msg);
    void handleGroupInfo(const std::shared_ptr<BasicMessage> &msg);
    void handleLogoutInfo(const std::shared_ptr<BasicMessage> &msg);

    int getLocalPort() { return protocol.getLocolPort(); }

    void sendUserInfo(UserInfo &userInfo);
    void sendChatInfo(ChatInfo &chatInfo);
    void sendGroupInfo(GroupInfo &groupInfo);
    void sendLogoutInfo(LogoutInfo &logoutInfo);
  public slots:
    // 从QML发送消息
    Q_INVOKABLE void sendUserInfo(const QVariantMap &userInfo);
    Q_INVOKABLE void sendChatInfo(const QVariantMap &chatInfo);
    Q_INVOKABLE void sendGroupInfo(const QVariantMap &groupInfo);
    Q_INVOKABLE void sendLogoutInfo(const QVariantMap &logoutInfo);

  private:
    Protocol protocol;
    std::unordered_map<std::string, UserInfo> userInfos;
    QImage createGroupAvatar(const GroupInfo &groupInfo, int size);

  signals:
    void userInfoReceived(const QVariantMap &userInfo);
    void chatInfoReceived(const QVariantMap &userInfo);
    void groupInfoReceived(const GroupInfo &groupInfo);
    void logoutInfoReceived(const QVariantMap &logoutInfo);
};

#endif  // CLIENT_H

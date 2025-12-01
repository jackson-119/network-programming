import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "./components" as Components
import FluentUI

FluWindow {
    id: applicationWindow
    visible: true
    width: 800
    height: 600
    title: "聊天应用"

    // 创建一个 JavaScript 对象来存储所有聊天记录
    property var allChatMessages: ({})
    property var allUsers: ({})
    property var files: ({})
    property string myNickname: "用户x"
    property string peerNickname: ""

    SplitView {
        handle: Rectangle {
            width: 0
            color: "#F0F0F0"
        }
        anchors.fill: parent
        spacing: 0
        Components.AvatarBar {
            SplitView.preferredWidth: 80
            SplitView.minimumWidth: 80
            SplitView.fillHeight: true
            onNicknameChanged: {
                myNickname = nickname;
            }
        }
        // Rectangle
        SplitView.preferredWidth: 250
        SplitView.minimumWidth: 200
        SplitView.fillHeight: true

        ColumnLayout {
            spacing: 0
            SplitView.preferredWidth: 250
            SplitView.minimumWidth: 200
            SplitView.fillHeight: true
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Components.ChatList {
                    id: chatList
                    anchors.fill: parent
                    onChatSelected: function (nickname) {
                        messageList.updateMessages(allChatMessages[nickname]);
                    }
                }
            }
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 50
                color: "#FFFFFF"
                RoundButton {
                    id: createGroupButton
                    width: 30
                    height: 30
                    anchors.centerIn: parent

                    background: Rectangle {
                        color: "#007AFF"
                        radius: width / 2  // 保持圆形
                    }

                    Image {
                        anchors.centerIn: parent
                        width: parent.width * 0.6  // 将图标大小设置为按钮大小的60%
                        height: width  // 保持图标的宽高比
                        sourceSize.width: width
                        sourceSize.height: height
                        source: 'data:image/svg+xml;utf8,<svg t="1733051503917" class="icon" viewBox="0 0 1024 1024" version="1.1" xmlns="http://www.w3.org/2000/svg" p-id="4253" width="200" height="200"><path d="M925.696 384q19.456 0 37.376 7.68t30.72 20.48 20.48 30.72 7.68 37.376q0 20.48-7.68 37.888t-20.48 30.208-30.72 20.48-37.376 7.68l-287.744 0 0 287.744q0 20.48-7.68 37.888t-20.48 30.208-30.72 20.48-37.376 7.68q-20.48 0-37.888-7.68t-30.208-20.48-20.48-30.208-7.68-37.888l0-287.744-287.744 0q-20.48 0-37.888-7.68t-30.208-20.48-20.48-30.208-7.68-37.888q0-19.456 7.68-37.376t20.48-30.72 30.208-20.48 37.888-7.68l287.744 0 0-287.744q0-19.456 7.68-37.376t20.48-30.72 30.208-20.48 37.888-7.68q39.936 0 68.096 28.16t28.16 68.096l0 287.744 287.744 0z" p-id="4254" fill="#FFFFFF"></path></svg>'
                        smooth: true
                        antialiasing: true
                    }

                    onClicked: {
                        var component = Qt.createComponent("components/CreateGroupWindow.qml");
                        if (component.status === Component.Ready) {
                            var window = component.createObject();
                            window.show();
                        } else if (component.status === Component.Error) {
                            console.error("Error creating component:", component.errorString());
                        }
                    }

                    ToolTip.visible: hovered
                    ToolTip.text: "创建群组"
                }
            }
        }

        Rectangle {
            color: "#F0F0F0"
            gradient: Gradient {
                GradientStop {
                    position: 0.0
                    color: "#FAF4FF"
                }  // 起始颜色
                GradientStop {
                    position: 1.0
                    color: "#D9EBFF"
                }  // 结束颜色
            }
            SplitView.fillWidth: true
            ColumnLayout {
                anchors.fill: parent
                Components.MessageList {
                    id: messageList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
                Rectangle {
                    color: "#D5E2F5"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                }
                Components.FileTrans {
                    id: fileTransferBar
                    Layout.fillWidth: true
                    Layout.bottomMargin: 10
                    // Layout.preferredHeight:
                }
                Components.InputArea {
                    id: inputArea
                    Layout.margins: 10
                    Layout.fillWidth: true
                    Layout.preferredHeight: 50
                }
            }
        }
    }
    Component.onCompleted: {
        console.log("hey", allUsers);
        client.chatInfoReceived.connect(function (chatInfo) {
            var sender = chatInfo.sender;
            var receiver = chatInfo.receiver;
            var chatKey = receiver === myNickname ? sender : receiver;
            if (!allChatMessages.hasOwnProperty(chatKey)) {
                allChatMessages[chatKey] = [];
            }
            allChatMessages[chatKey].push({
                avatar: allUsers[sender].avatar,
                message: chatInfo.content,
                sentTime: new Date(chatInfo.timestamp),
                isOwnMessage: false,
                userName: sender
            });
            messageList.updateMessages(allChatMessages[peerNickname]);
        });
        client.userInfoReceived.connect(function (userInfo) {
            // 更新全局用户信息字典
            if (allUsers[userInfo.nickName] === undefined) {
                showInfo(qsTr("用户") + userInfo.nickName + qsTr("加入了聊天"));
            }
            allUsers[userInfo.nickName] = userInfo;
            if (allChatMessages[userInfo.nickName] === undefined) {
                allChatMessages[userInfo.nickName] = [];
            }
            chatList.updateUserList();  // 通知 ChatList 更新列表
        });
        client.logoutInfoReceived.connect(function (logoutInfo) {
            // 更新全局用户信息字典
            showWarning(qsTr("用户") + logoutInfo.nickName + qsTr("离开了聊天"));
            delete allUsers[logoutInfo.nickName];
            chatList.updateUserList();  // 通知 ChatList 更新列表
        });
        client.fileInfoReceived.connect(function (fileinfo) {
            var chatKey = fileinfo.receiver === myNickname ? fileinfo.sender : fileinfo.receiver;
            var fileName = fileinfo.fileName;
            var localPath = fileinfo.localPath;
            var receivedTime = Date.now();
            var totalSize = fileinfo.totalSize;
            var sliceSize = fileinfo.sliceSize;
            var isDir = fileinfo.isDir;

            if (!files[chatKey]) {
                files[chatKey] = {};
            }

            if (!files[chatKey][fileName] || files[chatKey][fileName].fileName !== fileName) {
                files[chatKey][fileName] = {
                    fileName: fileName,
                    localPath: localPath,
                    startTime: receivedTime,
                    totalSize: totalSize,
                    recvedSize: sliceSize,
                    progress: 100 * sliceSize / totalSize,
                    speed: 0,
                    isDir: isDir,
                    isOwnFile: false
                };
            } else {
                var fileInfo = files[chatKey][fileName];
                fileInfo.recvedSize += sliceSize;
                var duration = (Date.now() - fileInfo.startTime) / 1000; // 转换为秒
                fileInfo.speed = duration > 0 ? (fileInfo.recvedSize / duration) : 0; // bytes/second
                fileInfo.progress = 100 * fileInfo.recvedSize / fileInfo.totalSize;
            }
            // console.log("fileInfoReceived", files[chatKey][fileName]);
            fileTransferBar.updateFileInfo(files[chatKey][fileName]);
        });
    }
    Component.onDestruction: {
        console.log("onDestruction");
        client.sendLogoutInfo({
            nickName: myNickname
        });
    }
}

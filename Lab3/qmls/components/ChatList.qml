// ChatList.qml
import QtQuick 
import QtQuick.Controls 

ListView {
    id: chatListView
    clip: true
    interactive: true
    highlightFollowsCurrentItem: true
    model: ListModel {}

    delegate: ChatListItem {
        width: chatListView.width
        nickname: model.nickName
        avatar: model.avatar
        ip: model.ip
        port: model.port

        MouseArea {
            anchors.fill: parent
            onClicked: {
                chatListView.currentIndex = index;
            }
        }
    }
    highlight: Rectangle {
        color: "#CCEBFF"
    }
    focus: true

    signal chatSelected(string nickname)

    onCurrentIndexChanged: {
        peerNickname = model.get(currentIndex).nickName;
        if (currentIndex >= 0) {
            chatSelected(model.get(currentIndex).nickName);
        }
    }

    function updateUserList() {
        model.clear();
        if (Object.keys(allUsers).length === 0) {
            chatListModel.clear();
            return;
        }
        for (var nickname in allUsers) {
            if (allUsers.hasOwnProperty(nickname)) {
                if (allUsers[nickname].ip) {
                    model.append(allUsers[nickname]);
                }
            }
        }
    }

    Component.onCompleted:
    // updateUserList();  // 初始化时更新用户列表
    {}
}

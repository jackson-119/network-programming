// SelectableUserList.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import FluentUI 1.0

ListView {
    id: selectableUserListView
    clip: true
    interactive: true
    model: ListModel {}

    delegate: ChatListItem {
        width: selectableUserListView.width
        nickname: model.nickName || ""
        avatar: model.avatar || ""
        ip: model.ip || ""
        port: model.port || ""

        FluCheckBox {
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            onCheckedChanged: {
                console.log(nickname + " 选中状态: " + checked);
                // 这里可以添加选中逻辑
            }
        }
    }

    function updateUserList() {
        model.clear();
        if (typeof allUsers === "undefined" || Object.keys(allUsers).length === 0) {
            return;
        }
        for (var nickname in allUsers) {
            if (allUsers.hasOwnProperty(nickname) && allUsers[nickname].ip) {
                model.append(allUsers[nickname]);
            }
        }
    }
}

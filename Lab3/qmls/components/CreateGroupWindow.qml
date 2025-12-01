// CreateGroupWindow.qml
import QtQuick 
import QtQuick.Controls 
import QtQuick.Layouts 
import QtQuick.Window 
import FluentUI 

FluWindow {
    id: createGroupWindow
    width: 400
    height: 600
    title: qsTr("创建群组")
    modality: Qt.ApplicationModal

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10

        FluTextBox {
            id: groupNameInput
            Layout.fillWidth: true
            placeholderText: qsTr("输入群组名")
        }

        SelectableUserList {
            id: userList
            Layout.fillWidth: true
            Layout.fillHeight: true

            delegate: ChatListItem {
                width: userList.width
                nickname: model.nickName || ""
                avatar: model.avatar || ""
                ip: model.ip || ""
                port: model.port || ""

                CheckBox {
                    id: userCheckBox
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    onCheckedChanged: {
                        if (checked) {
                            selectedUsers.push(nickname);
                        } else {
                            var index = selectedUsers.indexOf(nickname);
                            if (index > -1) {
                                selectedUsers.splice(index, 1);
                            }
                        }
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true

            FluFilledButton {
                text: qsTr("创建")
                onClicked: {
                    if (groupNameInput.text.trim() === "") {
                        console.error("群组名称不能为空");
                        return;
                    }
                    if (selectedUsers.length < 2) {
                        console.error("群组至少需要两个成员");
                        return;
                    }
                    // 添加自己到群组成员中
                    selectedUsers.push(myNickname);
                    var groupInfo = {
                        "groupName": groupNameInput.text.trim(),
                        "members": selectedUsers
                    };
                    client.sendGroupInfo(groupInfo);
                    createGroupWindow.close();
                }
            }
            FluButton {
                text: qsTr("取消")
                onClicked: createGroupWindow.close()
            }
        }
    }

    property var selectedUsers: []

    Component.onCompleted: {
        if (userList && typeof userList.updateUserList === "function") {
            userList.updateUserList();
        } else {
            console.error("userList or updateUserList function is not available");
        }
    }
}

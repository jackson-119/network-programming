import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import QtQuick.Dialogs
import FluentUI

Rectangle {
    id: root
    color: "#00ffaeff"
    width: 80

    signal avatarClicked
    signal nicknameChanged(string nickname)

    property string avatarSource: "qrc:/avatars/1.png"

    Column {
        anchors.fill: parent
        spacing: 10
        topPadding: 10

        RoundButton {
            anchors.horizontalCenter: parent.horizontalCenter
            width: 50
            height: 50

            background: Image {
                source: root.avatarSource
                fillMode: Image.PreserveAspectCrop
                layer.enabled: true
                layer.effect: OpacityMask {
                    maskSource: Rectangle {
                        width: 40
                        height: 40
                        radius: 20
                    }
                }
            }

            onClicked: fileDialog.open()
        }

        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            width: Math.min(parent.width * 0.8, Math.max(80, nickNameTextField.implicitWidth + 20))
            height: 26
            color: "#E6FFFFFF"  // 半透明白色
            radius: 13

            TextField {
                id: nickNameTextField
                anchors.centerIn: parent
                text: myNickname
                font.pixelSize: 12
                color: "#333333"
                background: Rectangle {
                    color: "transparent"
                }
                width: parent.width - 10
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                readOnly: false
                padding: 0

                onEditingFinished: {
                    if (text !== myNickname) {
                        myNickname = text;
                        // root.updateUserInfo();
                        readOnly = true;
                        focus = false;
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (!parent.readOnly) {
                            parent.forceActiveFocus();
                        }
                    }
                }
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: "Select an Image"
        nameFilters: ["Image files (*.png *.jpg *.jpeg)"]
        onAccepted: {
            root.avatarSource = fileDialog.selectedFile.toString();
            updateUserInfo();
        }
    }

    function updateUserInfo() {
        var nickName = nickNameTextField.text;
        var avatarSource = root.avatarSource;
        var avatarFormat = avatarSource.split('.').pop();
        allUsers[myNickname] = {
            nickName: nickName,
            avatar: avatarSource
        };

        client.sendUserInfo({
            nickName: nickName,
            avatar: avatarSource,
            avatarFormat: avatarFormat
        });
    }

    Component.onCompleted: {
        nickNameTextField.text = myNickname;  // 确保初始化时使用全局属性
    }
}
import QtQuick 
import QtQuick.Layouts 
import "." as Components

Item {
    id: root
    width: ListView.view.width
    height: Math.max(avatarColumn.height, messageBubble.height) + 20

    Column {
        id: avatarColumn
        spacing: 8
        anchors {
            left: isOwnMessage ? undefined : parent.left
            right: isOwnMessage ? parent.right : undefined
            top: parent.top
            margins: 12
        }

        Components.Avatar {
            id: avatarImage
            source: avatar
            size: 44
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
            id: nameText
            text: userName
            font.family: "Microsoft YaHei"
            font.pixelSize: 11
            font.weight: Font.Medium
            color: "#444444"
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    Rectangle {
        id: messageBubble
        width: Math.min(messageText.implicitWidth + 40, root.width * 0.7)
        height: messageText.implicitHeight + timeText.height + 34
        color: isOwnMessage ? "#0099FF" : "#FFFFFF"
        radius: 12
        anchors {
            left: isOwnMessage ? undefined : avatarColumn.right
            right: isOwnMessage ? avatarColumn.left : undefined
            top: parent.top
            margins: 12
        }
        border.color: isOwnMessage ? "transparent" : "#E0E0E0"
        border.width: 1

        Text {
            id: messageText
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: 12
            }
            color: isOwnMessage ? "#FFFFFF" : "#000000"
            text: message
            wrapMode: Text.Wrap
            font.family: "Microsoft YaHei"
            font.pixelSize: 15
            lineHeight: 1.2
        }

        Rectangle {
            anchors {
                right: isOwnMessage ? undefined : parent.right
                left: isOwnMessage ? parent.left : undefined
                bottom: parent.bottom
                leftMargin: isOwnMessage ? 12 : 0
                rightMargin: isOwnMessage ? 0 : 12
                bottomMargin: 8
            }
            color: isOwnMessage ? "#0077CC" : "#F0F0F0"
            radius: 10
            width: timeText.width + 12
            height: timeText.height + 6

            Text {
                id: timeText
                anchors.centerIn: parent
                text: {
                    var date = new Date(sentTime);
                    return Qt.formatTime(date, "hh:mm:ss");
                }
                font.family: "Microsoft YaHei"
                font.pixelSize: 11
                color: isOwnMessage ? "#FFFFFF" : "#666666"
            }
        }
    }
}

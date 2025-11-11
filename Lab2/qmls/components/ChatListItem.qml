import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Item {
    id: root
    width: ListView.view.width
    height: 80

    property string nickname: ""
    property string avatar: ""
    property string ip: ""
    property int port: 0

    Rectangle {
        anchors.fill: parent
        color: ListView.isCurrentItem ? "#f0f0f0" : "transparent"
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 15

        Avatar {
            source: root.avatar
            size: 50
        }

        Column {
            Layout.fillWidth: true
            spacing: 10

            Text {
                text: root.nickname
                font.family: "Microsoft YaHei"
                font.bold: false
                font.pixelSize: 16
                color: "#333333"
            }

            Rectangle {
                width: ipPortText.width + 20
                height: ipPortText.height + 10
                radius: 12
                color: "#E6F5FF"

                Text {
                    id: ipPortText
                    anchors.centerIn: parent
                    text: root.port == -1 ? root.ip : root.ip + ":" + root.port
                    font.family: "Microsoft YaHei"
                    font.pixelSize: 12
                    color: "#666666"
                }
            }
        }

        Item {
            width: 20
            height: 20

            Rectangle {
                width: 8
                height: 8
                radius: 4
                color: "#4CAF50"
                anchors.centerIn: parent
            }
        }
    }

    Rectangle {
        anchors.bottom: parent.bottom
        width: parent.width
        height: 1
        color: "#e0e0e0"
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            ListView.view.currentIndex = index;
        }
    }
}

import QtQuick 
import Qt5Compat.GraphicalEffects
import FluentUI 

Item {
    id: root
    property string source
    property int size: 40

    width: size
    height: size

    FluImage {
        id: image
        anchors.fill: parent
        source: root.source
        fillMode: Image.PreserveAspectCrop
        visible: false
        cache: false
        onStatusChanged: {
            if (status === Image.Error) {
                console.error("Error loading image:", source);
            }
        }
    }

    Rectangle {
        id: mask
        anchors.fill: parent
        radius: width / 2
        visible: false
    }

    OpacityMask {
        anchors.fill: image
        source: image
        maskSource: mask
    }

    Rectangle {
        anchors.fill: parent
        color: "lightgray"
        radius: width / 2
        visible: image.status !== Image.Ready

        Text {
            anchors.centerIn: parent
            text: image.status === Image.Loading ? "..." : "群"
            font.pixelSize: root.size / 3
            color: "gray"
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border.width: 1
        border.color: "#e0e0e0"
        radius: width / 2
    }
}